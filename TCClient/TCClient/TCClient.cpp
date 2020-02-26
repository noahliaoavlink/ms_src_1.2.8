// TCClient.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "TCClient.h"
#include "Client.h"

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

// CTCClientApp

BEGIN_MESSAGE_MAP(CTCClientApp, CWinApp)
END_MESSAGE_MAP()


// CTCClientApp construction

CTCClientApp::CTCClientApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CTCClientApp object

CTCClientApp theApp;


// CTCClientApp initialization

BOOL CTCClientApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

extern "C" __declspec(dllexport)void* _Net_TCC_Create()
{
	return new Client;
}

extern "C" __declspec(dllexport)void _Net_TCC_Destroy(void* pTCCObj)
{
	delete (Client*)pTCCObj;
}

extern "C" __declspec(dllexport)void _Net_TCC_SearchServer(void* pTCCObj)
{
	((Client*)pTCCObj)->SearchServer();
}

extern "C" __declspec(dllexport)void* _Net_TCC_GetServerList(void* pTCCObj)
{
	return ((Client*)pTCCObj)->GetServerList();
}

extern "C" __declspec(dllexport)void _Net_TCC_SetEventCallback(void* pTCCObj,EventCallback* pObj)
{
	((Client*)pTCCObj)->SetEventCallback(pObj);
}

extern "C" __declspec(dllexport)bool _Net_TCC_Connect(void* pTCCObj, char* szIP, int iPort)
{
	return ((Client*)pTCCObj)->Connect(szIP, iPort);
}

extern "C" __declspec(dllexport)void _Net_TCC_Close(void* pTCCObj)
{
	((Client*)pTCCObj)->Close();
}

extern "C" __declspec(dllexport)void _Net_TCC_Login(void* pTCCObj, char* szID, char* szPW)
{
	((Client*)pTCCObj)->Login(szID, szPW);
}

extern "C" __declspec(dllexport)bool _Net_TCC_IsConnected(void* pTCCObj)
{
	return ((Client*)pTCCObj)->IsConnected();
}

extern "C" __declspec(dllexport)int _Net_TCC_GetReceiveStatus(void* pTCCObj)
{
	return ((Client*)pTCCObj)->GetReceiveStatus();
}
