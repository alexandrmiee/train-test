/*
 * serial.c
 * Autogenerated code from TrainFramework
 *
 *  Created on: 2019-10-04T07:59:49.432Z
 *      Author: ALex
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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
#include "udp.h"
#include "snmp.h"
#include "flash.h"
#include "serial.h"
#include "ow.h"
#include "ble.h"


#define DEBUG_TRACE
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif


int _uartDmaIO(Socket_st* socket){
	TRACE(printf("\n"););
	for(int i=0;i<socket->bufLen;i++){
		TRACE(printf("0x%02X ",socket->buffer[i]););
	}
	TRACE(printf("\n\n"););
	return EXIT_SUCCESS;
}
uint32_t _uartDmaCountIO(Socket_st* socket){
	static uint8_t i = 100;
	return i--;
}
Socket_st *getBleRxSocket(){
	static Socket_st socketRx;
	socketRx.id = SOCKET_USART_RX;
	return &socketRx;
}
Socket_st *getBleTxSocket(){
	static Socket_st socketTx;
	socketTx.id = SOCKET_USART_TX;
	return &socketTx;
}


static char serialBufTx[128];
static char serialBufRx[128];
static Parcel_st serialParcel[4];
static Train_st serialTrain;
static Parcel_st *pBox[4];


uint8_t sendSerialAsync(Hitch_st *hitch, RailSerial_st *serial){
	if(serial->socket){
		switch (serial->socket->id) {
			default:
				TRACE(printf("\tSend serial ERROR. Socket not found\n"););
			break;
			
			case SOCKET_USART_RX:
			{ 
				_uartDmaIO((Socket_st*)serial->socket);
			}
			break;
			
			case SOCKET_USART_TX:
			{ 
				_uartDmaIO((Socket_st*)serial->socket);
			}
			break;
			
		}
	}
	return EXIT_SUCCESS;
}

int serialPoll(RailSerial_st *serial){
	if(serial->socket){
		if(serial->socket->bufLen){
			uint32_t rest = 0;
			switch (serial->socket->id) {
				default:
				break;
				
				case SOCKET_USART_RX:
				{ 
					rest = _uartDmaCountIO((Socket_st*)serial->socket);
					if(serial->socket->bufLen>rest){
						serial->socket->bufLen = rest;
						sendTrainsFromDepot(ROUTE_SERIAL,serial->routeResp,serial);
					}
				}
				break;
				
				case SOCKET_USART_TX:
				{ 
					rest = _uartDmaCountIO((Socket_st*)serial->socket);
					if(serial->socket->bufLen>rest){
						serial->socket->bufLen = rest;
						sendTrainsFromDepot(ROUTE_SERIAL,serial->routeResp,serial);
					}
				}
				break;
				
			}
		}
	}
	return EXIT_SUCCESS;
}


void serialStationInit(void){
	fillDepot(&serialTrain);
	serialTrain.box = pBox;
	for(uint16_t iParcel=0;iParcel<4;iParcel++){
		serialTrain.box[iParcel] = (Parcel_st*)&serialParcel[iParcel];
	}
	serialTrain.capacity = 4;
	serialTrain.route = ROUTE_SERIAL;
}

int serialStation(void *p){
	
	static RailSerial_st railSerial_0;
	serialPoll(&railSerial_0);
	
	static RailSerial_st railSerial_1;
	serialPoll(&railSerial_1);
	

	uint16_t iBox = meetTrainBox(&serialTrain,0);
	Parcel_st *box = serialTrain.box[iBox];
	while(box){
		if(iBox>serialTrain.capacity) return EXIT_SUCCESS;

		static void *car;
		car = box->parcel;
		uint8_t i=0;

		while(car){
			
			if(0==1){}
			
			else if( COMMAND_SERIAL_READ_OBSERVER == ((RailBle_st *)car)->command ){
				TRACE(printf("\tCOMMAND_SERIAL_READ_OBSERVER\n"););
				// get the parcel from the car
				RailBle_st *railBle = ((RailBle_st *)car);


				try( (railBle), "the rails RailBle_st do not go to serialStation\n", EXIT_FAILURE );
				railSerial_0.socket = (Socket_st*)getBleRxSocket();
				railSerial_0.socket->bufLen = railBle->respLen;
				railSerial_0.socket->buffer = railBle->response;
				railSerial_0.routeResp = ROUTE_BLE;
				railSerial_0.command = COMMAND_SERIAL_READ_OBSERVER;
				
				sendSerialAsync((Hitch_st*)railBle,&railSerial_0);
				

				
			}

			
			else if( COMMAND_SERIAL_SEND == ((RailBle_st *)car)->command ){
				TRACE(printf("\tCOMMAND_SERIAL_SEND\n"););
				// get the parcel from the car
				RailBle_st *railBle = ((RailBle_st *)car);


				try( (railBle), "the rails RailBle_st do not go to serialStation\n", EXIT_FAILURE );
				railSerial_1.socket = (Socket_st*)getBleTxSocket();
				railSerial_1.socket->bufLen = railBle->respLen;
				railSerial_1.socket->buffer = railBle->response;
				railSerial_1.routeResp = ROUTE_BLE;
				railSerial_1.command = COMMAND_SERIAL_SEND;
				
				sendSerialAsync((Hitch_st*)railBle,&railSerial_1);
				

				
			}

			
			
			car = ((Hitch_st*)car)->car;
			i++;
		}

		iBox = meetTrainBox(&serialTrain,iBox);
		box = serialTrain.box[iBox];
	}

	return EXIT_SUCCESS;
}


