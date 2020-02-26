#ifndef _MediaFileManager_H_
#define _MediaFileManager_H_

#include "..\\..\\Include\\SQList.h"
//#include "..\\..\\..\\Include\\FFAVCodecDLL.h"
#include "FFMediaFileDll.h"
#include "..\\..\\Include\\StrConv.h"
#include "..\\..\\Include\\SString.h"
#include "MFLFile.h"
#include "ThumbnailCtrl.h"

#include "..\\..\\Include\\MediaFileCommon.h"
#include "..\\..\\Include\\FilePlayerCommon.h"
#include "RTSPPlayerDll.h"

typedef struct
{
	MediaFileInfo* pInfo;
}MediaFileInfoCopy;


class MediaFileManager
{
		SQList* m_pFileList;

		SQList* m_pMP4FileList;
		SQList* m_pAVIFileList;
		SQList* m_pMKVFileList;
		SQList* m_pRMVBFileList;
		SQList* m_pWMVFileList;
		SQList* m_pRTSPFileList;
		SQList* m_pOtherFileList;
		SQList* m_pSearchResultsList;

		wchar_t m_wszFileName[256];

		wchar_t m_wszKeywords[5][64];
		int m_iKeywordCount;

		//FFAVCodecDLL* m_pFFAVCodecDLL;
		FFMediaFileDll* m_pFFMediaFileDll;
		ThumbnailCtrl* m_pThumbnailCtrl;
		int m_iFileRet;

		RTSPPlayerDll* m_pRTSPPlayerDll;
		void* m_pRTSPObj;

		char m_szErrorMsg[512];

		IOSourceCtrlCallback* m_pIOSourceCtrlCallback;
	public:
		MediaFileManager();
        ~MediaFileManager();

		void Init();
		void Load();
		void Load_V1();
		void Load_V2();
		int CheckMFFileVersion();
		void Save();

		int Add(wchar_t* wszFileName, bool bIsEncrypted = false);
		//int Add(wchar_t* wszFileName,wchar_t** wszTags,long lCoverIndex = -1);
		int Add(wchar_t* wszFileName,wchar_t* wszTag, bool IsEncrypted = false,unsigned long ulID = 0,long lCoverIndex = -1);
		void Delete(int iIndex);
		void Delete(wchar_t* wszFileName);
		bool IsCharged(int iCodecID);
		int ParseExtensionName(wchar_t* wszFileName);
		int Find(wchar_t* wszFileName);
		void RestMediaFileInfo(MediaFileInfo* pInfo);
		int OpenFile(wchar_t* wszFileName);
		void CloseFile();
		int ParseMediaFileInfo(wchar_t* wszFileName,MediaFileInfo* pInfo);
		void CreateLists();
		void DestoryLists();
		void CopyTo(MediaFileInfo* pInfo);
		void CopyTo(int iFileFormat,MediaFileInfo* pInfo);
		SQList* GetFileList(int iFileFormat);
		int GetTotalOfFileList(int iFileFormat = MFF_ALL);
		MediaFileInfo* GetMediaFileInfo(int iIndex);
		MediaFileInfo* GetMediaFileInfo(int iFileFormat,int iIndex);
		MediaFileInfo* GetMediaFileInfo(wchar_t* wszFileName);
		MediaFileInfo* GetMediaFileInfobyID(unsigned long ulID);
		wchar_t* GetFileName(wchar_t* pLongPath);
		int GetFileFormatbyText(wchar_t* wszText);
		void DoTagSearch(wchar_t* wszInputKeyword);
		void ParseKeywords(wchar_t* wszInputKeyword);
		ThumbnailPicture* GetPicture(MediaFileInfo* pInfo);
		void MakeID(MediaFileInfo* pInfo);
		void SetIOSourceCtrlCallback(void* pObj);
		void Reset();

		//RTSP 
		int AddRTSPUrl(wchar_t* wszUrl, double dLen);
		int AddRTSPUrl2(wchar_t* wszUrl, double dLen, unsigned long ulID = 0, long lCoverIndex = -1);
		int OpenRTSPUrl(wchar_t* wszUrl);
		void CloseRTSPUrl();
		wchar_t* GetShortUrl(wchar_t* wszUrl);
};

#endif
