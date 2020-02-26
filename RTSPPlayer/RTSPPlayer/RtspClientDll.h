#ifndef _RtspClientDll_H
#define _RtspClientDll_H

#include "../../../Include/RtspCommon.h"

typedef void* (*Create_Rtsp_Proc)(RtspCallbackObj* pObj);
typedef void (*Destroy_Rtsp_Proc)(void* pObj);
typedef void (*Open_Proc)(void* pObj,char* szUrl);
typedef void (*Close_Proc)(void* pObj);
typedef void (*SetUserInfo_Proc)(void* pObj,char* szUserName,char* szPassword);
typedef void (*EnableOverTCP_Proc)(void* pObj,bool bEnable);
typedef void (*EnableAudio_Proc)(void* pObj,bool bEnable);
typedef void (*EnableReconnect_Proc)(void* pObj,bool bEnable);
typedef int (*GetVideoCodecID_Proc)(void* pObj);
typedef int (*GetAudioCodecID_Proc)(void* pObj);
typedef int (*CheckNetworkStatus_Proc)(void* pObj, char* szUrl);

class RtspClientDll
{
		HINSTANCE m_hInst;
	public:
		RtspClientDll();
       ~RtspClientDll();	

	   Create_Rtsp_Proc Create;
	   Destroy_Rtsp_Proc Destroy;
	   Open_Proc Open;
	   Close_Proc Close;
	   SetUserInfo_Proc SetUserInfo;
	   EnableOverTCP_Proc EnableOverTCP;
	   EnableAudio_Proc EnableAudio;
	   EnableReconnect_Proc EnableReconnect;
	   GetVideoCodecID_Proc GetVideoCodecID;
	   GetAudioCodecID_Proc GetAudioCodecID;
	   CheckNetworkStatus_Proc CheckNetworkStatus;

	   void LoadLib();
	   void FreeLib();
};

#endif

