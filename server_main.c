#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "dictionary.h"
#include "bundle.h"
#include "worker.h"

#define DEFAULT_PORT "55522"
#define BACKLOG 10
#define CAPACITY 10
#define NUM_WORKERS 5
#define MAX_LINE 64

// The default dictionary file.
const char* DEFAULT_DICTIONARY = "dictionary.txt";

// Function to start the server.
void start_server(char*);
// Function to get the listening socket.
int get_listen_fd(char*);

// The global bundle.
Bundle* bundle;

int main(int argc, char** argv){

	char* dictionary_filename = NULL;
	char** dictionary = NULL;
	char* port_number = NULL;

	// If too many arguments, exit.
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

		port_number = argv[2];
	}
	else if(argc == 2){

		// If only dictionary file is provided.

		dictionary_filename = (char*) calloc(strlen(argv[1]) + 1, sizeof(char));

		if(dictionary_filename == NULL){
			printf("Memory Allocation Error.\n");
			exit(1);
		}

                strncpy(dictionary_filename, argv[1], strlen(argv[1]) + 1);

		port_number = DEFAULT_PORT;
	}
	else if(argc == 1){

		// If none are provided, use default.

		dictionary_filename = (char*) calloc(strlen(DEFAULT_DICTIONARY) + 1, sizeof(char));

		if(dictionary_filename == NULL){
			printf("Memory Allocation Error.\n");
			exit(1);
		}

		strncpy(dictionary_filename, DEFAULT_DICTIONARY, strlen(DEFAULT_DICTIONARY) + 1 * sizeof(char));

		port_number = DEFAULT_PORT;
	}
	
	// Array of threads that will be worker threads that work on the client buffer.
	pthread_t client_workers[NUM_WORKERS];
	// Single thread to work on the log buffer.
	pthread_t log_worker;

	// Create a new socket queue with the following CAPACITY.
	SocketQueue* sockets = create_queue_socket(CAPACITY);
	// Create a new log queue with the following CAPACITY.
	LogQueue* logs = create_queue_log(CAPACITY);
	// Make the dictionary with the file.
	dictionary = make_dictionary(dictionary_filename);

	// Create the global bundle with the data structures.
	bundle = create_bundle(sockets, logs, dictionary);
		
	// Create the threads.
	for(int i = 0; i < NUM_WORKERS; i++){
		pthread_create(&(client_workers[i]), NULL, &process_sockets, NULL);
	}
	pthread_create(&log_worker, NULL, &process_logs, NULL);
	
	// Start the server with the port number.
	start_server(port_number);

	// Free any allocated memory, not that it will get to here though.
	free(dictionary_filename);
	free_dictionary(&dictionary);
	free_queue_socket(sockets);
	free_queue_log(logs);
	free_bundle(bundle);

	return 0;
}

void start_server(char* port_number){

	// Listening socket descriptor.
	int listen_fd;
	// Connected socket descriptor.
	int connected_fd;

	// Struct that stores information about the client.
	struct sockaddr_storage client_addr;

	// Size of the client information.
	socklen_t client_addr_size;

	// Stores client name.
	char client_name[MAX_LINE];
	// Stores the client port.
	char client_port[MAX_LINE];

	// Set up the listening socket that will listen for incoming connections.
	listen_fd = get_listen_fd(port_number);

	// Continue accepting connections.
	while(1){

		// Subsequent connections will have to wait until it loops back around after close to get back to accepting connections.

		// Get the size of the client address.
		client_addr_size = sizeof(client_addr);

		// Try to accept a connection, and get the client information.
		if((connected_fd = accept(listen_fd, (struct sockaddr*) &client_addr, &client_addr_size)) == -1){
			printf("Accept connection error.\n");
			continue;
		}
		// Get the client name information to display who connected.
		if(getnameinfo((struct sockaddr*) &client_addr, client_addr_size, client_name, MAX_LINE, client_port, MAX_LINE, 0) != 0){
			printf("Error, couldn't get client name.\n");
		}else{
			printf("Connection accepted from %s:%s\n", client_name, client_port);
		}

		// Lock the client buffer.
		pthread_mutex_lock(&(bundle->client_lock));

		// If the client buffer is full, wait until it isn't. For now unlock.
		if(is_full_socket(bundle->client_buffer)){
			pthread_cond_wait(&(bundle->client_full), &(bundle->client_lock));
		}

		// Add the connected socket to the queue.
		enqueue_socket(bundle->client_buffer, connected_fd);		

		// Unlock the client buffer. Finished adding.
		pthread_mutex_unlock(&(bundle->client_lock));

		// Signal the client buffer is not empty, and therefore lock whoever was waiting.
		pthread_cond_signal(&(bundle->client_empty));
	}	
}

int get_listen_fd(char* port_number){

	int listen_fd;
	int status;

	struct addrinfo hints, *res, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;

	// Gets linked list of possible addresses.
	if((status == getaddrinfo(NULL, port_number, &hints, &res)) != 0){
		printf("getaddrinfo error.\n");
		exit(1);
	}
	
	// Look for a valid address.
	for(p = res; p != NULL; p = p->ai_next){
		if((listen_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0){
			continue;
		}
		// Bind socket to that valid address.
		if(bind(listen_fd, p->ai_addr, p->ai_addrlen) == 0){
			break;
		}
	}
	
	// Free the linked list.
	freeaddrinfo(res);

	if(p == NULL){
		printf("bind error.\n");
		exit(1);
	}

	// Set the listening socket for listening.
	if(listen(listen_fd, BACKLOG) < 0){
		printf("listen error.\n");
		close(listen_fd);
		exit(1);
	}
	return listen_fd;
}
