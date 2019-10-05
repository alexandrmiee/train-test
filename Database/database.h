/*
 * database.h
 *
 *  Created on: 2019-10-05T20:40:13.447Z
 *      Author: AlexL
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"


typedef struct LinkedList{
	char *key;
	char *value;
	struct LinkedList *next;
}LinkedList_st;

typedef struct Table{
	char *primary_index;
	char *secondary_index;
	LinkedList_st *parameters;
	struct Table *next;
}DbTable_st;


typedef struct RailDb{
	char *params;
	uint32_t sizeMax;
	int(*function)(struct RailDb *params, Parcel_st* box);
}RailDb_st;

typedef struct module{
  struct {
    char FS[3]; /**buffer for `FS{`*/
    
    char flash_bl[9]; /**buffer for key `flash_bl:`*/
    char flash_blVal[1]; /**buffer for value, set default value to `2`*/
    
    char flash_boot[11]; /**buffer for key `flash_boot:`*/
    char flash_bootVal[1]; /**buffer for value, set default value to `1`*/
    
    char flash_can[10]; /**buffer for key `flash_can:`*/
    char flash_canVal[1]; /**buffer for value, set default value to `5`*/
    
    char flash_db[9]; /**buffer for key `flash_db:`*/
    char flash_dbVal[1]; /**buffer for value, set default value to `0`*/
    
    char flash_mc[9]; /**buffer for key `flash_mc:`*/
    char flash_mcVal[1]; /**buffer for value, set default value to `6`*/
    
    char flash_spi[10]; /**buffer for key `flash_spi:`*/
    char flash_spiVal[1]; /**buffer for value, set default value to `4`*/
    
    char sd_spi[7]; /**buffer for key `sd_spi:`*/
    char sd_spiVal[1]; /**buffer for value, set default value to `3`*/
    char FSPad[1]; /**padding for `}`*/
  }__attribute__((packed))FS;
  struct {
    char HTTP[5]; /**buffer for `HTTP{`*/
    
    char errors[7]; /**buffer for key `errors:`*/
    char errorsVal[1]; /**buffer for value, set default value to `0`*/
    
    char isEnable[9]; /**buffer for key `isEnable:`*/
    char isEnableVal[1]; /**buffer for value, set default value to `y`*/
    
    char port[5]; /**buffer for key `port:`*/
    char portVal[2]; /**buffer for value, set default value to `80`*/
    
    char type[5]; /**buffer for key `type:`*/
    char typeVal[1]; /**buffer for value, set default value to `s`*/
    char HTTPPad[1]; /**padding for `}`*/
  }__attribute__((packed))HTTP;
  struct {
    char Telnet[7]; /**buffer for `Telnet{`*/
    
    char errors[7]; /**buffer for key `errors:`*/
    char errorsVal[1]; /**buffer for value, set default value to `0`*/
    
    char isEnable[9]; /**buffer for key `isEnable:`*/
    char isEnableVal[1]; /**buffer for value, set default value to `y`*/
    
    char name[5]; /**buffer for key `name:`*/
    char nameVal[1]; /**buffer for value, set default value to `&gt;`*/
    
    char port[5]; /**buffer for key `port:`*/
    char portVal[2]; /**buffer for value, set default value to `23`*/
    
    char type[5]; /**buffer for key `type:`*/
    char typeVal[1]; /**buffer for value, set default value to `s`*/
    char TelnetPad[1]; /**padding for `}`*/
  }__attribute__((packed))Telnet;
  struct {
    char Therm[6]; /**buffer for `Therm{`*/
    
    char elt[4]; /**buffer for key `elt:`*/
    char eltVal[1]; /**buffer for value, set default value to `0`*/
    
    char err[4]; /**buffer for key `err:`*/
    char errVal[1]; /**buffer for value, set default value to `0`*/
    
    char note[5]; /**buffer for key `note:`*/
    char noteVal[14]; /**buffer for value, set default value to `ds18b20onboard`*/
    
    char period[7]; /**buffer for key `period:`*/
    char periodVal[1]; /**buffer for value, set default value to `0`*/
    
    char thl[4]; /**buffer for key `thl:`*/
    char thlVal[2]; /**buffer for value, set default value to `35`*/
    
    char thu[4]; /**buffer for key `thu:`*/
    char thuVal[2]; /**buffer for value, set default value to `20`*/
    
    char type[5]; /**buffer for key `type:`*/
    char typeVal[3]; /**buffer for value, set default value to `rin`*/
    
    char units[6]; /**buffer for key `units:`*/
    char unitsVal[6]; /**buffer for value, set default value to `&amp;deg;C`*/
    
    char value[6]; /**buffer for key `value:`*/
    char valueVal[1]; /**buffer for value, set default value to `0`*/
    char ThermPad[1]; /**padding for `}`*/
  }__attribute__((packed))Therm;
  
}__attribute__((packed)) ModuleDb_st;

uint8_t strConfigLen(char *param);
uint8_t strConfigTableLen(char *param);
int reindex(char *address, uint32_t sizeMax);
DbTable_st * getDbInstance(void);
ModuleDb_st *getDbConfig(void);


int findValues(char **argv,uint32_t resultLenMax,uint16_t* argc,DbTable_st *db,char* name,char *key, char *value);
int findTablesWithKeyValue(char **argv,uint32_t resultLenMax,uint16_t* argc,DbTable_st *db,char* name,char *key, char *value);
int dbStation(void *p);
void dbStationInit(void);

#endif /* DATABASE_H_ */


