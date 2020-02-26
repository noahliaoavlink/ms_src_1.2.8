#include "stdafx.h"
#include "SyncPBCtrl.h"

SyncPBCtrl::SyncPBCtrl()
{
	m_iStatus = PBCS_IDLE;
	m_iJoinCount = 0;
}

SyncPBCtrl::~SyncPBCtrl()
{
}

void SyncPBCtrl::Init()
{
	Reset();
}

void SyncPBCtrl::Start()
{
	ThreadBase::Start();
}

void SyncPBCtrl::Stop()
{
	ThreadBase::Stop();
}

int SyncPBCtrl::GetStatus()
{
	return m_iStatus;
}

void SyncPBCtrl::SetPanelStatus(int iPanelIndex,int iStatus)
{
#if 0
	if (m_iStatus != PBCS_IDLE)
	{
		m_Panels[iPanelIndex].iStatus = iStatus;
	}
#else
	if(iStatus == PSS_UPDATED && m_Panels[iPanelIndex].iStatus == PSS_UPDATING)
		m_Panels[iPanelIndex].iStatus = iStatus;
	else if(iStatus == PSS_PRESENTED && m_Panels[iPanelIndex].iStatus == PSS_PRESENTING)
		m_Panels[iPanelIndex].iStatus = iStatus;

#endif
}

int SyncPBCtrl::GetPanelStatus(int iPanelIndex)
{
	return m_Panels[iPanelIndex].iStatus;
}

void SyncPBCtrl::SetSourceCount(int iPanelIndex, int iCount)
{
	m_Panels[iPanelIndex].iSourceCount = iCount;
}

void SyncPBCtrl::SetPanelSourcePos(int iPanelIndex, int iSourceIndex,double dPos)
{
	if (iSourceIndex != -1)
	{
		if(m_Panels[iPanelIndex].iStatus == PSS_UPDATING)
			m_Panels[iPanelIndex].dPos[iSourceIndex] = dPos;
	}
}

void SyncPBCtrl::CheckSourceCount()
{
	m_iJoinCount = 0;
	for (int i = 0; i < _MAX_PANELS; i++)
	{
		if (m_Panels[i].iSourceCount > 0)
		{
			m_bIsWorking[i] = true;
			m_iJoinCount++;
		}
	}
}

void SyncPBCtrl::UpdateStatus()
{
	switch (m_iStatus)
	{
		case PBCS_IDLE:
			CheckSourceCount();
			if (m_iJoinCount > 0)
				m_iStatus = PBCS_NORMAL;
			break;
		case PBCS_NORMAL:
			Normal();
			break;
		case PBCS_UPDATING:
			Updating();
			break;
		case PBCS_UPDATED:
			Updated();
			break;
		case PBCS_PRESENTING:
			Presenting();
			break;
		case PBCS_PRESENTED:
			m_iStatus = PBCS_IDLE;
			break;
	}
}

void SyncPBCtrl::Normal()
{
	for (int i = 0; i < _MAX_PANELS; i++)
	{
		if (m_bIsWorking[i])
			m_Panels[i].iStatus = PSS_UPDATING;
	}

	m_iStatus = PBCS_UPDATING;
}

void SyncPBCtrl::Updating()
{
	int iUpdatedCount = 0;
	double dMin = 0.0;
	double dMax = 0.0;

	for (int i = 0; i < _MAX_PANELS; i++)
	{
		if (m_bIsWorking[i] && m_Panels[i].iStatus == PSS_UPDATED)
			iUpdatedCount++;
	}

	if (iUpdatedCount == m_iJoinCount)
	{
		//m_Panels[iPanelIndex].dPos[0];
		int iCount = 0;
		int iSyncCount = 0;
		double dPos = 0.0;
		
		for (int i = 0; i < _MAX_PANELS; i++)
		{
			if (m_bIsWorking[i])
			{
				if (iCount == 0)
				{
					dPos = m_Panels[i].dPos[0];
					dMin = dPos;
					dMax = dPos;
					iSyncCount++;
				}
				else
				{
					if (dPos == m_Panels[i].dPos[0])
						iSyncCount++;
					else
					{
						if (m_Panels[i].dPos[0] < dMin)
							dMin = m_Panels[i].dPos[0];
						if (m_Panels[i].dPos[0] > dMax)
							dMax = m_Panels[i].dPos[0];
					}
				}
				iCount++;
			}
		}

		if (iSyncCount == m_iJoinCount)
		{
			m_iStatus = PBCS_UPDATED;
		}
		else
		{
			
			if (dMax > dMin + 1)
				m_iStatus = PBCS_UPDATED;
			else 
			{
				for (int i = 0; i < _MAX_PANELS; i++)
				{
					if (m_bIsWorking[i] && m_Panels[i].dPos[0] < dMax)
					{
						m_Panels[i].iStatus = PSS_UPDATING;
					}
				}
			}

		}
	}
}

void SyncPBCtrl::Updated()
{
	for (int i = 0; i < _MAX_PANELS; i++)
	{
		if (m_bIsWorking[i])
		{
			m_Panels[i].iStatus = PSS_PRESENTING;
		}
	}

	m_iStatus = PBCS_PRESENTING;
}

void SyncPBCtrl::Presenting()
{
	int iPresentedCount = 0;
	for (int i = 0; i < _MAX_PANELS; i++)
	{
		if (m_bIsWorking[i] && m_Panels[i].iStatus == PSS_PRESENTED)
			iPresentedCount++;
	}

	if (iPresentedCount == m_iJoinCount)
	{
		m_iStatus = PBCS_PRESENTED;
	}
}

void SyncPBCtrl::Reset()
{
	for (int i = 0; i < _MAX_PANELS; i++)
	{
		//		m_Panels[i].iIndex = i;
		m_Panels[i].iSourceCount = 0;
		m_Panels[i].iStatus = PSS_IDLE;

		for (int j = 0; j < 10; j++)
			m_Panels[i].dPos[j] = 0.0;

		m_bIsWorking[i] = false;
	}

	m_iStatus = PBCS_IDLE;
	m_iJoinCount = 0;
}

void SyncPBCtrl::ThreadEvent()
{
	ThreadBase::Lock();

	UpdateStatus();
	Sleep(1);
	ThreadBase::Unlock();
}
