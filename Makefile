CC=gcc
CFLAGS= -g -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11

bin/cli: clean bin/serv
	$(CC) $(CFLAGS) src/client.c -o bin/cli
bin/serv:# src/server.c
	$(CC) $(CFLAGS) src/server.c -o bin/serv
clean:
	rm -f bin/cli
	rm -f bin/serv
