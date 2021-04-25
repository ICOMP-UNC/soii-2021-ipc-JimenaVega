
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../inc/client_list.h" 
#define TAM 256


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
void delete_node(struct Node** head_ref, char* ip){

    printf("ENTRE delete in single list \n");
    struct Node *temp = *head_ref, *prev;

    if(!is_in_list(temp,ip)){
        //printf("No esta en la lista");
        return;
    }
 
    if (temp != NULL && (strcmp(temp->ip, ip) == 0)){
        *head_ref = temp->next; 
        free(temp); 
        return;
    }
 
    while (temp != NULL && (strcmp(temp->ip, ip) != 0)) {
        prev = temp;
        temp = temp->next;
    }
 
    if (temp == NULL)
        return;
 
    prev->next = temp->next;
 
    free(temp); 
    
    printf("SALI delete in single list \n");
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
        printf("\nIP: %s\t", node->ip);
        printf("port: %d\t", node->port);
        printf("socket: %d\n", node->cli_sock_fd);
        node = node->next;
    }
    printf("---------------\n");
}

int is_empty(struct Node* node){
    
    
    int count = 0;
    while(node != NULL){
        count++;
        node = node->next;
    }
    if(count > 0){
        return 0;
    }
    return 1;
}

int get_client_fd(struct Node* node, char* ip){

    while(node != NULL){

        if(strcmp(node->ip,ip) == 0){

            return node->cli_sock_fd;
        }
        node = node->next;
    }

    return 0;
}

char* get_ip(struct Node* node, int clifd){


    while(node != NULL){

        if(node->cli_sock_fd == clifd){

            return node->ip;
        }
        node = node->next;
    }

    return NULL;  
}