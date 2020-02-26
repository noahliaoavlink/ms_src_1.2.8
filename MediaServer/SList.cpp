//Designer: Noah
#include "stdafx.h"
#include "SList.h"

/*
int CreateSIList(SIList** pList)
{
	SList* pTemp;
	int iRet;

	if(pList == 0)
		return 0;

	pTemp = new SList();
	if(pTemp == 0)
		return 0;

	iRet = pTemp->QueryInterface(IID_SILIST,(void**)pList);
	return iRet;
}
*/

SList::SList()
{
//	m_iRef = 0;

	m_pTail = new BList;
	m_pTail->pNext = 0;
	m_pTail->pData = 0;
	m_pHead = new BList;
	m_pHead->pNext = m_pTail;
	m_pHead->pData = 0;

	m_pLast = m_pHead;
	m_ulTotal = 0;
	m_bRemoveData = true;
	m_bReady = true;
}

SList::~SList()
{
	//delete m_pHead->pData;
	//delete m_pTail->pData;

    
	pTemp = m_pHead;
	pTemp = pTemp->pNext;
	while(pTemp->pNext != 0)
	{
        pList = pTemp;
		pTemp = pTemp->pNext;
		if(m_bRemoveData)
		    delete pList->pData;
		delete pList;
	}

	delete m_pTail;
	delete m_pHead;
	//delete m_pTail; 
}

void SList::Add(void* pData)
{
	BList* pNew = new BList;
	pNew->pData = pData;
	
	pNew->pNext = m_pTail;
	m_pLast->pNext = pNew;
	m_pLast = m_pLast->pNext;
	m_ulTotal++;
}

void* SList::Get()
{
	BList* pList = m_pCurrent->pNext;
	if(!pList || !m_bReady)
		return 0;
	    m_pData = pList->pData;
        m_pCurrent = pList; 
	    return m_pData;
}

void SList::Begin()
{
    m_pCurrent = m_pHead;
}

bool SList::Insert(unsigned long ulIndex,void* pData)
{
#if 0
	if(ulIndex == m_ulTotal)
	{
        Add(pData);
		return true;
	}

	if(ulIndex > m_ulTotal)
		return false;

	BList* pNew = new BList;
	pNew->pData = pData;

	m_ulTotal++;
	pTemp = m_pHead;
//	pTemp = pTemp->pNext;
	for(unsigned long i = 0;i < m_ulTotal;i++)
	{
        pList = pTemp;
		if(i == ulIndex)
		{
			pNew->pNext = pList->pNext;
            pList->pNext = pNew;
			break;
		}
		pTemp = pTemp->pNext;
	}
	Begin();
	return true;
#else
	if(ulIndex == m_ulTotal)
	{
        Add(pData);
		return true;
	}

	if(ulIndex > m_ulTotal)
		return false;

	BList* pNew = new BList;
	pNew->pData = pData;

	m_ulTotal++;
	pTemp = m_pHead;
//	pTemp = pTemp->pNext;
	for(unsigned long i = 0;i < m_ulTotal;i++)
	{
        pList = pTemp;
		if(i == ulIndex)
		{
			pNew->pNext = pList->pNext;
            pList->pNext = pNew;

			m_pCurrent = pList;
			break;
		}
		pTemp = pTemp->pNext;
	}
	return true;
#endif
}

unsigned long SList::GetTotal()
{
	return m_ulTotal;
}

void SList::Reset()
{
	m_bReady = false;
    pTemp = m_pHead;
	pTemp = pTemp->pNext;
	while(pTemp->pNext != 0)
	{
        pList = pTemp;
		pTemp = pTemp->pNext;
		if(m_bRemoveData && pList->pData)
		   delete pList->pData;
		delete pList;
	}

	m_pHead->pNext = m_pTail;

	m_pLast = m_pHead;
    m_ulTotal = 0;
    m_bReady = true;
}

void SList::Delete()
{
	BList* pList = m_pCurrent->pNext;
	m_pCurrent->pNext = pList->pNext;
	if(m_bRemoveData)
	    delete pList->pData;
	delete pList;
	m_ulTotal--;

	if(m_ulTotal == 0)
	{
		m_pLast = m_pHead;
		m_pHead->pNext = m_pTail;
	}
	else
	{
		//reset m_pLast pos 
		Begin();
		for(unsigned long i = 0;i < m_ulTotal;i++)
			Get();
	    m_pLast = m_pCurrent;	
	}
}

void SList::EnableRemoveData(bool bEnable)
{
	m_bRemoveData = bEnable;
}

bool SList::RemoveDataIsEnable()
{
	return m_bRemoveData;
}

/*
int SList::QueryInterface(int iID,void** ppv)
{
	*ppv = 0;
	if(IID_SILIST == iID || IID_SIBASE == iID)
		*ppv = this;
	if(*ppv == 0)
		return 0;
	((SIBase*)*ppv)->IncRef();
	return 1;
}

int SList::IncRef()
{
	return ++m_iRef;
}

int SList::DecRef()
{
	if(0 != --m_iRef)
		return m_iRef;
	delete this;
	return 0;
}
*/