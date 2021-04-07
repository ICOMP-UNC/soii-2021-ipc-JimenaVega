/*! 
 * \author Jimena Vega
 * \date   2021
 * \section intro_sec Introduction
 * \section compile_sec Compilation
 * To compile this file you need to run "make" command.
 * This will be enough to start using the binary called "client".
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#define TAM 80

int main( int argc, char *argv[] ){
	int sockfd, servlen;
	ssize_t n;
	struct sockaddr_un serv_addr;
	char buffer[TAM];
	int terminar = 0;

	if ( argc < 2 ) {
		fprintf( stderr, "Input error. You must write server's socket name. Ex: %s <socket_name>\n", argv[0]);
		exit(0);
	}

	memset((char *)&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy( serv_addr.sun_path, argv[1]);
	servlen = (int) (strlen( serv_addr.sun_path) + sizeof(serv_addr.sun_family));

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		perror( "client: Socket creation error" );
		exit(1);
	}

 	if (connect( sockfd, (struct sockaddr *)&serv_addr,(socklen_t) servlen ) < 0 ) {
		perror("Client: connection error" );
		exit(1);
	}

	while(1){
		memset(buffer, '\0', TAM );
		printf("Messege to send: ");
		fgets(buffer, TAM-1, stdin);

		n = write( sockfd, buffer, strlen(buffer) );
		if ( n < 0 ) {
			perror( "Client: fail to write message to socket." );
			exit(1);
		}

		buffer[strlen(buffer)-1] = '\0';
		if( !strcmp( "fin", buffer ) ) {
			terminar = 1;
		}

		memset( buffer, '\0', TAM );
		n = read( sockfd, buffer, TAM );

		printf("Answer: %s\n", buffer);
		if( terminar ) {
				printf( "Client: end of execution.\n" );
				exit(0);
		}
	}
	return 0;
}
