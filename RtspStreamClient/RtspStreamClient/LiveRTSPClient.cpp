#include "stdafx.h"
#include "LiveRTSPClient.h"
#include <process.h>
//#include "../../Include/PluginObjMsg.h"

#pragma comment(lib, "Ws2_32.lib")

#if 0

#ifdef _M_X64
#pragma comment(lib, "../../../Lib/liveMedia131011/lib_x64/libBasicUsageEnvironment.lib")
#pragma comment(lib, "../../../Lib/liveMedia131011/lib_x64/libgroupsock.lib")
#pragma comment(lib, "../../../Lib/liveMedia131011/lib_x64/libliveMedia.lib")
#pragma comment(lib, "../../../Lib/liveMedia131011/lib_x64/libUsageEnvironment.lib")  //liveMedia131011
#else
#pragma comment(lib, "../../../Lib/liveMedia131011/lib/libBasicUsageEnvironment.lib")
#pragma comment(lib, "../../../Lib/liveMedia131011/lib/libgroupsock.lib")
#pragma comment(lib, "../../../Lib/liveMedia131011/lib/libliveMedia.lib")
#pragma comment(lib, "../../../Lib/liveMedia131011/lib/libUsageEnvironment.lib")  //liveMedia131011
#endif

#else

#ifdef _M_X64
/*
#pragma comment(lib, "../../../Lib/live20180228/lib_x64/libBasicUsageEnvironment.lib")
#pragma comment(lib, "../../../Lib/live20180228/lib_x64/libgroupsock.lib")
#pragma comment(lib, "../../../Lib/live20180228/lib_x64/libliveMedia.lib")
#pragma comment(lib, "../../../Lib/live20180228/lib_x64/libUsageEnvironment.lib")  //liveMedia131011
*/

#pragma comment(lib, "lib_x64/libBasicUsageEnvironment.lib")
#pragma comment(lib, "lib_x64/libgroupsock.lib")
#pragma comment(lib, "lib_x64/libliveMedia.lib")
#pragma comment(lib, "lib_x64/libUsageEnvironment.lib")  //liveMedia131011

#else
#pragma comment(lib, "../../../Lib/live20180228/lib/libBasicUsageEnvironment.lib")
#pragma comment(lib, "../../../Lib/live20180228/lib/libgroupsock.lib")
#pragma comment(lib, "../../../Lib/live20180228/lib/libliveMedia.lib")
#pragma comment(lib, "../../../Lib/live20180228/lib/libUsageEnvironment.lib")  //liveMedia131011

/*
#pragma comment(lib, "../../../Lib/liveMedia20151012/lib/libBasicUsageEnvironment.lib")
#pragma comment(lib, "../../../Lib/liveMedia20151012/lib/libgroupsock.lib")
#pragma comment(lib, "../../../Lib/liveMedia20151012/lib/libliveMedia.lib")
#pragma comment(lib, "../../../Lib/liveMedia20151012/lib/libUsageEnvironment.lib")  //liveMedia131011
*/
#endif

#endif

#define VIDEO_BUF_SIZE 1024*1024*2//2000000
#define AUDIO_BUF_SIZE 4096

#define RTSP_CLIENT_VERBOSITY_LEVEL 0 // by default, print verbose output from each "RTSPClient"

typedef struct
{
	 char szRtspURL[512];
	 void* pObj;
	 bool bEnableOverTCP;
	 bool bEnableOverHTTP;
	 int iHttpPort;
	 char szUserName[64];
	 char szPassword[64];
	 bool bEnableAudio;
	 int nRet;
}RtspParam;

static unsigned rtspClientCount = 0; // Counts how many streams (i.e., "RTSPClient"s) are currently in use.
//#define REQUEST_STREAMING_OVER_TCP False

int g_iResultCode;
bool g_bOptionsRequestIsFinished;

unsigned int __stdcall EventLoopProc(void* lpvThreadParm);

void setupNextSubsession(RTSPClient* rtspClient);
void shutdownStream(RTSPClient* rtspClient, int exitCode = 1);
void stopStream(void* clientData);
void* openURL(UsageEnvironment& env, char const* progName,RtspParam* pRtspParam, bool sendOptionsRequestOnly);
void shutdownStream(void* clientData);
void streamTimerHandler(void* clientData);
unsigned getOptions(RTSPClient* rtspClient,RTSPClient::responseHandler* afterFunc,Authenticator* ourAuthenticator);
void getSDPDescription(RTSPClient* rtspClient,RTSPClient::responseHandler* afterFunc,Authenticator* ourAuthenticator);
void continueAfterOPTIONS(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterOPTIONS2(RTSPClient* rtspClient, int resultCode, char* resultString);

//extern MsgInfo* pOutMsg;

LiveRTSPClient::LiveRTSPClient()
{
	m_bEnableOverTCP = true;
	m_bEnableOverHTTP = false;
	m_iHpptPort = 0;
	m_bEnableAudio = true;
	m_iChannel = -1;
	m_pRTSPClient = 0;

	m_iReconnect = -1;
	m_dwCurCount = 0;

	strcpy(m_szUserName,"");
	strcpy(m_szPassword,"");

	m_bEnableReconnect = false;
	m_pRTSPClient = 0;
	m_bOpened = false;

	g_iResultCode = -1;
	g_bOptionsRequestIsFinished = false;

	m_pNetTester = new NetTester;

	m_iVideoCodecID = -1;
	m_iAudioCodecID = -1;
	strcpy(m_szVideoCodec, "");
	strcpy(m_szAudioCodec, "");

	m_pRtspCallbackObj = 0;
}

LiveRTSPClient::~LiveRTSPClient()
{
	delete m_pNetTester;
}

void LiveRTSPClient::Init(RtspCallbackObj* pObj)
{
	m_pRtspCallbackObj = pObj;
}

void LiveRTSPClient::Open(char* szUrl)
{
	g_bOptionsRequestIsFinished = false;

	strcpy(m_szUrl,szUrl);

	m_bResetCount = true;

	m_iVideoCodecID = -1;
	m_iAudioCodecID = -1;
	strcpy(m_szVideoCodec, "");
	strcpy(m_szAudioCodec, "");

	// Begin by setting up our usage environment:
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

	RtspParam rtsp_param;
	strcpy(rtsp_param.szRtspURL,szUrl);
	rtsp_param.pObj = this;
	rtsp_param.bEnableOverTCP = m_bEnableOverTCP;
	rtsp_param.bEnableOverHTTP = m_bEnableOverHTTP;
	rtsp_param.iHttpPort = m_iHpptPort;
	rtsp_param.bEnableAudio = m_bEnableAudio;
	strcpy(rtsp_param.szUserName,m_szUserName);
	strcpy(rtsp_param.szPassword,m_szPassword);

	m_pRTSPClient = (RTSPClient*)openURL(*env,"LIVE555",&rtsp_param,false);
/*
	if(((ourRTSPClient*)m_pRTSPClient)->scs.iResultCode == 0)
		m_bOpened = true;
	else
		m_bOpened = false;
*/
	m_iReconnect = -1;

	eventLoopWatchVariable = 0;
	unsigned threadID;
	m_hThread = (HANDLE)_beginthreadex( NULL, 0, EventLoopProc, this, 0, &threadID );

	WaitForResult(&g_bOptionsRequestIsFinished, 5000);

	if (g_iResultCode == 0)
		m_bOpened = true;
	else
		m_bOpened = false;
}

int LiveRTSPClient::CheckNetworkStatus(char* szUrl)
{
	g_bOptionsRequestIsFinished = false;

	strcpy(m_szUrl, szUrl);

	m_bResetCount = true;

	m_iVideoCodecID = -1;
	m_iAudioCodecID = -1;
	strcpy(m_szVideoCodec, "");
	strcpy(m_szAudioCodec, "");

	// Begin by setting up our usage environment:
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

	RtspParam rtsp_param;
	strcpy(rtsp_param.szRtspURL, szUrl);
	rtsp_param.pObj = this;
	rtsp_param.bEnableOverTCP = m_bEnableOverTCP;
	rtsp_param.bEnableOverHTTP = m_bEnableOverHTTP;
	rtsp_param.iHttpPort = m_iHpptPort;
	rtsp_param.bEnableAudio = m_bEnableAudio;
	strcpy(rtsp_param.szUserName, m_szUserName);
	strcpy(rtsp_param.szPassword, m_szPassword);

	m_pRTSPClient = (RTSPClient*)openURL(*env, "LIVE555", &rtsp_param, true);

	eventLoopWatchVariable = 0;
	unsigned threadID;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, EventLoopProc, this, 0, &threadID);

	WaitForResult(&g_bOptionsRequestIsFinished, 5000);

	if (g_iResultCode == 0)
		return 1;
	return -1;
}

void LiveRTSPClient::Close()
{
	eventLoopWatchVariable = 1;
	::WaitForSingleObject(m_hThread,1000);

	if(m_pRTSPClient)
	{
		/*
		if(((ourRTSPClient*)m_pRTSPClient)->scs.iResultCode == 0)
			m_bOpened = true;
		else
			m_bOpened = false;
			*/
		if(m_bOpened)
		{
			__try
			{
			stopStream(m_pRTSPClient);
			Sleep(100);
			shutdownStream(m_pRTSPClient);
			m_pRTSPClient = 0;
		}
			__except (EXCEPTION_EXECUTE_HANDLER/*pException = GetExceptionInformation()*/)
			{
				OutputDebugString("The LiveRTSPClient::Close() caused an exception!! \n");
				//return -100;
			}
		}
	}
}

void LiveRTSPClient::EnableReconnect(bool bEnable)
{
	m_bEnableReconnect = bEnable;
}

void LiveRTSPClient::DoEventLoop()
{
	UsageEnvironment& env = m_pRTSPClient->envir();
	env.taskScheduler().doEventLoop(&eventLoopWatchVariable);
}

void LiveRTSPClient::SetID(unsigned long ulID)
{
	m_ulID = ulID;
}

void LiveRTSPClient::SetUserInfo(char* szUserName,char* szPassword)
{
	strcpy(m_szUserName,szUserName);
	strcpy(m_szPassword,szPassword);
}

void LiveRTSPClient::EnableOverTCP(bool bEnable)
{
	m_bEnableOverTCP = bEnable;
}

void LiveRTSPClient::SetHttpPort(int iPort)
{
	m_iHpptPort = iPort;
}

void LiveRTSPClient::EnableOverHttp(bool bEnable)
{
	m_bEnableOverHTTP = bEnable;
}

void LiveRTSPClient::EnableAudio(bool bEnable)
{
	m_bEnableAudio = bEnable;
}

void LiveRTSPClient::SetChannel(int iValue)
{
	m_iChannel = iValue;
}

void LiveRTSPClient::Reconnect()
{
	char szMsg[512];
	if(m_iReconnect == 1)
	{
		//TRACE("Close [%d]\n",m_ulID);
		sprintf(szMsg,"Close [%d]\n",m_ulID);
		OutputDebugString(szMsg);
		//RTSPClient* rtspClient = (RTSPClient*)m_pOurClient;
		//rtspClient->pauseMediaSession(*m_pInterface->session);
		Close();
		m_iReconnect++;
		return ;
	}

	if(m_iReconnect > 1 && m_iReconnect < 1000)
		m_iReconnect++;

	if(m_iReconnect == 3)
	{
		m_bResetCount = true;
		m_iReconnect = -1;
		//TRACE("ReOpen.. [%d]\n",m_ulID);
		//m_pInterface->qosRecordHead = NULL;

		if(m_pNetTester->IsFinished())
		{
			if(m_pNetTester->Connect(m_szUrl))
			{
				sprintf(szMsg,"ReOpen.. [%d]\n",m_ulID);
				OutputDebugString(szMsg);

				Open(m_szUrl);
			}
			else
			{
				sprintf(szMsg,"m_pNetTester->Connect - false\n");
				OutputDebugString(szMsg);
			}
		}
		//startPlayingStreams(m_pInterface);
	}
}

void LiveRTSPClient::TimeEvt()
{
	if(m_bEnableReconnect)
	{
		Reconnect();
		UpdateCount();
	}
}

void LiveRTSPClient::UpdateCount()
{
	char szMsg[512];
	DWORD dwNowCount = GetTickCount();
	if(m_dwCurCount == 0 || m_bResetCount)
		m_dwCurCount = dwNowCount;
	else
	{
		if(dwNowCount > m_dwCurCount + 3000)
		{
			//TRACE("TimeOut [%d]\n",m_ulID);
			sprintf(szMsg,"TimeOut [%d]\n",m_ulID);
			OutputDebugString(szMsg);

			if(m_iReconnect == -1)
				m_iReconnect = 1;
		}
	}
	m_bResetCount = false;
}

int LiveRTSPClient::GetVideoCodecID()
{
	return m_iVideoCodecID;
}

int LiveRTSPClient::GetAudioCodecID()
{
	return m_iAudioCodecID;
}

bool LiveRTSPClient::WaitForResult(bool* pValue, int iWaitTime)
{
	DWORD dwStart = 0;
	DWORD dwEnd = 0;
	bool bDo = true;
	bool bRet = false;

	dwStart = GetTickCount();

	while (bDo)
	{
		dwEnd = GetTickCount();

		if (*pValue)
		{
			bRet = true;
			bDo = false;
		}
		else if (dwEnd - dwStart > iWaitTime)  //\B5\A5\ABݮɶ\A1
			bDo = false;
		MSG Msg;
		if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE | PM_NOYIELD))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		Sleep(1);
	}
	return bRet;
}

void LiveRTSPClient::ProcessFrameData(unsigned long ulID,MediaFrameInfo* pInfo)
{
	m_bResetCount = true;
	if(pInfo->iFormat >= RTSP_DF_MPEG_A)
	{
		RtspFrameInfo info;
		info.ulID = m_iChannel;//ulID;
		info.iFormat = pInfo->iFormat;
		info.pBuffer = pInfo->pBuffer;
		info.iLen = pInfo->iLen;
		info.iSample = pInfo->iSample;
		info.iBits = pInfo->iBits;
		info.iChannels = pInfo->iChannels;
		info.ulTimestampSec = pInfo->ulTimestampSec;
		info.ulTimestampMSec = pInfo->ulTimestampMSec;
		info.iFrameType = pInfo->iFrameType;

		//pOutMsg->SendMsg(RT_MSG_CAPTURE_AUDIO,&info,0);

		if(m_pRtspCallbackObj)
			m_pRtspCallbackObj->DoEvent(RCE_RECEIVE_AUDIO,(void*)&info,0);

//		char szMsg[512];
//		sprintf(szMsg,"ProcessFrameData(audio) %d %d %d %d\n",pInfo->iFormat,pInfo->iLen,pInfo->iSample,pInfo->iBits);
//		OutputDebugString(szMsg);
	}
	else if(pInfo->iFormat >= RTSP_DF_VIDEO_EXTRA_DATA)
	{
		RtspFrameInfo info;
		info.ulID = m_iChannel;//ulID;
		info.iFormat = pInfo->iFormat;
		info.pBuffer = pInfo->pBuffer;
		info.iLen = pInfo->iLen;
		info.iWidth = pInfo->iWidth;
		info.iHeight = pInfo->iHeight;
		info.iIPCount = pInfo->iIPCount;
		info.iPixelFormat = pInfo->iPixelFormat;
		info.ulTimestampSec = pInfo->ulTimestampSec;
		info.ulTimestampMSec = pInfo->ulTimestampMSec;
		info.iFrameType = pInfo->iFrameType;

		if(m_pRtspCallbackObj)
			m_pRtspCallbackObj->DoEvent(RCE_RECEIVE_VIDEO_EXTRA_DATA,(void*)&info,0);
	}
	else 
	{
		RtspFrameInfo info;
		info.ulID = m_iChannel;//ulID;
		info.iFormat = pInfo->iFormat;
		info.pBuffer = pInfo->pBuffer;
		info.iLen = pInfo->iLen;
		info.iWidth = pInfo->iWidth;
		info.iHeight = pInfo->iHeight;
		info.iIPCount = pInfo->iIPCount;
		info.iPixelFormat = pInfo->iPixelFormat;
		info.ulTimestampSec = pInfo->ulTimestampSec;
		info.ulTimestampMSec = pInfo->ulTimestampMSec;
		info.iFrameType = pInfo->iFrameType;

		//pOutMsg->SendMsg(RT_MSG_CAPTURE_VIDEO,&info,0);

		if(m_pRtspCallbackObj)
			m_pRtspCallbackObj->DoEvent(RCE_RECEIVE_VIDEO,(void*)&info,0);

//		char szMsg[512];
//		sprintf(szMsg,"ProcessFrameData(video) %d %d %d %d\n",pInfo->iWidth,pInfo->iHeight,pInfo->iFormat,pInfo->iLen);
//		OutputDebugString(szMsg);
	}
	UpdateCount();
}

void* LiveRTSPClient::DoEvent2(int iEventID,void* pParameter1,void* pParameter2)
{
	char szMsg[512];
	switch (iEventID)
	{
		case RE_VIDEO_CODEC:
			strcpy(m_szVideoCodec, (char*)pParameter1);
			m_iVideoCodecID = (int)pParameter2;
			sprintf(szMsg,"LiveRTSPClient::DoEvent2 RE_VIDEO_CODEC - %s %d\n", m_szVideoCodec, m_iVideoCodecID);
			OutputDebugString(szMsg);
			break;
		case RE_AUDIO_CODEC:
			strcpy(m_szAudioCodec, (char*)pParameter1);
			m_iAudioCodecID = (int)pParameter2;
			sprintf(szMsg, "LiveRTSPClient::DoEvent2 RE_AUDIO_CODEC - %s %d\n", m_szAudioCodec, m_iAudioCodecID);
			OutputDebugString(szMsg);
			break;
	}
	return 0;
}

void LiveRTSPClient::CaptureFrame(int iPayload,unsigned char* pBuffer,int iLen,int iIPCount)
{
}
/*
bool LiveRTSPClient::IsAlive()
{
	return m_bIsAlive;
}
*/
void LiveRTSPClient::Test()
{
	char rtspURL[] = "rtsp://128.197.178.101/mpeg4/media.amp";
	Open(rtspURL);
}

unsigned int __stdcall EventLoopProc(void* lpvThreadParm)
{
	LiveRTSPClient* pObj = (LiveRTSPClient *)lpvThreadParm;
	pObj->DoEventLoop();
	return 0;
}

void subsessionAfterPlaying(void* clientData) 
{
  MediaSubsession* subsession = (MediaSubsession*)clientData;
  RTSPClient* rtspClient = (RTSPClient*)(subsession->miscPtr);

  // Begin by closing this subsession's stream:
  Medium::close(subsession->sink);
  subsession->sink = NULL;

  // Next, check whether *all* subsessions' streams have now been closed:
  MediaSession& session = subsession->parentSession();
  MediaSubsessionIterator iter(session);
  while ((subsession = iter.next()) != NULL) {
    if (subsession->sink != NULL) return; // this subsession is still active
  }

  // All subsessions' streams have now been closed, so shutdown the client:
  shutdownStream(rtspClient);
}

void subsessionByeHandler(void* clientData) 
{
  MediaSubsession* subsession = (MediaSubsession*)clientData;
  RTSPClient* rtspClient = (RTSPClient*)subsession->miscPtr;
  UsageEnvironment& env = rtspClient->envir(); // alias

//  env << *rtspClient << "Received RTCP \"BYE\" on \"" << *subsession << "\" subsession\n";

  // Now act as if the subsession had closed:
  subsessionAfterPlaying(subsession);
}

void stopStream(void* clientData) 
{
	ourRTSPClient* rtspClient = (ourRTSPClient*)clientData;
	StreamClientState& scs = rtspClient->scs; // alias

	if(scs.subsession)
	{
		if(scs.subsession->sink)
			((DummySink*)scs.subsession->sink)->Stop();
	}
}

void shutdownStream(void* clientData) 
{
  ourRTSPClient* rtspClient = (ourRTSPClient*)clientData;
  StreamClientState& scs = rtspClient->scs; // alias

  scs.streamTimerTask = NULL;

  // Shut down the stream:
  shutdownStream(rtspClient);
}

void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString) 
{
  Boolean success = False;

  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias
	scs.iResultCode = resultCode;

    if (resultCode != 0) {
//      env << *rtspClient << "Failed to start playing session: " << resultString << "\n";
      break;
    }

    // Set a timer to be handled at the end of the stream's expected duration (if the stream does not already signal its end
    // using a RTCP "BYE").  This is optional.  If, instead, you want to keep the stream active - e.g., so you can later
    // 'seek' back within it and do another RTSP "PLAY" - then you can omit this code.
    // (Alternatively, if you don't want to receive the entire stream, you could set this timer for some shorter value.)
    if (scs.duration > 0) {
      unsigned const delaySlop = 2; // number of seconds extra to delay, after the stream's expected duration.  (This is optional.)
      scs.duration += delaySlop;
      unsigned uSecsToDelay = (unsigned)(scs.duration*1000000);
      scs.streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)streamTimerHandler, rtspClient);
    }

//    env << *rtspClient << "Started playing session";
    if (scs.duration > 0) {
      env << " (for up to " << scs.duration << " seconds)";
    }
    env << "...\n";

    success = True;
  } while (0);
  delete[] resultString;

  if (!success) {
    // An unrecoverable error occurred with this stream.
    shutdownStream(rtspClient);
  }
}

void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString) 
{
  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias
	scs.iResultCode = resultCode;
	g_iResultCode = resultCode;

    if (resultCode != 0) {
//      env << *rtspClient << "Failed to set up the \"" << *scs.subsession << "\" subsession: " << resultString << "\n";
      break;
    }

//    env << *rtspClient << "Set up the \"" << *scs.subsession
//	<< "\" subsession (client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1 << ")\n";

    // Having successfully setup the subsession, create a data sink for it, and call "startPlaying()" on it.
    // (This will prepare the data sink to receive data; the actual flow of data from the client won't start happening until later,
    // after we've sent a RTSP "PLAY" command.)

	if (strcmp(scs.subsession->mediumName(), "video") == 0)
		scs.subsession->sink = DummySink::createNew(env, *scs.subsession, rtspClient->url(),VIDEO_BUF_SIZE,scs.pCallbackObj);
	else if (strcmp(scs.subsession->mediumName(), "audio") == 0)
	{
		if(!scs.bEnableAudio)
			break;

		scs.subsession->sink = DummySink::createNew(env, *scs.subsession, rtspClient->url(),AUDIO_BUF_SIZE,scs.pCallbackObj);
	}
	else
		break;

      // perhaps use your own custom "MediaSink" subclass instead
    if (scs.subsession->sink == NULL) {
//      env << *rtspClient << "Failed to create a data sink for the \"" << *scs.subsession
//	  << "\" subsession: " << env.getResultMsg() << "\n";
      break;
    }

//    env << *rtspClient << "Created a data sink for the \"" << *scs.subsession << "\" subsession\n";
    scs.subsession->miscPtr = rtspClient; // a hack to let subsession handle functions get the "RTSPClient" from the subsession 
    scs.subsession->sink->startPlaying(*(scs.subsession->readSource()),
				       subsessionAfterPlaying, scs.subsession);
    // Also set a handler to be called if a RTCP "BYE" arrives for this subsession:
    if (scs.subsession->rtcpInstance() != NULL) {
      scs.subsession->rtcpInstance()->setByeHandler(subsessionByeHandler, scs.subsession);
    }
  } while (0);
  delete[] resultString;

  // Set up the next subsession, if any:
  setupNextSubsession(rtspClient);
}

void setupNextSubsession(RTSPClient* rtspClient) 
{
  UsageEnvironment& env = rtspClient->envir(); // alias
  StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias
  
  scs.subsession = scs.iter->next();
  if (scs.subsession != NULL) {
    if (!scs.subsession->initiate()) {
//      env << *rtspClient << "Failed to initiate the \"" << *scs.subsession << "\" subsession: " << env.getResultMsg() << "\n";
      setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
    } else {
//      env << *rtspClient << "Initiated the \"" << *scs.subsession
//	  << "\" subsession (client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1 << ")\n";

      // Continue setting up this subsession, by sending a RTSP "SETUP" command:
		
		bool bEnableOverTCP = false;
		if(scs.bEnableOverHTTP || scs.bEnableOverTCP)
			bEnableOverTCP = true;

		rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, False, bEnableOverTCP,False,scs.ourAuthenticator);
    }
    return;
  }

  // We've finished setting up all of the subsessions.  Now, send a RTSP "PLAY" command to start the streaming:
  if (scs.session->absStartTime() != NULL) {
    // Special case: The stream is indexed by 'absolute' time, so send an appropriate "PLAY" command:
    rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, scs.session->absStartTime(), scs.session->absEndTime(),1.0f,scs.ourAuthenticator);
  } else {
    scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
    rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY,0.0f, -1.0f,1.0f,scs.ourAuthenticator);
  }
}

void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString) 
{
  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias
	scs.iResultCode = resultCode;
	g_iResultCode = resultCode;

    if (resultCode != 0) {
//      env << *rtspClient << "Failed to get a SDP description: " << resultString << "\n";
      delete[] resultString;
      break;
    }

    char* const sdpDescription = resultString;
//    env << *rtspClient << "Got a SDP description:\n" << sdpDescription << "\n";

    // Create a media session object from this SDP description:
    scs.session = MediaSession::createNew(env, sdpDescription);
    delete[] sdpDescription; // because we don't need it anymore
    if (scs.session == NULL) {
//      env << *rtspClient << "Failed to create a MediaSession object from the SDP description: " << env.getResultMsg() << "\n";
      break;
    } else if (!scs.session->hasSubsessions()) {
//      env << *rtspClient << "This session has no media subsessions (i.e., no \"m=\" lines)\n";
      break;
    }

    // Then, create and set up our data source objects for the session.  We do this by iterating over the session's 'subsessions',
    // calling "MediaSubsession::initiate()", and then sending a RTSP "SETUP" command, on each one.
    // (Each 'subsession' will have its own data source.)
    scs.iter = new MediaSubsessionIterator(*scs.session);
    setupNextSubsession(rtspClient);

    return;
  } while (0);

  // An unrecoverable error occurred with this stream.
  shutdownStream(rtspClient);
}

void* openURL(UsageEnvironment& env, char const* progName,RtspParam* pRtspParam,bool sendOptionsRequestOnly)
{
  // Begin by creating a "RTSPClient" object.  Note that there is a separate "RTSPClient" object for each stream that we wish
  // to receive (even if more than stream uses the same "rtsp://" URL).
  int tunnelOverHTTPPortNum = 0;

  if(pRtspParam->bEnableOverHTTP)
	  tunnelOverHTTPPortNum = pRtspParam->iHttpPort;

  OutPacketBuffer::increaseMaxSizeTo(1024 * 1024 * 2);

  RTSPClient* rtspClient = ourRTSPClient::createNew(env, pRtspParam->szRtspURL, RTSP_CLIENT_VERBOSITY_LEVEL, progName,tunnelOverHTTPPortNum);

  ((ourRTSPClient*)rtspClient)->scs.pCallbackObj = (CallbackObj *)pRtspParam->pObj;
  ((ourRTSPClient*)rtspClient)->scs.bEnableOverTCP = pRtspParam->bEnableOverTCP;
  ((ourRTSPClient*)rtspClient)->scs.bEnableOverHTTP = pRtspParam->bEnableOverHTTP;
  ((ourRTSPClient*)rtspClient)->scs.iHttpPort = pRtspParam->iHttpPort;
  ((ourRTSPClient*)rtspClient)->scs.bEnableAudio = pRtspParam->bEnableAudio;

  //((ourRTSPClient*)rtspClient)->SetReceiveBufferTo(env, 1024 * 1024 * 2);

  if(strcmp(pRtspParam->szUserName,"") != 0 && strcmp(pRtspParam->szPassword,"") != 0)
	((ourRTSPClient*)rtspClient)->scs.ourAuthenticator = new Authenticator(pRtspParam->szUserName,pRtspParam->szPassword);

  //rtspClient = ourRTSPClient::createNew(env, rtspURL, RTSP_CLIENT_VERBOSITY_LEVEL, progName);
  if (rtspClient == NULL) {
    env << "Failed to create a RTSP client for URL \"" << pRtspParam->szRtspURL << "\": " << env.getResultMsg() << "\n";
    return 0;
  }

  ++rtspClientCount;

  // Next, send a RTSP "DESCRIBE" command, to get a SDP description for the stream.
  // Note that this command - like all RTSP commands - is sent asynchronously; we do not block, waiting for a response.
  // Instead, the following function call returns immediately, and we handle the RTSP response later, from within the event loop:

  if(sendOptionsRequestOnly)
	  pRtspParam->nRet = getOptions(rtspClient, continueAfterOPTIONS2, ((ourRTSPClient*)rtspClient)->scs.ourAuthenticator);
  else
	pRtspParam->nRet = getOptions(rtspClient,continueAfterOPTIONS,((ourRTSPClient*)rtspClient)->scs.ourAuthenticator);
  return rtspClient;
}

void shutdownStream(RTSPClient* rtspClient, int exitCode) 
{
  UsageEnvironment& env = rtspClient->envir(); // alias
  StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

  // First, check whether any subsessions have still to be closed:
  if (scs.session != NULL) { 
    Boolean someSubsessionsWereActive = False;
    MediaSubsessionIterator iter(*scs.session);
    MediaSubsession* subsession;

    while ((subsession = iter.next()) != NULL) {
      if (subsession->sink != NULL) {
	Medium::close(subsession->sink);
	subsession->sink = NULL;

	if (subsession->rtcpInstance() != NULL) {
	  subsession->rtcpInstance()->setByeHandler(NULL, NULL); // in case the server sends a RTCP "BYE" while handling "TEARDOWN"
	}

	someSubsessionsWereActive = True;
      }
    }

    if (someSubsessionsWereActive) {
      // Send a RTSP "TEARDOWN" command, to tell the server to shutdown the stream.
      // Don't bother handling the response to the "TEARDOWN".
      rtspClient->sendTeardownCommand(*scs.session, NULL,scs.ourAuthenticator);
    }
  }

//  env << *rtspClient << "Closing the stream.\n";
  Medium::close(rtspClient);
    // Note that this will also cause this stream's "StreamClientState" structure to get reclaimed.

  if (--rtspClientCount == 0) {
    // The final stream has ended, so exit the application now.
    // (Of course, if you're embedding this code into your own application, you might want to comment this out,
    // and replace it with "eventLoopWatchVariable = 1;", so that we leave the LIVE555 event loop, and continue running "main()".)
 //   exit(exitCode);
  }
}

void streamTimerHandler(void* clientData) 
{
  ourRTSPClient* rtspClient = (ourRTSPClient*)clientData;
  StreamClientState& scs = rtspClient->scs; // alias

  scs.streamTimerTask = NULL;

  // Shut down the stream:
  shutdownStream(rtspClient);
}

void continueAfterOPTIONS(RTSPClient* rtspClient, int resultCode, char* resultString) 
{
	char szMsg[512];
	StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

	scs.iResultCode = resultCode;
	g_iResultCode = resultCode;

//  if (sendOptionsRequestOnly) {
    if (resultCode != 0) 
	{
		sprintf(szMsg,"continueAfterOPTIONS - resultCode:%d \n",resultCode);
		OutputDebugString(szMsg);
   //   *env << clientProtocolName << " \"OPTIONS\" request failed: " << resultString << "\n";
		shutdownStream(rtspClient);
		g_bOptionsRequestIsFinished = true;
		return;
    } 
	else 
	{
		sprintf(szMsg,"continueAfterOPTIONS - resultCode:%d \n",resultCode);
		OutputDebugString(szMsg);
 //     *env << clientProtocolName << " \"OPTIONS\" request returned: " << resultString << "\n";
    }
//    shutdown();
//  }
	delete[] resultString;

	g_bOptionsRequestIsFinished = true;

	// Next, get a SDP description for the stream:
    getSDPDescription(rtspClient,continueAfterDESCRIBE,scs.ourAuthenticator); //Authenticator* ourAuthenticator
}

void continueAfterOPTIONS2(RTSPClient* rtspClient, int resultCode, char* resultString)
{
	char szMsg[512];
	bool sendOptionsRequestOnly = true;
	StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

	scs.iResultCode = resultCode;
	g_iResultCode = resultCode;

  if (sendOptionsRequestOnly) {
		if (resultCode != 0)
		{
			sprintf(szMsg, "continueAfterOPTIONS - resultCode:%d \n", resultCode);
			OutputDebugString(szMsg);
			//   *env << clientProtocolName << " \"OPTIONS\" request failed: " << resultString << "\n";
			//shutdownStream(rtspClient);
			//return;
		}
		else
		{
			sprintf(szMsg, "continueAfterOPTIONS - resultCode:%d \n", resultCode);
			OutputDebugString(szMsg);
			//     *env << clientProtocolName << " \"OPTIONS\" request returned: " << resultString << "\n";
		}
		shutdownStream(rtspClient);
	}
	delete[] resultString;
	g_bOptionsRequestIsFinished = true;
}

unsigned getOptions(RTSPClient* rtspClient,RTSPClient::responseHandler* afterFunc,Authenticator* ourAuthenticator) 
{ 
	return rtspClient->sendOptionsCommand(afterFunc, ourAuthenticator);
}

void getSDPDescription(RTSPClient* rtspClient,RTSPClient::responseHandler* afterFunc,Authenticator* ourAuthenticator) 
{
	rtspClient->sendDescribeCommand(afterFunc, ourAuthenticator);
}
