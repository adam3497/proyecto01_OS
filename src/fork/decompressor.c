#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>

#include "../utilities/file_utils.c"
#include "../huffman/huffman.c"
#include "file_locks.c"

#define MAX_CONCURRENT_PROCESSES 4 // Set the maximum number of concurrent processes

int main() {
    pid_t pid;
    int numOfProcess = TOTAL_BOOKS;

    // Folder Paths
    const char* booksFolder = "books";
    const char* out = "out/bin/books_compressed_fork.bin";
    
    FILE *binary_source = fopen(out, "rb");

    // Set for every book in books folder
    struct EncodeArgs *paths = getAllPaths(booksFolder);
    
    // Decode 
    int miliseconds = 200;
    int activeProcesses = 0; // Track the number of active processes
    for (int i = 0; i < numOfProcess; i++) {
        // Wait until there are available process slots
        while (activeProcesses >= MAX_CONCURRENT_PROCESSES) {
            wait(NULL); // Wait for any child process to finish
            activeProcesses--;
        }

        pid = fork();
        usleep(miliseconds *1000); // Convierte milisegundos a microsegundos y espera
        
        // Código específico del proceso hijo
        if (pid == 0) {
            printf("\n");
            printf("[PID %d] DECODING : %s\n", i+1, paths->books[i]);
            decompress_and_write_to_file(binary_source, paths->decodes[i], i+1);
            printf("\n");
            return 0;
        } else if (pid > 0) {
            activeProcesses++; // Increment active process count in the parent
        } else {
            // Error handling for fork failure
            fprintf(stderr, "Fork failed.\n");
        }
    }
    
    // El padre espera a todos los hijos
    while (activeProcesses > 0) {
        wait(NULL);
        activeProcesses--;
    }

    /* // El padre espera a todos los hijos
    while (numOfProcess > 0) {
        wait(NULL);
        numOfProcess--;
    } */

    // Liberar la memoria asignada
    fclose(binary_source);
    free(paths);
    
    return EXIT_SUCCESS;
}
