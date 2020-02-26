#include "stdafx.h"
#include "TrackContentManager.h"
#include "../../../Include/StrConv.h"
#include "../../../Include/sstring.h"

TrackContentManager::TrackContentManager()
{
	m_pItemList = new SQList;
	m_pItemList->EnableRemoveData(false);

	m_pEditViewWnd = 0;

	strcpy(m_track_pos_info.szFullPath, "");
	m_track_pos_info.fUnit = 0.0;

	m_pIOSourceCtrlDll = 0;
	m_pRGB32Buf = 0;
	m_pTmpImage = 0;

	m_pSIniFileW = new SIniFileW;
}

TrackContentManager::~TrackContentManager()
{
	RemoveAll();
	/*
	if (m_pItemList)
	{
		int iTotal = m_pItemList->GetTotal();
		for (int i = 0; i < iTotal; i++)
		{
			TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(i);
			if (pCurItem)
				delete pCurItem;
		}
		m_pItemList->Reset();
		delete m_pItemList;
	}
	*/

	if (m_pItemList)
	{
		delete m_pItemList;
		m_pItemList = 0;
	}

	if (m_pRGB32Buf)
		delete m_pRGB32Buf;

	if (m_pTmpImage)
		delete m_pTmpImage;

	if (m_pSIniFileW)
		delete m_pSIniFileW;
}

int TrackContentManager::AddItem(char* szItem)
{
	int iIndex = FindItem(szItem);
	if (iIndex == -1)
	{
		TrackContentItem* pNewItem = new TrackContentItem;
		pNewItem->SetName(szItem);
		//int iRet = pNewItem->Add(szTrack, fTime, iValue, iLinkType, iAngle);
		m_pItemList->Add(pNewItem);
		return 1;
	}
	return -1;
}

void TrackContentManager::DeleteItem(char* szItem)
{
	int iIndex = FindItem(szItem);
	if (iIndex != -1)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iIndex);
		if (pCurItem)
		{
			delete pCurItem;
			m_pItemList->Delete(iIndex);
		}
	}
}

void TrackContentManager::RemoveAll()
{
	if (m_pItemList)
	{
		int iTotal = m_pItemList->GetTotal();
		for (int i = 0; i < iTotal; i++)
		{
			TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(i);
			if (pCurItem)
				delete pCurItem;
		}

		if(iTotal > 0)
			m_pItemList->Reset();
//		delete m_pItemList;
//		m_pItemList = 0;
	}
}

int TrackContentManager::Add(char* szItem,char* szTrack,float fTime,int iValue, int iLinkType, int iAngle)
{
	int iIndex = FindItem(szItem);
	if (iIndex == -1)
	{
		TrackContentItem* pNewItem = new TrackContentItem;
		pNewItem->SetName(szItem);
		int iRet = pNewItem->Add(szTrack, fTime,iValue, iLinkType, iAngle);
		m_pItemList->Add(pNewItem);
		return iRet;
	}
	else
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iIndex);
		if (pCurItem)
			return pCurItem->Add(szTrack, fTime, iValue, iLinkType, iAngle);
	}
	return -1;
}

#if _ENABLE_MEDIA_LEN
int TrackContentManager::Add(char* szItem, char* szTrack, float fTime, char* szData,float fLen)
#else
int TrackContentManager::Add(char* szItem, char* szTrack, float fTime, char* szData)
#endif
{
	int iIndex = FindItem(szItem);
	if (iIndex == -1)
	{
		TrackContentItem* pNewItem = new TrackContentItem;
		pNewItem->SetName(szItem);
#if _ENABLE_MEDIA_LEN
		int iRet = pNewItem->Add(szTrack, fTime, szData,fLen);
#else
		int iRet = pNewItem->Add(szTrack, fTime, szData);
#endif
		m_pItemList->Add(pNewItem);
		return iRet;
	}
	else
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iIndex);
		if (pCurItem)
		{
#if _ENABLE_MEDIA_LEN
			return pCurItem->Add(szTrack, fTime, szData, fLen);
#else
			return pCurItem->Add(szTrack, fTime, szData);
#endif
		}
	}
	return -1;
}

int TrackContentManager::Add(char* szItem, char* szTrack, float fTime, int iType, char* szName, char* szTargetName,int iJumpType)
{
	int iIndex = FindItem(szItem);
	if (iIndex == -1)
	{
		TrackContentItem* pNewItem = new TrackContentItem;
		pNewItem->SetName(szItem);
		int iRet = pNewItem->Add(szTrack, fTime, iType, szName, szTargetName, iJumpType);
		m_pItemList->Add(pNewItem);
		return iRet;
	}
	else
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iIndex);
		if (pCurItem)
			return pCurItem->Add(szTrack, fTime,iType, szName, szTargetName, iJumpType);
	}
	return -1;
}

int TrackContentManager::Add(char* szItem, char* szTrack, float fTime, int iEffectIndex)
{
	int iIndex = FindItem(szItem);
	if (iIndex == -1)
	{
		TrackContentItem* pNewItem = new TrackContentItem;
		pNewItem->SetName(szItem);
		int iRet = pNewItem->Add(szTrack, fTime, iEffectIndex);
		m_pItemList->Add(pNewItem);
		return iRet;
	}
	else
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iIndex);
		if (pCurItem)
			return pCurItem->Add(szTrack, fTime, iEffectIndex);
	}
	return -1;
}

int TrackContentManager::Add(char* szItem, char* szTrack, float fTime, int iEffectIndex, char* szName,float fDuration, int iLevel, int* iParam)
{
	int iIndex = FindItem(szItem);
	if (iIndex == -1)
	{
		TrackContentItem* pNewItem = new TrackContentItem;
		pNewItem->SetName(szItem);
		int iRet = pNewItem->Add(szTrack, fTime, iEffectIndex, szName, fDuration, iLevel, iParam);
		m_pItemList->Add(pNewItem);
		return iRet;
	}
	else
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iIndex);
		if (pCurItem)
			return pCurItem->Add(szTrack, fTime, iEffectIndex, szName,fDuration, iLevel, iParam);
	}
	return -1;
}

void TrackContentManager::Delete(char* szItem, char* szTrack, float fTime)
{
	int iIndex = FindItem(szItem);
	if (iIndex != -1)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iIndex);
		if (pCurItem)
			pCurItem->Delete(szTrack,fTime);
	}
}
/*
int TrackContentManager::Edit(char* szItem, char* szTrack, float fTime, int iValue, int iLinkType, int iAngle)
{
	int iIndex = FindItem(szItem);
	if (iIndex != -1)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iIndex);
		if (pCurItem)
			return pCurItem->Edit(szTrack, fTime,iValue, iLinkType, iAngle);
	}
	return -1;
}

int TrackContentManager::Edit(char* szItem, char* szTrack, float fTime, char* szData)
{
	int iIndex = FindItem(szItem);
	if (iIndex != -1)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iIndex);
		if (pCurItem)
			return pCurItem->Edit(szTrack, fTime, szData);
	}
	return -1;
}
*/
int TrackContentManager::FindNode(char* szItem, char* szTrack, float fTime)
{
	int iIndex = FindItem(szItem);
	if (iIndex != -1)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iIndex);
		if (pCurItem)
			return pCurItem->FindNode(szTrack, fTime);
	}
	return -1;
}

int TrackContentManager::FindNode_Backward(char* szItem, char* szTrack, float fTime)
{
	int iIndex = FindItem(szItem);
	if (iIndex != -1)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iIndex);
		if (pCurItem)
			return pCurItem->FindNode_Backward(szTrack, fTime);
	}
	return -1;
}

int TrackContentManager::FindItem(char* szItem)
{
	for (int i = 0; i < m_pItemList->GetTotal(); i++)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(i);
		if (pCurItem)
		{
			//pCurItem->GetName();
			if (strcmp(pCurItem->GetName(), szItem) == 0)
				return i;
		}
	}
	return -1;
}

int TrackContentManager::ChangeItemName(char* szItem,char* szNewItemName)
{
	for (int i = 0; i < m_pItemList->GetTotal(); i++)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(i);
		if (pCurItem)
		{
			if (strcmp(pCurItem->GetName(), szItem) == 0)
			{
				pCurItem->SetName(szNewItemName);
				return i;
			}
		}
	}
	return -1;
}

void TrackContentManager::SplitFullPath(char* szFullPath,char* szItem,char* szTrack)
{
	char* pch = strchr(szFullPath, '/');
	if (pch)
	{
		int iItemLen = strlen(szFullPath) - strlen(pch);
		memcpy(szItem, szFullPath, iItemLen);
		szItem[iItemLen] = '\0';

		strcpy(szTrack, pch + 1);
	}
}

int TrackContentManager::Add(char* szFullPath, float fTime, int iValue, int iLinkType, int iAngle)
{
	char szItem[256];
	char szTrack[256];
	SplitFullPath(szFullPath, szItem, szTrack);

	return Add(szItem,szTrack,fTime, iValue, iLinkType, iAngle);
}

#if _ENABLE_MEDIA_LEN
int TrackContentManager::Add(char* szFullPath, float fTime, char* szData,float fLen)
#else
int TrackContentManager::Add(char* szFullPath, float fTime, char* szData)
#endif
{
	char szItem[256];
	char szTrack[256];
	SplitFullPath(szFullPath, szItem, szTrack);

#if _ENABLE_MEDIA_LEN
	return Add(szItem, szTrack, fTime, szData,fLen);
#else
	return Add(szItem, szTrack, fTime, szData);
#endif
}

int TrackContentManager::Add(char* szFullPath, float fTime, int iType, char* szName, char* szTargetName,int iJumpType)
{
	char szItem[256];
	char szTrack[256];
	SplitFullPath(szFullPath, szItem, szTrack);

	return Add(szItem, szTrack, fTime, iType, szName, szTargetName, iJumpType);
}

int TrackContentManager::Add(char* szFullPath, float fTime, int iEffectIndex)
{
	char szItem[256];
	char szTrack[256];
	SplitFullPath(szFullPath, szItem, szTrack);

	return Add(szItem, szTrack, fTime, iEffectIndex);
}

int TrackContentManager::Add(char* szFullPath, float fTime, int iEffectIndex, char* szName,float fDuration, int iLevel, int* iParam)
{
	char szItem[256];
	char szTrack[256];
	SplitFullPath(szFullPath, szItem, szTrack);

	return Add(szItem, szTrack, fTime, iEffectIndex, szName,fDuration, iLevel, iParam);
}

void TrackContentManager::Delete(char* szFullPath, float fTime)
{
	char szItem[256];
	char szTrack[256];
	SplitFullPath(szFullPath, szItem, szTrack);
	Delete(szItem, szTrack, fTime);
}
/*
int TrackContentManager::Edit(char* szFullPath, float fTime, int iValue, int iLinkType, int iAngle)
{
	char szItem[256];
	char szTrack[256];
	SplitFullPath(szFullPath, szItem, szTrack);

	return Edit(szItem, szTrack, fTime, iValue, iLinkType, iAngle);
}

int TrackContentManager::Edit(char* szFullPath, float fTime, char* szData)
{
	char szItem[256];
	char szTrack[256];
	SplitFullPath(szFullPath, szItem, szTrack);

	return Edit(szItem, szTrack, fTime, szData);
}
*/
int TrackContentManager::GetItemTackIndex(char* szFullPath, int& iItemIndex, int& iTrackIndex)
{
	char szItem[256];
	char szTrack[256];
	SplitFullPath(szFullPath, szItem, szTrack);

	int iTmpItemIndex = -1;

	for (int i = 0; i < GetTotalOfItems(); i++)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(i);
		if (pCurItem)
		{
			if (strcmp(pCurItem->GetName(), szItem) == 0)
			{
				iTmpItemIndex = i;
				break;
			}
		}
	}

	if (iTmpItemIndex == -1)
		return -1;
	else
		iItemIndex = iTmpItemIndex;

	if (GetTotalOfTracks(iTmpItemIndex) > 0)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iTmpItemIndex);
		if (pCurItem)
		{
			int iTmpTrackIndex = pCurItem->FindTrack(szTrack);
			if (iTmpTrackIndex != -1)
				iTrackIndex = iTmpTrackIndex;
			else
			{
				iTrackIndex = -1;
				return -2;
			}
		}
	}

	return 1;
}

int TrackContentManager::GetTotalOfItems()
{
	return m_pItemList->GetTotal();
}

char* TrackContentManager::GetItemName(int iIndex)
{
	if (m_pItemList->GetTotal() > iIndex)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iIndex);
		if (pCurItem)
			return pCurItem->GetName();
	}
	return 0;
}

int TrackContentManager::GetTotalOfTracks(int iItemIndex)
{
	if (m_pItemList->GetTotal() > iItemIndex)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iItemIndex);
		if (pCurItem)
			return pCurItem->GetTotalOfTracks();
	}
	return 0;
}

int TrackContentManager::GetTotalOfNodes(int iItemIndex,int iTrackIndex)
{
	if (m_pItemList->GetTotal() > iItemIndex)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iItemIndex);
		if (pCurItem)
			return pCurItem->GetTotalOfNodes(iTrackIndex);
	}
	return 0;
}

void* TrackContentManager::GetNodeData(int iItemIndex, int iTrackIndex, int iNodeIndex)
{
	if (m_pItemList->GetTotal() > iItemIndex)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iItemIndex);
		if (pCurItem)
			return pCurItem->GetNodeData(iTrackIndex, iNodeIndex);
	}
	return 0;
}

float TrackContentManager::GetLastTime(int iItemIndex, int iTrackIndex)
{
	if (m_pItemList->GetTotal() > iItemIndex)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iItemIndex);
		if (pCurItem)
			return pCurItem->GetLastTime(iTrackIndex);
	}
	return 0.0;
}

float TrackContentManager::GetMaximumTime(int iItemIndex)
{
	if (m_pItemList->GetTotal() > iItemIndex)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iItemIndex);
		if (pCurItem)
			return pCurItem->GetMaximumTime();
	}
	return 0.0;
}

void TrackContentManager::Sort(int iItemIndex, int iTrackIndex)
{
	if (m_pItemList->GetTotal() > iItemIndex)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iItemIndex);
		if (pCurItem)
			pCurItem->Sort(iTrackIndex);
	}
}

int TrackContentManager::FindNode(char* szFullPath, float fTime)
{
	char szItem[256];
	char szTrack[256];
	SplitFullPath(szFullPath, szItem, szTrack);
	return FindNode(szItem, szTrack, fTime);
}

int TrackContentManager::FindNode_Backward(char* szFullPath, float fTime)
{
	char szItem[256];
	char szTrack[256];
	SplitFullPath(szFullPath, szItem, szTrack);
	return FindNode_Backward(szItem, szTrack, fTime);
}

//tl_track_list.txt
void TrackContentManager::Save(char* szFileName)
{
	wchar_t wszAppName[512];
	wchar_t wszKeyName[256];
	wchar_t wszData[512];
	wchar_t wszTmp[64];
	wchar_t wszFileName[512] = L"tl_track_list.txt";

	if (strcmp(szFileName, "") != 0)
	{
		wcscpy(wszFileName, ANSIToUnicode(szFileName));
	}

	m_pSIniFileW->Open(wszFileName, true, true);
//	TraverseAllNodes(true);

	int iTotalOfItems = GetTotalOfItems();
	swprintf(wszData, L"%d", iTotalOfItems);
	m_pSIniFileW->SetItemData(L"Base", L"Total", wszData);

	for (int i = 0; i < iTotalOfItems; i++)
	{
		swprintf(wszAppName, L"Item%d", i);
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(i);
		if (pCurItem)
		{
			wchar_t* pwszName = ANSIToUnicode(pCurItem->GetName());
			m_pSIniFileW->SetItemData(wszAppName, L"Name", pwszName);

			swprintf(wszData, L"%d", pCurItem->GetTotalOfTracks());
			m_pSIniFileW->SetItemData(wszAppName, L"TotalOfTracks", wszData);
			for (int j = 0; j < pCurItem->GetTotalOfTracks(); j++)
			{
				swprintf(wszKeyName, L"Track%d", j);
				wchar_t* pwszTrackName = ANSIToUnicode(pCurItem->GetTrackName(j));
				m_pSIniFileW->SetItemData(wszAppName, wszKeyName, pwszTrackName);
			}

			//audio config
			wcscpy(wszData,L"");

			for (int k = 0; k < 10; k++)
			{
				bool bIsEnable = pCurItem->AudioIsEnabled(k);
				if(k != 9)
					swprintf(wszTmp,L"%d,", bIsEnable);
				else
					swprintf(wszTmp, L"%d", bIsEnable);

				wcscat(wszData, wszTmp);
			}

			m_pSIniFileW->SetItemData(wszAppName, L"AudioConfig", wszData);

			//audio volume
			wcscpy(wszData, L"");

			for (int k = 0; k < 10; k++)
			{
				int iVolume = pCurItem->GetVolume(k);
				if (k != 9)
					swprintf(wszTmp, L"%d,", iVolume);
				else
					swprintf(wszTmp, L"%d", iVolume);

				wcscat(wszData, wszTmp);
			}

			m_pSIniFileW->SetItemData(wszAppName, L"AudioVolume", wszData);

			SaveAllTracks(pCurItem);
			
		}
	}

	m_pSIniFileW->Write();
	m_pSIniFileW->Close();
}

void TrackContentManager::Load(char* szFileName)
{
	wchar_t wszAppName[512];
	wchar_t wszKeyName[256];
	wchar_t wszData[512];
	wchar_t wszItemName[128];
	wchar_t wszTrackName[512];
	char szAudioConfig[256];
	char szAudioVolume[512];
	int k;

	//wchar_t wszFullPath[512];
	wchar_t wszFileName[512] = L"tl_track_list.txt";

	if (strcmp(szFileName, "") != 0)
	{
		wcscpy(wszFileName, ANSIToUnicode(szFileName));

		if (!PathFileExists(szFileName))
		{
			char szMsg[512];
			sprintf(szMsg, "Cannot find the %s !!", szFileName);
			MessageBox(NULL, szMsg, "Warning", MB_OK| MB_TOPMOST);
			return;
		}
	}

	RemoveAll();

	m_pSIniFileW->Open(wszFileName, true, false);

	wchar_t* wszTotal = m_pSIniFileW->GetItemValue(L"Base", L"Total", L"0");
	int iTotalOfItems = _wtol(wszTotal);
	for (int i = 0; i < iTotalOfItems; i++)
	{
		swprintf(wszAppName, L"Item%d", i);
		wchar_t* wszName = m_pSIniFileW->GetItemValue(wszAppName, L"Name", L"");
		wcscpy(wszItemName, wszName);

		wchar_t* wszTotalOfTracks = m_pSIniFileW->GetItemValue(wszAppName, L"TotalOfTracks", L"");
		int iTotalOfTracks = _wtol(wszTotalOfTracks);
		for (int j = 0; j < iTotalOfTracks; j++)
		{
			swprintf(wszKeyName, L"Track%d", j);
			wchar_t* wszCurTrackName = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"");
			swprintf(wszTrackName, L"%s/%s", wszItemName, wszCurTrackName);

			LoadAllNodes(wszTrackName);
		}

		//audio config
		wchar_t* wszAudioConfig = m_pSIniFileW->GetItemValue(wszAppName, L"AudioConfig", L"");
		strcpy(szAudioConfig, WCharToChar(wszAudioConfig));

		for (k = 0; k < 10; k++)
		{
			long lValue = GetNextNumberFromString(',', szAudioConfig);
			TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(i);
			if (pCurItem)
			{
				pCurItem->EnableAudio(k,lValue);
			}
		}

		//audio volume
		wchar_t* wszAudioVolume = m_pSIniFileW->GetItemValue(wszAppName, L"AudioVolume", L"50,50,50,50,50,50,50,50,50,50");
		strcpy(szAudioVolume, WCharToChar(wszAudioVolume));

		for (k = 0; k < 10; k++)
		{
			long lValue = GetNextNumberFromString(',', szAudioVolume);
			TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(i);
			if (pCurItem)
			{
				pCurItem->SetVolume(k, lValue);
			}
		}
	}

	/*
	char* WCharToChar(const std::wstring& str);

	//id
	wsprintfW(wszKeyName, L"FileID%d", j);
	wchar_t* wszFileID = m_pSIniFileW->GetItemValue(wszParentPath, wszKeyName, L"");
	//unsigned long ulID = _wtol(wszFileID);
	unsigned long ulID = _wtof(wszFileID);
	*/

	m_pSIniFileW->Close();
}

void TrackContentManager::SaveAllTracks(TrackContentItem* pItem)
{
	wchar_t wszAppName[512];
	wchar_t wszKeyName[256];
	wchar_t wszData[512];
	wchar_t wszItemName[128];

	wcscpy(wszItemName,ANSIToUnicode(pItem->GetName()));

	for (int i = 0; i < pItem->GetTotalOfTracks(); i++)
	{
		swprintf(wszAppName, L"%s/%s", wszItemName, ANSIToUnicode(pItem->GetTrackName(i)));
		int iDataType = pItem->GetDataType(i);

		int iTotalOfNodes = pItem->GetTotalOfNodes(i);

		if(iDataType == NT_MEDIA)
			pItem->Sort(i);

		swprintf(wszData, L"%d", iTotalOfNodes);
		m_pSIniFileW->SetItemData(wszAppName, L"Total", wszData);
		swprintf(wszData, L"%d", iDataType);
		m_pSIniFileW->SetItemData(wszAppName, L"NodeType", wszData);

		for (int j = 0; j < iTotalOfNodes; j++)
		{
			switch (iDataType)
			{
				case NT_NUMBER:
					{
						NumberNode* pCurNode = (NumberNode*)pItem->GetNodeData(i, j);

						swprintf(wszKeyName, L"T%d", j);
						swprintf(wszData, L"%.3f", pCurNode->fTime);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"V%d", j);
						swprintf(wszData, L"%d", pCurNode->iValue);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"LT%d", j);
						swprintf(wszData, L"%d", pCurNode->iLinkType);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"A%d", j);
						swprintf(wszData, L"%d", pCurNode->iAngle);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);
					}
					break;
				case NT_MEDIA:
					{
						MediaNode* pCurNode = (MediaNode*)pItem->GetNodeData(i, j);
						
						swprintf(wszKeyName, L"T%d", j);
						swprintf(wszData, L"%.3f", pCurNode->fTime);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"Path%d", j);
						wcscpy(wszData, ANSIToUnicode(pCurNode->szPath));
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

#if _ENABLE_MEDIA_LEN
						swprintf(wszKeyName, L"Len%d", j);
						swprintf(wszData, L"%.3f", pCurNode->fLen);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);
#endif
					}
					break;
				case NT_COMMAND:
					{
						CommandNode* pCurNode = (CommandNode*)pItem->GetNodeData(i, j);

						swprintf(wszKeyName, L"T%d", j);
						swprintf(wszData, L"%.3f", pCurNode->fTime);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"Name%d", j);
						wcscpy(wszData, ANSIToUnicode(pCurNode->szName));
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"Type%d", j);
						swprintf(wszData, L"%d", pCurNode->iType);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"JType%d", j);
						swprintf(wszData, L"%d", pCurNode->iJumpType);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"TName%d", j);
						wcscpy(wszData, ANSIToUnicode(pCurNode->szTargetName));
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);
					}
					break;
				case NT_EFFECT_INDEX:
					{
						EffectIndexNode* pCurNode = (EffectIndexNode*)pItem->GetNodeData(i, j);
						
						swprintf(wszKeyName, L"T%d", j);
						swprintf(wszData, L"%.3f", pCurNode->fTime);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"I%d", j);
						swprintf(wszData, L"%d", pCurNode->iIndex);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);
					}
					break;
				case NT_EFFECT:
					{
						EffectNode* pCurNode = (EffectNode*)pItem->GetNodeData(i, j);

						swprintf(wszKeyName, L"T%d", j);
						swprintf(wszData, L"%.3f", pCurNode->fTime);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"I%d", j);
						swprintf(wszData, L"%d", pCurNode->iIndex);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"DUR%d", j);
						swprintf(wszData, L"%.3f", pCurNode->fDuration);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"Name%d", j);
						wcscpy(wszData, ANSIToUnicode(pCurNode->szName));
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"LV%d", j);
						swprintf(wszData, L"%d", pCurNode->iLevel);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"Parm_0_%d", j);
						swprintf(wszData, L"%d", pCurNode->iParam[0]);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"Parm_1_%d", j);
						swprintf(wszData, L"%d", pCurNode->iParam[1]);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"Parm_2_%d", j);
						swprintf(wszData, L"%d", pCurNode->iParam[2]);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"Parm_3_%d", j);
						swprintf(wszData, L"%d", pCurNode->iParam[3]);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);

						swprintf(wszKeyName, L"Parm_4_%d", j);
						swprintf(wszData, L"%d", pCurNode->iParam[4]);
						m_pSIniFileW->SetItemData(wszAppName, wszKeyName, wszData);
					}
				break;
			}
		}
	}
}

void TrackContentManager::LoadAllNodes(wchar_t* wszTrackName)
{
	wchar_t wszAppName[512];
	wchar_t wszKeyName[256];
	wchar_t wszData[512];
	char szFullPath[512];
	int iNodeType = 0;
	NumberNode number_node;
	MediaNode media_node;
	CommandNode cmd_node;
	EffectIndexNode effect_index_node;
	EffectNode effect_node;

	wcscpy(wszAppName, wszTrackName);
	strcpy(szFullPath, WCharToChar(wszTrackName));

	wchar_t* wszTotalOfNodes = m_pSIniFileW->GetItemValue(wszAppName, L"Total", L"0");
	int iTotalOfNodes = _wtol(wszTotalOfNodes);
	wchar_t* wszDataType = m_pSIniFileW->GetItemValue(wszAppName, L"NodeType", L"0");
	int iDataType = _wtol(wszDataType);
	
	for (int i = 0; i < iTotalOfNodes; i++)
	{
		swprintf(wszKeyName, L"T%d", i);
		wchar_t* wszTime = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"0.0");
		float fTime = _wtof(wszTime);

		switch (iDataType)
		{
			case NT_NUMBER:
				{
					number_node.fTime = fTime;
					swprintf(wszKeyName, L"V%d", i);
					wchar_t* wszValue = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"0");
					number_node.iValue = _wtoi(wszValue);
					swprintf(wszKeyName, L"LT%d", i);
					wchar_t* wszLinkType = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"0");
					number_node.iLinkType = _wtoi(wszLinkType);
					swprintf(wszKeyName, L"A%d", i);
					wchar_t* wszAngle = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"0");
					number_node.iAngle = _wtoi(wszAngle);

					Add(szFullPath, number_node.fTime, number_node.iValue, number_node.iLinkType, number_node.iAngle);
				}
				break;
			case NT_MEDIA:
				{
					media_node.fTime = fTime;
					swprintf(wszKeyName, L"Path%d", i);
					wchar_t* wszPath = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"");
					strcpy(media_node.szPath, WCharToChar(wszPath));
					
					if (!PathFileExistsA(media_node.szPath))
					{
						char szTmpStr[512];
						char szDecriptFileName[512];
						wchar_t wszDecriptFileName[512];

						bool bFileExists = false;

						char* pch = strrchr(media_node.szPath, '.');
						if (pch)
						{
							int iTmpLen = strlen(media_node.szPath) - strlen(pch);
							memcpy(szTmpStr, media_node.szPath, iTmpLen);
							szTmpStr[iTmpLen] = '\0';
							sprintf(szDecriptFileName, "%s!@#$%%^%s", szTmpStr, pch);

							if (PathFileExistsA(szDecriptFileName))
							{
								bFileExists = true;
							}
						}

						if (!bFileExists)
						{
							char szMsg[512];
							sprintf(szMsg, "Cannot find the %s !!", media_node.szPath);
							MessageBox(NULL, szMsg, "Warning", MB_OK| MB_TOPMOST);
							return;
						}
					}
					
#if _ENABLE_MEDIA_LEN
					swprintf(wszKeyName, L"Len%d", i);
					wchar_t* wszLen = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"");
					media_node.fLen = _wtof(wszLen);

					Add(szFullPath, media_node.fTime, media_node.szPath,media_node.fLen);
#else
					Add(szFullPath, media_node.fTime, media_node.szPath);
#endif
				}
				break;
			case NT_COMMAND:
				{
					cmd_node.fTime = fTime;
					swprintf(wszKeyName, L"Name%d", i);
					wchar_t* wszName = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"");
					strcpy(cmd_node.szName, WCharToChar(wszName));
					swprintf(wszKeyName, L"Type%d", i);
					wchar_t* wszType = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"0");
					cmd_node.iType = _wtoi(wszType);
					swprintf(wszKeyName, L"JType%d", i);
					wchar_t* wszJumpType = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"0");
					cmd_node.iJumpType = _wtoi(wszJumpType);
					swprintf(wszKeyName, L"TName%d", i);
					wchar_t* wszTargetName = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"");
					strcpy(cmd_node.szTargetName, WCharToChar(wszTargetName));

					Add(szFullPath, cmd_node.fTime, cmd_node.iType, cmd_node.szName, cmd_node.szTargetName, cmd_node.iJumpType);
				}
				break;
			case NT_EFFECT_INDEX:
				{
					effect_index_node.fTime = fTime;

					swprintf(wszKeyName, L"I%d", i);
					wchar_t* wszValue = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"0");
					effect_index_node.iIndex = _wtoi(wszValue);

					Add(szFullPath, effect_index_node.fTime, effect_index_node.iIndex);
				}
				break;
			case NT_EFFECT:
				{
					effect_node.fTime = fTime;

					swprintf(wszKeyName, L"I%d", i);
					wchar_t* wszValue = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"0");
					effect_node.iIndex = _wtoi(wszValue);

					swprintf(wszKeyName, L"DUR%d", i);
					wchar_t* wszLen = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"");
					effect_node.fDuration = _wtof(wszLen);

					swprintf(wszKeyName, L"Name%d", i);
					wchar_t* wszName = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"");
					strcpy(effect_node.szName, WCharToChar(wszName));

					swprintf(wszKeyName, L"LV%d", i);
					wszValue = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"0");
					effect_node.iLevel = _wtoi(wszValue);

					swprintf(wszKeyName, L"Parm_0_%d", i);
					wszValue = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"0");
					effect_node.iParam[0] = _wtoi(wszValue);

					swprintf(wszKeyName, L"Parm_1_%d", i);
					wszValue = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"0");
					effect_node.iParam[1] = _wtoi(wszValue);

					swprintf(wszKeyName, L"Parm_2_%d", i);
					wszValue = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"0");
					effect_node.iParam[2] = _wtoi(wszValue);

					swprintf(wszKeyName, L"Parm_3_%d", i);
					wszValue = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"0");
					effect_node.iParam[3] = _wtoi(wszValue);

					swprintf(wszKeyName, L"Parm_4_%d", i);
					wszValue = m_pSIniFileW->GetItemValue(wszAppName, wszKeyName, L"0");
					effect_node.iParam[4] = _wtoi(wszValue);

					//Add(szFullPath, effect_node.fTime, effect_node.iIndex, effect_node.fDuration);
					
					//int iParam[5] = { effect_node.iParam[0] ,effect_node.iParam[1] ,effect_node.iParam[2] ,effect_node.iParam[3] ,effect_node.iParam[4] };
					Add(szFullPath, effect_node.fTime, effect_node.iIndex, effect_node.szName,effect_node.fDuration, effect_node.iLevel, effect_node.iParam);
				}
			break;
		}
	}
}

void TrackContentManager::ReturnFileName(wchar_t* wszFileName)
{
	if (wszFileName)
	{
		wcscpy(m_wszFilePath, wszFileName);
		//theApp.m_strSelVideoPath = wszFileName;
		//int kk = 0;

		if (m_pEditViewWnd)
			m_pEditViewWnd->PostMessage(WM_TM_RETURN_FILENAME, 1, 0);
	}
	else
	{
		if (m_pEditViewWnd)
			m_pEditViewWnd->PostMessage(WM_TM_RETURN_FILENAME, 0, 0);
	}
}

void TrackContentManager::SetEditViewWnd(CWnd* pObj)
{
	m_pEditViewWnd = pObj;
}

void TrackContentManager::SetTrackPosInfo(TrackPosInfo* pInfo)
{
	memcpy(&m_track_pos_info, pInfo,sizeof(TrackPosInfo));
}

TrackPosInfo* TrackContentManager::GetTrackPosInfo()
{
	return &m_track_pos_info;
}

wchar_t* TrackContentManager::GetFilePath()
{
	return m_wszFilePath;
}

void TrackContentManager::SetIOSourceCtrlDll(IOSourceCtrlDll* pObj)
{
	m_pIOSourceCtrlDll = pObj;
}

IOSourceCtrlDll* TrackContentManager::GetIOSourceCtrlDll()
{
	return m_pIOSourceCtrlDll;
}

Gdiplus::Bitmap* TrackContentManager::GetPicture(wchar_t* wszFileName)
{
	MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfo(wszFileName);
	if (pCurItem)
	{
		ThumbnailPicture* pPic = m_pIOSourceCtrlDll->GetPicture(pCurItem);
		if (pPic)
		{
			if (!m_pRGB32Buf)
				m_pRGB32Buf = new unsigned char[_THUMBNAIL_FRAME_W * _THUMBNAIL_FRAME_H * 4];

			m_yuv_converter.I420ToARGB(pPic->pBuffer, m_pRGB32Buf, _THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H);

			BITMAPINFO bmi;
			memset(&bmi, 0, sizeof(bmi));
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = _THUMBNAIL_FRAME_W;
			bmi.bmiHeader.biHeight = _THUMBNAIL_FRAME_H;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biBitCount = 32;

			if (m_pTmpImage)
			{
				delete m_pTmpImage;
				m_pTmpImage = 0;
			}

			m_pTmpImage = new Gdiplus::Bitmap(&bmi, m_pRGB32Buf);
			return m_pTmpImage;
		}
	}
	return 0;
}

char* TrackContentManager::WChar2Char(wchar_t* wszText)
{
	return WCharToChar(wszText);
}

wchar_t* TrackContentManager::Char2WChar(char* szText)
{
	wcscpy(m_wszTempStr, ANSIToUnicode(szText));
	return m_wszTempStr;// ANSIToUnicode(szText);
}

void TrackContentManager::EnableAudio(int iItemIndex,int iIndex, bool bEnable)
{
	if (m_pItemList->GetTotal() > iItemIndex)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iItemIndex);
		if (pCurItem)
			pCurItem->EnableAudio(iIndex,bEnable);
	}
}

bool TrackContentManager::AudioIsEnabled(int iItemIndex, int iIndex)
{
	if (m_pItemList->GetTotal() > iItemIndex)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iItemIndex);
		if (pCurItem)
			return pCurItem->AudioIsEnabled(iIndex);
	}
	return false;
}

void TrackContentManager::SetVolume(int iItemIndex, int iIndex, int iVolume)
{
	if (m_pItemList->GetTotal() > iItemIndex)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iItemIndex);
		if (pCurItem)
			pCurItem->SetVolume(iIndex, iVolume);
	}
}

int TrackContentManager::GetVolume(int iItemIndex, int iIndex)
{
	if (m_pItemList->GetTotal() > iItemIndex)
	{
		TrackContentItem* pCurItem = (TrackContentItem*)m_pItemList->Get(iItemIndex);
		if (pCurItem)
			return pCurItem->GetVolume(iIndex);
	}
	return 0;
}

void TrackContentManager::Test()
{
	/*
	char szFullPath[] = "Item1/Mixer/Layer1/Color/R";
	char szTime[] = "00:00:00.000";
	float fTime = 1.500;
	int iValue = 50;

	int iRet = Add(szFullPath,fTime,iValue);
	*/
}
