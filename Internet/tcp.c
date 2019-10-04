/*
 * Tcp.c
 * Autogenerated code from TrainFramework
 *
 *  Created on: 2019-10-04T11:23:44.388Z
 *      Author: AL
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#include "tcp.h"
#include "json.h"
#include "telnet.h"
#include "http.h"
#include "files.h"


#ifdef WIN32
#include 	<windows.h>
#include	<winsock2.h>
#include 	<ws2tcpip.h>

int getListenSocket(){
	return clientSoket;
}
#else
#define INVALID_SOCKET -1
#endif

int clientSoket = INVALID_SOCKET;


#define DEBUG_TRACE
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif





int initAdapter(void){
	#ifdef WIN32
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
		printf("Failed. Error Code : %d\n",WSAGetLastError());
		return EXIT_FAILURE;
	}
	#endif
	return EXIT_SUCCESS;
}

int _initNetAdapter(uint32_t inetAddress,uint16_t port, uint8_t protocol){
	#ifdef WIN32
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
		printf("Failed. Error Code : %d\n",WSAGetLastError());
		return EXIT_FAILURE;
	}

	struct sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inetAddress;
	service.sin_port = htons(port);
	//create socket
	clientSoket=socket(AF_INET, SOCK_STREAM, protocol);
	if( clientSoket == INVALID_SOCKET ){
		printf("Error: Socket Failed\n%d\n", errno);
		return INVALID_SOCKET;
	}
	int i=1;
	setsockopt(clientSoket,SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof (i));

	//bind socket
	if(bind(clientSoket, (SOCKADDR *) &service, sizeof (service))== SOCKET_ERROR){
		printf("bind failed with error %u\n", WSAGetLastError());
		return INVALID_SOCKET;
	}

	//server socket listen
	if(listen(clientSoket,SOMAXCONN) == SOCKET_ERROR){
		printf("Failed. Error Code : %d\n",WSAGetLastError());
		return INVALID_SOCKET;
	}
	#endif
	return EXIT_SUCCESS;
}

int initServer(uint32_t inetAddress,uint16_t port, uint8_t protocol){
	#ifdef WIN32
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
//		printf("Failed. Error Code : %d\n",WSAGetLastError());
//		return EXIT_FAILURE;
//	}

	struct sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inetAddress;
	service.sin_port = htons(port);
	//create socket
	clientSoket=socket(AF_INET, SOCK_STREAM, protocol);
	if( clientSoket == INVALID_SOCKET ){
		printf("Error: Socket Failed\n%d\n", errno);
		return INVALID_SOCKET;
	}
	int i=1;
	setsockopt(clientSoket,SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof (i));

	//bind socket
	if(bind(clientSoket, (SOCKADDR *) &service, sizeof (service))== SOCKET_ERROR){
		printf("bind failed with error %u\n", WSAGetLastError());
		return INVALID_SOCKET;
	}

	//server socket listen
	if(listen(clientSoket,SOMAXCONN) == SOCKET_ERROR){
		printf("Failed. Error Code : %d\n",WSAGetLastError());
		return INVALID_SOCKET;
	}
	#endif
	return EXIT_SUCCESS;
}

#ifdef WIN32
SOCKET ConnectSocket = INVALID_SOCKET;
#else
int ConnectSocket = INVALID_SOCKET;
#endif
int initClient(uint32_t inetAddress,uint16_t port, uint8_t protocol){
	#ifdef WIN32
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
//		printf("Failed. Error Code : %d\n",WSAGetLastError());
//		return EXIT_FAILURE;
//	}



	// Create a SOCKET for connecting to server
	struct sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inetAddress;
	service.sin_port = htons(port);
	//create socket
	ConnectSocket=socket(AF_INET, SOCK_STREAM, protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Connect to server.
	int iResult = connect(ConnectSocket, (SOCKADDR *) &service, sizeof (service));
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	#endif
	return ConnectSocket;
}

int getClientSocket(){
	return ConnectSocket;
}

int getServerSocket(){
	#ifdef WIN32
	struct sockaddr_in client = {};
	int clientSize = sizeof(client);
	int nSocket =  accept(clientSoket, (struct sockaddr*)&client, &clientSize);
	return nSocket;
	#endif
	return INVALID_SOCKET;
}

int getSocket(){
	#ifdef WIN32
	struct sockaddr_in client = {};
	int clientSize = sizeof(client);
	int nSocket =  accept(clientSoket, (struct sockaddr*)&client, &clientSize);
	return nSocket;
	#endif
	return INVALID_SOCKET;
}

Parcel_st tcpParcel[4];
Train_st tcpTrain;
void tcpStationInit(void){
	fillDepot(&tcpTrain);
	tcpTrain.box = malloc(sizeof(Parcel_st*)*4);
	for(uint16_t iParcel=0;iParcel<4;iParcel++){
		tcpTrain.box[iParcel] = (Parcel_st*)&tcpParcel[iParcel];
	}
	tcpTrain.capacity = 4;
	tcpTrain.route = ROUTE_TCP;
}

/*
 * TCP Railway Station
 */
int tcpStation(void *p){
	//Parcel_st *box = meetTrain(&tcpTrain);
	uint16_t iBox = meetTrainBox(&tcpTrain,0);
	Parcel_st *box = tcpTrain.box[iBox];
	while(box){
		if(iBox>tcpTrain.capacity) return EXIT_SUCCESS;
		printf("\nTCP Station\n");
		printf("\t%p %d %p\n",box,iBox,&tcpParcel[iBox]);
		static void *car;
		car = box->parcel;

		try((car),"\tempty car on tcp train",EXIT_FAILURE);
		int socket = 0;
		try(( ((RailTcp_st*)car)->socket ),"\tInvalid socket",EXIT_FAILURE);
		socket = ((RailTcp_st*)car)->socket->id;
		uint8_t i=0;
		while(car){
			TRACE(printf("\tCar number %d command (Hitch_st) %I64u \n",i,((Hitch_st *)car)->command););
			if(car==NULL) return EXIT_FAILURE;
			

			else if( (0 == i) & ( ROUTE_JSON_ERROR == ((RailTcp_st *)car)->command ) ){
				TRACE(printf("\tROUTE_JSON_ERROR load\n"););
				
				RailTcp_st *railTcp = ((RailTcp_st *)car);

				

				static RailTcp_st railTcp_0={NULL};


				try( (railTcp), "\tthe rails RailTcp_st do not go to tcpStation\n", EXIT_FAILURE );

				
				
				railTcp_0.response = (uint8_t*)railTcp->response;
				railTcp_0.respLen = railTcp->respLen;
				TRACE(printf("\tSend response:\n\t\t%d bytes\n\t\t%s\n",railTcp_0.respLen,railTcp_0.response););
				


				
				/*
				 * send response to TCP client or
				 * send request to TCP server
				 */
				try( (socket), "\tthe rails RailTcp_st do not go to tcpStation\n", EXIT_FAILURE );
				send(
					socket,
					(char*)railTcp_0.response,
					railTcp_0.respLen,
					0
				);
				
				((RailTcp_st *)car)->command = 0;
			}

			

			else if( (1 == i) & ( ROUTE_JSON_GET_TELNET == ((RailTelnet_st *)car)->command ) ){
				TRACE(printf("\tROUTE_JSON_GET_TELNET load\n"););
				
				RailTelnet_st *railTelnet = ((RailTelnet_st *)car);

				

				static RailTcp_st railTcp_1={NULL};


				try( (railTelnet), "\tthe rails RailTelnet_st do not go to tcpStation\n", EXIT_FAILURE );

				
				
				railTcp_1.response = (uint8_t*)railTelnet->response;
				railTcp_1.respLen = railTelnet->respLen;
				TRACE(printf("\tSend response:\n\t\t%d bytes\n\t\t%s\n",railTcp_1.respLen,railTcp_1.response););
				


				
				/*
				 * send response to TCP client or
				 * send request to TCP server
				 */
				try( (socket), "\tthe rails RailTelnet_st do not go to tcpStation\n", EXIT_FAILURE );
				send(
					socket,
					(char*)railTcp_1.response,
					railTcp_1.respLen,
					0
				);
				
				((RailTelnet_st *)car)->command = 0;
			}

			

			else if( (1 == i) & ( ROUTE_JSON_GET_HTTP == ((RailHttp_st *)car)->command ) ){
				TRACE(printf("\tROUTE_JSON_GET_HTTP load\n"););
				
				RailHttp_st *railHttp = ((RailHttp_st *)car);

				

				static RailTcp_st railTcp_2={NULL};


				try( (railHttp), "\tthe rails RailHttp_st do not go to tcpStation\n", EXIT_FAILURE );

				
				
				railTcp_2.response = (uint8_t*)railHttp->response;
				railTcp_2.respLen = railHttp->respLen;
				TRACE(printf("\tSend response:\n\t\t%d bytes\n\t\t%s\n",railTcp_2.respLen,railTcp_2.response););
				


				
				/*
				 * send response to TCP client or
				 * send request to TCP server
				 */
				try( (socket), "\tthe rails RailHttp_st do not go to tcpStation\n", EXIT_FAILURE );
				send(
					socket,
					(char*)railTcp_2.response,
					railTcp_2.respLen,
					0
				);
				
				((RailHttp_st *)car)->command = 0;
			}

			

			else if( (1 == i) & ( ROUTE_JSON_GETV == ((RailHttp_st *)car)->command ) ){
				TRACE(printf("\tROUTE_JSON_GETV load\n"););
				
				RailHttp_st *railHttp = ((RailHttp_st *)car);

				

				static RailTcp_st railTcp_3={NULL};


				try( (railHttp), "\tthe rails RailHttp_st do not go to tcpStation\n", EXIT_FAILURE );

				
				
				railTcp_3.response = (uint8_t*)railHttp->response;
				railTcp_3.respLen = railHttp->respLen;
				TRACE(printf("\tSend response:\n\t\t%d bytes\n\t\t%s\n",railTcp_3.respLen,railTcp_3.response););
				


				
				/*
				 * send response to TCP client or
				 * send request to TCP server
				 */
				try( (socket), "\tthe rails RailHttp_st do not go to tcpStation\n", EXIT_FAILURE );
				send(
					socket,
					(char*)railTcp_3.response,
					railTcp_3.respLen,
					0
				);
				
				((RailHttp_st *)car)->command = 0;
			}

			

			else if( (2 == i) & ( COMMAND_JSON_RESPONSE == ((RailJson_st *)car)->command ) ){
				TRACE(printf("\tCOMMAND_JSON_RESPONSE load\n"););
				
				RailJson_st *railJson = ((RailJson_st *)car);

				

				static RailTcp_st railTcp_4={NULL};


				try( (railJson), "\tthe rails RailJson_st do not go to tcpStation\n", EXIT_FAILURE );

				
				
				railTcp_4.response = (uint8_t*)railJson->response;
				railTcp_4.respLen = railJson->respLen;
				TRACE(printf("\tSend response:\n\t\t%d bytes\n\t\t%s\n",railTcp_4.respLen,railTcp_4.response););
				


				
				/*
				 * send response to TCP client or
				 * send request to TCP server
				 */
				try( (socket), "\tthe rails RailJson_st do not go to tcpStation\n", EXIT_FAILURE );
				send(
					socket,
					(char*)railTcp_4.response,
					railTcp_4.respLen,
					0
				);
				
				((RailJson_st *)car)->command = 0;
			}

			

			else if( (1 == i) & ( ROUTE_FILE_GET == ((RailHttp_st *)car)->command ) ){
				TRACE(printf("\tROUTE_FILE_GET load\n"););
				
				RailHttp_st *railHttp = ((RailHttp_st *)car);

				

				static RailTcp_st railTcp_5={NULL};


				try( (railHttp), "\tthe rails RailHttp_st do not go to tcpStation\n", EXIT_FAILURE );

				
				
				railTcp_5.response = (uint8_t*)railHttp->response;
				railTcp_5.respLen = railHttp->respLen;
				TRACE(printf("\tSend response:\n\t\t%d bytes\n\t\t%s\n",railTcp_5.respLen,railTcp_5.response););
				


				
				/*
				 * send response to TCP client or
				 * send request to TCP server
				 */
				try( (socket), "\tthe rails RailHttp_st do not go to tcpStation\n", EXIT_FAILURE );
				send(
					socket,
					(char*)railTcp_5.response,
					railTcp_5.respLen,
					0
				);
				
				((RailHttp_st *)car)->command = 0;
			}

			

			else if( (1 == i) & ( ROUTE_DIR_GET == ((RailHttp_st *)car)->command ) ){
				TRACE(printf("\tROUTE_DIR_GET load\n"););
				
				RailHttp_st *railHttp = ((RailHttp_st *)car);

				

				static RailTcp_st railTcp_6={NULL};


				try( (railHttp), "\tthe rails RailHttp_st do not go to tcpStation\n", EXIT_FAILURE );

				
				
				railTcp_6.response = (uint8_t*)railHttp->response;
				railTcp_6.respLen = railHttp->respLen;
				TRACE(printf("\tSend response:\n\t\t%d bytes\n\t\t%s\n",railTcp_6.respLen,railTcp_6.response););
				


				
				/*
				 * send response to TCP client or
				 * send request to TCP server
				 */
				try( (socket), "\tthe rails RailHttp_st do not go to tcpStation\n", EXIT_FAILURE );
				send(
					socket,
					(char*)railTcp_6.response,
					railTcp_6.respLen,
					0
				);
				
				((RailHttp_st *)car)->command = 0;
			}

			

			else if( (1 == i) & ( ROUTE_FILE_PUT == ((RailHttp_st *)car)->command ) ){
				TRACE(printf("\tROUTE_FILE_PUT load\n"););
				
				RailHttp_st *railHttp = ((RailHttp_st *)car);

				

				static RailTcp_st railTcp_7={NULL};


				try( (railHttp), "\tthe rails RailHttp_st do not go to tcpStation\n", EXIT_FAILURE );

				
				
				railTcp_7.response = (uint8_t*)railHttp->response;
				railTcp_7.respLen = railHttp->respLen;
				TRACE(printf("\tSend response:\n\t\t%d bytes\n\t\t%s\n",railTcp_7.respLen,railTcp_7.response););
				


				
				/*
				 * send response to TCP client or
				 * send request to TCP server
				 */
				try( (socket), "\tthe rails RailHttp_st do not go to tcpStation\n", EXIT_FAILURE );
				send(
					socket,
					(char*)railTcp_7.response,
					railTcp_7.respLen,
					0
				);
				
				((RailHttp_st *)car)->command = 0;
			}

			

			else if( (2 == i) & ( COMMAND_HTTP_RESPONSE == ((RailFiles_st *)car)->command ) ){
				TRACE(printf("\tCOMMAND_HTTP_RESPONSE load\n"););
				
				RailFiles_st *railFiles = ((RailFiles_st *)car);

				

				static RailTcp_st railTcp_8={NULL};


				try( (railFiles), "\tthe rails RailFiles_st do not go to tcpStation\n", EXIT_FAILURE );

				
				
				railTcp_8.response = (uint8_t*)railFiles->response;
				railTcp_8.respLen = railFiles->respLen;
				TRACE(printf("\tSend response:\n\t\t%d bytes\n\t\t%s\n",railTcp_8.respLen,railTcp_8.response););
				


				
				/*
				 * send response to TCP client or
				 * send request to TCP server
				 */
				try( (socket), "\tthe rails RailFiles_st do not go to tcpStation\n", EXIT_FAILURE );
				send(
					socket,
					(char*)railTcp_8.response,
					railTcp_8.respLen,
					0
				);
				
				((RailFiles_st *)car)->command = 0;
			}

			

			else if( (0 == i) & ( COMMAND_READ == ((RailTcp_st *)car)->command ) ){
				TRACE(printf("\tCOMMAND_READ load\n"););
				
				RailTcp_st *railTcp = ((RailTcp_st *)car);

				

				static RailTcp_st railTcp_9={NULL};


				try( (railTcp), "\tthe rails RailTcp_st do not go to tcpStation\n", EXIT_FAILURE );

				
				
				


				
				
				/*
				 * send message to TCP observers
				 * example  railTcp->to_port=80 send message to ROUTE_HTTP
				 */
				railTcp->car = &railTcp_9;
				sendTrainsFromDepot(ROUTE_TCP,railTcp->packet->to_port,box->parcel);
				
				((RailTcp_st *)car)->command = 0;
			}

			

			car = ((Hitch_st*)car)->car;
			i++;
		}

		//box=meetTrain(&tcpTrain);
		iBox = meetTrainBox(&tcpTrain,iBox);
		box = tcpTrain.box[iBox];
	}
	return EXIT_SUCCESS;
}