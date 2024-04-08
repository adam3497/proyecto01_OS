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
    int pos;
    size_t* offsetsPtr; // Pointer to the offsets array
};


void encode(char *input_file, FILE *binary_output, int pos, size_t* offsetsPtr){
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

    printf("[PID %d][CODING #%d] %s\n", getpid(), pos, input_file);

    // Lock the mutex before accessing the output file
    pthread_mutex_lock(&mutex);
    write_encoded_bits_to_file(buffer, buffer_size, input_file, huffmanRoot, huffmanCodesArray, binary_output, offsetsPtr, pos);
    // Unlock the mutex after accessing the output file
    pthread_mutex_unlock(&mutex);

    // Free dynamically allocated memory of buffer and huffman tree
    free(buffer);
    free_huffman_tree(huffmanRoot);
}

// Define the thread function for encoding a book
void* encode_book(void* arg) {
    struct ThreadArgs* args = (struct ThreadArgs*)arg;
    
    // Perform encoding and register the offset for that file
    encode(args->bookFile, args->outputFile, args->pos, args->offsetsPtr);

    // Release the semaphore to allow another thread to start
    sem_post(&sem);

    return NULL;
}


int main() {
    // Initialize the semaphore with the maximum number of allowed threads
    sem_init(&sem, 0, 8);

    // Folder Paths
    const char* booksFolder = "books";
    const char* out = "out/bin/compressed.bin";

    FILE *binary_output = fopen(out, "wb");
    if (binary_output == NULL) {
        perror("Error opening output binary file");
        exit(EXIT_FAILURE);
    }

    // Set for every book in books folder
    struct EncodeArgs *paths = getAllPaths(booksFolder);

    struct DirectoryMetadata dirMetadata = {
        .directory = booksFolder,
        .numTxtFiles = paths->fileCount,
        .offsets = {0}
    };

    // Write content metadata to binary file and get the position for the offsets array
    long offsets_pos = write_directory_metadata(binary_output, &dirMetadata);

    // Create an array of pthreads
    pthread_t threads[paths->fileCount];
    struct ThreadArgs thread_args[paths->fileCount]; // Array to hold thread arguments

    // Offsets array to be populated
    size_t offsets[MAX_TOTAL_BOOKS] = {0};

    // Encode books using pthreads
    for (int i = 0; i < paths->fileCount; ++i) {
        // Wait until a slot is available in the semaphore
        sem_wait(&sem);

        // Populate thread arguments
        thread_args[i].bookFile = paths->books[i];
        thread_args[i].outputFile = binary_output;
        thread_args[i].pos = i+1;
        thread_args[i].offsetsPtr = offsets; // Pass the offsets array pointer
        
        // Create a thread to encode the book
        pthread_create(&threads[i], NULL, encode_book, (void*)&thread_args[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < paths->fileCount; i++) {
        pthread_join(threads[i], NULL);
    }

    // Update the offsets array in the binary file
    fseek(binary_output, offsets_pos, SEEK_SET);
    fwrite(offsets, sizeof(size_t), paths->fileCount, binary_output);

    fclose(binary_output);
    free(paths);
    pthread_mutex_destroy(&mutex);
    return EXIT_SUCCESS;
}
