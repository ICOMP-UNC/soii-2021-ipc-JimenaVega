#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#include "../inc/liblist.h"
#include "../inc/client_list.h"

#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
#define PROJECT_ID 'M'
#define QUEUE_PERMISSIONS 0660
#define TAM 256
#define MAX_EVENTS 5000

struct message_text {
    int qid;
    char buf [200];
};

struct message {
    long message_type;
    struct message_text message_text;
};

int config_socket(uint16_t port);
void message_interpreter(char buffer[TAM], int clisockfd);
void suscribe_client(char *producer, char* ip, int port, int clisockfd);
void unsuscribe_client(char *producer, char* ip);
void send_to_suscribers(int producer, char msg[TAM]);
char** parse_string(char* line);
int config_queue();


struct epoll_event ev, events_array[MAX_EVENTS];
int listen_sock, client_sock, ready_fds, epoll_fd;

struct Node* single_clients = NULL;
struct Node* p1 = NULL;
struct Node* p2 = NULL;
struct Node* p3 = NULL;


int main( int argc, char *argv[] ) {
	int serv_sock_fd, ctrl_write, ctrl_read, qid;
	socklen_t clilen;
	char buffer[TAM];
	uint16_t port;
	struct sockaddr_in cli_addr;
	struct message message;

	int test_fd = 0; /*BORRAR*/


	if (argc < 2) {
    	fprintf(stderr, "You should write: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	port = (uint16_t) atoi(argv[1]);

	//queue configuration
	qid = config_queue();
	printf("server qid = %d\n\n", qid);

	//socket configuration
	serv_sock_fd = config_socket(port);
	clilen = sizeof(cli_addr);

	//epoll configuration
	epoll_fd = epoll_create(MAX_EVENTS);
	if(epoll_fd == -1){
		perror("server: epoll_create error");
		exit(EXIT_FAILURE);
	}

	ev.events = EPOLLIN;
	ev.data.fd = serv_sock_fd;

	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serv_sock_fd, &ev) == -1){
		perror("server: epoll_ctl error");
		exit(EXIT_FAILURE);
	}

	while(1){
		
		ready_fds = epoll_wait(epoll_fd, events_array, MAX_EVENTS, 500);
		if(ready_fds == -1){
			perror("Error in epoll_wait()");
			exit(EXIT_FAILURE);
		}

		for(int i = 0; i < ready_fds; i++){
			if(events_array[i].data.fd == serv_sock_fd){

				client_sock = accept(serv_sock_fd,
								  (struct sockaddr *) &cli_addr, 
								   &clilen);

				if (client_sock == -1){
					perror("accept");
					exit(EXIT_FAILURE);
            	}  
				//--------------test------------------------
				char *client_ip = inet_ntoa(cli_addr.sin_addr);
				printf(" client [%d] ip: %s arrived\n", client_sock, client_ip);
				//------------------------------------------

				ev.events = EPOLLIN | EPOLLET | EPOLLOUT;
            	ev.data.fd = client_sock;

			    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock,
                        &ev) == -1) {
					perror("server: epoll_ctl error");
					exit(EXIT_FAILURE);
           		}

				printf("Envio el checksum a client %d\n", client_sock);
				ctrl_write = (int) write(client_sock, "1- a ver si pasas this checksum",TAM);

				if(ctrl_write < 0){
					perror("Error in writing client socket");
					exit(EXIT_FAILURE);
				}
			}
			else{
				if(events_array[i].events & EPOLLIN){

					//printf("Leo lo que me mando cliente %d\n", events_array[i].data.fd);
					ctrl_read = (int) read(events_array[i].data.fd, buffer, TAM);

					if(ctrl_read > 0){
						printf("client [%d] : %s\n", events_array[i].data.fd, buffer);
						test_fd = events_array[i].data.fd;
					}
					else if(ctrl_read < 0){
						perror("Error reading");
						exit(EXIT_FAILURE);
					}

					message_interpreter(buffer, events_array[i].data.fd);
				}
				//else if(events_array[i].events & EPOLLOUT){
	
				//}
			}
		}
		/***************WOKING********************/
		//queue checking
		char msg [TAM];
//for(int p = 1; p < 4; p++){
			int p = 1;
			if (msgrcv (qid, &message, sizeof (struct message_text), p, IPC_NOWAIT) == -1) {

			}else{
				sprintf (msg, "2- msg: %s ", message.message_text.buf);
				printf("MSG = %s\n", msg);
			
				/*printf("---------test----------\n");
				printf("p1 list is empty = %d\n", is_empty(p1));
				print_clients_list(p1);
				send_to_suscribers(p, msg);*/
				if(write(test_fd, msg, TAM) < 0){
					perror("server: could't write message to suscriber\n");
					exit(EXIT_FAILURE);
				}
				
			}
		//}
	}

	return 0; 
} 

int config_socket(uint16_t port){

	struct sockaddr_in serv_addr;
	int serv_sock_fd;
	
	serv_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("192.168.100.7");//INADDR_ANY;
	serv_addr.sin_port = htons(port);
	printf("server address = %s\n", inet_ntoa(serv_addr.sin_addr));

	if (bind(serv_sock_fd, (struct sockaddr *) &serv_addr, sizeof( serv_addr )) < 0 ) {
		perror("Server: error in binding fd with address");
		exit(1);
	}

	printf("Process: %d - available socket: %d\n", getpid(), ntohs(serv_addr.sin_port) );
	listen(serv_sock_fd, MAX_EVENTS);

	return serv_sock_fd;

}

/*********************************************************************//**
* @brief        
* @param[in]   

* @return        None
**********************************************************************/
void message_interpreter(char buffer[TAM], int clisockfd){

	char** commands = parse_string(buffer);

	if((strncmp(commands[0], "ACK", 3) == 0)){
	
		if(!is_in_list(single_clients, commands[1])){
			push(&single_clients, commands[1], (int)strtol(commands[2],(char **)NULL, 10) ,clisockfd);
			printf("...SINGLE LIS....T\n");
			print_clients_list(single_clients);
		}
		
    }
	else if(strncmp(commands[0], "CLI", 3) == 0){

		if((strncmp(commands[1], "add", 3) == 0)){
	
			if(is_in_list(single_clients, commands[2])){
				printf("suscribi P[%s] con ip[%s]\n",commands[4], commands[2]);
				suscribe_client(commands[4], commands[2],
						 ((int)strtol(commands[3],(char **)NULL, 10)), clisockfd);
				//se borra de la lista de solteros y se la agrega a la lista con productores
				delete_node(&single_clients, commands[2]);
			}
			else{
				printf("Client doesn't exist. Please try again.\n");
			}
		}
		else if((strncmp(commands[1], "delete", 6) == 0)){
			if(is_in_list(single_clients, commands[2])){
				unsuscribe_client(commands[4], commands[2]);
			}
			else{
				printf("Client doesn't exist. Please try again.\n");
			}
		}
		else if((strncmp(commands[1], "log", 3) == 0)){
			//still dont know
		}

	}
}

void suscribe_client(char *producer, char* ip, int port, int clisockfd){

	if(strncmp(producer, "P1", 2) == 0){
		printf("I suscribed %s into P1 list\n", ip);
		push(&p1, ip, port, clisockfd);
		print_clients_list(p1);
	}
	else if(strncmp(producer, "P2", 2) == 0){
		push(&p2, ip, port, clisockfd);
		print_clients_list(p2);
	}
	else if(strncmp(producer, "P3", 2) == 0){
		push(&p3, ip, port, clisockfd);
		print_clients_list(p3);
	}
	delete_node(&single_clients, ip);
}

void unsuscribe_client(char *producer, char* ip){

	if(strncmp(producer, "P1", 2) == 0){
		printf("*****************borre al cliente en P1\n");
		delete_node(&p1, ip);
	}
	else if(strncmp(producer, "P2", 2) == 0){
		delete_node(&p2, ip);
	}
	else if(strncmp(producer, "P3", 2) == 0){
		delete_node(&p3, ip);
	}
}

char** parse_string(char* line){
    
    char *token;
    char **arr = NULL;
    size_t i = 0;
    
    token = strtok(line, " ");
    arr   = (char**) malloc(sizeof(char*));

    do{
       
        *(arr+i) = (char*) calloc (strlen(token+1), sizeof(char));
        arr      = (char**) realloc(arr,  ((sizeof(char*)) *(2+i)));
        strcpy(arr[i],token);
        token = strtok(NULL, " ");
    
        i++;
    }
    while(token != NULL);

    arr[i]=NULL;
    arr[i-1][strlen(arr[i-1])-1]= '\0';
    free(token);

	// int x = 0;
	// while(arr[x] != NULL){
	// 	printf("commands[%d] = %s|",x, arr[x]);
	// 	x++;
	// }
	printf("\n");
    //free(line);
     
    return arr;
}

int config_queue(){

    key_t msg_queue_key;
    int qid;

	if ((msg_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (1);
    }

    if ((qid = msgget (msg_queue_key, IPC_CREAT | QUEUE_PERMISSIONS)) == -1) {
        perror ("msgget");
        exit (1);
    }

	return qid;

}

void send_to_suscribers(int producer, char msg[TAM] ){

/***************WOKING********************/
	

	if(producer == 1){
		
		if(!is_empty(p1)){
			printf("Sending to suscribers of p1...\n");
			while(p1 != NULL){

				if(write(p1->cli_sock_fd, msg, TAM) < 0){
					perror("server: could't write message to suscriber\n");
					exit(EXIT_FAILURE);
				}
				printf("Le escribi a p1->ip = %s fd = %d\n",p1->ip, p1->cli_sock_fd);
				p1 = p1->next;
			}
		}
		else{
			printf("There are no suscribers for producer P1\n");
			return;
		}
	}
	else if(producer == 2){
		
		if(!is_empty(p2)){
			printf("Sending to suscribers of p2...\n");
			while(p2 != NULL){

				if(write(p2->cli_sock_fd, msg, TAM) < 0){
					perror("server: could't write message to suscriber\n");
					exit(EXIT_FAILURE);
				}
				p2 = p2->next;
			}
		}
		else{
			printf("There are no suscribers for producer P2\n");
			return;
		}

	}
	else if(producer == 3){
		
		if(!is_empty(p3)){
			printf("Sending to suscribers of p3...\n");
			while(p3 != NULL){

				if(write(p3->cli_sock_fd, msg, TAM) < 0){
					perror("server: could't write message to suscriber\n");
					exit(EXIT_FAILURE);
				}
				p3 = p3->next;
			}
		}
		else{
			printf("There are no suscribers for producer P3\n");
			return;
		}
	}
}
