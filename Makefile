CC=gcc
CFLAGS= -g -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11

all: clean  bin/cli bin/serv bin/p1 bin/p2 bin/p3 bin/CLI bin/dummy

bin/cli:
	$(CC) $(CFLAGS) src/client.c -o bin/cli -lcrypto -lssl
bin/serv:  obj/client_list.o  obj/libserv.o obj/discon_list.o
	$(CC) $(CFLAGS) src/server.c obj/client_list.o obj/libserv.o obj/discon_list.o -o bin/serv -lcrypto -lssl -lzip
bin/CLI:
	$(CC) $(CFLAGS) src/CLI.c -o bin/CLI
obj/libserv.o:
	$(CC) $(CFLAGS) -c src/libserv.c -o obj/libserv.o
obj/client_list.o:
	$(CC) $(CFLAGS) -c src/client_list.c -o obj/client_list.o
obj/discon_list.o:
	$(CC) $(CFLAGS) -c src/discon_list.c -o obj/discon_list.o
bin/p1:
	$(CC) $(CFLAGS) src/producer_1.c -o bin/p1
bin/p2:
	$(CC) $(CFLAGS) src/producer_2.c -o bin/p2
bin/p3:
	$(CC) $(CFLAGS) src/producer_3.c -o bin/p3
bin/dummy:
	$(CC) $(CFLAGS) src/client_dummy.c -o bin/dummy -lcrypto -lssl
clean:
	rm -f bin/*
	rm -f obj/*
