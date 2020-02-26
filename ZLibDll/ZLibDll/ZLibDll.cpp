// ZLibDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ZLibDll.h"

#include "ZLibAPI.h"

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

// CZLibDllApp

BEGIN_MESSAGE_MAP(CZLibDllApp, CWinApp)
END_MESSAGE_MAP()


// CZLibDllApp construction

CZLibDllApp::CZLibDllApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CZLibDllApp object

CZLibDllApp theApp;


// CZLibDllApp initialization

BOOL CZLibDllApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

extern "C" __declspec(dllexport)void* _ZIP_Create()
{
	return new ZLibAPI;
}

extern "C" __declspec(dllexport)void _ZIP_Destroy(void* pZIPObj)
{
	delete (ZLibAPI*)pZIPObj;
}

extern "C" __declspec(dllexport)void _ZIP_CreateZip(void* pZIPObj,char* szFileName)
{
	((ZLibAPI*)pZIPObj)->CreateZip(szFileName);
}

extern "C" __declspec(dllexport)void _ZIP_CloseZip(void* pZIPObj)
{
	((ZLibAPI*)pZIPObj)->CloseZip();
}

extern "C" __declspec(dllexport)void _ZIP_AddFile(void* pZIPObj, char* szFileNameInZip, char* szSrcFile)
{
	((ZLibAPI*)pZIPObj)->AddFile(szFileNameInZip,szSrcFile);
}

extern "C" __declspec(dllexport)void _ZIP_OpenUnZip(void* pZIPObj, char* szFileName)
{
	((ZLibAPI*)pZIPObj)->OpenUnZip(szFileName);
}

extern "C" __declspec(dllexport)void _ZIP_CloseUnZip(void* pZIPObj)
{
	((ZLibAPI*)pZIPObj)->CloseUnZip();
}

extern "C" __declspec(dllexport)void _ZIP_Extract(void* pZIPObj, char* szOutPath)
{
	((ZLibAPI*)pZIPObj)->Extract(szOutPath);
}