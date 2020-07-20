#include "stdafx.h"
#include "../Include/FrameBufferManager.h"

#define _HEADER_LEN 64

/*
FrameBufferManager::FrameBufferManager(int iBufferSize,int iMax)
{
	m_pTempFrameBuffer1 = new unsigned char[iBufferSize];
	m_pTempFrameBuffer2 = new unsigned char[iBufferSize];
	m_pFrameQBuffer = new QBufferManagerEx;
	m_pFrameQBuffer->SetMax(iMax);
	m_pFrameQBuffer->SetBufferSize(iBufferSize);
}
*/
FrameBufferManager::FrameBufferManager()
{
	m_pTempFrameBuffer1 = 0;
	m_pTempFrameBuffer2 = 0;
#if (_ENABLE_DUAL_BUFFER == 1)
	for(int i = 0;i < 2;i++)
	{
		m_pFrameQBuffer[i] = new QBufferManagerEx;
		m_pFrameQBuffer[i]->SetMax(0);
	}
	m_iCurQBIndex = 0;
#else
	m_pFrameQBuffer = new QBufferManagerEx;
	m_pFrameQBuffer->SetMax(0);
#endif
}

FrameBufferManager::~FrameBufferManager()
{
#if (_ENABLE_DUAL_BUFFER == 1)
	for(int i = 0;i < 2;i++)
	{
		if (m_pFrameQBuffer[i])
		{
			delete m_pFrameQBuffer[i];
			m_pFrameQBuffer[i] = 0;
		}
	}
#else
	if(m_pFrameQBuffer)
		delete m_pFrameQBuffer;
#endif

	delete m_pTempFrameBuffer1;
	delete m_pTempFrameBuffer2;
}

void FrameBufferManager::Alloc(int iBufferSize,int iMax)
{
	if(m_pTempFrameBuffer1)
		delete m_pTempFrameBuffer1;
	if(m_pTempFrameBuffer2)
		delete m_pTempFrameBuffer2;

	//int iNewBufferSize = MultipleOf(iBufferSize,64);
	int iNewBufferSize = CalBuffLen(iBufferSize);

	m_pTempFrameBuffer1 = new unsigned char[iNewBufferSize];
	m_pTempFrameBuffer2 = new unsigned char[iNewBufferSize];
//	m_pFrameQBuffer = new QBufferManagerEx;
#if (_ENABLE_DUAL_BUFFER == 1)
	m_pFrameQBuffer[m_iCurQBIndex]->SetMax(iMax);
	m_pFrameQBuffer[m_iCurQBIndex]->SetBufferSize(iNewBufferSize);
#else
	m_pFrameQBuffer->SetMax(iMax);
	m_pFrameQBuffer->SetBufferSize(iNewBufferSize);
#endif
}

void FrameBufferManager::AddToQueueBuffer(StreamFrame* pInfo)
{
#if (_ENABLE_DUAL_BUFFER == 1)
	if(m_pFrameQBuffer[m_iCurQBIndex]->GetUnitSize() <= (pInfo->iLen + _HEADER_LEN) )
#else
	if(m_pFrameQBuffer->GetUnitSize() <= (pInfo->iLen + _HEADER_LEN) )
#endif
	{
		int iBufferLen = pInfo->iLen + _HEADER_LEN;
		ReAlloc(iBufferLen);
	}

	int iHeadLen = sizeof(StreamFrame) - sizeof(unsigned char*);
	memcpy(m_pTempFrameBuffer1,pInfo,iHeadLen);

	memcpy(m_pTempFrameBuffer1 + _HEADER_LEN,pInfo->pBuffer,pInfo->iLen);
#if (_ENABLE_DUAL_BUFFER == 1)
	m_pFrameQBuffer[m_iCurQBIndex]->Add(m_pTempFrameBuffer1,_HEADER_LEN + pInfo->iLen);
#else
	m_pFrameQBuffer->Add(m_pTempFrameBuffer1,_HEADER_LEN + pInfo->iLen);
#endif

}

StreamFrame* FrameBufferManager::GetNext()
{
#if (_ENABLE_DUAL_BUFFER == 1)
	if(!m_pFrameQBuffer || !m_pFrameQBuffer[m_iCurQBIndex])
#else
	if(!m_pFrameQBuffer)
#endif
		return 0;

#if (_ENABLE_DUAL_BUFFER == 1)
	if(m_pFrameQBuffer[m_iCurQBIndex]->GetTotal() > 0)
	{
		int iLen = m_pFrameQBuffer[m_iCurQBIndex]->GetNext(m_pTempFrameBuffer2);
#else
	if(m_pFrameQBuffer->GetTotal() > 0)
	{
		int iLen = m_pFrameQBuffer->GetNext(m_pTempFrameBuffer2);
#endif
		if(iLen > 0)
		{
			m_pStreamFrame = (StreamFrame*)m_pTempFrameBuffer2;
			//int iHeadLen = sizeof(StreamFrame) - sizeof(unsigned char*);
			int iHeadLen = sizeof(StreamFrame);

			m_pStreamFrame->pBuffer = (char *)(m_pTempFrameBuffer2 + _HEADER_LEN);
			return m_pStreamFrame;
		}
	}
	return 0;
}

void FrameBufferManager::Reset()
{
#if (_ENABLE_DUAL_BUFFER == 1)
	if(m_iCurQBIndex >= 0 && m_iCurQBIndex <= 2 && GetTotal() > 0 && !m_pFrameQBuffer[m_iCurQBIndex]->IsEmpty())
		m_pFrameQBuffer[m_iCurQBIndex]->Reset();
#else
	m_pFrameQBuffer->Reset();
#endif
}

bool FrameBufferManager::IsFulled()
{
#if (_ENABLE_DUAL_BUFFER == 1)
	return m_pFrameQBuffer[m_iCurQBIndex]->IsFull();
#else
	return m_pFrameQBuffer->IsFull();
#endif
}

bool FrameBufferManager::IsReady()
{
#if (_ENABLE_DUAL_BUFFER == 1)
	int iMax = m_pFrameQBuffer[m_iCurQBIndex]->GetMax();
#else
	int iMax = m_pFrameQBuffer->GetMax();
#endif
	if(iMax > 0)
		return true;
	return false;
}

int FrameBufferManager::GetTotal()
{
#if (_ENABLE_DUAL_BUFFER == 1)
	return m_pFrameQBuffer[m_iCurQBIndex]->GetTotal();
#else
	return m_pFrameQBuffer->GetTotal();
#endif
}

int FrameBufferManager::GetMax()
{
#if (_ENABLE_DUAL_BUFFER == 1)
	return m_pFrameQBuffer[m_iCurQBIndex]->GetMax();
#else
	return m_pFrameQBuffer->GetMax();
#endif
}

void FrameBufferManager::ReAlloc(int iBufferLen)
{
#if (_ENABLE_DUAL_BUFFER == 1)
	int iNextIndex = (m_iCurQBIndex + 1) % 2;
	do
	{
		int iNewBufferLen = CalBuffLen(iBufferLen);//iBufferLen * 1.2;

		if(m_pTempFrameBuffer1)
			delete m_pTempFrameBuffer1;
		if(m_pTempFrameBuffer2)
			delete m_pTempFrameBuffer2;

		m_pTempFrameBuffer1 = new unsigned char[iNewBufferLen];
		m_pTempFrameBuffer2 = new unsigned char[iNewBufferLen];

		m_pFrameQBuffer[iNextIndex]->SetMax(m_pFrameQBuffer[m_iCurQBIndex]->GetMax());
		m_pFrameQBuffer[iNextIndex]->SetBufferSize(iNewBufferLen);

		//Copy data to new QBuffer.
		int iTotal = m_pFrameQBuffer[m_iCurQBIndex]->GetTotal();
		for(int i = 0;i < iTotal;i++)
		{
			int iLen = m_pFrameQBuffer[m_iCurQBIndex]->GetNext(m_pTempFrameBuffer2);
			if(iLen > 0)
				m_pFrameQBuffer[iNextIndex]->Add(m_pTempFrameBuffer2,iLen);
		}

		m_pFrameQBuffer[m_iCurQBIndex]->Free();
		m_iCurQBIndex = iNextIndex;

	}while(0);
#else
	do
	{
		int iNewBufferLen = CalBuffLen(iBufferLen);//iBufferLen * 1.2;

		if(m_pTempFrameBuffer1)
			delete m_pTempFrameBuffer1;
		if(m_pTempFrameBuffer2)
			delete m_pTempFrameBuffer2;

		m_pTempFrameBuffer1 = new unsigned char[iNewBufferLen];
		m_pTempFrameBuffer2 = new unsigned char[iNewBufferLen];

		if(m_pFrameQBuffer->GetTotal() > 0)
		{
			int i;
			int iTotal;
			QBufferManagerEx* pTempFrameQBuffer = new QBufferManagerEx;
			pTempFrameQBuffer->SetMax(m_pFrameQBuffer->GetMax());
			pTempFrameQBuffer->SetBufferSize(m_pFrameQBuffer->GetUnitSize());

			//Copy data to pTempFrameQBuffer.
			iTotal = m_pFrameQBuffer->GetTotal();
			for(i = 0;i < iTotal;i++)
			{
				int iLen = m_pFrameQBuffer->GetNext(m_pTempFrameBuffer2);
				if(iLen > 0)
					pTempFrameQBuffer->Add(m_pTempFrameBuffer2,iLen);
			}

			m_pFrameQBuffer->SetBufferSize(iNewBufferLen);

			//Restore data from pTempFrameQBuffer.
			iTotal = pTempFrameQBuffer->GetTotal();
			for(i = 0;i < iTotal;i++)
			{
				int iLen = pTempFrameQBuffer->GetNext(m_pTempFrameBuffer2);
				if(iLen > 0)
					m_pFrameQBuffer->Add(m_pTempFrameBuffer2,iLen);
			}
		}
		else
		{
			m_pFrameQBuffer->SetBufferSize(iNewBufferLen);
		}
	}while(0);
#endif
}

unsigned long FrameBufferManager::CalBuffLen(int iLen)
{
#if 1
	if(iLen < 1024 * 128)
		return 1024 * 128;
	else 
	{
		for(int i = 2;i < 256;i+=2)
		{
			if(iLen < 1024 * 128 * i)
				return 1024 * 128 * (i + 1);
		}
	}
#else
	if(iLen < 1024 * 8)
		return 1024 * 8;
	else 
	{
		for(int i = 2;i < 128*16;i+=2)
		{
			if(iLen < 1024 * 8 * i)
				return 1024 * 8 * (i + 4);
		}
	}
#endif
}
