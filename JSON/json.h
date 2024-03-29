/*
 * json.h
 * Autogenerated code from TrainFramework
 *
 *  Created on: 2019-10-05T20:40:13.487Z
 *      Author: ALex
 */

#ifndef JSON_H_
#define JSON_H_

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


typedef struct RailJson{
	void *car;
	uint64_t command;
	char *response;
	uint16_t respLen;
}__attribute__ ((__packed__)) RailJson_st;


int jsonStation(void *p);
void jsonStationInit(void);

#endif /* JSON_H_ */

