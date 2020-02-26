#pragma once

#ifndef _TCAdapter_H_
#define _TCAdapter_H_

#include "TCServerDll.h"
#include "TCClientDll.h"
#include "LogFileDll.h"

#include "..\..\..\Include\NetCommon.h"
#include "..\..\..\Include\MathCommon.h"

enum TCMode
{
	TCM_NORMAL = 0,
	TCM_SERVER,
	TCM_CLIENT,
	TCM_MIDI_CTRL,
};

class TCAdapter : public EventCallback
{
	int m_iMode;

	char m_szServerIP[64];
	int m_iPort;

	bool m_bDoSynPB;

	int m_iTimeCodeOffset;
	Average m_AvgTimeCodeOffset;
	double m_dOldDisplayPos;
	double m_dOldServerDisplayTime;

	TCServerDll* m_pTCServerDll;
	TCClientDll* m_pTCClientDll;

	bool m_bEnableLog;
	LogFileDll* m_pLogFileDll;
	void* m_pLogFileObj;
protected:
public:
	TCAdapter();
	~TCAdapter();

	void SetMode(int iMode);
	int GetMode();
	void SetServerIP(char* szIP);
	void SetPort(int iPort);
	char* GetServerIP();
	int GetPort();

	void UpdateTimeCodeFromTCEngine(double dTimeCode);
	void UpdateTimeCodeFromTCClient(double dTimeCode);
	void UpdateTimeCodeFromTCClient(double dTimeCode, double dServerDisplayTime);

	void SetTotalOfTimeCode(unsigned long ulTimeCode);
	void TC_Start();
	void TC_Stop();
	void TC_Pause();
	void Start();
	void Stop();
	bool IsWorking();
	void SearchServer();
	void* GetServerList();
	int GetReceiveStatus();
	int GetTotalOfClients();

	//log file
	void CheckLogStatus();
	void SetLogFileDll(LogFileDll* pObj);
	void Add2LogFile(int iLevel, char* szMsg);
	void FreeObj();

	void* Event(int iMsg, void* pParameter1, void* pParameter2);
};

#endif
