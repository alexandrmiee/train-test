/*
 * HTTP.c
 * Autogenerated code from TrainFramework
 *  Created on: 2019-10-04T11:23:44.386Z
 *      Author: AL
 */

#ifndef HTTP_H_
#define HTTP_H_

typedef struct RailHttp {
	void *car;
	uint64_t command;
	char *response;
	uint16_t respLen;
	uint16_t respBufLen;
	char **argv;
	uint16_t argc;
	char *respOk;
	char *respErr;
	uint8_t *attach;
	uint8_t *attachFileName;
	uint8_t *contentType;
	uint16_t attachLen;
	uint8_t isStreamEnd;
}__attribute__ ((__packed__)) RailHttp_st;

int httpStation(void *p);
void httpStationInit(void);

#endif /*HTTP_H_*/