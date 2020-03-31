#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "bundle.h"

// Bundle Functions.

Bundle* create_bundle(SocketQueue* sockets, LogQueue* logs, char** dictionary){

	Bundle* bundle = (Bundle*) malloc(sizeof(Bundle));

	bundle->client_buffer = sockets;
	bundle->log_buffer = logs;
	bundle->dictionary = dictionary;

	return bundle;
}


// SocketQueue Functions.

SocketQueue* create_queue_socket(int size){

	SocketQueue* queue = (SocketQueue*) malloc(sizeof(SocketQueue));

	queue->max_size = size;
	queue->front = 0;
	queue->current_size = 0;
	queue->end = size - 1;
	pthread_cond_init(&(queue->socket_full), NULL);
	queue->array = (int*) malloc(size * sizeof(int));
	return queue;
}

int is_empty_socket(SocketQueue* queue){
	return (queue->current_size == 0);
}

int is_full_socket(SocketQueue* queue){
	return (queue->current_size == queue->max_size);
}

void enqueue_socket(SocketQueue* queue, int item){	

	if(is_full_socket(queue)){
		return;
	}
	queue->end = (queue->end + 1) % (queue->max_size);
	queue->array[queue->end] = item;
	queue->current_size = queue->current_size + 1;

}
int dequeue_socket(SocketQueue* queue){

	if(is_empty_socket(queue)){
		return -1;
	}

	int item = queue->array[queue->front];
	queue->front = (queue->front + 1) % queue->max_size;
	queue->current_size = queue->current_size - 1;
	return item;
}

void print_queue_socket(SocketQueue* queue){

	for(int i = queue->front; i < queue->end + 1; i++){
	
		printf("%d\n", (queue->array[i]));
	}
}

void free_queue_socket(SocketQueue* queue){
	
	free(queue->array);
	free(queue);
}

// LogQueue Functions.

LogQueue* create_queue_log(int size){

	LogQueue* queue = (LogQueue*) malloc(sizeof(LogQueue));

        queue->max_size = size;
        queue->front = 0;
        queue->current_size = 0;
        queue->end = size - 1;
        queue->array = (char**) malloc(size * sizeof(char*));
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
