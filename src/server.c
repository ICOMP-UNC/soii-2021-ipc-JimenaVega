 #include "../inc/libserv.h"



int main( int argc, char *argv[] ) {

	signal(SIGINT, signal_handler);
	int serv_sock_fd, ctrl_write, ctrl_read, qid;
	socklen_t clilen;
	char buffer[TAM];
	uint16_t port;
	struct sockaddr_in cli_addr;
	struct message message;


	if (argc < 3) {
    	fprintf(stderr, "You should write: %s <port> <server-ip>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	port = (uint16_t) atoi(argv[1]);

	//queue configuration
	qid = config_queue();
	printf("server QID : %d\n", qid);

	//socket configuration
	serv_sock_fd = config_socket(port, argv[2]);
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
				
				printf(" client [%d] arrived\n", client_sock);
				

				ev.events = EPOLLIN | EPOLLET | EPOLLOUT | EPOLLRDHUP;
            	ev.data.fd = client_sock;

				printf("client socket %d\n", client_sock);

			    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock,
                        &ev) == -1) {
					perror("server: epoll_ctl error");
					exit(EXIT_FAILURE);
           		}

				char* checksum = wrap_in_frame("SA");
				char wrapped[TAM];
				sprintf(wrapped, "%s", checksum);
				
				ctrl_write = (int) write(client_sock, wrapped, TAM);

				if(ctrl_write < 0){
					perror("Error in writing client socket");
					exit(EXIT_FAILURE);
				}
			}
			else{
				if(events_array[i].events & EPOLLIN){

					ctrl_read = (int) read(events_array[i].data.fd, buffer, TAM);
					printf("R: %s\n", buffer);

					if(ctrl_read > 0){
						printf("client [%d] : %s\n", events_array[i].data.fd, buffer);
						message_interpreter(buffer, events_array[i].data.fd);
						
					}
					else if(ctrl_read <= 0){
						printf("Client [%d] disconnected\n",  events_array[i].data.fd);
						int aux[3];

						char* cli_ip = get_ip(single_clients, events_array[i].data.fd);
						if(cli_ip != NULL){
							printf("1-pase\n");
							aux[0] = is_in_list(p1, cli_ip);
							aux[1] = is_in_list(p2, cli_ip);
							aux[2] = is_in_list(p3, cli_ip);
							printf("2-pase\n");

							send_to_log(cli_ip, "", -1);

							push_disc_list(&disc_clients, cli_ip, aux);
							print_disc_list(disc_clients);
							delete_node(&single_clients, cli_ip);
						}
						else{
							printf("SOY LA CLI u un cliente con la IP repetida\n");
						}

					


						free(cli_ip);
					}
					
					bzero(buffer, TAM);
				}
		
			}
		}
		
		//queue messages checking
		char msg [TAM];
		for(int p = 1; p < 4; p++){
			
			if (msgrcv (qid, &message, sizeof (struct message_text), p, IPC_NOWAIT) == -1) {

			}else{

				sprintf (msg, "%s", message.message_text.buf);

				send_to_suscribers(p, msg);
			}
		}
	}

	return 0; 
} 

