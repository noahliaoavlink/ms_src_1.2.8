#include "stdafx.h"
#include "TrackContentItem.h"

TrackContentItem::TrackContentItem()
{
	strcpy(m_szName, "");
	m_pTrackList = new SQList;
	m_pTrackList->EnableRemoveData(false);

	memset(m_bAudioConfig, 1, _MAX_SOURCES);

	for(int i = 0;i < _MAX_SOURCES;i++)
		m_iVolume[i] = 50;
}

TrackContentItem::~TrackContentItem()
{
	if (m_pTrackList)
	{
		int iTotal = m_pTrackList->GetTotal();
		for (int i = 0; i < iTotal; i++)
		{
			TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(i);
			if (pCurTrack)
				delete pCurTrack;
		}
		m_pTrackList->Reset();
		delete m_pTrackList;
	}
}

void TrackContentItem::SetName(char* szData)
{
	strcpy(m_szName, szData);
}

char* TrackContentItem::GetName()
{
	return m_szName;
}

int TrackContentItem::Add(char* szTrack, float fTime, int iValue, int iLinkType, int iAngle)
{
	int iIndex = FindTrack(szTrack);
	if (iIndex == -1)
	{
		TrackContent* pNewTrack = new TrackContent;
		pNewTrack->SetType(NT_NUMBER);
		pNewTrack->SetName(szTrack);
		int iRet = pNewTrack->Add(fTime, iValue, iLinkType, iAngle);

		m_pTrackList->Add(pNewTrack);
		return iRet;
	}
	else
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iIndex);
		if (pCurTrack)
		{
			pCurTrack->Add(fTime, iValue, iLinkType, iAngle);
			return 1;
		}
	}
	return -1;
}

#if _ENABLE_MEDIA_LEN
int TrackContentItem::Add(char* szTrack, float fTime, char* szData,float fLen)
#else
int TrackContentItem::Add(char* szTrack, float fTime, char* szData)
#endif
{
	int iIndex = FindTrack(szTrack);
	if (iIndex == -1)
	{
		TrackContent* pNewTrack = new TrackContent;
		pNewTrack->SetType(NT_MEDIA);
		pNewTrack->SetName(szTrack);
#if _ENABLE_MEDIA_LEN
		int iRet = pNewTrack->Add(fTime, szData,fLen);
#else
		int iRet = pNewTrack->Add(fTime, szData);
#endif

		m_pTrackList->Add(pNewTrack);
		return 1;
	}
	else
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iIndex);
		if (pCurTrack)
		{
#if _ENABLE_MEDIA_LEN
			pCurTrack->Add(fTime, szData, fLen);
#else
			pCurTrack->Add(fTime, szData);
#endif
			return 1;
		}
	}
	return -1;
}

int TrackContentItem::Add(char* szTrack, float fTime, int iType, char* szName, char* szTargetName, int iJumpType)
{
	int iIndex = FindTrack(szTrack);
	if (iIndex == -1)
	{
		TrackContent* pNewTrack = new TrackContent;
		pNewTrack->SetType(NT_COMMAND);
		pNewTrack->SetName(szTrack);
		int iRet = pNewTrack->Add(fTime, iType, szName, szTargetName, iJumpType);

		m_pTrackList->Add(pNewTrack);
		return 1;
	}
	else
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iIndex);
		if (pCurTrack)
		{
			pCurTrack->Add(fTime, iType, szName, szTargetName, iJumpType);
			return 1;
		}
	}
	return -1;
}

int TrackContentItem::Add(char* szTrack, float fTime, int iEffectIndex)
{
	int iIndex = FindTrack(szTrack);
	if (iIndex == -1)
	{
		TrackContent* pNewTrack = new TrackContent;
		pNewTrack->SetType(NT_EFFECT_INDEX);
		pNewTrack->SetName(szTrack);
		int iRet = pNewTrack->Add(fTime, iEffectIndex);

		m_pTrackList->Add(pNewTrack);
		return iRet;
	}
	else
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iIndex);
		if (pCurTrack)
		{
			pCurTrack->Add(fTime, iEffectIndex);
			return 1;
		}
	}
	return -1;
}

int TrackContentItem::Add(char* szTrack, float fTime, int iEffectIndex, char* szName, float fDuration, int iLevel, int* iParam)
{
	int iIndex = FindTrack(szTrack);
	if (iIndex == -1)
	{
		TrackContent* pNewTrack = new TrackContent;
		pNewTrack->SetType(NT_EFFECT);
		pNewTrack->SetName(szTrack);
		int iRet = pNewTrack->Add(fTime, iEffectIndex, szName, fDuration, iLevel, iParam);

		m_pTrackList->Add(pNewTrack);
		return iRet;
	}
	else
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iIndex);
		if (pCurTrack)
		{
			pCurTrack->Add(fTime, iEffectIndex, szName,fDuration, iLevel, iParam);
			return 1;
		}
	}
	return -1;
}

void TrackContentItem::Delete(char* szTrack, float fTime)
{
	int iIndex = FindTrack(szTrack);
	if (iIndex != -1)
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iIndex);
		if (pCurTrack)
			pCurTrack->Delete(fTime);
	}
}
/*
int TrackContentItem::Edit(char* szTrack, float fTime, int iValue, int iLinkType, int iAngle)
{
	int iIndex = FindTrack(szTrack);
	if (iIndex != -1)
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iIndex);
		if (pCurTrack)
			return pCurTrack->Edit(fTime, iValue, iLinkType, iAngle);
	}
	return -1;
}

int TrackContentItem::Edit(char* szTrack, float fTime, char* szData)
{
	int iIndex = FindTrack(szTrack);
	if (iIndex != -1)
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iIndex);
		if (pCurTrack)
			return pCurTrack->Edit(fTime, szData);
	}
	return -1;
}
*/
int TrackContentItem::FindTrack(char* szTrack)
{
	for (int i = 0; i < m_pTrackList->GetTotal(); i++)
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(i);
		if (pCurTrack)
		{
			if (strcmp(pCurTrack->GetName(), szTrack) == 0)
				return i;
		}
	}
	return -1;
}

int TrackContentItem::FindNode(char* szTrack, float fTime)
{
	int iIndex = FindTrack(szTrack);
	if (iIndex != -1)
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iIndex);
		if (pCurTrack)
			return pCurTrack->FindNode(fTime);
	}
	return -1;
}

int TrackContentItem::FindNode_Backward(char* szTrack, float fTime)
{
	int iIndex = FindTrack(szTrack);
	if (iIndex != -1)
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iIndex);
		if (pCurTrack)
			return pCurTrack->FindNode_Backward(fTime);
	}
	return -1;
}

int TrackContentItem::GetTotalOfTracks()
{
	return m_pTrackList->GetTotal();
}
/*
char* TrackContentItem::GetTrackName(int iIndex)
{
	if (m_pTrackList->GetTotal() > iIndex)
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iIndex);
		if (pCurTrack)
			pCurTrack->GetName();
	}
	return 0;
}
*/

int TrackContentItem::GetTotalOfNodes(int iTrackIndex)
{
	if (m_pTrackList->GetTotal() > iTrackIndex && iTrackIndex >= 0)
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iTrackIndex);
		if (pCurTrack)
			return pCurTrack->GetTotalOfNodes();
	}
	return 0;
}

void* TrackContentItem::GetNodeData(int iTrackIndex,int iNodeIndex)
{
	if (m_pTrackList->GetTotal() > iTrackIndex && iTrackIndex >= 0)
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iTrackIndex);
		if (pCurTrack)
		{
			if (pCurTrack->GetTotalOfNodes() > iNodeIndex)
				return pCurTrack->GetNodeData(iNodeIndex);
		}
	}
	return 0;
}

float TrackContentItem::GetLastTime(int iTrackIndex)
{
	if (m_pTrackList->GetTotal() > iTrackIndex && iTrackIndex >= 0)
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iTrackIndex);
		if (pCurTrack)
			return pCurTrack->GetLastTime();
	}
	return 0.0;
}

float TrackContentItem::GetMaximumTime()
{
	m_fTmpTime = 0.0;
	for (int i = 0; i < m_pTrackList->GetTotal(); i++)
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(i);
		if (pCurTrack)
		{
			float fCurTime = pCurTrack->GetMaximumTime();

			if (fCurTime > m_fTmpTime)
				m_fTmpTime = fCurTime;
		}
	}
	return m_fTmpTime;
}

char* TrackContentItem::GetTrackName(int iIndex)
{
	if (m_pTrackList->GetTotal() > iIndex && iIndex >= 0)
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iIndex);
		if (pCurTrack)
			return pCurTrack->GetName();
	}
	return "";
}

int TrackContentItem::GetDataType(int iIndex)
{
	if (m_pTrackList->GetTotal() > iIndex && iIndex >= 0)
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iIndex);
		if (pCurTrack)
			return pCurTrack->GetType();
	}
	return -1;
}

void TrackContentItem::Sort(int iTrackIndex)
{
	if (m_pTrackList->GetTotal() > iTrackIndex && iTrackIndex >= 0)
	{
		TrackContent* pCurTrack = (TrackContent*)m_pTrackList->Get(iTrackIndex);
		if (pCurTrack)
			pCurTrack->Sort();
	}
}

void TrackContentItem::EnableAudio(int iIndex,bool bEnable)
{
	if (_MAX_SOURCES > iIndex)
		m_bAudioConfig[iIndex] = bEnable;
}

bool TrackContentItem::AudioIsEnabled(int iIndex)
{
	if (_MAX_SOURCES > iIndex)
		return m_bAudioConfig[iIndex];
	return false;
}

void TrackContentItem::SetVolume(int iIndex, int iVolume)
{
	if(_MAX_SOURCES > iIndex)
		m_iVolume[iIndex] = iVolume;
}

int TrackContentItem::GetVolume(int iIndex)
{
	if (_MAX_SOURCES > iIndex)
		return m_iVolume[iIndex];
	return 0;
}