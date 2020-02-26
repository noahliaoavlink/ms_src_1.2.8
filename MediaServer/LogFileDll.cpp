#include "stdafx.h"
#include "LogFileDll.h"

LogFileDll::LogFileDll()
{
	m_hInst = 0;
	LoadLib();
}

LogFileDll::~LogFileDll()
{
	FreeLib();
}

void LogFileDll::LoadLib()
{
	char szFileName[] = "LogFileCtrl.dll";
	m_hInst = LoadLibraryA(szFileName);

	if (m_hInst)
	{
		_Out_LMC_Create = (_Out_LMC_Create_Proc)GetProcAddress(m_hInst, "_Out_LMC_Create");
		_Out_LMC_Destroy = (_Out_LMC_Destroy_Proc)GetProcAddress(m_hInst, "_Out_LMC_Destroy");
		_Out_LMC_Init = (_Out_LMC_Init_Proc)GetProcAddress(m_hInst, "_Out_LMC_Init");
		_Out_LMC_AddLog = (_Out_LMC_AddLog_Proc)GetProcAddress(m_hInst, "_Out_LMC_AddLog");
		_Out_LMC_Suspend = (_Out_LMC_Suspend_Proc)GetProcAddress(m_hInst, "_Out_LMC_Suspend");
		_Out_LMC_Resume = (_Out_LMC_Resume_Proc)GetProcAddress(m_hInst, "_Out_LMC_Resume");

		_Out_LGD_Create = (_Out_LGD_Create_Proc)GetProcAddress(m_hInst, "_Out_LGD_Create");
		_Out_LGD_Destroy = (_Out_LGD_Destroy_Proc)GetProcAddress(m_hInst, "_Out_LGD_Destroy");
		_Out_LGD_Init = (_Out_LGD_Init_Proc)GetProcAddress(m_hInst, "_Out_LGD_Init");
		_Out_LGD_AddLogW = (_Out_LGD_AddLogW_Proc)GetProcAddress(m_hInst, "_Out_LGD_AddLogW");
		_Out_LGD_AddLog = (_Out_LGD_AddLog_Proc)GetProcAddress(m_hInst, "_Out_LGD_AddLog");

		_Out_LGD_DoRecycle = (_Out_LGD_DoRecycle_Proc)GetProcAddress(m_hInst, "_Out_LGD_DoRecycle");
	}
}

void LogFileDll::FreeLib()
{
	if (m_hInst)
		FreeLibrary(m_hInst);
}