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

struct Node* clients_head = NULL;


int config_socket(uint16_t port);
void message_interpreter(char buffer[TAM]);



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
				printf("address of client [%d]= %d",client_sock, cli_addr.sin_addr.s_addr);
				char *client_ip = inet_ntoa(cli_addr.sin_addr);
				printf(" client ip: %s\n", client_ip);
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

					printf("Leo lo que me mando cliente %d\n", events_array[i].data.fd);
					ctrl_read = (int) read(events_array[i].data.fd, buffer, TAM);

					if(ctrl_read > 0){
						printf("recv from cli %d = %s\n", events_array[i].data.fd, buffer);
					}
					else if(ctrl_read < 0){
						perror("Error reading");
						exit(EXIT_FAILURE);
					}

					//parser func & interpreter
					message_interpreter(buffer);



				}
				else if(events_array[i].events & EPOLLOUT){

					printf("Le mando mensajes normalmente el cli %d\n",events_array[i].data.fd);
					char buf [TAM];
        			sprintf (buf, "2- client: %d pasaste", events_array[i].data.fd);
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

void message_interpreter(char buffer[TAM]){

	int aux_port = 0;
	char* aux_ip = "";
	char* token = strtok(buffer, " ");

	if(strncmp(token, "ACK", 3) == 0){
		//parse & add to list
	
		printf("----------------------------\n");
		for(int i=0; i<2; i++ ){
        
			token = strtok(NULL," ");
			if(i == 1){
				aux_port = (int) strtol(token, (char **)NULL, 10);
			}
			else{
				aux_ip = strdup(token);
			}
			printf("token = %s\n", token);
		}
		printf("IP = %s port = %d\n", aux_ip, aux_port);
		printf("----------------------------\n");
    }
	else if(strncmp(token, "CLI", 3) == 0){
		//parse, trverse client list and put into marriedclient list
	}
}
