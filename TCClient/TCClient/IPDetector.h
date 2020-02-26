#pragma once

#ifndef _IPDetector_H
#define _IPDetector_H

#include "UDPClient.h"
//#include <Winsock.h>

class IPDetector
{
	SOCKET m_ClientSocket;

	char m_szTargetIP[128];
	char m_RecieveBuf[_BUF_SIZE];
	bool m_bFinished;
	unsigned long m_ulStartTime;
	bool m_bIsConnected;

	bool m_bIsAlive;
	HANDLE m_hThread;
	CRITICAL_SECTION m_CriticalSection;
public:
	IPDetector();
	~IPDetector();

	bool Initialize();
	bool UnInitialize();
	void SetTargetIP(char* szIP);
	void Open();
	void Close();
	bool IsAlive();
	void Recieve();
	void WaitForFinished();
	bool IsConnected();
};

#endif