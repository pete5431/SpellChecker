# OS-SPELL-CHECKER

How to use the SpellChecker Server:

Use the provided makefile to compile the files into an executable using make.

Use ./testServer to run the server on default dictionary and port number (55522).

./testServer filename.txt port# (For custom files or when choosing a port).

Once the server is ran it will be awaiting for connections.

The simplest way to test it is to open a separate terminal on the same system and use a client program or your own.

Use the loopback address 127.0.0.1 and the port number that was set or the default (55522).

Upon connecting, the server will say a client connected.

On your client you can begin to type in words (the character limit is 64).

The server will tell the client if the word is spelled correctly.

word: OK means it is.

word: MISSPELLED means it is not.

All spell check activities will be recorded in a log.txt for the server.
