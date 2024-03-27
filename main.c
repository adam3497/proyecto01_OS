#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "utilities/utils.h"
#include "huffman/huffman.h"

int main() {
    const char *input_file = "books/A Christmas Carol in Prose; Being a Ghost Story of Christmas by Charles Dickens (12287).txt";
    const char *output_file = "test/output.txt";
    wchar_t *buffer = NULL;
    get_wchars_from_file(input_file, &buffer);

    // Initialize the freq_table with zeros
    int freq_table[CHAR_SET_SIZE] = {0};

    // Extract the frequencies for each character in the text file
    char_frequencies(buffer, freq_table);
    
    // Write the wchar and its frequency to the output file
    write_wchars_to_file(output_file, freq_table);

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
    write_huffman_codes_to_file("test/output_huffman_tree.txt", huffmanCodesArray);

    return 0;
}