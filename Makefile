CC=gcc
CFLAGS= -g -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11

#bin/dm
all: clean bin/cli bin/serv
#bin/dm: 
#	$(CC) src/deliveryManager.c -o bin/dm
bin/cli:
	$(CC) $(CFLAGS) src/client.c -o bin/cli
bin/serv:# src/server.c
	$(CC) $(CFLAGS) src/server.c -o bin/serv
clean:
	rm -f bin/cli
	rm -f bin/serv
	rm -f bin/dm
