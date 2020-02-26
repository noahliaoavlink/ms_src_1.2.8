#include "stdafx.h"
#include "TrackContent.h"

TrackContent::TrackContent()
{
	strcpy(m_szName, "");
	m_iType = NT_NUMBER;

	m_pNodeList = new SQList;
	m_pNodeList->EnableRemoveData(false);
}

TrackContent::~TrackContent()
{
	if (m_pNodeList)
	{
		int iTotal = m_pNodeList->GetTotal();
		for (int i = 0; i < iTotal; i++)
		{
			if (m_iType == NT_NUMBER)
			{
				NumberNode* pCurNode = (NumberNode*)m_pNodeList->Get(i);
				if (pCurNode)
					delete pCurNode;
			}
			else if (m_iType == NT_MEDIA)
			{
				MediaNode* pCurNode = (MediaNode*)m_pNodeList->Get(i);
				if (pCurNode)
					delete pCurNode;
			}
		}
		m_pNodeList->Reset();
		delete m_pNodeList;
	}
}

void TrackContent::SetName(char* szData)
{
	strcpy(m_szName, szData);
}

char* TrackContent::GetName()
{
	return m_szName;
}

void TrackContent::SetType(int iValue)
{
	m_iType = iValue;
}

int TrackContent::GetType()
{
	return m_iType;
}

int TrackContent::Add(float fTime, int iValue, int iLinkType , int iAngle)
{
	if (m_iType == NT_NUMBER)
	{
		int iIndex = FindNode(fTime);
		if (iIndex == -1)
		{
			NumberNode* pNewNode = new NumberNode;
			pNewNode->fTime = fTime;
			pNewNode->iValue = iValue;
			pNewNode->iLinkType = iLinkType;
			pNewNode->iAngle = iAngle;
			m_pNodeList->Add(pNewNode);
			return 1;
		}
		else
			return -2;
	}
	return -1;
}

#if _ENABLE_MEDIA_LEN
int TrackContent::Add(float fTime, char* szData,float fLen)
#else
int TrackContent::Add(float fTime, char* szData)
#endif
{
	if (m_iType == NT_MEDIA)
	{
		int iIndex = FindNode(fTime);
		if (iIndex == -1)
		{
			MediaNode* pNewNode = new MediaNode;
			pNewNode->fTime = fTime;
			strcpy(pNewNode->szPath, szData);
#if _ENABLE_MEDIA_LEN
			pNewNode->fLen = fLen;
#endif
			m_pNodeList->Add(pNewNode);
			return 1;
		}
		else
			return -2;
	}
	return -1;
}

int TrackContent::Add(float fTime, int iType,char* szName,char* szTargetName, int iJumpType)
{
	if (m_iType == NT_COMMAND)
	{
		int iIndex = FindNode(fTime);
		if (iIndex == -1)
		{
			CommandNode* pNewNode = new CommandNode;
			pNewNode->fTime = fTime;
			strcpy(pNewNode->szName, szName);
			strcpy(pNewNode->szTargetName, szTargetName);
			pNewNode->iType = iType;
			pNewNode->iJumpType = iJumpType;
			m_pNodeList->Add(pNewNode);
			return 1;
		}
		else
			return -2;
	}
	return -1;
}

int TrackContent::Add(float fTime, int iEffectIndex)
{
	if (m_iType == NT_EFFECT_INDEX)
	{
		int iIndex = FindNode(fTime);
		if (iIndex == -1)
		{
			EffectIndexNode* pNewNode = new EffectIndexNode;
			pNewNode->fTime = fTime;
			pNewNode->iIndex = iEffectIndex;
			m_pNodeList->Add(pNewNode);
			return 1;
		}
		else
			return -2;
	}
	return -1;
}

int TrackContent::Add(float fTime, int iEffectIndex, char* szName,float fDuration,int iLevel,int* iParam)
{
	if (m_iType == NT_EFFECT)
	{
		int iIndex = FindNode(fTime);
		if (iIndex == -1)
		{
			EffectNode* pNewNode = new EffectNode;
			pNewNode->fTime = fTime;
			pNewNode->iIndex = iEffectIndex;
			pNewNode->fDuration = fDuration;
			pNewNode->iLevel = iLevel;
			strcpy(pNewNode->szName, szName);
			for (int i = 0; i < 5; i++)
				pNewNode->iParam[i] = iParam[i];
			m_pNodeList->Add(pNewNode);
			return 1;
		}
		else
			return -2;
	}
	return -1;
}

void TrackContent::Delete(float fTime)
{
	int iIndex = FindNode(fTime);
	if (iIndex != -1)
	{
		if (m_iType == NT_NUMBER)
		{
			NumberNode* pCurNode = (NumberNode*)m_pNodeList->Get(iIndex);
			if (pCurNode)
				delete pCurNode;
		}
		else if (m_iType == NT_MEDIA)
		{
			MediaNode* pCurNode = (MediaNode*)m_pNodeList->Get(iIndex);
			if (pCurNode)
				delete pCurNode;
		}
		else if (m_iType == NT_COMMAND)
		{
			CommandNode* pCurNode = (CommandNode*)m_pNodeList->Get(iIndex);
			if (pCurNode)
				delete pCurNode;
		}
		else if (m_iType == NT_EFFECT_INDEX)
		{
			EffectIndexNode* pCurNode = (EffectIndexNode*)m_pNodeList->Get(iIndex);
			if (pCurNode)
				delete pCurNode;
		}
		else if (m_iType == NT_EFFECT)
		{
			EffectNode* pCurNode = (EffectNode*)m_pNodeList->Get(iIndex);
			if (pCurNode)
				delete pCurNode;
		}

		m_pNodeList->Delete(iIndex);
	}
}
/*
int TrackContent::Edit(float fTime, int iValue, int iLinkType, int iAngle)
{
	if (m_iType == NT_NUMBER)
	{
		int iIndex = FindNode(fTime);
		if (iIndex == -1)
		{
			return -2;
		}
		else
		{
			NumberNode* pCurNode = (NumberNode*)m_pNodeList->Get(iIndex);
			if (pCurNode)
			{
				pCurNode->iValue = iValue;
				pCurNode->iLinkType = iLinkType;
				pCurNode->iAngle = iAngle;
			}
			return 1;
		}
	}
	return -1;
}

int TrackContent::Edit(float fTime, char* szData)
{
	if (m_iType == NT_MEDIA)
	{
		int iIndex = FindNode(fTime);
		if (iIndex == -1)
		{
			return -2;
		}
		else
		{
			MediaNode* pCurNode = (MediaNode*)m_pNodeList->Get(iIndex);
			if (pCurNode)
				strcpy(pCurNode->szPath, szData);
			return 1;
		}
	}
	return -1;
}
*/

int TrackContent::FindNode(float fTime)
{
	if (m_pNodeList->GetTotal() == 0)
		return -1;

	for (int i = 0; i < m_pNodeList->GetTotal(); i++)
	{
		if (m_iType == NT_NUMBER)
		{
			NumberNode* pCurNode = (NumberNode*)m_pNodeList->Get(i);

			if(pCurNode && fTime >= pCurNode->fTime - _TIME_CHECK_RANGE2 && fTime <= pCurNode->fTime + _TIME_CHECK_RANGE2)
			//if (pCurNode && pCurNode->fTime == fTime)
				return i;
		}
		else if (m_iType == NT_MEDIA)
		{
			MediaNode* pCurNode = (MediaNode*)m_pNodeList->Get(i);

#if _ENABLE_MEDIA_LEN
			if (pCurNode && fTime >= pCurNode->fTime && fTime <= pCurNode->fTime + pCurNode->fLen + 0.02)
#else
			if (pCurNode && fTime >= pCurNode->fTime && fTime <= pCurNode->fTime + _TIME_CHECK_RANGE)
#endif
			//if (pCurNode && pCurNode->fTime == fTime)
				return i;
		}
		else if (m_iType == NT_COMMAND)
		{
			CommandNode* pCurNode = (CommandNode*)m_pNodeList->Get(i);
			if (pCurNode && fTime >= pCurNode->fTime && fTime <= pCurNode->fTime + _TIME_CHECK_RANGE3)
				return i;
		}
		else if (m_iType == NT_EFFECT_INDEX)
		{
			EffectIndexNode* pCurNode = (EffectIndexNode*)m_pNodeList->Get(i);
			if (pCurNode && fTime >= pCurNode->fTime && fTime <= pCurNode->fTime + _TIME_CHECK_RANGE4)
				return i;
		}
		else if (m_iType == NT_EFFECT)
		{
			EffectNode* pCurNode = (EffectNode*)m_pNodeList->Get(i);
			if (pCurNode && fTime >= pCurNode->fTime && fTime <= pCurNode->fTime + pCurNode->fDuration + 0.02)
				return i;
		}
	}
	return -1;
}

int TrackContent::FindNode_Backward(float fTime)
{
	if (m_pNodeList->GetTotal() == 0)
		return -1;

	//for (int i = 0; i < m_pNodeList->GetTotal(); i++)
	for (int i = m_pNodeList->GetTotal() - 1; i >= 0; i--)
	{
		if (m_iType == NT_NUMBER)
		{
			NumberNode* pCurNode = (NumberNode*)m_pNodeList->Get(i);

			if (pCurNode && fTime >= pCurNode->fTime - _TIME_CHECK_RANGE2 && fTime <= pCurNode->fTime + _TIME_CHECK_RANGE2)
				//if (pCurNode && pCurNode->fTime == fTime)
				return i;
		}
		else if (m_iType == NT_MEDIA)
		{
			MediaNode* pCurNode = (MediaNode*)m_pNodeList->Get(i);

#if _ENABLE_MEDIA_LEN
			if (pCurNode && fTime >= pCurNode->fTime && fTime <= pCurNode->fTime + pCurNode->fLen + 0.02)
#else
			if (pCurNode && fTime >= pCurNode->fTime && fTime <= pCurNode->fTime + _TIME_CHECK_RANGE)
#endif
				//if (pCurNode && pCurNode->fTime == fTime)
				return i;
		}
		else if (m_iType == NT_COMMAND)
		{
			CommandNode* pCurNode = (CommandNode*)m_pNodeList->Get(i);
			if (pCurNode && fTime >= pCurNode->fTime && fTime <= pCurNode->fTime + _TIME_CHECK_RANGE3)
				return i;
		}
		else if (m_iType == NT_EFFECT_INDEX)
		{
			EffectIndexNode* pCurNode = (EffectIndexNode*)m_pNodeList->Get(i);
			if (pCurNode && fTime >= pCurNode->fTime && fTime <= pCurNode->fTime + _TIME_CHECK_RANGE4)
				return i;
		}
		else if (m_iType == NT_EFFECT)
		{
			EffectNode* pCurNode = (EffectNode*)m_pNodeList->Get(i);
			if (pCurNode && fTime >= pCurNode->fTime && fTime <= pCurNode->fTime + pCurNode->fDuration + 0.02)
				return i;
		}
	}
	return -1;
}

void* TrackContent::GetNodeData(int iIndex)
{
	if (m_pNodeList->GetTotal() > iIndex)
		return m_pNodeList->Get(iIndex);
	return 0;
}

int TrackContent::GetTotalOfNodes()
{
	return m_pNodeList->GetTotal();
}

float TrackContent::GetLastTime()
{
	m_fTmpTime = 0.0;
	if (m_pNodeList->GetTotal() == 0)
		return 0.0;

#if 0

	int iIndex = m_pNodeList->GetTotal() - 1;
	if (m_iType == NT_NUMBER)
	{
		NumberNode* pCurNode = (NumberNode*)m_pNodeList->Get(iIndex);
		if (pCurNode->fTime > m_fTmpTime)
			m_fTmpTime = pCurNode->fTime;
	}
	else if (m_iType == NT_MEDIA)
	{
		MediaNode* pCurNode = (MediaNode*)m_pNodeList->Get(iIndex);

#if _ENABLE_MEDIA_LEN
		if (pCurNode->fTime + pCurNode->fLen > m_fTmpTime)
			m_fTmpTime = pCurNode->fTime + pCurNode->fLen;
#else
		if (pCurNode->fTime > m_fTmpTime)
			m_fTmpTime = pCurNode->fTime;
#endif
	}
	else if (m_iType == NT_COMMAND)
	{
		CommandNode* pCurNode = (CommandNode*)m_pNodeList->Get(iIndex);
		if (pCurNode->fTime > m_fTmpTime)
			m_fTmpTime = pCurNode->fTime;
	}
	else if (m_iType == NT_EFFECT_INDEX)
	{
		EffectIndexNode* pCurNode = (EffectIndexNode*)m_pNodeList->Get(iIndex);
		if (pCurNode->fTime > m_fTmpTime)
			m_fTmpTime = pCurNode->fTime;
	}
	return m_fTmpTime;

#else
	for (int i = 0; i < m_pNodeList->GetTotal(); i++)
	{
		if (m_iType == NT_NUMBER)
		{
			NumberNode* pCurNode = (NumberNode*)m_pNodeList->Get(i);
			if (pCurNode->fTime > m_fTmpTime)
				m_fTmpTime = pCurNode->fTime;
		}
		else if (m_iType == NT_MEDIA)
		{
			MediaNode* pCurNode = (MediaNode*)m_pNodeList->Get(i);

#if _ENABLE_MEDIA_LEN
			if (pCurNode->fTime + pCurNode->fLen > m_fTmpTime)
				m_fTmpTime = pCurNode->fTime + pCurNode->fLen;
#else
			if (pCurNode->fTime > m_fTmpTime)
				m_fTmpTime = pCurNode->fTime;
#endif
		}
		else if (m_iType == NT_COMMAND)
		{
			CommandNode* pCurNode = (CommandNode*)m_pNodeList->Get(i);
			if (pCurNode->fTime > m_fTmpTime)
				m_fTmpTime = pCurNode->fTime;
		}
		else if (m_iType == NT_EFFECT_INDEX)
		{
			EffectIndexNode* pCurNode = (EffectIndexNode*)m_pNodeList->Get(i);
			if (pCurNode->fTime > m_fTmpTime)
				m_fTmpTime = pCurNode->fTime;
		}
		else if (m_iType == NT_EFFECT)
		{
			EffectNode* pCurNode = (EffectNode*)m_pNodeList->Get(i);
			if (pCurNode->fTime + pCurNode->fDuration > m_fTmpTime)
				m_fTmpTime = pCurNode->fTime + pCurNode->fDuration;
		}
	}
	return m_fTmpTime;
#endif
}

float TrackContent::GetMaximumTime()
{
#if 0
	m_fTmpTime = 0.0;
	for (int i = 0; i < m_pNodeList->GetTotal(); i++)
	{
		if (m_iType == NT_NUMBER)
		{
			NumberNode* pCurNode = (NumberNode*)m_pNodeList->Get(i);
			if (pCurNode->fTime > m_fTmpTime)
				m_fTmpTime = pCurNode->fTime;
		}
		else if (m_iType == NT_MEDIA)
		{
			MediaNode* pCurNode = (MediaNode*)m_pNodeList->Get(i);
			if (pCurNode->fTime > m_fTmpTime)
				m_fTmpTime = pCurNode->fTime;
		}
	}
	return m_fTmpTime;
#else
	return GetLastTime();
#endif
}

void TrackContent::Sort()
{
	unsigned int i = 0;
	int iIndex = 0;

	if (m_pNodeList->GetTotal() == 0)
		return;

	for (i = 0; i < m_pNodeList->GetTotal() - 1; i++)
	{
		switch (m_iType)
		{
			case NT_NUMBER:
				{
				/*
					NumberNode cur_item;
					NumberNode tmp_item;
					NumberNode* pCurNode = (NumberNode*)m_pNodeList->Get(i);

					memcpy(&cur_item, pCurNode,sizeof(NumberNode));
					for (unsigned int j = i + 1; j < m_pNodeList->GetTotal(); j++)
					{
						NumberNode* pNextItem = (NumberNode*)m_pNodeList->Get(j);

						if (Compare(pNextItem->fTime, cur_item.fTime) == 1)
						{
							iIndex = j;
							memcpy(&cur_item, pNextItem, sizeof(NumberNode));
						}
					}

					memcpy(&tmp_item, (NumberNode*)m_pNodeList->Get(i), sizeof(NumberNode));

					NumberNode* pAItem = (NumberNode*)m_pNodeList->Get(i);
					NumberNode* pBItem = (NumberNode*)m_pNodeList->Get(iIndex);

					memcpy(pAItem, pBItem, sizeof(NumberNode));
					memcpy(pBItem, &tmp_item, sizeof(NumberNode));
					*/
				}
				break;
			case NT_MEDIA:
				{
					MediaNode cur_item;
					MediaNode tmp_item;
					MediaNode* pCurNode = (MediaNode*)m_pNodeList->Get(i);

					iIndex = -1;

					memcpy(&cur_item, pCurNode, sizeof(MediaNode));
					for (unsigned int j = i + 1; j < m_pNodeList->GetTotal(); j++)
					{
						MediaNode* pNextItem = (MediaNode*)m_pNodeList->Get(j);

						if (Compare(pNextItem->fTime, cur_item.fTime) == 1)
						{
							iIndex = j;
							memcpy(&cur_item, pNextItem, sizeof(MediaNode));
						}
					}

					if (iIndex != -1)
					{
						memcpy(&tmp_item, (MediaNode*)m_pNodeList->Get(i), sizeof(MediaNode));

						MediaNode* pAItem = (MediaNode*)m_pNodeList->Get(i);
						MediaNode* pBItem = (MediaNode*)m_pNodeList->Get(iIndex);

						memcpy(pAItem, pBItem, sizeof(MediaNode));
						memcpy(pBItem, &tmp_item, sizeof(MediaNode));
					}
				}
				break;
			case NT_COMMAND:
				break;
			case NT_EFFECT_INDEX:
				break;
			case NT_EFFECT:
				{
					EffectNode cur_item;
					EffectNode tmp_item;
					EffectNode* pCurNode = (EffectNode*)m_pNodeList->Get(i);

					iIndex = -1;

					memcpy(&cur_item, pCurNode, sizeof(EffectNode));
					for (unsigned int j = i + 1; j < m_pNodeList->GetTotal(); j++)
					{
						EffectNode* pNextItem = (EffectNode*)m_pNodeList->Get(j);

						if (Compare(pNextItem->fTime, cur_item.fTime) == 1)
						{
							iIndex = j;
							memcpy(&cur_item, pNextItem, sizeof(EffectNode));
						}
					}
					if (iIndex != -1)
					{
						memcpy(&tmp_item, (EffectNode*)m_pNodeList->Get(i), sizeof(EffectNode));

						EffectNode* pAItem = (EffectNode*)m_pNodeList->Get(i);
						EffectNode* pBItem = (EffectNode*)m_pNodeList->Get(iIndex);

						memcpy(pAItem, pBItem, sizeof(EffectNode));
						memcpy(pBItem, &tmp_item, sizeof(EffectNode));
					}
				}
				break;
		}
	}
	/*
	char szMsg[512];
	sprintf(szMsg, "=====Sort=====\n");
	OutputDebugStringA(szMsg);
	for (i = 0; i < m_pNodeList->GetTotal(); i++)
	{
		switch (m_iType)
		{
			case NT_MEDIA:
			{
				MediaNode* pCurNode = (MediaNode*)m_pNodeList->Get(i);
				sprintf(szMsg,"Sort - [%d - %5.3f]\n",i,pCurNode->fTime);
				OutputDebugStringA(szMsg);
			}
			break;
			case NT_EFFECT:
			{
				EffectNode* pCurNode = (EffectNode*)m_pNodeList->Get(i);
				sprintf(szMsg, "Sort - [%d - %5.3f]\n", i, pCurNode->fTime);
				OutputDebugStringA(szMsg);
			}
			break;
		}
	}

	sprintf(szMsg, "=====Sort=====\n");
	OutputDebugStringA(szMsg);
	*/
}

int TrackContent::Compare(float fATime, float fBTime)
{
	if (fATime > fBTime)
		return -1;
	else if (fATime < fBTime)
		return 1;
	return 0;
}
