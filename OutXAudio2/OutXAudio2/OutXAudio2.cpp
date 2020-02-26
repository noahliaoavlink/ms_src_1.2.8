// OutXAudio2.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "OutXAudio2.h"

#include "XA2Stream.h"

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

// COutXAudio2App

BEGIN_MESSAGE_MAP(COutXAudio2App, CWinApp)
END_MESSAGE_MAP()


// COutXAudio2App construction

COutXAudio2App::COutXAudio2App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only COutXAudio2App object

COutXAudio2App theApp;


// COutXAudio2App initialization

BOOL COutXAudio2App::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

extern "C" __declspec(dllexport)void* _Out_OXA_Create()
{
	return new XA2Stream;
}

extern "C" __declspec(dllexport)void _Out_OXA_Destroy(void* pOXAObj)
{
	delete (XA2Stream*)pOXAObj;
}

extern "C" __declspec(dllexport)void _Out_OXA_Init(void* pOXAObj)
{
	((XA2Stream*)pOXAObj)->Init();
}

extern "C" __declspec(dllexport)void _Out_OXA_OpenDevice(void* pOXAObj, int nChannels, int nSampleRate, int iBits, int iBufferLen, int iTotalOfBuffers)
{
	((XA2Stream*)pOXAObj)->OpenDevice(nChannels,nSampleRate,iBits,iBufferLen,iTotalOfBuffers);
}

extern "C" __declspec(dllexport)void _Out_OXA_Close(void* pOXAObj)
{
	((XA2Stream*)pOXAObj)->Close();
}

extern "C" __declspec(dllexport)void _Out_OXA_Output(void* pOXAObj, char* pBuffer, int iLen)
{
	((XA2Stream*)pOXAObj)->Output((unsigned char*)pBuffer, iLen);
}

extern "C" __declspec(dllexport)void _Out_OXA_Stop(void* pOXAObj)
{
	((XA2Stream*)pOXAObj)->Stop();
}

extern "C" __declspec(dllexport)void _Out_OXA_Continue(void* pOXAObj)
{
	((XA2Stream*)pOXAObj)->Continue();
}

extern "C" __declspec(dllexport)void _Out_OXA_SetVolume(void* pOXAObj, int iValue)
{
	((XA2Stream*)pOXAObj)->SetVolume(iValue);
}

extern "C" __declspec(dllexport)bool _Out_OXA_IsOpened(void* pOXAObj)
{
	return ((XA2Stream*)pOXAObj)->IsOpened();
}

extern "C" __declspec(dllexport)void _Out_OXA_ResetBuffer(void* pOXAObj)
{
	((XA2Stream*)pOXAObj)->ResetBuffer();
}

extern "C" __declspec(dllexport)long _Out_OXA_GetVolume(void* pOXAObj)
{
	return ((XA2Stream*)pOXAObj)->GetVolume();
}

extern "C" __declspec(dllexport)bool _Out_OXA_IsFulled(void* pOXAObj)
{
	return ((XA2Stream*)pOXAObj)->IsFulled();
}

extern "C" __declspec(dllexport)int _Out_OXA_GetBufferCount(void* pOXAObj)
{
	return ((XA2Stream*)pOXAObj)->GetBufferCount();
}

extern "C" __declspec(dllexport)void _Out_OXA_Pause(void* pOXAObj)
{
	((XA2Stream*)pOXAObj)->Pause();
}