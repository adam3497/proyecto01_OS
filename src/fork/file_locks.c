#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#include "../utilities/utils.h"

void set_lock(FILE *fp, int type) {
    int fd = fileno(fp); // Obtener el descriptor de archivo
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = type; // F_RDLCK para bloqueo de lectura, F_WRLCK para bloqueo de escritura
    lock.l_whence = SEEK_SET;
    lock.l_start = 0; // Inicio del archivo
    lock.l_len = 0; // 0 significa hasta el final del archivo

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error al intentar establecer el bloqueo");
        exit(1);
    }
}

void unlock_file(FILE *fp) {
    int fd = fileno(fp); // Obtener el descriptor de archivo
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_UNLCK; // Desbloquear
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("Error al intentar desbloquear el archivo");
        exit(1);
    }
}


// Function to read metadata (filename and size) from the binary file
void fork_read_metadata(const char* filename, size_t* size, FILE* file) {
    
    // Read filename length and filename string
    size_t filename_length;
    fread(&filename_length, sizeof(size_t), 1, file);
    
    printf("- Filename lenght: %zu\n", filename_length);
    
    char* filename_buffer = (char*)malloc((filename_length + 1) * sizeof(char));

    if (filename_buffer == NULL) {
        perror("Error allocating memory for filename buffer");
        printf("\n");
        exit(EXIT_FAILURE);
    }

    fread(filename_buffer, sizeof(char), filename_length, file);
    filename_buffer[filename_length] = '\0';
    
    strcpy((char *) filename, filename_buffer);
    
    free(filename_buffer);

    // Read file size
    fread(size, sizeof(size_t), 1, file);
    
}

void fork_decompress(FILE *source, const char *output_path) {
    printf("- Decompressing binary file\n");
    
    // Open binary file 
    if (source == NULL) {
        perror("Error opening binary file");
        exit(EXIT_FAILURE);
    }

    // Read metadata (filename and size) from the input file
    size_t file_size;
    char filename[256];
    fork_read_metadata(filename, &file_size, source);
    
    printf("- File name: %s\n", filename);
    printf("- File size: %zu B\n", file_size);

    // Deserialize Huffman Tree from the binary file
    struct MinHeapNode* huffmanRoot = deserialize_huffman_tree(source);

    if (huffmanRoot == NULL) {
        perror("Error allocating memory for Huffman tree node");
        exit(EXIT_FAILURE);
    }
    struct MinHeapNode* currentNode = huffmanRoot;

    // Set the locale to handle wide characters properly
    setlocale(LC_ALL, "");
    
    // Open output file
    FILE *output_file = fopen(output_path, "w, ccs=UTF-8");

    if (output_file == NULL) {
        perror("Error opening output file");
        fclose(source);
        exit(EXIT_FAILURE);
    }
    

    // set_lock(source, F_RDLCK);
    // Read the encoded bits from the input file and decode them
    unsigned char buffer;
    size_t bytes_written = 0;
    while (bytes_written < file_size) {
        buffer = fgetc(source);
        for (int i = 7; i >=0; --i) {
            int bit = (buffer >> i) & 1;
            if (bit == 0) {
                currentNode = currentNode->left;
            } else {
                currentNode = currentNode->right;
            }

            // If we reach a leaf node (character node), write the character to the output file
            if (!(currentNode->left) && !(currentNode->right)) {
                fputwc(currentNode->data, output_file);
                currentNode = huffmanRoot; // Reset to root for the next character
                bytes_written++;
                if (bytes_written >= file_size) {
                    break;
                }
            }
        }
    }

    // Assert end of usage 
    // unlock_file(source);
    fclose(output_file);
}