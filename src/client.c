#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>

#define TAM 256
#define h_addr h_addr_list[0] //ver porque no esta tomando la de netdb.h

int main( int argc, char *argv[] ){

	int sockfd;
	ssize_t n_read,n_write;
	uint16_t puerto;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[TAM];

	if ( argc < 3 ) {
		fprintf( stderr, "You should enter: %s <host> <port>\n", argv[0]);
		exit( 0 );
	}

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
		exit( 1 );
	}

	while(1) {

		//memset( buffer, '\0', TAM );
		n_read = read(sockfd, &buffer, TAM);
		printf("Server msg for client :%s\n", buffer);

		if(n_read < 0){
			perror("Client: invalid read");
			exit(EXIT_FAILURE);
		}
		
		n_write = write(sockfd,"client ACK\n", TAM);
		if(n_write == -1){
			perror("Client: invalid write.");
			exit(0);
		}
		
			

	
		
	}
	return 0;
} 