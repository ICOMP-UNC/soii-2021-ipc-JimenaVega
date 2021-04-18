CC=gcc
CFLAGS= -g -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11

all: clean obj/liblist.o bin/cli bin/serv bin/p1 bin/p2 bin/p3 bin/CLI

bin/cli:
	$(CC) $(CFLAGS) src/client.c -o bin/cli
bin/serv:# src/server.c
	$(CC) $(CFLAGS) src/server.c obj/liblist.o -o bin/serv
bin/CLI:
	$(CC) $(CFLAGS) src/CLI.c -o bin/CLI
obj/liblist.o:
	$(CC) $(CFLAGS) -c src/liblist.c -o obj/liblist.o
bin/p1:
	$(CC) $(CFLAGS) src/producer_1.c -o bin/p1
bin/p2:
	$(CC) $(CFLAGS) src/producer_2.c -o bin/p2
bin/p3:
	$(CC) $(CFLAGS) src/producer_3.c -o bin/p3
clean:
	rm -f bin/*
