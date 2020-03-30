#include <stdio.h>
#include <stdlib.h>
#include "fixed_queue.h"

typedef struct{

	int max_size;

	int front, end;

	int current_size;

	void** array;
} Queue;

Queue* create_queue(int size){

	Queue* queue = (Queue*) malloc(sizeof(Queue));

	queue->max_size = size;
	queue->front = 0;
	queue->current_size = 0;
	queue->end = size - 1;
	queue->array = (void**) malloc(size * sizeof(void*));
	return queue;
}

int is_empty(Queue* queue){
	return (queue->current_size == 0);
}

int is_full(Queue* queue){
	return (queue->current_size == queue->max_size);
}

void enqueue(Queue* queue, void* item){
	if(is_full(queue)){
		return;
	}
	queue->end = (queue->end + 1) % (queue->max_size);
	queue->array[queue->end] = item;
	queue->current_size = queue->current_size + 1;
}

void* dequeue(Queue* queue){

	if(is_empty(queue)){
		return NULL;
	}

	void* item = queue->array[queue->front];
	queue->front = (queue->front + 1) % queue->max_size;
	queue->current_size = queue->current_size - 1;
	return item;	
}

void print_queue(Queue* queue){

	for(int i = 0; i < queue->current_size; i++){

		printf("%d\n", *((int*) queue->array[i]));

	}
}

void free_queue(Queue* queue){
	
	free(queue->array);
	free(queue);
}

int main(){

	Queue* queue = create_queue(5);

	int a = 10;

	void* add  = &a;

	enqueue(queue, add);

	print_queue(queue);

	free_queue(queue);

	return 0;
}
