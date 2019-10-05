/*
 * files.c
 *
 *  Created on: 2019-10-05T20:40:13.450Z
 *      Author: AL
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#include "tcp.h"

#include "telnet.h"

#include "json.h"

#include "http.h"

#include "files.h"

#include "database.h"

#include "mqtt.h"

#include "flash.h"


#define NONE
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif





extern FileDesc_st indexHtmlDescriptor;

extern FileDesc_st indexJsDescriptor;

extern FileDesc_st indexCssDescriptor;

extern FileDesc_st indexMqttDescriptor;

extern FileDesc_st indexVueDescriptor;


FileDesc_st *files[] = {
	
	&indexHtmlDescriptor,
	
	&indexJsDescriptor,
	
	&indexCssDescriptor,
	
	&indexMqttDescriptor,
	
	&indexVueDescriptor,
	
	NULL
};

char filesBufRx[128];
char filesBufTx[2048];

FileDesc_st *findFile(char *fname){
	uint16_t len = strlen(fname);
	for(int i=0;i<(sizeof(files)/sizeof(files[0])-1);i++){
		if(equals(files[i]->fname,fname,len)==0){
			return files[i];
		}
	}
	return NULL;
}


#define HEADER_OK_JSON	"HTTP/1.0 200 OK\r\n"\
						"Content-Length: %d\r\n"\
						"Content-Type: application/json; charset=utf-8\r\n\r\n"

#define HEADER_OK_TEXT	"HTTP/1.0 200 OK\r\n"\
						"Content-Type: %s; charset=utf-8\r\n"\
						"Content-Length: %d\r\n"\
						"Content-Encoding: gzip\r\n\r\n"

int cd(RailHttp_st *railHitch){
	switch(atoi(railHitch->argv[ARG_DISK_DRIVE])){
		case DISK_SD_SPI:
		case DISK_FLASH_SPI:
		case DISK_FLASH_MC:
			
			return EXIT_SUCCESS;
		case DISK_FLASH_DB:
		case DISK_FLASH_BOOT:
		case DISK_BOOTLOADER:
		case DISK_FLASH_CAN:
		default:
			TRACE(printf("\tFiles ERROR. Cannot change SFS directory\n"););
	}
	return EXIT_FAILURE;
}

int saveFile(RailHttp_st *http,RailFiles_st *railFile){
	TRACE(printf("Save new file\n"););
	TRACE(printf("\tdrive %s\n",http->argv[ARG_DISK_DRIVE]););
	TRACE(printf("\tdirectory %s\n",http->argv[ARG_DIR]););
	TRACE(printf("\tfile name %s\n",http->argv[ARG_FILE]););
	railFile->response = (uint8_t*)filesBufTx;
	railFile->respLen = 0;
	railFile->respBufLen = sizeof(filesBufTx);
	switch(atoi(http->argv[ARG_DISK_DRIVE])){
		case DISK_FLASH_BOOT:
		case DISK_BOOTLOADER:
		case DISK_FLASH_CAN:
		break;
		case DISK_FLASH_DB:
		{
			static void* db = 0;

			/*
			ModuleDb_st *module = getDbConfig();
			if(
				!equals(
					http->argv[ARG_DIR],
					&(module->HTTP.object),
					strConfigLen(&(module->HTTP.object))
				)
			){
				printf("\t%s\n",&(module->HTTP.object));
			}
			else
				printf("\terror object not found %.*s\n",strConfigLen(&(module->HTTP.object)),&(module->HTTP.object));
			*/
			char *tables[1];
			uint16_t iTables = 0;
			tables[0] = NULL;
			findTablesWithKeyValue(
					&tables[0],
					1,
					&iTables,
					getDbInstance(),
					http->argv[ARG_TABLE],
					http->argv[ARG_KEY],
					"*"
			);

			DbTable_st *table = (DbTable_st *)tables[0];
			TRACE(printf("\t%.10s\n",table->primary_index););

			uint32_t len = strConfigTableLen(table->primary_index);
			if(http->attachLen==len)
				memcpy(table->primary_index,http->attach,http->attachLen);
			else{
				TRACE(printf("\t object len error: table len %d, object len: %d",len,http->attachLen););
			}
			break;
		}
		case DISK_FLASH_MC:
		case DISK_SD_SPI:
		case DISK_FLASH_SPI:
		{	
			//FIXME: multy thread errors
			static uint8_t isNewFile = 1;
			if(isNewFile){
				//find file
				//delete, cause fragmentation
				//create new file
				FileDesc_st *file = malloc(sizeof(file));
				if(!file){
					TRACE(printf("\tFiles ERROR. Cannot malloc memory for SFS file\n"););
					return EXIT_FAILURE;
				}
				uint32_t pFile = getAddress();
				file->fsize = http->attachLen;
				file->fname = (char*)(pFile+sizeof(file));
				file->content = file->fname+strlen(http->attachFileName)+1;
				file->file = (uint8_t*)(file->content+strlen(http->contentType)+1);

				while(writeChunk(&file,sizeof(file),0)){}
				while(writeChunk(http->attachFileName,strlen(http->attachFileName)+1,0)){}
				while(writeChunk(http->contentType,strlen(http->contentType)+1,0)){}

				railFile->respLen += snprintf(
					railFile->response,
					railFile->respBufLen,
					"{\"file\":\"saved\"}"
				);
			}
			isNewFile = http->isStreamEnd;
			while(writeChunk(http->attach,http->attachLen,http->isStreamEnd)){}
			
			
			break;
		}
		default:
			TRACE(printf("\tFiles ERROR. Save file, disk drive not found\n"););
			return EXIT_FAILURE;
	}
	http->respLen =
		snprintf(
			http->response,
			http->respBufLen,
			HEADER_OK_JSON,
			railFile->respLen
		);
	return EXIT_SUCCESS;
}

int makeDir(Hitch_st *railHttp,RailFiles_st *railFile){
	railFile->response = filesBufTx;
	railFile->respLen = 0;
	railFile->respBufLen = sizeof(filesBufTx);
	switch(atoi(railHttp->argv[ARG_DISK_DRIVE])){
		case DISK_SD_SPI:
		case DISK_FLASH_SPI:
			
		case DISK_FLASH_DB:
		case DISK_FLASH_BOOT:
		case DISK_BOOTLOADER:
		case DISK_FLASH_CAN:
		case DISK_FLASH_MC:
		default:
			TRACE(printf("\tFiles ERROR. Cannot create new directory\n"););
			return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}


int getListDir(RailHttp_st *railHitch,RailFiles_st *railFile){
	railFile->response = filesBufTx;
	railFile->respLen = 0;
	railFile->respBufLen = sizeof(filesBufTx);
	if(cd(railHitch)==EXIT_FAILURE){
		TRACE(printf("\tFiles ERROR. Change drive\n"););
		return EXIT_FAILURE;
	}

	switch(atoi(railHitch->argv[ARG_DISK_DRIVE])){
		case DISK_FLASH_DB:
			railFile->respLen += snprintf(
				railFile->response,
				railFile->respBufLen,
				HTTP_FILE,
				0,
				"database"
			);
		break;
		case DISK_FLASH_BOOT:
		railFile->respLen += snprintf(
				railFile->response,
				railFile->respBufLen,
				HTTP_FILE,
				0,
				"firmware"
			);
		break;
		case DISK_BOOTLOADER:
		railFile->respLen += snprintf(
				railFile->response,
				railFile->respBufLen,
				HTTP_FILE,
				0,
				"bootloader"
			);
		break;
		case DISK_FLASH_MC:
		{ 
			if(!railFile->respBufLen){
				TRACE(printf("\tFiles ERROR. Not enough memory to read directory info\n\t%s\n",railFile->response););
				break;
			}
			railFile->respLen += snprintf(
					railFile->response,
					railFile->respBufLen,
					"["
				);
			railFile->respBufLen--;
			for(int i=0;i<(sizeof(files)/sizeof(files[0])-1);i++){
				uint16_t len = snprintf(
					railFile->response+railFile->respLen,
					railFile->respBufLen,
					HTTP_FILE,
					files[i]->fsize,
					files[i]->fname
				);
				railFile->respLen += len;
				railFile->respBufLen -= len;
				if(railFile->respLen >= railFile->respBufLen){
					TRACE(printf("\tFiles ERROR. Not enough memory to read directory info\n\t%s\n",railFile->response););
					break;
				}
			}

			FileDesc_st file;
			uint32_t addr = 0;
			while(readData(&file,&addr,sizeof(file)));
			while(file.fsize!=0xFFFF){
				if(file.fsize){
					uint16_t len = snprintf(
						railFile->response+railFile->respLen,
						railFile->respBufLen,
						HTTP_FILE,
						file.fsize,
						(char*)*file.fname
					);
					railFile->respLen += len;
					railFile->respBufLen -= len;
					if(railFile->respLen >= railFile->respBufLen){
						TRACE(printf("\tFiles ERROR. Not enough memory to read directory info\n\t%s\n",railFile->response););
						break;
					}
				}
				uint8_t st;
				do{
					st = readData(&file,&addr,sizeof(file));
					addr += sizeof(file)+file.fsize;
					if(st==0xff){
						file.fsize = 0xffff;
						TRACE(printf("\tRead Flash data error. MEMORY_OVERFLOW\n"););
						break;
					}
				}while(st);
			}
			snprintf(
				railFile->response+railFile->respLen-1,
				2,
				"]"
			);
			break;
		}
		case DISK_SD_SPI:
		case DISK_FLASH_SPI:
			
		case DISK_FLASH_CAN:
			
			break;
			
		default:
			TRACE(printf("\tFiles ERROR. Drive not found\n"););
			return EXIT_FAILURE;
	}
	railHitch->respLen =
			snprintf(
				railHitch->response,
				railHitch->respBufLen,
				HEADER_OK_JSON,
				railFile->respLen
			);
	return EXIT_SUCCESS;
}

int getFileHttp(RailHttp_st *railHttp,RailFiles_st *railFile){
	TRACE(printf("Get File\n"););

	FileDesc_st *fileD = findFile(railHttp->argv[0]);
	if(fileD){
		railFile->response = (uint8_t*)fileD->file;
		railFile->respLen = fileD->fsize;

		railHttp->respLen =
			snprintf(
				railHttp->response,
				railHttp->respBufLen,
				HEADER_OK_TEXT,
				fileD->content,
				railFile->respLen
			);
	}
	else{
	}
	return EXIT_SUCCESS;
}



static Parcel_st filesParcel[4];
static Train_st filesTrain;
static Parcel_st *pBox[4];
void filesStationInit(void){
	filesTrain.box = pBox;//malloc(sizeof(Parcel_st*)*4);
	for(uint16_t iParcel=0;iParcel<4;iParcel++){
		filesTrain.box[iParcel] = (Parcel_st*)&filesParcel[iParcel];
	}
	filesTrain.capacity = 4;
	filesTrain.route = ROUTE_FILES;
	fillDepot(&filesTrain);

}

/*
 * build a railway to Json
 */
int filesStation(void *p){

	uint16_t iBox = meetTrainBox(&filesTrain,0);
	Parcel_st *box = filesTrain.box[iBox];
	while(box){
		if(iBox>filesTrain.capacity) return EXIT_SUCCESS;
		
		static void *car;
		car = box->parcel;
		
		for(uint8_t iHitch=0;iHitch<(2-1);iHitch++){
			car = ((Hitch_st*)car)->car;
		}

		if(car==NULL) return EXIT_FAILURE;
		
		else if( ROUTE_FILE_GET == ((RailHttp_st *)car)->command ){

			
			RailHttp_st *railHttp = ((RailHttp_st *)car);

			
			static RailFiles_st railFiles_0;

			TRACE(printf("ROUTE_FILE_GET\n\t%s\n",railHttp->argv[0]););
			
			if(getFileHttp(railHttp,&railFiles_0) ==EXIT_SUCCESS){

			}
			else{
				TRACE(printf("ROUTE_FILE_GET error\n"););
			}

			
			railFiles_0.command = COMMAND_HTTP_RESPONSE;
			railHttp->car = &railFiles_0;
			
			sendTrainsFromDepot(ROUTE_FILES,ROUTE_TCP,box->parcel);
		}
		
		else if( ROUTE_FILE_PUT == ((RailHttp_st *)car)->command ){

			
			RailHttp_st *railHttp = ((RailHttp_st *)car);

			
			static RailFiles_st railFiles_1;

			TRACE(printf("ROUTE_FILE_PUT\n\t%s\n",railHttp->argv[0]););
			
			if(saveFile(railHttp,&railFiles_1) ==EXIT_SUCCESS){

			}
			else{
				TRACE(printf("ROUTE_FILE_PUT error\n"););
			}

			
			railFiles_1.command = COMMAND_HTTP_RESPONSE;
			railHttp->car = &railFiles_1;
			
			sendTrainsFromDepot(ROUTE_FILES,ROUTE_TCP,box->parcel);
		}
		
		else if( ROUTE_DIR_GET == ((RailHttp_st *)car)->command ){

			
			RailHttp_st *railHttp = ((RailHttp_st *)car);

			
			static RailFiles_st railFiles_2;

			TRACE(printf("ROUTE_DIR_GET\n\t%s\n",railHttp->argv[0]););
			
			if(getListDir(railHttp,&railFiles_2) ==EXIT_SUCCESS){

			}
			else{
				TRACE(printf("ROUTE_DIR_GET error\n"););
			}

			
			railFiles_2.command = COMMAND_HTTP_RESPONSE;
			railHttp->car = &railFiles_2;
			
			sendTrainsFromDepot(ROUTE_FILES,ROUTE_TCP,box->parcel);
		}
		
		else{
			
		}
		
		iBox = meetTrainBox(&filesTrain,iBox);
		box = filesTrain.box[iBox];
	}

	return EXIT_SUCCESS;

}


