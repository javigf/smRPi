// sudo g++ test.c -o test.sh -L/usr/local/lib -lwiringPi

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <wiringPi.h>

unsigned int activePorts [10]={0,2,3,4,5,6,21,22,23,24}; 		//-- PINS

int main (int argc, char *argv[]){
	
	if (wiringPiSetup () < 0){

		printf ( "Error unable to setup wiringPi: %s\n", strerror (errno));
		return 1 ;
	}
	
	for (int i=0 ; i<10 ; i++)
			pinMode (activePorts[i] , OUTPUT);
	
	while (TRUE){
			
		for (int i=0 ; i<10 ; i++){
			digitalWrite (activePorts[i],LOW);
			delay (50);
		}	
		printf ("ALL LOW\n");
		for (int i=0 ; i<10 ; i++){
			digitalWrite (activePorts[i],HIGH);
			delay (50);
		}
		printf ("ALL HIGH\n");
	}
	return 0;
}
