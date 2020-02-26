#include "stdafx.h"
#include "../Include/OutXAudio2Dll.h"

OutXAudio2Dll::OutXAudio2Dll()
{
	m_hInst = 0;
	m_pOXAObj = 0;
}

OutXAudio2Dll::~OutXAudio2Dll()
{
	if (m_pOXAObj)
		_Out_OXA_Destroy(m_pOXAObj);

	FreeLib();
}

bool OutXAudio2Dll::LoadLib()
{
	char szFileName[] = "OutXAudio2.dll";
	m_hInst = LoadLibraryA(szFileName);

	if (m_hInst)
	{
		_Out_OXA_Create = (_Out_OXA_Create_Proc)GetProcAddress(m_hInst, "_Out_OXA_Create");
		_Out_OXA_Destroy = (_Out_OXA_Destroy_Proc)GetProcAddress(m_hInst, "_Out_OXA_Destroy");
		_Out_OXA_Init = (_Out_OXA_Init_Proc)GetProcAddress(m_hInst, "_Out_OXA_Init");
		_Out_OXA_OpenDevice = (_Out_OXA_OpenDevice_Proc)GetProcAddress(m_hInst, "_Out_OXA_OpenDevice");
		_Out_OXA_Close = (_Out_OXA_Close_Proc)GetProcAddress(m_hInst, "_Out_OXA_Close");
		_Out_OXA_Output = (_Out_OXA_Output_Proc)GetProcAddress(m_hInst, "_Out_OXA_Output");
		_Out_OXA_Stop = (_Out_OXA_Stop_Proc)GetProcAddress(m_hInst, "_Out_OXA_Stop");
		_Out_OXA_Continue = (_Out_OXA_Continue_Proc)GetProcAddress(m_hInst, "_Out_OXA_Continue");
		_Out_OXA_SetVolume = (_Out_OXA_SetVolume_Proc)GetProcAddress(m_hInst, "_Out_OXA_SetVolume");
		_Out_OXA_IsOpened = (_Out_OXA_IsOpened_Proc)GetProcAddress(m_hInst, "_Out_OXA_IsOpened");
		_Out_OXA_ResetBuffer = (_Out_OXA_ResetBuffer_Proc)GetProcAddress(m_hInst, "_Out_OXA_ResetBuffer");
		_Out_OXA_GetVolume = (_Out_OXA_GetVolume_Proc)GetProcAddress(m_hInst, "_Out_OXA_GetVolume");
		_Out_OXA_IsFulled = (_Out_OXA_IsFulled_Proc)GetProcAddress(m_hInst, "_Out_OXA_IsFulled");
		_Out_OXA_GetBufferCount = (_Out_OXA_GetBufferCount_Proc)GetProcAddress(m_hInst, "_Out_OXA_GetBufferCount");
		_Out_OXA_Pause = (_Out_OXA_Pause_Proc)GetProcAddress(m_hInst, "_Out_OXA_Pause");

		m_pOXAObj = _Out_OXA_Create();

		return true;
	}

	return false;
}

void OutXAudio2Dll::FreeLib()
{
	if (m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = 0;
	}
}

void OutXAudio2Dll::Init()
{
	if (m_pOXAObj)
		_Out_OXA_Init(m_pOXAObj);
}

void OutXAudio2Dll::OpenDevice(int iCH, int iFrequency, int iBits, int iBufferLen, int iTotalOfBuffers)
{
	if (m_pOXAObj)
		_Out_OXA_OpenDevice(m_pOXAObj, iCH, iFrequency,iBits,iBufferLen, iTotalOfBuffers);
}

void OutXAudio2Dll::Close()
{
	if (m_pOXAObj)
	{
		if (IsOpened())
		{
			//Stop();
			_Out_OXA_Close(m_pOXAObj);
		}
	}
}

void OutXAudio2Dll::Pause()
{
	if (m_pOXAObj)
	{
		_Out_OXA_Pause(m_pOXAObj);
	}
}

void OutXAudio2Dll::Output(char* pBuffer, int iLen)
{
	if (m_pOXAObj)
	{
		//if (IsReady())
		if (IsOpened())
			_Out_OXA_Output(m_pOXAObj, pBuffer, iLen);
	}
}

void OutXAudio2Dll::Stop()
{
	if (m_pOXAObj)
		_Out_OXA_Stop(m_pOXAObj);
}

void OutXAudio2Dll::Continue()
{
	if (m_pOXAObj)
		_Out_OXA_Continue(m_pOXAObj);
}

void OutXAudio2Dll::SetVolume(int iValue)
{
	//DSound volume
	if (m_pOXAObj)
		_Out_OXA_SetVolume(m_pOXAObj, iValue);
}

bool OutXAudio2Dll::IsOpened()
{
	if (m_pOXAObj)
	{
		return _Out_OXA_IsOpened(m_pOXAObj);
	}
	return false;
}

void OutXAudio2Dll::ResetBuffer()
{
	if (m_pOXAObj)
		_Out_OXA_ResetBuffer(m_pOXAObj);
}

long OutXAudio2Dll::GetVolume()
{
	//DSound volume
	if (m_pOXAObj)
		return _Out_OXA_GetVolume(m_pOXAObj);
	return 0;
}

bool OutXAudio2Dll::IsFulled()
{
	if (m_pOXAObj)
		return _Out_OXA_IsFulled(m_pOXAObj);
	return false;
}

int OutXAudio2Dll::GetBufferCount()
{
	if (m_pOXAObj)
		return _Out_OXA_GetBufferCount(m_pOXAObj);
	return 0;
}