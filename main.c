//https://www.youtube.com/watch?v=Jy_O8g9iiHI
// sudo g++ main.c -o sm.sh `mysql_config --cflags` `mysql_config --libs` -L/usr/local/lib -lwiringPi
// Last modified by Javier Gonzalez Ferreyra
// Date 12 May 2016

// SENSORS DEFINITIONS										  QTY
// DHT SENSORS (TEMP - HUM - DPOINT) - LIBRARY REQUIRED 	- 6
// DOOR SENSORS - DETECTED by INTERRUPT - ACTIVE LOW		- 1
// MAINS SENSORS - DETECTED by INTERRUPT - ACTIVE LOW		- 1
// SMOKE SENSORS - DETECTED by INTERRUPT - ACTIVE LOW		- 3


#define SMART_MONITOR_VER 0.1
#define MAXPORTS 28
#define INT_PORTS 4

#define DEBUG TRUE
#include "db_conn.h"
#include "db_insert.c"
#include "db_update.c"

// TIMING FOR SLEEPING
unsigned long interval				= 60000;
unsigned long previousMillis		= 0;		// millis() returns an unsigned long.

unsigned int intPorts []={21,22,23,24,25};

bool flag = false;

//unsigned int availablePorts [MAXPORTS]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,
//										15,16,21,22,23,24,25,26,27,28,29,30,31};
// Ports 12, 13, 14 son SPI pueden traer conflictos, no los uso
// Ports 15, 16 son UART y no los uso
										
unsigned int activePorts []={0,1,2,3,4,5,6,7,8,9,10,11};

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////// START OF SETUP ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
int setup (void){
	if (!(initWiringPi())) 	// MUST BE CALLED FIRST-BEFORE EVERYTHING PIN RELATED
		intSetup();			// SETUP INTERRUPTS AND PULL UP PORT DEFINITIONS
	else
		return 1;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// END OF SETUP ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////START OF MAIN /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

int main (int argc, char *argv[]){
	
	if (!(setup())){
		
		while (TRUE){ // <---- MAIN LOOP 
		//resolvSensorID (0,1);
			//if (flag)
			if ((unsigned long)(millis() - previousMillis) >= interval) {
				previousMillis = millis();
			}			
			
			timestamp();
			readActivePorts();
			short a;
			for ( a=0 ; a<8 ; a++)
				resolvSensorID (a,a);
			delay (5000);
		}
		
		/*if (!mysql_connect ()){
			db_insert (1,1,21.3);
			//db_update ();
			mysql_disconnect ();
		}*/
	}
	else
		return 1;
}
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////// END OF MAIN //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++++ DOOR CHECK FUNCTION ++++++++++++++++++++++++++++++++++
/////////////////////////////////////////////////////////////////////////////////
//  DETECTA VALOR -10 => PUERTA ABIERTA	0
// 	DETECTA VALOR -20 => PUERTA CERRADA 1





/////////////////////////////////////////////////////////////////////////////////
//----------------------- END DOOR CHECK FUNCTION -------------------------------
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++ INTERRUPT SETUP FUNCTION ++++++++++++++++++++++++++++++
/////////////////////////////////////////////////////////////////////////////////
// ISR START
// Function created to setup ISR 
// Once Interrupt occurs will trigger the function &func

int intSetup (void){
	unsigned int i;
	
	setPinMode();

	for (i=0 ; i < INT_PORTS ; i++)
		wiringPiISR (intPorts[i], INT_EDGE_FALLING, &func);

	return 0;
} 

/////////////////////////////////////////////////////////////////////////////////
//----------------------- END INTERRUPT SETUP FUNCTION --------------------------
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++ PIN PUD SETUP FUNCTION +++++++++++++++++++++++++++++++++
/////////////////////////////////////////////////////////////////////////////////
//USAGE of FUNCTION - void pullUpDnControl (int pin, int pud) ;

int setPinMode (void){
	unsigned int i;
	int size = sizeof (activePorts)/ sizeof (int);

	for (i=0 ; i < size ; i++)
		pinMode( activePorts[i], INPUT);
	

	for (i=0 ; i <= INT_PORTS ; i++)
		pullUpDnControl (intPorts[i], PUD_UP);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////
//----------------------- END PIN PUD SETUP FUNCTION ----------------------------
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++ INIT WIRINGPI FUNCTION +++++++++++++++++++++++++++++++++
/////////////////////////////////////////////////////////////////////////////////

int initWiringPi (void){

	if (wiringPiSetup () < 0){
		timestamp();
		printf ( "Unable to setup wiringPi: %s\n", strerror (errno));
		return 1 ;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////
//----------------------- END WIRINGPI FUNCTION ---------------------------------
/////////////////////////////////////////////////////////////////////////////////

void func (void){
	
	flag = true;

}

int checkInts (void){
	unsigned int i,pin = 0;

	for (i=0 ; i < INT_PORTS ; i++){
		
		if (!digitalRead (intPorts[i]))
			++pin;
	}	
	return pin;
}

// ISR END
// Funciones para medir el puerto (DOOR + SMOKE)


int readActivePorts (void){
	int size = sizeof (activePorts)/ sizeof (int);
	for  (int i=0 ; ((i < MAXPORTS) && (i < size)) ; i++){ 
		//printf ("%d \n",activePorts[i]);
		
		if(!displayDHTData (activePorts[i])){
			if (!mysql_connect ()){
				
				db_insert (0,i,temperature);
				db_update (0,i,0);

				db_insert (1,i,humidity);
				db_update (1,i,0);

				db_insert (4,i,dewPoint);
				db_update (4,i,0);
				
				mysql_disconnect ();
			}
		}
		else{
			if (!mysql_connect ()){

				db_update (0,i,10);

				db_update (1,i,10);

				db_update (4,i,10);
				
				mysql_disconnect ();
			}
		}
	}

	return 0;
}

// Function to convert sensor reading to string

int resolvSensorReading (long reading){
	
	return 0;
}

	
// Function to convert sensor name and id to string
	
int resolvSensorID (short type, short id){
	short sid = id + 1;
	str[0] = '\0';	
	
	if (((type < 8) && (type >= 0)) && ((sid < 99) && (sid > 0))){
		strcat (str,sensorType[type]);
		sprintf (str+4, "%02d", sid);
		printf ("%s\n",str);
	}
	else{
		printf ("Error Sensor Value type:%d / id:%d \n",type,sid);
		return 1;
	}
	return 0;
}
