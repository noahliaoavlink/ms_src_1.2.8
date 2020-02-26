#include "stdafx.h"
#include "BackwardHelper2.h"

BackwardHelper2::BackwardHelper2()
{
	m_RawIndex = 0;

	int i;
	
	for (i = 0; i < _MAX_RAW_STACK_ITEMS; i++)
	{
		m_RawStack[i].iStatus = SM_INSERT;
	}

	m_ulLastTimestamp = 0;

	m_bDoSkip = false;
}

BackwardHelper2::~BackwardHelper2()
{
	Reset();
}

void BackwardHelper2::Reset()
{
	int i;

	char szMsg[512];
	sprintf(szMsg, "BackwardHelper2::Reset() 0\n");
	OutputDebugStringA(szMsg);
	
	for (i = 0; i < _MAX_RAW_STACK_ITEMS; i++)
	{
		int iTotal = m_RawStack[i].Stack.size();
		for (int j = 0; j < iTotal; j++)
		{
			RawVideoFrame* pCurFrame = &m_RawStack[i].Stack.top();
			delete pCurFrame->pBuffer;
			m_RawStack[i].Stack.pop();
		}
	}
	m_RawIndex = 0;

	sprintf(szMsg, "BackwardHelper2::Reset() 1\n");
	OutputDebugStringA(szMsg);
}

bool BackwardHelper2::IsWritable()
{
	for (int i = 0; i < _MAX_RCV_STACK_ITEMS; i++)
	{
		if (m_RawStack[i].iStatus == SM_INSERT)
			return true;
	}
	return false;
}

int BackwardHelper2::FindNextRawStack()
{
	int iOtherIndex = (m_RawIndex + 1) % _MAX_RAW_STACK_ITEMS;
	if (m_RawStack[iOtherIndex].iStatus == SM_INSERT)
		return iOtherIndex;
	return -1;
}

int BackwardHelper2::PushToRaw(RawVideoFrame* pFrame)
{
	char szMsg[512];
	
	if (pFrame->bIsKeyFrame && m_RawStack[m_RawIndex].Stack.size() > 0)
	{
		m_RawStack[m_RawIndex].iStatus = SM_FULL;
		
//		sprintf(szMsg, "PushToRaw: idx:%d FULL!!!\n", m_RawIndex);
//		OutputDebugStringA(szMsg);

		if (!CheckIndex(m_DisplayQueue, m_RawIndex))
		{
			m_DisplayQueue.push_back(m_RawIndex);  //push

			sprintf(szMsg, "PushToRaw: idx:%d total:%d TS:%d!!!\n", m_RawIndex, m_RawStack[m_RawIndex].Stack.size(), pFrame->ulTimestampSec);
			OutputDebugStringA(szMsg);
		}
		
	}

	if (m_RawStack[m_RawIndex].iStatus == SM_INSERT)
	{
		RawVideoFrame raw_frame;
		raw_frame.pBuffer = new char[pFrame->iLen];
		memcpy(&raw_frame, pFrame, sizeof(RawVideoFrame) - sizeof(char*));
		memcpy(raw_frame.pBuffer, pFrame->pBuffer, pFrame->iLen);
		m_RawStack[m_RawIndex].Stack.push(raw_frame);

		sprintf(szMsg, "PushToRaw: idx:%d total:%d TS:%d!!!\n", m_RawIndex, m_RawStack[m_RawIndex].Stack.size(), pFrame->ulTimestampSec);
		OutputDebugStringA(szMsg);
	}
	else
	{
		int iNextIndex = FindNextRawStack();
		if (iNextIndex == -1)
		{
			sprintf(szMsg, "PushToRaw: Error!!!\n");
			OutputDebugStringA(szMsg);

			return -1;
		}
		else
		{
//			sprintf(szMsg, "PushToRaw: FindNextRawStack :idx:%d -> %d !!!\n", m_RawIndex, iNextIndex);
//			OutputDebugStringA(szMsg);

			m_RawIndex = iNextIndex;

			

			RawVideoFrame raw_frame;
			raw_frame.pBuffer = new char[pFrame->iLen];
			memcpy(&raw_frame, pFrame, sizeof(RawVideoFrame) - sizeof(char*));
			memcpy(raw_frame.pBuffer, pFrame->pBuffer, pFrame->iLen);
			m_RawStack[m_RawIndex].Stack.push(raw_frame);

			sprintf(szMsg, "PushToRaw: idx:%d total:%d TS:%d!!!\n", m_RawIndex, m_RawStack[m_RawIndex].Stack.size(), pFrame->ulTimestampSec);
			OutputDebugStringA(szMsg);
		}
	}
	

	return 0;
}

RawVideoFrame* BackwardHelper2::GetNextFromRaw()
{
	if (m_DisplayQueue.size() > 0)
	{
		int iCurIndex = m_DisplayQueue.front();
		if (m_RawStack[iCurIndex].Stack.empty())
		{
			m_DisplayQueue.pop_front(); //pop();
			m_RawStack[iCurIndex].iStatus = SM_INSERT;
			return 0;
		}

		if (m_RawStack[iCurIndex].iStatus == SM_FULL)
			m_RawStack[iCurIndex].iStatus = SM_PROCESSING;
		if (m_RawStack[iCurIndex].iStatus == SM_PROCESSING)
			return &m_RawStack[iCurIndex].Stack.top();
	}
	return 0;
}

void BackwardHelper2::PopFromRaw()
{
	if (m_DisplayQueue.size() > 0)
	{
		int iCurIndex = m_DisplayQueue.front();
		if (!m_RawStack[iCurIndex].Stack.empty())
		{
			RawVideoFrame* p = &m_RawStack[iCurIndex].Stack.top();
			BOOL bRet = IsBadReadPtr(p->pBuffer, p->iLen);
			if (!bRet)
			{
				delete p->pBuffer;
				//delete p;
			}
			m_RawStack[iCurIndex].Stack.pop();
		}
	}
}

bool BackwardHelper2::CheckIndex(std::list<int> queue, int iIndex)
{
	if (queue.size() > 0)
	{
		for (auto it = queue.begin(); it != queue.end(); ++it)
		{
			if (iIndex == *it)
				return true;
		}
	}
	return false;
}