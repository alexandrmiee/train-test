/*
 * TrainFramework test
 * 
 * 2019-10-04T11:23:44.389Z
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#include "tcp.h"
#include "telnet.h"
#include "json.h"
#include "http.h"
#include "files.h"
#include "database.h"
#include "mqtt.h"
#include "flash.h"



int main(void) {

	initAdapter();

	
	tcpStationInit();
	telnetStationInit();
	jsonStationInit();
	httpStationInit();
	filesStationInit();
	dbStationInit();
	mqttStationInit();

	
	for(;;) {
		dbStation(NULL);
		
		tcpStation(NULL);
		telnetStation(NULL);
		jsonStation(NULL);
		httpStation(NULL);
		filesStation(NULL);
		dbStation(NULL);
		mqttStation(NULL);
	}

	return EXIT_SUCCESS;
}

