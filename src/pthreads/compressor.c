#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <pthread.h>
#include <semaphore.h>

#include "../utilities/file_utils.c"
#include "../huffman/huffman.c"
#include "../huffman/freq.c"

// Define a semaphore to limit the number of concurrent threads
sem_t sem;

// Define a mutex to synchronize access to the output binary file
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct ThreadArgs {
    char* bookFile;
    FILE *outputFile;
};


void encode(char *input_file, FILE *binary_output){
    // Fill the buffer
    wchar_t *buffer = NULL;
    get_wchars_from_file(input_file, &buffer);

    // Extract the frequencies for each character in the text file
    int freq_table[CHAR_SET_SIZE] = {0};
    char_frequencies(buffer, freq_table);

    // We first calculate the size of the freq table (only the characters' freq > 0)
    int freq_table_size = calculateFreqTableSize(freq_table);

    // Build Huffman Tree from the frequency table
    struct MinHeapNode* huffmanRoot = buildHuffmanTree(freq_table, freq_table_size);

    // Huffman codes struct array, where each position represents a character and the struct contains its code and code length
    struct HuffmanCode* huffmanCodesArray[MAX_FREQ_TABLE_SIZE] = {NULL};
    
    // An array where the Huffman code for each character is gonna be stored
    int bits[MAX_CODE_SIZE];

    // Generate Huffman codes for each character in the text file
    generateHuffmanCodes(huffmanRoot, bits, 0, huffmanCodesArray);

    // Write the Huffman Codes to file    
    size_t buffer_size = wcslen(buffer);

    // Lock the mutex before accessing the output file
    pthread_mutex_lock(&mutex);

    printf("[ENCODING]: %s\n", input_file);
    write_encoded_bits_to_file(buffer, buffer_size, input_file, huffmanRoot, huffmanCodesArray, binary_output);
    printf("  File encoded successfully\n");

    // Unlock the mutex after accessing the output file
    pthread_mutex_unlock(&mutex);

    // Free dynamically allocated memory of buffer and huffman tree
    free(buffer);
    free_huffman_tree(huffmanRoot);
}

// Define the thread function for encoding a book
void* encode_book(void* arg) {
    struct ThreadArgs* args = (struct ThreadArgs*)arg;
    
    // Perform encoding
    encode(args->bookFile, args->outputFile);

    // Release the semaphore to allow another thread to start
    sem_post(&sem);

    return NULL;
}


int main() {
    // Initialize the semaphore with the maximum number of allowed threads
    sem_init(&sem, 0, 8);

    // Folder Paths
    const char* booksFolder = "books";
    const char* out = "out/bin/books_compressed_serial.bin";

    FILE *binary_output = fopen(out, "wb");
    if (binary_output == NULL) {
        perror("Error opening output binary file");
        exit(EXIT_FAILURE);
    }

    // Set for every book in books folder
    struct EncodeArgs *paths = getAllPaths(booksFolder);

    struct DirectoryMetadata dirMetadata = {
        .directory = booksFolder,
        .numTxtFiles = paths->fileCount
    };

    // Write content metadata to binary file
    write_directory_metadata(binary_output, &dirMetadata);

    // Create an array of pthreads
    pthread_t threads[paths->fileCount];

    // Encode books using pthreads
    for (int i = 0; i < paths->fileCount; i++) {
        // Wait until a slot is available in the semaphore
        sem_wait(&sem);

        // Create arguments for the thread
        struct ThreadArgs args = {
            .bookFile = paths->books[i],            
            .outputFile = binary_output
        };
        
        // Create a thread to encode the book
        pthread_create(&threads[i], NULL, encode_book, (void*)&args);
    }

    // Wait for all threads to finish
    for (int i = 0; i < paths->fileCount; i++) {
        pthread_join(threads[i], NULL);
    }

    fclose(binary_output);
    free(paths);
    return EXIT_SUCCESS;
}
