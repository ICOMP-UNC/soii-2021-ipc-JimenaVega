#ifndef _LIBLIST_
#define _LIBLIST_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    char* ip;
    int client_sock_fd;
    int port;
    struct node * next;
} node_t;


extern void print_list(node_t * head);
extern void push_end(node_t * head, char* ip, int client_sock_fd, int port);
extern void push_start(node_t ** head, char* ip, int client_sock_fd, int port);
extern int remove_by_index(node_t ** head, char* ip, int client_sock_fd, int port);

#endif