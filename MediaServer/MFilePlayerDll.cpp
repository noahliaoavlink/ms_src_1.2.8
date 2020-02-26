#include "stdafx.h"
#include "MFilePlayerDll.h"

MFilePlayerDll::MFilePlayerDll()
{
	m_hInst = 0;
	
	for (int i = 0; i < _MAX_MF_OBJS; i++)
		m_pMFPObjs[i] = 0;
}

MFilePlayerDll::~MFilePlayerDll()
{
	if (m_hInst)
	{
		for (int i = 0; i < _MAX_MF_OBJS; i++)
		{
			if (m_pMFPObjs[i])
			{
				_In_MFP_Destroy(m_pMFPObjs[i]);
				m_pMFPObjs[i] = 0;
			}
		}
	}
	FreeLib();
}

bool MFilePlayerDll::LoadLib(CString strLibPath)
{
	m_hInst = LoadLibraryA(strLibPath);
	
	if(m_hInst)
	{
		_In_MFP_Create = (_In_MFP_Create_Proc)GetProcAddress(m_hInst, "_In_MFP_Create");
		_In_MFP_Destroy = (_In_MFP_Destroy_Proc)GetProcAddress(m_hInst, "_In_MFP_Destroy");
		_In_MFP_Init = (_In_MFP_Init_Proc)GetProcAddress(m_hInst, "_In_MFP_Init");
		_In_MFP_Open = (_In_MFP_Open_Proc)GetProcAddress(m_hInst, "_In_MFP_Open");
		_In_MFP_Close = (_In_MFP_Close_Proc)GetProcAddress(m_hInst, "_In_MFP_Close");
		_In_MFP_Play = (_In_MFP_Play_Proc)GetProcAddress(m_hInst, "_In_MFP_Play");
		_In_MFP_Stop = (_In_MFP_Stop_Proc)GetProcAddress(m_hInst, "_In_MFP_Stop");
		_In_MFP_Stop2 = (_In_MFP_Stop2_Proc)GetProcAddress(m_hInst, "_In_MFP_Stop2");
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

		_In_MFP_EnableReSampleRate = (_In_MFP_EnableReSampleRate_Proc)GetProcAddress(m_hInst, "_In_MFP_EnableReSampleRate");
		_In_MFP_GetD3DDevice = (_In_MFP_GetD3DDevice_Proc)GetProcAddress(m_hInst, "_In_MFP_GetD3DDevice");
		_In_MFP_IsAudioFile = (_In_MFP_IsAudioFile_Proc)GetProcAddress(m_hInst, "_In_MFP_IsAudioFile");
	
		_In_MFP_EnableSuspendDisplay = (_In_MFP_EnableSuspendDisplay_Proc)GetProcAddress(m_hInst, "_In_MFP_EnableSuspendDisplay");
		_In_MFP_SetAudioDelay = (_In_MFP_SetAudioDelay_Proc)GetProcAddress(m_hInst, "_In_MFP_SetAudioDelay");
		_In_MFP_SetReSampleRateOffset = (_In_MFP_SetReSampleRateOffset_Proc)GetProcAddress(m_hInst, "_In_MFP_SetReSampleRateOffset");

		_In_MFP_SetProcessFPS = (_In_MFP_SetProcessFPS_Proc)GetProcAddress(m_hInst, "_In_MFP_SetProcessFPS");

		//m_pMFPObj = _In_MFP_Create();
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

void MFilePlayerDll::Create(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
		{
			_In_MFP_Destroy(m_pMFPObjs[iIndex]);
			m_pMFPObjs[iIndex] = 0;
		}
		m_pMFPObjs[iIndex] = _In_MFP_Create();
	}
}

void MFilePlayerDll::Init(int iIndex, HWND hWnd,FilePlayerCallback* pFilePlayerCallback)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_Init(m_pMFPObjs[iIndex], hWnd, pFilePlayerCallback);
	}

}

int MFilePlayerDll::Open(int iIndex, char* szFileName)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_Open(m_pMFPObjs[iIndex], szFileName);
	}
}

void MFilePlayerDll::Close(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_Close(m_pMFPObjs[iIndex]);
	}
}

void MFilePlayerDll::Play(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_Play(m_pMFPObjs[iIndex]);
	}
}

void MFilePlayerDll::Stop(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_Stop(m_pMFPObjs[iIndex]);
	}
}

void MFilePlayerDll::Stop2(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_Stop2(m_pMFPObjs[iIndex]);
	}
}

void MFilePlayerDll::SetRepeat(int iIndex, bool bEnable)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetRepeat(m_pMFPObjs[iIndex], bEnable);
	}
}

void MFilePlayerDll::EnableFadeIn(int iIndex, bool bEnable)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_EnableFadeIn(m_pMFPObjs[iIndex], bEnable);
	}
}

void MFilePlayerDll::EnableFadeOut(int iIndex, bool bEnable)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_EnableFadeOut(m_pMFPObjs[iIndex], bEnable);
	}
}

void MFilePlayerDll::SetOutputVideoFormat(int iIndex, int iFormat)
{
	if (iIndex < _MAX_MF_OBJS)

	if (m_pMFPObjs[iIndex])
		_In_MFP_SetOutputVideoFormat(m_pMFPObjs[iIndex], iFormat);
}

void MFilePlayerDll::Seek(int iIndex, double dPos)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_Seek(m_pMFPObjs[iIndex], dPos);
	}
}

void MFilePlayerDll::EnableAudio(int iIndex, bool bEnable)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_EnableAudio(m_pMFPObjs[iIndex], bEnable);
	}
}

float MFilePlayerDll::GetVideoFPS(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GetVideoFPS(m_pMFPObjs[iIndex]);
	}
	return 0.0f;
}

int MFilePlayerDll::GetTotalOfFrames(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GetTotalOfFrames(m_pMFPObjs[iIndex]);
	}
	return 0;
}

int MFilePlayerDll::GetVideoWidth(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GetVideoWidth(m_pMFPObjs[iIndex]);
	}
	return 0;
}

int MFilePlayerDll::GetVideoHeight(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GetVideoHeight(m_pMFPObjs[iIndex]);
	}
	return 0;
}

void MFilePlayerDll::SpeedUp(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_SpeedUp(m_pMFPObjs[iIndex]);
	}
}

void MFilePlayerDll::SpeedDown(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_SpeedDown(m_pMFPObjs[iIndex]);
	}
}

void MFilePlayerDll::SetVolume(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetVolume(m_pMFPObjs[iIndex], iValue);
	}
}

void MFilePlayerDll::SetSystemVolume(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetSystemVolume(m_pMFPObjs[iIndex], iValue);
	}
}

int MFilePlayerDll::GetVolume(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GetVolume(m_pMFPObjs[iIndex]);
	}
	return 0;
}

int MFilePlayerDll::GetSystemVolume(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GetSystemVolume(m_pMFPObjs[iIndex]);
	}
	return 0;
}

void MFilePlayerDll::SetBrightness(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetBrightness(m_pMFPObjs[iIndex], iValue);
	}
}

void MFilePlayerDll::SetContrast(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetContrast(m_pMFPObjs[iIndex], iValue);
	}
}

void MFilePlayerDll::SetSaturation(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetSaturation(m_pMFPObjs[iIndex], iValue);
	}
}

void MFilePlayerDll::SetHue(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetHue(m_pMFPObjs[iIndex], iValue);
	}
}

void MFilePlayerDll::SetR(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetR(m_pMFPObjs[iIndex], iValue);
	}
}

void MFilePlayerDll::SetG(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetG(m_pMFPObjs[iIndex], iValue);
	}
}

void MFilePlayerDll::SetB(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetB(m_pMFPObjs[iIndex], iValue);
	}
}

void MFilePlayerDll::PlayBackward(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_PlayBackward(m_pMFPObjs[iIndex]);
	}
}

int MFilePlayerDll::GetPlaybackMode(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GetPlaybackMode(m_pMFPObjs[iIndex]);
	}
	return 0;
}

void MFilePlayerDll::Pause(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_Pause(m_pMFPObjs[iIndex]);
	}
}

void MFilePlayerDll::SetFadeDuration(int iIndex, float fValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetFadeDuration(m_pMFPObjs[iIndex], fValue);
	}
}

void MFilePlayerDll::SetupDXVA2(int iIndex, void* pD3DDevice)
{
	//if (m_pMFPObj)
		//_In_MFP_SetupDXVA2(m_pMFPObj, pD3DDevice);

	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetupDXVA2(m_pMFPObjs[iIndex], pD3DDevice);
	}
}

void MFilePlayerDll::EnableGPU(int iIndex, bool bEnable)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_EnableGPU(m_pMFPObjs[iIndex], bEnable);
	}
}

bool MFilePlayerDll::GPUIsEnabled(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GPUIsEnabled(m_pMFPObjs[iIndex]);
	}
	return 0;
}

bool MFilePlayerDll::IsOpened(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_IsOpened(m_pMFPObjs[iIndex]);
	}
	return 0;
}

void MFilePlayerDll::SetScale(int iIndex,float fW, float fH)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetScale(m_pMFPObjs[iIndex], fW, fH);
	}
}

void MFilePlayerDll::SetGray(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetGray(m_pMFPObjs[iIndex], iValue);
	}
}

void MFilePlayerDll::SetNegative(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetNegative(m_pMFPObjs[iIndex], iValue);
	}
}

void MFilePlayerDll::ResetFilter(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_ResetFilter(m_pMFPObjs[iIndex]);
	}
}int MFilePlayerDll::GetAudioInfo(int iIndex,int& iChannels, int& iSampleRate, int& iBits)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GetAudioInfo(m_pMFPObjs[iIndex], iChannels,iSampleRate,iBits);
	}
	return 0;
}

void MFilePlayerDll::SetReferenceTimeCode(int iIndex, double dNewRefTime)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_SetRefTimeCode(m_pMFPObjs[iIndex], dNewRefTime);
	}

}
/*
void MFilePlayerDll::EnableGlobalMode(int iIndex, bool bEnable)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_EnableGlobalMode(m_pMFPObjs[iIndex], bEnable);
	}
}
*/
void MFilePlayerDll::UpdateGlobalTimeCode(int iIndex, double dTimeCode)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_UpdateGlobalTimeCode(m_pMFPObjs[iIndex], dTimeCode);
	}
}

void MFilePlayerDll::SetDisplayIntervalMode(int iIndex, int iMode)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetDisplayIntervalMode(m_pMFPObjs[iIndex], iMode);
	}
}

void MFilePlayerDll::SetStartTime(int iIndex, double dTime)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetStartTime(m_pMFPObjs[iIndex], dTime);
	}
}

void MFilePlayerDll::EnableReSampleRate(int iIndex, bool bEnable)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_EnableReSampleRate(m_pMFPObjs[iIndex], bEnable);
	}
}

void* MFilePlayerDll::GetD3DDevice(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GetD3DDevice(m_pMFPObjs[iIndex]);
	}
	return 0;
}

bool MFilePlayerDll::IsAudioFile(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_IsAudioFile(m_pMFPObjs[iIndex]);
	}
	return 0;
}

void MFilePlayerDll::EnableSuspendDisplay(int iIndex, bool bEnable)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_EnableSuspendDisplay(m_pMFPObjs[iIndex], bEnable);
	}
}

void MFilePlayerDll::SetAudioDelay(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetAudioDelay(m_pMFPObjs[iIndex], iValue);
	}
}

void MFilePlayerDll::SetReSampleRateOffset(int iIndex, float fValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetReSampleRateOffset(m_pMFPObjs[iIndex], fValue);
	}
}

void MFilePlayerDll::SetProcessFPS(int iIndex, float fFPS)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetProcessFPS(m_pMFPObjs[iIndex], fFPS);
	}
}

//_In_MFP_SetProcessFPS
