#ifndef _LIBSERV_
#define _LIBSERV_

#include "../inc/lib_glob_struct.h"

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


#endif