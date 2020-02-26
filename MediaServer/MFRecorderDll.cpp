#include "stdafx.h"
#include "MFRecorderDll.h"

MFRecorderDll::MFRecorderDll()
{
	m_hInst = 0;
	m_pMFRObj = 0;
}

MFRecorderDll::~MFRecorderDll()
{
	if (m_pMFRObj)
		_Out_MFR_Destroy(m_pMFRObj);
}

bool MFRecorderDll::LoadLib()
{
	char szFileName[] = "MFRecorder.dll";
	m_hInst = LoadLibraryA(szFileName);

	if (m_hInst)
	{
		_Out_MFR_Create = (_Out_MFR_Create_Proc)GetProcAddress(m_hInst, "_Out_MFR_Create");
		_Out_MFR_Destroy = (_Out_MFR_Destroy_Proc)GetProcAddress(m_hInst, "_Out_MFR_Destroy");
		_Out_MFR_Init = (_Out_MFR_Init_Proc)GetProcAddress(m_hInst, "_Out_MFR_Init");
		_Out_MFR_Open = (_Out_MFR_Open_Proc)GetProcAddress(m_hInst, "_Out_MFR_Open");
		_Out_MFR_Close = (_Out_MFR_Close_Proc)GetProcAddress(m_hInst, "_Out_MFR_Close");
		_Out_MFR_SetVideoInfo = (_Out_MFR_SetVideoInfo_Proc)GetProcAddress(m_hInst, "_Out_MFR_SetVideoInfo");
		_Out_MFR_SetAudioInfo = (_Out_MFR_SetAudioInfo_Proc)GetProcAddress(m_hInst, "_Out_MFR_SetAudioInfo");
		_Out_MFR_WriteHeader = (_Out_MFR_WriteHeader_Proc)GetProcAddress(m_hInst, "_Out_MFR_WriteHeader");
		_Out_MFR_WriteVideoFrame = (_Out_MFR_WriteVideoFrame_Proc)GetProcAddress(m_hInst, "_Out_MFR_WriteVideoFrame");
		_Out_MFR_WriteAudioFrame = (_Out_MFR_WriteAudioFrame_Proc)GetProcAddress(m_hInst, "_Out_MFR_WriteAudioFrame");

		_Out_MFR_SetOriginalAudioInfo = (_Out_MFR_SetOriginalAudioInfo_Proc)GetProcAddress(m_hInst, "_Out_MFR_SetOriginalAudioInfo");

		m_pMFRObj = _Out_MFR_Create();

		return true;
	}
	return false;
}

void MFRecorderDll::FreeLib()
{
	if (m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = 0;
	}
}

void MFRecorderDll::Init()
{
	if (m_pMFRObj)
		_Out_MFR_Init(m_pMFRObj);
}

int MFRecorderDll::Open(char* szFileName)
{
	if (m_pMFRObj)
		return _Out_MFR_Open(m_pMFRObj, szFileName);
	return 0;
}

void MFRecorderDll::Close()
{
	if (m_pMFRObj)
		_Out_MFR_Close(m_pMFRObj);
}

void MFRecorderDll::SetVideoInfo(int iWidth, int iHeight, int iBitRate, float fFps)
{
	if (m_pMFRObj)
		_Out_MFR_SetVideoInfo(m_pMFRObj, iWidth,iHeight,iBitRate,fFps);
}

void MFRecorderDll::SetAudioInfo(int iChannels, int iSamples, int iBitRate)
{
	if (m_pMFRObj)
		_Out_MFR_SetAudioInfo(m_pMFRObj, iChannels,iSamples,iBitRate);
}

void MFRecorderDll::WriteHeader()
{
	if (m_pMFRObj)
		_Out_MFR_WriteHeader(m_pMFRObj);
}

int MFRecorderDll::WriteVideoFrame(unsigned char* pBuffer, int iLen)
{
	if (m_pMFRObj)
		return _Out_MFR_WriteVideoFrame(m_pMFRObj, pBuffer,iLen);
	return 0;
}

int MFRecorderDll::WriteAudioFrame(unsigned char* pBuffer, int iLen)
{
	if (m_pMFRObj)
		return _Out_MFR_WriteAudioFrame(m_pMFRObj, pBuffer,iLen);
	return 0;
}

void MFRecorderDll::SetOriginalAudioInfo(int iChannelLayout, int iSampleRate, int iNBSamples)
{
	if (m_pMFRObj)
		_Out_MFR_SetOriginalAudioInfo(m_pMFRObj, iChannelLayout, iSampleRate, iNBSamples);
}