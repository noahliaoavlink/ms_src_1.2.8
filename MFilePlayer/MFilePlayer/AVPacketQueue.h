#pragma once
#ifndef _AVPacketQueue_H_
#define _AVPacketQueue_H_

#define _ENABLE_SQLIST 1

//#include "../../Include/FFAVCodecDLL.h"
#include "FFMediaFileDll.h"

#ifdef _ENABLE_SQLIST
#include "..\\..\\Include\\SQList.h"
#else
#include <stack>          // std::stack
#include <queue>          // std::queue
#endif

class AVPacketQueue
{
#ifdef _ENABLE_SQLIST
	SQList* m_pVideoFrameQueue;
#else
	std::queue<AVPacket> m_VideoFrameQueue;
#endif
	AVPacket* m_pTmpAVPacket;

	CRITICAL_SECTION m_CriticalSection;
public:
	AVPacketQueue();
	~AVPacketQueue();

	void Add(AVPacket pkt);
	AVPacket* Get();
	void Remove();
	int GetCount();
	void Reset();
};

#define MAX_AV_PACKET_ITMES 500

typedef struct
{
	int iSize;
	AVPacket avpkt;
}AVPacketItem;

class AVPacketQueue2
{
	int m_iMax;
	unsigned long m_ulID;
	int m_iReadIndex;
	int m_iNextReadIndex;
	int m_iWriteIndex;
	int m_iNextWriteIndex;

	long m_lFrameCount;

	AVPacket* m_pTmpAVPacket;
	bool m_bDoGetNext;

	AVPacketItem m_RBMItems[MAX_AV_PACKET_ITMES];
public:
	AVPacketQueue2();
	~AVPacketQueue2();

	void Free();
	bool IsFull();
	bool IsEmpty();
	void SetMax(int iValue);
	int GetMax();
	int Insert(int iIndex, AVPacket pkt);
	AVPacket* GetNext();


	bool Add(AVPacket pkt);
	AVPacket* Get();
	int GetCount();
	void Reset();
	void Remove();
};

#endif
