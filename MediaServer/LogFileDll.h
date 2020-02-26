#pragma once

#ifndef _LogFileDll_H_
#define _LogFileDll_H_

#include "..\\..\\..\\Include\\LogCommon.h"

typedef void* (*_Out_LMC_Create_Proc)();
typedef void (*_Out_LMC_Destroy_Proc)(void* pLMCObj);
typedef void (*_Out_LMC_Init_Proc)(void* pLMCObj);
typedef void (*_Out_LMC_AddLog_Proc)(void* pLMCObj, int iCmd, wchar_t* wszFolderName, wchar_t* wszFileName, unsigned long ulFileID);
typedef void (*_Out_LMC_Suspend_Proc)(void* pLMCObj);
typedef void (*_Out_LMC_Resume_Proc)(void* pLMCObj);

typedef void* (*_Out_LGD_Create_Proc)();
typedef void (*_Out_LGD_Destroy_Proc)(void* pLGDObj);
typedef void (*_Out_LGD_Init_Proc)(void* pLGDObj, char* szModulName);
typedef void (*_Out_LGD_AddLogW_Proc)(void* pLGDObj, int iLevel, wchar_t* wszLog);
typedef void (*_Out_LGD_AddLog_Proc)(void* pLGDObj, int iLevel, char* szLog);

typedef void (*_Out_LGD_DoRecycle_Proc)(char* szPath);

class LogFileDll
{
		HINSTANCE m_hInst;
	protected:
	public:
		LogFileDll();
		~LogFileDll();

		void LoadLib();
		void FreeLib();

		_Out_LMC_Create_Proc _Out_LMC_Create;
		_Out_LMC_Destroy_Proc _Out_LMC_Destroy;
		_Out_LMC_Init_Proc _Out_LMC_Init;
		_Out_LMC_AddLog_Proc _Out_LMC_AddLog;
		_Out_LMC_Suspend_Proc _Out_LMC_Suspend;
		_Out_LMC_Resume_Proc _Out_LMC_Resume;

		_Out_LGD_Create_Proc _Out_LGD_Create;
		_Out_LGD_Destroy_Proc _Out_LGD_Destroy;
		_Out_LGD_Init_Proc _Out_LGD_Init;
		_Out_LGD_AddLogW_Proc _Out_LGD_AddLogW;
		_Out_LGD_AddLog_Proc _Out_LGD_AddLog;

		_Out_LGD_DoRecycle_Proc _Out_LGD_DoRecycle;
	
};

#endif
