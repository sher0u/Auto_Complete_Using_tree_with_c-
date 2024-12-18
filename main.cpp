#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#define ALPHABET_SIZE 26

// Trie Node Structure
typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];  // Array for 26 letters (a-z)
    int frequency;                  // Frequency of the word
    int isEndOfWord;                // Flag if it's the end of a word
} TrieNode;

// Min-Heap Structure for storing word completions
typedef struct HeapNode {
    char word[100];  // Word stored in the heap
    int frequency;   // Frequency of the word
} HeapNode;

typedef struct MinHeap {
    HeapNode** array;  // Array of HeapNodes
    int size;           // Current size of heap
    int capacity;       // Capacity of heap
} MinHeap;

// Function Prototypes
TrieNode* createTrieNode();
HeapNode* createHeapNode(const char* word, int frequency);
MinHeap* createMinHeap(int capacity);
void swapHeapNodes(HeapNode** a, HeapNode** b);
void minHeapify(MinHeap* heap, int index);
void insertMinHeap(MinHeap* heap, HeapNode* node);
TrieNode* search(TrieNode* root, const char* prefix);
void collectCompletions(TrieNode* node, char* prefix, MinHeap* heap);
void insert(TrieNode* root, const char* word, int frequency);
void loadDictionary(TrieNode* root, const char* filename);

// Main function for testing
int main() {
    TrieNode* root = createTrieNode();
    loadDictionary(root, "words.txt"); // Load words from a file

    printf("\n=== Autocomplete System ===\n");

    char prefix[100];
    printf("Enter prefix for autocomplete: ");
    scanf("%s", prefix);

    TrieNode* node = search(root, prefix);
    if (node == nullptr) {
        printf("No words found with this prefix.\n");
    } else {
        MinHeap* heap = createMinHeap(10); // Store top 10 completions
        collectCompletions(node, prefix, heap);

        // Output top completions
        printf("Top completions:\n");
        for (int i = 0; i < heap->size; i++) {
            printf("%s \n", heap->array[i]->word);
        }
    }

    return 0;
}



// Function to create a new Trie Node
TrieNode* createTrieNode() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    if (node == NULL) {
        printf("Memory allocation failed for TrieNode.\n");
        exit(1);  // Exit if memory allocation fails
    }
    for (int i = 0; i < 26; i++) {
        node->children[i] = nullptr;
    }
    node->frequency = 0;
    node->isEndOfWord = 0;
    return node;
}


// Function to create a new MinHeap Node
HeapNode* createHeapNode(const char* word, int frequency) {
    HeapNode* node = (HeapNode*)malloc(sizeof(HeapNode));
    strcpy(node->word, word);
    node->frequency = frequency;
    return node;
}

// Function to create a new MinHeap
MinHeap* createMinHeap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->capacity = capacity;
    heap->size = 0;
    heap->array = (HeapNode**)malloc(capacity * sizeof(HeapNode*));
    return heap;
}

// Function to swap two heap nodes
void swapHeapNodes(HeapNode** a, HeapNode** b) {
    HeapNode* temp = *a;
    *a = *b;
    *b = temp;
}

// Min-Heapify function to maintain the min-heap property
void minHeapify(MinHeap* heap, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->size && heap->array[left]->frequency < heap->array[smallest]->frequency) {
        smallest = left;
    }
    if (right < heap->size && heap->array[right]->frequency < heap->array[smallest]->frequency) {
        smallest = right;
    }

    if (smallest != index) {
        swapHeapNodes(&heap->array[index], &heap->array[smallest]);
        minHeapify(heap, smallest);
    }
}

// Insert a node into the MinHeap
void insertMinHeap(MinHeap* heap, HeapNode* node) {
    if (heap->size < heap->capacity) {
        heap->size++;
        int index = heap->size - 1;
        heap->array[index] = node;

        while (index > 0 && heap->array[(index - 1) / 2]->frequency > heap->array[index]->frequency) {
            swapHeapNodes(&heap->array[(index - 1) / 2], &heap->array[index]);
            index = (index - 1) / 2;
        }
    } else if (heap->array[0]->frequency < node->frequency) {
        // Replace the root with the new node if the new node has higher frequency
        free(heap->array[0]);  // Free the old root
        heap->array[0] = node;
        minHeapify(heap, 0);    // Reheapify
    }
}

// Function to search for a prefix in the Trie
TrieNode* search(TrieNode* root, const char* prefix) {
    TrieNode* node = root;
    char lower_prefix[100];

    // Convert the prefix to lowercase
    for (int i = 0; prefix[i]; i++) {
        if (isalpha(prefix[i])) {
            lower_prefix[i] = tolower(prefix[i]);
        } else {
            lower_prefix[i] = prefix[i];  // Non-alphabetic characters stay as they are
        }
    }

    // Traverse the Trie based on the prefix
    for (int i = 0; lower_prefix[i] != '\0'; i++) {
        int index = lower_prefix[i] - 'a';  // Adjust indexing for lowercase letters
        if (node->children[index] == nullptr) {
            return nullptr; // If the prefix doesn't match, return NULL
        }
        node = node->children[index];
    }

    return node;
}

// Function to collect completions from a node in the Trie
void collectCompletions(TrieNode* node, char* prefix, MinHeap* heap) {
    if (node == nullptr || heap->size == heap->capacity) {
        return;
    }

    // If the node marks the end of a word, add it to the heap
    if (node->isEndOfWord) {
        HeapNode* newNode = createHeapNode(prefix, node->frequency);
        insertMinHeap(heap, newNode);
    }

    // Traverse the children of the node
    for (int i = 0; i < 26; i++) {
        if (node->children[i] != nullptr) {
            char newPrefix[100];
            size_t len = strlen(prefix);
            strcpy(newPrefix, prefix);
            newPrefix[len] = 'a' + i;
            newPrefix[len + 1] = '\0';
            collectCompletions(node->children[i], newPrefix, heap);
        }
    }
}

// Function to insert a word into the Trie
void insert(TrieNode* root, const char* word, int frequency) {
    TrieNode* node = root;
    char lower_word[100];

    // Convert word to lowercase
    for (int i = 0; word[i]; i++) {
        lower_word[i] = tolower(word[i]);
    }

    // Traverse the Trie for the word
    for (int i = 0; lower_word[i] != '\0'; i++) {
        int index = lower_word[i] - 'a';
        if (node->children[index] == nullptr) {
            node->children[index] = createTrieNode();
        }
        node = node->children[index];
    }

    // Mark the end of the word and set the frequency
    node->isEndOfWord = 1;
    node->frequency = frequency;
}

// Load the dictionary from a file and insert the words into the Trie
void loadDictionary(TrieNode* root, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file ==nullptr) {
        printf("Error opening file.\n");
        return;
    }

    char word[100];
    while (fscanf(file, "%s", word) != EOF) {
        insert(root, word, 1);
    }

    fclose(file);
}
