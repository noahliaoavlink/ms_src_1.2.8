#pragma once

#ifndef _NetCommon_H_
#define _NetCommon_H_

#define _MAX_BUFFER_LEN 65536
#define _MAX_RCV_BUFFER 65536
#define _MAX_SEND_BUFFER 10240
#define _MAX_LINE_BUFFER 2048

enum PacketType
{
	PT_CMD_ONLY = 0,
	PT_SIMPLE,
	PT_STRING,
	PT_STRING2,
	PT_DUAL_LONG,
};

enum TCCommandID
{
	CID_LOGIN = 101,
	CID_LOGOUT,
	CID_START,
	CID_STOP,
	CID_PAUSE,
	CID_UPDATE_TIME_CODE,
	CID_DO_SYN_PB,
	CID_CONTINUE,
	CID_DISCONNECT,
	CID_CLIENT_EVENT,
	CID_SERVER_EVENT,
	CID_REPORT,
	CID_HELLO,

	CID_LOGIN_RET = 1001,
};

enum LoginResult
{
	LR_FAILED = 0,
	LR_SUCESSED,
};

enum TCStatus
{
	TCS_STOP = 0,
	TCS_RUNNING,
	TCS_PAUSE,
};

enum ClientReceiveStatus
{
	CRS_NONE = 0,
	CRS_NORMAL,
	CRS_HUNGER,
};

enum ReconnectStatus
{
	RS_NONE = 0,
	RS_PING,
	RS_RECONNECT,
	RS_LOGIN,
	RS_LOGIN_RET,
	RS_FINISHED,
};

typedef struct
{
	char szServerVersion[10];
	char szProtocolVersion[10];  //network protocol version
	char szIP[100];
	int iPort;
}BCUDPServerInfo;

/*
<NP + packet_type(1) + cmd_id(4) + data_len(4) + data(?) + NP>

packet_type 0 - cmd only
packet_type 1 - cmd + simple data (ex. int )
packet_type 2 - cmd + string data(len + data)
packet_type 3 - cmd + string1(len + data) ¡Ï string2(len + data)

login , logout , start , stop , pause , update_time_code
*/

#endif
