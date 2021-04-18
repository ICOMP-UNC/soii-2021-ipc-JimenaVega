#include <stdio.h>
#include <stdlib.h>
#include "../inc/liblist.h"

int main(){

    node_t * head = NULL;
    head = (node_t *) malloc(sizeof(node_t));
    if (head == NULL) {
        return 1;
    }

    head->val = 1;
    head->next = NULL;

    print_list(head);

}