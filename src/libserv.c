
#include <stdio.h>
#include <stdlib.h>


#include "../inc/libserv.h"


/**
 * @brief configure IPv4 with connection socket
 * 
 * @param port to connect
 * @return int socket fd
 */
int config_socket(uint16_t port, char* server_ip){

	struct sockaddr_in serv_addr;
	int serv_sock_fd;
	
	serv_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(server_ip);//INADDR_ANY;
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

/**
 * @brief interprets commands from CLI and redirect to the 
 * 			right functions.
 * 			commands[0] = ACK/CLI
 * 			commands[1] = client-ip
 * 			commands[2] = port
 * 
 * @param buffer message to send from producer
 * @param clisockfd 
 */
void message_interpreter(char buffer[TAM], int clisockfd){

	char** commands = parse_string(buffer);

	if((strncmp(commands[0], "ACK", 3) == 0)){

		if(is_in_disclist(disc_clients, commands[1])){

			long int disc_time = get_start_time(disc_clients, commands[1]);
			if((time(NULL) - disc_time) < MAX_WAIT ){
				printf("\nENVIO BUFFER A CLIENTE\n");
			}
			else{
				// if (close(clisockfd) < 0) //el cliente llego despues de 5s, se desconecta
         		// 	perror("close");
				printf("Me cerre :(\n");
				unsuscribe_client("P1", commands[1]);
				unsuscribe_client("P2", commands[1]);
				unsuscribe_client("P3", commands[1]);
				
				delete_Node_d(&disc_clients, commands[1]);
			}

			/****************WORKING**************/
		}
		else if(!is_in_list(single_clients, commands[1])){

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

/**
 * @brief suscribe client to a list using its IP, port and socket fd.
 * 
 * @param producer 
 * @param ip 
 * @param port 
 * @param clisockfd 
 */
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
}

void unsuscribe_client(char *producer, char* ip){

	if(strncmp(producer, "P1", 2) == 0){
		delete_node(&p1, ip);
	}
	else if(strncmp(producer, "P2", 2) == 0){
		delete_node(&p2, ip);
	}
	else if(strncmp(producer, "P3", 2) == 0){
		delete_node(&p3, ip);
	}
	printf("client erased from %s list\n", producer);
}

/**
 * @brief parse string into a char array
 * 
 * @param line 
 * @return char** 
 */
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

	printf("\n");
     
    return arr;
}

/**
 * @brief configure queue to receive messages from producers
 * 
 * @return int 
 */
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

/**
 * @brief send message to the right suscriber
 * 
 * @param producer 
 * @param msg 
 */
void send_to_suscribers(int producer, char msg[TAM]){

	if(producer == 1){
		
		if(!is_empty(p1)){
			
			printf("Sending to suscribers of p1...\n");
			char *wrapped  = wrap_in_frame(msg);
			send_in_list(p1, wrapped, producer);
			print_clients_list(p1);

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

	}
	else if(producer == 3){
		
		if(!is_empty(p3)){
			char *wrapped  = wrap_in_frame(msg);
			send_in_list(p3, wrapped, producer);
			printf("\n---printing p3 list----\n");
			print_clients_list(p3);

		}
	}
}

/**
 * @brief writes to client socket. 
 * 
 * @param p 
 * @param msg 
 * @param producer 
 */
void send_in_list(struct Node* p, char msg[TAM], int producer){

    while(p != NULL){
		if(is_in_disclist(disc_clients, p->ip)){
			add_disc_buff(disc_clients, msg);

		/*****WORKING******/
			print_disc_buffer(disc_clients);



		}
		else{

			if(write(p->cli_sock_fd, msg, TAM) < 0){
				perror("server: could't write message to suscriber\n");
				exit(EXIT_FAILURE);
			}
			send_to_log(p->ip, msg, producer);
			printf("Le escribi a p%d->ip = %s fd = %d\n",producer,p->ip, p->cli_sock_fd);
		}

        p = p->next;
    }
}

/**
 * @brief writes log file and control its size.
 * 
 * @param cli_ip 
 * @param msg 
 * @param producer 
 */
void send_to_log(char* cli_ip, char msg[TAM], int producer){

	if ((file = fopen("log.txt", "a")) == NULL) {
        
        perror("Can't open log.txt");
        exit(EXIT_FAILURE);
    }
	
	static int max_lines = 0;

	if(max_lines < 1000){
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

/**
 * @brief return the current date and time.
 * 
 * @return char* 
 */
char* time_stamp(){

    char *timestamp = (char *)malloc(sizeof(char) * 16);
    time_t ltime = time(NULL);
    struct tm *tm = localtime(&ltime);
 
    sprintf(timestamp,"%02d/%02d/%04d %02d:%02d:%02d", tm->tm_mday, tm->tm_mon, 
        tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
    return timestamp;
}

/**
 * @brief puts message into frame
 * 
 * @param msg 
 * @return char* frame
 */
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

/**
 * @brief Get the md5hash object
 * 
 * @param str 
 * @param digest 
 */

void get_md5hash(char *str, unsigned char digest[MD5_DIGEST_LENGTH]) {

    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, str, strlen(str));
    MD5_Final(digest, &ctx);
}

/**
 * @brief compress file into .zip
 * 
 */
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

/**
 * @brief in case of CTRL-C signal closes the file
 * 
 * @param signum 
 */
void signal_handler(int signum){

  printf("\nInside handler function  %d\n", signum);
  fclose(file);
  //signal(SIGINT,SIG_DFL);   // Re Register signal handler for default action
  exit(EXIT_FAILURE);
}