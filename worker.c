#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "worker.h"
#include "bundle.h"

Bundle* bundle;

void* process_sockets(void* args){

	print_queue_socket(bundle->client_buffer);

	//pthread_mutex_lock(&(queue->socket_lock));

	printf("first\n");
	printf("second\n");
	printf("third\n");
	printf("fourth\n");

	//pthread_mutex_unlock(&(queue->socket_lock));
}

void* process_logs(void* args){



}
