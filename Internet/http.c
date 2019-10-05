/*
 * Telnet.c
 * Autogenerated code from TrainFramework
 *  Created on: 2019-10-05T20:40:13.470Z
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
#include "http.h"
#include "RestFullController.h"

#define ERROR_CODE	"{\"error\":\"%s receive %d parameter(s) instead of %d\"}\r\n%s"
#define EQUAL(command,data)	(strstr(data,command))
#define ARGS_CHECK(v,c)	((c)<2)

#define NONE
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif

#define HEADER_ERR_404	"HTTP/1.0 404 PAGE NOT FOUND\r\n\r\n"

#define HEADER_OK_JSON	"HTTP/1.0 200 OK\r\n"\
						"Content-Length: %d\r\n"\
						"Content-Type: application/json; charset=utf-8\r\n\r\n"

#define HEADER_OK_TEXT	"HTTP/1.0 200 OK\r\n"\
						"Content-Type: text/html; charset=utf-8\r\n"\
						"Content-Length: %d\r\n"\
						"Content-Encoding: gzip\r\n\r\n"

char httpBufRx[512];
char httpBufTx[512];

static char *argv[32];
static uint8_t argc;


int getAttachFile(RailHttp_st *http, RailTcp_st *tcp){
	if(tcp->socket->stream){

	}
	else{
		uint8_t *files[32];
		uint8_t c=0;
		TRACE(printf("\tinput\n%.10s\n...\n",http->attach));
		restParser(
			http->attach,
			httpBufRx,
			sizeof(httpBufRx),
			"filename",
			"\"",
			&files[0],
			&c,
			3
		);
		if(!files[1]){
			TRACE(printf("\tattach file name not found\n"));
			return EXIT_FAILURE;
		}
		http->attachFileName = files[1];
		TRACE(printf("\tattach file name %s\n",http->attachFileName));
		restParser(
				files[2],
				httpBufRx,
				sizeof(httpBufRx),
				"Content-Type",
				" \r\n",
				&files[0],
				&c,
				2
			);
		http->contentType = files[1];
		if(!files[1]){
			TRACE(printf("\tattach file content not found\n"));
			return EXIT_FAILURE;
		}
		TRACE(printf("\tattach file content type %s\n",http->contentType));
		http->attach = files[2]+3;//+2 = move on " \r\n"
		if(!files[2]){
			TRACE(printf("\tattach file not found\n"));
			return EXIT_FAILURE;
		}
		TRACE(printf("\tattach file\n%.10s\n...\n",http->attach));
	}
	char *end=http->attach;
	while(memcmp(end,"\r\n------WebKitFormBoundary",strlen("\r\n------WebKitFormBoundary"))){
		end++;
		if( end>(tcp->packet->data+tcp->requestLen) ){
			TRACE(printf("\tfile end not found\n"));
			end = NULL;
			break;
		}
	}
	if(end){
		tcp->socket->stream = 0;
		http->isStreamEnd = 1;
		http->attachLen = end-(char*)http->attach;
	}
	else{
		http->attachLen = tcp->requestLen-(http->attach-tcp->packet->data);
	}
	TRACE(printf("\tattach file len %d\n",http->attachLen));
	return EXIT_SUCCESS;
}

Parcel_st httpParcel[4];
Train_st httpTrain;
void httpStationInit(void){
	httpTrain.box = malloc(sizeof(Parcel_st*)*4);
	for(uint16_t iParcel=0;iParcel<4;iParcel++){
		httpTrain.box[iParcel] = (Parcel_st*)&httpParcel[iParcel];
	}
	httpTrain.capacity = 4;
	httpTrain.route = ROUTE_HTTP;
	fillDepot(&httpTrain);
}

int httpStation(void *p){
	uint16_t iBox = meetTrainBox(&httpTrain,0);
	Parcel_st *box = httpTrain.box[iBox];
	while(box){
		

		if(iBox>httpTrain.capacity) return EXIT_SUCCESS;

		
		static void *car;
		car = box->parcel;

		enum Type{get=0,post,put,patch,delete};
		enum Type type = get;
		RailTcp_st *railTcp = ((RailTcp_st *)car);
		
		if(railTcp->socket->stream==post){
		}
		else if(EQUAL("GET",(char*)((RailTcp_st*)car)->packet->data)){
			restParser(
				(char*)((RailTcp_st*)car)->packet->data,
				httpBufRx,
				sizeof(httpBufRx),
				"GET",
				" ",
				&argv[0],
				&argc,
				2
			);
			type=get;
		}
		else if(EQUAL("POST",(char*)((RailTcp_st*)car)->packet->data)){
			restParser(
				(char*)((RailTcp_st*)car)->packet->data,
				httpBufRx,
				sizeof(httpBufRx),
				"POST",
				" ",
				&argv[0],
				&argc,
				2
			);
			type=post;
		}
		else if(EQUAL("PUT",(char*)((RailTcp_st*)car)->packet->data)){
			restParser(
				(char*)((RailTcp_st*)car)->packet->data,
				httpBufRx,
				sizeof(httpBufRx),
				"PUT",
				" ",
				&argv[0],
				&argc,
				2
			);
			type=put;
		}
		else if(EQUAL("PATCH",(char*)((RailTcp_st*)car)->packet->data)){
			restParser(
				(char*)((RailTcp_st*)car)->packet->data,
				httpBufRx,
				sizeof(httpBufRx),
				"PATCH",
				" ",
				&argv[0],
				&argc,
				2
			);
			type=patch;
		}
		else if(EQUAL("DELETE",(char*)((RailTcp_st*)car)->packet->data)){
			restParser(
				(char*)((RailTcp_st*)car)->packet->data,
				httpBufRx,
				sizeof(httpBufRx),
				"DELETE",
				" ",
				&argv[0],
				&argc,
				2
			);
			type=delete;
		}

		TRACE(printf("HTTP request \n\t%s with \n\t%d arqument(s)\n",argv[0],argc););
		char *body = argv[2];
		
		restParser(
			argv[1],
			httpBufRx,
			sizeof(httpBufRx),
			"/",
			"/",
			&argv[0],
			&argc,
			6
		);

		TRACE(printf("\t%s with \n\t%d arqument(s)\n",argv[0],argc););
		//dummy if
		if(0){}

		
		else if(EQUAL(COMMAND_HTTP_PATH_FILES,argv[0]) && type==get){
			TRACE(printf("HTTP COMMAND_HTTP_PATH_FILES request\n"););
			
			static RailHttp_st railHttp_0;

			railHttp_0.argv = &argv[1];
			railHttp_0.argc = argc;
			railHttp_0.response = httpBufTx;
			railHttp_0.respBufLen = sizeof(httpBufTx);
			railHttp_0.respOk = HEADER_OK_TEXT;
			railHttp_0.respErr = HEADER_ERR_404;
			railHttp_0.command = ROUTE_FILE_GET;
			
			railTcp->car = &railHttp_0;
			sendTrainsFromDepot(ROUTE_HTTP,ROUTE_FILES,box->parcel);

		}
		
		else if(EQUAL(COMMAND_HTTP_PATH_FILES_FORM,argv[0]) && type==post){
			TRACE(printf("HTTP COMMAND_HTTP_PATH_FILES_FORM request\n"););
			
			static RailHttp_st railHttp_1;

			railHttp_1.argv = &argv[1];
			railHttp_1.argc = argc;
			railHttp_1.response = httpBufTx;
			railHttp_1.respBufLen = sizeof(httpBufTx);
			railHttp_1.respOk = HEADER_OK_TEXT;
			railHttp_1.respErr = HEADER_ERR_404;
			railHttp_1.command = ROUTE_FILE_PUT;
			
			railHttp_1.attach = body;
			if(getAttachFile(&railHttp_1,railTcp)!=EXIT_SUCCESS) return EXIT_SUCCESS;
			
			railTcp->car = &railHttp_1;
			sendTrainsFromDepot(ROUTE_HTTP,ROUTE_FILES,box->parcel);

		}
		
		else if(EQUAL(COMMAND_HTTP_PATH_DIR,argv[0]) && type==get){
			TRACE(printf("HTTP COMMAND_HTTP_PATH_DIR request\n"););
			
			static RailHttp_st railHttp_2;

			railHttp_2.argv = &argv[1];
			railHttp_2.argc = argc;
			railHttp_2.response = httpBufTx;
			railHttp_2.respBufLen = sizeof(httpBufTx);
			railHttp_2.respOk = HEADER_OK_TEXT;
			railHttp_2.respErr = HEADER_ERR_404;
			railHttp_2.command = ROUTE_DIR_GET;
			
			railTcp->car = &railHttp_2;
			sendTrainsFromDepot(ROUTE_HTTP,ROUTE_FILES,box->parcel);

		}
		
		else if(EQUAL(COMMAND_HTTP_PATH_JSON,argv[0]) && type==get){
			TRACE(printf("HTTP COMMAND_HTTP_PATH_JSON request\n"););
			
			static RailHttp_st railHttp_3;

			railHttp_3.argv = &argv[1];
			railHttp_3.argc = argc;
			railHttp_3.response = httpBufTx;
			railHttp_3.respBufLen = sizeof(httpBufTx);
			railHttp_3.respOk = HEADER_OK_TEXT;
			railHttp_3.respErr = HEADER_ERR_404;
			railHttp_3.command = ROUTE_JSON_GETV;
			
			railTcp->car = &railHttp_3;
			sendTrainsFromDepot(ROUTE_HTTP,ROUTE_JSON,box->parcel);

		}
		
		else if(EQUAL(COMMAND_HTTP_PATH_JSON,argv[0]) && type==get){
			TRACE(printf("HTTP COMMAND_HTTP_PATH_JSON request\n"););
			
			static RailHttp_st railHttp_4;

			railHttp_4.argv = &argv[1];
			railHttp_4.argc = argc;
			railHttp_4.response = httpBufTx;
			railHttp_4.respBufLen = sizeof(httpBufTx);
			railHttp_4.respOk = HEADER_OK_TEXT;
			railHttp_4.respErr = HEADER_ERR_404;
			railHttp_4.command = ROUTE_JSON_GET_HTTP;
			
			railTcp->car = &railHttp_4;
			sendTrainsFromDepot(ROUTE_HTTP,ROUTE_JSON,box->parcel);

		}
		
		railTcp->socket->stream = type;

		iBox = meetTrainBox(&httpTrain,iBox);
		box = httpTrain.box[iBox];
	}

	return EXIT_SUCCESS;
}
