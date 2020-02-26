#include "stdafx.h"
#include "../Include/BackwardHelper.h"

#pragma comment(lib, "avcodec.lib")
//#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
//#pragma comment(lib, "swscale.lib")
//#pragma comment(lib, "swresample.lib")
//#pragma comment(lib, "winmm.lib")

unsigned int __stdcall _DecodeThreadProc(void* lpvThreadParm);

BackwardHelper::BackwardHelper()
{
	m_RcvIndex = 0;
	m_RawIndex = 0;

	int i;
	for(i = 0;i < _MAX_RCV_STACK_ITEMS;i++)
	{
		m_RcvStack[i].iStatus = SM_INSERT;
		m_RcvStack[i].iMax = 0;
	}

	for(i = 0;i < _MAX_RAW_STACK_ITEMS;i++)
	{
		m_RawStack[i].iStatus = SM_INSERT;
	}
}

BackwardHelper::~BackwardHelper()
{
	Reset();
}

void BackwardHelper::Reset()
{
	int i;
	for(i = 0;i < _MAX_RCV_STACK_ITEMS;i++)
	{
		int iTotal = m_RcvStack[i].Stack.size();
		for(int j = 0;j < iTotal;j++)
		{
#if 0
			StreamFrame* pCurFrame = &m_RcvStack[i].Stack.top();
			delete pCurFrame->pBuffer;
#else
			AVPacket* pCurFrame = &m_RcvStack[i].Stack.front();// top();
				//av_packet_free(pCurFrame);

				av_packet_unref(pCurFrame);
				av_freep(pCurFrame);
#endif
			m_RcvStack[i].Stack.pop();
		}
	}

	for(i = 0;i < _MAX_RAW_STACK_ITEMS;i++)
	{
		int iTotal = m_RawStack[i].Stack.size();
		for(int j = 0;j < iTotal;j++)
		{
			RawVideoFrame* pCurFrame = &m_RawStack[i].Stack.top();
			delete pCurFrame->pBuffer;
			m_RawStack[i].Stack.pop();
		}
	}
	m_RcvIndex = 0;
	m_RawIndex = 0;
}

bool BackwardHelper::IsWritable()
{
	for(int i = 0;i < _MAX_RCV_STACK_ITEMS;i++)
	{
		if(m_RcvStack[i].iStatus == SM_INSERT)
			return true;
	}
	return false;
}

#if 0
int BackwardHelper::PushToRcv(StreamFrame* pFrame)
#else
int BackwardHelper::PushToRcv(AVPacket* pFrame)
#endif
{
	char szMsg[512];

	int iFlags = pFrame->flags;

	if (pFrame->flags == AV_PKT_FLAG_KEY && m_RcvStack[m_RcvIndex].Stack.size() > 0)
	{
		if (!CheckIndex(m_DecodeQueue, m_RcvIndex))
		{
			m_RcvStack[m_RcvIndex].iStatus = SM_FULL;
			m_RcvStack[m_RcvIndex].iMax = m_RcvStack[m_RcvIndex].Stack.size();
			m_DecodeQueue.push_back(m_RcvIndex);   ////push

			sprintf(szMsg, "PushToRcv: idx:%d total:%d !!!\n", m_RcvIndex, m_RcvStack[m_RcvIndex].Stack.size());
			OutputDebugStringA(szMsg);
		}
	}
	
	if (m_RcvStack[m_RcvIndex].iStatus == SM_INSERT)
	{
		AVPacket pkt;
		av_copy_packet(&pkt, pFrame);
		m_RcvStack[m_RcvIndex].Stack.push(pkt);

		sprintf(szMsg, "PushToRcv: idx:%d num:%d %d [%d] !!!\n", m_RcvIndex, m_RcvStack[m_RcvIndex].Stack.size(), iFlags, pFrame->dts);
		OutputDebugStringA(szMsg);
	}
	else
	{
		int iNextIndex = FindNextRcvStack();
		if (iNextIndex == -1)
			return -1;
		else
		{
			m_RcvIndex = iNextIndex;

			AVPacket pkt;
			av_copy_packet(&pkt, pFrame);
			m_RcvStack[m_RcvIndex].Stack.push(pkt);

			sprintf(szMsg, "PushToRcv: idx:%d num:%d %d [%d] !!!\n", m_RcvIndex, m_RcvStack[m_RcvIndex].Stack.size(), iFlags,pFrame->dts);
			OutputDebugStringA(szMsg);
		}

	}

	/*
	if(m_RcvStack[m_RcvIndex].iStatus == SM_INSERT)
	{

#if 0
		if(pFrame->bIsKeyFrame)
#else
		if (pFrame->flags == AV_PKT_FLAG_KEY && m_RcvStack[m_RcvIndex].Stack.size() > 0)
#endif
		{
			m_RcvStack[m_RcvIndex].iStatus = SM_FULL;	
			m_RcvStack[m_RcvIndex].iMax = m_RcvStack[m_RcvIndex].Stack.size();
			m_DecodeQueue.push(m_RcvIndex);

			sprintf(szMsg,"PushToRcv: idx:%d total:%d !!!\n",m_RcvIndex,m_RcvStack[m_RcvIndex].Stack.size());
			OutputDebugStringA(szMsg);
		}

#if 0
		StreamFrame stream_frame;
		stream_frame.pBuffer = new char[pFrame->iLen];
		memcpy(&stream_frame, pFrame, sizeof(StreamFrame) - sizeof(char*));
		memcpy(stream_frame.pBuffer, pFrame->pBuffer, pFrame->iLen);
		m_RcvStack[m_RcvIndex].Stack.push(stream_frame);

		sprintf(szMsg, "PushToRcv: idx:%d num:%d [%d %d]!!!\n", m_RcvIndex, m_RcvStack[m_RcvIndex].Stack.size(), pFrame->ulTimestampSec, pFrame->ulTimestampMSec);
		OutputDebugStringA(szMsg);
#else
		AVPacket pkt;
		av_copy_packet(&pkt, pFrame);
		m_RcvStack[m_RcvIndex].Stack.push(pkt);

		//		sprintf(szMsg, "PushToRcv: idx:%d num:%d [%d]!!!\n", m_RcvIndex, m_RcvStack[m_RcvIndex].Stack.size(), pFrame->dts);
		//		OutputDebugStringA(szMsg);
#endif
	}
	else 
	{
		int iNextIndex = FindNextRcvStack();
		if(iNextIndex == -1)
			return -1;
		else
		{
			m_RcvIndex = iNextIndex;

#if 0
			StreamFrame stream_frame;
			stream_frame.pBuffer = new char[pFrame->iLen];
			memcpy(&stream_frame,pFrame,sizeof(StreamFrame) - sizeof(char*));
			memcpy(stream_frame.pBuffer,pFrame->pBuffer,pFrame->iLen);
			m_RcvStack[m_RcvIndex].Stack.push(stream_frame);

			sprintf(szMsg,"PushToRcv: idx:%d num:%d [%d %d]!!!\n",m_RcvIndex,m_RcvStack[m_RcvIndex].Stack.size(),pFrame->ulTimestampSec,pFrame->ulTimestampMSec);
			OutputDebugStringA(szMsg);
#else
			AVPacket pkt;
			av_copy_packet(&pkt, pFrame);
			m_RcvStack[m_RcvIndex].Stack.push(pkt);

//			sprintf(szMsg, "PushToRcv: idx:%d num:%d [%d]!!!\n", m_RcvIndex, m_RcvStack[m_RcvIndex].Stack.size(), pFrame->dts);
//			OutputDebugStringA(szMsg);
#endif
		}
	}

	*/

	return 0;
}

int BackwardHelper::FindNextRcvStack()
{
	int iOtherIndex = (m_RcvIndex + 1) % _MAX_RCV_STACK_ITEMS;
	if(m_RcvStack[iOtherIndex].iStatus == SM_INSERT)
		return iOtherIndex;
	return -1;
}

int BackwardHelper::FindNextRawStack()
{
	int iOtherIndex = (m_RawIndex + 1) % _MAX_RAW_STACK_ITEMS;
	if(m_RawStack[iOtherIndex].iStatus == SM_INSERT)
		return iOtherIndex;
	return -1;
}

#if 0
StreamFrame* BackwardHelper::GetNextFromRcv()
#else
AVPacket* BackwardHelper::GetNextFromRcv()
#endif
{
	if(m_DecodeQueue.size() > 0)
	{
		int iCurIndex = m_DecodeQueue.front();
		if(m_RcvStack[iCurIndex].Stack.empty())
		{
			m_DecodeQueue.pop_front();// pop();
			m_RcvStack[iCurIndex].iStatus = SM_INSERT;
			return 0;
		}

		if(m_RcvStack[iCurIndex].iStatus == SM_FULL)
			m_RcvStack[iCurIndex].iStatus = SM_PROCESSING;
		if (m_RcvStack[iCurIndex].iStatus == SM_PROCESSING)
			return &m_RcvStack[iCurIndex].Stack.front();// top();
	}
	return 0;
}

void BackwardHelper::PopFromRcv()
{
	if(m_DecodeQueue.size() > 0)
	{
		int iCurIndex = m_DecodeQueue.front();
		if(!m_RcvStack[iCurIndex].Stack.empty())
		{
#if 0
			StreamFrame* p = &m_RcvStack[iCurIndex].Stack.top();
			delete p->pBuffer;
#else
			AVPacket* pCurFrame = &m_RcvStack[iCurIndex].Stack.front();// top();
			
			if (pCurFrame)
			{
				//BOOL bRet = IsBadReadPtr(pCurFrame->buf, 4);
				//if (!bRet)
				{
					av_packet_unref(pCurFrame);
					av_freep(pCurFrame);
				}
			}
			
#endif
			m_RcvStack[iCurIndex].Stack.pop();
		}
	}
}

int BackwardHelper::GetTotalOfRcvItems()
{
	int iTotal = 0;
	for(int i = 0;i < _MAX_RAW_STACK_ITEMS;i++)
		iTotal += m_RcvStack[i].Stack.size();

	return iTotal;
}

int BackwardHelper::GetMaxRcvItems()
{
	int iTotal = 0;
	for(int i = 0;i < _MAX_RAW_STACK_ITEMS;i++)
		iTotal += m_RcvStack[i].iMax;

	return iTotal;
}

int BackwardHelper::PushToRaw(RawVideoFrame* pFrame)
{
	char szMsg[512];

	if (pFrame->bIsKeyFrame && m_RawStack[m_RawIndex].Stack.size() > 0)
	{
		m_RawStack[m_RawIndex].iStatus = SM_FULL;

		if (!CheckIndex(m_DisplayQueue, m_RawIndex))
		{
			m_DisplayQueue.push_back(m_RawIndex);  //push

			sprintf(szMsg, "PushToRaw: idx:%d total:%d !!!\n", m_RawIndex, m_RawStack[m_RawIndex].Stack.size());
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

		if (m_ulLastTimestamp != pFrame->ulTimestampSec)
		{
			m_ulLastTimestamp = pFrame->ulTimestampSec;
		}
		else
			int kk = 0;

		sprintf(szMsg, "PushToRaw: idx:%d num:%d [%d %d]!!!\n", m_RawIndex, m_RawStack[m_RawIndex].Stack.size(), pFrame->ulTimestampSec, pFrame->ulTimestampMSec);
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
			m_RawIndex = iNextIndex;

			RawVideoFrame raw_frame;
			raw_frame.pBuffer = new char[pFrame->iLen];
			memcpy(&raw_frame, pFrame, sizeof(RawVideoFrame) - sizeof(char*));
			memcpy(raw_frame.pBuffer, pFrame->pBuffer, pFrame->iLen);
			m_RawStack[m_RawIndex].Stack.push(raw_frame);

			if (m_ulLastTimestamp != pFrame->ulTimestampSec)
			{
				m_ulLastTimestamp = pFrame->ulTimestampSec;
			}
			else
				int kk = 0;

			sprintf(szMsg, "PushToRaw: idx:%d num:%d [%d %d]!!!\n", m_RawIndex, m_RawStack[m_RawIndex].Stack.size(), pFrame->ulTimestampSec, pFrame->ulTimestampMSec);
			OutputDebugStringA(szMsg);
		}
	}
/*
	if(m_RawStack[m_RawIndex].iStatus == SM_INSERT)
	{
		if(pFrame->bIsKeyFrame && m_RawStack[m_RawIndex].Stack.size() > 0)
		{
			m_RawStack[m_RawIndex].iStatus = SM_FULL;	
			m_DisplayQueue.push(m_RawIndex);

			sprintf(szMsg,"PushToRaw: idx:%d total:%d !!!\n",m_RawIndex,m_RawStack[m_RawIndex].Stack.size());
			OutputDebugStringA(szMsg);

			int iNextIndex = FindNextRawStack();
			if(iNextIndex != -1)
			{
				m_RawIndex = iNextIndex;
		//		m_RawStack[m_RawIndex].Stack.push(*pFrame);
			}
			else
			{
				sprintf(szMsg,"PushToRaw: Error !!!\n");
				OutputDebugStringA(szMsg);
			}
		}
		
		RawVideoFrame raw_frame;
		raw_frame.pBuffer = new char[pFrame->iLen];
		memcpy(&raw_frame,pFrame,sizeof(RawVideoFrame) - sizeof(char*));
		memcpy(raw_frame.pBuffer,pFrame->pBuffer,pFrame->iLen);
		m_RawStack[m_RawIndex].Stack.push(raw_frame);

		sprintf(szMsg,"PushToRaw: idx:%d num:%d [%d %d]!!!\n",m_RawIndex,m_RawStack[m_RawIndex].Stack.size(),pFrame->ulTimestampSec,pFrame->ulTimestampMSec);
		OutputDebugStringA(szMsg);
	}
	else 
	{
		int iNextIndex = FindNextRawStack();
		if(iNextIndex == -1)
		{
			sprintf(szMsg,"PushToRaw: Error!!!\n");
			OutputDebugStringA(szMsg);

			return -1;
		}
		else
		{
			m_RawIndex = iNextIndex;

			RawVideoFrame raw_frame;
			raw_frame.pBuffer = new char[pFrame->iLen];
			memcpy(&raw_frame,pFrame,sizeof(RawVideoFrame) - sizeof(char*));
			memcpy(raw_frame.pBuffer,pFrame->pBuffer,pFrame->iLen);
			m_RawStack[m_RawIndex].Stack.push(raw_frame);

			sprintf(szMsg,"PushToRaw: idx:%d num:%d [%d %d]!!!\n",m_RawIndex,m_RawStack[m_RawIndex].Stack.size(),pFrame->ulTimestampSec,pFrame->ulTimestampMSec);
			OutputDebugStringA(szMsg);
		}
	}
	*/

	return 0;
}

RawVideoFrame* BackwardHelper::GetNextFromRaw()
{
	if(m_DisplayQueue.size() > 0)
	{
		int iCurIndex = m_DisplayQueue.front();
		if(m_RawStack[iCurIndex].Stack.empty())
		{
			m_DisplayQueue.pop_front(); //pop();
			m_RawStack[iCurIndex].iStatus = SM_INSERT;
			return 0;
		}

		if(m_RawStack[iCurIndex].iStatus == SM_FULL)
			m_RawStack[iCurIndex].iStatus = SM_PROCESSING;
		if(m_RawStack[iCurIndex].iStatus == SM_PROCESSING)
			return &m_RawStack[iCurIndex].Stack.top();
	}
	return 0;
}

void BackwardHelper::PopFromRaw()
{
	if(m_DisplayQueue.size() > 0)
	{
		int iCurIndex = m_DisplayQueue.front();
		if(!m_RawStack[iCurIndex].Stack.empty())
		{
			RawVideoFrame* p = &m_RawStack[iCurIndex].Stack.top();
			BOOL bRet = IsBadReadPtr(p->pBuffer,p->iLen);
			if (!bRet)
			{
				delete p->pBuffer;
				//delete p;
			}
			m_RawStack[iCurIndex].Stack.pop();
		}
	}
}

bool BackwardHelper::CheckIndex(std::list<int> queue,int iIndex)
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

