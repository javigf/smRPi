main:
	sudo g++ smonitor.c -o smonitor.sh `mysql_config --cflags` `mysql_config --libs` -L/usr/local/lib -lwiringPi
	sudo g++ ints.c -o ints.sh `mysql_config --cflags` `mysql_config --libs` -L/usr/local/lib -lwiringPi
