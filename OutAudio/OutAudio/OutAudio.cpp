// OutAudio.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "OutAudio.h"
#include "OutAudioDevice.h"

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

// COutAudioApp

BEGIN_MESSAGE_MAP(COutAudioApp, CWinApp)
END_MESSAGE_MAP()


// COutAudioApp construction

COutAudioApp::COutAudioApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only COutAudioApp object

COutAudioApp theApp;


// COutAudioApp initialization

BOOL COutAudioApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

extern "C" __declspec( dllexport )void* _Out_OAD_Create()
{
	return new OutAudioDevice;
}

extern "C" __declspec( dllexport )void _Out_OAD_Destroy(void* pOADObj)
{
	delete (OutAudioDevice*)pOADObj;
}

extern "C" __declspec( dllexport )void _Out_OAD_Init(void* pOADObj,HWND hWnd)
{
	((OutAudioDevice*)pOADObj)->Init(hWnd);
}

extern "C" __declspec( dllexport )void _Out_OAD_Open(void* pOADObj)
{
	((OutAudioDevice*)pOADObj)->Open(false);
}

extern "C" __declspec( dllexport )void _Out_OAD_Close(void* pOADObj)
{
	((OutAudioDevice*)pOADObj)->Close();
}

extern "C" __declspec( dllexport )void _Out_OAD_Output(void* pOADObj,char* pBuffer,int iLen)
{
	((OutAudioDevice*)pOADObj)->Output(pBuffer,iLen);
}

extern "C" __declspec( dllexport )void _Out_OAD_Stop(void* pOADObj)
{
	((OutAudioDevice*)pOADObj)->Stop();
}

extern "C" __declspec( dllexport )void _Out_OAD_Continue(void* pOADObj)
{
	((OutAudioDevice*)pOADObj)->Continue();
}

extern "C" __declspec( dllexport )void _Out_OAD_SetWaveFormat(void* pOADObj,int nChannels,int nFrequency,int nBits)
{
	((OutAudioDevice*)pOADObj)->SetWaveFormat(nChannels,nFrequency,nBits);
}

extern "C" __declspec( dllexport )void _Out_OAD_SetBufferInfo(void* pOADObj,int iBufferSize,int iTotalOfBuffers)
{
	((OutAudioDevice*)pOADObj)->SetBufferInfo(iBufferSize,iTotalOfBuffers);
}

extern "C" __declspec( dllexport )bool _Out_OAD_IsReady(void* pOADObj)
{
	return ((OutAudioDevice*)pOADObj)->IsReady();
}

extern "C" __declspec( dllexport )void _Out_OAD_SetVolume(void* pOADObj,int iValue)
{
	((OutAudioDevice*)pOADObj)->SetVolume(iValue);
}

extern "C" __declspec( dllexport )bool _Out_OAD_IsOpened(void* pOADObj)
{
	return ((OutAudioDevice*)pOADObj)->IsOpened();
}

extern "C" __declspec(dllexport)void _Out_OAD_ResetBuffer(void* pOADObj)
{
	((OutAudioDevice*)pOADObj)->ResetBuffer();
}

extern "C" __declspec(dllexport)long _Out_OAD_GetVolume(void* pOADObj)
{
	return ((OutAudioDevice*)pOADObj)->GetVolume();
}

extern "C" __declspec(dllexport)bool _Out_OAD_IsFulled(void* pOADObj)
{
	return ((OutAudioDevice*)pOADObj)->IsFulled();
}
extern "C" __declspec(dllexport)int _Out_OAD_GetBufferCount(void* pOADObj)
{
	return ((OutAudioDevice*)pOADObj)->GetBufferCount();
}

extern "C" __declspec(dllexport)void _Out_OAD_AddTimestamp(void* pOADObj,unsigned long ulTimestamp)
{
	((OutAudioDevice*)pOADObj)->AddTimestamp(ulTimestamp);
}

extern "C" __declspec(dllexport)unsigned long _Out_OAD_GetLastTimestamp(void* pOADObj)
{
	return ((OutAudioDevice*)pOADObj)->GetLastTimestamp();
}