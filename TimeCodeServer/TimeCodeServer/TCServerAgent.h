#pragma once

#ifndef _TCServerAgent_H_
#define _TCServerAgent_H_

#include "TCServerDll.h"

class TCServerAgent
{
	TCServerDll* m_pTCServerDll;

	HWND m_hWnd;

	HANDLE m_hTCThread;
	bool m_bIsAlive;
	CRITICAL_SECTION m_CriticalSection;

	unsigned long m_ulCurTimeCode;
	DWORD m_dwLastTime;

protected:
public:
	TCServerAgent();
	~TCServerAgent();

	void Init(HWND hWnd);

	int StartServer(int iPort);
	void StopServer();
	
	//void UpdateTimeCode(unsigned long ulTimeCode);
	void SetTotalOfTimeCode(unsigned long ulTimeCode);
	void TC_Start();
	void TC_Stop();
	void TC_Pause();

	bool IsAlive();
	void TimeCodeLoop();
	
};

#endif