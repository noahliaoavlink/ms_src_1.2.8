#pragma once
#ifndef _BackwardHelper2_H
#define _BackwardHelper2_H

#include "../../Include/BackwardHelper.h"


class BackwardHelper2
{
	RawFrameBufferStack m_RawStack[_MAX_RAW_STACK_ITEMS];
	int m_RawIndex;

	std::list<int> m_DisplayQueue;

	unsigned long m_ulLastTimestamp;

	bool m_bDoSkip;
public:
	BackwardHelper2();
	~BackwardHelper2();

	void Reset();

	bool IsWritable();
	int FindNextRawStack();
	int PushToRaw(RawVideoFrame* pFrame);
	RawVideoFrame* GetNextFromRaw();
	void PopFromRaw();

	bool CheckIndex(std::list<int> queue, int iIndex);

};

#endif
