#pragma once

#ifndef _AutoPBHelper_H
#define _AutoPBHelper_H

#include "ThreadBase.h"
#include "TimelineManager.h"

class AutoPBHelper : public ThreadBase
{
	int m_iStatus;
	DWORD m_dwLastTime;
	int m_iWaitCount;
	TimelineManager* m_pTimelineManager;
public:
	AutoPBHelper();
	~AutoPBHelper();

	void SetTimelineManager(TimelineManager* pObj);
	void Run();
	void CheckStatus();
	void WaitForConnected();
	void WaitForReady();
	void DoPlay();
	void Done();

	virtual void ThreadEvent();
};

#endif