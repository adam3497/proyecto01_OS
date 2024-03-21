#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>

#include "utils.h"

/**
 * @brief Function to read characters from a file into a buffer using wide characters (wchar_t)
 * @param filename: a string with the name of the input file
 * @param buffer: a pointer to the buffer where the characters will be stored
 * @return void
 * 
 * @details This function reads characters from the specified input file using wide characters (wchar_t)
 * and stores them in the provided buffer. The buffer is dynamically allocated and resized as needed
 * to accommodate the file content. The function opens the file in wide character mode with UTF-8 encoding.
*/
void get_wchars_from_file(const char *filename, wchar_t **buffer) {
    FILE *file;
    size_t buffer_size = INITIAL_BUFFER_SIZE;
    size_t num_chars_read = 0;

    // Set the locale to handle wide characters properly
    setlocale(LC_ALL, "");

    // Open the file for reading in wide character mode
    file = fopen(filename, "r, ccs=UTF-8");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    // Allocate memory for the buffer
    *buffer = (wchar_t *)malloc(buffer_size * sizeof(wchar_t));
    if (*buffer == NULL) {
        perror("Error allocating memory for the buffer");
        fclose(file);
        exit(1);
    }

    // Read characters from the file into the buffer
    wint_t wchar;
    while ((wchar = fgetwc(file)) != WEOF) {
        // Check if the buffer needs to be reallocated
        if (num_chars_read > buffer_size - 1) {
            buffer_size *= 2; // Double the buffer size
            wchar_t *new_buffer = (wchar_t *)realloc(*buffer, buffer_size * sizeof(wchar_t));
            if (new_buffer == NULL) {
                perror("Error reallocating memory for the buffer");
                free(*buffer);
                fclose(file);
                exit(1);
            }
            *buffer = new_buffer;
        }

        // Store the character in the buffer
        (*buffer)[num_chars_read++] = wchar;
    }

    // Add null terminator to the end of the buffer
    (*buffer)[num_chars_read] = L'\0';

    // Close the file
    fclose(file);
    printf("Data read from input file\n");
}

/**
 * @brief Function to write character frequencies to a file using wide characters (wchar_t)
 * @param filename: a string with the name of the output file
 * @param freq_table: an array containing character frequencies
 * @return void
 * 
 * @details This function writes the character frequencies stored in the freq_table array to the specified output file.
 * It opens the output file in wide character mode with UTF-8 encoding.
 * For each non-zero frequency in the freq_table, it writes the corresponding character and its frequency to the file.
 * Characters are written as wide characters, and frequencies are written as integers, separated by a colon ':',
 * followed by a newline character '\n'.
*/
void write_wchars_to_file(const char *filename, int freq_table[]) {
    FILE *file;

    // Set the locale to handle wide characters properly
    setlocale(LC_ALL, "");

    // Open the output file
    file = fopen(filename, "w, ccs=UTF-8");
    if (file == NULL) {
        perror("Error opening output file");
        exit(1);
    }

    // Write content to the output file
    for (int i = 0; i < CHAR_SET_SIZE; i++) {
        if (freq_table[i] > 0) {
            // Write each character (converted to wchar_t) and its frequency
            fwprintf(file, L"%lc:%d\n", (wchar_t)i, freq_table[i]); 
        }
    }

    // Close the output file
    fclose(file);
    printf("Data written to file successfully\n");
}