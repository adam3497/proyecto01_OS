#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

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
    printf("- Frequencies written to file successfully\n");
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
    printf("- Huffman codes written to file successfully\n");
    
}

// ENCODE

void encode_file(wchar_t *buffer, const char* filename, struct HuffmanCode* huffmanCodes[]) {
    FILE *file;

    // Open the output file
    file = fopen(filename, "w, ccs=UTF-8");
    if (file == NULL) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    printf("- Encoding file with Huffman codes\n");

    // Iterate through the wchar_t *buffer
    wchar_t *ptr = buffer;
    while (*ptr != L'\0') {
        for (int i = 0; i < huffmanCodes[*ptr]->length; i++) {
            fwprintf(file, L"%d", huffmanCodes[*ptr]->code[i]);
        }
        ptr++;
    }

    fclose(file);
    printf("- File encoded successfully\n");

}

/**
 * 
*/
// Function to write directory metadata to the binary file
long write_directory_metadata(FILE *binary_file, const struct DirectoryMetadata* metadata) {
    
    // Write directory name length
    size_t dirname_length = strlen(metadata->directory);
    fwrite(&dirname_length, sizeof(size_t), 1, binary_file);
    
    // Write directory name
    fwrite(metadata->directory, sizeof(char), dirname_length, binary_file);
    
    // Write number of text files
    fwrite(&(metadata->numTxtFiles), sizeof(int), 1, binary_file);

    // Get the current position in the file
    long position_before_offsets = ftell(binary_file);

    // Write offsets array
    fwrite(metadata->offsets, sizeof(size_t), metadata->numTxtFiles, binary_file);

    // Return the position before writing the offsets array
    return position_before_offsets;
}

/**
 * 
*/
void serialize_huffman_tree(struct MinHeapNode* root, FILE* file) {
    // Write a marker for NULL node
    if (root == NULL) {
        fputc(0, file);
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

/**
 * 
*/
void write_metadata(size_t offset, const char* filename, size_t size, FILE* file) {
    // Write offset, filename length, and filename string
    fwrite(&offset, sizeof(size_t), 1, file);
    size_t filename_length = strlen(filename);
    fwrite(&filename_length, sizeof(size_t), 1, file);
    fwrite(filename, sizeof(char), filename_length, file);

    // Write file size
    fwrite(&size, sizeof(size_t), 1, file);
}

const char* extract_filename(const char* filepath) {
    const char* filename = filepath;
    const char* last_slash = strrchr(filepath, '/'); // Find the last occurrence of '/'
    if (last_slash != NULL) {
        filename = last_slash + 1; // Move pointer to the last character after the last '/'
    }
    return filename;
}

void set_lock(FILE *fp, int type) {
    int fd = fileno(fp); // Obtener el descriptor de archivo
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = type; // F_RDLCK para bloqueo de lectura, F_WRLCK para bloqueo de escritura
    lock.l_whence = SEEK_SET;
    lock.l_start = 0; // Inicio del archivo
    lock.l_len = 0; // 0 significa hasta el final del archivo

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error al intentar establecer el bloqueo");
        exit(1);
    }
}

void unlock_file(FILE *fp) {
    int fd = fileno(fp); // Obtener el descriptor de archivo
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_UNLCK; // Desbloquear
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("Error al intentar desbloquear el archivo");
        exit(1);
    }
}



void write_encoded_bits_to_file(wchar_t *buffer, size_t buffer_size, const char* filepath, struct MinHeapNode* huffmanRoot, struct HuffmanCode* huffmanCodes[], FILE *output_file, size_t* offsetsPtr, int pos) {
    // Extract the name for the current file
    const char* filename = extract_filename(filepath);
    
    // Get current position in output file (offset)
    size_t offset = ftell(output_file);
    offsetsPtr[pos-1] = offset;

    // Write metadata (filename and size) to the output file;
    write_metadata(offset, filename, buffer_size, output_file);
    
    // Serialize Huffman tree and write it to the output file
    serialize_huffman_tree(huffmanRoot, output_file);

    // Compress data using Huffman codes and write it to the output file
    // Buffer to store bits before writing to file
    unsigned char buffer_byte = 0; 
    
    // Number of bits currently buffered
    int bit_count = 0;

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
}


// DECODE

/**
 * 
*/
void read_directory_metadata(struct DirectoryMetadata* metadata, FILE* binary_file) {
    // Read directory name length
    size_t dirname_length;
    if (fread(&dirname_length, sizeof(size_t), 1, binary_file) != 1) {
        perror("Error reading directory name length");
        exit(EXIT_FAILURE);
    }
    
    // Allocate memory for directory name buffer
    char* dirname_buffer = (char*)malloc((dirname_length + 1) * sizeof(char));
    if (dirname_buffer == NULL) {
        perror("Error allocating memory for directory name buffer");
        exit(EXIT_FAILURE);
    }

    // Read directory name from file
    if (fread(dirname_buffer, sizeof(char), dirname_length, binary_file) != dirname_length) {
        perror("Error reading directory name from file");
        exit(EXIT_FAILURE);
    }
    dirname_buffer[dirname_length] = '\0';

    // Assign directory name to metadata
    metadata->directory = dirname_buffer;
    
    // Read number of text files
    if (fread(&(metadata->numTxtFiles), sizeof(int), 1, binary_file) != 1) {
        perror("Error reading number of text files");
        exit(EXIT_FAILURE);
    }

    // Read offsets array
    size_t numOffsets = metadata->numTxtFiles;
    if (fread(metadata->offsets, sizeof(size_t), numOffsets, binary_file) != numOffsets) {
        perror("Error reading offsets array from file");
        exit(EXIT_FAILURE);
    }
}

/**
 * 
*/
const char* create_output_dir(const char* dirname) {
    const char* out_path = "out/";
    const char* dir_path = concat_strings(out_path, dirname);
    // Check if folder already exists
    struct stat st;
    if (stat(dir_path, &st) == -1) {
        // Folder doesn't exist, create it
        if (mkdir(dir_path, 0777) == -1) { 
            if (errno == EEXIST) {
            printf("Folder '%s' already exists.\n", dir_path);
            } else {
                perror("Error creating folder");
                exit(EXIT_FAILURE);
            }
        } else {
            printf("Folder '%s' created successfully.\n", dir_path);
        }
        
    } else {
        printf("Folder '%s' already exists.\n", dir_path);
    }
    return dir_path;
}

/**
 * 
*/
const char* concat_strings(const char* str1, const char* str2) {
    // Calculate the total length of the concatenated string
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t total_len = len1 + len2 + 1; // Add 1 for null terminator

    // Allocate memory for the concatenated string
    char* result = (char*)malloc(total_len);
    if (result == NULL) {
        perror("Error allocating memory for the new string");
        exit(EXIT_FAILURE);
    }

    // Copy the first string into the result buffer
    strcpy(result, str1);

    // Concatenate the second string to the end of the first string
    strcat(result, str2);

    return result;
}

/**
 * 
*/
struct MinHeapNode* deserialize_huffman_tree(FILE* file) {
    int marker = fgetc(file);

    // NULL node marker
    if (marker == 0) return NULL;

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
void read_metadata(size_t* offset, const char* filename, size_t* size, FILE* file) {
    // Read offset
    if (fread(offset, sizeof(size_t), 1, file) != 1) {
        perror("- Error reading offset");
        exit(EXIT_FAILURE);
    }
    
    // Read filename length
    size_t filename_length;
    if (fread(&filename_length, sizeof(size_t), 1, file) != 1) {
        perror("- Error reading filename length");
        exit(EXIT_FAILURE);
    }

    // Ensure the filename length is reasonable
    if (filename_length == 0 || filename_length > MAX_BOOK_NAME_LENGTH) {
        fprintf(stderr, "- Invalid filename length: %zu\n", filename_length);
        exit(EXIT_FAILURE);
    }
    
    // Allocate memory for filename buffer
    char* filename_buffer = (char*)malloc((filename_length + 1) * sizeof(char));
    if (filename_buffer == NULL) {
        perror("- Error allocating memory for filename buffer");
        exit(EXIT_FAILURE);
    }

    // Read filename from file
    if (fread(filename_buffer, sizeof(char), filename_length, file) != filename_length) {
        perror("- Error reading filename from file");
        exit(EXIT_FAILURE);
    }
    filename_buffer[filename_length] = '\0';

    // Read file size
    if (fread(size, sizeof(size_t), 1, file) != 1) {
        perror("- Error reading file size");
        exit(EXIT_FAILURE);
    }
    
    strcpy((char *) filename, filename_buffer);
    free(filename_buffer);
}

/**
 * @param source: binary file to be decompressed
 * @param output_path: name of the output directory where the files are gonna be stored
*/
void decompress_and_write_to_file(FILE *source, const char *output_path, int pos) {
    
    // Open binary file 
    if (source == NULL) {
        perror("Error opening binary file");
        exit(EXIT_FAILURE);
    }

    // Read metadata (filename and size) from the input file
    size_t file_size;
    char filename[256];
    size_t offset;
    read_metadata(&offset, filename, &file_size, source);
    printf("[PID %d][DECODING #%d] %s\n", getpid(), pos, filename);

    // Deserialize Huffman Tree from the binary file
    struct MinHeapNode* huffmanRoot = deserialize_huffman_tree(source);
    if (huffmanRoot == NULL) {
        perror("Error allocating memory for Huffman tree node");
        exit(EXIT_FAILURE);
    }
    struct MinHeapNode* currentNode = huffmanRoot;

    // Set the locale to handle wide characters properly
    setlocale(LC_ALL, "");
    
    // Concatenate output_path with filename to create the full file path
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s/%s", output_path, filename);

    // Open output file
    FILE *output_file = fopen(file_path, "w+, ccs=UTF-8");

    if (output_file == NULL) {
        perror("Error opening output file");
        fclose(source);
        exit(EXIT_FAILURE);
    }

    // Read the encoded bits from the input file and decode them
    unsigned char buffer;
    size_t bytes_written = 0;
    while (bytes_written < file_size) {
        buffer = fgetc(source);
        for (int i = 7; i >=0; --i) {
            int bit = (buffer >> i) & 1;
            if (bit == 0) {
                currentNode = currentNode->left;
            } else {
                currentNode = currentNode->right;
            }

            // If we reach a leaf node (character node), write the character to the output file
            if (!(currentNode->left) && !(currentNode->right)) {
                fputwc(currentNode->data, output_file);
                currentNode = huffmanRoot; // Reset to root for the next character
                bytes_written++;
                if (bytes_written >= file_size) {
                    break;
                }
            }
        }
    }
    
    free_huffman_tree(huffmanRoot);
    // Assert end of usage 
    fclose(output_file);
}


struct EncodeArgs* getAllPaths(const char* booksFolder){

    // Array dinámico para almacenar los nombres de los libros
    int fileCounter = 0;
    struct EncodeArgs* args = (struct EncodeArgs*) malloc(sizeof(struct EncodeArgs));
        
    // Paths
    DIR* dir;
    struct dirent* entrada;

    dir = opendir(booksFolder);
    if (dir == NULL) {
        perror("Error al abrir el directorio");
    }

    while ((entrada = readdir(dir)) != NULL) {
        if (strstr(entrada->d_name, ".txt") != NULL) {
            // Book paths
            char bookPath[MAX_BOOK_NAME_LENGTH];
            snprintf(bookPath, sizeof(bookPath), "%s/%s", booksFolder, entrada->d_name);
            strncpy(args->books[fileCounter], bookPath, MAX_BOOK_NAME_LENGTH - 1);

            // Save atributes
            args->books[fileCounter][MAX_BOOK_NAME_LENGTH - 1] = '\0';                        
            fileCounter++;
        }
    }
    // Save the amount of files within the directory
    args->fileCount = fileCounter;
    closedir(dir);
    return args;
}
