#ifndef _LiveRTSPClient_H
#define _LiveRTSPClient_H

#include "Winsock2.h"

#if 0

#include "../../../Lib/liveMedia131011/include/liveMedia.hh"
#include "../../../Lib/liveMedia131011/include/BasicUsageEnvironment.hh"

#else

//#include "../../../Lib/live20180228/include/liveMedia.hh"
//#include "../../../Lib/live20180228/include/BasicUsageEnvironment.hh"

#include "../../../Lib/liveMedia20151012/include/liveMedia.hh"
#include "../../../Lib/liveMedia20151012/include/BasicUsageEnvironment.hh"

#endif

#include "ourRTSPClient.h"
#include "DummySink.h"
#include "CallbackObj.h"
#include "NetTester.h"

class LiveRTSPClient : public CallbackObj
{
		bool m_bEnableOverTCP;
		bool m_bEnableOverHTTP;
		int m_iHpptPort;
		bool m_bEnableAudio;
		int m_iChannel;
		RTSPClient* m_pRTSPClient;
		char m_szUserName[64];
		char m_szPassword[64];
		unsigned long m_ulID;

		char m_szUrl[512];

		int m_iReconnect;
		bool m_bResetCount;
		unsigned long m_dwCurCount;
		bool m_bEnableReconnect;

		bool m_bOpened;

		NetTester* m_pNetTester;

		HANDLE m_hThread;
		char eventLoopWatchVariable;

		char m_szVideoCodec[128];
		char m_szAudioCodec[128];
		int m_iVideoCodecID;
		int m_iAudioCodecID;

		RtspCallbackObj* m_pRtspCallbackObj;
	public:
		LiveRTSPClient();
       ~LiveRTSPClient();	 

	   void Init(RtspCallbackObj* pObj);
	   void Open(char* szUrl);
	   int CheckNetworkStatus(char* szUrl);
	   void Close();
	   void DoEventLoop();

	   void SetID(unsigned long ulID);
	   void SetUserInfo(char* szUserName,char* szPassword);
	   void EnableOverTCP(bool bEnable);
	   void SetHttpPort(int iPort);
	   void EnableOverHttp(bool bEnable);
	   void EnableAudio(bool bEnable);
	   void SetChannel(int iValue);

	   void Reconnect();
	   void UpdateCount();
	   void EnableReconnect(bool bEnable);

	   int GetVideoCodecID();
	   int GetAudioCodecID();

	   bool WaitForResult(bool* pValue, int iWaitTime);

	   void TimeEvt();

	   void ProcessFrameData(unsigned long ulID,MediaFrameInfo* pInfo);
	   void* DoEvent2(int iEventID,void* pParameter1,void* pParameter2);
	   void CaptureFrame(int iPayload,unsigned char* pBuffer,int iLen,int iIPCount);
	
	   void Test();

};

#endif
