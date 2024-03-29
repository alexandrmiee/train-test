/*
 *	Autogenerated code from TrainFramework
 *  Created on: 2019-10-05T20:40:13.488Z
 *      Author: AL
 */

#ifndef TCP_H_
#define TCP_H_

#define TCP_ANSWER_LEN 		1024

#define TCP_FLAG_URG		0x20
#define TCP_FLAG_ACK		0x10
#define TCP_FLAG_PSH		0x08
#define TCP_FLAG_RST		0x04
#define TCP_FLAG_SYN		0x02
#define TCP_FLAG_FIN		0x01

#define TCP_OPTION_PUSH			0x01
#define TCP_OPTION_CLOSE		0x02
#define TCP_OPTION_NEW			0x03

#define TCP_HEAD_SIZE(tcp)		(((tcp)->data_offset & 0xf0) >> 2)
#define TCP_GET_DATA(tcp)		((uint8_t*)(tcp) + TCP_HEAD_SIZE(tcp))

#define TCP_MAX_CONNECTIONS     4
#define TCP_WINDOW_SIZE			NET_BUF_MAX-64
#define TCP_TIMEOUT             1//600*_s //s
#define TCP_SYN_MSS             128

#define TCP_REXMIT_TIMEOUT 		TCP_TIMEOUT
#define TCP_REXMIT_LIMIT 		2

typedef enum {
	TCP_CLOSED,
	LISTEN,
	TCP_SYN_SENT,
	TCP_SYN_RECEIVED,
	TCP_ESTABLISHED,
	TCP_FIN_WAIT,
	TCP_FIN_WAIT_1,
	TCP_FIN_WAIT_2,
	LAST_ACK,
	TIME_WAIT,
	CLOSING,
}__attribute__ ((__packed__)) tcpStatusCode_e;

typedef enum {
	TCP_SENDING_SEND,
	TCP_SENDING_REPLY,
	TCP_SENDING_RESEND,
	TCP_SENDING_NEWMES
}__attribute__ ((__packed__)) tcpSendingMode_e;

typedef struct TcpPacket {
	uint16_t from_port;
	uint16_t to_port;
	uint32_t seq_num;
	uint32_t ack_num;
	uint8_t data_offset;
	uint8_t flags;
	uint16_t window;
	uint16_t cksum;
	uint16_t urgent_ptr;
	uint8_t *data;
}__attribute__ ((__packed__)) TcpPacket_st;

typedef struct{
	int id;
	tcpStatusCode_e status;
	uint32_t event_time;
	uint32_t seq_num;
	uint32_t ack_num;
	uint32_t remote_addr;
	uint16_t remote_port;
	uint16_t local_port;
	uint8_t is_closing;
	uint8_t rexmit_count;
	uint32_t seq_num_saved;
	uint8_t stream;
}__attribute__ ((__packed__)) TcpSocket_st;

typedef struct RailTcp {
	void *car;
	uint64_t command;
	uint8_t* response;
	uint16_t respLen;
	uint16_t respBufLen;
	TcpPacket_st* packet;
	TcpSocket_st* socket;
	uint8_t *request;
	uint16_t requestLen;
}__attribute__ ((__packed__)) RailTcp_st;

int getSocket();
int _initNetAdapter(uint32_t inetAddress,uint16_t port, uint8_t protocol);
int initAdapter(void);
int initServer(uint32_t inetAddress,uint16_t port, uint8_t protocol);
int initClient(uint32_t inetAddress,uint16_t port, uint8_t protocol);
int getClientSocket();
int getServerSocket();
int getListenSocket();

int tcpStation(void *p);
void tcpStationInit(void);

#endif /* TCP_H_ */
