#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "dictionary.h"
#include "bundle.h"
#include "worker.h"

#define DEFAULT_PORT 2222
#define CAPACITY 10
#define NUM_WORKERS 2

#define MAX_LIN 64

const char* DEFAULT_DICTIONARY = "dictionary.txt";

Bundle* bundle;

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

		dictionary_filename = (char*)calloc(strlen(argv[1]) + 1, sizeof(char));

		if(dictionary_filename == NULL){
			printf("Memory Allocation Error.\n");
			exit(1);
		}

		strncpy(dictionary_filename, argv[1], strlen(argv[1]) + 1);

		port_number = atoi(argv[2]);
	}
	else if(argc == 2){

		// If only dictionary file is provided.

		dictionary_filename = (char*) calloc(strlen(argv[1]) + 1, sizeof(char));

                strncpy(dictionary_filename, argv[1], strlen(argv[1]) + 1);

		port_number = DEFAULT_PORT;
	}
	else if(argc == 1){

		// If none are provided, use default.

		dictionary_filename = (char*) calloc(strlen(DEFAULT_DICTIONARY) + 1, sizeof(char));

		strncpy(dictionary_filename, DEFAULT_DICTIONARY, strlen(DEFAULT_DICTIONARY) + 1 * sizeof(char));

		port_number = DEFAULT_PORT;
	}

	pthread_t workers[NUM_WORKERS];

	SocketQueue* sockets = create_queue_socket(CAPACITY);
	LogQueue* logs = create_queue_log(CAPACITY);
	dictionary = make_dictionary(dictionary_filename);

	bundle = create_bundle(sockets, logs, dictionary);

	enqueue_socket(bundle->client_buffer, 10);

	for(int i = 0; i < NUM_WORKERS; i++){
		pthread_create(&(workers[i]), NULL, &process_sockets, NULL);
	}

	for(int i = 0; i < NUM_WORKERS; i++){
		pthread_join(workers[i], NULL);
	}

	printf("Dictionary filename: %s\n", dictionary_filename);
	printf("Port Number: %d\n", port_number);

	free(dictionary_filename);

	free_dictionary(&dictionary);

	return 0;
}

