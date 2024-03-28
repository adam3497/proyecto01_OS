#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <dirent.h>
#include "utilities/utils.h"
#include "huffman/huffman.h"

typedef struct {
    char file_name[200];
    long size;
} MetadataArchivo;

void encode(const char *input_file, const char *freq_file, const char *bin_file){

    wchar_t *buffer = NULL;
    get_wchars_from_file(input_file, &buffer);

    // Initialize the freq_table with zeros
    int freq_table[CHAR_SET_SIZE] = {0};

    // Extract the frequencies for each character in the text file
    char_frequencies(buffer, freq_table);
    
    // Write the wchar and its frequency to the output file
    write_wchars_to_file(freq_file, freq_table);

    // We first calculate the size of the freq table (only the characters' freq > 0)
    int freq_table_size = calculateFreqTableSize(freq_table);

    // Build Huffman Tree from the frequency table
    struct MinHeapNode* huffmanRoot = buildHuffmanTree(freq_table, freq_table_size);

    // Huffman codes 2D array where the first array contains the character and the second array
    // the length of the code and the code for that character
    struct HuffmanCode* huffmanCodesArray[MAX_FREQ_TABLE_SIZE] = {NULL};
    
    // An array where the Huffman code for each character is gonna be stored
    int bits[MAX_CODE_SIZE];

    // Generate Huffman codes for each character in the text file
    generateHuffmanCodes(huffmanRoot, bits, 0, huffmanCodesArray);

    // Write the Huffman Codes to file    
    size_t buffer_size = wcslen(buffer);
    write_encoded_bits_to_file(buffer, buffer_size, bin_file, huffmanRoot, huffmanCodesArray);
}

int main() {
    DIR* dir;
    struct dirent* entrada;
    const char* frequenciesFolder = "out/frequencies"; // Ruta de la carpeta, ajustar según sea necesario
    const char* binariesFolder = "out/bins"; // Ruta de la carpeta, ajustar según sea necesario
    const char* resourceFolder = "books"; // Ruta de la carpeta, ajustar según sea necesario
    const char* extension = ".txt";
    
    dir = opendir(resourceFolder);
    if (dir == NULL) {
        perror("Error al abrir el directorio");
        return EXIT_FAILURE;
    }

    while ((entrada = readdir(dir)) != NULL) {
        // Verificar si el archivo termina en .txt
        if (strstr(entrada->d_name, extension) != NULL) {
            
            // Book path
            char bookPath[1024];
            snprintf(bookPath, sizeof(bookPath), "%s/%s", resourceFolder, entrada->d_name);

            // Frequencies output path
            char frequenciesPath[1024];
            snprintf(frequenciesPath, sizeof(frequenciesPath), "%s/%s", frequenciesFolder, entrada->d_name);

            // Remove file extension
            int len = strlen(entrada->d_name);
            entrada->d_name[len-4] = '\0';

            char binariesPath[1024];
            snprintf(binariesPath, sizeof(binariesPath), "%s/%s.bin", binariesFolder, entrada->d_name);

            printf("Codificando: %s\n", bookPath);
            encode(bookPath, frequenciesPath, binariesPath);
            // Deserialize the binary file
            //decompress_and_write_to_file(binariesPath, "test/decompress.txt");
            printf("\n");
        }
    }

    closedir(dir);
    return EXIT_SUCCESS;
}