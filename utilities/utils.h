#ifndef UTILS_h
#define UTILS_H

#define INITIAL_BUFFER_SIZE 1024 // size for the buffer to read
#define CHAR_SET_SIZE 65536

// ******** File Utils ********

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
void get_wchars_from_file(const char *filename, wchar_t **buffer);

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
void write_wchars_to_file(const char *filename, int freq_table[]);

#endif // !UTILS_H