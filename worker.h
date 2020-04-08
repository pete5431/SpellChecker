#ifndef WORKER_THREAD_H_
#define WORKER_THREAD_H_

void* process_sockets(void* args);
void* process_logs(void* args);
int read_word(int, void*, int);

#endif
