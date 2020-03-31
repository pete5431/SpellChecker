CC = gcc

CFLAGS = rm -rf *o testServer

all:
	gcc -pthread -o testServer server_main.c dictionary.c bundle.c worker.c

clean:
	$(CLEANUP)
