#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "../utilities/file_utils.c"
#include "../huffman/huffman.c"
#include "file_locks.c"

#define MAX_CONCURRENT_PROCESSES 1 // Set the maximum number of concurrent processes

int main() {
    int numOfProcess = TOTAL_BOOKS;

    // Folder Paths
    const char* booksFolder = "books";
    const char* out = "out/bin/compressed.bin";
    
    FILE *binary_source = fopen(out, "rb");

    // Set for every book in books folder
    struct EncodeArgs *paths = getAllPaths(booksFolder);
    
    // Read directory metadata, it also reads the offsets for decompression
    struct DirectoryMetadata dirMetadata;
    read_directory_metadata(&dirMetadata, binary_source);

    // Create folder to store decompressed files
    const char* dir_result = create_output_dir(dirMetadata.directory);
    const char* dir_path = concat_strings(dir_result, "/");

    // Track the number of active processes
    int activeProcesses = 0; 
    int miliseconds = 150;

    // Decode 
    for (int i = 0; i < numOfProcess; i++) {
        
        // Wait until there are available process slots
        while (activeProcesses >= MAX_CONCURRENT_PROCESSES) {
            wait(NULL); 
            activeProcesses--;
        }

        pid_t pid = fork();
        
        // Código específico del proceso hijo
        if (pid == 0) {
            // Move to file to decompress
            size_t currentOffset = dirMetadata.offsets[i];
            fseek(binary_source, currentOffset, SEEK_SET);
            decompress_and_write_to_file(binary_source, dir_path, i+1);
            printf("[PID %d][FINISHED #%d]\n", getpid(), i+1);

            return 0;
        } else if (pid > 0) {
            activeProcesses++;
        } else {
            // Error handling for fork failure
            fprintf(stderr, "Fork failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    
    // El padre espera a todos los hijos
    while (activeProcesses > 0) {
        wait(NULL);
        activeProcesses--;
    }

    // Liberar la memoria asignada
    fclose(binary_source);
    free(paths);
    
    return EXIT_SUCCESS;
}
