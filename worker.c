#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include "worker.h"
#include "bundle.h"
#include "dictionary.h"

#define MAX_LINE 64

// Global bundle. Holds data structures and mutex, cond variables.
Bundle* bundle;

void* process_sockets(void* args){

	//The connected socket.
	int connected_fd;
	// The buffer to hold words read from socket.
	char line[MAX_LINE];	
	// The number of bytes read.
	int bytes_read;

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

		// Continue reading words from socket.
		while((bytes_read = read_word(connected_fd, line, MAX_LINE - 1)) > 0){
			
			printf("Connection from %d typed: %s\n", connected_fd, line);

			// Malloc enough space for the word and the status that will be appended.
			char* log_event = (char*) malloc((bytes_read + 13) * sizeof(char));

			if(log_event == NULL){
				printf("Allocation failed for log_event.\n");
				pthread_exit((void*)1);
			}

			// Copy the line into log_event.
			strncpy(log_event, line, bytes_read);

			// However terminate it at where the newline is.
			log_event[bytes_read - 1] = '\0';

			// Depending on if the word is found.
			if(find_word(bundle->dictionary, line)){
				// Append OK if found.
				strncat(log_event, ": OK\n", 5);
				// Write to socket.
				write(connected_fd, log_event, bytes_read + 5);
			}
			else{
				// Append MISSPELLED if not found.
				strncat(log_event, ": MISSPELLED\n", 13);
				// Write to socket.
				write(connected_fd, log_event, bytes_read + 13);
			}

			// Get the lock for log buffer.
			pthread_mutex_lock(&(bundle->log_lock));

			// If the log buffer is full wait till it is not full. Meanwhile give up the lock.
                        if(is_full_log(bundle->log_buffer)){
                                pthread_cond_wait(&(bundle->log_full), &(bundle->log_lock));
                        }

			// Enqueue the string into the log buffer.
			enqueue_log(bundle->log_buffer, log_event);
			
			// Unlock the lock for log buffer.
			pthread_mutex_unlock(&(bundle->log_lock));
			
			// Signal to those waiting for log to be not empty that it is not longer empty.
			pthread_cond_signal(&(bundle->log_empty));			
		}

		// If user terminates the connect, then the connection closes.
		printf("Connection from %d closed.\n", connected_fd);
		close(connected_fd);
	}
}

void* process_logs(void* args){

	// Open the log file to reset it.
	FILE* fp = fopen("log.txt", "w");
	if(fp == NULL){
		printf("Failed to reset file.\n");
	}
	fclose(fp);

	while(1){

		// Lock the log buffer.
		pthread_mutex_lock(&(bundle->log_lock));
	
		// If the log buffer is empty, wait until it isn't. Meanwhile unlock the lock.
		if(is_empty_log(bundle->log_buffer)){
			pthread_cond_wait(&(bundle->log_empty), &(bundle->log_lock));
		}	

		// Dequeue a string from log buffer.
		char* log_event = dequeue_log(bundle->log_buffer);

		// Unlock the lock.
		pthread_mutex_unlock(&(bundle->log_lock));

		// Signal to those waitin for log to be not full that it is no longer full.
                pthread_cond_signal(&(bundle->log_full));

		// Open the file for appending.
		FILE *fp = fopen("log.txt", "a");

		if(fp == NULL){
			printf("File open failed.\n");
			pthread_exit((void*)1);
		}

		// Write to log file.
		fprintf(fp, "%s", log_event); 

		// Free the allocated memory.
		free(log_event);

		// Close the file.
		fclose(fp);
	}
}

int read_word(int connected_fd, void* buffer, int n){

	// The number of bytes read.
	int numRead;

	// The total number of bytes read.
	int totalRead;

	// The pointer to the passed buffer.
	char *buf;
	
	// Stores read chars.
	char ch;

	if(n <= 0 || buffer == NULL){
		errno = EINVAL;
		return -1;
	}

	buf = buffer;

	totalRead = 0;

	// While true.
	while(1){

		// Read one char from socket at a time.
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
			// As long as size isn't exceeded and the character isn't \r, add to buffer.
			if(totalRead < n - 1 && ch != 13){
				totalRead++;
				*buf++ = ch;
			}
			// Upong newline break the loop.
			if(ch == '\n'){
				break;
			}
		}
	}
	// Add null terminator to end.
	*buf = '\0';
	// Return the total number of bytes read.
	return totalRead;
}
