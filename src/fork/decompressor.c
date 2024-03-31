#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>

#include "../utilities/file_utils.c"
#include "file_locks.c"


int main() {
    pid_t pid;
    int numOfProcess = TOTAL_BOOKS;

    // Folder Paths
    const char* booksFolder = "books";
    const char* out = "out/bin/compressed.bin";
    
    FILE *binary_source = fopen(out, "rb");

    // Set for every book in books folder
    struct EncodeArgs *paths = getAllPaths(booksFolder);
    
    // Decode 
    int miliseconds = 200;
    for (int i = 0; i < numOfProcess; i++) {
        pid = fork();
        usleep(miliseconds *1000); // Convierte milisegundos a microsegundos y espera
        
        // Código específico del proceso hijo
        if (pid == 0) {
            printf("\n");
            printf("[PID %d] DECODING : %s\n", i+1, paths->books[i]);
            decompress_and_write_to_file(binary_source, paths->decodes[i]);
            printf("\n");
            return 0;
        }
    }
    
    // El padre espera a todos los hijos
    while (numOfProcess > 0) {
        wait(NULL);
        numOfProcess--;
    }

    // Liberar la memoria asignada
    fclose(binary_source);
    free(paths);
    
    return EXIT_SUCCESS;
}
