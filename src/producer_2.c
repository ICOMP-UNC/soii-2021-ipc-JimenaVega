#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SLEEP_TIME 2

int main(){
    while(1){
        sleep(SLEEP_TIME);
        FILE *file = fopen("/proc/meminfo","r");
        char buffer[100];
        char *token;

        if(file == NULL){
            printf("Error opening file");
            exit(1);
        }

        while(fgets(buffer,100,file)){
            if(strncmp(buffer, "MemFree", 7)==0){
                token = strtok(buffer,":");
                token = strtok(NULL,":");
                token = strtok(token," ");
                printf("P2:%skB",token);
            }
        }
    }


    return 0;
}