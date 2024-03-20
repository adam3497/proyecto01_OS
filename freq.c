#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#define CHAR_SET_SIZE 65536
#define CHUNK_SIZE 1024 // Size of each chunk to read

void char_frequencies(wchar_t input_array[], int frequency_table[]) {
    // Initialize frequency table
    for (int i = 0; i < CHAR_SET_SIZE; i++)
        frequency_table[i] = 0;

    // Iterate through the input array
    for (int i = 0; input_array[i] != L'\0'; i++)
        // Increment the frequency of the char in the table
        frequency_table[input_array[i]]++;
}

/**
 * Function to read a given file using wchar (wide characters)
 * @param filename: a string with the name of the file
 * @param buffer: a pointer to the buffer where the characters are gonna be stored
 * @return: This function does not return 
*/
int get_chars_from_file(const char *filename, wchar_t *buffer) {
    FILE *file;
    size_t buffer_size = 0; // Current buffer size
    size_t total_size = 0; // Total buffer size

    // Open the file for reading
    file = fopen(filename, "rb"); // Open in text mode for wide character handling
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Read the file in chunks and dynamically allocate memory
    while (!feof(file)) {
        // Increase buffer size by CHUNK_SIZE
        buffer_size += CHUNK_SIZE;
        buffer = (wchar_t *)realloc(buffer, buffer_size * sizeof(wchar_t)); // reallocate memory

        // Read a chunk of data from the file
        size_t chars_read = fgetws(buffer + total_size, CHUNK_SIZE, file);
        if (chars_read == 0 && !feof(file)) {
            perror("Error reading from file");
            free(buffer); // Free allocated memory
            fclose(file); // Close the file
            return 1;
        }

        // Update total size
        total_size += chars_read;
    }

    // Close the file
    fclose(file);

    // Null-terminate the buffer
    buffer = (wchar_t *)realloc(buffer, (total_size + 1) * sizeof(wchar_t));
    buffer[total_size] = L'\0';

    // Use the buffer here (e.g., print its content)
    wprintf(L"File content:\n%s\n", buffer);

    return 0;
}

int main() {
    wchar_t *buffer = NULL;
    int status = get_chars_from_file("input.txt", buffer);

    if (status != 0) 
        return 1;
    
    return 0;
}