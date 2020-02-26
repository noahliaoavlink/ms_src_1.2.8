#include "stdafx.h"
#include "ourRTSPClient.h"

// Implementation of "StreamClientState":

StreamClientState::StreamClientState()
  : iter(NULL), session(NULL), subsession(NULL), streamTimerTask(NULL), duration(0.0) 
{
	pCallbackObj = 0;
	ourAuthenticator = 0;
	session = 0;
	subsession = 0;
}

StreamClientState::~StreamClientState() 
{
	if(ourAuthenticator)
		delete ourAuthenticator;

	delete iter;

	if (session != NULL) 
	{
		// We also need to delete "session", and unschedule "streamTimerTask" (if set)
		UsageEnvironment& env = session->envir(); // alias

		env.taskScheduler().unscheduleDelayedTask(streamTimerTask);
		Medium::close(session);
	}
}

// Implementation of "ourRTSPClient":
ourRTSPClient* ourRTSPClient::createNew(UsageEnvironment& env, char const* rtspURL,
					int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum) 
{
  return new ourRTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

ourRTSPClient::ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
			     int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum)
  : RTSPClient(env,rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1) 
{
//	scs.pCallbackObj = (CallbackObj *)pObj;
}

ourRTSPClient::~ourRTSPClient() 
{
}

void ourRTSPClient::SetReceiveBufferTo(UsageEnvironment& env, int iBufferSize)
{
	setReceiveBufferTo(env, this->socketNum(), iBufferSize);
}