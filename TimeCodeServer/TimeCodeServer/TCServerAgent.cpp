#include "stdafx.h"
#include "TCServerAgent.h"

#include "..\..\Include\NetCommon.h"
#include "Mmsystem.h"

//"Winmm.lib"
#pragma comment(lib, "winmm.lib")

DWORD WINAPI TCServerThread(LPVOID lpParam);

TCServerAgent::TCServerAgent()
{
	m_pTCServerDll = new TCServerDll;
	m_pTCServerDll->LoadLib();

	m_ulCurTimeCode = 0;
	m_dwLastTime = 0;

	m_hWnd = NULL;

	InitializeCriticalSection(&m_CriticalSection);
}

TCServerAgent::~TCServerAgent()
{
	if (m_pTCServerDll)
		delete m_pTCServerDll;

	DeleteCriticalSection(&m_CriticalSection);
}

void TCServerAgent::Init(HWND hWnd)
{
	m_hWnd = hWnd;
}

int TCServerAgent::StartServer(int iPort)
{
	if (m_pTCServerDll)
		return m_pTCServerDll->StartServer(iPort);
}

void TCServerAgent::StopServer()
{
	if (m_pTCServerDll)
		m_pTCServerDll->StopServer();
}

void TCServerAgent::SetTotalOfTimeCode(unsigned long ulTimeCode)
{
	if (m_pTCServerDll)
		m_pTCServerDll->SetTotalOfTimeCode(ulTimeCode);
}

void TCServerAgent::TC_Start()
{
	if (m_pTCServerDll->GetTCStatus() == TCS_STOP)
	{
		m_ulCurTimeCode = 0;
		m_dwLastTime = 0;

		m_bIsAlive = true;
		DWORD nThreadID;
		m_hTCThread = CreateThread(0, 0, TCServerThread, (void *)this, 0, &nThreadID);
	}

	if (m_pTCServerDll)
		m_pTCServerDll->TC_Start();
}

void TCServerAgent::TC_Stop()
{
	m_bIsAlive = false;
	::WaitForSingleObject(m_hTCThread, 10000);

	if (m_pTCServerDll)
		m_pTCServerDll->TC_Stop();
}

void TCServerAgent::TC_Pause()
{
	if (m_pTCServerDll)
		m_pTCServerDll->TC_Pause();
}

bool TCServerAgent::IsAlive()
{
	return m_bIsAlive;
}

void TCServerAgent::TimeCodeLoop()
{
	EnterCriticalSection(&m_CriticalSection);
	if (m_pTCServerDll->GetTCStatus() == TCS_RUNNING)
	{
		DWORD dwCurTime = timeGetTime();

		if (m_ulCurTimeCode >= m_pTCServerDll->GetTotalOfTimeCode() + 20)
		{
			m_dwLastTime = 0;
			m_ulCurTimeCode = 0;
		}
		else if (m_dwLastTime != 0 && dwCurTime - m_dwLastTime >= 20)
		{
			m_ulCurTimeCode += (dwCurTime - m_dwLastTime);

//			extern void gUpdateTimeCodeStr(long lTimeCode);
//			gUpdateTimeCodeStr(m_ulCurTimeCode);

			if (m_hWnd)
				::PostMessage(m_hWnd, WM_UPDATE_TIME_CODE_STR, m_ulCurTimeCode,0);

			m_pTCServerDll->UpdateTimeCode(m_ulCurTimeCode);

			m_dwLastTime = dwCurTime;
		}
		else if (m_dwLastTime == 0)
			m_dwLastTime = dwCurTime;
	}
	Sleep(1);
	LeaveCriticalSection(&m_CriticalSection);
}

DWORD WINAPI TCServerThread(LPVOID lpParam)
{
	TCServerAgent* pObj = (TCServerAgent*)lpParam;

	while (pObj->IsAlive())
	{
		pObj->TimeCodeLoop();
	}
	return 0;
}