#include "stdafx.h"
#include "SynCtrl.h"
#include "MediaServer.h"
#include "Panel.h"
#include "../../../Include/sstring.h"

#include "Mmsystem.h"
#pragma comment(lib, "Winmm.lib")

extern CArray<CPanel*, CPanel*> g_PannelAry;

DWORD WINAPI ShowVideoThreadLoop(LPVOID lpParam);

SynCtrl::SynCtrl()
{
	char szData[32];
	m_dwLastTime = 0;
	m_dwPresentFPSLastTime = 0;
	m_iPresentCount = 0;
	m_iTotalOfOrderItems = 0;
	m_iCurOrderIndex = -1;

	GetPrivateProfileString("Media", "DisplayFPS", "25.0", szData, 32, theApp.m_strCurPath + "Setting.ini");
	m_fFPS = atof(szData);
	m_iInterval = 1000.0 / m_fFPS;

	m_iDelayTime = GetPrivateProfileInt("Media", "Delay", 0, theApp.m_strCurPath + "Setting.ini");

	m_bPlayerWaitForDisplay = GetPrivateProfileInt("Media", "PlayerWaitForDisplay", 0, theApp.m_strCurPath + "Setting.ini");

	m_iAudioDelay = GetPrivateProfileInt("Media", "AudioDelay", 0, theApp.m_strCurPath + "Setting.ini");

	GetPrivateProfileString("Media", "ReSampleRateOffset", "0.0", szData, 32, theApp.m_strCurPath + "Setting.ini");
	m_fReSampleRateOffset = atof(szData);

	m_AvgPresentFPS.SetMax(30);

	LoadDisplayOrder();

	m_hEvent = ::CreateEvent(NULL, TRUE, TRUE, "SynCtrl_Evt");
	//::ResetEvent(m_hEvent);
	::SetEvent(m_hEvent);

#ifdef _MT_MODE
	char szEventName[64];
	for (int i = 0; i < _MAX_PANELS; i++)
	{
		m_hEvent_MT[i] = 0;
		m_hThread[i] = 0;

		sprintf(szEventName, "SynCtrl_Evt%d", i);
		m_hEvent_MT[i] = ::CreateEvent(NULL, TRUE, TRUE, szEventName);

		::ResetEvent(m_hEvent_MT[i]);
	}

	InitializeCriticalSection(&m_CriticalSection);
#endif

//	m_bFinished = true;
}

SynCtrl::~SynCtrl()
{
	if (m_hEvent)
		::CloseHandle(m_hEvent);

#ifdef _MT_MODE
	//::ResetEvent(m_hEvent1);
	//::WaitForSingleObject(m_hEvent2, INFINITE);
	for (int i = 0; i < _MAX_PANELS; i++)
	{
		if (m_hEvent_MT[i])
			::CloseHandle(m_hEvent_MT[i]);

		if (m_hThread[i])
			::WaitForSingleObject(m_hThread[i], 3000);
	}
	DeleteCriticalSection(&m_CriticalSection);
#endif
}

void SynCtrl::Start()
{
	for (int y = 0; y < g_MediaStreamAry.size(); y++)
	{
		for (int i = 0; i < 4; i++)
			g_MediaStreamAry[y]->SetDisplayOrder(i, m_iDisplayOrder[i]);

		g_MediaStreamAry[y]->EnablePlayerWaitForDisplay(m_bPlayerWaitForDisplay);
		g_MediaStreamAry[y]->SetAudioDelay(m_iAudioDelay);
		g_MediaStreamAry[y]->SetReSampleRateOffset(m_fReSampleRateOffset);
	}

	ThreadBase::Start();

#ifdef _MT_MODE
	DWORD nThreadID;
	for (int i = 0; i < _MAX_PANELS; i++)
	{
		m_ThreadItems[i].pObj = this;
		m_ThreadItems[i].iIndex = i;
		m_hThread[i] = CreateThread(0, 0, ShowVideoThreadLoop, (void *)&m_ThreadItems[i], 0, &nThreadID);
	}
#endif
}

void SynCtrl::Stop()
{
	ThreadBase::Stop();

#ifdef _MT_MODE
	for (int i = 0; i < _MAX_PANELS; i++)
	{
		if (m_hThread[i])
			::WaitForSingleObject(m_hThread[i], 3000);
		m_hThread[i] = 0;
	}
#endif
}

void SynCtrl::ThreadEvent()
{
	char szMsg[256];
	ThreadBase::Lock();

	DWORD dwCurTime = timeGetTime();

	if (m_dwLastTime == 0)
		m_dwLastTime = dwCurTime;

	if (dwCurTime >= m_dwLastTime + m_iInterval)//40,33
	{
		DWORD dwRet = WaitForSingleObject(m_hEvent, INFINITE);
		m_dwLastTime = dwCurTime;

#ifdef _MT_MODE

		DWORD dwTime1 = timeGetTime();

		::ResetEvent(m_hEvent);
		UpdateVideo();
		Sleep(1);
		for (int i = 0; i < _MAX_PANELS; i++)
			::SetEvent(m_hEvent_MT[i]);
		::SetEvent(m_hEvent);

		DWORD dwTime2 = timeGetTime();

		m_AvgPresentSpendTime.Calculate(dwTime2 - dwTime1);

		if (m_AvgPresentSpendTime.GetCount() % 30 == 29)
		{
			sprintf(szMsg, "#MS# Present avg spend time : %d (%3.2f)\n", dwTime2 - dwTime1, m_AvgPresentSpendTime.GetAverage());
			OutputDebugStringA(szMsg);
		}
#else
		DWORD dwTime1 = timeGetTime();
		DisplayPanelVideo();
		DWORD dwTime2 = timeGetTime();

		m_AvgPresentSpendTime.Calculate(dwTime2 - dwTime1);

		if (m_AvgPresentSpendTime.GetCount() % 30 == 29)
		{
//			sprintf(szMsg, "#MS# Present avg spend time : %d (%3.2f)\n", dwTime2 - dwTime1, m_AvgPresentSpendTime.GetAverage());
//			OutputDebugStringA(szMsg);
			/*
			for (int y = 0; y < g_MediaStreamAry.size(); y++)
			{
				g_MediaStreamAry[y]->SetAudioDelay(m_AvgPresentSpendTime.GetAverage());
			}
			*/
		}
#endif
//		m_dwLastTime = timeGetTime();
		CalPresentFPS();
	}
	Sleep(1);
	ThreadBase::Unlock();
}

void SynCtrl::DisplayPanelVideo()
{
	int y;
	::ResetEvent(m_hEvent);

//	m_bFinished = false;
	char szMsg[256];
	for (y = 0; y < g_MediaStreamAry.size(); y++)
	{
		if (g_MediaStreamAry[y]->IsOpened())
		{
			g_MediaStreamAry[y]->WaitForFrameSyn();
			g_MediaStreamAry[y]->Lock();
		}
	}

	UpdateDisplayOrder();
	
	for (y = 0; y < g_MediaStreamAry.size(); y++)
	{
		g_MediaStreamAry[y]->DisplayPanelVideo();
	}
	
	if(m_iDelayTime > 0)
		Sleep(m_iDelayTime);

	for (y = 0; y < g_MediaStreamAry.size(); y++)
	{
		if (g_MediaStreamAry[y]->IsOpened())
			g_MediaStreamAry[y]->Unlock();
	}

	int iFrameCount = 0;
	int iCount = 0;
	for (int x = 1; x < g_PannelAry.GetCount(); x++)
	{
		if(x == 1)
			iFrameCount = g_PannelAry[x]->GetLastFrameCount();
		if (iFrameCount == g_PannelAry[x]->GetLastFrameCount())
			iCount++;
	}

	if (iCount < g_PannelAry.GetCount() - 1)
	{
		sprintf(szMsg, "#MS# S----------\n");
		OutputDebugStringA(szMsg);
		for (int x = 1; x < g_PannelAry.GetCount(); x++)
		{
			sprintf(szMsg, "#MS# Panel:%d fc:%d\n", x, g_PannelAry[x]->GetLastFrameCount());
			OutputDebugStringA(szMsg);
		}
		sprintf(szMsg, "#MS# E----------\n");
		OutputDebugStringA(szMsg);
	}
	::SetEvent(m_hEvent);

//	m_bFinished = true;
#if 0
	for (int x = 0; x < m_pDisplayManagerAry.GetCount(); x++)
	{

#ifdef _ENABLE_VIDEO_WALL
		//		if (m_pDisplayManagerAry.GetCount() > i)
		{
			DrawPannel(x, pBuffer, w, h, false);

			int iSurfaceIdx = m_iSurfaceMappingAry.GetAt(x);
			if (iSurfaceIdx != -1)
			{
				DisplayManager* pCurDisplayManager = m_pDisplayManagerAry.GetAt(x);

				pCurDisplayManager->UpdateFrameCount(m_iFrameCount);

				D3DSurfaceItem* pD3DSurfaceItem = pCurDisplayManager->GetSurface(iSurfaceIdx);
				g_PannelAry[m_pDisplayManagerAry[x]->GetPannelIdx()]->DisplayVideo_VW(pD3DSurfaceItem->pOverlaySurface, pD3DSurfaceItem->iWidth, pD3DSurfaceItem->iHeight);
			}
		}
#else
		DrawPannel(x, pBuffer, w, h, false);
#endif
	}

#endif
}

#ifdef _MT_MODE
void SynCtrl::UpdateVideo()
{
	for (int y = 0; y < g_MediaStreamAry.size(); y++)
	{
		if (g_MediaStreamAry[y]->IsOpened())
		{
			g_MediaStreamAry[y]->WaitForFrameSyn();
			g_MediaStreamAry[y]->Lock();
		}
	}

	for (int y = 0; y < g_MediaStreamAry.size(); y++)
	{
		if (g_MediaStreamAry[y]->IsOpened())
			g_MediaStreamAry[y]->DisplayPanelVideo(false);
	}

	for (int y = 0; y < g_MediaStreamAry.size(); y++)
	{
		if (g_MediaStreamAry[y]->IsOpened())
			g_MediaStreamAry[y]->Unlock();
	}
}

void SynCtrl::ShowVideo(int iIndex)
{
	WaitForSingleObject(m_hEvent_MT[iIndex], INFINITE);
	//	EnterCriticalSection(&m_CriticalSection);
	if (g_PannelAry.GetCount() > iIndex + 1)
		g_PannelAry[iIndex + 1]->Flip();
	Sleep(1);
	//	LeaveCriticalSection(&m_CriticalSection);
	::ResetEvent(m_hEvent_MT[iIndex]);
}

DWORD WINAPI ShowVideoThreadLoop(LPVOID lpParam)
{
	ThreadItem* pThreadItem = (ThreadItem*)lpParam;
	SynCtrl* pSynCtrl = (SynCtrl*)pThreadItem->pObj;

	while (pSynCtrl->IsAlive())
	{
		pSynCtrl->ShowVideo(pThreadItem->iIndex);
	}
	return 0;
}
#endif

void SynCtrl::LoadDisplayOrder()
{
	char szKeyName[64];
	char szData[32];

	GetPrivateProfileString("Media", "DisplayOrder", "0,1,2,3", szData, 32, theApp.m_strCurPath + "Setting.ini");

	for (int i = 0; i < 4; i++)
		m_iDisplayOrder[i] = GetNextNumberFromString(',', szData);

	m_iTotalOfOrderItems = GetPrivateProfileInt("OrderTable", "Total", 0, theApp.m_strCurPath + "Setting.ini");

	if (m_iTotalOfOrderItems > 0)
	{
		for (int j = 0; j < m_iTotalOfOrderItems; j++)
		{
			sprintf(szKeyName, "FrameNum%d", j);
			m_OrderItems[j].iFrameNum = GetPrivateProfileInt("OrderTable", szKeyName, 0, theApp.m_strCurPath + "Setting.ini");

			sprintf(szKeyName, "Order%d", j);
			GetPrivateProfileString("OrderTable", szKeyName, "0,1,2,3", szData, 32, theApp.m_strCurPath + "Setting.ini");

			for (int i = 0; i < 4; i++)
				m_OrderItems[j].iDisplayOrder[i] = GetNextNumberFromString(',', szData);
		}
	}
}

void SynCtrl::UpdateDisplayOrder()
{
	char szMsg[256];
	if (m_iTotalOfOrderItems > 0)
	{
		int iFrameCount = g_PannelAry[1]->GetLastFrameCount();
		int iNextOrderIndex = CheckOrderTable(iFrameCount);
		if (iNextOrderIndex != -1 && iNextOrderIndex != m_iCurOrderIndex)
		{
			m_iCurOrderIndex = iNextOrderIndex;

			for (int y = 0; y < g_MediaStreamAry.size(); y++)
			{
				for (int i = 0; i < 4; i++)
					g_MediaStreamAry[y]->SetDisplayOrder(i, m_OrderItems[m_iCurOrderIndex].iDisplayOrder[i]);
			}

			sprintf(szMsg, "#MS# UpdateDisplayOrder : %d (%d)\n", m_iCurOrderIndex, iFrameCount);
			OutputDebugStringA(szMsg);
		}
	}
}

void SynCtrl::CalPresentFPS()
{
	char szMsg[256];
	DWORD dwCurTime = timeGetTime();

	if (m_dwPresentFPSLastTime == 0)
		m_dwPresentFPSLastTime = dwCurTime;

	if (m_iPresentCount >= 30)
	{
		DWORD dwSpendTime = dwCurTime - m_dwPresentFPSLastTime;

		m_fPresentFPS = 30.0 / ((float)dwSpendTime / 1000.0);

		m_AvgPresentFPS.Calculate(m_fPresentFPS);

		theApp.m_fPresentFPS = m_AvgPresentFPS.GetAverage();

		sprintf(szMsg, "#MS# PresentFPS: %3.2F\n", m_AvgPresentFPS.GetAverage());
		OutputDebugStringA(szMsg);

		m_iPresentCount = 0;
		m_dwPresentFPSLastTime = dwCurTime;
	}
	else
		m_iPresentCount++;
}

int SynCtrl::CheckOrderTable(int iFrameCount)
{
	int iIndex = -1;
	for (int j = 0; j < m_iTotalOfOrderItems; j++)
	{
		if (m_OrderItems[j].iFrameNum < iFrameCount)
			iIndex = j;
		else if (m_OrderItems[j].iFrameNum > iFrameCount)
			break;
	}
	return iIndex;
}
