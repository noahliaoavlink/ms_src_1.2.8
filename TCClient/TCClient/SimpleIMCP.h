#pragma once

#ifndef _SimpleIMCP_H
#define _SimpleIMCP_H

#include <Winsock.h>

class SimpleIMCP
{
	int m_iErrorCode;
	SOCKET m_hSocket;

	int nSequence;
	int nRoundTripTime;
public:
	SimpleIMCP();
	~SimpleIMCP();

	bool Initialize();
	bool UnInitialize();

	int CreateSocket();
	void Close();
	int Ping(char* szIP);
	int GetRoundTripTime();
};

#endif
