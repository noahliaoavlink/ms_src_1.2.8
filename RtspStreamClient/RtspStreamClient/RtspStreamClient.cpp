// RtspStreamClient.cpp : 定義 DLL 應用程式的進入點。
//

#include "stdafx.h"
#include "RtspStreamClient.h"

#include "LiveRTSPClient.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif
/*
// 這是匯出變數的範例
RTSPSTREAMCLIENT_API int nRtspStreamClient=0;

// 這是匯出函式的範例。
RTSPSTREAMCLIENT_API int fnRtspStreamClient(void)
{
	return 42;
}

// 這是已匯出的類別建構函式。
// 請參閱 RtspStreamClient.h 中的類別定義
CRtspStreamClient::CRtspStreamClient()
{
	return;
}
*/

extern "C" __declspec( dllexport )void* Create(RtspCallbackObj* pObj)
{
	LiveRTSPClient* pNewRTSPClient = new LiveRTSPClient;

	pNewRTSPClient->Init(pObj);
	return pNewRTSPClient;
}

extern "C" __declspec( dllexport )void Destroy(void* pObj)
{
	if(pObj)
	{
		delete ((LiveRTSPClient*)pObj);
		pObj = 0;
	}
}

extern "C" __declspec( dllexport )void Open(void* pObj,char* szUrl)
{
	if(pObj)
		((LiveRTSPClient*)pObj)->Open(szUrl);
}

extern "C" __declspec( dllexport )void Close(void* pObj)
{
	if(pObj)
		((LiveRTSPClient*)pObj)->Close();
}

extern "C" __declspec( dllexport )void SetUserInfo(void* pObj,char* szUserName,char* szPassword)
{
	if(pObj)
		((LiveRTSPClient*)pObj)->SetUserInfo(szUserName,szPassword);
}

extern "C" __declspec( dllexport )void EnableOverTCP(void* pObj,bool bEnable)
{
	if(pObj)
		((LiveRTSPClient*)pObj)->EnableOverTCP(bEnable);
}

extern "C" __declspec( dllexport )void EnableAudio(void* pObj,bool bEnable)
{
	if(pObj)
		((LiveRTSPClient*)pObj)->EnableAudio(bEnable);
}

extern "C" __declspec( dllexport )void EnableReconnect(void* pObj,bool bEnable)
{
	if(pObj)
		((LiveRTSPClient*)pObj)->EnableReconnect(bEnable);
}

extern "C" __declspec(dllexport)int GetVideoCodecID(void* pObj)
{
	if (pObj)
		return ((LiveRTSPClient*)pObj)->GetVideoCodecID();
}

extern "C" __declspec(dllexport)int GetAudioCodecID(void* pObj)
{
	if (pObj)
		return ((LiveRTSPClient*)pObj)->GetAudioCodecID();
}

extern "C" __declspec(dllexport)int CheckNetworkStatus(void* pObj, char* szUrl)
{
	if (pObj)
		return ((LiveRTSPClient*)pObj)->CheckNetworkStatus(szUrl);
}
