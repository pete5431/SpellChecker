#ifndef WORKER_THREAD_H_
#define WORKER_THREAD_H_

void* process_sockets(void* args);
void* process_logs(void* args);
ssize_t read_word(int, void*, size_t);

#endif
