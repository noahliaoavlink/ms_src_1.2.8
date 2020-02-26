#ifndef _NetTester_H
#define _NetTester_H

#include <Winsock2.h>

class NetTester 
{
		bool m_bIsAlive;
		HANDLE m_hThread;
		SOCKET m_Socket;
		bool m_bConnected;
		bool m_bConnectRet;

		int m_iErrorCode;
		bool m_bIsFinished;

		WSAEVENT m_hConnectEvent;
		HANDLE m_hConnectThread;
	public:
		NetTester();
       ~NetTester();

	   bool Startup();
	   void Cleanup();

	   bool Connect(char* szUrl);
	   bool Connect(char* szIP,int iPort);
	   void Close();
	   bool IsAlive();
	   void DoEvent();
	   unsigned long getAddrFromIP(char *ip);
	   bool IsFinished();

	   bool WaitForResult(bool* pValue,int iWaitTime);
	   bool WaitForConnection(int iWaitTime);
};

#endif