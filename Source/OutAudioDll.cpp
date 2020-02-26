#include "stdafx.h"
#include "../Include/OutAudioDll.h"

OutAudioDll::OutAudioDll()
{
	m_hInst = 0;
	m_pOADObj = 0;
}

OutAudioDll::~OutAudioDll()
{
	if(m_pOADObj)
		_Out_OAD_Destroy(m_pOADObj);

	FreeLib();
}

bool OutAudioDll::LoadLib()
{
	char szFileName[] = "OutAudio.dll";
	m_hInst = LoadLibraryA(szFileName);
	
	if(m_hInst)
	{
		_Out_OAD_Create = (_Out_OAD_Create_Proc)GetProcAddress(m_hInst, "_Out_OAD_Create");
		_Out_OAD_Destroy = (_Out_OAD_Destroy_Proc)GetProcAddress(m_hInst, "_Out_OAD_Destroy");
		_Out_OAD_Init = (_Out_OAD_Init_Proc)GetProcAddress(m_hInst, "_Out_OAD_Init");
		_Out_OAD_Open = (_Out_OAD_Open_Proc)GetProcAddress(m_hInst, "_Out_OAD_Open");
		_Out_OAD_Close = (_Out_OAD_Close_Proc)GetProcAddress(m_hInst, "_Out_OAD_Close");
		_Out_OAD_Output = (_Out_OAD_Output_Proc)GetProcAddress(m_hInst, "_Out_OAD_Output");
		_Out_OAD_Stop = (_Out_OAD_Stop_Proc)GetProcAddress(m_hInst, "_Out_OAD_Stop");
		_Out_OAD_Continue = (_Out_OAD_Continue_Proc)GetProcAddress(m_hInst, "_Out_OAD_Continue");
		_Out_OAD_SetWaveFormat = (_Out_OAD_SetWaveFormat_Proc)GetProcAddress(m_hInst, "_Out_OAD_SetWaveFormat");
		_Out_OAD_SetBufferInfo = (_Out_OAD_SetBufferInfo_Proc)GetProcAddress(m_hInst, "_Out_OAD_SetBufferInfo");
		_Out_OAD_IsReady = (_Out_OAD_IsReady_Proc)GetProcAddress(m_hInst, "_Out_OAD_IsReady");
		_Out_OAD_SetVolume = (_Out_OAD_SetVolume_Proc)GetProcAddress(m_hInst, "_Out_OAD_SetVolume");
		_Out_OAD_IsOpened = (_Out_OAD_IsOpened_Proc)GetProcAddress(m_hInst, "_Out_OAD_IsOpened");
		_Out_OAD_ResetBuffer = (_Out_OAD_ResetBuffer_Proc)GetProcAddress(m_hInst, "_Out_OAD_ResetBuffer");
		_Out_OAD_GetVolume = (_Out_OAD_GetVolume_Proc)GetProcAddress(m_hInst, "_Out_OAD_GetVolume");

		_Out_OAD_IsFulled = (_Out_OAD_IsFulled_Proc)GetProcAddress(m_hInst, "_Out_OAD_IsFulled");
		_Out_OAD_GetBufferCount = (_Out_OAD_GetBufferCount_Proc)GetProcAddress(m_hInst, "_Out_OAD_GetBufferCount");

		_Out_OAD_AddTimestamp = (_Out_OAD_AddTimestamp_Proc)GetProcAddress(m_hInst, "_Out_OAD_AddTimestamp");
		_Out_OAD_GetLastTimestamp = (_Out_OAD_GetLastTimestamp_Proc)GetProcAddress(m_hInst, "_Out_OAD_GetLastTimestamp");

		m_pOADObj = _Out_OAD_Create();
		return true;
	}
	return false;
}

void OutAudioDll::FreeLib()
{
	if(m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = 0;
	}
}

void OutAudioDll::Init(HWND hWnd)
{
	if(m_pOADObj)
		_Out_OAD_Init(m_pOADObj,hWnd);
}

void OutAudioDll::Open()
{
	if(m_pOADObj)
		_Out_OAD_Open(m_pOADObj);
}

void OutAudioDll::Close()
{
	if(m_pOADObj)
	{
		if(IsOpened())
		{
			Stop();
			_Out_OAD_Close(m_pOADObj);
		}
	}
}

void OutAudioDll::Output(char* pBuffer,int iLen)
{
	if(m_pOADObj)
	{
		if(IsReady())
			_Out_OAD_Output(m_pOADObj,pBuffer,iLen);
	}
}

void OutAudioDll::Stop()
{
	if(m_pOADObj)
		_Out_OAD_Stop(m_pOADObj);
}

void OutAudioDll::Continue()
{
	if(m_pOADObj)
		_Out_OAD_Continue(m_pOADObj);
}

void OutAudioDll::SetWaveFormat(int nChannels,int nFrequency,int nBits)
{
	if(m_pOADObj)
		_Out_OAD_SetWaveFormat(m_pOADObj,nChannels,nFrequency,nBits);
}

void OutAudioDll::SetBufferInfo(int iBufferSize,int iTotalOfBuffers)
{
	if(m_pOADObj)
		_Out_OAD_SetBufferInfo(m_pOADObj,iBufferSize,iTotalOfBuffers);
}

bool OutAudioDll::IsReady()
{
	if(m_pOADObj)
		return _Out_OAD_IsReady(m_pOADObj);
	return false;
}

void OutAudioDll::SetVolume(int iValue)
{
	//DSound volume
	if(m_pOADObj)
		_Out_OAD_SetVolume(m_pOADObj,iValue);
}

bool OutAudioDll::IsOpened()
{
	if(m_pOADObj)
	{
		if(IsReady())
			return _Out_OAD_IsOpened(m_pOADObj);
	}
	return false;
}

void OutAudioDll::OpenDevice(int iCH,int iFrequency,int iBits,int iBufferLen,int iTotalOfBuffers)
{
	SetWaveFormat(iCH,iFrequency,iBits);
	SetBufferInfo(iBufferLen,iTotalOfBuffers);
	Open();

	SetVolume(100);
}

void OutAudioDll::ResetBuffer()
{
	if (m_pOADObj)
		_Out_OAD_ResetBuffer(m_pOADObj);
}

long OutAudioDll::GetVolume()
{
	//DSound volume
	if (m_pOADObj)
		return _Out_OAD_GetVolume(m_pOADObj);
	return 0;
}

bool OutAudioDll::IsFulled()
{
	if (m_pOADObj)
		return _Out_OAD_IsFulled(m_pOADObj);
	return false;
}

int OutAudioDll::GetBufferCount()
{
	if (m_pOADObj)
		return _Out_OAD_GetBufferCount(m_pOADObj);
	return 0;
}

void OutAudioDll::AddTimestamp(unsigned long ulTimestamp)
{
	if (m_pOADObj)
		_Out_OAD_AddTimestamp(m_pOADObj,ulTimestamp);
}

unsigned long OutAudioDll::GetLastTimestamp()
{
	if (m_pOADObj)
		return _Out_OAD_GetLastTimestamp(m_pOADObj);
	return 0;
}
