#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SLEEP_TIME 6
#define Q_NORM_LOAD 3
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

int get_CPUs();
void get_load(float load_avg[Q_NORM_LOAD]);
int config_q();

struct message p3_message;

int main(){

    int serv_qid = config_q();

    int norm_load[Q_NORM_LOAD];
    float load_avg[Q_NORM_LOAD];
    float n_cpus = (float)get_CPUs();
    

    while(1){
        sleep(SLEEP_TIME);

        get_load(load_avg);

        for(int i=0; i<Q_NORM_LOAD; i++){
            norm_load[i] = (int)((load_avg[i]/n_cpus)*100);
        }

        char bbf[60] = "";
        for(int i=0; i<Q_NORM_LOAD;i++){
            char buf [20];
            sprintf (buf, "%d ", norm_load[i]);
            strcat (bbf, buf);
            
        }
        //is this ok? controlar p1 too
        strcpy(p3_message.message_text.buf, bbf);
        printf("P3: norm_load  = %s%%\n",bbf);

        if(msgsnd(serv_qid, &p3_message, sizeof(struct message_text),IPC_NOWAIT) == -1){
			perror("P3: msgsnd error");
			exit(EXIT_FAILURE);
		}

        bzero(bbf, 60);
        printf(" P3 sigo aki");
    }

    return 0;
}

int get_CPUs(){

    char buffer[100];

    FILE *file = fopen("/proc/cpuinfo","r");
    int CPU_quant = 0;

    if(file == NULL){
        perror("Error opening the file. ");
        exit(EXIT_FAILURE);
    }

    while(fgets(buffer,100,file) != NULL){
        if(strncmp(buffer, "processor",9) == 0){
            CPU_quant++;
        }
    }
    fclose(file);
    
    return CPU_quant;
}

void get_load(float load_avg[Q_NORM_LOAD]){

    FILE *file = fopen("/proc/loadavg", "r");
    char buffer[100];

    fgets(buffer,100,file);
    load_avg[0] = strtof(strtok(buffer, " "),NULL);

    for(int i = 1; i < Q_NORM_LOAD; i++){
        load_avg[i] = strtof(strtok(NULL, " "),NULL);    
    }
}

int config_q(){

	key_t serv_q_key;
	int serv_qid;

	if((serv_q_key = ftok (KEY_PATHNAME, PROJECT_ID)) == -1){
		perror("error in P3 ftok");
		exit(1);
	}
	if((serv_qid = msgget(serv_q_key, Q_PERMISSIONS)) == -1){
		perror("error in P3 msget");
		exit(1);
	}

	p3_message.message_type = 3;
	p3_message.message_text.id = 3;

	return serv_qid;
}
