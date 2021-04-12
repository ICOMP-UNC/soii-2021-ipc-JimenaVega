#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>

#define TAM 256

int main( int argc, char *argv[] ){

	int sockfd,n;
	uint16_t puerto;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int terminar = 0;
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
		printf( "Ingrese el mensaje a transmitir: " );
		memset( buffer, '\0', TAM );
		fgets( buffer, TAM-1, stdin );

		n = (int) write( sockfd, buffer, strlen(buffer) );

		if(n == -1){
			perror("Invalid write.");
			exit(0);
		}
		// Verificando si se escribió: fin
		buffer[strlen(buffer)-1] = '\0';
		if( !strcmp( "fin", buffer ) ) {
			terminar = 1;
		}

		memset( buffer, '\0', TAM );
		n = (int)read( sockfd, buffer, TAM );
		printf( "Answer: %s\n", buffer );

		if(terminar) {
			printf("End of execution\n" );
			exit(0);
		}
	}
	return 0;
} 