/********************************************************************\
Project Name: Timeline Prototype

File Name: TimeCodeEngine.cpp

Definition of following classes:
TimeCodeThread
TimeCodeEngine

Copyright:
Media Server
(C) Copyright C&C TECHNIC TAIWAN CO., LTD.
All rights reserved.

Author:
Stone Chang, mail: stonechang.cctch@gmail.com

Other issue:
N/A

\********************************************************************/

// ==== Inlcude Microsoft MFC library ==== //
#include <Windows.h>
#include "stdafx.h"


// ==== Inlcude STL library ==== //
#include <string>
#include <boost/format.hpp>

// ==== Include local library ==== //
#include "TLUtility.h"
#include "TimeCodeEngine.h"
#include <Mmsystem.h>
#include "ServiceProvider.h"
#include "MediaServer.h"

#define ENGINNERING_MODE

#pragma comment(lib, "Winmm.lib")

using namespace Timeline;

enum TimeCodeEngineStatus
{
	TCES_NORMAL = 0,
	TCES_RESET,
	CHS_WAIT,
	CHS_UPDATE,
};

/* ====================================================== *\
TimeCodeThread
\* ====================================================== */
CCriticalSection TimeCodeThread::m_critSection;

IMPLEMENT_DYNAMIC(TimeCodeThread, CWinThread)

BEGIN_MESSAGE_MAP(TimeCodeThread, CWinThread)
END_MESSAGE_MAP()

#pragma optimize( "", off )

TimeCodeThread::TimeCodeThread()
{
	m_isStart = false;

	m_bAutoDelete = FALSE;

	// kill event starts out in the signaled state
	m_hEventKill = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_dwCount = 0;

	int iTimeCodeInterval = GetPrivateProfileInt("System", "TimeCodeInterval", 20, theApp.m_strCurPath + "Setting.ini");
	m_iInterval = iTimeCodeInterval;// 20;

	InitializeCriticalSection(&m_CriticalSection);
}

TimeCodeThread::~TimeCodeThread()
{
	CloseHandle(m_hEventKill);
	CloseHandle(m_hEventDead);
	DeleteCriticalSection(&m_CriticalSection);
}

BOOL TimeCodeThread::InitInstance()
{
	// thread setup

	// loop but check for kill notification
	while (WaitForSingleObject(m_hEventKill, 0) == WAIT_TIMEOUT)
		SingleStep();

	// thread cleanup

	// avoid entering standard message loop by returning FALSE
	return FALSE;
}

void TimeCodeThread::Delete()
{
	// calling the base here won't do anything but it is a good habit
	CWinThread::Delete();

	// acknowledge receipt of kill notification
	VERIFY(SetEvent(m_hEventDead));
}

void TimeCodeThread::Start()
{
	m_isStart = true;

	m_dwLastTime = timeGetTime();
}

void TimeCodeThread::Stop()
{
	m_isStart = false;
}

bool TimeCodeThread::IsAlive()
{
	return m_isStart;
}

void TimeCodeThread::ApplySetting()
{
	int iTimeCodeInterval = GetPrivateProfileInt("System", "TimeCodeInterval", 20, theApp.m_strCurPath + "Setting.ini");
	m_iInterval = iTimeCodeInterval;// 20;
}

void TimeCodeThread::KillThread()
{
	// Note: this function is called in the context of other threads,
	//  not the thread itself.

	// reset the m_hEventKill which signals the thread to shutdown
	VERIFY(SetEvent(m_hEventKill));

	// allow thread to run at higher priority during kill process
	SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	WaitForSingleObject(m_hEventDead, INFINITE);
	WaitForSingleObject(m_hThread, INFINITE);

	// now delete CWinThread object since no longer necessary
	delete this;
}

void TimeCodeThread::SingleStep()
{
#if 0

	if (m_isStart)
	{
		CSingleLock singleLock(&m_critSection);
		singleLock.Lock();
		if (singleLock.IsLocked())
		{
#ifdef ENGINNERING_MODE
#if false
			DumpToLog(boost::str(boost::format("TimeCodeThread::diff=%d milliseconds") % diff));
#endif
#endif // ENGINNERING_MODE

			this->TimeCodeUpdate(TimeCode(0, 0, 0, 20)); // Send update TimeCode signal

			singleLock.Unlock();
		}
		::Sleep(20);
	}
	else
	{
		::Sleep(1);
	}
	
#else
	int dTimeDiff;
	if (m_isStart)
	{
		bool bUpdate = true;
		TimelineManager* m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
		TCAdapter* pTCAdapter = m_manager->GetTCAdapter();
		if (pTCAdapter)
		{
			if (pTCAdapter->GetMode() == TCM_CLIENT && pTCAdapter->IsWorking() && pTCAdapter->GetReceiveStatus() != CRS_HUNGER || pTCAdapter->GetMode() == TCM_MIDI_CTRL)
				bUpdate = false;
		}

		if (bUpdate)
		{
			DWORD dwCurTime = timeGetTime();
			if (dwCurTime - m_dwLastTime >= m_iInterval)
			{
				int iQuotient = m_iInterval / 20;

				dTimeDiff = dwCurTime - m_dwLastTime;

				m_dwLastTime = dwCurTime;

#if 0
				CSingleLock singleLock(&m_critSection);
				singleLock.Lock();
				if (singleLock.IsLocked())
				{
					this->TimeCodeUpdate(TimeCode(0, 0, 0, dTimeDiff / iQuotient)); // Send update TimeCode signal
					singleLock.Unlock();
				}
#else
				EnterCriticalSection(&m_CriticalSection);
				if(iQuotient > 0)
					this->TimeCodeUpdate(TimeCode(0, 0, 0, dTimeDiff / iQuotient)); // Send update TimeCode signal
				LeaveCriticalSection(&m_CriticalSection);
#endif
				
			}
		}
	}

	m_dwCount++;
	if(m_dwCount %10 == 9)
		::Sleep(1);
	else if (m_dwCount == 10)
		m_dwCount = 0;

#endif
}

void TimeCodeThread::SetInterval(int iValue)
{
	m_iInterval = iValue;
}

int TimeCodeThread::GetInterval()
{
	return m_iInterval;
}
/* ====================================================== *\
TimeCodeUiThread
\* ====================================================== */
CCriticalSection TimeCodeUiThread::m_critSection;

IMPLEMENT_DYNAMIC(TimeCodeUiThread, CWinThread)

BEGIN_MESSAGE_MAP(TimeCodeUiThread, CWinThread)
END_MESSAGE_MAP()

TimeCodeUiThread::TimeCodeUiThread()
{
	m_isStart = false;

	m_bAutoDelete = FALSE;

	// kill event starts out in the signaled state
	m_hEventKill = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_beginTime = this->GetTime();
	m_enterTime = m_beginTime;
	m_exitTime = m_beginTime;

	m_dwCount = 0;
}

TimeCodeUiThread::~TimeCodeUiThread()
{
	CloseHandle(m_hEventKill);
	CloseHandle(m_hEventDead);
}

BOOL TimeCodeUiThread::InitInstance()
{
	// thread setup

	// loop but check for kill notification
	while (WaitForSingleObject(m_hEventKill, 0) == WAIT_TIMEOUT)
		SingleStep();

	// thread cleanup

	// avoid entering standard message loop by returning FALSE
	return FALSE;
}

void TimeCodeUiThread::Delete()
{
	// calling the base here won't do anything but it is a good habit
	CWinThread::Delete();

	// acknowledge receipt of kill notification
	VERIFY(SetEvent(m_hEventDead));
}

void TimeCodeUiThread::Start()
{
	m_isStart = true;
	m_dwLastTime = timeGetTime();
}

void TimeCodeUiThread::Stop()
{
	m_isStart = false;
}

bool TimeCodeUiThread::IsAlive()
{
	return m_isStart;
}

void TimeCodeUiThread::KillThread()
{
	// Note: this function is called in the context of other threads,
	//  not the thread itself.

	// reset the m_hEventKill which signals the thread to shutdown
	VERIFY(SetEvent(m_hEventKill));

	// allow thread to run at higher priority during kill process
	SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	WaitForSingleObject(m_hEventDead, INFINITE);
	WaitForSingleObject(m_hThread, INFINITE);

	// now delete CWinThread object since no longer necessary
	delete this;
}

void TimeCodeUiThread::SingleStep()
{
#if 0

	if (m_isStart)
	{
		CSingleLock singleLock(&m_critSection);
		singleLock.Lock();
		if (singleLock.IsLocked())
		{
#ifdef ENGINNERING_MODE
#if false
			DumpToLog(boost::str(boost::format("TimeCodeUiThread::diff=%d milliseconds") % diff));
#endif
#endif // ENGINNERING_MODE

			this->TimeCodeUpdate(TimeCode(0, 0, 0, 20)); // Send update TimeCode signal

			singleLock.Unlock();
		}
		::Sleep(20);
	}
	else
	{
		::Sleep(1);
	}

#else
	if (m_isStart)
	{
		DWORD dwCurTime = timeGetTime();
		int dTimeDiff;
		if (dwCurTime - m_dwLastTime >= 20)
		{
			dTimeDiff = dwCurTime - m_dwLastTime;

			//m_dwLastTime += 20;
			dTimeDiff = dwCurTime - m_dwLastTime;
			m_dwLastTime = dwCurTime;
			CSingleLock singleLock(&m_critSection);
			singleLock.Lock();
			if (singleLock.IsLocked())
			{
				this->TimeCodeUpdate(TimeCode(0, 0, 0, dTimeDiff)); // Send update TimeCode signal
				singleLock.Unlock();
			}
		}
	}

	m_dwCount++;
	if (m_dwCount % 10 == 9)
		::Sleep(1);
	else if (m_dwCount == 10)
		m_dwCount = 0;

#endif
}

double TimeCodeUiThread::GetTime()
{
	return GetWallTime();
}

double TimeCodeUiThread::GetWallTime()
{
	LARGE_INTEGER time, freq;
	if (!QueryPerformanceFrequency(&freq)) {
		//  Handle error
		return 0;
	}
	if (!QueryPerformanceCounter(&time)) {
		//  Handle error
		return 0;
	}
	return (double)time.QuadPart / (double)freq.QuadPart;
}

double TimeCodeUiThread::GetCpuTime()
{
	FILETIME a, b, c, d;
	if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
		//  Returns total user time.
		//  Can be tweaked to include kernel times as well.
		return
			(double)(d.dwLowDateTime |
			((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
	}
	else {
		//  Handle error
		return 0;
	}
}

/* ====================================================== *\
TimeCodeEngine
\* ====================================================== */
TimeCodeEngine::TimeCodeEngine()
{
	m_iIndex = 0;
#ifndef _ENABLE_TC_NET
	m_Socket = NULL;
	m_bMaster = false;
	m_bIsRunning = false;
	//m_hSoketThread = NULL;
#endif

	if (m_timecodeThread != nullptr)
	{
		return;
	}

	if (m_uiThread != nullptr)
	{
		return;
	}

	m_timecodeThread = new TimeCodeThread();
	m_uiThread = new TimeCodeUiThread();

	if (m_timecodeThread == NULL)
		return;
	if (m_uiThread == NULL)
		return;

	ASSERT_VALID(m_timecodeThread);
	ASSERT_VALID(m_uiThread);
	m_timecodeThread->m_pThreadParams = NULL;
	m_uiThread->m_pThreadParams = NULL;

	// Create Thread in a suspended state so we can set the Priority
	// before it starts getting away from us
	if (!m_timecodeThread->CreateThread(CREATE_SUSPENDED))
	{
		delete m_timecodeThread;
		return;
	}

	// Create Thread in a suspended state so we can set the Priority
	// before it starts getting away from us
	if (!m_uiThread->CreateThread(CREATE_SUSPENDED))
	{
		delete m_uiThread;
		return;
	}

#ifndef _ENABLE_TC_NET
	InitSocket(FALSE);
#endif

	// If you want to make the sample more sprightly, set the thread priority here
	// a little higher. It has been set at idle priority to keep from bogging down
	// other apps that may also be running.
	VERIFY(m_timecodeThread->SetThreadPriority(THREAD_PRIORITY_IDLE));
	VERIFY(m_uiThread->SetThreadPriority(THREAD_PRIORITY_IDLE));
	// Now the thread can run wild

	m_total_timecode = 0;
	m_dwRefTime = 0;

	m_iStatus = TCES_NORMAL;
	m_iWaitCount = 0;

	m_bRepeat = true;

	m_timecodeThread->TimeCodeUpdate.connect(boost::bind(&TimeCodeEngine::UpdateTimeCode, this, _1));
	m_uiThread->TimeCodeUpdate.connect(boost::bind(&TimeCodeEngine::UpdateUi, this, _1));

	m_timecodeThread->ResumeThread();
	m_uiThread->ResumeThread();
}

TimeCodeEngine::~TimeCodeEngine()
{
	// tell all threads to shutdown
	VERIFY(SetEvent(m_timecodeThread->m_hEventKill));
	VERIFY(SetEvent(m_uiThread->m_hEventKill));

	// wait for all threads to finish shutdown
	WaitForSingleObject(m_timecodeThread->m_hEventKill, INFINITE);
	WaitForSingleObject(m_timecodeThread->m_hEventDead, INFINITE);
	WaitForSingleObject(m_timecodeThread->m_hThread, INFINITE);
	WaitForSingleObject(m_uiThread->m_hEventKill, INFINITE);
	WaitForSingleObject(m_uiThread->m_hEventDead, INFINITE);
	WaitForSingleObject(m_uiThread->m_hThread, INFINITE);

	// delete all thread objects
	m_timecodeThread->TimeCodeUpdate.disconnect(boost::bind(&TimeCodeEngine::UpdateTimeCode, this, _1));
	m_uiThread->TimeCodeUpdate.disconnect(boost::bind(&TimeCodeEngine::UpdateUi, this, _1));

	delete m_timecodeThread;
	delete m_uiThread;
	m_timecodeThread = nullptr;
	m_uiThread = nullptr;

	this->TimeCodeUpdate.disconnect_all_slots();
}

void TimeCodeEngine::StartTimeCodeUpdate()
{
#ifdef ENGINNERING_MODE
#if false
	DumpToLog("TimeCodeEngine::StartTimeCodeUpdate");
#endif
#endif // ENGINNERING_MODE

#ifndef _ENABLE_TC_NET
	CloseSocket();
	InitSocket(false);

	m_bIsRunning = true;
	m_pSoketThread = AfxBeginThread(SocketThread, this);
#endif

	m_dwStartTime = timeGetTime();
	m_timecodeThread->Start();
	m_uiThread->Start();
}

void TimeCodeEngine::StopTimeCodeUpdate()
{
#ifndef _ENABLE_TC_NET
	m_bIsRunning = false;
	CloseSocket();
	::WaitForSingleObject(m_pSoketThread, 3000);
	m_pSoketThread = NULL;
#endif

	m_timecodeThread->Stop();
	m_uiThread->Stop();
	this->ResetTimeCode();

#ifdef ENGINNERING_MODE
#if false
	DumpToLog("TimeCodeEngine::StopTimeCodeUpdate");
#endif
#endif // ENGINNERING_MODE
}

bool TimeCodeEngine::IsAlive()
{
	return m_uiThread->IsAlive();
}

void TimeCodeEngine::PauseTimeCodeUpdate()
{
#ifndef _ENABLE_TC_NET
	m_bIsRunning = false;
	CloseSocket();
	::WaitForSingleObject(m_pSoketThread, 3000);
	m_pSoketThread = NULL;
#endif

	m_timecodeThread->Stop();
	m_uiThread->Stop();

#ifdef ENGINNERING_MODE
#if false
	DumpToLog("TimeCodeEngine::PauseTimeCodeUpdate");
#endif
#endif // ENGINNERING_MODE
}

void TimeCodeEngine::ResetTimeCode()
{
	m_iStatus = TCES_RESET;
	this->m_timecode = TimeCode(0);
}

void TimeCodeEngine::SetTotalTimecode(TimeCode timecode)
{
	m_total_timecode = timecode;
	//m_total_timecode = timecode - 40;  //DIM_GOBAL
	//m_total_timecode = timecode + 40; //DIM_NORMAL , DIM_LOCAL
}

TimeCode TimeCodeEngine::GetTotalTimecode()
{
	return m_total_timecode;
}

void TimeCodeEngine::UpdateTimeCode(TimeCode timecode)
{
	char szMsg[512];

	int iRet = CheckStatus();

	if (iRet == -1)
		return;

	memcpy(&m_tmp_timecode[m_iIndex], &timecode,sizeof(TimeCode));

	unsigned long ulTotalMS = 0;
	if (m_timecodeThread->GetInterval() > 20)
		ulTotalMS = m_total_timecode.TotalMilliseconds() - 40;
	else
		ulTotalMS = m_total_timecode.TotalMilliseconds();

	//if (m_timecode.TotalMilliseconds() > 10000)
	if (m_timecode.TotalMilliseconds() > ulTotalMS)
	{
		if (!m_bRepeat)
			return ;

//		this->TimeCodeUpdate(m_timecode);
		this->ResetTimeCode();
//		this->TimeCodeUpdate(m_timecode);

		this->TimeCodeUpdate(m_tmp_timecode[m_iIndex]);

		int iNewIndex = m_iIndex + 1;
		m_iIndex = iNewIndex % 100;

		//m_timecode = m_total_timecode;

		memcpy(&m_tmp_timecode[m_iIndex], &m_timecode, sizeof(TimeCode));

		sprintf(szMsg, "#TimeCodeEngine::UpdateTimeCode - ResetTimeCode\n");
		OutputDebugString(szMsg);
	}
	else
	{
		int iLastMS = m_timecode.TotalMilliseconds();

		if (m_dwRefTime != 0)
		{
			this->m_timecode = m_dwRefTime;
			m_dwRefTime = 0;
		}
		else
			this->m_timecode = this->m_timecode + timecode;

		int iCurMS = m_timecode.TotalMilliseconds();

		//sprintf(szMsg, "#TimeCodeEngine::UpdateTimeCode - %u\n", iLastMS);
		//OutputDebugString(szMsg);

	//	if (iCurMS - iLastMS > 20)
		//	int kk = 0;
	}


#ifdef ENGINNERING_MODE
#if false
	DumpToLog(boost::str(boost::format("TimeCodeEngine::UpdateTimeCode=%s") % m_timecode.ToString()));
#endif
#endif // ENGINNERING_MODE

	this->TimeCodeUpdate(m_tmp_timecode[m_iIndex]);

	int iNewIndex = m_iIndex + 1;
	m_iIndex = iNewIndex % 100;
}

void TimeCodeEngine::UpdateUi(TimeCode timecode)
{
#ifdef ENGINNERING_MODE
#if false
	DumpToLog(boost::str(boost::format("TimeCodeEngine::UpdateUi=%s") % m_timecode.ToString()));
#endif
#endif // ENGINNERING_MODE

	this->UiUpdate(m_timecode);
}

void TimeCodeEngine::SetTimecode(TimeCode timecode)
{
	this->m_timecode = timecode;
}

void TimeCodeEngine::SetInterval(int iValue)
{
	m_timecodeThread->SetInterval(iValue);
}

void TimeCodeEngine::DoUpdateTimeCode()
{
	if(m_total_timecode.TotalMilliseconds() > m_timecode.TotalMilliseconds())
		this->UiUpdate2(m_timecode);
}

void TimeCodeEngine::SetRefTimeCode(DWORD dwRefTime)
{
	m_dwRefTime = dwRefTime;// +m_dwStartTime;
	//m_timecodeThread->SetRefTimeCode(m_dwRefTime);
	//m_uiThread->SetRefTimeCode(m_dwRefTime);
}

#ifndef _ENABLE_TC_NET
void TimeCodeEngine::InitSocket(BOOL bMaster)
{
	WSAData wsaData;
	WORD version = MAKEWORD(2, 2); // 版本
	const USHORT iServerPort = m_iPort;// 2017;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // 成功回傳 0
	if (iResult != 0)
	{
		// 初始化Winsock 失敗
	}

//	m_bMaster = bMaster;

	m_Socket = socket(AF_INET, SOCK_DGRAM, NULL);

	SOCKADDR_IN sMyAddr;

	//設定 addr 資料
	ZeroMemory((char *)&sMyAddr, sizeof(sMyAddr));
	sMyAddr.sin_family = AF_INET;
	sMyAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	ZeroMemory((char *)&m_sBrotherAddr, sizeof(m_sBrotherAddr));
	m_sBrotherAddr.sin_family = AF_INET;
	m_sBrotherAddr.sin_port = htons(iServerPort);

	if (!m_bMaster)
	{
		sMyAddr.sin_port = htons(iServerPort);

		if (bind(m_Socket, (struct sockaddr *)&sMyAddr, sizeof(sMyAddr)) < 0)
		{
			perror("bind failed\n");
			exit(1);
		}

		m_sBrotherAddr.sin_addr.s_addr = inet_addr(m_szIPAdress); //"10.1.1.153"
	}
	else
	{
		sMyAddr.sin_port = htons(0);

		if (bind(m_Socket, (struct sockaddr *)&sMyAddr, sizeof(sMyAddr)) < 0)
		{
			perror("bind failed\n");
			exit(1);
		}

		bool optval = true;
		setsockopt(m_Socket, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(bool));

		m_sBrotherAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	}
}

void TimeCodeEngine::CloseSocket()
{
	if (m_Socket)
		closesocket(m_Socket);
	m_Socket = 0;

	WSACleanup();
}

UINT TimeCodeEngine::SocketThread(LPVOID pParam)
{
	TimeCodeEngine *pThis = reinterpret_cast<TimeCodeEngine *>(pParam);
	pThis->SocketFun();
	return 1;
}

void TimeCodeEngine::SocketFun()
{
	const int MAXDATA = 10;
	char cData[MAXDATA];
	CString strTemp;
	ZeroMemory(cData, MAXDATA);

	while (m_bIsRunning) //m_pSoketThread
	{
		if (m_bMaster)
		{
			strTemp.Format("%i", m_timecode.TotalMilliseconds());
			strcpy_s(cData, strTemp.GetBuffer());
			cData[strTemp.GetLength()] = '\0';


			if (sendto(m_Socket, cData, MAXDATA, 0, (struct sockaddr*)&m_sBrotherAddr, sizeof(m_sBrotherAddr)) < 0)
			{
				perror("send to client error !");
				exit(1);
			}
			Sleep(1000);
		}
		else
		{
			int iAdrLen = sizeof(m_sBrotherAddr);
			if (recvfrom(m_Socket, cData, MAXDATA, 0, (struct sockaddr*)&m_sBrotherAddr, &iAdrLen) < 0)
			{
				perror("read from server error !");
				//exit(1);
			}
			else
			{
				m_dwRefTime = atoi(cData);

				for (int x = 0; x < g_MediaStreamAry.size(); x++)
				{
					if (g_MediaStreamAry.at(x)->IsOpened())
						g_MediaStreamAry.at(x)->SetReferenceTimeCode(double(m_dwRefTime));
				}
			}
		}
	}
}
#endif
/*
void TimeCodeEngine::SetIPAdress(char* szIP)
{
	strcpy(m_szIPAdress,szIP);
}

void TimeCodeEngine::SetPort(int iValue)
{
	m_iPort = iValue;
}

void TimeCodeEngine::SetMaster(bool bEnable)
{
	m_bMaster = bEnable;
}
*/
void TimeCodeEngine::ApplySetting()
{
	//m_timecodeThread->ApplySetting();
	m_bRepeat = GetPrivateProfileInt("Timeline_Setting", "Repeat", 0, theApp.m_strCurPath + "Setting.ini");
}

int TimeCodeEngine::CheckStatus()
{
	switch (m_iStatus)
	{
		case TCES_NORMAL:
			m_iWaitCount = 1;
			m_iStatus = CHS_WAIT;// CHS_UPDATE;
			break;
		case TCES_RESET:
			m_iWaitCount = 1;
			m_iStatus = CHS_WAIT;
			break;
		case CHS_WAIT:
			DoWait();
			break;
		case CHS_UPDATE:
			return 1;
			break;
	}
	return -1;
}

void TimeCodeEngine::DoWait()
{
	if (m_iWaitCount > 0)
		m_iWaitCount--;
	else
	{
		m_iStatus = CHS_UPDATE;
	}
}

void TimeCodeEngine::EnableRepeat(bool bEnable)
{
	m_bRepeat = bEnable;
}

#pragma optimize( "", on )