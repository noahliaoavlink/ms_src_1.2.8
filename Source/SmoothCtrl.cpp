#include "stdafx.h"
#include "../Include/SmoothCtrl.h"
#include "Mmsystem.h"

SmoothCtrl::SmoothCtrl()
{
	Reset();
}

SmoothCtrl::~SmoothCtrl()
{
}

void SmoothCtrl::SetAvgFPS(float fValue)
{
	if(fValue > 1)
		m_fAvgFPS = fValue;
}

bool SmoothCtrl::IsAllowed()
{
	DWORD dwCutTime = timeGetTime();

	if(m_ulLastTime == 0)
	{
		m_ulLastTime = dwCutTime;
		return true;
	}
	else
	{
		float fOffset = 1.0;
		if(m_DoSolw)
			fOffset = 1.25;
		
		float fInterval = (1000.0 / m_fAvgFPS) * fOffset;
		//float fInterval = (1000.0 / m_fAvgFPS) * 0.95;
		if(fInterval <= dwCutTime - m_ulLastTime)
		{
			m_ulLastTime = dwCutTime;
			return true;
		}
	}
	return false;
}

bool SmoothCtrl::IsBigHistory(float fValue)
{
	float fAvg = GetAvgHistory();
	
	if(fValue > fAvg)
		return true;
	return false;	
}

float SmoothCtrl::GetAvgHistory()
{
	float fTotal = 0.0;
	float fAvg = 0.0;
	for(int i = 0;i < _MAX_HISTORY;i++)
	{
		fTotal += m_fHistory[i];
	}
	fAvg = fTotal / _MAX_HISTORY;
	return fAvg;
}

void SmoothCtrl::ResetHistory()
{
	for(int i = 0;i < _MAX_HISTORY;i++)
		m_fHistory[i] = 0.0;
	m_iHistoryCount = 0;	
}

void SmoothCtrl::ComputeIntervalOffset(int iValue,int iMax)
{
	float fCurUsage = (float)iValue * 100 / (float)iMax;
	
	m_fHistory[m_iHistoryCount % _MAX_HISTORY] = fCurUsage;
	m_iHistoryCount++;
	
	float fAvg = GetAvgHistory();
	
	if(IsBigHistory(fCurUsage) && fAvg > 70 && m_iHistoryCount >= _MAX_HISTORY)
	{
		m_iIntervalOffset--;
		ResetHistory();
	}
		
	if(fAvg < 20 && m_iIntervalOffset < 0 && m_iHistoryCount >= _MAX_HISTORY)	
	{
		m_iIntervalOffset += 1;
		//m_iHistoryCount = 0;
	}	
		
	//return m_iIntervalOffset;	
}

void SmoothCtrl::EnableSolw(bool bEnable)
{
	m_DoSolw = bEnable;
}

void SmoothCtrl::Reset()
{
	m_DoSolw = false;
	m_fAvgFPS = 30.0;
	m_ulLastTime = 0;

	m_iIntervalOffset = 0;
	ResetHistory();
}