#ifndef _CLI_LIST_
#define _CLI_LIST_

// #include <stdio.h>
// #include <stdlib.h>
#define TAM 256

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
extern int is_empty(struct Node* node);
extern void send_in_list(struct Node* p, char msg[TAM]);
extern int get_client_fd(struct Node* node, char* ip);

#endif