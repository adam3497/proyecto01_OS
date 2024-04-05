#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>


#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#include "../utilities/file_utils.c"
#include "../huffman/huffman.c"
#include "file_locks.c"

#define SEM_1 "/sem_1"

int main() {
    pid_t pid;
    
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
        pid = fork();
        
        // Código específico del proceso hijo
        if (pid == 0) {
            
            // Abrir una nueva instancia del archivo fuente para cada proceso hijo
            FILE *local_binary_source = fopen(out, "rb");
            if (local_binary_source == NULL) {
                perror("Error opening binary source file in child process");
                exit(EXIT_FAILURE);
            }

            // Mover a la posición correcta del archivo para descomprimir
            size_t currentOffset = dirMetadata.offsets[i];
            fseek(local_binary_source, currentOffset, SEEK_SET);
            decompress_and_write_to_file(local_binary_source, dir_path, i+1);
            

            fclose(local_binary_source); // Asegurarse de cerrar el archivo local
            exit(EXIT_SUCCESS);

        } else if (pid < 0) {
            // Error handling for fork failure
            fprintf(stderr, "Fork failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    
   // El padre espera a todos los hijos
    for (int i = 0; i < numOfProcess; i++) {
        wait(NULL);
    }

    // Liberar la memoria asignada
    fclose(binary_source);
    free(paths);
    
    return EXIT_SUCCESS;
}
