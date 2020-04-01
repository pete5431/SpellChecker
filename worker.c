#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include "worker.h"
#include "bundle.h"

Bundle* bundle;

void* process_sockets(void* args){

	int connected_fd;
	ssize_t bytes_read;

	while(1){

		pthread_mutex_lock(&(bundle->client_lock));
	
		if(is_empty_socket(bundle->client_buffer)){
			pthread_cond_wait(&(bundle->client_empty), &(bundle->client_lock));
			connected_fd = dequeue_socket(bundle->client_buffer);
			pthread_cond_signal(&(bundle->client_full));
		}	
		else connected_fd = dequeue_socket(bundle->client_buffer);		

		

		pthread_mutex_lock(&(bundle->client_lock));

	}
}

void* process_logs(void* args){



}


