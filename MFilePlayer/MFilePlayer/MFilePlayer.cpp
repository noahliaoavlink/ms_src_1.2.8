// MFilePlayer.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "MFilePlayer.h"
#include "AVFilePlayer.h"

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

// CMFilePlayerApp

BEGIN_MESSAGE_MAP(CMFilePlayerApp, CWinApp)
END_MESSAGE_MAP()


// CMFilePlayerApp construction

CMFilePlayerApp::CMFilePlayerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMFilePlayerApp object

CMFilePlayerApp theApp;


// CMFilePlayerApp initialization

BOOL CMFilePlayerApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

extern "C" __declspec( dllexport )void* _In_MFP_Create()
{
	return new AVFilePlayer;
}

extern "C" __declspec( dllexport )void _In_MFP_Destroy(void* pMFPObj)
{
	delete (AVFilePlayer*)pMFPObj;
}

extern "C" __declspec( dllexport )void _In_MFP_Init(void* pMFPObj,HWND hWnd,FilePlayerCallback* pFilePlayerCallback)
{
	((AVFilePlayer*)pMFPObj)->Init(hWnd,pFilePlayerCallback);
}

extern "C" __declspec( dllexport )int _In_MFP_Open(void* pMFPObj,char* szFileName)
{
	return ((AVFilePlayer*)pMFPObj)->Open(szFileName);
}

extern "C" __declspec( dllexport )void _In_MFP_Close(void* pMFPObj)
{
	((AVFilePlayer*)pMFPObj)->Close();
}

extern "C" __declspec( dllexport )void _In_MFP_Play(void* pMFPObj)
{
	((AVFilePlayer*)pMFPObj)->Play();
}

extern "C" __declspec( dllexport )void _In_MFP_Stop(void* pMFPObj)
{
	((AVFilePlayer*)pMFPObj)->Stop();
}
/*
extern "C" __declspec(dllexport)void _In_MFP_Stop2(void* pMFPObj)
{
	((AVFilePlayer*)pMFPObj)->Stop2();
}
*/
extern "C" __declspec( dllexport )void _In_MFP_SetRepeat(void* pMFPObj,bool bEnable)
{
	((AVFilePlayer*)pMFPObj)->SetRepeat(bEnable);
}

extern "C" __declspec( dllexport )void _In_MFP_EnableFadeIn(void* pMFPObj,bool bEnable)
{
	((AVFilePlayer*)pMFPObj)->EnableFadeIn(bEnable);
}

extern "C" __declspec( dllexport )void _In_MFP_EnableFadeOut(void* pMFPObj,bool bEnable)
{
	((AVFilePlayer*)pMFPObj)->EnableFadeOut(bEnable);
}

extern "C" __declspec(dllexport)void _In_MFP_SetOutputVideoFormat(void* pMFPObj, int iFormat)
{
	((AVFilePlayer*)pMFPObj)->SetOutputVideoFormat(iFormat);
}

extern "C" __declspec(dllexport)void _In_MFP_Seek(void* pMFPObj, double dPos)
{
	((AVFilePlayer*)pMFPObj)->Seek(dPos);
}

extern "C" __declspec(dllexport)void _In_MFP_EnableAudio(void* pMFPObj, bool bEnable)
{
	((AVFilePlayer*)pMFPObj)->EnableAudio(bEnable);
}

extern "C" __declspec(dllexport)float _In_MFP_GetVideoFPS(void* pMFPObj)
{
	return ((AVFilePlayer*)pMFPObj)->GetVideoFPS();
}

extern "C" __declspec(dllexport)int _In_MFP_GetTotalOfFrames(void* pMFPObj)
{
	return ((AVFilePlayer*)pMFPObj)->GetTotalOfFrames();
}

extern "C" __declspec(dllexport)int _In_MFP_GetVideoFrame(void* pMFPObj,unsigned char* pBuffer, int* w, int* h, double* pos, char* szTime)
{
	return ((AVFilePlayer*)pMFPObj)->GetVideoFrame(pBuffer, w, h, pos, szTime);
}

extern "C" __declspec(dllexport)int _In_MFP_GetVideoWidth(void* pMFPObj)
{
	return ((AVFilePlayer*)pMFPObj)->GetVideoWidth();
}

extern "C" __declspec(dllexport)int _In_MFP_GetVideoHeight(void* pMFPObj)
{
	return ((AVFilePlayer*)pMFPObj)->GetVideoHeight();
}

extern "C" __declspec(dllexport)void _In_MFP_SpeedUp(void* pMFPObj)
{
	((AVFilePlayer*)pMFPObj)->SpeedUp();
}

extern "C" __declspec(dllexport)void _In_MFP_SpeedDown(void* pMFPObj)
{
	((AVFilePlayer*)pMFPObj)->SpeedDown();
}

extern "C" __declspec(dllexport)void _In_MFP_PlayBackward(void* pMFPObj)
{
	((AVFilePlayer*)pMFPObj)->PlayBackward();
}

extern "C" __declspec(dllexport)void _In_MFP_SetVolume(void* pMFPObj,int iValue)
{
	((AVFilePlayer*)pMFPObj)->SetVolume(iValue);
}

extern "C" __declspec(dllexport)int _In_MFP_GetVolume(void* pMFPObj)
{
	return ((AVFilePlayer*)pMFPObj)->GetVolume();
}

extern "C" __declspec(dllexport)void _In_MFP_SetBrightness(void* pMFPObj, int iValue)
{
	((AVFilePlayer*)pMFPObj)->SetBrightness(iValue);
}

extern "C" __declspec(dllexport)void _In_MFP_SetContrast(void* pMFPObj, int iValue)
{
	((AVFilePlayer*)pMFPObj)->SetContrast(iValue);
}

extern "C" __declspec(dllexport)void _In_MFP_SetSaturation(void* pMFPObj, int iValue)
{
	((AVFilePlayer*)pMFPObj)->SetSaturation(iValue);
}

extern "C" __declspec(dllexport)int _In_MFP_GetPlaybackMode(void* pMFPObj)
{
	return ((AVFilePlayer*)pMFPObj)->GetPlaybackMode();
}

extern "C" __declspec(dllexport)void _In_MFP_Pause(void* pMFPObj)
{
	((AVFilePlayer*)pMFPObj)->Pause();
}

extern "C" __declspec(dllexport)void _In_MFP_SetHue(void* pMFPObj, int iValue)
{
	((AVFilePlayer*)pMFPObj)->SetHue(iValue);
}

extern "C" __declspec(dllexport)void _In_MFP_SetR(void* pMFPObj, int iValue)
{
	((AVFilePlayer*)pMFPObj)->SetR(iValue);
}

extern "C" __declspec(dllexport)void _In_MFP_SetG(void* pMFPObj, int iValue)
{
	((AVFilePlayer*)pMFPObj)->SetG(iValue);
}

extern "C" __declspec(dllexport)void _In_MFP_SetB(void* pMFPObj, int iValue)
{
	((AVFilePlayer*)pMFPObj)->SetB(iValue);
}

extern "C" __declspec(dllexport)void _In_MFP_SetGray(void* pMFPObj, int iValue)
{
	((AVFilePlayer*)pMFPObj)->SetGray(iValue);
}

extern "C" __declspec(dllexport)void _In_MFP_SetNegative(void* pMFPObj, int iValue)
{
	((AVFilePlayer*)pMFPObj)->SetNegative(iValue);
}

extern "C" __declspec(dllexport)void _In_MFP_SetSystemVolume(void* pMFPObj, int iValue)
{
	((AVFilePlayer*)pMFPObj)->SetSystemVolume(iValue);
}

extern "C" __declspec(dllexport)int _In_MFP_GetSystemVolume(void* pMFPObj)
{
	return ((AVFilePlayer*)pMFPObj)->GetSystemVolume();
}

extern "C" __declspec(dllexport)void _In_MFP_SetFadeDuration(void* pMFPObj, float fDuration)
{
	((AVFilePlayer*)pMFPObj)->SetFadeDuration(fDuration);
}

extern "C" __declspec(dllexport)void _In_MFP_SetupDXVA2(void* pMFPObj, void* pD3DDevice)
{
	((AVFilePlayer*)pMFPObj)->SetupDXVA2(pD3DDevice);
}

extern "C" __declspec(dllexport)void _In_MFP_EnableGPU(void* pMFPObj, bool bEnable)
{
	((AVFilePlayer*)pMFPObj)->EnableGPU(bEnable);
}

extern "C" __declspec(dllexport)bool _In_MFP_GPUIsEnabled(void* pMFPObj)
{
	return ((AVFilePlayer*)pMFPObj)->GPUIsEnabled();
}

extern "C" __declspec(dllexport)bool _In_MFP_IsOpened(void* pMFPObj)
{
	return ((AVFilePlayer*)pMFPObj)->IsOpened();
}

extern "C" __declspec(dllexport)bool _In_MFP_IsEOF(void* pMFPObj, bool bEOF2)
{
	return ((AVFilePlayer*)pMFPObj)->IsEOF(bEOF2);
}
extern "C" __declspec(dllexport)void _In_MFP_SetScale(void* pMFPObj, float fW, float fH)
{
	((AVFilePlayer*)pMFPObj)->SetScale(fW, fH);
}
/*
extern "C" __declspec(dllexport)void _In_MFP_EnableGlobalMode(void* pMFPObj, bool bEnable)
{
	((AVFilePlayer*)pMFPObj)->EnableGlobalMode(bEnable);
}
*/
extern "C" __declspec(dllexport)void _In_MFP_UpdateGlobalTimeCode(void* pMFPObj, double dTimeCode)
{
	((AVFilePlayer*)pMFPObj)->UpdateGlobalTimeCode(dTimeCode);
}

extern "C" __declspec(dllexport)void _In_MFP_ResetFilter(void* pMFPObj)
{
	((AVFilePlayer*)pMFPObj)->ResetFilter();
}

extern "C" __declspec(dllexport)int _In_MFP_GetAudioInfo(void* pMFPObj, int& iChannelLayout, int& iSampleRate, int& iNBSamples)
{
	return ((AVFilePlayer*)pMFPObj)->GetAudioInfo(iChannelLayout, iSampleRate, iNBSamples);
}

extern "C" __declspec(dllexport)void _In_MFP_SetRefTimeCode(void* pMFPObj, double dNewRefTime)
{
	((AVFilePlayer*)pMFPObj)->SetReferenceTimeCode(dNewRefTime);
}

extern "C" __declspec(dllexport)void _In_MFP_SetDisplayIntervalMode(void* pMFPObj, int iMode)
{
	((AVFilePlayer*)pMFPObj)->SetDisplayIntervalMode(iMode);
}

extern "C" __declspec(dllexport)void _In_MFP_SetStartTime(void* pMFPObj, double dTime)
{
	((AVFilePlayer*)pMFPObj)->SetStartTime(dTime);
}

extern "C" __declspec(dllexport)void _In_MFP_EnableReSampleRate(void* pMFPObj, bool bEnable)
{
	((AVFilePlayer*)pMFPObj)->EnableReSampleRate(bEnable);
}

extern "C" __declspec(dllexport)void* _In_MFP_GetD3DDevice(void* pMFPObj)
{
	return ((AVFilePlayer*)pMFPObj)->GetD3DDevice();
}

extern "C" __declspec(dllexport)bool _In_MFP_IsAudioFile(void* pMFPObj)
{
	return ((AVFilePlayer*)pMFPObj)->IsAudioFile();
}

extern "C" __declspec(dllexport)void _In_MFP_EnableSuspendDisplay(void* pMFPObj,bool bEnable)
{
	((AVFilePlayer*)pMFPObj)->EnableSuspendDisplay(bEnable);
}

extern "C" __declspec(dllexport)void _In_MFP_SetAudioDelay(void* pMFPObj, int iValue)
{
	((AVFilePlayer*)pMFPObj)->SetAudioDelay(iValue);
}

extern "C" __declspec(dllexport)void _In_MFP_SetReSampleRateOffset(void* pMFPObj, float fOffset)
{
	((AVFilePlayer*)pMFPObj)->SetReSampleRateOffset(fOffset);
}

extern "C" __declspec(dllexport)void _In_MFP_SetProcessFPS(void* pMFPObj, float fFPS)
{
	((AVFilePlayer*)pMFPObj)->SetProcessFPS(fFPS);
}