//  mysql --user=usrsensor --password=sm.m0n1t0r smartmonitor
// UPDATE smartmonitor.sensors SET id_status = 3 WHERE id = 'TEMP01';
/*
 +-----+-----+---------+------+---+---Pi 2---+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 |     |     |    3.3v |      |   |  1 || 2  |   |      | 5v      |     |     |
 |   2 |   8 |   SDA.1 |   IN | 1 |  3 || 4  |   |      | 5V      |     |     |
 |   3 |   9 |   SCL.1 |   IN | 1 |  5 || 6  |   |      | 0v      |     |     |
 |   4 |   7 | GPIO. 7 |   IN | 0 |  7 || 8  | 1 | IN   | TxD     | 15  | 14  |
 |     |     |      0v |      |   |  9 || 10 | 1 | IN   | RxD     | 16  | 15  |
 |  17 |   0 | GPIO. 0 |   IN | 0 | 11 || 12 | 1 | IN   | GPIO. 1 | 1   | 18  |
 |  27 |   2 | GPIO. 2 |   IN | 0 | 13 || 14 |   |      | 0v      |     |     |
 |  22 |   3 | GPIO. 3 |   IN | 0 | 15 || 16 | 0 | IN   | GPIO. 4 | 4   | 23  |
 |     |     |    3.3v |      |   | 17 || 18 | 0 | IN   | GPIO. 5 | 5   | 24  |
 |  10 |  12 |    MOSI |   IN | 0 | 19 || 20 |   |      | 0v      |     |     |
 |   9 |  13 |    MISO |   IN | 0 | 21 || 22 | 0 | IN   | GPIO. 6 | 6   | 25  |
 |  11 |  14 |    SCLK |   IN | 0 | 23 || 24 | 1 | IN   | CE0     | 10  | 8   |
 |     |     |      0v |      |   | 25 || 26 | 1 | IN   | CE1     | 11  | 7   |
 |   0 |  30 |   SDA.0 |   IN | 1 | 27 || 28 | 1 | IN   | SCL.0   | 31  | 1   |
 |   5 |  21 | GPIO.21 |   IN | 1 | 29 || 30 |   |      | 0v      |     |     |
 |   6 |  22 | GPIO.22 |   IN | 1 | 31 || 32 | 0 | IN   | GPIO.26 | 26  | 12  |
 |  13 |  23 | GPIO.23 |   IN | 0 | 33 || 34 |   |      | 0v      |     |     |
 |  19 |  24 | GPIO.24 |   IN | 0 | 35 || 36 | 0 | IN   | GPIO.27 | 27  | 16  |
 |  26 |  25 | GPIO.25 |   IN | 0 | 37 || 38 | 0 | IN   | GPIO.28 | 28  | 20  |
 |     |     |      0v |      |   | 39 || 40 | 0 | IN   | GPIO.29 | 29  | 21  |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+---Pi 2---+---+------+---------+-----+-----+
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <mysql.h>
#include <time.h>
// DHT Library WiringPi based
#include "read_dht.c"
#include "util.c"

/*
typedef int bool;
#define true 1
#define false 0
*/

#define DATABASE_NAME		"smartmonitor"
#define DATABASE_USERNAME	"usrsensor"
#define DATABASE_PASSWORD	"sm.m0n1t0r"
#define DATABASE_SERVER		"localhost"

char str[8];

MYSQL *mysql1;

char const * sensorType[] = {"TEMP","HDAD","TALT","TCON","PCON","PRTA","HUMO"};
short sensorID = 0;

// SETUP FUNCTIONS
int initWiringPi (void);
int setPinMode (void);
int setPinMode (int, int, bool);
int intSetup (void);
int setup (void);

// DB FUNCTIONS
int db_insert (short, short, float);
int db_update (short, short, short);
int mysql_connect (void);
int mysql_disconnect (void);

// DHT & VARIOUS
int resolvSensorID (short, short);
float calcDewPoint (float, float);
int versions (void);
int displayDHTData (unsigned short);
int readActivePorts (void);
int timestamp (void);

// ISR Functions

void isr (void);
void func (void);
int checkInts (void);


//*****************************************
//*****************************************
//********** CONNECT TO DATABASE **********
//*****************************************
//*****************************************

int mysql_connect (void){
    //initialize MYSQL object for connections
	mysql1 = mysql_init(NULL);

    if(mysql1 == NULL){
        fprintf(stderr, "%s\n", mysql_error(mysql1));
        return 1;
    }

    //Connect to the database
    if(mysql_real_connect(mysql1, DATABASE_SERVER, DATABASE_USERNAME, DATABASE_PASSWORD, DATABASE_NAME, 0, NULL, 0) == NULL){
    	fprintf(stderr, "%s\n", mysql_error(mysql1));
		return 1;
    }
    //else{
    //    printf("DB connection successful.\n");
    //}
	return 0;
}

//**********************************************
//**********************************************
//********** DISCONNECT FROM DATABASE **********
//**********************************************
//**********************************************
int mysql_disconnect (void){
    
	mysql_close(mysql1);
    //printf( "Disconnected from DB.\n");
	return 0;
}
//*******************************************************************************
//****************************** END C SCRIPT ***********************************
//*******************************************************************************

