/**
 * @file CLI.c
 * @author Jimena Vega
 * @brief  This command line interpreter allows the following inputs
 *          add <client-ip> <port> <producer>
 *          delete <client-ip> <port> <producer>
 *          log <client-ip> <port>
 * 
 *         Usage : ./CLI <server-ip> <port>
 *
 * @version 0.1
 * @date 2021-04-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <arpa/inet.h>

#define INPUT_SIZE 512
#define TAM 256
#define h_addr h_addr_list[0] 

void display_name();
int validate_input(char* input);
void print_prompt();
int config_socket(char *argv[]);


int main(int argc, char *argv[]){

    char *input = (char*)malloc(INPUT_SIZE*sizeof(char));
    char buffer[TAM];
    ssize_t n_read, n_write;
    int sockfd;

    if (argc != 3){
		fprintf(stderr, "You should enter: %s <server-ip> <port>\n", argv[0]);
		exit(0);
	}

    sockfd = config_socket(argv);
    print_prompt();

    static int first_time = 1;

    while(1){

        display_name();
        fgets(input, INPUT_SIZE, stdin);
        
        if(!validate_input(input)){
            printf("\nInvalid input\n");
            print_prompt();
        }
        else{
            if(first_time){
               
                n_read = read(sockfd, &buffer, TAM);
                printf("R: %s\n", buffer);
            }
            if(n_read < 0){
                perror("Client: invalid read");
                exit(EXIT_FAILURE);
            }
            else if(n_read > 0){
                //formato de mensaje CLI <ommand> <client-ip> <client-port> <producer>
                char to_send [TAM];
                sprintf (to_send, "CLI %s", input);
                printf("S: %s\n", to_send);

                n_write = write(sockfd, to_send, TAM);
                if(n_write == -1){
                    perror("Client: invalid write.");
                    exit(EXIT_FAILURE);
                }
                n_read = 1;
                first_time = 0;
            }
        }
    }
    //free(input);
    return 0;
}

void display_name(){

    char hostname[100];
    gethostname(hostname,sizeof(hostname));
    printf("\033[1;31m%s@%s:~$\033[0m ",getenv("USER"), hostname);
}

int validate_input(char* input){

    char *copy = strdup(input);
    char *token = strtok(copy, " ");
    int count = 0;

    if((strncmp(token,"add",3) != 0) 
        && (strncmp(token,"log",3) != 0)
        && (strncmp(token,"delete",6) != 0)){

        return 0;
    }

    while(token != NULL){
        token = strtok(NULL," ");
        count++;
    }

    free(token);
    free(copy);

    if(count < 0 || count > 4){
        return 0;
    }
    return 1;
}

void print_prompt(){
    printf("Please enter one of the following commands:\n");
    printf("add \033[2m<client-ip> <port> <producer>\033[0m\n");
    printf("delete \033[2m<client-ip> <port> <producer>\033[0m\n");
    printf("log \033[2m<client-ip> <port>\033[0m\n");

}

int config_socket(char *argv[]){

    int sockfd;
	uint16_t puerto;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	puerto = (uint16_t)atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname(argv[1]);

	memset( (char *) &serv_addr, '0', sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;

	bcopy( (char *) server->h_addr,
		   (char *)&serv_addr.sin_addr.s_addr, 
		   (size_t) server->h_length );

	serv_addr.sin_port = htons(puerto);

	if (connect( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr ) ) < 0 ) {
		perror( "Client: connection error");
		exit(1);
	}

    return sockfd;
}