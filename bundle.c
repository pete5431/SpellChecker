#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "bundle.h"

// Bundle Functions.

// Creates Bundle struc and initializes mutexes.
Bundle* create_bundle(SocketQueue* sockets, LogQueue* logs, char** dictionary){

	Bundle* bundle = (Bundle*) malloc(sizeof(Bundle));

	if(bundle == NULL){
		printf("Bunde allocation failed.\n");
		return NULL;
	}

	bundle->client_buffer = sockets;
	bundle->log_buffer = logs;
	bundle->dictionary = dictionary;

	pthread_mutex_init(&(bundle->client_lock), NULL);
	pthread_mutex_init(&(bundle->log_lock), NULL);

	pthread_cond_init(&(bundle->client_full), NULL);
	pthread_cond_init(&(bundle->client_empty), NULL);
	
	pthread_cond_init(&(bundle->log_full), NULL);
	pthread_cond_init(&(bundle->log_empty), NULL);

	return bundle;
}


// Frees the bundle.
void free_bundle(Bundle* bundle){

	pthread_mutex_destroy(&(bundle->client_lock));
	pthread_mutex_destroy(&(bundle->log_lock));
	pthread_cond_destroy(&(bundle->client_full));
	pthread_cond_destroy(&(bundle->client_empty));
	pthread_cond_destroy(&(bundle->log_full));
	pthread_cond_destroy(&(bundle->log_empty));
	free(bundle);
}

// SocketQueue Functions.

SocketQueue* create_queue_socket(int size){

	// Allocate memory for struct.
	SocketQueue* queue = (SocketQueue*) malloc(sizeof(SocketQueue));

	if(queue == NULL){
		printf("SocketQueue allocation failed.\n");
		return NULL;
	}

	// Max size of queue will be the passed size.
	queue->max_size = size;
	// Points to front of queue, initial value is zero.
	queue->front = 0;
	// Current number of items in queue.
	queue->current_size = 0;
	// Points to end of queue, initial is size - 1 (the last index).
	queue->end = size - 1;
	// Allocate memory for array of ints.
	queue->array = (int*) malloc(size * sizeof(int));

	if(queue->array == NULL){
		printf("SocketQueue array allocation failed.\n");
		return NULL;
	}

	return queue;
}

// If the current size is 0, the queue is empty.
int is_empty_socket(SocketQueue* queue){
	return (queue->current_size == 0);
}

// If the current size is equal to max size, the queue is full.
int is_full_socket(SocketQueue* queue){
	return (queue->current_size == queue->max_size);
}

void enqueue_socket(SocketQueue* queue, int item){	

	if(is_full_socket(queue)){
		return;
	}
	// Increment queue (end is initially max_size - 1). Eventually loops around upon reaching max size.
	queue->end = (queue->end + 1) % (queue->max_size);
	// Add item to end of queue.
	queue->array[queue->end] = item;
	// Increment item count.
	queue->current_size = queue->current_size + 1;

}
int dequeue_socket(SocketQueue* queue){

	if(is_empty_socket(queue)){
		return -1;
	}
	// Get the item from the front of the queue.
	int item = queue->array[queue->front];
	// Increment the front. Eventually loops back around.
	queue->front = (queue->front + 1) % queue->max_size;
	// Decrement item count.
	queue->current_size = queue->current_size - 1;
	return item;
}

// Free the queue.
void free_queue_socket(SocketQueue* queue){
	
	free(queue->array);
	free(queue);
}

// LogQueue Functions.

// Same functions as socket queue just different names, and allocating for string array.
LogQueue* create_queue_log(int size){

	LogQueue* queue = (LogQueue*) malloc(sizeof(LogQueue));

	if(queue == NULL){
		printf("LogQueue allocation failed.\n");
		return NULL;
	}

        queue->max_size = size;
        queue->front = 0;
        queue->current_size = 0;
        queue->end = size - 1;
        queue->array = (char**) malloc(size * sizeof(char*));

	if(queue->array == NULL){
		printf("LogQueue array allocation failed.\n");
		return NULL;
	}

        return queue;
}

int is_empty_log(LogQueue* queue){
        return (queue->current_size == 0);
}

int is_full_log(LogQueue* queue){
        return (queue->current_size == queue->max_size);
}

void enqueue_log(LogQueue* queue, char* item){
        if(is_full_log(queue)){
                return;
        }
        queue->end = (queue->end + 1) % (queue->max_size);
        queue->array[queue->end] = item;
        queue->current_size = queue->current_size + 1;
}

char* dequeue_log(LogQueue* queue){

        if(is_empty_log(queue)){
                return NULL;
        }

        char* item = queue->array[queue->front];
        queue->front = (queue->front + 1) % queue->max_size;
        queue->current_size = queue->current_size - 1;
        return item;
}

void print_queue_log(LogQueue* queue){

        for(int i = queue->front; i < queue->end + 1; i++){
                printf("%s\n", (queue->array[i]));
        }
}

void free_queue_log(LogQueue* queue){
	
	free(queue->array);
	free(queue);
}
