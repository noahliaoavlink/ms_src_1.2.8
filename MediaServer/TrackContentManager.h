#pragma once

#ifndef _TrackContentManager_H_
#define _TrackContentManager_H_

#include "TrackContentItem.h"
#include "IOSourceCtrlDll.h"
#include "YUVConverter.h"
#include "SIniFileW.h"

typedef struct
{
	char szFullPath[512];
	float fUnit;
}TrackPosInfo;

class TrackContentManager : public MediaFileManagerCallback
{
		SQList* m_pItemList;

		wchar_t m_wszFilePath[512];
		CWnd* m_pEditViewWnd;
		TrackPosInfo m_track_pos_info;

		IOSourceCtrlDll* m_pIOSourceCtrlDll;
		unsigned char* m_pRGB32Buf;
		Gdiplus::Bitmap* m_pTmpImage;
		YUVConverter m_yuv_converter;

		SIniFileW* m_pSIniFileW;
		wchar_t m_wszTempStr[512];
	protected:
	public:
		TrackContentManager();
		~TrackContentManager();

		int AddItem(char* szItem);
		void DeleteItem(char* szItem);
		void RemoveAll();
		//int Add(char* szItem, char* szTrack, float fTime, int iValue, int iLinkType = LT_NORMAL, int iAngle = 0);
		int Add(char* szItem, char* szTrack, float fTime, int iValue, int iLinkType, int iAngle = 0);
#if _ENABLE_MEDIA_LEN
		int Add(char* szItem, char* szTrack, float fTime, char* szData,float fLen);
#else
		int Add(char* szItem, char* szTrack, float fTime, char* szData);
#endif
		int Add(char* szItem, char* szTrack, float fTime, int iType, char* szName, char* szTargetName = "", int iJumpType = JT_NONE);
		int Add(char* szItem, char* szTrack, float fTime, int iEffectIndex);
		int Add(char* szItem, char* szTrack, float fTime, int iEffectIndex, char* szName, float fDuration, int iLevel, int* iParam);
		void Delete(char* szItem, char* szTrack, float fTime);
		/*
		int Edit(char* szItem, char* szTrack, float fTime, int iValue, int iLinkType = LT_NORMAL, int iAngle = 0);
		int Edit(char* szItem, char* szTrack, float fTime, char* szData);
		*/

		int FindItem(char* szItem);
		int FindNode(char* szItem, char* szTrack, float fTime);
		int FindNode(char* szFullPath, float fTime);

		int FindNode_Backward(char* szItem, char* szTrack, float fTime);
		int FindNode_Backward(char* szFullPath, float fTime);

		int ChangeItemName(char* szItem, char* szNewItemName);

		//int Add(char* szFullPath, float fTime, int iValue, int iLinkType = LT_NORMAL, int iAngle = 0);
		int Add(char* szFullPath, float fTime, int iValue, int iLinkType, int iAngle = 0);
#if _ENABLE_MEDIA_LEN
		int Add(char* szFullPath, float fTime, char* szData,float fLen);
#else
		int Add(char* szFullPath, float fTime, char* szData);
#endif
		int Add(char* szFullPath, float fTime, int iType, char* szName, char* szTargetName = "", int iJumpType = JT_NONE);
		int Add(char* szFullPath, float fTime, int iEffectIndex);
		int Add(char* szFullPath, float fTime, int iEffectIndex, char* szName,float fDuration, int iLevel, int* iParam);
		void Delete(char* szFullPath, float fTime);
		/*
		int Edit(char* szFullPath, float fTime, int iValue, int iLinkType = LT_NORMAL, int iAngle = 0);
		int Edit(char* szFullPath, float fTime, char* szData);
		*/
		int GetItemTackIndex(char* szFullPath, int& iItemIndex, int& iTrackIndex);
		void SplitFullPath(char* szFullPath, char* szItem, char* szTrack);

		int GetTotalOfItems();
		char* GetItemName(int iIndex);
		int GetTotalOfTracks(int iItemIndex);
		int GetTotalOfNodes(int iItemIndex, int iTrackIndex);
		void* GetNodeData(int iItemIndex, int iTrackIndex, int iNodeIndex);
		float GetMaximumTime(int iItemIndex);
		float GetLastTime(int iItemIndex, int iTrackIndex);
		void Sort(int iItemIndex, int iTrackIndex);

		void Save(char* szFileName = "");
		void Load(char* szFileName = "");
		void SaveAllTracks(TrackContentItem* pItem);
		void LoadAllNodes(wchar_t* wszTrackName);

		void ReturnFileName(wchar_t* wszFileName);
		void SetEditViewWnd(CWnd* pObj);
		void SetTrackPosInfo(TrackPosInfo* pInfo);
		TrackPosInfo* GetTrackPosInfo();
		wchar_t* GetFilePath();
		void SetIOSourceCtrlDll(IOSourceCtrlDll* pObj);
		IOSourceCtrlDll* GetIOSourceCtrlDll();
		Gdiplus::Bitmap* GetPicture(wchar_t* wszFileName);
		char* WChar2Char(wchar_t* wszText);
		wchar_t* Char2WChar(char* szText);

		void EnableAudio(int iItemIndex, int iIndex, bool bEnable);
		bool AudioIsEnabled(int iItemIndex, int iIndex);
		void SetVolume(int iItemIndex, int iIndex, int iVolume);
		int GetVolume(int iItemIndex, int iIndex);

		void Test();
};

#endif