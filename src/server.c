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
#include <openssl/md5.h>
#include <time.h>
#include <signal.h>
#include <zip.h>


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
char* wrap_in_frame(char msg[TAM]);
void get_md5hash(char *str, unsigned char digest[MD5_DIGEST_LENGTH]);
void send_in_list(struct Node* p, char msg[TAM], int producer);
void send_to_log(char* cli_ip, char msg[TAM], int producer);
int config_queue();
char *time_stamp();
void signal_handler(int signum);
void compress_file();


struct epoll_event ev, events_array[MAX_EVENTS];
int listen_sock, client_sock, ready_fds, epoll_fd;

struct Node* single_clients = NULL;
struct Node* p1 = NULL;
struct Node* p2 = NULL;
struct Node* p3 = NULL;


FILE* file = NULL;



int main( int argc, char *argv[] ) {

	signal(SIGINT, signal_handler);
	int serv_sock_fd, ctrl_write, ctrl_read, qid;
	socklen_t clilen;
	char buffer[TAM];
	uint16_t port;
	struct sockaddr_in cli_addr;
	struct message message;


	if (argc < 2) {
    	fprintf(stderr, "You should write: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	port = (uint16_t) atoi(argv[1]);

	//queue configuration
	qid = config_queue();
	printf("server QID : %d\n", qid);

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
				
				printf(" client [%d] arrived\n", client_sock);
				

				ev.events = EPOLLIN | EPOLLET | EPOLLOUT;
            	ev.data.fd = client_sock;

			    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock,
                        &ev) == -1) {
					perror("server: epoll_ctl error");
					exit(EXIT_FAILURE);
           		}
				/*************WORKING****************************/
				printf("Envio el checksum a client %d\n", client_sock);

				char* checksum = wrap_in_frame("SA");
				char wrapped[TAM];
				sprintf(wrapped, "%s", checksum);
				printf("wrapped = %s\n", wrapped);
				ctrl_write = (int) write(client_sock, wrapped, TAM);

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

					message_interpreter(buffer, events_array[i].data.fd);
				}
			}
		}
		/***************WOKING********************/
		//queue checking
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

int config_socket(uint16_t port){

	struct sockaddr_in serv_addr;
	int serv_sock_fd;
	
	serv_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("192.168.100.7");//INADDR_ANY;
	serv_addr.sin_port = htons(port);
	printf("server address : %s\n", inet_ntoa(serv_addr.sin_addr));

	if (bind(serv_sock_fd, (struct sockaddr *) &serv_addr, sizeof( serv_addr )) < 0 ) {
		perror("Server: error in binding fd with address");
		exit(EXIT_FAILURE);
	}

	printf("Process: %d - available socket: %d\n", getpid(), ntohs(serv_addr.sin_port));
	printf("----------------------------------------------\n");
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

			push(&single_clients, commands[1], 
				((int)strtol(commands[2],(char **)NULL, 10)) ,clisockfd);

			printf("...SINGLE LIST....\n");
			print_clients_list(single_clients);
		}
		// else{
		// 	printf("There's already a client with that IP. Please try with other.\n");
		// } 
		
    }
	else if(strncmp(commands[0], "CLI", 3) == 0){

		if((strncmp(commands[1], "add", 3) == 0)){
	
			if(is_in_list(single_clients, commands[2])){

				char* aux_ip = strdup(commands[2]);
				int fd = get_client_fd(single_clients, aux_ip);
				printf("suscribi P%s con ip[%s] y fd[%d]\n",commands[4], commands[2],fd);

				
				suscribe_client(commands[4], commands[2],
						 ((int)strtol(commands[3],(char **)NULL, 10)), fd);
				//se borra de la lista de solteros y se la agrega a la lista con productores
				//delete_node(&single_clients, commands[2]);
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
			//commands[2] fd to send

			compress_file();

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
	//delete_node(&single_clients, ip);
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

void send_to_suscribers(int producer, char msg[TAM]){

	if(producer == 1){
		
		if(!is_empty(p1)){
			
			printf("Sending to suscribers of p1...\n");
			char *wrapped  = wrap_in_frame(msg);
			send_in_list(p1, wrapped, producer);
			print_clients_list(p1);

		}
		else{
			printf("There are no suscribers for producer P1\n");
			return;
		}
	}
	else if(producer == 2){
		
		if(!is_empty(p2)){
			printf("Sending to suscribers of p2...\n");
			char *wrapped  = wrap_in_frame(msg);
			send_in_list(p2, wrapped, producer);
			printf("\n---printing p2----\n");
			print_clients_list(p2);

		}
		else{
			printf("There are no suscribers for producer P2\n");
			return;
		}

	}
	else if(producer == 3){
		
		if(!is_empty(p3)){
			char *wrapped  = wrap_in_frame(msg);
			send_in_list(p3, wrapped, producer);
			printf("\n---printing p3 list----\n");
			print_clients_list(p3);

		}
		else{
			printf("There are no suscribers for producer P3\n");
			return;
		}
	}
}

void send_in_list(struct Node* p, char msg[TAM], int producer){

    while(p != NULL){

        if(write(p->cli_sock_fd, msg, TAM) < 0){
            perror("server: could't write message to suscriber\n");
            exit(EXIT_FAILURE);
        }
		send_to_log(p->ip, msg, producer);
        printf("Le escribi a p%d->ip = %s fd = %d\n",producer,p->ip, p->cli_sock_fd);
        p = p->next;
    }
}

void send_to_log(char* cli_ip, char msg[TAM], int producer){

	printf("ESTOY LOGEANDO\n");

	if ((file = fopen("log.txt", "a")) == NULL) {
        
        perror("Can't open log.txt");
        exit(EXIT_FAILURE);
    }
	
	static int max_lines = 0;

	if(max_lines < 1000){
		printf("inside if\n");
		/****************WORKING******************/
		fprintf(file, "%s from: P%d to: %s | msg: %s\n"
					,time_stamp(), producer, cli_ip, msg);  

		max_lines++;
	}
	else{
		max_lines = 0;
		fclose(file);
		file = fopen("log.txt", "w");
		fclose(file);
		file = fopen("log.txt", "a");
	}

}

char* time_stamp(){

    char *timestamp = (char *)malloc(sizeof(char) * 16);
    time_t ltime = time(NULL);
    struct tm *tm = localtime(&ltime);
 
    sprintf(timestamp,"%02d/%02d/%04d %02d:%02d:%02d", tm->tm_mday, tm->tm_mon, 
        tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
    return timestamp;
}

char* wrap_in_frame(char msg[TAM]){

	char *result = malloc(64);
    char new_buf[(MD5_DIGEST_LENGTH*2)+1];
    unsigned char digest2[MD5_DIGEST_LENGTH];

    get_md5hash(msg, digest2);

    for (int i = 0, j = 0; i < MD5_DIGEST_LENGTH; i++, j+=2){
		sprintf(new_buf+j, "%02x", digest2[i]);
	}

	sprintf(result,"server %s %s", msg, new_buf);

	return result;
}

void get_md5hash(char *str, unsigned char digest[MD5_DIGEST_LENGTH]) {

    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, str, strlen(str));
    MD5_Final(digest, &ctx);
}


void signal_handler(int signum){

  printf("\nInside handler function  %d\n", signum);
  fclose(file);
  signal(SIGINT,SIG_DFL);   // Re Register signal handler for default action
}

void compress_file(){

	char* buffer = NULL;
	long int buf_size = 0;
	zip_source_t* zs;
	zip_t* z;
	int err;

	fclose(file);
	

	if ((file = fopen("log.txt", "r")) == NULL) {
        
        perror("Can't open log.txt to read");
        exit(EXIT_FAILURE);
    }

	rewind(file);
    fseek(file, 0, SEEK_END);
    buf_size = ftell(file);

	rewind(file);
	buffer = malloc((size_t)(buf_size+1) * sizeof(buffer));
	fread(buffer,(size_t) buf_size, 1, file);

	buffer[buf_size] = '\0';
    printf("%s", buffer);


	z = zip_open("log.zip", ZIP_CREATE, &err);
    if(z == NULL){
        perror("Error al clear el archivo zip\n");
        exit(EXIT_FAILURE);
    }

	zs = zip_source_buffer(z, buffer, strlen(buffer), 0);
    zip_file_add(z, "log.txt", zs, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8);

    zip_close(z);

	
	if ((file = fopen("log.txt", "a")) == NULL) {
        
        perror("Can't open log.txt to append");
        exit(EXIT_FAILURE);
    }
	
	free(buffer);

}