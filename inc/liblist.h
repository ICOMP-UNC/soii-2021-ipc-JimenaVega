#ifndef _LIBLIST_
#define _LIBLIST_

#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int val;
    struct node * next;
} node_t;


extern void print_list(node_t * head);
extern void push_end(node_t * head, int val);
extern void push_start(node_t ** head, int val);
extern int remove_by_index(node_t ** head, int n);

#endif