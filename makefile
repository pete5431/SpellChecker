CC = gcc

CFLAGS = rm -rf *o spellCheckerServer

all:
	gcc -o spellCheckerServer server_main.c dictionary.c fixed_queue.c

clean:
	$(CLEANUP)
