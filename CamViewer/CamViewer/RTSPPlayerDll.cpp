#include "stdafx.h"
#include "RTSPPlayerDll.h"

RTSPPlayerDll::RTSPPlayerDll()
{
	m_hInst = 0;
}

RTSPPlayerDll::~RTSPPlayerDll()
{
	FreeLib();
}

void RTSPPlayerDll::LoadLib()
{
	char szFileName[] = "RTSPPlayer.dll";
	m_hInst = LoadLibraryA(szFileName);

	if (m_hInst)
	{
		_In_RTSP_Create = (_In_RTSP_Create_Proc)GetProcAddress(m_hInst, "_In_RTSP_Create");
		_In_RTSP_Destroy = (_In_RTSP_Destroy_Proc)GetProcAddress(m_hInst, "_In_RTSP_Destroy");
		_In_RTSP_Init = (_In_RTSP_Init_Proc)GetProcAddress(m_hInst, "_In_RTSP_Init");
		_In_RTSP_Open = (_In_RTSP_Open_Proc)GetProcAddress(m_hInst, "_In_RTSP_Open");
		_In_RTSP_Close = (_In_RTSP_Close_Proc)GetProcAddress(m_hInst, "_In_RTSP_Close");
		_In_RTSP_SetOutputVideoFormat = (_In_RTSP_SetOutputVideoFormat_Proc)GetProcAddress(m_hInst, "_In_RTSP_SetOutputVideoFormat");
		_In_RTSP_EnableAudio = (_In_RTSP_EnableAudio_Proc)GetProcAddress(m_hInst, "_In_RTSP_EnableAudio");

		_In_RTSP_Play = (_In_RTSP_Play_Proc)GetProcAddress(m_hInst, "_In_RTSP_Play");
		_In_RTSP_Stop = (_In_RTSP_Stop_Proc)GetProcAddress(m_hInst, "_In_RTSP_Stop");
		_In_RTSP_GetVolume = (_In_RTSP_GetVolume_Proc)GetProcAddress(m_hInst, "_In_RTSP_GetVolume");
		_In_RTSP_SetVolume = (_In_RTSP_SetVolume_Proc)GetProcAddress(m_hInst, "_In_RTSP_SetVolume");
		_In_RTSP_IsOpened = (_In_RTSP_IsOpened_Proc)GetProcAddress(m_hInst, "_In_RTSP_IsOpened");
		_In_RTSP_GetFPS = (_In_RTSP_GetFPS_Proc)GetProcAddress(m_hInst, "_In_RTSP_GetFPS");
		_In_RTSP_GetVideoCodecID = (_In_RTSP_GetVideoCodecID_Proc)GetProcAddress(m_hInst, "_In_RTSP_GetVideoCodecID");
		_In_RTSP_GetAudioCodecID = (_In_RTSP_GetAudioCodecID_Proc)GetProcAddress(m_hInst, "_In_RTSP_GetAudioCodecID");
		_In_RTSP_GetVideoCodecName = (_In_RTSP_GetVideoCodecName_Proc)GetProcAddress(m_hInst, "_In_RTSP_GetVideoCodecName");
		_In_RTSP_GetAudioCodecName = (_In_RTSP_GetAudioCodecName_Proc)GetProcAddress(m_hInst, "_In_RTSP_GetAudioCodecName");
		_In_RTSP_GetVideoWidth = (_In_RTSP_GetVideoWidth_Proc)GetProcAddress(m_hInst, "_In_RTSP_GetVideoWidth");
		_In_RTSP_GetVideoHeight = (_In_RTSP_GetVideoHeight_Proc)GetProcAddress(m_hInst, "_In_RTSP_GetVideoHeight");
		_In_RTSP_GetFrameBuffer = (_In_RTSP_GetFrameBuffer_Proc)GetProcAddress(m_hInst, "_In_RTSP_GetFrameBuffer");

		_In_RTSP_CheckNetworkStatus = (_In_RTSP_CheckNetworkStatus_Proc)GetProcAddress(m_hInst, "_In_RTSP_CheckNetworkStatus");
	}
	else
		MessageBoxA(NULL, "Load RTSPPlayer.dll failed!!", "Error", MB_OK | MB_TOPMOST);
}

void RTSPPlayerDll::FreeLib()
{
	if (m_hInst)
		FreeLibrary(m_hInst);
}
