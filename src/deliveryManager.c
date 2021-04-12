#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define INPUT_SIZE 512

// add <socket> <productor>
// delete <socket> <productor>
// log <socket>
// --help -h
void display_name();
char** parse_string(char* line);
int validate_input(char** input);

int main(int argc, char const* argv[]){
    char *input = (char*)malloc(INPUT_SIZE*sizeof(char));
    
    while(1){
        display_name();
        fgets(input, INPUT_SIZE, stdin);
        printf("input:%s\n",input);

        char** commands = parse_string(input);

    }
    free(input);
}

void display_name(){
    char hostname[100];
    gethostname(hostname,sizeof(hostname));
    printf("\033[1;31m%s@%s:~$\033[0m ",getenv("USER"), hostname);
}

char** parse_string(char* line){
    char *token;
    char **arr = NULL;
    int i = 0;
    
    token = strtok(line, " ");
    arr   = (char**) malloc(sizeof(char*));

    do{
       
        *(arr+i) = (char*) calloc (strlen(token+1), sizeof(char));
        arr      = (char**) realloc(arr, sizeof(char*) * (2+i));
        strcpy(arr[i],token);
        token = strtok(NULL, " ");
    
        i++;
    }
    while(token != NULL);

    arr[i]=NULL;
    arr[i-1][strlen(arr[i-1])-1]= '\0';
    free(token);
    //free(line);
     
    return arr;
}

int validate_input(char** input){

    int pass = 0;

    pass = strncmp(input[0],"add",3) + 
}