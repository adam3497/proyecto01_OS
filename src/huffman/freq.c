#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

#include "../utilities/utils.h"
#include "huffman.h"

/**
 * @brief Function to extract character frequencies from a buffer of wide characters (wchar_t)
 * @param buffer: a pointer to the buffer containing the characters
 * @param freq_table: an array representing the frequency of each character
 * @return void
 * 
 * @details This function extracts character frequencies from the specified buffer of wide characters (wchar_t).
 * It iterates through the buffer, ignoring spaces, line breaks, and carriage returns, and increments the frequency
 * of each non-space/non-line-break character in the freq_table array.
 */
void char_frequencies(wchar_t *buffer, int freq_table[]) {
    // Iterate through the wchar_t buffer
    wchar_t *ptr = buffer;
    while (*ptr != L'\0') {
        // Increment the frequency of the character
        freq_table[*ptr]++;
        ptr++; // Increment the pointer to move to the next character
    }
}