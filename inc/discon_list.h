#ifndef _DISCON_LIST_
#define _DISCON_LIST_

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct Node_d {
    char* ip;
    int prod[3];
    long int start_time;
    char** acum_msg;
    size_t pos_d; /*amounts of messages in the buffer of the client*/
    struct Node_d* next;
};

extern void free_array(char**);
extern int is_in_disclist(struct Node_d*, char*);
extern void delete_Node_d(struct Node_d**, char*);
extern long int get_start_time(struct Node_d*, char*);
extern void push_disc_list(struct Node_d**, char*, int[3]);
extern void add_disc_buff(struct Node_d* node, char* msg);
extern void print_disc_list(struct Node_d* node);
extern void print_disc_buffer(struct Node_d* node);
extern size_t get_total_msg(struct Node_d* node, char* ip);
extern char** get_buff(struct Node_d* node, char* ip);



#endif