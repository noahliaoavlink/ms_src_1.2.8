#include "stdafx.h"
#include "APProcessCtrl.h"
#include "../../Include/sstring.h"
#include "../../Include/MidiCtrlCommon.h"

APProcessCtrl::APProcessCtrl()
{
	pi.hProcess = 0;
	m_pNWin = 0;
}

APProcessCtrl::~APProcessCtrl()
{
	if (m_pNWin)
		delete m_pNWin;
}

void APProcessCtrl::Init()
{
	char szWinName[128];
	sprintf(szWinName, "MS_WatchDog_Win");
	m_pNWin = new CNWin(szWinName);
	m_pNWin->SetCallback(this);

	m_Schedule.Start();
}

void APProcessCtrl::Launch()
{
	char szAPPath[512];
	STARTUPINFOA si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(STARTUPINFOA);
	si.wShowWindow = SW_SHOW;

	GetExecutionPath(NULL, szAPPath);

	char szAPFileNamep[512] = "";
	char szCmdLine[512];

	sprintf(szAPFileNamep, "%s\\MediaServer.exe", szAPPath);

	sprintf(szCmdLine,"null switch_to_shape");

	BOOL result = CreateProcessA(szAPFileNamep, szCmdLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

	m_Schedule.SetProcessHandle(pi.hProcess);
}

void APProcessCtrl::Close()
{
	if (pi.hProcess != 0)
		bool bRet = ::TerminateProcess(pi.hProcess, 0);
}

void APProcessCtrl::ParseCopyData(COPYDATASTRUCT* pCopyDataStruct)
{
	//char szMsg[512];
	//sprintf(szMsg, "#MS_Launcher# APProcessCtrl::ParseCopyData 0 \n");
	//OutputDebugStringA(szMsg);

	switch (pCopyDataStruct->dwData)
	{
		case MSLC_EXIT:
			{
//				sprintf(szMsg, "#MS_Launcher# APProcessCtrl::ParseCopyData 1 \n");
//				OutputDebugStringA(szMsg);

				m_Schedule.Stop();

				exit(0);
			}
			break;
	}

//	sprintf(szMsg, "#MS_Launcher# APProcessCtrl::ParseCopyData 2 \n");
//	OutputDebugStringA(szMsg);
}

void* APProcessCtrl::WinMsg(int iMsg, void* pParameter1, void* pParameter2)
{
//	char szMsg[512];

	switch (iMsg)
	{
		case WM_COPYDATA:
		{
//			sprintf(szMsg, "#MS_Launcher# APProcessCtrl::WinMsg - WM_COPYDATA \n");
//			OutputDebugStringA(szMsg);

			COPYDATASTRUCT* pCopyDataStruct = (COPYDATASTRUCT*)pParameter2;
			ParseCopyData(pCopyDataStruct);
		}
		break;
	}
	return 0;
}