// MFRecorder.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "MFRecorder.h"

#include "AVRecorder.h"

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

// CMFRecorderApp

BEGIN_MESSAGE_MAP(CMFRecorderApp, CWinApp)
END_MESSAGE_MAP()


// CMFRecorderApp construction

CMFRecorderApp::CMFRecorderApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMFRecorderApp object

CMFRecorderApp theApp;


// CMFRecorderApp initialization

BOOL CMFRecorderApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

extern "C" __declspec(dllexport)void* _Out_MFR_Create()
{
	return new AVRecorder;
}

extern "C" __declspec(dllexport)void _Out_MFR_Destroy(void* pMFRObj)
{
	delete (AVRecorder*)pMFRObj;
}

extern "C" __declspec(dllexport)void _Out_MFR_Init(void* pMFRObj)
{
	((AVRecorder*)pMFRObj)->Init();
}

extern "C" __declspec(dllexport)int _Out_MFR_Open(void* pMFRObj,char* szFileName)
{
	return ((AVRecorder*)pMFRObj)->Open(szFileName);
}

extern "C" __declspec(dllexport)void _Out_MFR_Close(void* pMFRObj)
{
	((AVRecorder*)pMFRObj)->Close();
}

extern "C" __declspec(dllexport)void _Out_MFR_SetVideoInfo(void* pMFRObj,int iWidth, int iHeight, int iBitRate, float fFps)
{
	((AVRecorder*)pMFRObj)->SetVideoInfo(iWidth,iHeight,iBitRate,fFps);
}

extern "C" __declspec(dllexport)void _Out_MFR_SetAudioInfo(void* pMFRObj,int iChannels, int iSamples, int iBitRate)
{
	((AVRecorder*)pMFRObj)->SetAudioInfo(iChannels,iSamples,iBitRate);
}

extern "C" __declspec(dllexport)void _Out_MFR_WriteHeader(void* pMFRObj)
{
	((AVRecorder*)pMFRObj)->WriteHeader();
}

extern "C" __declspec(dllexport)int _Out_MFR_WriteVideoFrame(void* pMFRObj,unsigned char* pBuffer, int iLen)
{
	return ((AVRecorder*)pMFRObj)->WriteVideoFrame(pBuffer,iLen);
}

extern "C" __declspec(dllexport)int _Out_MFR_WriteAudioFrame(void* pMFRObj,unsigned char* pBuffer, int iLen)
{
	return ((AVRecorder*)pMFRObj)->WriteAudioFrame(pBuffer, iLen);
}

extern "C" __declspec(dllexport)void _Out_MFR_SetOriginalAudioInfo(void* pMFRObj, int iChannelLayout, int iSampleRate, int iNBSamples)
{
	((AVRecorder*)pMFRObj)->SetOriginalAudioInfo(iChannelLayout,iSampleRate,iNBSamples);
}