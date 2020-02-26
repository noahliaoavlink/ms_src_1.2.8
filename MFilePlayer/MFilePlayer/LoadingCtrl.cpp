#include "stdafx.h"
#include "LoadingCtrl.h"

LoadingCtrl::LoadingCtrl()
{
	m_fDataFPS = 30.0;
	m_fProcessFPS = 30.0;
	m_iFrameCount = 0;
	m_iQ = 1;
	m_fR = 0.0;
	m_fR_Num = 0.0;
}

LoadingCtrl::~LoadingCtrl()
{
}

void LoadingCtrl::SetDataFPS(float fFPS)
{
	m_fDataFPS = fFPS;
}

void LoadingCtrl::SetProcessFPS(float fFPS)
{
	char szMsg[256];
#if 0
	if(fFPS < (m_fProcessFPS + 0.5))
		m_fProcessFPS = fFPS - 1.0;
		//m_fProcessFPS = fFPS - 0.5;
#else
	if (fFPS < m_fProcessFPS)
	{
		sprintf(szMsg, "#LoadingCtrl# LoadingCtrl::SetProcessFPS: %3.2F -> %3.2F\n", m_fProcessFPS, fFPS);
		OutputDebugStringA(szMsg);

		m_fProcessFPS = fFPS;
	}
#endif
}

void LoadingCtrl::Compute()
{
	float fQ = m_fDataFPS / m_fProcessFPS;
	m_iQ = fQ;
	m_fR = fQ - m_iQ;
}

bool LoadingCtrl::IsAllowed()
{
	m_iFrameCount++;
	if (m_fProcessFPS >= m_fDataFPS)
		return true;
	else
	{
		if (m_iQ == 1 && m_fR > 0)
		{
			float fCurRNum = m_fR_Num + m_fR;
			if (fCurRNum > 1)
			{
				m_fR_Num = fCurRNum - 1;
				return false;
			}
			else
			{
				m_fR_Num = fCurRNum;
				return true;
			}
		}
		else
		{
			if (m_iQ > 1)
			{
				if (m_iFrameCount % m_iQ == m_iQ - 1)
					return true;
				else
				{
					float fCurRNum = m_fR_Num + m_fR;
					if (fCurRNum > 1)
					{
						m_fR_Num = fCurRNum - 1;
						return false;
					}
					else
					{
						m_fR_Num = fCurRNum;
						return true;
					}
				}
			}
		}
	}
	return false;
}

void LoadingCtrl::Reset()
{
	m_fDataFPS = 25.0;
	m_fProcessFPS = 25.0;
	m_iFrameCount = 0;
	m_iQ = 1;
	m_fR = 0.0;
	m_fR_Num = 0.0;
}
