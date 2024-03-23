#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

// Structure for a Huffman tree node
struct MinHeapNode {
    wchar_t data;
    unsigned freq;
    struct MinHeapNode *left, *right;
};

// Structure for a Min Heap
struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct MinHeapNode **array;
};

// Function to create a new Min Heap node
struct MinHeapNode* newNode(wchar_t data, unsigned freq) {
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

// Function to create a Min Heap
struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

// Function to swap to Min Heap nodes
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// Function to heapify at a given index
void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx; // Parent node
    int left = 2 * idx + 1; // Left child node
    int right = 2 * idx + 2; // Right child node

    // Check of the left child node has a smaller frequency than its parent node
    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq) {
        // If so, the left child node is the smallest
        smallest = left;
    }

    // Check if the right child node has a smaller frequency than its parent node
    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq) {
        // If so, the right child node is the smallest
        smallest = right;
    }

    // If any of the above conditions was satisfied, we swap positions
    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]); 
        // We do it recursively
        minHeapify(minHeap, smallest);
    }
}

// Function to check if the size of the heap is 1 or not
int isSizeOne(struct MinHeap* minHeap) {
    return (minHeap->size == 1);
}

// Function to extract the minimum value node from heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

// Function to insert a new node to Min Heap
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

// Function to build the Huffman Tree
struct MinHeapNode* buildHuffmanTree(wchar_t data[], int freq[], int size) {
    struct MinHeapNode *left, *right, *top;
    struct MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        insertMinHeap(minHeap, newNode(data[i], freq[i]));

    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode(L'$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

// Function to print Huffman codes from the tree
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

// Function to build Huffman codes
void HuffmanCodes(wchar_t data[], int freq[], int size) {
    struct MinHeapNode* root = buildHuffmanTree(data, freq, size);
    int arr[100], top = 0;
    printCodes(root, arr, top);
}

// Driver program to test above functions
int main() {
    wchar_t data[] = { L'a', L'b', L'c', L'd', L'e', L'f', L'j'};
    int freq[] = { 5, 9, 120, 13, 16, 45, 98 };
    int size = sizeof(data) / sizeof(data[0]);
    printf("size is %d\n", size);
    printf("Huffman codes:\n");
    HuffmanCodes(data, freq, size);
    return 0;
}