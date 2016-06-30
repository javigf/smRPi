// File to support everything else
/*


CREATE USER 'usrsensor'@'localhost' IDENTIFIED BY 'passpass';
create user usrsensor@localhost
identified by 'sm.m0n1t0r';

grant all 
on smartmonitor.*
to 'usrsensor'@'localhost';

UPDATE smartmonitor.sensors SET id_status = 3 WHERE id = 'TEMP01';

INSERT INTO sensores_registros(id_sensor,registro,created_at) VALUES('TEMP01',23,now());
UPDATE sensors SET id_error = 0 WHERE id_error >= 10 and id = 'TEMP01';


INSERT INTO sensores_registros(id_sensor,registro,created_at) VALUES('HDAD01',23,now());
UPDATE sensors SET id_error = 0 WHERE id_error >= 10 and id = 'HDAD01';


INSERT INTO sensores_registros(id_sensor,registro,created_at) VALUES('PCON01',23,now());
UPDATE sensors SET id_error = 0 WHERE id_error >= 10 and id = 'PCON01';

*/
/////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++ PIN PUD SETUP FUNCTION +++++++++++++++++++++++++++++++++
/////////////////////////////////////////////////////////////////////////////////
//USAGE of FUNCTION - void pullUpDnControl (int pin, int pud) ;

/*int setPinMode (void){
	unsigned int i;
	int size = sizeof (activePorts)/ sizeof (int);

	for (i=0 ; i < size ; i++)
		pinMode( activePorts[i], INPUT);
	
	return 0;
}*/

/////////////////////////////////////////////////////////////////////////////////
//----------------------- END PIN PUD SETUP FUNCTION ----------------------------
/////////////////////////////////////////////////////////////////////////////////


// This Function displays the versions

int versions (void){
	
	printf ("Version Smart Monitor %2.2f \n", SMART_MONITOR_VER );
	printf ("Version AN2302/DHT22 Sensor %2.2f \n", DHT_VER );
	return 0;
}

// This Function displays the current Time Stamp used for loggin purposes

int timestamp (void){
    time_t ltime; /* calendar time */
    ltime=time(NULL); /* get current cal time */
    printf("%s",asctime( localtime(&ltime)));
    return 0;
}

//*******************************************************************************
//****************************** END C SCRIPT ***********************************
//*******************************************************************************

