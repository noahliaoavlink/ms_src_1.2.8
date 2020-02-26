#include "stdafx.h"
#include "ConnectHelper.h"
#include "Mmsystem.h"

/*
normal 
search
connect
done
*/

enum ConnectHelperStatus
{
	CHS_NORMAL = 0,
	CHS_SEARCH,
	CHS_WAIT_FOR_SEARCH_RESULT,
	CHS_CONNECTING,
	CHS_WAIT_FOR_CONNECT_RESULT,
	CHS_DONE,
};

#define _CH_INTERVAL 100

ConnectHelper::ConnectHelper()
{
	m_iStatus = CHS_NORMAL;
	m_dwLastTime = 0;
	m_iWaitCount = 0;
	strcpy(m_szIP, "127.0.0.1");
	m_iPort = 2017;
	m_pTimelineManager = 0;
	m_hDlgWnd = 0;
}

ConnectHelper::~ ConnectHelper()
{
}

void ConnectHelper::SetTimelineManager(TimelineManager* pObj)
{
	m_pTimelineManager = pObj;
}

void ConnectHelper::SetTimelineDlgWnd(HWND hWnd)
{
	m_hDlgWnd = hWnd;
}

void ConnectHelper::Run()
{
	m_iStatus = CHS_NORMAL;
	ThreadBase::Start();
}

void ConnectHelper::ThreadEvent()
{
	DWORD dwCurTime = timeGetTime();

	if (m_dwLastTime == 0)
		m_dwLastTime = dwCurTime;
	else
	{
		if (m_dwLastTime + _CH_INTERVAL <= dwCurTime)
		{
			m_dwLastTime = dwCurTime;
			CheckStatus();
		}
	}
}

void ConnectHelper::CheckStatus()
{
	switch (m_iStatus)
	{
		case CHS_NORMAL:
			m_iStatus = CHS_SEARCH;
			break;
		case CHS_SEARCH:
			Search();
			break;
		case CHS_WAIT_FOR_SEARCH_RESULT:
			WaitForSearchResult();
			break;
		case CHS_CONNECTING:
			Connecting();
			break;
		case CHS_WAIT_FOR_CONNECT_RESULT:
			WaitForConnectResult();
			break;
		case CHS_DONE:
			Done();
			break;
	}
}

void ConnectHelper::Search()
{
	m_iStatus = CHS_WAIT_FOR_SEARCH_RESULT;
	m_iWaitCount = 10;

	//auto time_line_manager = m_timelineService.GetTimelineManager();
	auto tc_adapter = m_pTimelineManager->GetTCAdapter();
	tc_adapter->SearchServer();
	//auto pServerList = tc_adapter->GetServerList();
}

void ConnectHelper::WaitForSearchResult()
{
	if(m_iWaitCount > 0)
		m_iWaitCount--;
	else
	{
		auto tc_adapter = m_pTimelineManager->GetTCAdapter();
		auto pServerList = tc_adapter->GetServerList();
		
		if (((SQList*)pServerList)->GetTotal() == 0)
			m_iStatus = CHS_SEARCH;
		else
		{
			BCUDPServerInfo* pCurItem = (BCUDPServerInfo*)((SQList*)pServerList)->Get(0);
			if (pCurItem)
			{
				strcpy(m_szIP,pCurItem->szIP);
				m_iPort = pCurItem->iPort;
				m_iStatus = CHS_CONNECTING;
			}
		}
	}
}

void ConnectHelper::Connecting()
{
	m_iStatus = CHS_WAIT_FOR_CONNECT_RESULT;
	m_iWaitCount = 20;
	auto tc_adapter = m_pTimelineManager->GetTCAdapter();

	tc_adapter->SetServerIP(m_szIP);
	tc_adapter->SetPort(m_iPort);

	tc_adapter->Start();

	if (m_hDlgWnd)
		PostMessage(m_hDlgWnd, WM_SET_NETWORK_INFO, 0, 0);
}

void ConnectHelper::WaitForConnectResult()
{
	if (m_iWaitCount > 0)
		m_iWaitCount--;
	else
	{
		auto tc_adapter = m_pTimelineManager->GetTCAdapter();
		bool bIsWorking =((TCAdapter*) tc_adapter)->IsWorking();
		if (bIsWorking == true)
		{
			m_iStatus = CHS_DONE;

			if (m_hDlgWnd)
			{
				PostMessage(m_hDlgWnd, WM_SET_NETWORK_INFO, 1, 0);
				PostMessage(m_hDlgWnd, WM_UPDATE_UI_STATE, 0, 0);
			}
		}
		else
			m_iStatus = CHS_CONNECTING;
	}
}

void ConnectHelper::Done()
{
	ThreadBase::Stop();
}
