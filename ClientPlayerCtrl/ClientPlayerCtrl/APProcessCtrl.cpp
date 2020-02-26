#include "stdafx.h"
#include "APProcessCtrl.h"
#include "../../Include/sstring.h"

APProcessCtrl::APProcessCtrl()
{
	pi.hProcess = 0;
}

APProcessCtrl::~APProcessCtrl()
{
}

void APProcessCtrl::Launch(char* szLayoutStr,int iIndex)
{
	char szAPPath[512];
	STARTUPINFOA si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(STARTUPINFOA);
	si.wShowWindow = SW_SHOW;

	GetExecutionPath(NULL, szAPPath);

	char szAPFileNamep[512] = "ClientPlayer.exe";
	char szCmdLine[512];

	sprintf(szAPFileNamep, "%s\\ClientPlayer.exe", szAPPath);

	sprintf(szCmdLine,"null %s %d", szLayoutStr, iIndex);

	BOOL result = CreateProcess(szAPFileNamep, szCmdLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
}

void APProcessCtrl::Close()
{
	if (pi.hProcess != 0)
		bool bRet = ::TerminateProcess(pi.hProcess, 0);
}