#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define Q_NORM_LOAD 3

int get_CPUs();
void get_load(float load_avg[Q_NORM_LOAD]);

int main(){

    float load_avg[Q_NORM_LOAD];
    int norm_load[Q_NORM_LOAD];
    float n_cpus = (float)get_CPUs();
    get_load(load_avg);

    for(int i=0; i<Q_NORM_LOAD; i++){
        norm_load[i] = (int)((load_avg[i]/n_cpus)*100);
    }

    for(int i=0; i<Q_NORM_LOAD;i++){
        printf("norm_load [%d] = %d%%\n",i,norm_load[i]);
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