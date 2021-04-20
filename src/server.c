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


#include "../inc/liblist.h"
#include "../inc/client_list.h"

#define TAM 256
#define MAX_EVENTS 5000

struct epoll_event ev, events_array[MAX_EVENTS];
int listen_sock, client_sock, ready_fds, epoll_fd;

struct Node* single_clients = NULL;
struct Node* p1 = NULL;
struct Node* p2 = NULL;
struct Node* p3 = NULL;


int config_socket(uint16_t port);
void message_interpreter(char buffer[TAM], int clisockfd);
void suscribe_client(char *producer, char* ip, int port, int clisockfd);


int main( int argc, char *argv[] ) {
	int serv_sock_fd, ctrl_write, ctrl_read;
	socklen_t clilen;
	char buffer[TAM];
	uint16_t port;
	struct sockaddr_in cli_addr;


	if (argc < 2) {
    	fprintf(stderr, "You should write: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	port = (uint16_t) atoi(argv[1]);

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
				printf(" client [%d] ip: %s\n", client_sock, client_ip);
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
					}
					else if(ctrl_read < 0){
						perror("Error reading");
						exit(EXIT_FAILURE);
					}

					//parser func & interpreter
					message_interpreter(buffer, events_array[i].data.fd);



				}
				else if(events_array[i].events & EPOLLOUT){

					char buf [TAM];
        			sprintf (buf, "2- client: %d mensaje regular", events_array[i].data.fd);
					ctrl_write = (int) write(events_array[i].data.fd, buf, TAM);
				}
			}

	
		}
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
	printf("server: server address = %d\n", serv_addr.sin_addr.s_addr);

	if (bind(serv_sock_fd, (struct sockaddr *) &serv_addr, sizeof( serv_addr )) < 0 ) {
		perror("Server: error in binding fd with address");
		exit(1);
	}

	printf("Process: %d - available socket: %d\n", getpid(), ntohs(serv_addr.sin_port) );
	listen(serv_sock_fd, MAX_EVENTS);

	return serv_sock_fd;

}

void message_interpreter(char buffer[TAM], int clisockfd){

	int aux_port = 0;
	char* aux_ip = "";
	char* token = strtok(buffer, " ");
	char* producer = "";

	if((strncmp(token, "ACK", 3) == 0)){
	
		for(int i=0; i<2; i++ ){
        
			token = strtok(NULL," ");
			if(i == 1){
				aux_port = (int) strtol(token, (char **)NULL, 10);
			}
			else{
				aux_ip = strdup(token);
			}
		}
		
		if(!is_in_list(single_clients,aux_ip)){
			push(&single_clients, aux_ip, aux_port, clisockfd);
		}
		//print_clients_list(single_clients);
    }
	else if(strncmp(token, "CLI", 3) == 0){

		//parse, trverse client list and put into marriedclient list
		token = strtok(NULL," ");
		if((strncmp(token, "add", 3) == 0)){

			for(int i=0; i<3; i++){
        
				token = strtok(NULL," ");
				if(i == 1){
					aux_port = (int) strtol(token, (char **)NULL, 10);
				}
				else if(i == 0){
					aux_ip = strdup(token);
				}
				else{
					producer = strdup(token);
				}
			}
	
			if(is_in_list(single_clients, aux_ip)){
		
				suscribe_client(producer, aux_ip, aux_port, clisockfd);
			}
			else{
				printf("Client doesn't exist. Please try again.\n");
			}
		}
		else if((strncmp(token, "delete", 6) == 0)){
			//pop
		}
		else if((strncmp(token, "log", 6) == 0)){
			//still dont know
		}

	}
}

void suscribe_client(char *producer, char* ip, int port, int clisockfd){

	if(strncmp(producer, "P1", 2) == 0){
		printf("***Es un P1 entonces pusheo\n");
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



