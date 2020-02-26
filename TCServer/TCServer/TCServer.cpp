// TCServer.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "TCServer.h"

#include "ServerManager.h"

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

// CTCServerApp

BEGIN_MESSAGE_MAP(CTCServerApp, CWinApp)
END_MESSAGE_MAP()


// CTCServerApp construction

CTCServerApp::CTCServerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CTCServerApp object

CTCServerApp theApp;


// CTCServerApp initialization

BOOL CTCServerApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

extern "C" __declspec(dllexport)void* _Net_TCS_Create()
{
	return new ServerManager;
}

extern "C" __declspec(dllexport)void _Net_TCS_Destroy(void* pTCSObj)
{
	delete (ServerManager*)pTCSObj;
}

extern "C" __declspec(dllexport)int _Net_TCS_StartServer(void* pTCSObj,int iPort)
{
	return ((ServerManager*)pTCSObj)->Open(iPort);
}

extern "C" __declspec(dllexport)void _Net_TCS_StopServer(void* pTCSObj)
{
	((ServerManager*)pTCSObj)->Close();
}

extern "C" __declspec(dllexport)void _Net_TCS_UpdateTimeCode(void* pTCSObj, unsigned long ulTimeCode)
{
	((ServerManager*)pTCSObj)->UpdateTimeCode(ulTimeCode);
}

extern "C" __declspec(dllexport)void _Net_TCS_SetTotalOfTimeCode(void* pTCSObj, unsigned long ulTimeCode)
{
	((ServerManager*)pTCSObj)->SetTotalOfTimeCode(ulTimeCode);
}

extern "C" __declspec(dllexport)void _Net_TCS_TC_Start(void* pTCSObj)
{
	((ServerManager*)pTCSObj)->TC_Start();
}

extern "C" __declspec(dllexport)void _Net_TCS_TC_Stop(void* pTCSObj)
{
	((ServerManager*)pTCSObj)->TC_Stop();
}

extern "C" __declspec(dllexport)void _Net_TCS_TC_Pause(void* pTCSObj)
{
	((ServerManager*)pTCSObj)->TC_Pause();
}

extern "C" __declspec(dllexport)int _Net_TCS_GetTCStatus(void* pTCSObj)
{
	return ((ServerManager*)pTCSObj)->GetTCStatus();
}

extern "C" __declspec(dllexport)unsigned long _Net_TCS_GetTotalOfTimeCode(void* pTCSObj)
{
	return ((ServerManager*)pTCSObj)->GetTotalOfTimeCode();
}

extern "C" __declspec(dllexport)bool _Net_TCS_IsWorking(void* pTCSObj)
{
	return ((ServerManager*)pTCSObj)->IsWorking();
}

extern "C" __declspec(dllexport)void _Net_TCS_UpdateCurFrameTime(void* pTCSObj,unsigned long ulTime)
{
	((ServerManager*)pTCSObj)->UpdateCurFrameTime(ulTime);
}

extern "C" __declspec(dllexport)int _Net_TCS_GetTotalOfClients(void* pTCSObj)
{
	return ((ServerManager*)pTCSObj)->GetTotalOfClients();
}