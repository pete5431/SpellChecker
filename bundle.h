#ifndef SERVER_QUEUES_H_
#define SERVER_QUEUES_H_

#include <pthread.h>

typedef struct{

	int max_size;
	int front;
	int end;
	int current_size;
	int* array;


} SocketQueue;

SocketQueue* create_queue_socket(int);
int is_empty_socket(SocketQueue*);
int is_full_socket(SocketQueue*);
void enqueue_socket(SocketQueue*, int);
int dequeue_socket(SocketQueue*);
void print_queue_socket(SocketQueue*);
void free_queue_socket(SocketQueue*);

typedef struct{

	int max_size;
	int front;
	int end;
	int current_size;
	char** array;


} LogQueue;

LogQueue* create_queue_log(int);
int is_empty_log(LogQueue*);
int is_full_log(LogQueue*);
void enqueue_log(LogQueue*, char*);
char* dequeue_log(LogQueue*);
void print_queue_log(LogQueue*);
void free_queue_log(LogQueue*);

typedef struct{

	SocketQueue* client_buffer;
	LogQueue* log_buffer;
	char** dictionary;
	pthread_mutex_t client_lock;
	pthread_mutex_t log_lock;
} Bundle;

extern Bundle* bundle;
Bundle* create_bundle(SocketQueue*, LogQueue*, char**);
void free_bundle(Bundle*);

#endif

