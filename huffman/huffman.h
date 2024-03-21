#ifndef HUFFMAN_H
#define HUFFMAN_H

// ******** Frequency functions ********

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
void char_frequencies(wchar_t *buffer, int freq_table[]);

// ******** Huffman Algorithm ********



#endif // !HUFFMAN_H