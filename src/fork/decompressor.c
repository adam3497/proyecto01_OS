#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <unistd.h>
#include <sys/wait.h>

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
    for (int i = 0; i < numOfProcess; i++) {
        pid = fork();
        
        // Código específico del proceso hijo
        if (pid == 0) {
            set_lock(binary_source, F_RDLCK);

            printf("[PID %d] DECODING : %s\n", i+1, paths->books[i]);
            fork_decompress(binary_source, paths->decodes[i]);
            printf("\n");

            unlock_file(binary_source);
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
