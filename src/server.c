/*! 
 * \author Jimena Vega
 * \date   2021
 * \section intro_sec Introduction
 * \section compile_sec Compilation
 * To compile this file you need to run "make" command.
 * This will be enough to start using the binary called "serv".
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define TAM 80

int main(int argc, char *argv[]){
  int sockfd, newsockfd, servlen, clilen, pid;
  long int n;

  struct sockaddr_un  cli_addr, serv_addr;
  char buffer[TAM];
  
  
  if( argc != 2 ) {
    printf( "Input error. You must name the socket. Ex: %s <socket_name>\n", argv[0] );
    exit(1);
  }
  
  if ( ( sockfd = socket( AF_UNIX, SOCK_STREAM, 0) ) < 0 ) {
    perror( "Server: socket creation error");
    exit(1);
  }
  
  
  unlink ( argv[1] );
  
  memset( &serv_addr, 0, sizeof(serv_addr) );
  serv_addr.sun_family = AF_UNIX;
  strcpy( serv_addr.sun_path, argv[1] );
  servlen = (int)(strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family));
  
  if(bind(sockfd,(struct sockaddr *)&serv_addr,(unsigned int)servlen )<0 ) {
    perror("Server: bind error"); 
    exit(1);
  }
  
  printf("Proccess: %d - available socket: %s\n", getpid(), serv_addr.sun_path );
  
  listen( sockfd, 5 );
  clilen = sizeof( cli_addr );
  
  while(1){
    newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr,(socklen_t*) &clilen );   
        
    pid = fork();
        
    if ( pid == 0 ){    
		close( sockfd );
			
		while(1) { 
			memset( buffer, 0, TAM );
			
			n = read( newsockfd, buffer, (size_t)(TAM-1 ));

			if(n < 0){
				printf("\nServer: error trying to read the buffer: %s\n", strerror(errno));
			}
			
			printf( "PROCESS: %d. ", getpid() );
			printf( "Server received: %s", buffer );
			
			n = write( newsockfd, "Server: I got ur msg", 21 );

			if(n < 0){
				printf("\nServer: Error trying to write the buffer: %s\n", strerror(errno));
			}
			
			buffer[strlen(buffer)-1] = '\0';
			if( !strcmp( "fin", buffer ) ) {
				printf( "PROCESS %d. I received'fin', so bye bye.\n\n", getpid() );
				exit(0);
			}
		}
	}
    else {
		printf( "SERVER: New client, attended by child process: %d\n", pid );
		close( newsockfd );
      }
    }
  return 0;
}
