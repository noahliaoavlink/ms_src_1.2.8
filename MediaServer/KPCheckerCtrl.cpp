#include "stdafx.h"
#include "KPCheckerCtrl.h"

#include "../Include/sstring.h"

KPCheckerCtrl::KPCheckerCtrl()
{
	GetExecutionPath(NULL, m_szAPPath);
}

KPCheckerCtrl::~KPCheckerCtrl()
{
}

void KPCheckerCtrl::Launch()
{
	PROCESS_INFORMATION pi;

	char szAPFileNamep[512] = "";
	char szCmdLine[512];

	STARTUPINFOA si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(STARTUPINFOA);
	si.wShowWindow = SW_HIDE;  //SW_SHOW;

	sprintf(szCmdLine, "null %s", "");

	sprintf(szAPFileNamep, "%s\\KChecker.exe", m_szAPPath);

	BOOL result = CreateProcessA(szAPFileNamep, szCmdLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
}