/*references https://www.geeksforgeeks.org/linked-list-set-3-deleting-node/*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/client_list.h" 

void push(struct Node** head_ref, char* ip, int port, int cli_sock_fd){

    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));

    new_node->ip = strdup(ip);
    new_node->port = port;
    new_node->cli_sock_fd = cli_sock_fd;
    new_node->next = (*head_ref);
    (*head_ref) = new_node;
}
 
/* Given a reference (pointer to pointer) to the head of a
   list and a key, deletes the first occurrence of key in
   linked list */
void delete_node(struct Node** head_ref, char* key){

    struct Node *temp = *head_ref, *prev;
 
    if (temp != NULL && (strcmp(temp->ip, key) == 0)){
        *head_ref = temp->next; 
        free(temp); 
        return;
    }
 
    while (temp != NULL && (strcmp(temp->ip, key) != 0)) {
        prev = temp;
        temp = temp->next;
    }
 
    if (temp == NULL)
        return;
 
    prev->next = temp->next;
 
    free(temp); 
}

int is_in_list(struct Node* node, char* key){
  
    while(node != NULL){
        if(strcmp(node->ip, key) == 0){
            return 1;
        }
        node = node->next;
    }
    return 0;
}
 

void print_clients_list(struct Node* node){

    while (node != NULL) {
        printf("\nIP: %s ", node->ip);
        printf("port: %d \n", node->port);
        printf("socket: %d \n", node->cli_sock_fd);
        node = node->next;
    }
}
 