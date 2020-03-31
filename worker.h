#ifndef WORKER_THREAD_H_
#define WORKER_THREAD_H_

#include <pthread.h>

void* process_sockets(void* args);
void* process_logs(void* args);

#endif
