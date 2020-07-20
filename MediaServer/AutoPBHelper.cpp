#include "stdafx.h"
#include "AutoPBHelper.h"
#include "Mmsystem.h"

#define _APBH_INTERVAL 100

enum AutoPBHelperStatus
{
	APBHS_NORMAL = 0,
	APBHS_WAIT_FOR_CONNECTED,
	APBHS_WAIT_FOR_READY,
	APBHS_DO_PLAY,
	APBHS_DONE,
};

AutoPBHelper::AutoPBHelper()
{
	m_iStatus = APBHS_NORMAL;
	m_dwLastTime = 0;
	m_iWaitCount = 0;
}

AutoPBHelper::~AutoPBHelper()
{
}

void AutoPBHelper::SetTimelineManager(TimelineManager* pObj)
{
	m_pTimelineManager = pObj;
}

void AutoPBHelper::Run()
{
	m_iStatus = APBHS_NORMAL;
	ThreadBase::Start();
}

void AutoPBHelper::ThreadEvent()
{
	DWORD dwCurTime = timeGetTime();

	if (m_dwLastTime == 0)
		m_dwLastTime = dwCurTime;
	else
	{
		if (m_dwLastTime + _APBH_INTERVAL <= dwCurTime)
		{
			m_dwLastTime = dwCurTime;
			CheckStatus();
		}
	}
	Sleep(1);
}

void AutoPBHelper::CheckStatus()
{
	switch (m_iStatus)
	{
	case APBHS_NORMAL:
		m_iStatus = APBHS_WAIT_FOR_CONNECTED;
		break;
	case APBHS_WAIT_FOR_CONNECTED:
		WaitForConnected();
		break;
	case APBHS_WAIT_FOR_READY:
		WaitForReady();
		break;
	case APBHS_DO_PLAY:
		DoPlay();
		break;
	case APBHS_DONE:
		Done();
		break;
	}
}

void AutoPBHelper::WaitForConnected()
{
	auto tc_adapter = m_pTimelineManager->GetTCAdapter();
	if (tc_adapter->GetTotalOfClients() > 0)
	{
		m_iStatus = APBHS_WAIT_FOR_READY;
		m_iWaitCount = 5;
	}
}

void AutoPBHelper::WaitForReady()
{
	if (m_iWaitCount > 0)
		m_iWaitCount--;
	else
	{
		m_iStatus = APBHS_DO_PLAY;
	}
}

void AutoPBHelper::DoPlay()
{
	CWnd* pEditViewWnd = m_pTimelineManager->GetEditViewWnd();
	::SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PLAY2, 0, 0);

	auto cur_timecode = m_pTimelineManager->GetTotalTimeOfCurrentItem();

	TCAdapter* pTCAdapter = m_pTimelineManager->GetTCAdapter();
	if (pTCAdapter)
	{
		pTCAdapter->SetTotalOfTimeCode(cur_timecode.TotalMilliseconds());
		pTCAdapter->TC_Start();
	}

	m_iStatus = APBHS_DONE;
}

void AutoPBHelper::Done()
{
	ThreadBase::Stop();
}