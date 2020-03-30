#ifndef FIXED_QUEUE_H_
#define FIXED_QUEUE_H_

typedef struct{

	int max_size;
	int front;
	int end;
	int current_size;
	void** array;

} Queue;

Queue* create_queue(int);
//int is_empty(Queue*);
//int is_full(Queue*);
//void enqueue(Queue*, void*);
//void* dequeue(Queue*);
//void free_queue(Queue*);

#endif
