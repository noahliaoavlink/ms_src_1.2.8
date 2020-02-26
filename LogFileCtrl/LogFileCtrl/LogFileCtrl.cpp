// LogFileCtrl.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "LogFileCtrl.h"
#include "Log4MediaCtrl.h"
#include "Log4Debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CLogFileCtrlApp

BEGIN_MESSAGE_MAP(CLogFileCtrlApp, CWinApp)
END_MESSAGE_MAP()


// CLogFileCtrlApp construction

CLogFileCtrlApp::CLogFileCtrlApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CLogFileCtrlApp object

CLogFileCtrlApp theApp;


// CLogFileCtrlApp initialization

BOOL CLogFileCtrlApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

extern "C" __declspec(dllexport)void* _Out_LMC_Create()
{
	return new Log4MediaCtrl;
}

extern "C" __declspec(dllexport)void _Out_LMC_Destroy(void* pLMCObj)
{
	delete (Log4MediaCtrl*)pLMCObj;
}

extern "C" __declspec(dllexport)void _Out_LMC_Init(void* pLMCObj)
{
	((Log4MediaCtrl*)pLMCObj)->Init();
}

extern "C" __declspec(dllexport)void _Out_LMC_AddLog(void* pLMCObj,int iCmd, wchar_t* wszFolderName, wchar_t* wszFileName, unsigned long ulFileID)
{
	((Log4MediaCtrl*)pLMCObj)->AddLog(iCmd,wszFolderName,wszFileName, ulFileID);
}

extern "C" __declspec(dllexport)void _Out_LMC_Suspend(void* pLMCObj)
{
	((Log4MediaCtrl*)pLMCObj)->Suspend();
}

extern "C" __declspec(dllexport)void _Out_LMC_Resume(void* pLMCObj)
{
	((Log4MediaCtrl*)pLMCObj)->Resume();
}

extern "C" __declspec(dllexport)void* _Out_LGD_Create()
{
	return new Log4Debug;
}

extern "C" __declspec(dllexport)void _Out_LGD_Destroy(void* pLGDObj)
{
	delete (Log4Debug*)pLGDObj;
}

extern "C" __declspec(dllexport)void _Out_LGD_Init(void* pLGDObj,char* szModulName)
{
	((Log4Debug*)pLGDObj)->Init(szModulName);
}

extern "C" __declspec(dllexport)void _Out_LGD_AddLogW(void* pLGDObj, int iLevel, wchar_t* wszLog)
{
	((Log4Debug*)pLGDObj)->AddLogW(iLevel,wszLog);
}

extern "C" __declspec(dllexport)void _Out_LGD_AddLog(void* pLGDObj, int iLevel, char* szLog)
{
	((Log4Debug*)pLGDObj)->AddLog(iLevel, szLog);
}

extern "C" __declspec(dllexport)void _Out_LGD_DoRecycle(char* szPath)
{
	FileTrimmer File_Trimmer;

	File_Trimmer.SetPath(szPath);
	File_Trimmer.DoRemoveFiles();
}