#ifndef _ThumbnailCtrl_H
#define _ThumbnailCtrl_H

//#include "..\\..\\..\\Include\\FFAVCodecDLL.h"
#include "FFMediaFileDll.h"
#include "..\\..\\..\\Include\\MediaFileCommon.h"
#include "RTSPPlayerDll.h"

#define _MAX_THUMBNAIL_ITEMS 200000
#define _THUMBNAIL_FRAME_LEN _THUMBNAIL_FRAME_W*_THUMBNAIL_FRAME_H*3/2
#define _DATA_OFFSET 128

typedef struct
{
	unsigned long ulTotal;
	bool bIsUsed[_MAX_THUMBNAIL_ITEMS];
	unsigned long ulReserve[7];
}ThumbnailFileHeader;

class ThumbnailFile
{
		int m_iIndex;
		FILE* m_pFile;
		unsigned long m_ulFileLen;
		ThumbnailFileHeader m_FileHeader;
		ThumbnailPicture* m_pPicture;
	 public:
	    ThumbnailFile();
        ~ThumbnailFile();

		void Open(int iIndex);
		void Close();
		void Reset();
		unsigned long GetLength();
		void WriteHeader();
		ThumbnailFileHeader* ReadHeader();
		long GetNextAvailableIndex();
		void SetUsed(long lPicIndex,bool bUsed);
		void AddPicture(long lPicIndex,unsigned char* pBuffer,int iLen);
		void DeletePicture(long lPicIndex);
		ThumbnailPicture* GetPicture(long lPicIndex);
		void WritePicture(long lPicIndex,unsigned char* pBuffer,int iLen);
		void ReadPicture(long lPicIndex);
		bool Seek(unsigned long ulOffset);
		bool IsUsed(long lPicIndex);
};

class ThumbnailCtrl
{
		//FFAVCodecDLL* m_pFFAVCodecDLL;
		FFMediaFileDll* m_pFFMediaFileDll;
		ThumbnailFile* m_pThumbnailFile;

		RTSPPlayerDll* m_pRTSPPlayerDll;
		void* m_pRTSPObj;
   public:
	    ThumbnailCtrl();
        ~ThumbnailCtrl();

		//void SetFFAVCodecDLL(FFAVCodecDLL* pObj);
		void SetFFMediaFileDll(FFMediaFileDll* pObj);
		void SetRTSPPlayerDll(RTSPPlayerDll* pDllObj, void* pObj);
		void Open();
		void Close();
		ThumbnailPicture* GetPicture(long lPicIndex);
		long AddPicture();
		long AddPictureFromRTSP();
		void DeletePicture(long lPicIndex);
		void RemoveAll();
		void SetUsed(long lPicIndex,bool bUsed);
};

#endif
