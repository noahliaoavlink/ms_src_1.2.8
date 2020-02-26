#pragma once

#include "../Include/FilePlayerCommon.h"

#define _MAX_MF_OBJS 3

typedef void* (*_In_MFP_Create_Proc)();
typedef void(*_In_MFP_Destroy_Proc)(void* pMFPObj);
typedef void(*_In_MFP_Init_Proc)(void* pMFPObj, HWND hWnd, FilePlayerCallback* pFilePlayerCallback);
typedef int(*_In_MFP_Open_Proc)(void* pMFPObj, char* szFileName);
typedef void(*_In_MFP_Close_Proc)(void* pMFPObj);
typedef void(*_In_MFP_Play_Proc)(void* pMFPObj);
typedef void(*_In_MFP_Stop_Proc)(void* pMFPObj);
typedef void(*_In_MFP_SetRepeat_Proc)(void* pMFPObj, bool bEnable);
typedef void(*_In_MFP_EnableFadeIn_Proc)(void* pMFPObj, bool bEnable);
typedef void(*_In_MFP_EnableFadeOut_Proc)(void* pMFPObj, bool bEnable);
typedef void(*_In_MFP_SetOutputVideoFormat_Proc)(void* pMFPObj, int iFormat);
typedef void(*_In_MFP_Seek_Proc)(void* pMFPObj, double dPos);
typedef void(*_In_MFP_EnableAudio_Proc)(void* pMFPObj, bool bEnable);
typedef float(*_In_MFP_GetVideoFPS_Proc)(void* pMFPObj);
typedef int(*_In_MFP_GetTotalOfFrames_Proc)(void* pMFPObj);
//typedef int(*_In_MFP_GetVideoFrame_Proc)(void* pMFPObj, unsigned char* pBuffer, int* w, int* h, double* pos, char* szTime);
typedef int(*_In_MFP_GetVideoWidth_Proc)(void* pMFPObj);
typedef int(*_In_MFP_GetVideoHeight_Proc)(void* pMFPObj);
typedef void(*_In_MFP_SpeedUp_Proc)(void* pMFPObj);
typedef void(*_In_MFP_SpeedDown_Proc)(void* pMFPObj);
typedef void(*_In_MFP_SetVolume_Proc)(void* pMFPObj, int iValue);
typedef int(*_In_MFP_GetVolume_Proc)(void* pMFPObj);
typedef void(*_In_MFP_SetBrightness_Proc)(void* pMFPObj, int iValue);
typedef void(*_In_MFP_SetContrast_Proc)(void* pMFPObj, int iValue);
typedef void(*_In_MFP_SetSaturation_Proc)(void* pMFPObj, int iValue);

class MixerPlayerDll
{
	HINSTANCE m_hInst;
	void* m_pMFPObjs[_MAX_MF_OBJS];

protected:

public:
	MixerPlayerDll();
	~MixerPlayerDll();

	bool LoadLib();
	void FreeLib();

	void Create(int iIndex);
	void Init(int iIndex, HWND hWnd, FilePlayerCallback* pFilePlayerCallback);
	int Open(int iIndex, char* szFileName);
	void Close(int iIndex);
	void Play(int iIndex);
	void Stop(int iIndex);
	void SetRepeat(int iIndex, bool bEnable);
	void EnableFadeIn(int iIndex, bool bEnable);
	void EnableFadeOut(int iIndex, bool bEnable);
	void SetOutputVideoFormat(int iIndex, int iFormat);
	void Seek(int iIndex, double dPos);
	void EnableAudio(int iIndex, bool bEnable);
	float GetVideoFPS(int iIndex);
	int GetTotalOfFrames(int iIndex);
	//int GetVideoFrame(int iIndex, unsigned char* pBuffer, int* w, int* h, double* pos, char* szTime);
	int GetVideoWidth(int iIndex);
	int GetVideoHeight(int iIndex);
	void SpeedUp(int iIndex);
	void SpeedDown(int iIndex);
	//void PlayBackward(int iIndex);
	void SetVolume(int iIndex, int iValue);
	int GetVolume(int iIndex);
	void SetBrightness(int iIndex, int iValue);
	void SetContrast(int iIndex, int iValue);
	void SetSaturation(int iIndex, int iValue);

	_In_MFP_Create_Proc _In_MFP_Create;
	_In_MFP_Destroy_Proc _In_MFP_Destroy;
	_In_MFP_Init_Proc _In_MFP_Init;
	_In_MFP_Open_Proc _In_MFP_Open;
	_In_MFP_Close_Proc _In_MFP_Close;
	_In_MFP_Play_Proc _In_MFP_Play;
	_In_MFP_Stop_Proc _In_MFP_Stop;
	_In_MFP_SetRepeat_Proc _In_MFP_SetRepeat;
	_In_MFP_EnableFadeIn_Proc _In_MFP_EnableFadeIn;
	_In_MFP_EnableFadeOut_Proc _In_MFP_EnableFadeOut;
	_In_MFP_SetOutputVideoFormat_Proc _In_MFP_SetOutputVideoFormat;
	_In_MFP_Seek_Proc _In_MFP_Seek;
	_In_MFP_EnableAudio_Proc _In_MFP_EnableAudio;
	_In_MFP_GetVideoFPS_Proc _In_MFP_GetVideoFPS;
	_In_MFP_GetTotalOfFrames_Proc _In_MFP_GetTotalOfFrames;
	//_In_MFP_GetVideoFrame_Proc _In_MFP_GetVideoFrame;
	_In_MFP_GetVideoWidth_Proc _In_MFP_GetVideoWidth;
	_In_MFP_GetVideoHeight_Proc _In_MFP_GetVideoHeight;
	_In_MFP_SpeedUp_Proc _In_MFP_SpeedUp;
	_In_MFP_SpeedDown_Proc _In_MFP_SpeedDown;
	_In_MFP_SetVolume_Proc _In_MFP_SetVolume;
	_In_MFP_GetVolume_Proc _In_MFP_GetVolume;
	_In_MFP_SetBrightness_Proc _In_MFP_SetBrightness;
	_In_MFP_SetContrast_Proc _In_MFP_SetContrast;
	_In_MFP_SetSaturation_Proc _In_MFP_SetSaturation;
};