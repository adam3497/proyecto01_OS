#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "../utilities/file_utils.c"
#include "../huffman/huffman.c"


int main() {
    // Folder Paths
    const char* booksFolder = "books";
    const char* out = "out/bin/books_compressed_serial.bin";
    
    FILE *binary_source = fopen(out, "rb");
    if (binary_source == NULL) {
        perror("Error opening binary file for decompression");
        exit(EXIT_FAILURE);
    }

    // Read directory metadata
    struct DirectoryMetadata dirMetadata;
    read_directory_metadata(&dirMetadata, binary_source);

    printf("Directory name: %s", dirMetadata.directory);
    // Create folder to store decompressed files
    const char* dir_result = create_output_dir(dirMetadata.directory);
    const char* dir_path = concat_strings(dir_result, "/");

    // Set for every book in books folder
    struct EncodeArgs *paths = getAllPaths(booksFolder);
    
    // Number of books to compress
    int runs = TOTAL_BOOKS;

    // Decode 
    for (int i = 0; i < runs; i++) {
        printf("[%d] DECODING : %s\n", i+1, paths->books[i]);
        decompress_and_write_to_file(binary_source, dir_path);
        printf("\n");
    }

    // Liberar la memoria asignada
    fclose(binary_source);
    free(paths);
    return EXIT_SUCCESS;
}
