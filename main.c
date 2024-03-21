#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "utilities/utils.h"
#include "huffman/huffman.h"

int main() {
    const char *input_file = "books/A Christmas Carol in Prose; Being a Ghost Story of Christmas by Charles Dickens (12062).txt";
    const char *output_file = "test/output.txt";
    wchar_t *buffer = NULL;
    get_wchars_from_file(input_file, &buffer);

    // Initialize the freq_table with zeros
    int freq_table[CHAR_SET_SIZE] = {0};

    // Extract the frequencies for each character in the text file
    char_frequencies(buffer, freq_table);
    
    // Write the wchar and its frequency to the output file
    write_wchars_to_file(output_file, freq_table);

    return 0;
}