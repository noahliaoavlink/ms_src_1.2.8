#include "stdafx.h"
#include "FrameRateCtrl.h"

FrameRateCtrl::FrameRateCtrl()
{
	m_fInterval = 0.0;
}

FrameRateCtrl::~FrameRateCtrl()
{
}

void FrameRateCtrl::SetInterval(float fInterval)
{
	/*
	if((int)fInterval == 66) //fps 15.0
		m_fInterval = fInterval * 0.996;
	else if((int)fInterval == 40) //fps 25.0
		m_fInterval = fInterval * 0.991;
	else if ((int)fInterval == 33) // 29.x ~ 30
		m_fInterval = fInterval * 0.99;
	else
		m_fInterval = fInterval * 0.993;
	*/
	
	if ((int)fInterval == 66) //fps 15.0
		m_fInterval = fInterval * 0.996;
	else if ((int)fInterval == 40) //fps 25.0
		m_fInterval = fInterval * 0.993;
	else if ((int)fInterval == 33) // 29.x ~ 30
		m_fInterval = fInterval * 0.991;
	else
		m_fInterval = fInterval * 0.993;
	//m_fInterval = fInterval;
}

int FrameRateCtrl::GetInterval(int iCount)
{
	int iIntegerInterval = m_fInterval;
	if(iCount >= 2)
	{
		float fRemainder = m_fInterval - iIntegerInterval;

		int iFrontOffset = fRemainder * (iCount - 1);
		int iCurOffset = fRemainder * iCount;

		if(iCurOffset > iFrontOffset)
			iIntegerInterval += (iCurOffset - iFrontOffset);
	}
	return iIntegerInterval;
}