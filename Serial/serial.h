/*
 * ble.h
 * Autogenerated code from TrainFramework
 *
 *  Created on: 2019-10-04T07:59:49.432Z
 *      Author: ALex
 */

#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#include "database.h"
#include "serial.h"

typedef struct Socket{
	uint8_t id;
	uint8_t *buffer;
	uint32_t bufLen;
}__attribute__ ((__packed__)) Socket_st;

typedef struct RailSerial{
	void *car;
	uint64_t command;
	uint8_t *response;
	uint16_t respLen;
	uint16_t respBufLen;
	uint8_t *request;
	uint16_t reqLen;
	Socket_st *socket;
	uint64_t routeResp;
	//void *socket;
	//uint8_t socketId;
}__attribute__ ((__packed__)) RailSerial_st;


#define SOCKET_USART_RX	1

#define SOCKET_USART_TX	2



int serialStation(void *p);
void serialStationInit(void);

#endif /* _SERIAL_H_ */
