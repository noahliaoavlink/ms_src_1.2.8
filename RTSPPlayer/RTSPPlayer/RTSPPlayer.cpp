// RTSPPlayer.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "RTSPPlayer.h"
#include "StreamPlayer.h"

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

// CRTSPPlayerApp

BEGIN_MESSAGE_MAP(CRTSPPlayerApp, CWinApp)
END_MESSAGE_MAP()


// CRTSPPlayerApp construction

CRTSPPlayerApp::CRTSPPlayerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CRTSPPlayerApp object

CRTSPPlayerApp theApp;


// CRTSPPlayerApp initialization

BOOL CRTSPPlayerApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

extern "C" __declspec(dllexport)void* _In_RTSP_Create()
{
	return new StreamPlayer;
}

extern "C" __declspec(dllexport)void _In_RTSP_Destroy(void* pRTSPCObj)
{
	delete (StreamPlayer*)pRTSPCObj;
}

extern "C" __declspec(dllexport)void _In_RTSP_Init(void* pRTSPCObj,FilePlayerCallback* pFilePlayerCallback)
{
	((StreamPlayer*)pRTSPCObj)->Init(pFilePlayerCallback);
}

extern "C" __declspec(dllexport)void _In_RTSP_Open(void* pRTSPCObj,char* szUrl)
{
	((StreamPlayer*)pRTSPCObj)->Open(szUrl);
}

extern "C" __declspec(dllexport)void _In_RTSP_Close(void* pRTSPCObj)
{
	((StreamPlayer*)pRTSPCObj)->Close();
}

extern "C" __declspec(dllexport)void _In_RTSP_SetOutputVideoFormat(void* pRTSPCObj, int iFormat)
{
	((StreamPlayer*)pRTSPCObj)->SetOutputVideoFormat(iFormat);
}

extern "C" __declspec(dllexport)void _In_RTSP_EnableAudio(void* pRTSPCObj, bool bEnable)
{
	((StreamPlayer*)pRTSPCObj)->EnableAudio(bEnable);
}

extern "C" __declspec(dllexport)float _In_RTSP_GetFPS(void* pRTSPCObj)
{
	return ((StreamPlayer*)pRTSPCObj)->GetFPS();
}

extern "C" __declspec(dllexport)int _In_RTSP_GetVideoCodecID(void* pRTSPCObj)
{
	return ((StreamPlayer*)pRTSPCObj)->GetVideoCodecID();
}

extern "C" __declspec(dllexport)int _In_RTSP_GetAudioCodecID(void* pRTSPCObj)
{
	return ((StreamPlayer*)pRTSPCObj)->GetAudioCodecID();
}

extern "C" __declspec(dllexport)char* _In_RTSP_GetVideoCodecName(void* pRTSPCObj)
{
	return ((StreamPlayer*)pRTSPCObj)->GetVideoCodecName();
}

extern "C" __declspec(dllexport)char* _In_RTSP_GetAudioCodecName(void* pRTSPCObj)
{
	return ((StreamPlayer*)pRTSPCObj)->GetAudioCodecName();
}

extern "C" __declspec(dllexport)int _In_RTSP_GetVideoWidth(void* pRTSPCObj)
{
	return ((StreamPlayer*)pRTSPCObj)->GetVideoWidth();
}

extern "C" __declspec(dllexport)int _In_RTSP_GetVideoHeight(void* pRTSPCObj)
{
	return ((StreamPlayer*)pRTSPCObj)->GetVideoHeight();
}

extern "C" __declspec(dllexport)void _In_RTSP_Play(void* pRTSPCObj)
{
	((StreamPlayer*)pRTSPCObj)->Play();
}

extern "C" __declspec(dllexport)void _In_RTSP_Stop(void* pRTSPCObj)
{
	((StreamPlayer*)pRTSPCObj)->Stop();
}

extern "C" __declspec(dllexport)int _In_RTSP_GetVolume(void* pRTSPCObj)
{
	return ((StreamPlayer*)pRTSPCObj)->GetVolume();
}

extern "C" __declspec(dllexport)void _In_RTSP_SetVolume(void* pRTSPCObj, int iValue)
{
	((StreamPlayer*)pRTSPCObj)->SetVolume(iValue);
}

extern "C" __declspec(dllexport)bool _In_RTSP_IsOpened(void* pRTSPCObj)
{
	return ((StreamPlayer*)pRTSPCObj)->IsOpened();
}

extern "C" __declspec(dllexport)unsigned char* _In_RTSP_GetFrameBuffer(void* pRTSPCObj)
{
	return ((StreamPlayer*)pRTSPCObj)->GetFrameBuffer();
}

extern "C" __declspec(dllexport)int _In_RTSP_CheckNetworkStatus(void* pRTSPCObj, char* szUrl)
{
	return ((StreamPlayer*)pRTSPCObj)->CheckNetworkStatus(szUrl);
}
