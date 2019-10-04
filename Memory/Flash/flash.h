#ifndef _FLASH_H_
#define _FLASH_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#define MEMORY_OVERFLOW	255

#define SECTOR_SIZE	128

#define PAGE_SIZE	32

#define BOOT_PAGE	0

#define DATABASE_PAGE	64

#define FLASH_ADDRESS_START	(uint32_t)flashTestArray

#define FLASH_ADDRESS_END	(uint32_t)flashTestArray+sizeof(flashTestArray)


uint8_t writeChunk(uint8_t *buf, uint32_t len,uint8_t isEnd);

#endif /*_FLASH_H_*/

