#ifndef FIXED_QUEUE_H
#define FIXED_QUEUE_H

typedef struct{

	int max_size;
	int front;
	int end;
	int current_size;
	void* array;
} Queue;

int is_empty(Queue*);
int is_full(Queue*);
void enqueue(Queue*, void*);
void* dequeue(Queue*);

#endif
