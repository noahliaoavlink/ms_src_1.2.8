#pragma once

#ifndef _TrackContentItem_H_
#define _TrackContentItem_H_

#include "TrackContent.h"

#define _MAX_SOURCES 10

class TrackContentItem
{
	char m_szName[256];
	SQList* m_pTrackList;
	float m_fTmpTime;

	bool m_bAudioConfig[_MAX_SOURCES];
	int m_iVolume[_MAX_SOURCES];
protected:
public:
	TrackContentItem();
	~TrackContentItem();

	void SetName(char* szData);
	char* GetName();
	//int Add(char* szTrack, float fTime, int iValue, int iLinkType = LT_NORMAL, int iAngle = 0);
	int Add(char* szTrack, float fTime, int iValue, int iLinkType, int iAngle = 0);
#if _ENABLE_MEDIA_LEN
	int Add(char* szTrack, float fTime, char* szData,float fLen);
#else
	int Add(char* szTrack, float fTime, char* szData);
#endif
	int Add(char* szTrack, float fTime, int iType, char* szName, char* szTargetName = "",int iJumpType = JT_NONE);
	int Add(char* szTrack, float fTime, int iEffectIndex);
	int Add(char* szTrack, float fTime, int iEffectIndex, char* szName, float fDuration, int iLevel, int* iParam);
	void Delete(char* szTrack, float fTime);
	/*
	int Edit(char* szTrack, float fTime, int iValue, int iLinkType = LT_NORMAL, int iAngle = 0);
	int Edit(char* szTrack, float fTime, char* szData);
	*/

	int FindTrack(char* szTrack);
	int FindNode(char* szTrack, float fTime);
	int FindNode_Backward(char* szTrack, float fTime);

	int GetTotalOfTracks();
	//char* GetTrackName(int iIndex);
	int GetTotalOfNodes(int iTrackIndex);
	void* GetNodeData(int iTrackIndex, int iNodeIndex);
	float GetLastTime(int iTrackIndex);
	float GetMaximumTime();
	char* GetTrackName(int iIndex);
	int GetDataType(int iIndex);
	void Sort(int iTrackIndex);

	void EnableAudio(int iIndex, bool bEnable);
	bool AudioIsEnabled(int iIndex);
	void SetVolume(int iIndex, int iVolume);
	int GetVolume(int iIndex);
};

#endif
