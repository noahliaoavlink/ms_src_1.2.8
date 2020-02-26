#pragma once

#ifndef _CSVReader_H_
#define _CSVReader_H_

#include "SFile.h"
#include "SQList.h"

#define _MAX_TAGS 200

typedef struct
{
	char szFileFolder[512];
	char szFileName[256];
	bool bTags[_MAX_TAGS];
}TagInfo;

class CSVReader
{
	SFile* m_pSFile;
	SQList* m_pTitleItemList;
	SQList* m_pTagInfoList;
	char m_szTagString[1024];
	bool m_bOpened;

	char m_szTmpStr[256];
protected:
public:
	CSVReader();
	~CSVReader();

	int Open(char* szFileName);
	void Close();
	void ParseData();

	void ParseTitelItems(char* szData);
	void ParseTagInfo(char* szData);
	char* GetTagString(int iIndex);
	TagInfo* GetTagInfo(int iIndex);
	int GetTotal();
	int GetTotalOfTags();
	char* GetNextStr(char* szString, char ch);
	char* GetTagItemString(int iIndex);

	void Test();
};

#endif
