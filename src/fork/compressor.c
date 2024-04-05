#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "../utilities/file_utils.c"
#include "../huffman/huffman.c"
#include "../huffman/freq.c"
#include "file_locks.c"

#define SHM_SIZE (MAX_TOTAL_BOOKS * sizeof(size_t))
#define MAX_ACTIVE_PROCESSES 4 // number of processes that can be executing at a time
#define SEM_MUTEX_NAME "/sem_mutex"
#define SHM_NAME "/offsets"

// Semaphore for locking and unlocking the binary output file
sem_t *mutex;

void encode(char *input_file, FILE* binary_output, size_t *offsets, int pos){
    // Fill the buffer
    wchar_t *buffer = NULL;
    get_wchars_from_file(input_file, &buffer);

    // Extract the frequencies for each character in the text file
    int freq_table[CHAR_SET_SIZE] = {0};
    char_frequencies(buffer, freq_table);

    // We first calculate the size of the freq table (only the characters' freq > 0)
    int freq_table_size = calculateFreqTableSize(freq_table);

    // Build Huffman Tree from the frequency table
    struct MinHeapNode* huffmanRoot = buildHuffmanTree(freq_table, freq_table_size);

    // Huffman codes 2D array where the first array contains the character and the second array
    // the length of the code and the code for that character
    struct HuffmanCode* huffmanCodesArray[MAX_FREQ_TABLE_SIZE] = {NULL};
    
    // An array where the Huffman code for each character is gonna be stored
    int bits[MAX_CODE_SIZE];

    // Generate Huffman codes for each character in the text file
    generateHuffmanCodes(huffmanRoot, bits, 0, huffmanCodesArray);

    // Get the size of the buffer to encode into the binary output  
    size_t buffer_size = wcslen(buffer);

    // We lock the mutex to signal other processes to wait until the current process finish working on the binary output file
    sem_wait(mutex);

    // This process starts writing the encoded txt file into the binary output file
    write_encoded_bits_to_file(buffer, buffer_size, input_file, huffmanRoot, huffmanCodesArray, binary_output, offsets, pos);

    // Free dynamically allocated memory
    free_huffman_tree(huffmanRoot);
    free(buffer);

    // We release the semaphore in this child process
    sem_post(mutex); 
}

int main() {
    // Initialize shared memory for storing the offsets
    sem_unlink(SEM_MUTEX_NAME);
    size_t *offsets;
    int shm_offsets = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_offsets == -1) {
        perror("Error creating shared memory");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shm_offsets, SHM_SIZE) == -1) {
        perror("Error resizing shared memory");
        shm_unlink(SHM_NAME); // Cleanup shared memory if resizing fails
        exit(EXIT_FAILURE);
    }
    offsets = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_offsets, 0);
    if (offsets == MAP_FAILED) {
        perror("Error mapping shared memory");
        shm_unlink(SHM_NAME); // Cleanup shared memory if mapping fails
        exit(EXIT_FAILURE);
    }

    // Initialize the array in the parent process
    for (int i = 0; i < TOTAL_BOOKS; ++i) {
        offsets[i] = 0;
    }

    // Initialize the semaphore
    mutex = sem_open(SEM_MUTEX_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    if (mutex == SEM_FAILED) {
    perror("Error creating semaphore");
    if (errno == EEXIST) {
        // Semaphore already exists, open it without creating
        mutex = sem_open(SEM_MUTEX_NAME, 0);
        if (mutex == SEM_FAILED) {
            perror("Error opening existing semaphore");
            sem_unlink(SEM_MUTEX_NAME);
            exit(EXIT_FAILURE);
        }
    } else {
        exit(EXIT_FAILURE);
    }
}

    // Folder Paths
    const char* booksFolder = "books";
    const char* out = "out/bin/compressed.bin";

    // We open the binary file for the first time so if it doesn't exists, we created
    FILE *binary_output = fopen(out, "wb");
    if (binary_output == NULL) {
        perror("Error opening output binary file");
        exit(EXIT_FAILURE);
    }

    // Set for every book in books folder
    struct EncodeArgs *paths = getAllPaths(booksFolder);

    struct DirectoryMetadata dirMetadata = {
        .directory = booksFolder,
        .numTxtFiles = paths->fileCount,
        .offsets = {0}
    };

    // Write content metadata to binary file and get the position for the offsets array
    long offsets_pos = write_directory_metadata(binary_output, &dirMetadata);

    // Fork multiple processes, limiting to MAX_ACTIVE_PROCESSES
    int processes_remaining = paths->fileCount;
    int currentFile = 0;
    // Start encoding the text files within the input directory
    while (processes_remaining > 0) {
        int num_processes_to_spawn = (processes_remaining > MAX_ACTIVE_PROCESSES) ? MAX_ACTIVE_PROCESSES : processes_remaining;
        for (int i = 0; i < num_processes_to_spawn; i++) {
            char* input_file = paths->books[currentFile];
            pid_t pid = fork();
            
            // Child process
            if (pid == 0) {
                // Start encoding for the current file in this child processes
                printf("[PID %d][CODING #%d] %s\n", getpid(), currentFile+1, paths->books[currentFile]);
                encode(input_file, binary_output, offsets, currentFile);
                exit(EXIT_SUCCESS);
            } else if (pid == -1) {
                perror("Error creating a sub process with fork()");
                exit(EXIT_FAILURE);
            }
            currentFile++;  
            processes_remaining--;
        }
        for (int i = 0; i < num_processes_to_spawn; i++) {
            wait(NULL);
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < paths->fileCount; i++) {
        wait(NULL);
    }

    // Clean up semaphore 
    sem_close(mutex);
    sem_unlink(SEM_MUTEX_NAME);

    // Update the offsets array in the binary file
    fseek(binary_output, offsets_pos, SEEK_SET);
    fwrite(offsets, sizeof(size_t), paths->fileCount, binary_output);

    // We close the binary file
    fclose(binary_output);
    // Detach and remove shared memory segment
    munmap(offsets, SHM_SIZE);
    shm_unlink(SHM_NAME);

    // Free memory
    free(paths);
    
    return EXIT_SUCCESS;
}
