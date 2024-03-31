#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "huffman.h"

/**
 * @brief Creates a new Min Heap node with the specified data and frequency.
 * 
 * Creates a new Min Heap node and initializes its data and frequency fields.
 * 
 * @param data The character data to be stored in the node.
 * @param freq The frequency associated with the character.
 * 
 * @return Pointer to the newly created Min Heap node.
 * 
 * @details This function allocates memory for a new Min Heap node and sets its
 * left and right child pointers to NULL. It assigns the provided data and frequency
 * values to the corresponding fields of the node and returns a pointer to it.
 */
struct MinHeapNode* newNode(wchar_t data, unsigned freq) {
    // Allocate memory for the new Min Heap node
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    // Initialize left and right child pointers to NULL
    temp->left = temp->right = NULL;
    // Set the data and frequency fields of the node
    temp->data = data;
    temp->freq = freq;
    // Return a pointer to the newly created Min Heap node
    return temp;
}

/**
 * @brief Creates a Min Heap with the specified capacity.
 * 
 * Creates a Min Heap structure with the given capacity.
 * 
 * @param capacity The maximum number of nodes the Min Heap can hold.
 * 
 * @return Pointer to the newly created Min Heap.
 * 
 * @details This function allocates memory for the Min Heap structure and its array
 * of node pointers. It initializes the size of the Min Heap to 0 and sets the capacity
 * to the provided value.
 */
struct MinHeap* createMinHeap(unsigned capacity) {
    // Allocate memory for the Min Heap structure
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    // Initialize the Min Heap size to 0
    minHeap->size = 0;
    // Set the capacity of the Min Heap
    minHeap->capacity = capacity;
    // Allocate memory for the array of node pointers
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    // Return a pointer to the newly created Min Heap
    return minHeap;
}

/**
 * @brief Swaps two nodes in the Min Heap.
 * 
 * Swaps the positions of two nodes in the Min Heap.
 * 
 * @param a Pointer to the first node.
 * @param b Pointer to the second node.
 * 
 * @details This function swaps the positions of the nodes pointed to by 'a' and 'b'.
 * It is commonly used in Min Heap operations to adjust the heap structure.
 */
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    // Temporary variable to store the first node
    struct MinHeapNode* t = *a;
    // Swap the nodes
    *a = *b;
    *b = t;
}

/**
 * @brief Heapifies the Min Heap at a given index.
 * 
 * Adjusts the Min Heap structure starting from the specified index to maintain the Min Heap property.
 * 
 * @param minHeap Pointer to the Min Heap structure.
 * @param idx Index of the node to start heapifying.
 * 
 * @details This function compares the frequency of the node at the given index with its left and right child nodes.
 * If either child node has a smaller frequency, it swaps positions with the smallest child node. The process continues
 * recursively until the Min Heap property is restored.
 */
void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx; // Parent node
    int left = 2 * idx + 1; // Left child node
    int right = 2 * idx + 2; // Right child node

    // Check if the left child node has a smaller frequency than its parent node
    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq) {
        // If so, update the index of the smallest node to the left child node
        smallest = left;
    }

    // Check if the right child node has a smaller frequency than its parent node
    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq) {
        // If so, update the index of the smallest node to the right child node
        smallest = right;
    }

    // If the index of the smallest node is different from the current index, swap nodes and heapify recursively
    if (smallest != idx) {
        // Swap the positions of the smallest node and the current node
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]); 
        // Recursively heapify the subtree rooted at the smallest node
        minHeapify(minHeap, smallest);
    }
}

/**
 * @brief Checks if the size of the Min Heap is 1.
 * 
 * Determines whether the size of the Min Heap is equal to 1.
 * 
 * @param minHeap Pointer to the Min Heap structure.
 * 
 * @return 1 if the size of the Min Heap is 1, otherwise 0.
 * 
 * @details This function returns 1 if the size of the Min Heap is exactly 1,
 * indicating that there is only one node in the heap. Otherwise, it returns 0,
 * indicating that the heap contains more than one node.
 */
int isSizeOne(struct MinHeap* minHeap) {
    // Check if the size of the Min Heap is equal to 1
    return (minHeap->size == 1);
}

/**
 * @brief Extracts the minimum value node from the Min Heap.
 * 
 * This function removes and returns the minimum value node from the Min Heap,
 * ensuring that the Min Heap property is maintained.
 * 
 * @param minHeap Pointer to the Min Heap structure.
 * 
 * @return Pointer to the extracted minimum value node.
 * 
 * @details The function first extracts the root node of the Min Heap, which contains
 * the minimum value. It replaces the root node with the last node in the heap and 
 * decrements the heap size. Then, it calls the minHeapify function to restore the 
 * Min Heap property by adjusting the heap structure.
 */
struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    // Store the root node (minimum value node) to be extracted
    struct MinHeapNode* temp = minHeap->array[0];
    // Replace the root node with the last node in the heap
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    // Decrease the size of the heap
    --minHeap->size;
    // Restore the Min Heap property after extraction
    minHeapify(minHeap, 0);
    // Return the extracted minimum value node
    return temp;
}

/**
 * @brief Inserts a new node into the Min Heap.
 * 
 * Inserts a new node into the Min Heap while maintaining the Min Heap property.
 * 
 * @param minHeap Pointer to the Min Heap structure.
 * @param minHeapNode Pointer to the new node to be inserted.
 * 
 * @details This function inserts a new node into the Min Heap pointed to by 'minHeap'.
 * It ensures that the Min Heap property is maintained, where the frequency of each 
 * parent node is less than or equal to the frequencies of its children. The new node
 * is inserted at the appropriate position in the heap to maintain this property.
 */
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    ++minHeap->size; // Increment the min heap size
    // Index to the last position of the heap array
    int i = minHeap->size - 1;
    // Compares the frequency of the new node with its parent node's frequency
    // with (i - 1) / 2 we get the parent for i-th
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        // If the frequency of the new node is less than its parent's, it swaps 
        // the new node with its parent
        minHeap->array[i] = minHeap->array[(i - 1) / 2]; 
        i = (i - 1) / 2; // Set i to be the position of i's parent node 
    }
    // Once either the new node becomes the root or its frequency is not lees than its parent's
    // We insert the new node to the i position result
    minHeap->array[i] = minHeapNode;
}

/**
 * @brief Constructs a Huffman Tree from a frequency table.
 * 
 * Constructs a Huffman Tree from the provided frequency table.
 * Each index represents a character, and its value represents the frequency.
 * 
 * @param freq_table The frequency table containing the frequencies of characters.
 * @param size The size of the frequency table.
 * 
 * @return A pointer to the root of the constructed Huffman Tree.
 * 
 * @details The Huffman Tree is constructed using a MinHeap data structure. First,
 * a MinHeap is created with a size equal to the size of the provided frequency
 * table. Then, each non-zero frequency in the frequency table corresponds to a
 * node in the MinHeap. Nodes are inserted into the MinHeap based on their
 * frequency, with lower frequency nodes having higher priority.
 * 
 * Next, the function merges nodes from the MinHeap to form the Huffman Tree.
 * This process continues until there is only one node remaining in the MinHeap,
 * which becomes the root of the Huffman Tree.
 * 
 * After the Huffman Tree is constructed, the function extracts the final minimum
 * node from the MinHeap, which serves as the root of the Huffman Tree.
 */
struct MinHeapNode* buildHuffmanTree(int freq_table[], int size) {
    // Create variables to store left and right nodes, and the top node of the heap
    struct MinHeapNode *left, *right, *top;
    // Create a MinHeap with the specified size
    struct MinHeap* minHeap = createMinHeap(size);
    // Populate the minHeap with nodes based on the provided frequency table
    for (int i = 0; i < MAX_FREQ_TABLE_SIZE; i++) {
        // Only the characters with a frequency > 0 are characters from the original buffer
        if (freq_table[i] > 0) {
            // Create and insert the new node into the min heap
            // i in this context is the wchar character to be casted
            // and freq_table[i] its frequency
            insertMinHeap(minHeap, newNode((wchar_t)i, freq_table[i]));
        }
    }

    // Construct the Huffman Tree by merging nodes until there is only one node left
    while (!isSizeOne(minHeap)) {
        // Extract the two minimum nodes from the min heap
        // With each extract, the min heap is minheapify
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        // Create the new node that represents the sum of both minimum nodes
        top = newNode(L'$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        // Insert the new node into the min heap
        insertMinHeap(minHeap, top);
    }

    // Extract the final minimum node before freeing the MinHeap
    struct MinHeapNode* huffmanTreeRoot = extractMin(minHeap);

    // Free the memory allocated for the minHeap
    free(minHeap->array);
    free(minHeap);

    return huffmanTreeRoot;
}

// Function to print Huffman codes from the tree
//TODO: This function can be modified so it returns the binary code for a specific character
void printCodes(struct MinHeapNode* root, int arr[], int top) {
    if (root->left) {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1);
    }

    if (root->right) {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }

    if (!(root->left) && !(root->right)) {
        wprintf(L"%lc: ", root->data);
        for (int i = 0; i < top; ++i)
            printf("%d", arr[i]);
        printf("\n");
    }
}

/**
 * @brief Generates Huffman codes from a Huffman tree.
 * 
 * Generates Huffman codes using the provided Huffman tree,
 * where each character's code is represented by a sequence
 * of bits stored in the huffmanCodes array.
 * 
 * @param node The current node in the Huffman tree.
 * @param bits An array to store the bits of the Huffman code.
 * @param idx The index indicating the current position in the bits array.
 * @param huffmanCodes The array to store the generated Huffman codes.
 * 
 * @details The function traverses the Huffman tree recursively,
 * assigning '0' to the left child nodes and '1' to the right
 * child nodes. When a leaf node is encountered, the function
 * saves the Huffman code for that character in the huffmanCodes
 * array, where the first element represents the code length
 * and subsequent elements store the bits of the code.
 */
void generateHuffmanCodes(struct MinHeapNode* node, int bits[], int idx, struct HuffmanCode* huffmanCodes[]) {
    // Base case
    if (node == NULL) {
        return;
    }
    // Check if the current node has left or child nodes
    // If so, we assign 0 to left and 1 to right and make a recursive call to this function again
    if (node->left) {
        bits[idx] = 0;
        generateHuffmanCodes(node->left, bits, idx + 1, huffmanCodes);
    }

    if (node->right) {
        bits[idx] = 1;
        generateHuffmanCodes(node->right, bits, idx + 1, huffmanCodes);
    }

    // If the current node is a leaf node, assign the Huffman code for that character
    if (!(node->left) && !(node->right)) {
        // We initialize the struct that will contain the Huffman code
        huffmanCodes[node->data] = (struct HuffmanCode*)malloc(sizeof(struct HuffmanCode));
        if (huffmanCodes[node->data] == NULL) {
            perror("Memory allocation failed. Couldn't reserve memory for the HuffmanCode struct");
            exit(EXIT_FAILURE);
        }
        // Set the length of the Huffman code
        huffmanCodes[node->data]->length = idx;
        for (int i = 0; i < idx; ++i) {
            // Store the bits of the code in the HuffmanCode struct's code array
            huffmanCodes[node->data]->code[i] = bits[i];
        }
    }
}

/**
 * @brief Calculates the number of characters with non-zero frequency in the frequency table.
 * 
 * This function calculates the number of characters with non-zero frequency in the provided frequency table.
 * 
 * @param freq_table Array representing the frequency of characters.
 * 
 * @return The number of characters with non-zero frequency.
 * 
 * @details Iterates through the frequency table and counts the characters with non-zero frequency,
 * incrementing the size variable for each non-zero frequency encountered.
 */
int calculateFreqTableSize(int freq_table[]) {
    int size = 0; // Initialize the size counter
    // Iterate through the frequency table
    for (int i = 0; i < MAX_FREQ_TABLE_SIZE; i++) {
        // Check if the frequency of the character at index i is greater than 0
        if (freq_table[i] > 0) {
            // Increment the size counter
            size++;
        }
    }
    // Return the total number of characters with non-zero frequency
    return size;
}

void free_huffman_tree(struct MinHeapNode* root) {
    if (root == NULL) {
        return;
    }

    // Recursively free the left and right subtrees
    free_huffman_tree(root->left);
    free_huffman_tree(root->right);

    // Free the memory allocated for the current node
    free(root);
}

/* // Driver program to test above functions
int main() {
    // Testing buffer
    wchar_t *buffer = L"abcdefj"; 
    int freq[] = { 5, 9, 120, 13, 16, 45, 98 };
    int freq_table[MAX_FREQ_TABLE_SIZE] = {0};

    for (int i = 0; buffer[i] != L'\0'; i++) {
        wprintf(L"Value at position %d: %lc\n", i, buffer[i]);
        freq_table[buffer[i]] = freq[i];
        wprintf(L"Freq of character %lc is %d\n", buffer[i], freq_table[buffer[i]]);
    }

    wchar_t data[] = { L'a', L'b', L'c', L'd', L'e', L'f', L'j'};
    
    int size = calculateFreqTableSize(freq_table);
    printf("size is %d\n", size);
    printf("Huffman codes:\n");
    generateHuffmanCodes(freq_table, size);
    return 0;
} */