/*
 * database.c
 *
 *  Created on: 2019-10-05T20:40:13.446Z
 *      Author: AlexL
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#include "database.h"


#ifdef WIN32
#include<winsock2.h>
#endif

#define DB_TABLE_KEYS_MAX	30	//@db.table.keys.max
#define DB_TABLE_KEY_SEPARATOR_START "{"	//@db.table.keys.separator.start
#define DB_TABLE_KEY_SEPARATOR_VAL	":"	//@db.table.keys.separator.val
#define DB_TABLE_KEY_SEPARATOR_NEW	","	//@db.table.keys.separator.new
#define DB_TABLE_KEY_SEPARATOR_END	"}"	//@db.table.keys.separator.end

//#define DB_TRACE
//#undef DB_TRACE

#define NONE
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif

DbTable_st *dbIndexTable;

DbTable_st * getDbInstance(){
	return dbIndexTable;
}
/*
 * parcer configuration file,
 * file structure
 * a{b:c,d:e,...,f:g,}h{..}...{...}'0'
 * primary key -> a
 * secondary key -> not set here
 * key[0] -> b
 * value[0] -> c
 */
DbTable_st dbTable[];
LinkedList_st ll[];
int reindex(char *address, uint32_t sizeMax){
	if(address==0) return EXIT_FAILURE;
	char *db = address;

//	DbTable_st *table=NULL;
//	table = malloc(sizeof(DbTable_st));
//	memset(table,0,sizeof(DbTable_st));
	uint16_t 	iTable = 0,
				iLlist = 0;
	DbTable_st *table = &dbTable[iTable++];
	if(!table) return EXIT_FAILURE;
	dbIndexTable = table;
	while(*db){
		//db=&a
		table->primary_index = db;
		TRACE(printf("pi %s\n",table->primary_index););
		//db=&b
		db = strstr(db,DB_TABLE_KEY_SEPARATOR_START)+1;
		uint8_t i=0;
		//end=&h
		char *end = strstr(db,DB_TABLE_KEY_SEPARATOR_END);
		TRACE(printf("end %s\n",end););
		if(!table->parameters){
//			table->parameters = malloc(sizeof(LinkedList_st));
			table->parameters = &ll[iLlist++];
			if(!table->parameters) return EXIT_FAILURE;
		}
		LinkedList_st *parameters = table->parameters;
		memset(parameters,0,sizeof(LinkedList_st));
		while(i++<DB_TABLE_KEYS_MAX){
			//key[0]=&b
			parameters->key = db;
			//TRACE(printf("key %s\n",parameters->key););
			//value[0]=&c
			parameters->value = strstr(db,DB_TABLE_KEY_SEPARATOR_VAL)+1;
			//TRACE(printf("val %s\n",parameters->value););
			//db=&d
			db = strstr(db,DB_TABLE_KEY_SEPARATOR_NEW)+1;
			TRACE(printf("%s,%p,%p\n",db,db,end););
			if(db>=end)break;
			if(!parameters->next){
//				parameters->next = malloc(sizeof(LinkedList_st));
				parameters->next = &ll[iLlist++];;
				if(!parameters->next) return EXIT_FAILURE;
			}
			parameters = parameters->next;
			memset(parameters,0,sizeof(LinkedList_st));
		}
		//db=&h
		db = end+1;
		if(!(*db)) break;
//		table->next = malloc(sizeof(DbTable_st));
		table->next = &dbTable[iTable++];
		if(!table->next) return EXIT_FAILURE;
		table = table->next;
		memset(table,0,sizeof(DbTable_st));
	}//EOF

	return EXIT_SUCCESS;
}

/*
 * find end of parameter
 * char *param start address for search
 * return length of parameter string in bytes
 * end separators
 *  DB_TABLE_KEY_SEPARATOR_END
 *  DB_TABLE_KEY_SEPARATOR_NEW
 *  DB_TABLE_KEY_SEPARATOR_START
 *  DB_TABLE_KEY_SEPARATOR_VAL
 */
uint8_t strConfigLen(char *param){
	uint8_t i = 0;
	while(
			param[i]
			&& param[i] != *DB_TABLE_KEY_SEPARATOR_END
			&& param[i] != *DB_TABLE_KEY_SEPARATOR_NEW
			&& param[i] != *DB_TABLE_KEY_SEPARATOR_START
			&& param[i] != *DB_TABLE_KEY_SEPARATOR_VAL
	){i++;}
	return i;
}

/*
 * find end of table
 * char *param start address for search
 * return length of parameter string in bytes
 * end separators
 *  DB_TABLE_KEY_SEPARATOR_END
 */
uint8_t strConfigTableLen(char *param){
	uint8_t i = 0;
	while(
			param[i]
			&& param[i] != *DB_TABLE_KEY_SEPARATOR_END
	){i++;}
	return i;
}

/*
 * loader in for a train
 * RailDb_st *rail message protocol
 * Parcel_st *msg message from sender
 */
int setIndex(RailDb_st *rail, Parcel_st *msg){
	TRACE(printf(">>receive parcel from %I64u\n",msg->sender););
	if(!rail->params) return EXIT_FAILURE;
	reindex(rail->params,rail->sizeMax);
	/*
	 * yes, parcel was arrived and database's indexes was updated
	 */
	//sendTrain(msg->from,msg->address,NULL);
	return EXIT_SUCCESS;
}



/*
 * find table in database with name, key and value
 * if parameter equal '*' - find all
 * char **result array of pointers to return results
 * uint32_t resultLenMax size of array result
 * DbTable_st *db database
 * char* name primary key
 * char *key parameter
 * char *value
 */
int findTablesWithKeyValue(char **argv,uint32_t resultLenMax,uint16_t* argc,DbTable_st *db,char* name,char *key, char *value){
	if(!db) return EXIT_FAILURE;
	TRACE(printf("Database search tables %s where key %s = %s in %p\n",name,key,value,db););
	uint16_t nameSize = strlen(name);
	uint16_t keySize = strlen(key);
	uint16_t valueSize = strlen(value);
	DbTable_st *table = db;
	while(table){
		if( !equals(name,"*",1) || !equals(table->primary_index, name,nameSize)){
			LinkedList_st *parameter = table->parameters;
			while(parameter){
				if(  !equals(key,"*",1) || !equals(parameter->key,key,keySize)){
					if( !equals(value,"*",1) || !equals(parameter->value,value,valueSize)){
						TRACE(printf("Find \n\t%.1s \t%.1s \t%.1s \n",table->primary_index,parameter->key,parameter->value););
						argv[*argc] = (char*)table;
						*argc += 1;
						break;
					}
				}
				parameter = parameter->next;
			}
		}
		if( (*argc)>resultLenMax ){
			TRACE(printf("findTablesWithKeyValue error: (*argc)>resultLenMax"););
			return EXIT_FAILURE;
		}
		table = table->next;
	}
	return EXIT_SUCCESS;
}

int findValues(char **argv,uint32_t resultLenMax,uint16_t* argc,DbTable_st *db,char* name,char *key, char *value){
	if(!db) return EXIT_FAILURE;
	TRACE(printf("Database search values %s where key %s = %s in %p\n",name,key,value,db););
	uint16_t nameSize = strlen(name);
	uint16_t keySize = strlen(key);
	uint16_t valueSize = strlen(value);
	DbTable_st *table = db;
	while(table){
		if( !equals(name,"*",1) || !equals(table->primary_index, name,nameSize)){
			LinkedList_st *parameter = table->parameters;
			while(parameter){
				if(  !equals(key,"*",1) || !equals(parameter->key,key,keySize)){
					if( !equals(value,"*",1) || !equals(parameter->value,value,valueSize)){
						TRACE(printf("Find \n\t%.1s \t%.1s \t%.1s \n",table->primary_index,parameter->key,parameter->value););
						argv[*argc] = parameter->value;
						*argc += 1;
						break;
					}
				}
				parameter = parameter->next;
			}
		}
		if( (*argc)>resultLenMax ){
			TRACE(printf("findTablesWithKeyValue error: (*argc)>resultLenMax"););
			return EXIT_FAILURE;
		}
		table = table->next;
	}
	return EXIT_SUCCESS;
}

char dbConfig[] =
  "FS{"
    "flash_bl:2,"
    "flash_boot:1,"
    "flash_can:5,"
    "flash_db:0,"
    "flash_mc:6,"
    "flash_spi:4,"
    "sd_spi:3,"
    "}"
  "HTTP{"
    "errors:0,"
    "isEnable:y,"
    "port:80,"
    "type:s,"
    "}"
  "Telnet{"
    "errors:0,"
    "isEnable:y,"
    "name:&gt;,"
    "port:23,"
    "type:s,"
    "}"
  "Therm{"
    "elt:0,"
    "err:0,"
    "note:ds18b20onboard,"
    "period:0,"
    "thl:35,"
    "thu:20,"
    "type:rin,"
    "units:&amp;deg;C,"
    "value:0,"
    "}"
  
;

ModuleDb_st *getDbConfig(){
	return (ModuleDb_st*)dbConfig;
}

static Parcel_st dbParcel[1];
static Train_st dbTrain;
static Parcel_st* pBox[1];
void dbStationInit(void){
	dbTrain.box = pBox;
	for(uint16_t iParcel=0;iParcel<1;iParcel++){
		dbTrain.box[iParcel] = (Parcel_st*)&dbParcel[iParcel];
	}
	dbTrain.capacity = 1;
	dbTrain.route = ROUTE_DATABASE;
	fillDepot(&dbTrain);
	reindex(dbConfig,sizeof(dbConfig));
}

int dbStation(void *p){
	Parcel_st *msg=meetTrain(&dbTrain);
	while(msg){
		msg=meetTrain(&dbTrain);
	}

	return EXIT_SUCCESS;
}


