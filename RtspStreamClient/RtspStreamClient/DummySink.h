#ifndef _DummySink_H
#define _DummySink_H

#if 0

#include "../../../Lib/liveMedia131011/include/liveMedia.hh"
#include "../../../Lib/liveMedia131011/include/BasicUsageEnvironment.hh"

#else

//#include "../../../Lib/live20180228/include/liveMedia.hh"
//#include "../../../Lib/live20180228/include/BasicUsageEnvironment.hh"

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"

#endif

#include "CallbackObj.h"
#include "MVideoHeaderParser.h"
#include "H264Parser.h"
#include "../../../Include/RtspCommon.h"

class DummySink: public MediaSink 
{
	CallbackObj* m_pCallbackObj;
	int m_iDataFormat;
	char m_SPropParameterSetsStr[256];
	bool m_bHaveWrittenFirstFrame;
	int m_iOffset;
	long fBufferSize;
	bool m_bHasVPS;

	char* m_szSpropVPS;
	char* m_szSpropSPS;
	char* m_szSpropPPS;

	int m_iExtraDataLen;
	unsigned char m_szExtraData[1024];

	unsigned long m_ulID;

	int m_iWidth;
	int m_iHeight;
	int m_iPixelFormat;

	//for audio
	int m_iSample;
	int m_iBits;
	int m_iChannels;

	unsigned char* m_pTempBuffer;
	int m_iTempBufferLen;
	int m_iIPCount;

	bool m_bStop;

	unsigned long m_ulStartTimestamp;

	char m_SPS[256];
	char m_PPS[256];
	int m_iSPSLen;
	int m_iPPSLen;
public:
  static DummySink* createNew(UsageEnvironment& env,
			      MediaSubsession& subsession, // identifies the kind of data that's being received
			      char const* streamId = NULL,long bufferSize = 0,CallbackObj* pObj = 0); // identifies the stream itself (optional)

  void SetCallbackObj(CallbackObj* pObj);
  int GetDataFormat(char* szMediumName,char* szCodecName);
  void Stop();

private:
  DummySink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId,long bufferSize,CallbackObj* pObj);
    // called only by "createNew()"
  virtual ~DummySink();

  static void afterGettingFrame(void* clientData, unsigned frameSize,
                                unsigned numTruncatedBytes,
				struct timeval presentationTime,
                                unsigned durationInMicroseconds);
  void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
			 struct timeval presentationTime, unsigned durationInMicroseconds);

private:
  // redefined virtual functions:
  virtual Boolean continuePlaying();

private:
  u_int8_t* fReceiveBuffer;
  MediaSubsession& fSubsession;
  char* fStreamId;
};

#endif