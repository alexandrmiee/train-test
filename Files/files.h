/*
 * files.h
 *
 *  Created on: 2019-10-04T11:23:44.379Z
 *      Author: home
 */

#ifndef FILES_H_
#define FILES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"



typedef struct RailFiles{
	void *car;
	uint64_t command;
	uint8_t *response;
	uint16_t respLen;
	uint16_t respBufLen;
}__attribute__ ((__packed__)) RailFiles_st;

typedef struct{
	uint16_t fsize;
	char *fname;
	char *content;
	uint8_t *file;
} FileDesc_st;

#define HTML_CONTENT_TEXT_HTML	"text/html"
#define HTML_CONTENT_TEXT_CSS	"text/css"
#define HTML_CONTENT_TEXT_JS	"application/javascript"
#define HTML_CONTENT_JSON		"application/json"


#define DISK_FLASH_DB	0

#define DISK_FLASH_BOOT	1

#define DISK_BOOTLOADER	2

#define DISK_SD_SPI	3

#define DISK_FLASH_SPI	4

#define DISK_FLASH_CAN	5

#define DISK_FLASH_MC	6

#define ARG_DISK_DRIVE	0

#define ARG_DIR	1

#define ARG_FILE	2

#define ARG_TABLE	1

#define ARG_KEY	2

#define FTP_DIR	"type=dir;modify=20170515122627; %s\r\n"

#define FTP_MKDIR	"\"%s/\"\r\n"

#define FTP_FILE	"type=file;size=%u;modify=20170515122627; %s\r\n"

#define HTTP_DIR	"{\"type\":\"d\",\"name\":\"%s\"},"

#define HTTP_FILE	"{\"type\":\"f\",\"size\":\"%u\",\"name\":\"%s\"},"


int filesStation(void *p);
void filesStationInit(void);

#endif /* FILES_H_ */

