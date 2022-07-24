#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#define SLEEP_TIME 2
#define KEY_PATHNAME "/tmp/mqueue_server_key"
#define PROJECT_ID 'M'
#define Q_PERMISSIONS 0666
//Un productor que envía un mensaje 
//random con una tasa de X/segundos.

struct message_text {
    int id;
    char buf [200];
};

struct message {
    long message_type;
    struct message_text message_text;
};

int config_q();

struct message p1_message;

int main(){

	int serv_qid = config_q();

	while(1){
		sleep(SLEEP_TIME);
		

		char buf [20];
        sprintf (buf, "%d", rand());
        strcpy (p1_message.message_text.buf, buf);
		printf("P1:%s\n",p1_message.message_text.buf);

		if(msgsnd(serv_qid, &p1_message, sizeof(struct message_text),IPC_NOWAIT) == -1){
			perror("P1: msgsnd error");
			exit(EXIT_FAILURE);
		}

        bzero(buf, 20);

	}

	return 0;
}

int config_q(){

	key_t serv_q_key;
	int serv_qid;

	if((serv_q_key = ftok (KEY_PATHNAME, PROJECT_ID)) == -1){
		perror("error in P1 ftok");
		exit(1);
	}
	if((serv_qid = msgget(serv_q_key, Q_PERMISSIONS)) == -1){
		perror("error in P1 msget");
		exit(1);
	}

	p1_message.message_type = 1;
	p1_message.message_text.id = 1;

	return serv_qid;
}