CC = gcc

CFLAGS = rm -rf *o spellCheckerServer

all:
	gcc -o spellCheckerServer server_main.c

clean:
	$(CLEANUP)
