#pragma once

#ifndef _MFRecorderDll_H_
#define _MFRecorderDll_H_

typedef void* (*_Out_MFR_Create_Proc)();
typedef void (*_Out_MFR_Destroy_Proc)(void* pMFRObj);
typedef void (*_Out_MFR_Init_Proc)(void* pMFRObj);
typedef int (*_Out_MFR_Open_Proc)(void* pMFRObj, char* szFileName);
typedef void (*_Out_MFR_Close_Proc)(void* pMFRObj);
typedef void (*_Out_MFR_SetVideoInfo_Proc)(void* pMFRObj, int iWidth, int iHeight, int iBitRate, float fFps);
typedef void (*_Out_MFR_SetAudioInfo_Proc)(void* pMFRObj, int iChannels, int iSamples, int iBitRate);
typedef void (*_Out_MFR_WriteHeader_Proc)(void* pMFRObj);
typedef int (*_Out_MFR_WriteVideoFrame_Proc)(void* pMFRObj, unsigned char* pBuffer, int iLen);
typedef int (*_Out_MFR_WriteAudioFrame_Proc)(void* pMFRObj, unsigned char* pBuffer, int iLen);
typedef void(*_Out_MFR_SetOriginalAudioInfo_Proc)(void* pMFRObj, int iChannelLayout, int iSampleRate, int iNBSamples);

class MFRecorderDll
{
	HINSTANCE m_hInst;
	void* m_pMFRObj;
public:
	MFRecorderDll();
	~MFRecorderDll();

	_Out_MFR_Create_Proc _Out_MFR_Create;
	_Out_MFR_Destroy_Proc _Out_MFR_Destroy;
	_Out_MFR_Init_Proc _Out_MFR_Init;
	_Out_MFR_Open_Proc _Out_MFR_Open;
	_Out_MFR_Close_Proc _Out_MFR_Close;
	_Out_MFR_SetVideoInfo_Proc _Out_MFR_SetVideoInfo;
	_Out_MFR_SetAudioInfo_Proc _Out_MFR_SetAudioInfo;
	_Out_MFR_WriteHeader_Proc _Out_MFR_WriteHeader;
	_Out_MFR_WriteVideoFrame_Proc _Out_MFR_WriteVideoFrame;
	_Out_MFR_WriteAudioFrame_Proc _Out_MFR_WriteAudioFrame;
	_Out_MFR_SetOriginalAudioInfo_Proc _Out_MFR_SetOriginalAudioInfo;

	bool LoadLib();
	void FreeLib();

	void Init();
	int Open(char* szFileName);
	void Close();
	void SetVideoInfo(int iWidth, int iHeight, int iBitRate, float fFps);
	void SetAudioInfo(int iChannels, int iSamples, int iBitRate);
	void WriteHeader();
	int WriteVideoFrame(unsigned char* pBuffer, int iLen);
	int WriteAudioFrame(unsigned char* pBuffer, int iLen);
	void SetOriginalAudioInfo(int iChannelLayout, int iSampleRate, int iNBSamples);
};

#endif