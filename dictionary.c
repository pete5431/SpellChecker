#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"

// Makes the dictionary and returns the array.
char** make_dictionary(char* dictionary_filename){

        FILE* fp = fopen(dictionary_filename, "r");

        if(fp == NULL){
                printf("File open failed for dictionary.\n");
                exit(1);
        }

        size_t size = 0;
        char* read_word = NULL;
        int dictionary_size = 0;

	// First get how many lines are to be read. Words must be separated by line.
        while(getline(&read_word, &size, fp) != -1){
                dictionary_size++;
        }

	// Reset the file pointer.
        fseek(fp, 0, SEEK_SET);

        char** dictionary = (char**)malloc((dictionary_size + 1) * sizeof(char*));

	if(dictionary == NULL){
		printf("Memory allocation for dictionary failed\n");
		return NULL;
	}

        int i = 0;

	// Read each word into the dictionary.
        while(getline(&read_word, &size, fp) != -1){

                dictionary[i] = (char*) malloc((strlen(read_word) + 1) * sizeof(char));

		if(dictionary[i] == NULL){
			printf("Memory allocation for word failed\n");
			return NULL;
		}

                strncpy(dictionary[i], read_word, strlen(read_word) + 1);

                i++;
        }

	// Set final index to NULL.
        dictionary[i] = NULL;

	// Free the buffer used for getline.
        free(read_word);

        return dictionary;
}

// Frees the dictionary.
void free_dictionary(char*** dictionary){

        int i = 0;

        while((*dictionary)[i] != NULL){
                free((*dictionary)[i]);
                i++;
        }
        free(*dictionary);
}

// Loops through dictionary to find a word.
int find_word(char** dictionary, char* word){

	if(dictionary == NULL){
		printf("Empty dictionary.\n");
		return 0;
	}

	int i = 0;

	while(dictionary[i] != NULL){

		if(strcmp(word, dictionary[i]) == 0){
			return 1;
		}
		i++;
	}
	return 0;
}
                                       
