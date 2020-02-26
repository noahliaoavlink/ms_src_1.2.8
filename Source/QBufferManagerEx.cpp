#include "stdafx.h"
#include "../Include/QBufferManagerEx.h"

#define _VMEM 1 

QBufferManagerEx::QBufferManagerEx()
{
	m_iHeaderIndex = 0;
	m_iLastIndex = 0;
	m_iMax = 50;//30;
	m_ulID = 0;

	m_iReadIndex = 0;
	m_iNextReadIndex = 0;
	m_iWriteIndex = 0;
	m_iNextWriteIndex = 0;
//	m_pBuffer = 0;

	m_iUnitSize = 0;

	m_lFrameCount = 0;

	for(int i = 0;i < m_iMax;i++)
		m_RBMItems[i].pBuffer = 0;

}

QBufferManagerEx::~QBufferManagerEx()
{
	Free();
}

void QBufferManagerEx::Free()
{
#if _VMEM
		for(int i = 0;i < m_iMax;i++)
		{
			if(m_RBMItems[i].pBuffer)
				VirtualFree(m_RBMItems[i].pBuffer,0, MEM_RELEASE);
		}
#else
		for(int i = 0;i < m_iMax;i++)
		{
			if(m_RBMItems[i].pBuffer)
				free(m_RBMItems[i].pBuffer);
		}
#endif
}

void QBufferManagerEx::Lock()
{
#if _VMEM
	if(m_RBMItems[m_iWriteIndex].pBuffer)
		VirtualLock(m_RBMItems[m_iWriteIndex].pBuffer,m_iUnitSize);
#else
#endif
}

void QBufferManagerEx::Unlock()
{
#if _VMEM
	if(m_RBMItems[m_iWriteIndex].pBuffer)
		VirtualUnlock(m_RBMItems[m_iWriteIndex].pBuffer,m_iUnitSize);
#else
#endif
}

bool QBufferManagerEx::IsFull()
{
#if 0
	m_iNextReadIndex = (m_iReadIndex + 1) % m_iMax;
	if(m_iNextReadIndex == m_iWriteIndex)
		return true;
	return false;
#else
	if(m_lFrameCount >= m_iMax)
		return true;
	return false;
#endif
}

bool QBufferManagerEx::IsEmpty()
{
#if 0
	if(m_iWriteIndex == m_iReadIndex)
		return true;
	return false;
#else
	if(m_lFrameCount == 0)
		return true;
	return false;
#endif
}

void QBufferManagerEx::SetBufferSize(int iSize)
{
	m_iUnitSize = iSize + 128;
	//m_iTotal = 0;
	
	int iNewSize = (iSize + 128) * m_iMax;
	if(m_iBufferSize != iNewSize)
	{
		m_iReadIndex = 0;
		m_iWriteIndex = 0;
		m_lFrameCount = 0;

#if _VMEM
		for(int i = 0;i < m_iMax;i++)
			m_RBMItems[i].pBuffer = (unsigned char *)VirtualAlloc(0,m_iUnitSize,MEM_COMMIT, PAGE_READWRITE);
#else
		for(int i = 0;i < m_iMax;i++)
			m_RBMItems[i].pBuffer = (unsigned char *)malloc(m_iUnitSize);
#endif
	}
}

void QBufferManagerEx::SetMax(int iValue)
{
	if(m_iMax != iValue)
		Free();

	m_iMax = iValue;
}

int QBufferManagerEx::GetMax()
{
	return m_iMax;
}

int QBufferManagerEx::GetUnitSize()
{
	return m_iUnitSize - 128;
}

int QBufferManagerEx::Insert(int iIndex,unsigned char* pBuffer,int iLen)
{
		if(iLen > m_iUnitSize || iLen < 0 || iIndex < 0 || iIndex >= m_iMax)
		{
			return -1;
		}

#if 0
		memcpy(m_RBMItems[iIndex].pBuffer,&m_ulID,4);
		memcpy((unsigned char*)m_RBMItems[iIndex].pBuffer + 4,pBuffer,iLen);
#else
		try
		{
			memcpy((unsigned char*)m_RBMItems[iIndex].pBuffer,pBuffer,iLen);
		}
		catch (...)
		{
			char szMsg[512];
			sprintf(szMsg,"HEError - QBufferManagerEx::Insert memcpy cause exception !!\n");
			OutputDebugStringA(szMsg);

			return -3;
		}
#endif
		m_RBMItems[iIndex].iSize = iLen;// + 4;

		return 1;
}

bool QBufferManagerEx::Add(void* pBuffer,int iSize)
{
	if(IsFull())
		return false;
	else
	{
		Lock();
		int iRet = Insert(m_iWriteIndex,(unsigned char *)pBuffer,iSize);
		if(iRet < 0)
			return false;
		Unlock();
		m_iWriteIndex = (m_iWriteIndex + 1) % m_iMax;

		m_ulID++;
		if(m_ulID > 4000000000)
			m_ulID = 0;

		m_lFrameCount++;
	}
	
	return false;
}

int QBufferManagerEx::GetNext(unsigned char* pBuffer)
{
	int iLen = 0;
	if(!IsEmpty())
	{
		Lock();
		if(m_RBMItems[m_iReadIndex].iSize < 0)
		{
			Unlock();
			m_lFrameCount--;
			return 0;
		}
		memcpy(pBuffer,m_RBMItems[m_iReadIndex].pBuffer,m_RBMItems[m_iReadIndex].iSize);
		Unlock();
		iLen = m_RBMItems[m_iReadIndex].iSize;
		m_iReadIndex = (m_iReadIndex + 1) % m_iMax;

		m_lFrameCount--;
		return iLen;
	}
	return 0;
}

int QBufferManagerEx::GetTotal()
{
#if 0
	int iTotal;
	int iNum = m_iWriteIndex - m_iReadIndex;
	if(iNum < 0)
		iTotal = m_iMax - iNum;
	else
		iTotal = iNum;
	return iTotal;
#else
	return m_lFrameCount;
#endif
}

void QBufferManagerEx::Reset()
{
	m_iReadIndex = 0;
	m_iWriteIndex = 0;
	m_lFrameCount = 0;
}
