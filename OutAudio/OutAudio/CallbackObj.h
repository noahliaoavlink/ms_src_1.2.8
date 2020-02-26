#ifndef CallbackObj_H
#define CallbackObj_H

enum CallbackEvent
{
	CE_TIME_EVT = 1,
	CE_THREAD_EVT,
	CE_THREAD_EVT1,
	CE_GET_TIME_OFFSET,
	CE_IS_ALIVE,
	CE_DECODE_VIDEO_FRAME,
	CE_SET_SEARCH_INFO,
	CE_GET_SEARCH_INFO,
	CE_SET_TIME_OFFSET,
	CE_GET_CODEC_ID,
	CE_TIMERCALLBACK,
};

class CallbackObj
{
	int m_iDoEventLoop;
   public:
	    CallbackObj();
        ~CallbackObj();

		virtual void* DoEvent(int iEvent,void* pParameter1,void* pParameter2) = 0;
		virtual void SetEventLoop(int iValue) {m_iDoEventLoop = iValue;}; 
		virtual int GetEventLoop() {return m_iDoEventLoop;}
};

#endif