#ifndef _LIBSERV_
#define _LIBSERV_

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

/*System V queue message*/
struct message {
    long message_type;
    struct message_text message_text;
};

extern int config_socket(uint16_t port);
extern void message_interpreter(char buffer[TAM], int clisockfd);
extern void suscribe_client(char *producer, char* ip, int port, int clisockfd);
extern void unsuscribe_client(char *producer, char* ip);
extern void send_to_suscribers(int producer, char msg[TAM]);
extern char** parse_string(char* line);
extern char* wrap_in_frame(char msg[TAM]);
extern void get_md5hash(char *str, unsigned char digest[MD5_DIGEST_LENGTH]);
extern void send_in_list(struct Node* p, char msg[TAM], int producer);
extern void send_to_log(char* cli_ip, char msg[TAM], int producer);
extern int config_queue();
extern char *time_stamp();
extern void signal_handler(int signum);
extern void compress_file();

struct epoll_event ev, events_array[MAX_EVENTS];
int listen_sock, client_sock, ready_fds, epoll_fd;

struct Node* single_clients;
struct Node* p1;
struct Node* p2;
struct Node* p3;


FILE* file;


#endif