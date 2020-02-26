//Designer: Noah
#include "stdafx.h"
#include "..\\Include\\SQList.h"
/*
int CreateSIQList(SIList** pList)
{
    SQList* pTemp;
	int iRet;

	if(pList == 0)
		return 0;

	pTemp = new SQList();
	if(pTemp == 0)
		return 0;

	iRet = pTemp->QueryInterface(IID_SIQLIST,(void**)pList);
	return iRet;
}
*/
SQList::SQList()
{
	m_iRef = 0;
	m_ulCurIndex = 0;
	m_bDoInit = true;
}

SQList::~SQList()
{
}

void* SQList::Get(unsigned long ulIndex)
{
	unsigned long ulTotal = SList::GetTotal();
	if(ulTotal <= ulIndex)
		return 0;

	bool bResetPos = false;
    if(m_bDoInit || m_ulCurIndex >= ulTotal || m_ulCurIndex >= ulIndex || m_ulCurIndex < 0)
	{
		m_bDoInit = false;
		SList::Begin();
		bResetPos = true;
		m_ulCurIndex = 0;
	}

	if(bResetPos)
	{
        for(unsigned long i = 0;i < ulIndex;i++)
			SList::Get(); 
	}
    else if(m_ulCurIndex + 1 == ulIndex)
		;
	else if(m_ulCurIndex + 1 < ulIndex)
	{
        unsigned long ulOffset = ulIndex - m_ulCurIndex;  
		for(unsigned long i = 0;i < ulOffset - 1;i++)
			SList::Get();
	}
    m_ulCurIndex = ulIndex;
	return SList::Get();
}

void SQList::Delete(unsigned long ulIndex)
{
	if(ulIndex == 0)
		SList::Begin();
	else
	    Get(ulIndex - 1);
	SList::Delete();
	m_bDoInit = true;
}

void SQList::Reset()
{
	m_ulCurIndex = 0;
	m_bDoInit = true;
    SList::Reset();
}

unsigned long SQList::GetCurIndex()
{
	return m_ulCurIndex;
}

/*
int SQList::QueryInterface(int iID,void** ppv)
{
	*ppv = 0;
	if(IID_SIQLIST == iID || IID_SIBASE == iID)
		*ppv = this;
	if(*ppv == 0)
		return 0;
	((SIBase*)*ppv)->IncRef();
	return 1;
}

int SQList::IncRef()
{
	return ++m_iRef;
}

int SQList::DecRef()
{
	if(0 != --m_iRef)
		return m_iRef;
	delete this;
	return 0;
}
*/