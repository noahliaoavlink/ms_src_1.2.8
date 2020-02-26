#ifndef _BackwardHelper_H
#define _BackwardHelper_H

#include <stack>          // std::stack
#include <queue>          // std::queue
#include <list>
#include "IDStreamCommon.h"
#include "../../../Include/MediaFrameInfo.h"

#define _MAX_RCV_STACK_ITEMS 2
#define _MAX_RAW_STACK_ITEMS 2

enum StackMode
{
	SM_INSERT = 0,
	SM_FULL,
	SM_PROCESSING,
};

typedef struct
{
#if 0
	std::stack<StreamFrame> Stack;
#else
	std::queue<AVPacket> Stack;
#endif
	int iStatus;
	int iMax;
}RcvFrameBufferStack;

typedef struct
{
	std::stack<RawVideoFrame> Stack;
	int iStatus;
}RawFrameBufferStack;

class BackwardHelper
{
		RcvFrameBufferStack m_RcvStack[_MAX_RCV_STACK_ITEMS];
		RawFrameBufferStack m_RawStack[_MAX_RAW_STACK_ITEMS];

		//std::queue<int> m_DecodeQueue;
		//std::queue<int> m_DisplayQueue;
		std::list<int> m_DecodeQueue;
		std::list<int> m_DisplayQueue;

		int m_RcvIndex;
		int m_RawIndex;

		//for debug
		unsigned long m_ulLastTimestamp;

   public:
	    BackwardHelper();
        ~BackwardHelper();

		void SetDecoderObj(void* pFFObj,void* pHikObj);

		bool IsWritable();

		void Reset();
		//Rcv
#if 0
		int PushToRcv(StreamFrame* pFrame);
#else
		int PushToRcv(AVPacket* pFrame);
#endif
		int FindNextRcvStack();
#if 0
		StreamFrame* GetNextFromRcv();
#else
		AVPacket* GetNextFromRcv();
#endif
		bool CheckIndex(std::list<int> queue, int iIndex);

		void PopFromRcv();
		int GetTotalOfRcvItems();
		int GetMaxRcvItems();

		//raw 
		int PushToRaw(RawVideoFrame* pFrame);
		int FindNextRawStack();
		RawVideoFrame* GetNextFromRaw();
		void PopFromRaw();
};

#endif