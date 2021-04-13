CC=gcc
CFLAGS= -g -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11

#bin/dm
all: clean bin/cli bin/serv bin/producer_1 bin/producer_2 bin/producer_3
#bin/dm: 
#	$(CC) src/deliveryManager.c -o bin/dm
bin/cli:
	$(CC) $(CFLAGS) src/client.c -o bin/cli
bin/serv:# src/server.c
	$(CC) $(CFLAGS) src/server.c -o bin/serv
bin/producer_1:
	$(CC) $(CFLAGS) src/producer_1.c -o bin/p1
bin/producer_2:
	$(CC) $(CFLAGS) src/producer_2.c -o bin/p2
bin/producer_3:
	$(CC) $(CFLAGS) src/producer_3.c -o bin/p3
clean:
	rm -f bin/cli
	rm -f bin/serv
	rm -f bin/dm
