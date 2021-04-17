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
#define Q_PERMISSIONS 0660

struct message_text {
    int id;
    char buf [200];
};

struct message {
    long message_type;
    struct message_text message_text;
};

char* generate_msg();
int config_q();


struct message p2_message;

int main(){

	int serv_qid = config_q();

    while(1){
        sleep(SLEEP_TIME);
		strcpy(p2_message.message_text.buf, generate_msg());

		printf("freemem = %s\n",p2_message.message_text.buf);

		int len = (int) strlen(p2_message.message_text.buf);
		if(p2_message.message_text.buf[len-1] == '\n'){
			p2_message.message_text.buf[len-1] = '\0';
		}

        if(msgsnd(serv_qid, &p2_message, sizeof(struct message_text),0) == -1){
			perror("P2: msgsnd error");
			exit(EXIT_FAILURE);
		}

    }

    return 0;
}

char* generate_msg(){

    FILE *file = fopen("/proc/meminfo","r");
    char buffer[100];
    char *token;
    char *token_copy;

    if(file == NULL){
        printf("Error opening file");
        exit(1);
    }

    while(fgets(buffer,100,file)){
        if(strncmp(buffer, "MemFree", 7)==0){
            token = strtok(buffer,":");
            token = strtok(NULL,":");
            token = strtok(token," ");

            token_copy = strdup(token);
        }
    }


	return token_copy;

}

int config_q(){

	key_t serv_q_key;
	int serv_qid;

	if((serv_q_key = ftok (KEY_PATHNAME, PROJECT_ID)) == -1){
		perror("error in P2 ftok");
		exit(1);
	}
	if((serv_qid = msgget(serv_q_key, 0)) == -1){
		perror("error in P2 msget");
		exit(1);
	}

	p2_message.message_type = 1;
	p2_message.message_text.id = 2;

	return serv_qid;
}

