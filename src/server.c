#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>

#define TAM 257
#define MAX_EVENTS 10

struct epoll_event ev, events_array[MAX_EVENTS];
int listen_sock, client_sock, ready_fds, epoll_fd;

int epoll_event_add(int epfd, int fs, int event);
int epoll_event_del(int epfd,int fd,int event);

//nfds = ready_fds
int main( int argc, char *argv[] ) {
	int serv_sock_fd, ctrl_write, ctrl_read;
	socklen_t clilen;
	//ssize_t n;
	uint16_t puerto;
	//char buffer[TAM];
	struct sockaddr_in serv_addr, cli_addr;


	if (argc < 2) {
    	fprintf( stderr, "You should write: %s <port>\n", argv[0] );
		exit(1);
	}
	//socket creation
	serv_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	puerto = (uint16_t) atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(puerto);

	//binding
	if (bind(serv_sock_fd, (struct sockaddr *) &serv_addr, sizeof( serv_addr )) < 0 ) {
		perror("Server: error in binding fd with address");
		exit(1);
	}

	printf("Proceso: %d - socket disponible: %d\n", getpid(), ntohs(serv_addr.sin_port) );

	listen(serv_sock_fd, MAX_EVENTS);
	clilen = sizeof(cli_addr);//??

	epoll_fd = epoll_create1(0);//??ver si 0
	if(epoll_fd == -1){
		perror("Error in epoll_create1()");
		exit(EXIT_FAILURE);
	}

	ev.events = EPOLLIN;
	ev.data.fd = serv_sock_fd;

	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serv_sock_fd, &ev) == -1){
		perror("epoll_ctl: listen:sock");
		exit(EXIT_FAILURE);
	}

	while(1){
		//blocking epoll_Wait
		ready_fds = epoll_wait(epoll_fd, events_array, MAX_EVENTS, -1);
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

				ctrl_write = (int) write(client_sock, "message <checksum>",18);

				if(ctrl_write < 0){
					perror("Error in writing client socket");
					exit(EXIT_FAILURE);
				}

				//add to interest list
				ev.events = EPOLLIN | EPOLLET;
            	ev.data.fd = client_sock;

			    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock,
                        &ev) == -1) {
					perror("epoll_ctl: client_sock");
					exit(EXIT_FAILURE);
           		}
			}
			else{
				char buffer[1024] = {0};
				ctrl_read = (int) read(events_array[i].data.fd, buffer, sizeof(buffer));
				if(ctrl_read > 0){
					printf("\nRECV from cli %d = %s\n", events_array[i].data.fd, buffer);
				}
				else if(ctrl_read < 0){
					perror("Error reading");
					exit(EXIT_FAILURE);
				}
			}
	
		}
	}

	return 0; 
} 
