#pragma once

#ifndef _OutXAudio2Dll_H
#define _OutXAudio2Dll_H

typedef void* (*_Out_OXA_Create_Proc)();
typedef void (*_Out_OXA_Destroy_Proc)(void* pOXAObj);
typedef void (*_Out_OXA_Init_Proc)(void* pOXAObj);
typedef void (*_Out_OXA_OpenDevice_Proc)(void* pOXAObj, int nChannels, int nSampleRate, int iBits, int iBufferLen, int iTotalOfBuffers);
typedef void (*_Out_OXA_Close_Proc)(void* pOXAObj);
typedef void (*_Out_OXA_Output_Proc)(void* pOXAObj, char* pBuffer, int iLen);
typedef void (*_Out_OXA_Stop_Proc)(void* pOXAObj);
typedef void (*_Out_OXA_Continue_Proc)(void* pOXAObj);
typedef void (*_Out_OXA_SetVolume_Proc)(void* pOXAObj, int iValue);
typedef bool (*_Out_OXA_IsOpened_Proc)(void* pOXAObj);
typedef void (*_Out_OXA_ResetBuffer_Proc)(void* pOXAObj);
typedef long (*_Out_OXA_GetVolume_Proc)(void* pOXAObj);
typedef bool (*_Out_OXA_IsFulled_Proc)(void* pOXAObj);
typedef int (*_Out_OXA_GetBufferCount_Proc)(void* pOXAObj);
typedef void (*_Out_OXA_Pause_Proc)(void* pOXAObj);


class OutXAudio2Dll
{
	HINSTANCE m_hInst;
	void* m_pOXAObj;
public:
	OutXAudio2Dll();
	~OutXAudio2Dll();

	_Out_OXA_Create_Proc _Out_OXA_Create;
	_Out_OXA_Destroy_Proc _Out_OXA_Destroy;
	_Out_OXA_Init_Proc _Out_OXA_Init;
	_Out_OXA_OpenDevice_Proc _Out_OXA_OpenDevice;
	_Out_OXA_Close_Proc _Out_OXA_Close;
	_Out_OXA_Output_Proc _Out_OXA_Output;
	_Out_OXA_Stop_Proc _Out_OXA_Stop;
	_Out_OXA_Continue_Proc _Out_OXA_Continue;
	_Out_OXA_SetVolume_Proc _Out_OXA_SetVolume;
	_Out_OXA_IsOpened_Proc _Out_OXA_IsOpened;
	_Out_OXA_ResetBuffer_Proc _Out_OXA_ResetBuffer;
	_Out_OXA_GetVolume_Proc _Out_OXA_GetVolume;
	_Out_OXA_IsFulled_Proc _Out_OXA_IsFulled;
	_Out_OXA_GetBufferCount_Proc _Out_OXA_GetBufferCount;
	_Out_OXA_Pause_Proc _Out_OXA_Pause;

	bool LoadLib();
	void FreeLib();

	void Init();
	void OpenDevice(int iCH, int iFrequency, int iBits, int iBufferLen, int iTotalOfBuffers);
	void Close();
	void Output(char* pBuffer, int iLen);
	void Stop();
	void Pause();
	void Continue();
	void SetVolume(int iValue);
	bool IsOpened();
	void ResetBuffer();
	long GetVolume();
	bool IsFulled();
	int GetBufferCount();
};

#endif