#include "stdafx.h"
#include "../Include/MathCommon.h"

Average::Average()
{
	m_iMax = _MAX_ITEMS;
	Reset();
}

Average::~Average()
{
}

void Average::Reset()
{
	m_iTotalOfItems = 0;
	for(int i = 0;i < _MAX_ITEMS;i++)
		m_fValue[i] = 0;
	m_fAvgValue = 0.0;
	m_ulCount = 0;
}

void Average::SetMax(int iValue)
{
	m_iMax = iValue;
}

void Average::Calculate(float fValue)
{
#if 0
	m_fValue[m_ulCount % _MAX_ITEMS] = fValue;

	if(m_iTotalOfItems < _MAX_ITEMS)
		m_iTotalOfItems++;
	
	float fTotalOfValues = 0.0;
	for(int i = 0;i < m_iTotalOfItems;i++)
		fTotalOfValues += m_fValue[i];

	m_fAvgValue = (float)fTotalOfValues / (float)m_iTotalOfItems;

	m_ulCount++;
#else
	m_fValue[m_ulCount % m_iMax] = fValue;

	if (m_iTotalOfItems < m_iMax)
		m_iTotalOfItems++;

	float fTotalOfValues = 0.0;
	for (int i = 0; i < m_iTotalOfItems; i++)
		fTotalOfValues += m_fValue[i];

	m_fAvgValue = (float)fTotalOfValues / (float)m_iTotalOfItems;

	m_ulCount++;
#endif
}

float Average::GetAverage()
{
	return m_fAvgValue;
}

int Average::GetCount()
{
	return m_ulCount;
}

float Average::GetAverage(int iLevel)
{
	switch(iLevel)
	{
		case 0:
			return m_fAvgValue;
			break;
		case 1:
			return m_fAvgValue * 1.25;
			break;
		case 2:
			return m_fAvgValue * 1.65;
			break;
		case 3:
			return m_fAvgValue * 2;
			break;
		case 4:
			return m_fAvgValue * 4;
			break;
		case 5:
			return m_fAvgValue * 6;
			break;
		case 6:
			return m_fAvgValue * 8;
			break;
		case 7:
			return m_fAvgValue * 10;
			break;
		case 8:
			return m_fAvgValue * 12;
			break;
		case 9:
			return m_fAvgValue * 14;
			break;
		case 10:
			return m_fAvgValue * 16;
			break;
	}
}
