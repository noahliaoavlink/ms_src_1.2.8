#ifndef CallbackObj_H
#define CallbackObj_H

enum CallbackEvent
{
	CE_UPDATE_STATUS = 1,
    CE_RECONNECT,
};

typedef struct
{
	int iFormat;             // (mpeg4,h264,mjpg)
	int iLen;                
	int iWidth;
	int iHeight;
	int iPixelFormat;      //for mjpeg
	int iIsIFrame;
	int iIPCount;
	int iFrameType;
	int iSample; //for audio
	int iBits; //for audio
	int iChannels;
	unsigned long ulTimestampSec;
	unsigned long ulTimestampMSec;
	unsigned char* pBuffer;
}MediaFrameInfo;

class CallbackObj
{
   public:
	    CallbackObj();
        ~CallbackObj();

		//virtual void DoEvent(unsigned long ulID,int iFormat,unsigned char* pBuffer,int iLen) = 0;
		//virtual void DoEvent1() = 0;
		virtual void* DoEvent2(int iEventID,void* pParameter1,void* pParameter2) = 0;
		virtual void CaptureFrame(int iPayload,unsigned char* pBuffer,int iLen,int iIPCount) = 0;

		virtual void ProcessFrameData(unsigned long ulID,MediaFrameInfo* pInfo) = 0;
};

#endif