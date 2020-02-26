#ifndef _UDPClient_H
#define _UDPClient_H

#include <Winsock.h>
#include "..\\..\\Include\\SQList.h"
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

class UDPClient 
{
	SOCKET m_ClientSocket;
	int m_iErrorCode;

	SQList* m_pRecieveList;
	bool m_bIsAlive;
	bool m_bFinished;

	char m_RecieveBuf[_BUF_SIZE];

	HANDLE m_hThread;
	CRITICAL_SECTION m_CriticalSection;
	unsigned long m_ulStartTime;
   public:
	    UDPClient();
        ~UDPClient();

		bool Startup();
		void Cleanup();

		int Open();
		void Close();
		void Recieve();
		//void AddToRecieveList(char* szServerIP);
		void AddToRecieveList(BCUDPServerInfo* pServerInfo);

		bool IsAlive();
		void WaitForFinished();
		void* GetRecieveList();
		void ResetRecieveList();

		void CheckDefaultServerInfo();
};

#endif