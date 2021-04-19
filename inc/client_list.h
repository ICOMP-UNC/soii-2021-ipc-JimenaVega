#ifndef _CLI_LIST_
#define _CLI_LIST_

#include <stdio.h>
#include <stdlib.h>

struct Node {
    char* ip;
    int port;
    int cli_sock_fd;
    struct Node* next;
};

extern void push(struct Node**, char*, int, int);
extern void delete_node(struct Node** head_ref, char*);
extern void print_clients_list(struct Node*);
extern int is_in_list(struct Node*, char*);

#endif