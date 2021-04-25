#include "../inc/discon_list.h"


void free_array(char** arr){

    int i = 0;
    while (arr[i] != NULL){
        free(arr[i]);
        i++;
    }
    free(arr);
}

void print_disc_list(struct Node_d* node){

    while (node != NULL) {
        printf("\nIP: %s\t", node->ip);
        printf("In Productores:\t");
        for(int i = 0; i<3; i++){
            printf("%d ", node->prod[i]);
        }
        //printf("\tstart time %ld\n", node->start_time);
        node = node->next;
    }
    printf("\n---------------\n");
}

void print_disc_buffer(struct Node_d* node){

    printf("print_disc_buffer\n");
    while(node != NULL){
        printf("buffer de %s\n", node->ip);
        for(size_t i = 0; i < node->pos_d ; i++){
            printf("%s\n", node->acum_msg[i]);
        }

        node = node->next;
    }
}

int is_in_disclist(struct Node_d* node, char* ip){

    while(node != NULL){
        if(strcmp(node->ip, ip) == 0){
            return 1;
        }
        node = node->next;
    }
    return 0;

 }

void delete_Node_d(struct Node_d** head_ref, char* ip){

    struct Node_d *temp = *head_ref, *prev;

    if(!is_in_disclist(temp,ip)){
        //printf("No esta en la lista");
        return;
    }

    if (temp != NULL && (strcmp(temp->ip, ip) == 0)){
        free_array(temp->acum_msg);
        *head_ref = temp->next; 
        free(temp); 
        return;
    }
 
    while (temp != NULL && (strcmp(temp->ip, ip) != 0)) {
        prev = temp;
        temp = temp->next;
    }

    free_array(temp->acum_msg);

    if (temp == NULL)
        return;
 
    prev->next = temp->next;
 
    free(temp);
}

long int get_start_time(struct Node_d* node, char* ip){

    while(node != NULL){

        if(strcmp(node->ip,ip) == 0){
            return node->start_time;
        }
        node = node->next;
    }

    return 0;  
}

void push_disc_list(struct Node_d** head_ref, char* ip, int p[3]){

    struct Node_d* new_Node_d = (struct Node_d*)malloc(sizeof(struct Node_d));

    new_Node_d->ip = strdup(ip);

    for (int i = 0; i < 3; i++){
        new_Node_d->prod[i] = p[i];
    }

    new_Node_d->start_time = time(NULL);
    new_Node_d->next = (*head_ref);
    (*head_ref) = new_Node_d;
}
/**
 * @brief  adds a new message from producer to a dinamic array
 * 
 * @param node 
 * @param ip 
 * @param msg 
 */
void add_disc_buff(struct Node_d* node, char msg[TAM]){

    *(node->acum_msg + node->pos_d) = (char*) calloc (strlen(msg+1), sizeof(char));

    node->acum_msg = (char**) realloc(node->acum_msg, 
                                    ((sizeof(char*)) * (2 + node->pos_d)));

    strcpy((node->acum_msg[node->pos_d]), msg);
   
    printf("1- arr[%ld]= %s\n\n",node->pos_d,node->acum_msg[node->pos_d]);

    node->pos_d++;  
}