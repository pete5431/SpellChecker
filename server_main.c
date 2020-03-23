#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_PORT 0.0

const char* DEFAULT_DICTIONARY = "dictionary.txt";

char** make_dictionary(char*);
void resize_dictionary(char***, int*);
void free_dictionary(char**);

int main(int argc, char** argv){

	char* dictionary_filename = NULL;
	char** dictionary = NULL;
	int port_number = 0;

	if(argc > 3){
		printf("Too many arguments to server.\n");
		exit(1);
	}
	else if(argc == 3){

		// If both dictionary file and port number is provided.

		dictionary_filename = malloc(strlen(argv[1]) + 1 * sizeof(char));

		if(dictionary_filename == NULL){
			printf("Memory Allocation Error.\n");
			exit(1);
		}

		strncpy(dictionary_filename, argv[1], strlen(argv[1]) + 1);

		port_number = atoi(argv[2]);
	}
	else if(argc == 2){

		// If only dictionary file is provided.

		dictionary_filename = malloc(strlen(argv[1]) + 1 * sizeof(char));

                strncpy(dictionary_filename, argv[1], strlen(argv[1]) + 1);

		port_number = DEFAULT_PORT;
	}
	else if(argc == 1){

		// If none are provided, use default.

		dictionary_filename = malloc(strlen(DEFAULT_DICTIONARY) + 1 * sizeof(char));

		strncpy(dictionary_filename, DEFAULT_DICTIONARY, strlen(DEFAULT_DICTIONARY) + 1 * sizeof(char));

		port_number = DEFAULT_PORT;
	}

	printf("Dictionary filename: %s\n", dictionary_filename);
	printf("Port Number: %d\n", port_number);

	dictionary = make_dictionary(dictionary_filename);

	free(dictionary_filename);

	free_dictionary(dictionary);

	return 0;
}

char** make_dictionary(char* dictionary_filename){

	int dictionary_size = 100;

	char** dictionary = (char**)calloc(dictionary_size, sizeof(char*));

	FILE* fp = fopen(dictionary_filename, "r");

	if(fp == NULL){
		printf("File open failed for dictionary.\n");
		exit(1);
	}
	
	size_t size = 0;
	char* read_word = NULL;
	int i = 0;

	while(getline(&read_word, &size, fp) != -1){

		printf("%s\n", read_word);

		if(i == dictionary_size - 2){
			resize_dictionary(&dictionary, &dictionary_size);
		}

		dictionary[i] = malloc(strlen(read_word) + 1 * sizeof(char));

		strncpy(dictionary[i], read_word, strlen(read_word) + 1);

		i++;
	}

	dictionary[i + 1] = NULL;

	free(read_word);

	return dictionary;
}

void resize_dictionary(char*** dictionary, int* dictionary_size){

	*dictionary_size = *dictionary_size * 2;

	char** new_dictionary = realloc(*dictionary, *dictionary_size * sizeof(char*));

	if(new_dictionary == NULL){
		printf("Reallocating dictionary failed.\n");
		exit(1);
	}
	else *dictionary = new_dictionary;
}

void free_dictionary(char** dictionary){

	int i = 0;

	while(dictionary[i] != NULL){
		free(dictionary[i]);
		i++;
	}
	free(dictionary);
}
