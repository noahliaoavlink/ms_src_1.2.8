#ifndef _MFLFile_H
#define _MFLFile_H

#include <stdio.h>

#include "SimpleFileBase.h"
#include "..\\..\\Include\\MediaFileCommon.h"

//#define _MAX_TAGS 5
/*
typedef struct
{
	unsigned long ulVersion;
	unsigned long ulTotalOfItems;
	unsigned long ulReserve[10];
}MFLHeader;
*/

typedef struct
{
	wchar_t wszFilePath[512];
	wchar_t wszTag[_MAX_TAG_LEN];
	//wchar_t wszTags[_MAX_TAGS][64];
	long lCoverIndex;
	unsigned long ulID;
	long bIsEncrypted;
//	double dlLen;
}MFLItem_V1;

typedef struct
{
	wchar_t wszFilePath[512];
	wchar_t wszTag[_MAX_TAG_LEN];
	//wchar_t wszTags[_MAX_TAGS][64];
	long lCoverIndex;
	unsigned long ulID;
	long bIsEncrypted;
	double dlLen;
}MFLItem;

class MFLFile : public SimpleFileBase
{
		//FILE* m_pFile;
		//MFLHeader m_MFLHeader;
		MFLItem m_TmpMFLItem;
		MFLItem_V1 m_TmpMFLItem_V1;
   public:
	    MFLFile();
        ~MFLFile();

		/*
		int Open(char* szFileName,bool bIsWritable = false);
		void Close();
		void WriteHeader(int iTotal);
		MFLHeader* ReadHeader();
		*/
		void WriteItem(MFLItem* pItem);
		MFLItem* ReadItem();
		MFLItem_V1* ReadItem_V1();
};

#endif