#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>

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
        exit(EXIT_FAILURE);
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
    printf("Frequencies written to file successfully\n");
}

void write_huffman_codes_to_file(const char* filename, struct HuffmanCode* huffmanCodes[]) {
    FILE *file;

    // Set the locale to handle wide characters properly
    setlocale(LC_ALL, "");

    // Open the output file
    file = fopen(filename, "w, ccs=UTF-8");
    if (file == NULL) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    // Write content to the output file
    for (int i = 0; i < MAX_FREQ_TABLE_SIZE; i++) {
        if (huffmanCodes[i] != NULL) {
            // Write each character (converted to wchar_t) and its frequency
            fwprintf(file, L"%lc: ", (wchar_t)i);
            for (int j = 0; j < huffmanCodes[i]->length; j++) {
                fwprintf(file, L"%d", huffmanCodes[i]->code[j]);
            } 
            fwprintf(file, L"\n");
        }
    }

    // Close the output file
    fclose(file);
    printf("Huffman codes written to file successfully\n");
    
}

void encode_file(wchar_t *buffer, const char* filename, struct HuffmanCode* huffmanCodes[]) {
    FILE *file;

    // Open the output file
    file = fopen(filename, "w, ccs=UTF-8");
    if (file == NULL) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    printf("Encoding file with Huffman codes\n");

    // Iterate through the wchar_t *buffer
    wchar_t *ptr = buffer;
    while (*ptr != L'\0') {
        for (int i = 0; i < huffmanCodes[*ptr]->length; i++) {
            fwprintf(file, L"%d", huffmanCodes[*ptr]->code[i]);
        }
        ptr++;
    }

    fclose(file);
    printf("File encoded successfully\n");

}

void serialize_huffman_tree(struct MinHeapNode* root, FILE* file) {
    if (root == NULL) {
        fputc(0, file); // Write a marker for NULL node
        return;
    }

    // Write a marker for non-NULL node and write node data
    fputc(1, file);
    fwrite(&(root->data), sizeof(wchar_t), 1, file);
    fwrite(&(root->freq), sizeof(unsigned), 1, file);

    // Recursively serialize left and right subtrees
    serialize_huffman_tree(root->left, file);
    serialize_huffman_tree(root->right, file);
}

void write_metadata(const char* filename, size_t size, FILE* file) {
    printf("Writing metadata to the binary file\n");
    // Write filename length and filename string
    size_t filename_length = strlen(filename);
    fwrite(&filename_length, sizeof(size_t), 1, file);
    fwrite(filename, sizeof(char), filename_length, file);

    // Write file size
    fwrite(&size, sizeof(size_t), 1, file);
    printf("Metadata written to binary file\n");
}

void write_encoded_bits_to_file(wchar_t *buffer, size_t buffer_size, const char* filename, struct MinHeapNode* huffmanRoot, struct HuffmanCode* huffmanCodes[]) {
    FILE *output_file = fopen(filename, "wb");
    if (output_file == NULL) {
        perror("Error opening output binary file");
        exit(EXIT_FAILURE);
    }

    // Write metadata (filename and size) to the output file;
    write_metadata(filename, buffer_size, output_file);

    printf("Serializing Huffman tree\n");
    // Serialize Huffman tree and write it to the output file
    serialize_huffman_tree(huffmanRoot, output_file);
    printf("Huffman tree serialized\n");

    printf("Encoding file...\n");
    // Compress data using Huffman codes and write it to the output file
    unsigned char buffer_byte = 0; // Buffer to store bits before writing to file
    int bit_count = 0; // Number of bits currently buffered

    // Iterate through the buffer character by character
    for (size_t i = 0; i < buffer_size; ++i) {
        // Get Huffman code for the current character
        int* code = huffmanCodes[buffer[i]]->code;
        int code_length = huffmanCodes[buffer[i]]->length; 

        // Write Huffman code to the output buffer
        for (int j = 0; j < code_length; ++j) {
            // Append the current bit to the buffer
            buffer_byte |= (code[j] << (7 - bit_count));
            ++bit_count;

            // If the buffer is full, write it to the file and reset it
            if (bit_count == 8) {
                fputc(buffer_byte, output_file);
                buffer_byte = 0;
                bit_count = 0;
            }
        }

    }

    // If there are remaining bits in the buffer, write them to the file
    if (bit_count > 0) {
        fputc(buffer_byte, output_file);
    }

    printf("File encoded\n");
    // Close output file
    fclose(output_file);
}


struct MinHeapNode* deserialize_huffman_tree(FILE* file) {
    int marker = fgetc(file);
    if (marker == 0) {
        return NULL; // NULL node marker
    }

    struct MinHeapNode* root = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    if (root == NULL) {
        perror("Error allocating memory for Huffman tree node");
        exit(EXIT_FAILURE);
    }

    // Read node data
    fread(&(root->data), sizeof(wchar_t), 1, file);
    fread(&(root->freq), sizeof(unsigned), 1, file);

    // Recursively deserialize left and right subtrees
    root->left = deserialize_huffman_tree(file);
    root->right = deserialize_huffman_tree(file);

    return root;
}

// Function to read metadata (filename and size) from the binary file
void read_metadata(const char* filename, size_t* size, FILE* file) {
    // Read filename length and filename string
    size_t filename_length;
    fread(&filename_length, sizeof(size_t), 1, file);
    char* filename_buffer = (char*)malloc((filename_length + 1) * sizeof(char));
    if (filename_buffer == NULL) {
        perror("Error allocating memory for filename buffer");
        exit(EXIT_FAILURE);
    }
    fread(filename_buffer, sizeof(char), filename_length, file);
    filename_buffer[filename_length] = '\0';
    strcpy((char *)filename, filename_buffer);
    free(filename_buffer);

    // Read file size
    fread(size, sizeof(size_t), 1, file);
}

void decompress_and_write_to_file(const char* binary_file, const char* output_file_name) {
    printf("Decompressing binary file\n");
    // Open binary file 
    FILE *input_file = fopen(binary_file, "rb");
    if (input_file == NULL) {
        perror("Error opening binary file");
        exit(EXIT_FAILURE);
    }

    // Read metadata (filename and size) from the input file
    size_t file_size;
    char filename[256];
    read_metadata(filename, &file_size, input_file);
    printf("File name: %c\n", filename);
    printf("File size: %zu\n", file_size);

    // Deserialize Huffman Tree from the binary file
    struct MinHeapNode* huffmanRoot = deserialize_huffman_tree(input_file);
    struct MinHeapNode* currentNode = huffmanRoot;

    // Open output file
    FILE *output_file = fopen(output_file_name, "wb");
    if (output_file == NULL) {
        perror("Error opening output file");
        fclose(input_file);
        exit(EXIT_FAILURE);
    }

    // Read the encoded bits from the input file and decode them
    unsigned char buffer;
    size_t bytes_written = 0;
    while (bytes_written < file_size) {
        buffer = fgetc(input_file);
        for (int i = 7; i >=0; --i) {
            int bit = (buffer >> i) & 1;
            if (bit == 0) {
                currentNode = currentNode->left;
            } else {
                currentNode = currentNode->right;
            }

            // If we reach a leaf node (character node), write the character to the output file
            if (!(currentNode->left) && !(currentNode->right)) {
                //fwprintf(output_file, L"%lc", currentNode->data);
                fputwc(currentNode->data, output_file);
                currentNode = huffmanRoot; // Reset to root for the next character
                bytes_written++;
                if (bytes_written >= file_size) {
                    break;
                }
            }
        }
    }

    // Close output file
    fclose(output_file);
    // Close input file
    fclose(input_file);
}