#include "stdafx.h"
#include "../Include/SQQueue.h"

SQQueue::SQQueue()
{
	m_pQBuffer = new QBufferManagerEx;
	m_pQBuffer->SetMax(0);

	m_pTempBuffer = 0;
}

SQQueue::~SQQueue()
{
	if (m_pQBuffer)
		delete m_pQBuffer;

	delete m_pTempBuffer;
}

void SQQueue::Alloc(int iBufferSize, int iMax)
{
	int iNewBufferSize = iBufferSize + 64;

	if (m_pTempBuffer)
	{
		delete m_pTempBuffer;
		m_pTempBuffer = 0;
	}
	
	m_pTempBuffer = new unsigned char[iNewBufferSize];

	m_pQBuffer->SetMax(iMax);
	m_pQBuffer->SetBufferSize(iNewBufferSize);
}

bool SQQueue::Add(unsigned char* pBuffer,int iLen)
{
	bool bRet = m_pQBuffer->Add(pBuffer,iLen);
	return bRet;
}

unsigned char* SQQueue::GetNext()
{
	m_iLen = m_pQBuffer->GetNext(m_pTempBuffer);
	return m_pTempBuffer;
}

int SQQueue::GetLength()
{
	return m_iLen;
}

void SQQueue::Reset()
{
	m_pQBuffer->Reset();
}

bool SQQueue::IsFulled()
{
	return m_pQBuffer->IsFull();
}

int SQQueue::GetTotal()
{
	return m_pQBuffer->GetTotal();
}

int SQQueue::GetMax()
{
	return m_pQBuffer->GetMax();
}
