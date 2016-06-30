//https://www.youtube.com/watch?v=Jy_O8g9iiHI
/////////////////////////////////////////////////////////////////////////////////////////////////////
// COMPILE FLAGS 
// sudo g++ debug.c -o debug.sh `mysql_config --cflags` `mysql_config --libs` -L/usr/local/lib -lwiringPi
/////////////////////////////////////////////////////////////////////////////////////////////////////
// Last modified by Javier Gonzalez Ferreyra
// Date 19 May 2016 - 11:05
//
// SENSORS DEFINITIONS										  QTY
// DHT SENSORS (TEMP - HUM - DPOINT) - LIBRARY REQUIRED 	- 6
////////////////////////////////////////////////////////////////////////////////////////////////////
// debug.c es exclusivo para sensores de H/T/DP, 
//
// Script to run ./debug.sh

// TODO 
// MAX 255 - VER
// RESPAWN
// 220v asegurar qe tenga bajos
// VER PERFOMANCE
////////////////////////////////////////////////////////////////////////////////////////////////////

#define SMART_MONITOR_VER 	0.1
#define MAXPORTS 			28
#define MAX_ERRORS 			15

unsigned int reading 	= 0;
unsigned int success 	= 0;
float volts 			= 220;
float humo 				= 20;

#define DEBUG TRUE
#include "db_conn.h"
#include "db_insert.c"
#include "db_update.c"

FILE *fr;

//unsigned int availablePorts [MAXPORTS]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,
//										15,16,21,22,23,24,25,26,27,28,29,30,31};
// Ports 12, 13, 14 ,8 , 9 son I2C-SPI pueden traer conflictos, no los uso
// Ports 15, 16 son UART y no los uso
// Ports 0, 10, 11 , +28 disponibles para extensiones
										
unsigned int activePorts []={0,2,3,4,5,6};

unsigned int smokePorts []={21,22};

unsigned int mainsPorts []={24};

/////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++++++ FILE READ FUNCTION +++++++++++++++++++++++++++++++++
/////////////////////////////////////////////////////////////////////////////////

int fileRead (void){

fr = fopen("/home/pi/SmartMonitor/input_data", "r+");

	if( fr == NULL ){
		
		printf("Error while opening the file.\n");
		fr = fopen("/home/pi/SmartMonitor/input_data", "w+");
		printf("New File has been created empty.\n");
		
	}
	else
		printf("File Open.\n");

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////// START OF SETUP ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

int setup (void){
	
	if (!(initWiringPi()))	// MUST BE CALLED FIRST-BEFORE EVERYTHING PIN RELATED
		return 0;
	else
		return 1;
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// END OF SETUP ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////// START OF MAIN ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

int main (int argc, char *argv[]){
	
	if (!(setup())){
		delay (2000);
		//while (TRUE){
			timestamp();
			readActivePorts();
			printf("Reading :%d\n", reading);
			printf("Success :%d\n", success);
			timestamp();
			printf("---------------------------------------------\n");
			//delay (5000);
		//}
	}
	else
		return 1;
}
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////// END OF MAIN //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++ INIT WIRINGPI FUNCTION +++++++++++++++++++++++++++++++++
/////////////////////////////////////////////////////////////////////////////////

int initWiringPi (void){

	if (wiringPiSetup () < 0){
		timestamp();
		printf ( "Error unable to setup wiringPi: %s\n", strerror (errno));
		return 1 ;
	}
	
	setPinMode (21,1,1);	//SMOKE
	setPinMode (22,1,1);	//SMOKE
	//setPinMode (23,1,1);	//DOOR
	setPinMode (24,1,1);	//MAINS
  
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////
//----------------------- END WIRINGPI FUNCTION ---------------------------------
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++ INIT SETPIN MODE FUNCTION ++++++++++++++++++++++++++++++
/////////////////////////////////////////////////////////////////////////////////
// This function setup the  pinmode for smoke & mains
// PIN : PIN NUMBER TO SETUP
// TYPE : INPUT - OUTPUT
// MODE : ONLY FOR INPUT PULL UP OR PULL DOWN
// RETURN : 0

int setPinMode (int pin, int type, bool mode){
	
	if  (type) {
		pinMode (pin, INPUT);
		if (mode)
			pullUpDnControl (pin, PUD_UP);
		else
			pullUpDnControl (pin, PUD_DOWN);
	}
	else
		pinMode (pin , OUTPUT);	
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////
//----------------------- END SETPIN MODE FUNCTION ------------------------------
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++ START READ PORTS FUNCTION ++++++++++++++++++++++++++++++
/////////////////////////////////////////////////////////////////////////////////
int readActivePorts (void){
	
	int size = sizeof (activePorts)/ sizeof (int);
	int result; 
	int lcontador = 0;
	
	if (!mysql_connect ()){
								
		if (!digitalRead (24))
			volts = 0;
		
		db_insert (2,0,volts);
		db_update (2,0,0);
		
		if (!digitalRead (21))	
			humo = 70;
		
		db_insert (6,0,humo);
		db_update (6,0,0);
		humo = 20;
		
		if (!digitalRead (22))
			humo = 70;
		
		db_insert (6,1,humo);
		db_update (6,1,0);

		fileRead ();

		for  (int i=0 ; ((i < MAXPORTS) && (i < size)) ; i++){ 
			
			result = displayDHTData (activePorts[i]);
			if(!result){
	
				db_insert (0,i,temperature);
				db_update (0,i,0);
				fprintf(fr, "%2.1f\t", temperature);

				db_insert (1,i,humidity);
				db_update (1,i,0);
				fprintf(fr, "%2.1f\t", humidity);

				db_insert (4,i,dewPoint);
				db_update (4,i,0);
				fprintf(fr, "%2.1f\n", dewPoint);
				//lcontador = 0;
					
			}
			//else {
				//if ((result == 20) && (lcontador < MAX_ERRORS)){
				//	++lcontador;
				//	--i;
				//}
			else if (result == 20){
				
				for (int j = 1; j < i;  j++){					
					fscanf(fr, "%*f %*f %*f");
					//printf ("J:%d\n",j);
					//printf ("I:%d\n",i);
				}
					
				fscanf(fr, "%f %f %f", &temperature, &humidity, &dewPoint);

				fprintf(stderr, "New value T %2.1f\n", temperature);
				fprintf(stderr, "New value H %2.1f\n", humidity);
				fprintf(stderr, "New value D %2.1f\n", dewPoint);
					
				db_insert (0,i,temperature);
				db_update (0,i,0);
					
				db_insert (1,i,humidity);
				db_update (1,i,0);
				
				db_insert (4,i,dewPoint);
				db_update (4,i,0);
				//	lcontador = 0;
			}
			else if (result == 10){
					db_update (0,i,result);
					db_update (1,i,result);
					db_update (4,i,result);
					//lcontador = 0;
			}
		}
		//fflush (fr);
		fclose (fr);
		printf("File Closed\n");
		mysql_disconnect ();
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////
//----------------------- END READ PORTS FUNCTION -------------------------------
/////////////////////////////////////////////////////////////////////////////////
	
/////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++ START RESOLV PORT NAME FUNCTION ++++++++++++++++++++++++++++
/////////////////////////////////////////////////////////////////////////////////
// Function to convert sensor name and id to string
	
int resolvSensorID (short type, short id){
	short sid = id + 1;
	str[0] = '\0';	
	
	if (((type < 8) && (type >= 0)) && ((sid < 99) && (sid > 0))){
		strcat (str,sensorType[type]);
		sprintf (str+4, "%02d", sid);
		//printf ("%s\n",str);
	}
	else{
		printf ("Error Sensor Value type:%d / id:%d \n",type,sid);
		return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////
//--------------------- END RESOLV PORTS FUNCTION -------------------------------
/////////////////////////////////////////////////////////////////////////////////

//*******************************************************************************
//****************************** END C SCRIPT ***********************************
//*******************************************************************************
