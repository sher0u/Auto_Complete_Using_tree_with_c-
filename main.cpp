#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 26
#define MAX_COMPLETIONS 5  // Number of completions to return

// Trie Node structure
typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];  // Child nodes
    int frequency;  // Frequency of the word ending at this node
    int isEndOfWord;  // True if the node is the end of a word
} TrieNode;

// Min-Heap node structure for Priority Queue
typedef struct HeapNode {
    char* word;  // Word
    int frequency;  // Frequency of the word
} HeapNode;

// Min-Heap structure
typedef struct MinHeap {
    HeapNode** array;
    int size;
    int capacity;
} MinHeap;

// Function prototypes
TrieNode* createTrieNode();
void insert(TrieNode* root, const char* word, int frequency);
TrieNode* search(TrieNode* root, const char* prefix);
MinHeap* createMinHeap(int capacity);
void insertMinHeap(MinHeap* heap, HeapNode* node);
HeapNode* extractMin(MinHeap* heap);
void getTopCompletions(TrieNode* root, const char* prefix, MinHeap* heap);
void freeTrie(TrieNode* root);
void freeMinHeap(MinHeap* heap);
void printTopCompletions(MinHeap* heap);
void loadDictionary(TrieNode* root, const char* filename);

int main() {
    TrieNode* root = createTrieNode();

    // Load words from dictionary file into the Trie
    loadDictionary(root, "words.txt");  // Full path

    char prefix[100];
    printf("Enter prefix for autocomplete: ");
    scanf("%s", prefix);

    MinHeap* heap = createMinHeap(MAX_COMPLETIONS);
    getTopCompletions(root, prefix, heap);

    // Print top completions
    printTopCompletions(heap);

    // Clean up
    freeTrie(root);
    freeMinHeap(heap);

    return 0;
}

// Trie Node creation
TrieNode* createTrieNode() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    node->frequency = 0;
    node->isEndOfWord = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        node->children[i] = NULL;
    }
    return node;
}

// Insert a word into the Trie
void insert(TrieNode* root, const char* word, int frequency) {
    TrieNode* node = root;
    for (int i = 0; i < strlen(word); i++) {
        int index = word[i] - 'a';
        if (node->children[index] == NULL) {
            node->children[index] = createTrieNode();
        }
        node = node->children[index];
    }
    node->isEndOfWord = 1;
    node->frequency += frequency;  // Increment frequency for duplicate words
}

// Search for a prefix in the Trie
TrieNode* search(TrieNode* root, const char* prefix) {
    TrieNode* node = root;
    for (int i = 0; i < strlen(prefix); i++) {
        int index = prefix[i] - 'a';
        if (node->children[index] == NULL) {
            return NULL;
        }
        node = node->children[index];
    }
    return node;
}

// Min-Heap creation
MinHeap* createMinHeap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->size = 0;
    heap->capacity = capacity;
    heap->array = (HeapNode**)malloc(sizeof(HeapNode*) * capacity);
    return heap;
}

// Insert a word into the Min-Heap
void insertMinHeap(MinHeap* heap, HeapNode* node) {
    if (heap->size < heap->capacity) {
        heap->array[heap->size++] = node;
        int i = heap->size - 1;
        while (i > 0 && heap->array[(i - 1) / 2]->frequency > heap->array[i]->frequency) {
            HeapNode* temp = heap->array[i];
            heap->array[i] = heap->array[(i - 1) / 2];
            heap->array[(i - 1) / 2] = temp;
            i = (i - 1) / 2;
        }
    } else if (node->frequency > heap->array[0]->frequency) {
        heap->array[0] = node;
        int i = 0;
        while (2 * i + 1 < heap->size) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int smallest = i;

            if (left < heap->size && heap->array[left]->frequency < heap->array[smallest]->frequency) {
                smallest = left;
            }
            if (right < heap->size && heap->array[right]->frequency < heap->array[smallest]->frequency) {
                smallest = right;
            }

            if (smallest != i) {
                HeapNode* temp = heap->array[i];
                heap->array[i] = heap->array[smallest];
                heap->array[smallest] = temp;
                i = smallest;
            } else {
                break;
            }
        }
    }
}

// Extract the min element from Min-Heap
HeapNode* extractMin(MinHeap* heap) {
    if (heap->size == 0) {
        return NULL;
    }
    HeapNode* root = heap->array[0];
    heap->array[0] = heap->array[--heap->size];
    // Heapify the array
    for (int i = (heap->size - 1) / 2; i >= 0; i--) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;
        if (left < heap->size && heap->array[left]->frequency < heap->array[smallest]->frequency) {
            smallest = left;
        }
        if (right < heap->size && heap->array[right]->frequency < heap->array[smallest]->frequency) {
            smallest = right;
        }
        if (smallest != i) {
            HeapNode* temp = heap->array[i];
            heap->array[i] = heap->array[smallest];
            heap->array[smallest] = temp;
        }
    }
    return root;
}

// Find top completions for a prefix using Trie and Min-Heap
void getTopCompletions(TrieNode* root, const char* prefix, MinHeap* heap) {
    TrieNode* node = search(root, prefix);
    if (node == NULL) return;

    // Traverse the Trie and insert words into the Min-Heap
    // Function to recursively find completions
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i] != NULL) {
            char childPrefix[100];
            snprintf(childPrefix, sizeof(childPrefix), "%s%c", prefix, 'a' + i);
            if (node->children[i]->isEndOfWord) {
                HeapNode* newNode = (HeapNode*)malloc(sizeof(HeapNode));
                newNode->word = strdup(childPrefix);
                newNode->frequency = node->children[i]->frequency;
                insertMinHeap(heap, newNode);
            }
            getTopCompletions(node->children[i], childPrefix, heap);
        }
    }
}

// Print the top completions
void printTopCompletions(MinHeap* heap) {
    printf("Top completions:\n");
    for (int i = 0; i < heap->size; i++) {
        printf("%s (Frequency: %d)\n", heap->array[i]->word, heap->array[i]->frequency);
    }
}

// Free the Trie memory
void freeTrie(TrieNode* root) {
    if (root == NULL) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        freeTrie(root->children[i]);
    }
    free(root);
}

// Free the Min-Heap memory
void freeMinHeap(MinHeap* heap) {
    if (heap == NULL) return;
    for (int i = 0; i < heap->size; i++) {
        free(heap->array[i]->word);
        free(heap->array[i]);
    }
    free(heap->array);
    free(heap);
}

// Load the dictionary from a file and insert the words into the Trie
void loadDictionary(TrieNode* root, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    char word[100];
    while (fscanf(file, "%s", word) != EOF) {
        insert(root, word, 1);  // Assuming frequency is 1 for now
    }

    fclose(file);
}
