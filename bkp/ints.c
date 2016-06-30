//https://www.youtube.com/watch?v=Jy_O8g9iiHI
// sudo g++ ints.c -o ints.sh `mysql_config --cflags` `mysql_config --libs` -L/usr/local/lib -lwiringPi
// Last modified by Javier Gonzalez Ferreyra
// Date 12 May 2016

// SENSORS DEFINITIONS										  QTY
// DHT SENSORS (TEMP - HUM - DPOINT) - LIBRARY REQUIRED 	- 6
// DOOR SENSORS - DETECTED by INTERRUPT - ACTIVE LOW		- 1 
// MAINS SENSORS - DETECTED by INTERRUPT - ACTIVE LOW		- 1  - 1 = NO AC
// SMOKE SENSORS - DETECTED by INTERRUPT - ACTIVE LOW		- 3

#define SMART_MONITOR_VER 0.1
#define MAXPORTS 28
#define INT_PORTS 4

unsigned int reading = 0;
unsigned int success = 0;

#define DEBUG TRUE
#include "db_conn.h"
#include "db_insert.c"
#include "db_update.c"

int updateStatus (int, bool);

unsigned int doorPorts 	= 23;
int led 				= 25;

bool flag 				= true;

float puerta_abierta 	= -10;
float puerta_cerrada 	= -20;

//unsigned int availablePorts [MAXPORTS]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,
//										15,16,21,22,23,24,25,26,27,28,29,30,31};
// Ports 12, 13, 14 son SPI pueden traer conflictos, no los uso
// Ports 15, 16 son UART y no los uso
										
// unsigned int activePorts []={0,1,2,3,4,5,6,7,8,9,10,11};

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

			digitalWrite (led,HIGH);
			delay (500);
			
			if (flag){
				timestamp ();
				checkInts ();
				flag = false;
			}
			
			digitalWrite (led,LOW);
			delay (500);
		}
			
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

	/*for (i=0 ; i < INT_PORTS ; i++)
		wiringPiISR (intPorts[i], INT_EDGE_FALLING, &func);*/
	wiringPiISR (doorPorts, INT_EDGE_BOTH, &func);

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
	//unsigned int i;
	//int size = sizeof (intPorts) / sizeof (int);
	
	pinMode (led, OUTPUT);

	pinMode (doorPorts, INPUT);
	pullUpDnControl (doorPorts, PUD_UP);
	/*for (i=0 ; i < size ; i++){
		pinMode( intPorts[i], INPUT);
		pullUpDnControl (intPorts[i], PUD_UP);
	}*/

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
	unsigned int i = 0;
	bool test = true;

	//for ( i=0 ; ((i < INT_PORTS) && (test)) ; i++ )
		test = digitalRead (doorPorts);   // IF HIGH DOOR OPEN
	
	if (test)	
		updateStatus (0,1);
	else
		updateStatus (0,0);
	
	return 0;
}

// ISR END

int updateStatus (int port, bool portStatus){
	
	//6 = HUMO - 21/22
	//5 = PRTA - 23 - NC - LOW
	//2 = TALT - 24
	
	//  DETECTA VALOR -10 => PUERTA ABIERTA	0
	// 	DETECTA VALOR -20 => PUERTA CERRADA 1
	
	
	if (!mysql_connect ()){
		
		if (portStatus){
			db_insert (5,0,puerta_abierta);
			printf ("PA\n");
		}
		else {
			db_insert (5,0,puerta_cerrada);
			printf ("PC\n");
		}
		db_update (5,0,0);
		mysql_disconnect ();
	}

	return 0;
}


// Function to convert sensor name and id to string
	
int resolvSensorID (short type, short id){
	short sid = id + 1;
	str[0] = '\0';	
	
	if (((type < 8) && (type >= 0)) && ((sid < 99) && (sid > 0))){
		strcat (str,sensorType[type]);
		sprintf (str+4, "%02d", sid);
	}
	else{
		printf ("Error Sensor Value type:%d / id:%d \n",type,sid);
		return 1;
	}
	return 0;
}
