/*
 * RestFullController.h
 *
 *  Created on: 4 ���. 2018 �.
 *      Author: home
 */

#ifndef RESTFULLCONTROLLER_H_
#define RESTFULLCONTROLLER_H_

int restParser(
	char *request,
	char *restBuf,	uint16_t restBufLen,
	char* command, char *restToken,
	char **argv, uint8_t *argc, uint8_t argcLen
);

#endif /* RESTFULLCONTROLLER_H_ */
