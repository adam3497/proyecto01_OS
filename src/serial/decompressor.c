#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "../utilities/file_utils.c"
#include "../huffman/huffman.c"


int main() {
    // Folder Paths
    const char* input_file = "out/bin/compressed.bin";
    
    FILE *binary_source = fopen(input_file, "rb");
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

    // Decode 
    for (int i = 0; i < dirMetadata.numTxtFiles; i++) {
        decompress_and_write_to_file(binary_source, dir_path, i+1);
    }

    // Liberar la memoria asignada
    fclose(binary_source);
    return EXIT_SUCCESS;
}
