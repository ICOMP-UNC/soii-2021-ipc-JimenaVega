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
#define h_addr h_addr_list[0] //ver porque no esta tomando la de netdb.h


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

		//I need checksum control here
		n_read = read(sockfd, &buffer, TAM);
		printf("Server msg for client: %s\n", buffer);

		if(n_read < 0){
			perror("Client: invalid read");
			exit(EXIT_FAILURE);
		}
		//formato = ACK <my-ip> <port>
		char to_send [TAM];
        sprintf (to_send, "ACK %s %s", argv[1], argv[3]);
		printf("client to_send = %s\n", to_send);
		n_write = write(sockfd,to_send, TAM);
		if(n_write == -1){
			perror("Client: invalid write.");
			exit(0);
		}
		
	}
	return 0;
} 