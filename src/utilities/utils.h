#ifndef UTILS_H
#define UTILS_H

#define INITIAL_BUFFER_SIZE 1024 // size for the buffer to read
#define CHAR_SET_SIZE 65536

#define TOTAL_BOOKS 98 
#define MAX_BOOK_NAME_LENGTH 256 
#define MAX_TOTAL_BOOKS 100

#define MAX_PATH_LENGTH 256

#include "../huffman/huffman.h"



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

/**
 * 
*/
void write_huffman_codes_to_file(const char* filename, struct HuffmanCode* huffmanCodes[]);

/**
 * 
*/
void encode_file(wchar_t *buffer, const char* filename, struct HuffmanCode* huffmanCodes[]);

/**
 * 
*/
void write_encoded_bits_to_file(wchar_t *buffer, size_t buffer_size, const char* filename, struct MinHeapNode* huffmanRoot, struct HuffmanCode* huffmanCodes[], FILE *output_file, size_t* offsetsPtr, int pos);

/**
 * 
*/
struct MinHeapNode* deserialize_huffman_tree(FILE* file);

/**
 * 
*/
void read_metadata(size_t* offset, const char* filename, size_t* size, FILE* file);

/**
 * 
 * 
*/
void decompress_and_write_to_file(FILE *source, const char *output_path, int pos);


struct EncodeArgs {
    char books[TOTAL_BOOKS][MAX_BOOK_NAME_LENGTH];
    int fileCount;
};

struct EncodeArgs* getAllPaths(const char* booksFolder);

// Struct to hold directory metadata
struct DirectoryMetadata {
    const char* directory;
    int numTxtFiles;
    size_t offsets[MAX_TOTAL_BOOKS];
};

/**
 * 
*/
long write_directory_metadata(FILE *binary_file, const struct DirectoryMetadata* metadata);

/**
 * 
*/
void read_directory_metadata(struct DirectoryMetadata* metadata, FILE* binary_file);

/**
 * 
*/
const char* create_output_dir(const char* dirname);

/**
 * 
*/
const char* concat_strings(const char* str1, const char* str2);

#endif // !UTILS_H