#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SLEEP_TIME 1

//Un productor que envía un mensaje 
//random con una tasa de X/segundos.

int main(){

	while(1){
		sleep(SLEEP_TIME);
		printf("P1:%d\n", rand());
	}
}