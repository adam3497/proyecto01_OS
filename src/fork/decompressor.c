#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../utilities/file_utils.c"
#include "../huffman/huffman.c"

#define SEM_1 "/sem_1"

int main() {
    pid_t pid;
    int numOfProcess;
    struct DirectoryMetadata dirMetadata;
    FILE *binary_source;
    const char *dir_result, *dir_path, *output_path;

    // Ruta del archivo de salida
    output_path = "out/bin/compressed.bin";
    
    // Leer los metadatos del directorio, también lee los offsets para la descompresión
    binary_source = fopen(output_path, "rb");
    if (!binary_source) {
        perror("Failed to open output_path");
        exit(EXIT_FAILURE);
    }
    read_directory_metadata(&dirMetadata, binary_source);
    fclose(binary_source);

    // Crear carpeta para almacenar archivos descomprimidos
    dir_result = create_output_dir(dirMetadata.directory);
    dir_path = concat_strings(dir_result, "/");

    // Decodificar utilizando múltiples procesos
    numOfProcess = dirMetadata.numTxtFiles;
    for (int i = 0; i < numOfProcess; i++) {
        pid = fork();
        
        if (pid == 0) { // Proceso hijo
            FILE *local_binary_source = fopen(output_path, "rb");
            if (!local_binary_source) {
                perror("Error opening binary source file in child process");
                exit(EXIT_FAILURE);
            }
            
            size_t currentOffset = dirMetadata.offsets[i];
            fseek(local_binary_source, currentOffset, SEEK_SET);
            decompress_and_write_to_file(local_binary_source, dir_path, i + 1);
            
            fclose(local_binary_source);
            exit(EXIT_SUCCESS);
        } else if (pid < 0) { // Manejo de errores de fork
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
    }
    
    // El proceso padre espera a todos los hijos
    while (numOfProcess > 0) {
        wait(NULL);
        numOfProcess--;
    }

    return EXIT_SUCCESS;
}
