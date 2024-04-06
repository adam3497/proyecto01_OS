#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "../utilities/file_utils.c"
#include "../huffman/huffman.c"
#include "../huffman/freq.c"


void encode(char *input_file, FILE *binary_output, size_t *offsets, int pos) {
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
    // Folder Paths
    const char* booksFolder = "books";
    const char* out = "out/bin/compressed.bin";

    FILE *binary_output = fopen(out, "wb");
    if (binary_output == NULL) {
        perror("Error opening output binary file");
        exit(EXIT_FAILURE);
    }

    // Set for every book in books folder
    struct EncodeArgs *paths = getAllPaths(booksFolder);

    struct DirectoryMetadata dirMetadata = {
        .directory = booksFolder,
        .numTxtFiles = paths->fileCount,
        .offsets = {0}
    };

    // Write content metadata to binary file and get the position for the offsets array
    long offsets_pos = write_directory_metadata(binary_output, &dirMetadata);

    // Offsets array to be populated
    size_t offsets[MAX_TOTAL_BOOKS] = {0};

    // Encode
    for (int i = 0; i < paths->fileCount; i++) {
        printf("[CODING #%d] %s\n", i+1, paths->books[i]);
        encode(paths->books[i], binary_output, offsets, i+1);
    }
    
    // Update the offsets array in the binary file
    fseek(binary_output, offsets_pos, SEEK_SET);
    fwrite(offsets, sizeof(size_t), paths->fileCount, binary_output);


    fclose(binary_output);
    free(paths);
    return EXIT_SUCCESS;
}
