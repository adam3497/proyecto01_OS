#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>


#include "../utilities/file_utils.c"
#include "../huffman/huffman.c"
#include "../huffman/freq.c"
#include "file_locks.c"

#define SHM_SIZE (MAX_TOTAL_BOOKS * sizeof(size_t))
#define SEM_1 "/sem_1"

void encode(char *input_file, char *freq_file, FILE *binary_output, size_t *offsets, int pos) {
    // Variables al inicio
    wchar_t *buffer = NULL;
    int freq_table[CHAR_SET_SIZE] = {0};
    int freq_table_size;
    struct MinHeapNode* huffmanRoot;
    struct HuffmanCode* huffmanCodesArray[MAX_FREQ_TABLE_SIZE] = {NULL};
    int bits[MAX_CODE_SIZE];
    size_t buffer_size;

    // Llenar el búfer
    get_wchars_from_file(input_file, &buffer);

    // Extraer las frecuencias para cada carácter en el archivo de texto
    char_frequencies(buffer, freq_table);
    
    // Calculamos el tamaño de la tabla de frecuencias (solo las frecuencias de caracteres > 0)
    freq_table_size = calculateFreqTableSize(freq_table);

    // Construir el árbol de Huffman a partir de la tabla de frecuencias
    huffmanRoot = buildHuffmanTree(freq_table, freq_table_size);

    // Generar códigos Huffman para cada carácter en el archivo de texto
    generateHuffmanCodes(huffmanRoot, bits, 0, huffmanCodesArray);

    // Escribir los códigos de Huffman en el archivo
    buffer_size = wcslen(buffer);
    write_encoded_bits_to_file(buffer, buffer_size, input_file, huffmanRoot, huffmanCodesArray, binary_output, offsets, pos);

    // Liberar memoria
    free(buffer);
}

int main() {
    // Variables al inicio
    int shmid;
    pid_t pid;
    sem_t *sem = NULL;

    size_t *offsets;
    key_t key = IPC_PRIVATE; // Usar IPC_PRIVATE por simplicidad (usar una clave real en producción)

    // Crear segmento de memoria compartida
    if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Adjuntar el segmento de memoria compartida al proceso
    if ((offsets = shmat(shmid, NULL, 0)) == (size_t *) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Inicializar el array en el proceso padre
    for (int i = 0; i < MAX_TOTAL_BOOKS; ++i) {
        offsets[i] = 0;
    }
    
    // Rutas de carpetas y archivos
    const char* booksFolder = "books";
    const char* out = "out/bin/compressed.bin";

    FILE *binary_output = fopen(out, "wb");
    if (binary_output == NULL) {
        perror("Error opening output binary file");
        exit(EXIT_FAILURE);
    }

    // Obtener rutas de todos los libros en la carpeta de libros
    struct EncodeArgs *paths = getAllPaths(booksFolder);

    // Metadatos del directorio
    struct DirectoryMetadata dirMetadata = {
        .directory = booksFolder,
        .numTxtFiles = paths->fileCount,
        .offsets = {0}
    };

    // Escribir los metadatos del directorio al archivo binario y obtener la posición para el array de offsets
    long offsets_pos = write_directory_metadata(binary_output, &dirMetadata);
    
    sem = sem_open(SEM_1, O_CREAT | O_EXCL, 0644, 1);
     
    // Codificar
    for (int i = 0; i < TOTAL_BOOKS; i++) {
        pid = fork();
        
        // Código específico del proceso hijo
        if (pid == 0) {
            sem_wait(sem);
            
            // Escribir
            printf("[PID %d][CODING #%d] %s\n", getpid(), i+1, paths->books[i]);
            encode(paths->books[i], paths->freqs[i], binary_output, offsets, i+1);

            sem_post(sem);
            sem_close(sem);

            exit(EXIT_SUCCESS);
        } else if (pid < 0) {
            // Error al crear el proceso hijo
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }
    
    // El padre espera a todos los hijos
    for (int i = 0; i < TOTAL_BOOKS; i++) {
        wait(NULL);
    }

    sem_close(sem);
    sem_unlink(SEM_1);

    fseek(binary_output, offsets_pos, SEEK_SET);
    fwrite(offsets, sizeof(size_t), paths->fileCount, binary_output);
    
    shmdt(offsets);
    shmctl(shmid, IPC_RMID, NULL);
    fclose(binary_output);
    free(paths);
    
    return EXIT_SUCCESS;
}
