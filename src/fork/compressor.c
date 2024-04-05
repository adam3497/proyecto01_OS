#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>


#include "../utilities/file_utils.c"
#include "../huffman/huffman.c"
#include "../huffman/freq.c"
#include "file_locks.c"

#define SHM_SIZE (MAX_TOTAL_BOOKS * sizeof(size_t))
#define SEM_1 "/sem_1"


void encode(char *input_file, char *freq_file, FILE *binary_output, size_t *offsets, int pos){

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

    // Write the Huffman Codes to file    
    size_t buffer_size = wcslen(buffer);
    write_encoded_bits_to_file(buffer, buffer_size, input_file, huffmanRoot, huffmanCodesArray, binary_output, offsets, pos);
}

int main() {

    int shmid;
    pid_t pid;
    sem_t *sem = NULL;
    
    size_t *offsets;
    
    // Use IPC_PRIVATE for simplicity (use a real key in production)
    key_t key = IPC_PRIVATE;
    
    // Create shared memory segment
    if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach shared memory segment to the process
    
    if ((offsets = shmat(shmid, NULL, 0)) == (size_t *) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Initialize the array in the parent process
    int numOfProcess = TOTAL_BOOKS;
    for (int i = 0; i < MAX_TOTAL_BOOKS; ++i) {
        offsets[i] = 0;
    }
    
    // Folder Paths
    const char* booksFolder = "books";
    const char* out = "out/bin/compressed.bin";

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
    
    sem = sem_open(SEM_1, O_CREAT | O_EXCL, 0644, 1);
     
    // Encode
    for (int i = 0; i < numOfProcess; i++) {
        pid = fork();

        // Código específico del proceso hijo
        if (pid == 0) {
            sem_wait(sem);
            
            // Escribir
            printf("[PID %d][CODING #%d] %s\n", getpid(), i+1, paths->books[i]);
            encode(paths->books[i], paths->freqs[i], binary_output, offsets, i+1);

            sem_post(sem);
            sem_close(sem);

            exit(EXIT_SUCCESS);
        } else if (pid < 0) {
            // Error al crear el proceso hijo
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }
    
    // El padre espera a todos los hijos
    for (int i = 0; i < numOfProcess; i++) {
        wait(NULL);
    }

    sem_close(sem);
    sem_unlink(SEM_1);

    fseek(binary_output, offsets_pos, SEEK_SET);
    fwrite(offsets, sizeof(size_t), paths->fileCount, binary_output);
    
    shmdt(offsets);
    shmctl(shmid, IPC_RMID, NULL);
    fclose(binary_output);
    free(paths);
    
    return EXIT_SUCCESS;
}
