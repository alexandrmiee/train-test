/*
 * Telnet.c
 * Autogenerated code from TrainFramework
 *  Created on: 2019-10-05T20:40:13.488Z
 *      Author: AL
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#include "json.h"
#include "tcp.h"
#include "RestFullController.h"

#define ERROR_CODE	"{\"error\":\"%s receive %d parameter(s) instead of %d\"}\r\n%s"
#define EQUAL(command,data)	(restParser(data,telnetBufRx,sizeof(telnetBufRx),(command)," ",&argv[0],&argc,32)==EXIT_SUCCESS)
#define ARGS_CHECK(v,c)	((c)!=4)

#define NONE
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif
char telnetBufRx[128];
char telnetBufTx[128];

#define buffer	telnetBufRx
char telnetCursor[] = "\ruser>";

uint16_t sendArgError(char *buf,uint16_t bufLen, char * command,uint8_t argc, uint8_t iParams){
	TRACE(printf("format error:\n\t%s\n",command););
	return snprintf(
			buf,
			sizeof(bufLen),
			ERROR_CODE,
			command,
			argc,
			iParams,
			telnetCursor
		);
}

static char *argv[32];
static uint8_t argc;


Parcel_st telnetParcel[4];
Train_st telnetTrain;
void telnetStationInit(void){
	telnetTrain.box = malloc(sizeof(Parcel_st*)*4);
	for(uint16_t iParcel=0;iParcel<4;iParcel++){
		telnetTrain.box[iParcel] = (Parcel_st*)&telnetParcel[iParcel];
	}
	telnetTrain.capacity = 4;
	telnetTrain.route = ROUTE_TELNET;
	fillDepot(&telnetTrain);
}

#include "integration-node.h"
#define MODULE_NAME __FILE__
char outputString [2048];
int telnetStation(void *p){
	uint16_t iBox = meetTrainBox(&telnetTrain,0);
	Parcel_st *box = telnetTrain.box[iBox];
	while(box && iBox<=telnetTrain.capacity){
		static void *car;
		car = box->parcel;

		sprintf(
			outputString,
			"action: 'telnetStation',"
			"box:{"
				"pointer:'%p',"
				"sender:%d,"
				"parcel:%p,"
				"message:'%s',"
			"},"
			"iBox:%d,",
			box,box->sender,box->parcel,(char*)(((RailTcp_st*)car)->packet->data),iBox
		);
		traceStdOut(MODULE_NAME,outputString);
		
		//box=meetTrain( &telnetTrain );
		//try( (box),"HTTP train is empty",EXIT_SUCCESS);

		// if(iBox>telnetTrain.capacity) return EXIT_SUCCESS;

		

		if( (box->sender==ROUTE_TCP) & !equals((char*)(((RailTcp_st*)car)->packet->data),"\r\n",2) ){
				traceStdOut(MODULE_NAME,"telnetCursor");
				traceStdOut(MODULE_NAME,telnetCursor);
			((RailTcp_st*)car)->response = (uint8_t*)telnetCursor;
			((RailTcp_st*)car)->respLen = strlen(telnetCursor);
			((RailTcp_st*)car)->command = ROUTE_JSON_ERROR;
			sendTrainsFromDepot(ROUTE_TELNET,ROUTE_TCP,box->parcel);
		}


		

		else if(EQUAL(COMMAND_REST_GET,(char*)(((RailTcp_st*)car)->packet->data))){
			if(ARGS_CHECK(argv,argc))
				traceStdOut(MODULE_NAME,"Telnet ERROR. Arguments error");
				// TRACE(printf("Telnet ERROR. Arguments error %d",argc););
//				sendTrainsFromDepot(ROUTE_TELNET,ROUTE_TCP,box->parcel);
			else{
				traceStdOut(MODULE_NAME,"COMMAND_REST_GET");
				
				RailTcp_st *railTcp = ((RailTcp_st *)car);
				
				static RailTelnet_st railTelnet_0;

				try( (railTcp), "the rails RailTcp_st do not go to telnetStation\n", EXIT_FAILURE );

				((RailTcp_st*)car)->car = &railTelnet_0;
				sendTrainsFromDepot(ROUTE_TELNET,ROUTE_JSON,box->parcel);

				railTelnet_0.command = ROUTE_JSON_GET_TELNET;
				railTelnet_0.argv = &argv[0];
				railTelnet_0.argc = argc;
				railTelnet_0.response = telnetBufTx;
				railTelnet_0.respBufLen = sizeof(telnetBufTx);
				railTelnet_0.respOk = telnetCursor;
				railTelnet_0.respErr = telnetCursor;
			}
		}

		

		else if(EQUAL(COMMAND_REST_GETV,(char*)(((RailTcp_st*)car)->packet->data))){
			if(ARGS_CHECK(argv,argc))
				traceStdOut(MODULE_NAME,"Telnet ERROR. Arguments error");
				// TRACE(printf("Telnet ERROR. Arguments error %d",argc););
//				sendTrainsFromDepot(ROUTE_TELNET,ROUTE_TCP,box->parcel);
			else{
				traceStdOut(MODULE_NAME,"COMMAND_REST_GETV");
				
				RailTcp_st *railTcp = ((RailTcp_st *)car);
				
				static RailTelnet_st railTelnet_1;

				try( (railTcp), "the rails RailTcp_st do not go to telnetStation\n", EXIT_FAILURE );

				((RailTcp_st*)car)->car = &railTelnet_1;
				sendTrainsFromDepot(ROUTE_TELNET,ROUTE_JSON,box->parcel);

				railTelnet_1.command = ROUTE_JSON_GETV;
				railTelnet_1.argv = &argv[0];
				railTelnet_1.argc = argc;
				railTelnet_1.response = telnetBufTx;
				railTelnet_1.respBufLen = sizeof(telnetBufTx);
				railTelnet_1.respOk = telnetCursor;
				railTelnet_1.respErr = telnetCursor;
			}
		}

		

		else if(EQUAL(COMMAND_REST_PUT,(char*)(((RailTcp_st*)car)->packet->data))){
			if(ARGS_CHECK(argv,argc))
				traceStdOut(MODULE_NAME,"Telnet ERROR. Arguments error");

				// TRACE(printf("Telnet ERROR. Arguments error %d",argc););
//				sendTrainsFromDepot(ROUTE_TELNET,ROUTE_TCP,box->parcel);
			else{
				traceStdOut(MODULE_NAME,"COMMAND_REST_PUT");
				
				RailTcp_st *railTcp = ((RailTcp_st *)car);
				
				static RailTelnet_st railTelnet_2;

				try( (railTcp), "the rails RailTcp_st do not go to telnetStation\n", EXIT_FAILURE );

				((RailTcp_st*)car)->car = &railTelnet_2;
				sendTrainsFromDepot(ROUTE_TELNET,ROUTE_JSON,box->parcel);

				railTelnet_2.command = ROUTE_JSON_PUT;
				railTelnet_2.argv = &argv[0];
				railTelnet_2.argc = argc;
				railTelnet_2.response = telnetBufTx;
				railTelnet_2.respBufLen = sizeof(telnetBufTx);
				railTelnet_2.respOk = telnetCursor;
				railTelnet_2.respErr = telnetCursor;
			}
		}
		else{
				traceStdOut(MODULE_NAME,"COMMAND NOT FOUND");
		}
		



		if( (box->sender==ROUTE_TCP) & (((RailTcp_st*)car)->respLen) )
			sendTrainsFromDepot(ROUTE_TELNET,ROUTE_TCP,box->parcel);
		
		iBox = meetTrainBox(&telnetTrain,iBox);
		box = telnetTrain.box[iBox];
	}

  	fflush(stdout);

	return EXIT_SUCCESS;
}
