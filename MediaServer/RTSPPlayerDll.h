#pragma once

#ifndef _RTSPPlayerDll_H
#define _RTSPPlayerDll_H

#include "../../../Include/FilePlayerCommon.h"
#include "../../../Include/RtspCommon.h"

typedef void* (*_In_RTSP_Create_Proc)();
typedef void (*_In_RTSP_Destroy_Proc)(void* pRTSPCObj);
typedef void (*_In_RTSP_Init_Proc)(void* pRTSPCObj, FilePlayerCallback* pFilePlayerCallback);
typedef void (*_In_RTSP_Open_Proc)(void* pRTSPCObj, char* szUrl);
typedef void (*_In_RTSP_Close_Proc)(void* pRTSPCObj);
typedef void (*_In_RTSP_SetOutputVideoFormat_Proc)(void* pRTSPCObj, int iFormat);
typedef void (*_In_RTSP_EnableAudio_Proc)(void* pRTSPCObj, bool bEnable);
typedef void (*_In_RTSP_Play_Proc)(void* pRTSPCObj);
typedef void (*_In_RTSP_Stop_Proc)(void* pRTSPCObj);
typedef int (*_In_RTSP_GetVolume_Proc)(void* pRTSPCObj);
typedef void (*_In_RTSP_SetVolume_Proc)(void* pRTSPCObj, int iValue);
typedef bool (*_In_RTSP_IsOpened_Proc)(void* pRTSPCObj);
typedef float (*_In_RTSP_GetFPS_Proc)(void* pRTSPCObj);
typedef int (*_In_RTSP_GetVideoCodecID_Proc)(void* pRTSPCObj);
typedef int (*_In_RTSP_GetAudioCodecID_Proc)(void* pRTSPCObj);
typedef char* (*_In_RTSP_GetVideoCodecName_Proc)(void* pRTSPCObj);
typedef char* (*_In_RTSP_GetAudioCodecName_Proc)(void* pRTSPCObj);
typedef int (*_In_RTSP_GetVideoWidth_Proc)(void* pRTSPCObj);
typedef int (*_In_RTSP_GetVideoHeight_Proc)(void* pRTSPCObj);
typedef unsigned char* (*_In_RTSP_GetFrameBuffer_Proc)(void* pRTSPCObj);
typedef int (*_In_RTSP_CheckNetworkStatus_Proc)(void* pRTSPCObj, char* szUrl);

class RTSPPlayerDll
{
	HINSTANCE m_hInst;
	//void* m_pRTSPCObj;
public:
	RTSPPlayerDll();
	~RTSPPlayerDll();

	_In_RTSP_Create_Proc _In_RTSP_Create;
	_In_RTSP_Destroy_Proc _In_RTSP_Destroy;
	_In_RTSP_Init_Proc _In_RTSP_Init;
	_In_RTSP_Open_Proc _In_RTSP_Open;
	_In_RTSP_Close_Proc _In_RTSP_Close;
	_In_RTSP_SetOutputVideoFormat_Proc _In_RTSP_SetOutputVideoFormat;
	_In_RTSP_EnableAudio_Proc _In_RTSP_EnableAudio;
	_In_RTSP_Play_Proc _In_RTSP_Play;
	_In_RTSP_Stop_Proc _In_RTSP_Stop;
	_In_RTSP_GetVolume_Proc _In_RTSP_GetVolume;
	_In_RTSP_SetVolume_Proc _In_RTSP_SetVolume;
	_In_RTSP_IsOpened_Proc _In_RTSP_IsOpened;
	_In_RTSP_GetFPS_Proc _In_RTSP_GetFPS;
	_In_RTSP_GetVideoCodecID_Proc _In_RTSP_GetVideoCodecID;
	_In_RTSP_GetAudioCodecID_Proc _In_RTSP_GetAudioCodecID;
	_In_RTSP_GetVideoCodecName_Proc _In_RTSP_GetVideoCodecName;
	_In_RTSP_GetAudioCodecName_Proc _In_RTSP_GetAudioCodecName;
	_In_RTSP_GetVideoWidth_Proc _In_RTSP_GetVideoWidth;
	_In_RTSP_GetVideoHeight_Proc _In_RTSP_GetVideoHeight;
	_In_RTSP_GetFrameBuffer_Proc _In_RTSP_GetFrameBuffer;
	
	_In_RTSP_CheckNetworkStatus_Proc _In_RTSP_CheckNetworkStatus;

	void LoadLib();
	void FreeLib();
};

#endif
