#ifndef _LIBVAR_
#define _LIBSVAR_
//global variables

struct epoll_event ev, events_array[MAX_EVENTS];
int listen_sock, client_sock, ready_fds, epoll_fd;

struct Node* single_clients = NULL;
struct Node* p1 = NULL;
struct Node* p2 = NULL;
struct Node* p3 = NULL;


FILE* file = NULL;

#endif