#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <semaphore.h>
#include <pthread.h>

#include "../utilities/file_utils.c"
#include "../huffman/huffman.c"

// Define a semaphore to limit the number of concurrent threads
sem_t sem;

// Define a mutex to synchronize access to the binary source file
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct ThreadArgs {
    FILE *binarySource;
    const char* outputPath;
    size_t offset;
    int pos;
};

void* decode_book(void* arg) {
    struct ThreadArgs* args = (struct ThreadArgs*)arg;

    // Lock the mutex before accessing the binary source file
    pthread_mutex_lock(&mutex);

    // Perform decompression and write to file
    fseek(args->binarySource, args->offset, SEEK_SET);
    decompress_and_write_to_file(args->binarySource, args->outputPath, args->pos);

    // Unlock the mutex after accessing the binary source file
    pthread_mutex_unlock(&mutex);

    // Release the semaphore to allow another thread to start
    // sem_post(&sem);

    return NULL;
}

int main() {
    // Folder Paths
    const char* input_file = "out/bin/compressed.bin";
    
    FILE *binary_source = fopen(input_file, "rb");
    if (binary_source == NULL) {
        perror("Error opening binary file for decompression");
        exit(EXIT_FAILURE);
    }

    // Read directory metadata, it also reads the offsets for decompression
    struct DirectoryMetadata dirMetadata;
    read_directory_metadata(&dirMetadata, binary_source);

    printf("- Directory name: %s\n", dirMetadata.directory);
    printf("- Amount of files to decompress: %d\n", dirMetadata.numTxtFiles);
    // Create folder to store decompressed files
    const char* dir_result = create_output_dir(dirMetadata.directory);
    const char* dir_path = concat_strings(dir_result, "/");

    // Initialize the semaphore with an initial value of 8
    // sem_init(&sem, 0, 8);

    // Create an array of pthreads
    pthread_t threads[dirMetadata.numTxtFiles];
    struct ThreadArgs thread_args[dirMetadata.numTxtFiles]; // Array to hold thread arguments

    // Decompress files using pthreads
    for (int i = 0; i < dirMetadata.numTxtFiles; ++i) {
        // Wait until a slot is available in the semaphore
        // sem_wait(&sem);
        
        //printf("Decompressing file [%d]: ", i+1);

       // Populate thread arguments
        thread_args[i].binarySource = binary_source;
        thread_args[i].outputPath = dir_path;
        thread_args[i].pos = i+1;
        thread_args[i].offset = dirMetadata.offsets[i];

        // Create a thread to decompress the file
        pthread_create(&threads[i], NULL, decode_book, (void*)&thread_args[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < dirMetadata.numTxtFiles; i++) {
        pthread_join(threads[i], NULL);
    }

    // Liberar la memoria asignada
    fclose(binary_source);
    pthread_mutex_destroy(&mutex);
    return EXIT_SUCCESS;
}
