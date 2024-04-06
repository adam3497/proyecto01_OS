#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <dirent.h>

#include "../utilities/file_utils.c"
#include "../huffman/huffman.c"
#include "../huffman/freq.c"

#define BUFFER_SIZE 4096
#define TEMP_FILENAME_LEN 256
#define SHM_SIZE (MAX_TOTAL_BOOKS * sizeof(size_t))

void init_shared_memory(int shmid, key_t key, size_t **offsets_ptr){
    // Crear segmento de memoria compartida
    if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Adjuntar el segmento de memoria compartida al proceso
    size_t *offsets = shmat(shmid, NULL, 0);
    if (offsets == (size_t *) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Asignar el puntero a puntero
    *offsets_ptr = offsets;

    // Inicializar el array en el proceso padre
    for (int i = 0; i < MAX_TOTAL_BOOKS; ++i) {
        (*offsets_ptr)[i] = 0;
    }
}

void encode(const char *input_file, FILE *output_temp_file, size_t *offsets, int pos) {
    // Variables
    wchar_t *buffer;
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

    // Escribir los códigos de Huffman en el archivo temporal
    buffer_size = wcslen(buffer);
    write_encoded_bits_to_file(buffer, buffer_size, input_file, huffmanRoot, huffmanCodesArray, output_temp_file, offsets, pos);

    // Liberar memoria
    free(buffer);
}

void get_temp_paths(char temp_filenames[TOTAL_BOOKS][TEMP_FILENAME_LEN]){
    DIR* dir;
    int count;
    struct dirent* entrada;

    // Recolección y escritura de resultados por el proceso padre
    dir = opendir("out/temp");
    if (dir == NULL) {
        perror("Error al abrir el directorio");
        exit(EXIT_FAILURE);
    }
    
    count = 0;
    while ((entrada = readdir(dir)) != NULL) {
        if (strstr(entrada->d_name, ".bin") != NULL) {
            snprintf(temp_filenames[count], sizeof(temp_filenames[count]), "out/temp/%s", entrada->d_name);
            count++;
        }
    }
}

int main() {
    // Shared memory
    int shmid;
    size_t *offsets;
    key_t key = IPC_PRIVATE;

    // Source folder
    const char* booksFolder = "books";
    const char* out = "out/bin/compressed.bin";
    struct EncodeArgs *paths;
    
    // File variables
    FILE *binary_output;
    long offsets_pos;

    // Process    
    pid_t pid;
    
    /* 1. Compartir offsets con todos los hijos */
    init_shared_memory(shmid, key, &offsets);

    binary_output = fopen(out, "wb");
    if (binary_output == NULL) {
        perror("Error opening output binary file");
        exit(EXIT_FAILURE);
    }

    // Temporary folders
    char temp_filenames[TOTAL_BOOKS][TEMP_FILENAME_LEN];

    // Obtener rutas de todos los libros en la carpeta de libros
    paths = getAllPaths(booksFolder);

    // Metadatos del directorio
    struct DirectoryMetadata dirMetadata = {
        .directory = booksFolder,
        .numTxtFiles = paths->fileCount,
        .offsets = {0}
    };

    // Escribir los metadatos del directorio al archivo binario y obtener la posición para el array de offsets
    offsets_pos = write_directory_metadata(binary_output, &dirMetadata);
    
    // Paralelizar codificación de libros
    for (int i = 0; i < TOTAL_BOOKS; i++) {
        pid = fork();
        
        if (pid == 0) {
            // Variables del hijo
            FILE *temp_output;
            size_t temp_offsets[MAX_TOTAL_BOOKS] = {0};
            char temp_filename[TEMP_FILENAME_LEN];

            // Construir el nombre del archivo temporal único para este proceso hijo
            snprintf(temp_filename, sizeof(temp_filename), "out/temp/temp_%d.bin", getpid());

            temp_output = fopen(temp_filename, "wb");
            if (temp_output == NULL) {
                perror("Error opening output binary file");
                exit(EXIT_FAILURE);
            }

            // Codificar el libro y escribir los resultados en el archivo temporal
            printf("[PID %d][CODING #%d] %s\n", getpid(), i+1, temp_filename);
            encode(paths->books[i], temp_output, temp_offsets, i);

            fclose(temp_output);
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

    // Obtiene las rutas d elos archivos temporales en una lista
    get_temp_paths(temp_filenames);


    // Concatena todos los archivos en uno solo
    for (int i = 0; i < TOTAL_BOOKS; i++) {
        FILE *input_file = fopen(temp_filenames[i], "rb");
        if (input_file == NULL) {
            perror("Error opening input file");
            exit(EXIT_FAILURE);
        }

        fseek(input_file, 0, SEEK_SET);

        unsigned char buffer[BUFFER_SIZE];
        size_t bytes_read;
        
        offsets[i] = ftell(binary_output);
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), input_file)) > 0) {
            fwrite(buffer, 1, bytes_read, binary_output);
        }

        fclose(input_file);
    }

    // Escribir los offsets al archivo de salida
    fseek(binary_output, offsets_pos, SEEK_SET);
    fwrite(offsets, sizeof(size_t), paths->fileCount, binary_output);
    
    // Liberar recursos
    shmdt(offsets);
    shmctl(shmid, IPC_RMID, NULL);
    fclose(binary_output);
    free(paths);
    
    return EXIT_SUCCESS;
}
