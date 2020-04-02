#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include "worker.h"
#include "bundle.h"

Bundle* bundle;

void* process_sockets(void* args){

	int connected_fd;
	char line[64];
	ssize_t bytes_read;

	while(1){

		// Lock the client buffer.
		pthread_mutex_lock(&(bundle->client_lock));

		// If the client buffer is empty, wait until not empty. Unlock the lock.
		if(is_empty_socket(bundle->client_buffer)){
			pthread_cond_wait(&(bundle->client_empty), &(bundle->client_lock));
		}

		// Take from the client buffer.
		connected_fd = dequeue_socket(bundle->client_buffer);

		// Unlock the client buffer after taking.
		pthread_mutex_unlock(&(bundle->client_lock));

		// Signal to any thread waiting for when client buffer is not full, that it is no longer full.
		pthread_cond_signal(&(bundle->client_full));

		while((bytes_read = read_word(connected_fd, line, 64 - 1)) > 0){

                        printf("%s", line);

     		}

		printf("Connection closed.\n");
		close(connected_fd);
	}
}

void* process_logs(void* args){

	

}

ssize_t  read_word(int connected_fd, void* buffer, size_t n){

	ssize_t numRead;

	size_t totalRead;

	char *buf;
	
	char ch;

	if(n <= 0 || buffer == NULL){
		errno = EINVAL;
		return -1;
	}

	buf = buffer;

	totalRead = 0;

	while(1){

		numRead = read(connected_fd, &ch, 1);

		if(numRead == -1){
			if(errno == EINTR){
				continue;
			}else return -1;
		}
		else if(numRead == 0){
			if(totalRead == 0){
				return 0;
			}
			else break;
		}
		else{
			if(totalRead < n - 1){
				totalRead++;
				*buf++ = ch;
			}
			if(ch == '\n'){
				break;
			}
		}

		*buf = '\0';
		return totalRead;
	}
}
