#include "stdafx.h"
#include "MediaFileManager.h"

#pragma comment(lib, "rpcrt4.lib")

__inline wchar_t* GetNextPatternW(wchar_t* wszString,wchar_t ch,int* iIndex)
{
	//wchar_t* pch = wcsrchr(wszFileName,'.');
	
	wchar_t wszNumber[64];
	int iNumber = 0;

	if(wcscmp(wszString,L"") == 0)
		return L"";

	wchar_t* pTemp = wcsrchr(wszString,ch);
	if(pTemp)
	{
	   int iLen = wcslen(wszString) - wcslen(pTemp);
	   memcpy(wszNumber,wszString,iLen*sizeof(wchar_t));
	   wszNumber[iLen] = L'\0';
	   /*
	   iNumber = atol(szNumber);
		*/
	   if(wcslen(pTemp) > 1)
	       wcscpy(wszString,pTemp + 1);
	   else 
		   wcscpy(wszString,L"");
	   return wszNumber;
	}
	else
	{
		//iNumber = atol(szSrc);
		if(wcslen(wszString) > 0)
			wcscpy(wszNumber,wszString);
		wcscpy(wszString,L"");
		return wszNumber;
	}
	
	/*
	int iCount = 0;
	static wchar_t wszPattern[10];
    int iLen = wcslen(wszString);
	bool bFind = false;
	for(int i = *iIndex;i < iLen;i++)
	{
		if(wszString[i] == ch && bFind)
		{
			wszPattern[iCount] = '\0';
			*iIndex = i + 1;
	        return wszPattern;
		}
		else if(wszString[i] == ch && !bFind)
            bFind = true;
		else if(bFind)
			wszPattern[iCount++] = wszString[i];
	}
    return 0;
	*/
}

MediaFileManager::MediaFileManager()
{
	CreateLists();

	//m_pFFAVCodecDLL = new FFAVCodecDLL;
	m_pFFMediaFileDll = new FFMediaFileDll;
	m_pThumbnailCtrl = new ThumbnailCtrl;
	//m_pThumbnailCtrl->SetFFAVCodecDLL(m_pFFAVCodecDLL);
	m_pThumbnailCtrl->SetFFMediaFileDll(m_pFFMediaFileDll);

	m_pRTSPPlayerDll = new RTSPPlayerDll;
	m_pRTSPObj = 0;

	m_pIOSourceCtrlCallback = 0;
}

MediaFileManager::~MediaFileManager()
{
	DestoryLists();

	//if(m_pFFAVCodecDLL)
		//delete m_pFFAVCodecDLL;
	if (m_pFFMediaFileDll)
		delete m_pFFMediaFileDll;
	if(m_pThumbnailCtrl)
		delete m_pThumbnailCtrl;

	if (m_pRTSPPlayerDll)
	{
		if (m_pRTSPObj)
			m_pRTSPPlayerDll->_In_RTSP_Destroy(m_pRTSPObj);
		delete m_pRTSPPlayerDll;
	}
}

void MediaFileManager::Init()
{
	//bool bInitDecoderRet = m_pFFAVCodecDLL->Init();
	m_pFFMediaFileDll->LoadLib();
	m_pRTSPPlayerDll->LoadLib();

	m_pRTSPObj = m_pRTSPPlayerDll->_In_RTSP_Create();
	m_pRTSPPlayerDll->_In_RTSP_Init(m_pRTSPObj,0);

	m_pThumbnailCtrl->SetRTSPPlayerDll(m_pRTSPPlayerDll, m_pRTSPObj);

	m_pThumbnailCtrl->Open();
	Load();
}

void MediaFileManager::SetIOSourceCtrlCallback(void* pObj)
{
	m_pIOSourceCtrlCallback = (IOSourceCtrlCallback*)pObj;
}

void MediaFileManager::Load()
{
#if 0
	//wchar_t wszMsg[512];
	char szMsg[512];
	char szAPPath[512];
	char szFileName[512];
	char szUrl[512];
	MFLFile mfl_file;

	GetExecutionPath(NULL, szAPPath);
	sprintf(szFileName,"%s\\FileList.mfl", szAPPath);

	//int iRet = mfl_file.Open("FileList.mfl");
	int iRet = mfl_file.Open(szFileName);
	if(iRet != -1)
	{
		int j;
		//wchar_t* pwszTags[_MAX_TAGS];
		//for(j = 0;j < _MAX_TAGS;j++)
			//pwszTags[j] = new wchar_t[64];

		bool bIsOldVersion = false;
		FileHeader* pHeader = mfl_file.ReadHeader();
		if (!pHeader)
		{
			mfl_file.Close();

			if (m_pIOSourceCtrlCallback)
			{
				sprintf(szMsg,"pHeader == NULL !!");
				m_pIOSourceCtrlCallback->Event(0, szMsg);
			}
			return;
		}

		unsigned long ulMFFileLen = mfl_file.GetLength();

		int iMFLItemLen = sizeof(MFLItem);
		
		unsigned long ulTotalOfItemLen = iMFLItemLen * pHeader->ulTotalOfItems + sizeof(FileHeader);

		if (ulMFFileLen < ulTotalOfItemLen)
			bIsOldVersion = true;

		int iMediaFileInfoLen = sizeof(MediaFileInfo);
		CFileFind fFind;

		if(pHeader->ulTotalOfItems > 0)
		{
			for(int i = 0;i < pHeader->ulTotalOfItems;i++)
			{
				MFLItem* pCurItem = mfl_file.ReadItem();
				if(pCurItem)
				{
					//for(j = 0;j < _MAX_TAGS;j++)
						//wcscpy(pwszTags[j],pCurItem->wszTags[j]);

					if (m_pIOSourceCtrlCallback && pCurItem->bIsEncrypted)
					{
						if (fFind.FindFile(pCurItem->wszFilePath))
							m_pIOSourceCtrlCallback->BeginOpenFile(pCurItem->wszFilePath);
						else
							continue;
					}

//					wsprintf(wszMsg, L"MediaFileManager::Load %s %u bIsEncrypted:%d\n", pCurItem->wszFilePath, pCurItem->ulID,pCurItem->bIsEncrypted);
//					OutputDebugString(wszMsg);

					strcpy(szUrl, WCharToChar(pCurItem->wszFilePath));
					bool bIsRtsp = IsRTSPUrl(szUrl);
					if (bIsRtsp)
					{
						//AddRTSPUrl(pCurItem->wszFilePath, pCurItem->dlLen);
						int iRet = AddRTSPUrl2(pCurItem->wszFilePath, pCurItem->dlLen, pCurItem->ulID, pCurItem->lCoverIndex);
						if (iRet != 1)
						{
							if (m_pIOSourceCtrlCallback)
							{
								sprintf(m_szErrorMsg, "AddRTSPUrl2() [%s] failed!! %d ", szUrl, iRet);
								m_pIOSourceCtrlCallback->Event(0, m_szErrorMsg);
							}
						}
					}
					else
					{
						int iRet = Add(pCurItem->wszFilePath, pCurItem->wszTag, pCurItem->bIsEncrypted, pCurItem->ulID, pCurItem->lCoverIndex);

						if (m_pIOSourceCtrlCallback)
						{
							strcpy(szFileName, WCharToChar(pCurItem->wszFilePath));
							sprintf(m_szErrorMsg, "Add() [%s] failed!! %d ", szFileName, iRet);
							m_pIOSourceCtrlCallback->Event(0, m_szErrorMsg);
						}
					}

					if (m_pIOSourceCtrlCallback&& pCurItem->bIsEncrypted)
						m_pIOSourceCtrlCallback->EndOpenFile(pCurItem->wszFilePath);
				}
			}
		}
		else
		{
			/*
			if (m_pIOSourceCtrlCallback)
			{
				sprintf(m_szErrorMsg, "pHeader->ulTotalOfItems = 0");
				m_pIOSourceCtrlCallback->Event(0, m_szErrorMsg);
			}
			*/
		}

		//for(j = 0;j < _MAX_TAGS;j++)
		//	delete pwszTags[j];
		m_pThumbnailCtrl->RemoveAll();

		for(int i = 0;i < m_pFileList->GetTotal();i++)
		{
			MediaFileInfo* pCurItem = (MediaFileInfo*)m_pFileList->Get(i);
			if(pCurItem->lCoverIndex >= 0)
				m_pThumbnailCtrl->SetUsed(pCurItem->lCoverIndex,true);
		}
		mfl_file.Close();
	}
	else
	{
		if (m_pIOSourceCtrlCallback)
		{
			sprintf(m_szErrorMsg, "mfl_file.Open failed!! iRet = %d", iRet);
			m_pIOSourceCtrlCallback->Event(0, m_szErrorMsg);
		}
	}
#else
	int iVersion = CheckMFFileVersion();
	if (iVersion == 1)
		Load_V1();
	else if (iVersion == 2)
		Load_V2();

	if (iVersion < 2)
		Save();

#endif
}

void MediaFileManager::Load_V1()
{
	char szMsg[512];
	char szAPPath[512];
	char szFileName[512];
	char szUrl[512];
	MFLFile mfl_file;

	GetExecutionPath(NULL, szAPPath);
	sprintf(szFileName, "%s\\FileList.mfl", szAPPath);

	//int iRet = mfl_file.Open("FileList.mfl");
	int iRet = mfl_file.Open(szFileName);
	if (iRet != -1)
	{
		int j;
		//wchar_t* pwszTags[_MAX_TAGS];
		//for(j = 0;j < _MAX_TAGS;j++)
		//pwszTags[j] = new wchar_t[64];

		FileHeader* pHeader = mfl_file.ReadHeader();
		if (!pHeader)
		{
			mfl_file.Close();

			if (m_pIOSourceCtrlCallback)
			{
				sprintf(szMsg, "pHeader == NULL !!");
				m_pIOSourceCtrlCallback->Event(0, szMsg);
			}
			return;
		}

		int iMFLItemLen = sizeof(MFLItem_V1);
		int iMediaFileInfoLen = sizeof(MediaFileInfo);
		CFileFind fFind;

		if (pHeader->ulTotalOfItems > 0)
		{
			for (int i = 0; i < pHeader->ulTotalOfItems; i++)
			{
				MFLItem_V1* pCurItem = mfl_file.ReadItem_V1();
				if (pCurItem)
				{
					//for(j = 0;j < _MAX_TAGS;j++)
					//wcscpy(pwszTags[j],pCurItem->wszTags[j]);

					if (m_pIOSourceCtrlCallback && pCurItem->bIsEncrypted)
					{
						if (fFind.FindFile(pCurItem->wszFilePath))
							m_pIOSourceCtrlCallback->BeginOpenFile(pCurItem->wszFilePath);
						else
							continue;
					}

					//					wsprintf(wszMsg, L"MediaFileManager::Load %s %u bIsEncrypted:%d\n", pCurItem->wszFilePath, pCurItem->ulID,pCurItem->bIsEncrypted);
					//					OutputDebugString(wszMsg);

					strcpy(szUrl, WCharToChar(pCurItem->wszFilePath));
					bool bIsRtsp = IsRTSPUrl(szUrl);
					if (bIsRtsp)
					{
						//int iRet = AddRTSPUrl2(pCurItem->wszFilePath, pCurItem->dlLen, pCurItem->ulID, pCurItem->lCoverIndex);
						int iRet = AddRTSPUrl2(pCurItem->wszFilePath, 30.0, pCurItem->ulID, pCurItem->lCoverIndex);
						if (iRet != 1)
						{
							if (m_pIOSourceCtrlCallback)
							{
								sprintf(m_szErrorMsg, "AddRTSPUrl2() [%s] failed!! %d ", szUrl, iRet);
								m_pIOSourceCtrlCallback->Event(0, m_szErrorMsg);
							}
						}
					}
					else
					{
						int iRet = Add(pCurItem->wszFilePath, pCurItem->wszTag, pCurItem->bIsEncrypted, pCurItem->ulID, pCurItem->lCoverIndex);

						if (m_pIOSourceCtrlCallback)
						{
							strcpy(szFileName, WCharToChar(pCurItem->wszFilePath));
							sprintf(m_szErrorMsg, "Add() [%s] failed!! %d ", szFileName, iRet);
							m_pIOSourceCtrlCallback->Event(0, m_szErrorMsg);
						}
					}

					if (m_pIOSourceCtrlCallback&& pCurItem->bIsEncrypted)
						m_pIOSourceCtrlCallback->EndOpenFile(pCurItem->wszFilePath);
				}
			}
		}
		else
		{
			/*
			if (m_pIOSourceCtrlCallback)
			{
			sprintf(m_szErrorMsg, "pHeader->ulTotalOfItems = 0");
			m_pIOSourceCtrlCallback->Event(0, m_szErrorMsg);
			}
			*/
		}

		//for(j = 0;j < _MAX_TAGS;j++)
		//	delete pwszTags[j];
		m_pThumbnailCtrl->RemoveAll();

		for (int i = 0; i < m_pFileList->GetTotal(); i++)
		{
			MediaFileInfo* pCurItem = (MediaFileInfo*)m_pFileList->Get(i);
			if (pCurItem->lCoverIndex >= 0)
				m_pThumbnailCtrl->SetUsed(pCurItem->lCoverIndex, true);
		}
		mfl_file.Close();
	}
	else
	{
		if (m_pIOSourceCtrlCallback)
		{
			sprintf(m_szErrorMsg, "mfl_file.Open failed!! iRet = %d", iRet);
			m_pIOSourceCtrlCallback->Event(0, m_szErrorMsg);
		}
	}
}

void MediaFileManager::Load_V2()
{
	char szMsg[512];
	char szAPPath[512];
	char szFileName[512];
	char szUrl[512];
	MFLFile mfl_file;

	GetExecutionPath(NULL, szAPPath);
	sprintf(szFileName, "%s\\FileList.mfl", szAPPath);

	//int iRet = mfl_file.Open("FileList.mfl");
	int iRet = mfl_file.Open(szFileName);
	if (iRet != -1)
	{
		int j;
		//wchar_t* pwszTags[_MAX_TAGS];
		//for(j = 0;j < _MAX_TAGS;j++)
		//pwszTags[j] = new wchar_t[64];

		FileHeader* pHeader = mfl_file.ReadHeader();
		if (!pHeader)
		{
			mfl_file.Close();

			if (m_pIOSourceCtrlCallback)
			{
				sprintf(szMsg, "pHeader == NULL !!");
				m_pIOSourceCtrlCallback->Event(0, szMsg);
			}
			return;
		}

		int iMFLItemLen = sizeof(MFLItem);
		int iMediaFileInfoLen = sizeof(MediaFileInfo);
		CFileFind fFind;

		if (pHeader->ulTotalOfItems > 0)
		{
			for (int i = 0; i < pHeader->ulTotalOfItems; i++)
			{
				MFLItem* pCurItem = mfl_file.ReadItem();
				if (pCurItem)
				{
					//for(j = 0;j < _MAX_TAGS;j++)
					//wcscpy(pwszTags[j],pCurItem->wszTags[j]);

					if (m_pIOSourceCtrlCallback && pCurItem->bIsEncrypted)
					{
						if (fFind.FindFile(pCurItem->wszFilePath))
							m_pIOSourceCtrlCallback->BeginOpenFile(pCurItem->wszFilePath);
						else
							continue;
					}

					//					wsprintf(wszMsg, L"MediaFileManager::Load %s %u bIsEncrypted:%d\n", pCurItem->wszFilePath, pCurItem->ulID,pCurItem->bIsEncrypted);
					//					OutputDebugString(wszMsg);

					strcpy(szUrl, WCharToChar(pCurItem->wszFilePath));
					bool bIsRtsp = IsRTSPUrl(szUrl);
					if (bIsRtsp)
					{
						//AddRTSPUrl(pCurItem->wszFilePath, pCurItem->dlLen);
						int iRet = AddRTSPUrl2(pCurItem->wszFilePath, pCurItem->dlLen, pCurItem->ulID, pCurItem->lCoverIndex);
						if (iRet != 1)
						{
							if (m_pIOSourceCtrlCallback)
							{
								sprintf(m_szErrorMsg, "AddRTSPUrl2() [%s] failed!! %d ", szUrl, iRet);
								m_pIOSourceCtrlCallback->Event(0, m_szErrorMsg);
							}
						}
					}
					else
					{
						int iRet = Add(pCurItem->wszFilePath, pCurItem->wszTag, pCurItem->bIsEncrypted, pCurItem->ulID, pCurItem->lCoverIndex);

						if (m_pIOSourceCtrlCallback)
						{
							strcpy(szFileName, WCharToChar(pCurItem->wszFilePath));
							sprintf(m_szErrorMsg, "Add() [%s] failed!! %d ", szFileName, iRet);
							m_pIOSourceCtrlCallback->Event(0, m_szErrorMsg);
						}
					}

					if (m_pIOSourceCtrlCallback&& pCurItem->bIsEncrypted)
						m_pIOSourceCtrlCallback->EndOpenFile(pCurItem->wszFilePath);
				}
			}
		}
		else
		{
			/*
			if (m_pIOSourceCtrlCallback)
			{
			sprintf(m_szErrorMsg, "pHeader->ulTotalOfItems = 0");
			m_pIOSourceCtrlCallback->Event(0, m_szErrorMsg);
			}
			*/
		}

		//for(j = 0;j < _MAX_TAGS;j++)
		//	delete pwszTags[j];
		m_pThumbnailCtrl->RemoveAll();

		for (int i = 0; i < m_pFileList->GetTotal(); i++)
		{
			MediaFileInfo* pCurItem = (MediaFileInfo*)m_pFileList->Get(i);
			if (pCurItem->lCoverIndex >= 0)
				m_pThumbnailCtrl->SetUsed(pCurItem->lCoverIndex, true);
		}
		mfl_file.Close();
	}
	else
	{
		if (m_pIOSourceCtrlCallback)
		{
			sprintf(m_szErrorMsg, "mfl_file.Open failed!! iRet = %d", iRet);
			m_pIOSourceCtrlCallback->Event(0, m_szErrorMsg);
		}
	}
}

int MediaFileManager::CheckMFFileVersion()
{
	char szMsg[512];
	char szAPPath[512];
	char szFileName[512];
	char szUrl[512];
	MFLFile mfl_file;
	bool bIsOldVersion = false;
	int iVersion = 0;

	GetExecutionPath(NULL, szAPPath);
	sprintf(szFileName, "%s\\FileList.mfl", szAPPath);

	int iRet = mfl_file.Open(szFileName);
	if (iRet != -1)
	{
		int j;
		FileHeader* pHeader = mfl_file.ReadHeader();
		if (!pHeader)
		{
			mfl_file.Close();

			if (m_pIOSourceCtrlCallback)
			{
				sprintf(szMsg, "pHeader == NULL !!");
				m_pIOSourceCtrlCallback->Event(0, szMsg);
			}
			return -1;
		}

		iVersion = pHeader->ulVersion;

		unsigned long ulMFFileLen = mfl_file.GetLength();

		int iMFLItemLen = sizeof(MFLItem);

		unsigned long ulTotalOfItemLen = iMFLItemLen * pHeader->ulTotalOfItems + sizeof(FileHeader);

		if (ulMFFileLen < ulTotalOfItemLen)
			bIsOldVersion = true;
		mfl_file.Close();
	}

	if(bIsOldVersion)
		return 1;
	{
		if (iVersion >= 2)
			return iVersion;
		return 2;
	}
}

void MediaFileManager::Save()
{
	wchar_t szMsg[512];
	char szAPPath[512];
	char szFileName[512];
	MFLFile mfl_file;

	GetExecutionPath(NULL, szAPPath);
	sprintf(szFileName, "%s\\FileList.mfl", szAPPath);

	//int iRet = mfl_file.Open("FileList.mfl",true);
	int iRet = mfl_file.Open(szFileName, true);
	if(iRet != -1)
	{
		mfl_file.WriteHeader(m_pFileList->GetTotal());
		for(int i = 0;i < m_pFileList->GetTotal();i++)
		{
			MediaFileInfo* pCurItem = (MediaFileInfo*)m_pFileList->Get(i);
			
			MFLItem mfl_item;
			wcscpy(mfl_item.wszFilePath,pCurItem->wszFileName);
			//for(int j = 0;j < _MAX_TAGS;j++)
				//wcscpy(mfl_item.wszTags[j],pCurItem->wszTags[j]);
			wcscpy(mfl_item.wszTag,pCurItem->wszTag);
			mfl_item.lCoverIndex = pCurItem->lCoverIndex;
			mfl_item.ulID = pCurItem->ulID;
			mfl_item.bIsEncrypted = pCurItem->bIsEncrypted;
			mfl_item.dlLen = pCurItem->dLen;

			//wsprintf(szMsg, L"MediaFileManager::Save %s %u\n", pCurItem->wszFileName, pCurItem->ulID);
			//OutputDebugString(szMsg);

			mfl_file.WriteItem(&mfl_item);
		}
		mfl_file.Close();
	}
}

int MediaFileManager::Add(wchar_t* wszFileName, bool bIsEncrypted)
{
#if 0
	int iIndex = Find(wszFileName);
	if(iIndex == -1)
	{
		MediaFileInfo* pNewItem = new MediaFileInfo;
		RestMediaFileInfo(pNewItem);

		int iRet = ParseMediaFileInfo(wszFileName,pNewItem);
		if(iRet < 0)
		{
			delete pNewItem;
			return -1;
		}

		pNewItem->iFileFormat = ParseExtensionName(wszFileName);

		m_pFileList->Add(pNewItem);
		CopyTo(pNewItem);
	}
	return 1;
#else
	//wchar_t* pwszTag[_MAX_TAGS] = {L"test",L"box",L"",L"",L""};
	wchar_t pwszTag[128] = L"";
	return Add(wszFileName,pwszTag, bIsEncrypted);

#endif
}

int MediaFileManager::AddRTSPUrl(wchar_t* wszUrl,double dLen)
{
	
	return AddRTSPUrl2(wszUrl, dLen);
}

int MediaFileManager::AddRTSPUrl2(wchar_t* wszUrl, double dLen, unsigned long ulID, long lCoverIndex)
{
	int iIndex = Find(wszUrl);
	if (iIndex == -1)
	{
		int iRet = OpenRTSPUrl(wszUrl);
		if (iRet == -1)
			return -1;
		Sleep(1000);

		MediaFileInfo* pNewItem = new MediaFileInfo;
		RestMediaFileInfo(pNewItem);

		wcscpy(pNewItem->wszFileFormat, L"RTSP");
		pNewItem->lCoverIndex = -1;
		pNewItem->bIsEncrypted = false;
		wcscpy(pNewItem->wszTag, L"");

		pNewItem->iFileFormat = MFF_RTSP;

		wcscpy(pNewItem->wszFileName, wszUrl);

		//long ThumbnailCtrl::AddPictureFromRTSP();

		if (m_pRTSPObj && m_pRTSPPlayerDll)
		{
			pNewItem->iVideoCodecID = m_pRTSPPlayerDll->_In_RTSP_GetVideoCodecID(m_pRTSPObj);
			wcscpy(pNewItem->wszVideoCodecName, ANSIToUnicode(m_pRTSPPlayerDll->_In_RTSP_GetVideoCodecName(m_pRTSPObj)));


			pNewItem->iAudioCodecID = m_pRTSPPlayerDll->_In_RTSP_GetAudioCodecID(m_pRTSPObj);
			wcscpy(pNewItem->wszAudioCodecName, ANSIToUnicode(m_pRTSPPlayerDll->_In_RTSP_GetAudioCodecName(m_pRTSPObj)));

			pNewItem->iH = m_pRTSPPlayerDll->_In_RTSP_GetVideoWidth(m_pRTSPObj);
			pNewItem->iW = m_pRTSPPlayerDll->_In_RTSP_GetVideoHeight(m_pRTSPObj);
		}

		CloseRTSPUrl();

		if (lCoverIndex == -1)
		{
			long lPicIndex = m_pThumbnailCtrl->AddPictureFromRTSP();
			pNewItem->lCoverIndex = lPicIndex;
		}
		else
		{
			pNewItem->lCoverIndex = lCoverIndex;
		}

		pNewItem->dLen = dLen;

		if (pNewItem->dLen > 0.0)
			wcscpy(pNewItem->wszLen, ANSIToUnicode(m_pFFMediaFileDll->PosToTimeInfo(pNewItem->dLen)));

		if (ulID == 0)
		{
			MakeID(pNewItem);
		}
		else
		{
			pNewItem->ulID = ulID;
		}

		m_pFileList->Add(pNewItem);
		CopyTo(pNewItem);
	}

	return 1;
}

int MediaFileManager::Add(wchar_t* wszFileName,wchar_t* wszTag, bool bIsEncrypted, unsigned long ulID,long lCoverIndex)
{
	wchar_t szMsg[512];
	int iIndex = Find(wszFileName);
	if(iIndex == -1)
	{
		m_iFileRet = OpenFile(wszFileName);
		if (m_iFileRet != 1)
			return -1;

		MediaFileInfo* pNewItem = new MediaFileInfo;
		RestMediaFileInfo(pNewItem);
		
		int iRet = ParseMediaFileInfo(wszFileName,pNewItem);
		if(iRet < 0)
		{
			delete pNewItem;
			return -1;
		}

		if(lCoverIndex == -1)
		{
			long lPicIndex = m_pThumbnailCtrl->AddPicture();
			pNewItem->lCoverIndex = lPicIndex;
		}
		else
		{
			pNewItem->lCoverIndex = lCoverIndex;
		}

		pNewItem->bIsEncrypted = bIsEncrypted;

		CloseFile();

		pNewItem->iFileFormat = ParseExtensionName(wszFileName);
		//for(int i = 0;i < _MAX_TAGS;i++)
			//wcscpy(pNewItem->wszTags[i],wszTags[i]);

		if (wcslen(wszTag) > _MAX_TAG_LEN - 1)
		{
			memcpy(pNewItem->wszTag, wszTag, _MAX_TAG_LEN - 1);
			pNewItem->wszTag[_MAX_TAG_LEN - 1] = L'\0';
		}
		else
			wcscpy(pNewItem->wszTag,wszTag);


		//int iLen = wcslen(wszString) - wcslen(pTemp);
		//memcpy(wszNumber, wszString, iLen * sizeof(wchar_t));
		//wszNumber[iLen] = L'\0';

		if (ulID == 0)
		{
			MakeID(pNewItem);

			//wsprintf(szMsg, L"MediaFileManager::Add %s %u (1)\n", pNewItem->wszFileName, pNewItem->ulID);
			//OutputDebugString(szMsg);
		}
		else
		{
			pNewItem->ulID = ulID;

			//wsprintf(szMsg, L"MediaFileManager::Add %s %u (2)\n", pNewItem->wszFileName, pNewItem->ulID);
			//OutputDebugString(szMsg);
		}

		m_pFileList->Add(pNewItem);
		CopyTo(pNewItem);
	}
	return 1;
}

void MediaFileManager::Delete(int iIndex)
{
	if(m_pFileList->GetTotal() > iIndex)
	{
		MediaFileInfo* pCurItem = (MediaFileInfo*)m_pFileList->Get(iIndex);
		if(pCurItem)
		{
			if(pCurItem->lCoverIndex != -1)
				m_pThumbnailCtrl->DeletePicture(pCurItem->lCoverIndex);
			delete pCurItem;
		}

		m_pFileList->Delete(iIndex);
	}
}

void MediaFileManager::Delete(wchar_t* wszFileName)
{
	int iIndex = Find(wszFileName);
	if (iIndex != -1)
	{
		Delete(iIndex);
	}
}

bool MediaFileManager::IsCharged(int iCodecID)
{
	if(AV_CODEC_ID_H265 == iCodecID)
		return true;
	else if(AV_CODEC_ID_MPEG2VIDEO == iCodecID)
		return true;
	return false;
}

int MediaFileManager::ParseExtensionName(wchar_t* wszFileName)
{
	wchar_t* pch = wcsrchr(wszFileName,'.');
	if(pch)
	{
		wchar_t wszTmpStr[256];
		wcscpy(wszTmpStr,pch+1);
		wcsupr(wszTmpStr);

		return GetFileFormatbyText(wszTmpStr);
		/*
		if(wcscmp(wszTmpStr,L"MP4") == 0)
			return MFF_MP4;
		else if(wcscmp(wszTmpStr,L"AVI") == 0)
			return MFF_AVI;
		else if(wcscmp(wszTmpStr,L"MKV") == 0)
			return MFF_MKV;
		else if(wcscmp(wszTmpStr,L"RMVB") == 0)
			return MFF_RMVB;
		else if(wcscmp(wszTmpStr,L"WMV") == 0)
			return MFF_WMV;
		else
			return MFF_OTHER;
			*/
	}
	return MFF_UNKNOW;
}

int MediaFileManager::Find(wchar_t* wszFileName)
{
	int iTotal = m_pFileList->GetTotal();
	for(int i = 0;i < iTotal;i++)
	{
		MediaFileInfo* pCurItem = (MediaFileInfo*)m_pFileList->Get(i);
		if(pCurItem)
		{
			if(wcscmp(pCurItem->wszFileName,wszFileName) == 0)
				return i;
		}
	}
	return -1;
}

void MediaFileManager::RestMediaFileInfo(MediaFileInfo* pInfo)
{
	wcscpy(pInfo->wszFileName,L"");
//	for(int i = 0;i < _MAX_TAGS;i++)
//		wcscpy(pInfo->wszTags[i],L"");
	wcscpy(pInfo->wszTag,L"");
	pInfo->iFileFormat = MFF_UNKNOW;
	pInfo->iVideoCodecID = AV_CODEC_ID_NONE;
	pInfo->iAudioCodecID = AV_CODEC_ID_NONE;
	pInfo->dLen = 0.0;
	pInfo->iW = 0;
	pInfo->iH = 0;
	pInfo->lCoverIndex = -1;

	wcscpy(pInfo->wszFileFormat,L"");
	wcscpy(pInfo->wszVideoCodecName,L"");
	wcscpy(pInfo->wszAudioCodecName,L"");
	wcscpy(pInfo->wszLen,L"");
}

int MediaFileManager::OpenFile(wchar_t* wszFileName)
{
	std::string sUTF8FileName = UnicodeToUTF8(wszFileName);
	//return m_pFFAVCodecDLL->MFR_Open((char*)sUTF8FileName.c_str());
	return m_pFFMediaFileDll->Open((char*)sUTF8FileName.c_str());
}

void MediaFileManager::CloseFile()
{
	if(m_iFileRet)
		//m_pFFAVCodecDLL->MFR_Close();
		m_pFFMediaFileDll->Close();
}

//cover
int MediaFileManager::ParseMediaFileInfo(wchar_t* wszFileName,MediaFileInfo* pInfo)
{
	//char* szFileName = WCharToChar(wszFileName);
#if 0
	std::string sUTF8FileName = UnicodeToUTF8(wszFileName);
	int iFileRet = m_pFFAVCodecDLL->MFR_Open((char*)sUTF8FileName.c_str());
	if(iFileRet == 1)
#else
	if(m_iFileRet == 1)
#endif
	{
		int w,h;
		//m_pFFAVCodecDLL->MFR_GetVideoFrameSize(&w,&h);
		m_pFFMediaFileDll->GetVideoFrameSize(&w, &h);
		//AVCodecContext* pVideoCodecContext = m_pFFAVCodecDLL->MFR_GetAVCodecContext(MFT_VIDEO);
		AVCodecContext* pVideoCodecContext = m_pFFMediaFileDll->GetAVCodecContext(MFT_VIDEO);
		if(pVideoCodecContext)
		{
			pInfo->iVideoCodecID = pVideoCodecContext->codec_id;
			//pVideoCodecContext->codec->name;
			wcscpy(pInfo->wszVideoCodecName,ANSIToUnicode(pVideoCodecContext->codec->name));
		}

		//AVCodecContext* pAudioCodecContext = m_pFFAVCodecDLL->MFR_GetAVCodecContext(MFT_AUDIO);
		AVCodecContext* pAudioCodecContext = m_pFFMediaFileDll->GetAVCodecContext(MFT_AUDIO);
		if(pAudioCodecContext)
		{
			pInfo->iAudioCodecID = pAudioCodecContext->codec_id;
			//pAudioCodecContext->codec->name;
			wcscpy(pInfo->wszAudioCodecName,ANSIToUnicode(pAudioCodecContext->codec->name));
		}

		if(pVideoCodecContext == 0 && pAudioCodecContext == 0)
		{
			//m_pFFAVCodecDLL->MFR_Close();
			m_pFFMediaFileDll->Close();
			return -2;
		}

		//pInfo->dLen = m_pFFAVCodecDLL->MFR_GetLength();
		pInfo->dLen = m_pFFMediaFileDll->GetLength();
		
		if(pInfo->dLen > 0.0)
			//wcscpy(pInfo->wszLen,ANSIToUnicode(m_pFFAVCodecDLL->PosToTimeInfo(pInfo->dLen)));
			wcscpy(pInfo->wszLen, ANSIToUnicode(m_pFFMediaFileDll->PosToTimeInfo(pInfo->dLen)));

		wcscpy(pInfo->wszFileName,wszFileName),

		pInfo->iH = h;
		pInfo->iW = w;

#if 0
		m_pFFAVCodecDLL->MFR_Close();
#endif
		return 1;
	}
	return -1;
}

void MediaFileManager::CreateLists()
{
	m_pFileList = new SQList;
	m_pFileList->EnableRemoveData(false);

	m_pMP4FileList = new SQList;
	m_pAVIFileList = new SQList;
	m_pMKVFileList = new SQList;
	m_pRMVBFileList = new SQList;
	m_pWMVFileList = new SQList;
	m_pOtherFileList = new SQList;
	m_pSearchResultsList = new SQList;
	m_pRTSPFileList = new SQList;
}

void MediaFileManager::DestoryLists()
{
	if(m_pFileList)
	{
		int iTotal = m_pFileList->GetTotal();
		for(int i = 0;i < iTotal;i++)
		{
			MediaFileInfo* pCurItem = (MediaFileInfo*)m_pFileList->Get(i);
			if(pCurItem)
				delete pCurItem;
		}
		delete m_pFileList;
	}

	if(m_pMP4FileList)
		delete m_pMP4FileList;

	if(m_pAVIFileList)
		delete m_pAVIFileList;

	if(m_pMKVFileList)
		delete m_pMKVFileList;

	if(m_pRMVBFileList)
		delete m_pRMVBFileList;

	if(m_pWMVFileList)
		delete m_pWMVFileList;

	if(m_pOtherFileList)
		delete m_pOtherFileList;

	if (m_pRTSPFileList)
		delete m_pRTSPFileList;
}

void MediaFileManager::CopyTo(MediaFileInfo* pInfo)
{
	CopyTo(pInfo->iFileFormat,pInfo);
}

void MediaFileManager::CopyTo(int iFileFormat,MediaFileInfo* pInfo)
{
	MediaFileInfoCopy* pNewItem = new MediaFileInfoCopy;
	pNewItem->pInfo = pInfo;
	switch(iFileFormat)
	{
		case MFF_OTHER:
			wcscpy(pInfo->wszFileFormat,L"Other");
			m_pOtherFileList->Add(pNewItem);
			break;
		case MFF_MP4:
			wcscpy(pInfo->wszFileFormat,L"MP4");
			m_pMP4FileList->Add(pNewItem);
			break;
		case MFF_AVI:
			wcscpy(pInfo->wszFileFormat,L"AVI");
			m_pAVIFileList->Add(pNewItem);
			break;
		case MFF_MKV:
			wcscpy(pInfo->wszFileFormat,L"MKV");
			m_pMKVFileList->Add(pNewItem);
			break;
		case MFF_RMVB:
			wcscpy(pInfo->wszFileFormat,L"RMVB");
			m_pRMVBFileList->Add(pNewItem);
			break;
		case MFF_WMV:
			wcscpy(pInfo->wszFileFormat,L"WMV");
			m_pWMVFileList->Add(pNewItem);
			break;
		case MFF_RTSP:
			wcscpy(pInfo->wszFileFormat, L"RTSP");
			m_pRTSPFileList->Add(pNewItem);
			break;
		case MFF_SEARCH_RESULTS:
			m_pSearchResultsList->Add(pNewItem);
			break;
		default:
			wcscpy(pInfo->wszFileFormat,L"Other");
			m_pOtherFileList->Add(pNewItem);
	}
}

SQList* MediaFileManager::GetFileList(int iFileFormat)
{
	switch(iFileFormat)
	{
		case MFF_OTHER:
			return m_pOtherFileList;
			break;
		case MFF_MP4:
			return m_pMP4FileList;
			break;
		case MFF_AVI:
			return m_pAVIFileList;
			break;
		case MFF_MKV:
			return m_pMKVFileList;
			break;
		case MFF_RMVB:
			return m_pRMVBFileList;
			break;
		case MFF_WMV:
			return m_pWMVFileList;
			break;
		case MFF_RTSP:
			return m_pRTSPFileList;
			break;
		case MFF_ALL:
			return m_pFileList;
			break;
		case MFF_SEARCH_RESULTS:
			return m_pSearchResultsList;
			break;
		default:
			return 0;
	}
}

int MediaFileManager::GetTotalOfFileList(int iFileFormat)
{
	SQList* pList = GetFileList(iFileFormat);
	if(pList)
		return pList->GetTotal();
	return 0;
}

MediaFileInfo* MediaFileManager::GetMediaFileInfo(int iFileFormat,int iIndex)
{
	SQList* pList = GetFileList(iFileFormat);
	if(pList && pList->GetTotal() > iIndex)
	{
		if(MFF_ALL == iFileFormat)
			return (MediaFileInfo*)pList->Get(iIndex);
		else
		{
			MediaFileInfoCopy* p = (MediaFileInfoCopy*)pList->Get(iIndex);
			return p->pInfo;
		}
	}
	return 0;
}

MediaFileInfo* MediaFileManager::GetMediaFileInfo(int iIndex)
{
	return GetMediaFileInfo(MFF_ALL,iIndex);
}

MediaFileInfo* MediaFileManager::GetMediaFileInfo(wchar_t* wszFileName)
{
	for(int i = 0;i < m_pFileList->GetTotal();i++)
	{
		MediaFileInfo* pCurItem = (MediaFileInfo*)m_pFileList->Get(i);
		if(pCurItem && wcscmp(pCurItem->wszFileName,wszFileName) == 0)
			return pCurItem;
	}
	return 0;
}

MediaFileInfo* MediaFileManager::GetMediaFileInfobyID(unsigned long ulID)
{
	for (int i = 0; i < m_pFileList->GetTotal(); i++)
	{
		MediaFileInfo* pCurItem = (MediaFileInfo*)m_pFileList->Get(i);
		if (pCurItem && pCurItem->ulID == ulID)
			return pCurItem;
	}
	return 0;
}

wchar_t* MediaFileManager::GetFileName(wchar_t* pLongPath)
{
	char szUrl[512];
	strcpy(szUrl, WCharToChar(pLongPath));
	bool bIsRtsp = IsRTSPUrl(szUrl);
	if (bIsRtsp)
	{
		wcscpy(m_wszFileName, GetShortUrl(pLongPath));
		return m_wszFileName;
	}
	else
	{
		wchar_t* pwch = wcsrchr(pLongPath, L'\\');
		if (pwch)
		{
			wcscpy(m_wszFileName, pwch + 1);
			return m_wszFileName;
		}
	}

	return L"";
}

int MediaFileManager::GetFileFormatbyText(wchar_t* wszText)
{
	if(wcscmp(wszText,L"MP4") == 0)
		return MFF_MP4;
	else if(wcscmp(wszText,L"AVI") == 0)
		return MFF_AVI;
	else if(wcscmp(wszText,L"MKV") == 0)
		return MFF_MKV;
	else if(wcscmp(wszText,L"RMVB") == 0)
		return MFF_RMVB;
	else if(wcscmp(wszText,L"WMV") == 0)
		return MFF_WMV;
	else if(wcscmp(wszText,L"ALL") == 0)
		return MFF_ALL;
	else if (wcscmp(wszText, L"RTSP") == 0)
		return MFF_RTSP;
	else if(wcscmp(wszText,L"OTHER") == 0)
		return MFF_OTHER;
	else
		return MFF_OTHER;
}

void MediaFileManager::DoTagSearch(wchar_t* wszInputKeyword)
{
	ParseKeywords(wszInputKeyword);

	m_pSearchResultsList->Reset();

	if(m_iKeywordCount > 0)
	{
		int iTotal = m_pFileList->GetTotal();
		for(int i = 0;i < iTotal;i++)
		{
			int iFindCount = 0;
			MediaFileInfo* pCurItem = (MediaFileInfo*)m_pFileList->Get(i);
			if(pCurItem)
			{
				/*
				for(int j = 0;j < _MAX_TAGS;j++)
				{
					for(int k = 0;k < m_iKeywordCount;k++)
					{
						if(wcscmp(pCurItem->wszTags[j],m_wszKeywords[k]) == 0)
						{
							iFindCount++;
							break;
						}
					}
				}
				*/
				for(int k = 0;k < m_iKeywordCount;k++)
				{
					//if(wcscmp(pCurItem->wszTag,m_wszKeywords[k]) == 0)
					if(wcsstr(pCurItem->wszTag,m_wszKeywords[k]))
					{
						iFindCount++;
//						break;
					}
				}

				if(iFindCount == m_iKeywordCount)
					CopyTo(MFF_SEARCH_RESULTS,pCurItem);
			}
		}
	}
}

void MediaFileManager::ParseKeywords(wchar_t* wszInputKeyword)
{
	wchar_t wszTmpKeyword[512];
	m_iKeywordCount = 0;
	int iIndex = 0;
	bool bDo = true;
	wcscpy(wszTmpKeyword,wszInputKeyword);

	while(bDo)
	{
		wchar_t* pwszCurPattern = GetNextPatternW(wszTmpKeyword,'+',&iIndex);
		if(wcscmp(pwszCurPattern,L"") == 0)
			bDo = false;
		else
		{
			if(m_iKeywordCount < 5)
			{
				wcscpy(m_wszKeywords[m_iKeywordCount],pwszCurPattern);
				m_iKeywordCount++;
			}
		}	
	}
}

ThumbnailPicture* MediaFileManager::GetPicture(MediaFileInfo* pInfo)
{
	return m_pThumbnailCtrl->GetPicture(pInfo->lCoverIndex);
}

void MediaFileManager::MakeID(MediaFileInfo* pInfo)
{
	UUID uuid;
	UuidCreate(&uuid);

	pInfo->ulID = uuid.Data1;
}

void MediaFileManager::Reset()
{
	DestoryLists();
	CreateLists();
	m_pThumbnailCtrl->RemoveAll();
}

int MediaFileManager::OpenRTSPUrl(wchar_t* wszUrl)
{
	char szUrl[512];
	char* WCharToChar(const std::wstring& str);
	strcpy(szUrl, WCharToChar(wszUrl));

	if (m_pRTSPObj && m_pRTSPPlayerDll)
	{
		int iRet = m_pRTSPPlayerDll->_In_RTSP_CheckNetworkStatus(m_pRTSPObj, szUrl);
		CloseRTSPUrl();
		if (iRet == 1)
			m_pRTSPPlayerDll->_In_RTSP_Open(m_pRTSPObj, szUrl);
		else
			return -1;
	}
	return 1;
}

void MediaFileManager::CloseRTSPUrl()
{
	if(m_pRTSPObj && m_pRTSPPlayerDll)
		m_pRTSPPlayerDll->_In_RTSP_Close(m_pRTSPObj);
}

wchar_t* MediaFileManager::GetShortUrl(wchar_t* wszUrl)
{
	char szUrl[512];
	char szPattern[64] = "rtsp://";
	char szSeparator[128];
	char szSuffix[128]; 
	int iPort;
	char szUserName[128];
	char szPW[128];

	wchar_t wszSeparator[128];
	wchar_t wszSuffix[128];

	char* WCharToChar(const std::wstring& str);
	strcpy(szUrl, WCharToChar(wszUrl));

	bool bRet = ParseUrl1(szUrl, szPattern, szSeparator, szSuffix, &iPort,szUserName,szPW);
	if (bRet)
	{
		wcscpy(wszSeparator, ANSIToUnicode(szSeparator));
		wcscpy(wszSuffix, ANSIToUnicode(szSuffix));

		wsprintf(m_wszFileName, L"%s/%s", wszSeparator, wszSuffix);
	}
	return m_wszFileName;
}
