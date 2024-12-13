#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "autocompleter.h"
#include "utils.h"

int main() {
    // Define the absolute file path for the list
    char file[] = "C:\\Users\\kader\\Downloads\\Autocompleter-in-c-masterrrrrrrrr\\Autocompleter-in-c-master\\list.txt";
    char input[32768];
    struct Node* root = NULL;

    printf("Welcome to the autocomplete program.\n");
    printf("[INFO]: Loading database file %s containing list of phrases into memory ...\n", file);

    // Open and parse the file containing the list of words
    FILE *fp;
    char line[32767];

    if ((fp = fopen(file, "r")) == NULL) {
        printf("Exiting to system.\n");
        exit(EXIT_FAILURE);
    }

    while (fscanf(fp, "%s", line) != EOF) {
        if (strlen(line) > 32767) {
            printf("Skipping phrase longer than 32767 characters: %s\n", line);
            continue;
        }
        strtok(line, "\r\n");  // Clean up any newlines from the line
        insert(&root, line);  // Insert the word into the ternary search tree
    }

    fclose(fp);
    printf("[INFO]: Finished loading database file.\n");

    // Interact with the user to get autocomplete suggestions
    printf("Please type the prefix of a word (more than one character) and press ENTER:\n");
    while (1) {
        fgets(input, 32768, stdin);
        if (strlen(input) > 32768) {
            printf("Prefix size is limited to 32768 characters only. Please reduce accordingly. Exiting ..!\n");
            exit(EXIT_FAILURE);
        }

        if (strcmp(input, "quit\n") == 0) {
            printf("Exiting the program.\n");
            break;
        }

        strtok(input, "\r\n");  // Clean up any newline character from the input

        // Output suggestions based on the input prefix
        printf("\nSuggestions:\n");
        char buffer[32767] = "";
        strncpy(buffer, input, strlen(input));
        searchTST(root, input, "", buffer);

        printf("\nPlease type the prefix of a word (more than one character and no more than 32768 characters) :\n");
    }

    // Free allocated memory
    printf("Freeing memory ...\n");
    freeTST(root);
    printf("Freeing memory done.\n");

    return 0;
}
