//code by https://www.softprayog.in/programming/interprocess-communication-using-system-v-message-queues-in-linux
//server

/*
 * server.c: Server program
 *           to demonstrate interprocess commnuication
 *           with System V message queues
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
#define PROJECT_ID 'M'
#define QUEUE_PERMISSIONS 0666

struct message_text {
    int qid;
    char buf [200];
};

struct message {
    long message_type;
    struct message_text message_text;
};

int main (int argc, char **argv)
{
    key_t msg_queue_key;
    int qid;
    struct message message;

    if ((msg_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (1);
    }

    if ((qid = msgget (msg_queue_key, IPC_CREAT | QUEUE_PERMISSIONS)) == -1) {
        perror ("msgget");
        exit (1);
    }

    printf ("Server: Hello, World!\n");

    while (1){
        // read an incoming message
        if (msgrcv (qid, &message, sizeof (struct message_text), 0, IPC_NOWAIT) == -1) {
  
        }else{

        printf ("Server: message received: ");

        // process message
        int length = strlen (message.message_text.buf);

        printf("Client msg = %s\n", message.message_text.buf);
        // char buf [20];
        // sprintf (buf, " %d", length);
        // strcat (message.message_text.buf, buf);

        // int client_qid = message.message_text.qid;
        // message.message_text.qid = qid;
        }
    }
}
