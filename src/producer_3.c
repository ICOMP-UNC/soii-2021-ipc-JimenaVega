#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int get_CPUs();

int main(){

    printf("CPUs = %d", get_CPUs());
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