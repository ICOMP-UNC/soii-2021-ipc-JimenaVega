#include <stdio.h>
#include <stdlib.h>

#include "../inc/liblist.h"


void print_list(node_t * head) {
    node_t * current = head;

    while (current != NULL) {
        printf("IP: %s\n", current->ip);
        printf("socket: %d\n", current->client_sock_fd);
        printf("port: %d\n\n", current->port);
        current = current->next;
    }
}

void push_end(node_t * head, char* ip, int client_sock_fd, int port) {
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = (node_t *) malloc(sizeof(node_t));
    current->next->ip = strdup(ip);
    current->next->client_sock_fd = client_sock_fd;
    current->next->port = port;
    current->next->next = NULL;
}

void push_start(node_t ** head, char* ip, int client_sock_fd, int port) {
    node_t * new_node;
    new_node = (node_t *) malloc(sizeof(node_t));

    new_node->ip = strdup(ip);
    new_node->client_sock_fd = client_sock_fd;
    new_node->port = port;
    new_node->next = *head;
    *head = new_node;
}

// char* pop(node_t ** head) {
//     char* retip = "";
//     node_t * next_node = NULL;

//     if (*head == NULL) {
//         return NULL;
//     }

//     next_node = (*head)->next;
//     retip = (*head)->ip;
//     free(*head);
//     *head = next_node;

//     return retip;
// }

// //lista sin funcar aun añadir remove_by_key
// int remove_by_index(node_t ** head, int n) {
//     int i = 0;
//     int retval = -1;
//     node_t * current = *head;
//     node_t * temp_node = NULL;

//     if (n == 0) {
//         return pop(head);
//     }

//     for (i = 0; i < n-1; i++) {
//         if (current->next == NULL) {
//             return -1;
//         }
//         current = current->next;
//     }

//     temp_node = current->next;
//     retval = temp_node->val;
//     current->next = temp_node->next;
//     free(temp_node);

//     return retval;

// }

