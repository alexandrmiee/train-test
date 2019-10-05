/*
 * flash.c
 * Autogenerated code from TrainFramework
 *  Created on: 2019-10-05T20:40:13.466Z
 *      Author: 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#include "JDEC.h"
#include "flash.h"


#define NONE
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif



typedef struct{
	uint8_t sectorStart;
	uint8_t sectorEnd;
	uint8_t sectorCur;
	uint32_t sectorLast;
	uint8_t *buf;
	uint8_t *cbuf;
	uint32_t len;
	uint32_t clen;
	uint32_t addr;
	uint32_t addrMax;
	uint32_t overflow;
	uint8_t isEnd;
}__attribute__ ((__packed__)) FlashAddr_st;

static uint8_t memBufRx[64];
static uint8_t memBufTx[64];
static uint8_t memBufRest[PAGE_SIZE];

extern uint8_t flashTestArray[1280];
uint8_t calcSectors(FlashAddr_st *flash){
	flash->sectorStart = ((flash->addr)/SECTOR_SIZE)*SECTOR_SIZE;
	flash->sectorCur = flash->sectorStart;
	flash->sectorEnd = (((flash->addr)+flash->len)/SECTOR_SIZE)*SECTOR_SIZE;
	#ifndef HAL
	flash->sectorStart = (((uint8_t*)flash->addr-flashTestArray)/SECTOR_SIZE)*SECTOR_SIZE;
	flash->sectorCur = flash->sectorStart;
	flash->sectorEnd = ((((uint8_t*)flash->addr-flashTestArray)+flash->len)/SECTOR_SIZE)*SECTOR_SIZE;
	#endif
	return 0;
}

uint8_t eraseData(FlashAddr_st *flash){
	if(flash->sectorCur<=flash->sectorEnd && flash->sectorLast != flash->sectorCur){
		if(!eraseSector(flash->sectorCur)){
			flash->sectorLast = flash->sectorCur;
			flash->sectorCur+=SECTOR_SIZE;
		}
		return 1;
	}
	else{
		return 0;
	}
}

uint8_t restFill(FlashAddr_st *flash){
	uint32_t rest = ((PAGE_SIZE-flash->overflow)>flash->clen)?flash->clen:PAGE_SIZE-flash->overflow;
	flash->buf = flash->cbuf+rest;
	flash->len = flash->clen-rest;
	memcpy(&memBufRest[flash->overflow],flash->cbuf,rest);
	flash->overflow += rest;
	return 0;
}

uint8_t restWrite(FlashAddr_st *flash){
	if(flash->addrMax<(flash->addr+PAGE_SIZE)) return MEMORY_OVERFLOW;
	if(flash->overflow == PAGE_SIZE){
		uint8_t status = writeFlash(memBufRest,flash->addr,PAGE_SIZE);
		if(status) return status;
		flash->addr += PAGE_SIZE;
		flash->overflow -= PAGE_SIZE;
		memset(memBufRest,0xff,PAGE_SIZE);
	}
	return 0;
}

uint8_t restSync(FlashAddr_st *flash){
	if(flash->overflow && flash->isEnd){
		if(flash->addrMax<(flash->addr+PAGE_SIZE)) return MEMORY_OVERFLOW;
		uint8_t status = writeFlash(memBufRest,flash->addr,PAGE_SIZE);
		if(status) return status;
		flash->addr += PAGE_SIZE;
		memset(memBufRest,0xff,PAGE_SIZE);
		TRACE(printf("\tdata (text)\n%s\n",(char*)flashTestArray););
	}
	return 0;
}

uint8_t writePages(FlashAddr_st *flash){
	uint32_t lenIntPage = flash->len/PAGE_SIZE*PAGE_SIZE;
	if(lenIntPage){
		if(flash->addrMax<(flash->addr+flash->len)) return MEMORY_OVERFLOW;
		uint8_t status = writeFlash(flash->buf,flash->addr,lenIntPage);
		if(status) return status;
		flash->addr += lenIntPage;
		flash->buf += lenIntPage;
	}
	if(flash->len){
		flash->overflow = flash->len-lenIntPage;
		memcpy(memBufRest,flash->buf,flash->overflow);
	}
	return 0;
}

static FlashAddr_st flash;
uint8_t setAddress(uint32_t addrStart,uint32_t addrEndMax){
	flash.addr = addrStart;
	flash.addrMax = addrEndMax;
	flash.sectorLast = SECTOR_SIZE - 1;
	return 0;
}
uint32_t getAddress(void){
	return flash.addr;
}
uint8_t writeChunk(uint8_t *buf, uint32_t len,uint8_t isEnd){
	enum States {
		
		initialize = 0,
		
		erase = 1,
		
		rest = 2,
		
		writeRest = 3,
		
		writeData = 4,
		
		sync = 5,
		
		error
	};

	flash.cbuf = buf;
	flash.clen = len;
	flash.isEnd = isEnd;
	static enum States state =  initialize;


	switch (state) {
		default:
			state = error;
		break;
		case error:
			state = initialize;
		
		case initialize:
			if(!calcSectors(&flash)){
				state = erase;
				TRACE(printf("\tinitialize done, len %d\n",len););
			}
			else{
				break;
			}
		
		
		case erase:
			if(!eraseData(&flash)){
				state = rest;
				TRACE(printf("\terase done, len %d\n",len););
			}
			else{
				break;
			}
		
		
		case rest:
			if(!restFill(&flash)){
				state = writeRest;
				TRACE(printf("\trest done, len %d\n",len););
			}
			else{
				break;
			}
		
		
		case writeRest:
			if(!restWrite(&flash)){
				state = writeData;
				TRACE(printf("\twriteRest done, len %d\n",len););
			}
			else{
				break;
			}
		
		
		case writeData:
			if(!writePages(&flash)){
				state = sync;
				TRACE(printf("\twriteData done, len %d\n",len););
			}
			else{
				break;
			}
		
		
		case sync:
			if(!restSync(&flash)){
				state = initialize;
				TRACE(printf("\tsync done, len %d\n",len););
			}
			else{
				break;
			}
		
		
	}
	return state;
}

uint8_t readData(uint8_t *buf, uint32_t *addr, uint32_t len){
	*addr = (!*addr)?FLASH_ADDRESS_START:*addr;
	if(*addr+len>FLASH_ADDRESS_END){
		TRACE(printf("\tRead Flash data error. MEMORY_OVERFLOW %d\n",len););
		return MEMORY_OVERFLOW;
	}
	return readFlash(buf,*addr,len);
}


