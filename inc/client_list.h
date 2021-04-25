#ifndef _CLI_LIST_
#define _CLI_LIST_


#define TAM 256

struct Node {
    char* ip;
    int port;
    int cli_sock_fd;    
    struct Node* next;
};
//void print_clients_list(struct Node* node)
extern void push(struct Node** head_ref, char* ip, int port, int cli_sock_fd);
extern void delete_node(struct Node** head_ref, char* ip);
extern void print_clients_list(struct Node* node);
extern int is_in_list(struct Node*, char*);
extern int is_empty(struct Node* node);
extern int get_client_fd(struct Node* node, char* ip);
extern char* get_ip(struct Node* node, int clifd);

#endif