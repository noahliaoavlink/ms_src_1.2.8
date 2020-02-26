#include "stdafx.h"
#include "MFilePlayerDll.h"

MFilePlayerDll::MFilePlayerDll()
{
	m_hInst = 0;
	m_pMFPObj = 0;
}

MFilePlayerDll::~MFilePlayerDll()
{
	if(m_pMFPObj)
		_In_MFP_Destroy(m_pMFPObj);

	FreeLib();
}

bool MFilePlayerDll::LoadLib()
{
	char szFileName[] = "MFilePlayer.dll";
	m_hInst = LoadLibraryA(szFileName);
	
	if(m_hInst)
	{
		_In_MFP_Create = (_In_MFP_Create_Proc)GetProcAddress(m_hInst, "_In_MFP_Create");
		_In_MFP_Destroy = (_In_MFP_Destroy_Proc)GetProcAddress(m_hInst, "_In_MFP_Destroy");
		_In_MFP_Init = (_In_MFP_Init_Proc)GetProcAddress(m_hInst, "_In_MFP_Init");
		_In_MFP_Open = (_In_MFP_Open_Proc)GetProcAddress(m_hInst, "_In_MFP_Open");
		_In_MFP_Close = (_In_MFP_Close_Proc)GetProcAddress(m_hInst, "_In_MFP_Close");
		_In_MFP_Play = (_In_MFP_Play_Proc)GetProcAddress(m_hInst, "_In_MFP_Play");
		_In_MFP_Stop = (_In_MFP_Stop_Proc)GetProcAddress(m_hInst, "_In_MFP_Stop");
		_In_MFP_SetRepeat = (_In_MFP_SetRepeat_Proc)GetProcAddress(m_hInst, "_In_MFP_SetRepeat");
		_In_MFP_EnableFadeIn = (_In_MFP_EnableFadeIn_Proc)GetProcAddress(m_hInst, "_In_MFP_EnableFadeIn");
		_In_MFP_EnableFadeOut = (_In_MFP_EnableFadeOut_Proc)GetProcAddress(m_hInst, "_In_MFP_EnableFadeOut");
		_In_MFP_SetOutputVideoFormat = (_In_MFP_SetOutputVideoFormat_Proc)GetProcAddress(m_hInst, "_In_MFP_SetOutputVideoFormat");
		_In_MFP_Seek = (_In_MFP_Seek_Proc)GetProcAddress(m_hInst, "_In_MFP_Seek");
		_In_MFP_EnableAudio = (_In_MFP_EnableAudio_Proc)GetProcAddress(m_hInst, "_In_MFP_EnableAudio");
		_In_MFP_GetVideoFPS = (_In_MFP_GetVideoFPS_Proc)GetProcAddress(m_hInst, "_In_MFP_GetVideoFPS");
		_In_MFP_GetTotalOfFrames = (_In_MFP_GetTotalOfFrames_Proc)GetProcAddress(m_hInst, "_In_MFP_GetTotalOfFrames");
		_In_MFP_GetVideoWidth = (_In_MFP_GetVideoWidth_Proc)GetProcAddress(m_hInst, "_In_MFP_GetVideoWidth");
		_In_MFP_GetVideoHeight = (_In_MFP_GetVideoHeight_Proc)GetProcAddress(m_hInst, "_In_MFP_GetVideoHeight");
		_In_MFP_SpeedUp = (_In_MFP_SpeedUp_Proc)GetProcAddress(m_hInst, "_In_MFP_SpeedUp");
		_In_MFP_SpeedDown = (_In_MFP_SpeedDown_Proc)GetProcAddress(m_hInst, "_In_MFP_SpeedDown");
		_In_MFP_PlayBackward = (_In_MFP_PlayBackward_Proc)GetProcAddress(m_hInst, "_In_MFP_PlayBackward");

		_In_MFP_SetVolume = (_In_MFP_SetVolume_Proc)GetProcAddress(m_hInst, "_In_MFP_SetVolume");
		_In_MFP_GetVolume = (_In_MFP_GetVolume_Proc)GetProcAddress(m_hInst, "_In_MFP_GetVolume");
		_In_MFP_SetBrightness = (_In_MFP_SetBrightness_Proc)GetProcAddress(m_hInst, "_In_MFP_SetBrightness");
		_In_MFP_SetContrast = (_In_MFP_SetContrast_Proc)GetProcAddress(m_hInst, "_In_MFP_SetContrast");
		_In_MFP_SetSaturation = (_In_MFP_SetSaturation_Proc)GetProcAddress(m_hInst, "_In_MFP_SetSaturation");
		_In_MFP_GetPlaybackMode = (_In_MFP_GetPlaybackMode_Proc)GetProcAddress(m_hInst, "_In_MFP_GetPlaybackMode");
		_In_MFP_Pause = (_In_MFP_Pause_Proc)GetProcAddress(m_hInst, "_In_MFP_Pause");

		_In_MFP_SetHue = (_In_MFP_SetHue_Proc)GetProcAddress(m_hInst, "_In_MFP_SetHue");
		_In_MFP_SetR = (_In_MFP_SetR_Proc)GetProcAddress(m_hInst, "_In_MFP_SetR");
		_In_MFP_SetG = (_In_MFP_SetG_Proc)GetProcAddress(m_hInst, "_In_MFP_SetG");
		_In_MFP_SetB = (_In_MFP_SetB_Proc)GetProcAddress(m_hInst, "_In_MFP_SetB");

		_In_MFP_SetSystemVolume = (_In_MFP_SetSystemVolume_Proc)GetProcAddress(m_hInst, "_In_MFP_SetSystemVolume");
		_In_MFP_GetSystemVolume = (_In_MFP_GetSystemVolume_Proc)GetProcAddress(m_hInst, "_In_MFP_GetSystemVolume");

		_In_MFP_SetFadeDuration = (_In_MFP_SetFadeDuration_Proc)GetProcAddress(m_hInst, "_In_MFP_SetFadeDuration");

		_In_MFP_SetupDXVA2 = (_In_MFP_SetupDXVA2_Proc)GetProcAddress(m_hInst, "_In_MFP_SetupDXVA2");
		_In_MFP_EnableGPU = (_In_MFP_EnableGPU_Proc)GetProcAddress(m_hInst, "_In_MFP_EnableGPU");
		_In_MFP_GPUIsEnabled = (_In_MFP_GPUIsEnabled_Proc)GetProcAddress(m_hInst, "_In_MFP_GPUIsEnabled");
		_In_MFP_IsOpened = (_In_MFP_IsOpened_Proc)GetProcAddress(m_hInst, "_In_MFP_IsOpened");
		_In_MFP_SetScale = (_In_MFP_SetScale_Proc)GetProcAddress(m_hInst, "_In_MFP_SetScale");

		_In_MFP_SetGray = (_In_MFP_SetGray_Proc)GetProcAddress(m_hInst, "_In_MFP_SetGray");
		_In_MFP_SetNegative = (_In_MFP_SetNegative_Proc)GetProcAddress(m_hInst, "_In_MFP_SetNegative");
		_In_MFP_ResetFilter = (_In_MFP_ResetFilter_Proc)GetProcAddress(m_hInst, "_In_MFP_ResetFilter");
		_In_MFP_GetAudioInfo = (_In_MFP_GetAudioInfo_Proc)GetProcAddress(m_hInst, "_In_MFP_GetAudioInfo");

		_In_MFP_SetRefTimeCode = (_In_MFP_SetRefTimeCode_Proc)GetProcAddress(m_hInst, "_In_MFP_SetRefTimeCode");

		//_In_MFP_EnableGlobalMode = (_In_MFP_EnableGlobalMode_Proc)GetProcAddress(m_hInst, "_In_MFP_EnableGlobalMode");
		_In_MFP_UpdateGlobalTimeCode = (_In_MFP_UpdateGlobalTimeCode_Proc)GetProcAddress(m_hInst, "_In_MFP_UpdateGlobalTimeCode");
		_In_MFP_SetDisplayIntervalMode = (_In_MFP_SetDisplayIntervalMode_Proc)GetProcAddress(m_hInst, "_In_MFP_SetDisplayIntervalMode");
		_In_MFP_SetStartTime = (_In_MFP_SetStartTime_Proc)GetProcAddress(m_hInst, "_In_MFP_SetStartTime");

		m_pMFPObj = _In_MFP_Create();
		return true;
	}
	return false;
}

void MFilePlayerDll::FreeLib()
{
	if(m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = 0;
	}
}

void MFilePlayerDll::Init(HWND hWnd,FilePlayerCallback* pFilePlayerCallback)
{
	if(m_pMFPObj)
		_In_MFP_Init(m_pMFPObj,hWnd,pFilePlayerCallback);
}

void MFilePlayerDll::Open(char* szFileName)
{
	if(m_pMFPObj)
		_In_MFP_Open(m_pMFPObj,szFileName);
}

void MFilePlayerDll::Close()
{
	if(m_pMFPObj)
		_In_MFP_Close(m_pMFPObj);
}

void MFilePlayerDll::Play()
{
	if(m_pMFPObj)
		_In_MFP_Play(m_pMFPObj);
}

void MFilePlayerDll::Stop()
{
	if(m_pMFPObj)
		_In_MFP_Stop(m_pMFPObj);
}

void MFilePlayerDll::SetRepeat(bool bEnable)
{
	if(m_pMFPObj)
		_In_MFP_SetRepeat(m_pMFPObj,bEnable);
}

void MFilePlayerDll::EnableFadeIn(bool bEnable)
{
	if(m_pMFPObj)
		_In_MFP_EnableFadeIn(m_pMFPObj,bEnable);
}

void MFilePlayerDll::EnableFadeOut(bool bEnable)
{
	if(m_pMFPObj)
		_In_MFP_EnableFadeOut(m_pMFPObj,bEnable);
}

void MFilePlayerDll::SetOutputVideoFormat(int iFormat)
{
	if (m_pMFPObj)
		_In_MFP_SetOutputVideoFormat(m_pMFPObj, iFormat);
}

void MFilePlayerDll::Seek(double dPos)
{
	if (m_pMFPObj)
		_In_MFP_Seek(m_pMFPObj, dPos);
}

void MFilePlayerDll::EnableAudio(bool bEnable)
{
	if (m_pMFPObj)
		_In_MFP_EnableAudio(m_pMFPObj, bEnable);
	
}

float MFilePlayerDll::GetVideoFPS()
{
	if (m_pMFPObj)
		return _In_MFP_GetVideoFPS(m_pMFPObj);
	return 0.0;
}

int MFilePlayerDll::GetTotalOfFrames()
{
	if (m_pMFPObj)
		return _In_MFP_GetTotalOfFrames(m_pMFPObj);
	return 0;
}
/*
int MFilePlayerDll::GetVideoFrame(unsigned char* pBuffer, int* w, int* h, double* pos, char* szTime)
{
	if (m_pMFPObj)
		return _In_MFP_GetVideoFrame(m_pMFPObj, pBuffer, w, h, pos, szTime);
	return 0;
}
*/
int MFilePlayerDll::GetVideoWidth()
{
	if (m_pMFPObj)
		return _In_MFP_GetVideoWidth(m_pMFPObj);
	return 0;
}

int MFilePlayerDll::GetVideoHeight()
{
	if (m_pMFPObj)
		return _In_MFP_GetVideoHeight(m_pMFPObj);
	return 0;
}

void MFilePlayerDll::SpeedUp()
{
	if (m_pMFPObj)
		return _In_MFP_SpeedUp(m_pMFPObj);
}

void MFilePlayerDll::SpeedDown()
{
	if (m_pMFPObj)
		return _In_MFP_SpeedDown(m_pMFPObj);
}

void MFilePlayerDll::PlayBackward()
{
	if (m_pMFPObj)
		_In_MFP_PlayBackward(m_pMFPObj);	
}

void MFilePlayerDll::SetVolume(int iValue)
{
	if (m_pMFPObj)
		_In_MFP_SetVolume(m_pMFPObj, iValue);
}

int MFilePlayerDll::GetVolume()
{
	if (m_pMFPObj)
		return _In_MFP_GetVolume(m_pMFPObj);
	return 0;
}

void MFilePlayerDll::SetBrightness(int iValue)
{
	if (m_pMFPObj)
		_In_MFP_SetBrightness(m_pMFPObj, iValue);
}

void MFilePlayerDll::SetContrast(int iValue)
{
	if (m_pMFPObj)
		_In_MFP_SetContrast(m_pMFPObj, iValue);
}

void MFilePlayerDll::SetSaturation(int iValue)
{
	if (m_pMFPObj)
		_In_MFP_SetSaturation(m_pMFPObj, iValue);
}

void MFilePlayerDll::SetHue(int iValue)
{
	if (m_pMFPObj)
		_In_MFP_SetHue(m_pMFPObj, iValue);
}

void MFilePlayerDll::SetR(int iValue)
{
	if (m_pMFPObj)
		_In_MFP_SetR(m_pMFPObj, iValue);
}

void MFilePlayerDll::SetG(int iValue)
{
	if (m_pMFPObj)
		_In_MFP_SetG(m_pMFPObj, iValue);
}

void MFilePlayerDll::SetB(int iValue)
{
	if (m_pMFPObj)
		_In_MFP_SetB(m_pMFPObj, iValue);
}

int MFilePlayerDll::GetPlaybackMode()
{
	if (m_pMFPObj)
		return _In_MFP_GetPlaybackMode(m_pMFPObj);
	return 0;
}

void MFilePlayerDll::Pause()
{
	if (m_pMFPObj)
		_In_MFP_Pause(m_pMFPObj);
}

void MFilePlayerDll::SetSystemVolume(int iValue)
{
	if (m_pMFPObj)
		_In_MFP_SetSystemVolume(m_pMFPObj, iValue);
}

int MFilePlayerDll::GetSystemVolume()
{
	if (m_pMFPObj)
		return _In_MFP_GetSystemVolume(m_pMFPObj);
	return 0;
}

void MFilePlayerDll::UpdateGlobalTimeCode(double dTimeCode)
{
	if (m_pMFPObj)
			_In_MFP_UpdateGlobalTimeCode(m_pMFPObj, dTimeCode);
}

void MFilePlayerDll::SetDisplayIntervalMode( int iMode)
{
	if (m_pMFPObj)
			_In_MFP_SetDisplayIntervalMode(m_pMFPObj, iMode);
}

void MFilePlayerDll::SetupDXVA2(void* pD3DDevice)
{
	if (m_pMFPObj)
		_In_MFP_SetupDXVA2(m_pMFPObj, pD3DDevice);
}

void MFilePlayerDll::EnableGPU(bool pD3DDevice)
{
	if (m_pMFPObj)
		_In_MFP_EnableGPU(m_pMFPObj, pD3DDevice);
}

bool MFilePlayerDll::GPUIsEnabled()
{
	if (m_pMFPObj)
		return _In_MFP_GPUIsEnabled(m_pMFPObj);
}