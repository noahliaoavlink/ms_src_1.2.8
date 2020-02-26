#ifndef _ourRTSPClient_H
#define _ourRTSPClient_H

#if 0
#include "../../../Lib/liveMedia131011/include/liveMedia.hh"
#include "../../../Lib/liveMedia131011/include/BasicUsageEnvironment.hh"
#else
//#include "../../../Lib/live20180228/include/liveMedia.hh"
//#include "../../../Lib/live20180228/include/BasicUsageEnvironment.hh"

#include "../../../Lib/liveMedia20151012/include/liveMedia.hh"
#include "../../../Lib/liveMedia20151012/include/BasicUsageEnvironment.hh"
#include "../../../Lib/liveMedia20151012/include/GroupsockHelper.hh"
#endif

#include "CallbackObj.h"

class StreamClientState 
{
public:
  StreamClientState();
  virtual ~StreamClientState();

public:
  MediaSubsessionIterator* iter;
  MediaSession* session;
  MediaSubsession* subsession;
  TaskToken streamTimerTask;
  double duration;

  Authenticator* ourAuthenticator;
  CallbackObj* pCallbackObj;
  bool bEnableOverTCP;
  bool bEnableOverHTTP;
  int iHttpPort;
  bool bEnableAudio;
  int iResultCode;
};

class ourRTSPClient: public RTSPClient 
{
public:
  static ourRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
				  int verbosityLevel = 0,
				  char const* applicationName = NULL,
				  portNumBits tunnelOverHTTPPortNum = 0);

  void SetReceiveBufferTo(UsageEnvironment& env, int iBufferSize);

protected:
  ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
		int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum);
    // called only by createNew();
  virtual ~ourRTSPClient();

public:
  StreamClientState scs;
};

#endif