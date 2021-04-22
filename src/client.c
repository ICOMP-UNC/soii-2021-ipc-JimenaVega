#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/md5.h>

#define TAM 256
#define h_addr h_addr_list[0] 


int translate_hash(char buffer[TAM]);
char* get_hash(char* msg);
void compute_md5(char *str, unsigned char digest[MD5_DIGEST_LENGTH]);

//argv[1] = <my-ip>
//argv[2] = <server-ip>
//argv[3] = <port>

//en ACK se manda mi ip y port
int main( int argc, char *argv[] ){

	int sockfd;
	ssize_t n_read,n_write;
	uint16_t puerto;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[TAM];

	if ( argc < 4 ) {
		fprintf( stderr, "You should enter: %s <my-ip> <server-ip> <port>\n", argv[0]);
		exit( 0 );
	}

	puerto = (uint16_t)atoi(argv[3]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server = gethostbyname(argv[2]);
	//gethostbyname("192.168.100.7");

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

	while(1) {

		n_read = read(sockfd, &buffer, TAM);

		if(n_read < 0){
			perror("Client: invalid read");
			exit(EXIT_FAILURE);
		}
		else if(n_read > 0){
			
			printf("R: %s\n", buffer);

			if(translate_hash(buffer)){
				//formato = ACK <my-ip> <port>
				char to_send [TAM];
				sprintf (to_send, "ACK %s %s", argv[1], argv[3]);
				printf("S: %s\n", to_send);

				n_write = write(sockfd,to_send, TAM);

				if(n_write == -1){
					perror("Client: invalid write.");
					exit(EXIT_FAILURE);
				}
			}
			else{
				printf("Error. Unrecognized hash\n");
			}
		}

	}
	return 0;
} 

int translate_hash(char buffer[TAM]){

	//both come with the message from server
	char* original_msg;
	char* original_hash = strtok(buffer, " ");

	original_hash = strtok(NULL, " ");
	original_msg = strdup(original_hash);

	original_hash = strtok(NULL, " ");

	//the one that I compute to compare
	char* hash = get_hash(original_msg);

	if(strncmp(hash, original_hash, 32) == 0){
        return 1;
    }
    return 0;
}

char* get_hash(char* msg){

	char* result = malloc(33 * sizeof(char));
	char new_buff[(MD5_DIGEST_LENGTH * 2) + 1];
	unsigned char digest[MD5_DIGEST_LENGTH];

	compute_md5(msg, digest);

	for(int i = 0, j = 0; i < MD5_DIGEST_LENGTH; i++, j+=2){
		sprintf(new_buff+j, "%02x", digest[i]);
	}

	sprintf(result, "%s", new_buff);

	return result;
}

void compute_md5(char *str, unsigned char digest[MD5_DIGEST_LENGTH]){

    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, str, strlen(str));
    MD5_Final(digest, &ctx);
}
