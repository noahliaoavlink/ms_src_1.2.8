#pragma once

#ifndef _ConnectHelper_H
#define _ConnectHelper_H

#include "ThreadBase.h"
#include "TimelineManager.h"

class ConnectHelper  :  public ThreadBase
{
	int m_iStatus;
	DWORD m_dwLastTime;
	int m_iWaitCount;
	TimelineManager* m_pTimelineManager;
	char m_szIP[100];
	int m_iPort;
	HWND m_hDlgWnd;
public:
	ConnectHelper();
	~ConnectHelper();

	void SetTimelineManager(TimelineManager* pObj);
	void SetTimelineDlgWnd(HWND hWnd);
	void Run();
	void CheckStatus();
	void Search();
	void WaitForSearchResult();
	void Connecting();
	void WaitForConnectResult();
	void Done();

	virtual void ThreadEvent();
};

#endif