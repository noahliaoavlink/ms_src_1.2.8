#include "stdafx.h"
#include "MixerPlayerDll.h"

MixerPlayerDll::MixerPlayerDll()
{
	m_hInst = 0;

	for (int i = 0; i < _MAX_MF_OBJS; i++)
		m_pMFPObjs[i] = 0;
}

MixerPlayerDll::~MixerPlayerDll()
{
	for (int i = 0; i < _MAX_MF_OBJS; i++)
	{
		if (m_pMFPObjs[i])
		{
			_In_MFP_Destroy(m_pMFPObjs[i]);
			m_pMFPObjs[i] = 0;
		}
	}
	FreeLib();
}

bool MixerPlayerDll::LoadLib()
{
	char szFileName[] = "MFilePlayer.dll";
	m_hInst = LoadLibraryA(szFileName);

	if (m_hInst)
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
		//_In_MFP_GetVideoFrame = (_In_MFP_GetVideoFrame_Proc)GetProcAddress(m_hInst, "_In_MFP_GetVideoFrame");
		_In_MFP_GetVideoWidth = (_In_MFP_GetVideoWidth_Proc)GetProcAddress(m_hInst, "_In_MFP_GetVideoWidth");
		_In_MFP_GetVideoHeight = (_In_MFP_GetVideoHeight_Proc)GetProcAddress(m_hInst, "_In_MFP_GetVideoHeight");
		_In_MFP_SpeedUp = (_In_MFP_SpeedUp_Proc)GetProcAddress(m_hInst, "_In_MFP_SpeedUp");
		_In_MFP_SpeedDown = (_In_MFP_SpeedDown_Proc)GetProcAddress(m_hInst, "_In_MFP_SpeedDown");
		//_In_MFP_PlayBackward = (_In_MFP_PlayBackward_Proc)GetProcAddress(m_hInst, "_In_MFP_PlayBackward");

		_In_MFP_SetVolume = (_In_MFP_SetVolume_Proc)GetProcAddress(m_hInst, "_In_MFP_SetVolume");
		_In_MFP_GetVolume = (_In_MFP_GetVolume_Proc)GetProcAddress(m_hInst, "_In_MFP_GetVolume");
		_In_MFP_SetBrightness = (_In_MFP_SetBrightness_Proc)GetProcAddress(m_hInst, "_In_MFP_SetBrightness");
		_In_MFP_SetContrast = (_In_MFP_SetContrast_Proc)GetProcAddress(m_hInst, "_In_MFP_SetContrast");
		_In_MFP_SetSaturation = (_In_MFP_SetSaturation_Proc)GetProcAddress(m_hInst, "_In_MFP_SetSaturation");

		return true;
	}
	return false;
}

void MixerPlayerDll::FreeLib()
{
	if (m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = 0;
	}
}

void MixerPlayerDll::Create(int iIndex)
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

void MixerPlayerDll::Init(int iIndex, HWND hWnd, FilePlayerCallback * pFilePlayerCallback)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_Init(m_pMFPObjs[iIndex], hWnd, pFilePlayerCallback);
	}
}

int MixerPlayerDll::Open(int iIndex, char * szFileName)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_Open(m_pMFPObjs[iIndex], szFileName);
	}
	return 0;
}

void MixerPlayerDll::Close(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_Close(m_pMFPObjs[iIndex]);
	}
}

void MixerPlayerDll::Play(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_Play(m_pMFPObjs[iIndex]);
	}
}

void MixerPlayerDll::Stop(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_Stop(m_pMFPObjs[iIndex]);
	}
}

void MixerPlayerDll::SetRepeat(int iIndex, bool bEnable)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetRepeat(m_pMFPObjs[iIndex], bEnable);
	}
}

void MixerPlayerDll::EnableFadeIn(int iIndex, bool bEnable)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_EnableFadeIn(m_pMFPObjs[iIndex], bEnable);
	}
}

void MixerPlayerDll::EnableFadeOut(int iIndex, bool bEnable)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_EnableFadeOut(m_pMFPObjs[iIndex], bEnable);
	}
}

void MixerPlayerDll::SetOutputVideoFormat(int iIndex, int iFormat)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetOutputVideoFormat(m_pMFPObjs[iIndex], iFormat);
	}
}

void MixerPlayerDll::Seek(int iIndex, double dPos)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_Seek(m_pMFPObjs[iIndex], dPos);
	}
}

void MixerPlayerDll::EnableAudio(int iIndex, bool bEnable)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_EnableAudio(m_pMFPObjs[iIndex], bEnable);
	}
}

float MixerPlayerDll::GetVideoFPS(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GetVideoFPS(m_pMFPObjs[iIndex]);
	}
	return 0.0f;
}

int MixerPlayerDll::GetTotalOfFrames(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GetTotalOfFrames(m_pMFPObjs[iIndex]);
	}
	return 0;
}

int MixerPlayerDll::GetVideoWidth(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GetVideoWidth(m_pMFPObjs[iIndex]);
	}
	return 0;
}

int MixerPlayerDll::GetVideoHeight(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GetVideoHeight(m_pMFPObjs[iIndex]);
	}
	return 0;
}

void MixerPlayerDll::SpeedUp(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_SpeedUp(m_pMFPObjs[iIndex]);
	}
}

void MixerPlayerDll::SpeedDown(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_SpeedDown(m_pMFPObjs[iIndex]);
	}
}

void MixerPlayerDll::SetVolume(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetVolume(m_pMFPObjs[iIndex], iValue);
	}
}

int MixerPlayerDll::GetVolume(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GetVolume(m_pMFPObjs[iIndex]);
	}
	return 0;
}

void MixerPlayerDll::SetBrightness(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetBrightness(m_pMFPObjs[iIndex], iValue);
	}
}

void MixerPlayerDll::SetContrast(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetContrast(m_pMFPObjs[iIndex], iValue);
	}
}

void MixerPlayerDll::SetSaturation(int iIndex, int iValue)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_SetSaturation(m_pMFPObjs[iIndex], iValue);
	}
}

/*
int MixerPlayerDll::GetVideoFrame(int iIndex, unsigned char* pBuffer, int* w, int* h, double* pos, char* szTime)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			return _In_MFP_GetVideoFrame(m_pMFPObjs[iIndex], pBuffer, w, h, pos, szTime);
	}
	return 0;
}

void MixerPlayerDll::PlayBackward(int iIndex)
{
	if (iIndex < _MAX_MF_OBJS)
	{
		if (m_pMFPObjs[iIndex])
			_In_MFP_PlayBackward(m_pMFPObjs[iIndex]);
	}
}
*/
