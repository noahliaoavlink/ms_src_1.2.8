#include "stdafx.h"
#include "AVPacketQueue.h"

AVPacketQueue::AVPacketQueue()
{
	InitializeCriticalSection(&m_CriticalSection);
#ifdef _ENABLE_SQLIST
	m_pVideoFrameQueue = new SQList;
	m_pVideoFrameQueue->EnableRemoveData(false);
#else
#endif
}

AVPacketQueue::~AVPacketQueue()
{
#ifdef _ENABLE_SQLIST
	int iTotal = m_pVideoFrameQueue->GetTotal();
	for (int i = 0; i < iTotal; i++)
	{
		AVPacket* pCurFrame = (AVPacket*)m_pVideoFrameQueue->Get(i);
		if (pCurFrame)
		{
			av_packet_unref(pCurFrame);
//			av_freep(pCurFrame);
		}
	}
	delete m_pVideoFrameQueue;
#else
	int iTotal = m_VideoFrameQueue.size();
	for (int j = 0; j < iTotal; j++)
	{
		AVPacket* pCurFrame = &m_VideoFrameQueue.front();

		av_packet_unref(pCurFrame);
		av_freep(pCurFrame);

		m_VideoFrameQueue.pop();
	}
#endif
	DeleteCriticalSection(&m_CriticalSection);
}

void AVPacketQueue::Add(AVPacket pkt)
{
	EnterCriticalSection(&m_CriticalSection);
#ifdef _ENABLE_SQLIST
	AVPacket* pNewPacket = (AVPacket*)malloc(sizeof(struct AVPacket));
	av_init_packet(pNewPacket);
	av_packet_ref(pNewPacket, &pkt);

	m_pVideoFrameQueue->Add(pNewPacket);

	av_packet_unref(&pkt);
//	av_freep(&pkt);
#else
	m_VideoFrameQueue.push(pkt);
#endif
	LeaveCriticalSection(&m_CriticalSection);
}

AVPacket* AVPacketQueue::Get()
{
	EnterCriticalSection(&m_CriticalSection);
#ifdef _ENABLE_SQLIST
	AVPacket* pCurFrame = (AVPacket*)m_pVideoFrameQueue->Get(0);
	m_pTmpAVPacket = pCurFrame;
#else
	m_pTmpAVPacket = &m_VideoFrameQueue.front();
#endif
	LeaveCriticalSection(&m_CriticalSection);
	return m_pTmpAVPacket;
}

void AVPacketQueue::Remove()
{
#ifdef _ENABLE_SQLIST
	if (m_pVideoFrameQueue->GetTotal() > 0)
	{
		AVPacket* pCurFrame = (AVPacket*)m_pVideoFrameQueue->Get(0);
		if (pCurFrame)
		{
			av_packet_unref(pCurFrame);
			av_freep(pCurFrame);
		}
		m_pVideoFrameQueue->Delete(0);
	}
#else
	if(m_VideoFrameQueue.size() > 0)
		m_VideoFrameQueue.pop();
#endif
}

int AVPacketQueue::GetCount()
{
#ifdef _ENABLE_SQLIST
	return m_pVideoFrameQueue->GetTotal();
#else
	return m_VideoFrameQueue.size();
#endif
}

void AVPacketQueue::Reset()
{
	EnterCriticalSection(&m_CriticalSection);
#ifdef _ENABLE_SQLIST
	int iTotal = m_pVideoFrameQueue->GetTotal();
	for (int i = 0; i < iTotal; i++)
	{
		AVPacket* pCurFrame = (AVPacket*)m_pVideoFrameQueue->Get(i);
		if (pCurFrame)
		{
			av_packet_unref(pCurFrame);
//			av_freep(pCurFrame);
		}
	}
	m_pVideoFrameQueue->Reset();
#else
	int iTotal = m_VideoFrameQueue.size();
	for (int j = 0; j < iTotal; j++)
	{
		if (m_VideoFrameQueue.size() > 0)
		{

			AVPacket* pCurFrame = &m_VideoFrameQueue.front();
			if (pCurFrame)
			{
				av_packet_unref(pCurFrame);
				av_freep(pCurFrame);
			}

			m_VideoFrameQueue.pop();
		}
	}
#endif
	LeaveCriticalSection(&m_CriticalSection);
}

//===============AVPacketQueue2===============//
AVPacketQueue2::AVPacketQueue2()
{
	m_iMax = 64;//30;
	m_ulID = 0;

	m_iReadIndex = 0;
	m_iNextReadIndex = 0;
	m_iWriteIndex = 0;
	m_iNextWriteIndex = 0;

	m_lFrameCount = 0;
	m_bDoGetNext = true;

	for (int i = 0; i < m_iMax; i++)
	{
		av_init_packet(&m_RBMItems[i].avpkt);
		m_RBMItems[i].bInit = true;
	}
}

AVPacketQueue2::~AVPacketQueue2()
{
	Free();
}

void AVPacketQueue2::Free()
{
	for (int i = 0; i < m_iMax; i++)
	{
		try
		{
			if (m_RBMItems[i].iSize > 0)
			{
				av_packet_unref(&m_RBMItems[i].avpkt);
				//av_free_packet(&m_RBMItems[i].avpkt);
//				av_freep(&m_RBMItems[i].avpkt);
			}
			m_RBMItems[i].iSize = 0;
		}
		catch (...)
		{
			char szMsg[512];
			sprintf(szMsg, "HEError - AVPacketQueue2::Free memcpy cause exception (i=%d)!!\n",i);
			OutputDebugStringA(szMsg);

			//return ;
		}
	}
}

bool AVPacketQueue2::IsFull()
{
	if (m_lFrameCount >= m_iMax)
		return true;
	return false;
}

bool AVPacketQueue2::IsEmpty()
{
	if (m_lFrameCount == 0)
		return true;
	return false;
}

void AVPacketQueue2::SetMax(int iValue)
{
	if (m_iMax != iValue)
		Free();

	m_iMax = iValue;
}

int AVPacketQueue2::GetMax()
{
	return m_iMax;
}

int AVPacketQueue2::Insert(int iIndex, AVPacket pkt)
{
	if (pkt.size <= 0 || iIndex < 0 || iIndex >= m_iMax)
	{
		return -1;
	}

	try
	{
		//memcpy((unsigned char*)m_RBMItems[iIndex].pBuffer, pBuffer, iLen);

#if 0
		av_packet_unref(&m_RBMItems[iIndex].avpkt);
		av_freep(&m_RBMItems[iIndex].avpkt);
		av_init_packet(&m_RBMItems[iIndex].avpkt);
		av_packet_ref(&m_RBMItems[iIndex].avpkt, &pkt);
		av_packet_unref(&pkt);
		av_free_packet(&pkt);
		//av_freep(&pkt);
#else
		//if (m_RBMItems[iIndex].iSize > 0)
		{
			av_packet_unref(&m_RBMItems[iIndex].avpkt);
			//av_free_packet(&m_RBMItems[iIndex].avpkt);
			//av_freep(&m_RBMItems[iIndex].avpkt);
		}
//		av_init_packet(&m_RBMItems[iIndex].avpkt);
		av_packet_ref(&m_RBMItems[iIndex].avpkt, &pkt);
		av_packet_unref(&pkt);
		//av_free_packet(&pkt);
		//av_freep(&pkt);
#endif
	}
	catch (...)
	{
		char szMsg[512];
		sprintf(szMsg, "HEError - AVPacketQueue2::Insert memcpy cause exception !!\n");
		OutputDebugStringA(szMsg);

		return -3;
	}

	m_RBMItems[iIndex].iSize = pkt.size;// + 4;

	return 1;
}

bool AVPacketQueue2::Add(AVPacket pkt)
{
	if (IsFull())
		return false;
	else
	{
		int iRet = Insert(m_iWriteIndex, pkt);
		if (iRet < 0)
			return false;
		m_iWriteIndex = (m_iWriteIndex + 1) % m_iMax;
		m_lFrameCount++;
	}

	return false;
}

AVPacket* AVPacketQueue2::GetNext()
{
	int iLen = 0;
	if (!IsEmpty())
	{
		if (m_RBMItems[m_iReadIndex].iSize < 0)
		{
			m_lFrameCount--;
			return 0;
		}
		//memcpy(pBuffer, m_RBMItems[m_iReadIndex].pBuffer, m_RBMItems[m_iReadIndex].iSize);

		m_pTmpAVPacket = &m_RBMItems[m_iReadIndex].avpkt;

		iLen = m_RBMItems[m_iReadIndex].iSize;
		m_iReadIndex = (m_iReadIndex + 1) % m_iMax;

		m_lFrameCount--;
		return m_pTmpAVPacket;
	}
	return 0;

}

AVPacket* AVPacketQueue2::Get()
{
	if (m_bDoGetNext)
	{
		m_bDoGetNext = false;
		m_pTmpAVPacket = GetNext();
	}

	return m_pTmpAVPacket;
}

int AVPacketQueue2::GetCount()
{
	return m_lFrameCount;
}

void AVPacketQueue2::Reset()
{
	/*
	av_packet_unref(&m_RBMItems[m_iWriteIndex].avpkt);
	av_free_packet(&m_RBMItems[m_iWriteIndex].avpkt);
	av_freep(&m_RBMItems[m_iWriteIndex].avpkt);
	*/

	Free();
	m_iReadIndex = 0;
	m_iWriteIndex = 0;
	m_lFrameCount = 0;
	m_bDoGetNext = true;
}

void AVPacketQueue2::Remove()
{
	m_bDoGetNext = true;
}
//===============AVPacketQueue2===============//
