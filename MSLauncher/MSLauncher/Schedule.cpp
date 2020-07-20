#include "stdafx.h"
#include "Schedule.h"
#include "../../Include/sstring.h"

#include "Mmsystem.h"
#pragma comment(lib, "Winmm.lib")

__inline void Shutdown(bool bReboot = true)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	if (bReboot)
		ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
	else
		ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0);
}

Schedule::Schedule()
{
	m_bEnable = false;

	m_iHour = -1;
	m_iMinute = -1;
	m_iSecond = -1;
	m_dwLastTime = 0;

	m_pPBSchedule = new PBSchedule;
}

Schedule::~Schedule()
{
	if (m_pPBSchedule)
		delete m_pPBSchedule;
}

void Schedule::SetProcessHandle(HANDLE hProcess)
{
	m_hProcess = hProcess;
}

void Schedule::Start()
{
	LoadConfig();
	m_pPBSchedule->LoadConfig();
	ThreadBase::Start();
}

void Schedule::Stop()
{
	ThreadBase::Stop();
}

void Schedule::LoadConfig()
{
	char szMsg[512];

	char szAPPath[512];
	char szIniPath[512];
	char szData[256];
	char szTmp[256];
	int iIndex = 0;

	int iHour = 0;
	int iMinute = 0;
	int iSecond = 0;

	GetExecutionPath(NULL, szAPPath);
	sprintf(szIniPath, "%s\\Setting.ini", szAPPath);

	int iEnable = GetPrivateProfileIntA("Shutdown", "Enable", 0, szIniPath);

	m_bEnable = iEnable;

	BOOL bRet;
	if (iEnable == 1)
	{
		GetPrivateProfileStringA("Shutdown", "Time", "", szData, 100, szIniPath);
		strcpy(szTmp, szData);

		sscanf(szTmp, "%02d:%02d:%02d", &m_iHour, &m_iMinute, &m_iSecond);
	}

	sprintf(szMsg, "#MS_Schedule# Enable = %d\n", iEnable);
	OutputDebugStringA(szMsg);

	sprintf(szMsg, "#MS_Schedule# Time = [%02d : %02d : %02d]\n", m_iHour, m_iMinute, m_iSecond);
	OutputDebugStringA(szMsg);

	m_dwLastTime = 0;
}

bool Schedule::CheckShutdownTime()
{
	char szMsg[512];

	SYSTEMTIME st;
	GetLocalTime(&st);

//	sprintf(szMsg, "#MS_Schedule# LocalTime = [%02d : %02d : %02d] \n", st.wHour, st.wMinute, st.wSecond);
//	OutputDebugStringA(szMsg);

	if (m_bEnable && st.wHour == m_iHour && st.wMinute == m_iMinute && st.wSecond == m_iSecond)
	{
		return true;
	}

	return false;
}

void Schedule::DoShutdown()
{
	char szMsg[512];

	m_bEnable = false;

	SYSTEMTIME st;
	GetLocalTime(&st);

	sprintf(szMsg, "#MS_Schedule# DoShutdown!! [%02d : %02d : %02d]\n", st.wHour, st.wMinute, st.wSecond);
	OutputDebugStringA(szMsg);

	if (m_hProcess != 0)
		bool bRet = ::TerminateProcess(m_hProcess, 0);

	Sleep(3000);

	Shutdown(false);
}

void Schedule::ThreadEvent()
{
	Lock();

	DWORD dwCurTime = timeGetTime();

	if (m_dwLastTime == 0)
		m_dwLastTime = dwCurTime;

	if (dwCurTime >= m_dwLastTime + 1000)
	{
		m_dwLastTime = dwCurTime;
		bool bRet = CheckShutdownTime();
		if (bRet)
			DoShutdown();
		m_pPBSchedule->CheckPlaylist();
	}

	Sleep(1);
	Unlock();
}