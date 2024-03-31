#ifndef HUFFMAN_H
#define HUFFMAN_H
#define MAX_FREQ_TABLE_SIZE 65536
#define MAX_CODE_SIZE 256

// ******** Frequency functions ********

/**
 * @brief Function to extract character frequencies from a buffer of wide characters (wchar_t)
 * @param buffer: a pointer to the buffer containing the characters
 * @param freq_table: an array representing the frequency of each character
 * @return void
 * 
 * @details This function extracts character frequencies from the specified buffer of wide characters (wchar_t).
 * It iterates through the buffer, ignoring spaces, line breaks, and carriage returns, and increments the frequency
 * of each non-space/non-line-break character in the freq_table array.
 */
void char_frequencies(wchar_t *buffer, int freq_table[]);

// ******** Huffman Algorithm ********

/**
 * @brief Structure representing a node in a Huffman tree.
 * 
 * This struct defines a node in a Huffman tree, which is used for encoding
 * and decoding data efficiently.
 */
struct MinHeapNode {
    wchar_t data;                 /**< Data stored in the node. */
    unsigned freq;                /**< Frequency associated with the data. */
    struct MinHeapNode *left;     /**< Pointer to the left child node. */
    struct MinHeapNode *right;    /**< Pointer to the right child node. */
};

/**
 * @brief Structure representing a Min Heap.
 * 
 * This struct defines a Min Heap, which maintains the Min Heap property
 * for a collection of nodes.
 */
struct MinHeap {
    unsigned size;              /**< Current size of the Min Heap. */
    unsigned capacity;          /**< Maximum capacity of the Min Heap. */
    struct MinHeapNode **array; /**< Array of pointers to Min Heap nodes. */
};

struct HuffmanCode {
    int code[MAX_CODE_SIZE];
    int length;
};

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
struct MinHeapNode* newNode(wchar_t data, unsigned freq);

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
struct MinHeap* createMinHeap(unsigned capacity);

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
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b);

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
void minHeapify(struct MinHeap* minHeap, int idx);

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
int isSizeOne(struct MinHeap* minHeap);

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
struct MinHeapNode* extractMin(struct MinHeap* minHeap);

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
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode);

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
struct MinHeapNode* buildHuffmanTree(int freq_table[], int size);

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
void generateHuffmanCodes(struct MinHeapNode* node, int bits[], int idx, struct HuffmanCode* huffmanCodes[]);

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
int calculateFreqTableSize(int freq_table[]);

/**
 * 
*/
void free_huffman_tree(struct MinHeapNode* root);


#endif // !HUFFMAN_H