#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"

char** make_dictionary(char* dictionary_filename){

        FILE* fp = fopen(dictionary_filename, "r");

        if(fp == NULL){
                printf("File open failed for dictionary.\n");
                exit(1);
        }

        size_t size = 0;
        char* read_word = NULL;
        int dictionary_size = 0;

        while(getline(&read_word, &size, fp) != -1){
                dictionary_size++;
        }

        fseek(fp, 0, SEEK_SET);

        char** dictionary = (char**)malloc((dictionary_size + 1) * sizeof(char*));

        int i = 0;

        while(getline(&read_word, &size, fp) != -1){

                dictionary[i] = (char*) malloc((strlen(read_word) + 1) * sizeof(char));

                strncpy(dictionary[i], read_word, strlen(read_word) + 1);

                i++;
        }

        dictionary[i] = NULL;

        free(read_word);

        return dictionary;
}

void free_dictionary(char*** dictionary){

        int i = 0;

        while((*dictionary)[i] != NULL){
                free((*dictionary)[i]);
                i++;
        }
        free(*dictionary);
}
                                       
