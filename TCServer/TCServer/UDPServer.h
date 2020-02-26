#ifndef _UDPServer_H
#define _UDPServer_H

#include <Winsock.h>
#include "..\..\Include\NetCommon.h"

#define _BUF_SIZE 2048
/*
typedef struct
{
	char szServerVersion[10];
	char szProtocolVersion[10];  //network protocol version
	char szIP[100];
	int iPort;
}BCUDPServerInfo;
*/
class UDPServer 
{
	int m_iErrorCode;

	bool m_bIsAlive;
	//bool m_bFinished;
	unsigned long m_ulLastTime;

	BCUDPServerInfo m_Server_Info;
	char m_szSendBuf[_BUF_SIZE];

	HANDLE m_hThread;
	CRITICAL_SECTION m_CriticalSection;
   public:
	    UDPServer();
        ~UDPServer();

		void SetInfo(char* szServerVersion,char* szProtocolVersion,char* szIP,int iPort);

		bool Startup();
		void Cleanup();
		
		int SendToClient();

		void Open();
		void Close();
		bool IsAlive();
		void TimeEvent();
};

#endif