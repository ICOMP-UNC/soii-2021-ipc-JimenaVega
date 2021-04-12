#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#define TAM 257
#define MAX_EVENTS 10

struct epoll_event ev, events_array[MAX_EVENTS];
int listen_sock, conn_sock, ready_fds, epoll_fd;
//nfds = ready_fds
int main( int argc, char *argv[] ) {
	int sock_fd, newsockfd, pid;
	socklen_t clilen;
	ssize_t n;
	uint16_t puerto;
	char buffer[TAM];
	struct sockaddr_in serv_addr, cli_addr;

	if (argc < 2) {
    	fprintf( stderr, "You should write: %s <port>\n", argv[0] );
		exit(1);
	}

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	puerto = (uint16_t) atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(puerto);

	if (bind(sock_fd, (struct sockaddr *) &serv_addr, sizeof( serv_addr )) < 0 ) {
		perror("Server: error in binding fd with address");
		exit(1);
	}

	printf("Proceso: %d - socket disponible: %d\n", getpid(), ntohs(serv_addr.sin_port) );

	listen(sock_fd, 5);
	clilen = sizeof(cli_addr);//??

	// epoll_fd = epoll_create1(0);//??ver si 0
	// if(epoll_fd == -1){
	// 	perror("Error in epoll_create1()");
	// 	exit(EXIT_FAILURE);
	// }




	// while(1){
	// 	ready_fds = epoll_wait(epoll_fd, events_array,)
	// }



	while( 1 ) {
		newsockfd = accept( sock_fd, (struct sockaddr *) &cli_addr,&clilen );

		pid = fork(); 

		if ( pid == 0 ){ 
		    close( sock_fd );
		    
		    while(1){
			memset( buffer, 0, TAM );
			
			n = (ssize_t)read( newsockfd, buffer, TAM-1 );
			if ( n < 0 ) {
			  perror( "lectura de socket" );
			  exit(1);
			}
			
			printf( "PROCESO %d. ", getpid() );
			printf( "Recibí: %s", buffer );
			
			n = write( newsockfd, "Obtuve su mensaje", 18 );
			if ( n < 0 ) {
			  perror( "escritura en socket" );
			  exit( 1 );
			}
			// Verificación de si hay que terminar
			buffer[strlen(buffer)-1] = '\0';
			if( !strcmp( "fin", buffer ) ) {
			  printf( "PROCESO %d. Como recibí 'fin', termino la ejecución.\n\n", getpid() );
			  exit(0);
			}
		      }
		}
		else{
			printf( "SERVIDOR: Nuevo cliente, que atiende el proceso hijo: %d\n", pid );
			close( newsockfd );
		}
	}
	return 0; 
} 