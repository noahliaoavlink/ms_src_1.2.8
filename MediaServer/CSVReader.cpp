#include "stdafx.h"
#include "CSVReader.h"

#include "../../../Include/sstring.h"


CSVReader::CSVReader()
{
	m_bOpened = false;

	m_pSFile = new SFile;

	m_pTitleItemList = new SQList;
	m_pTagInfoList = new SQList;

	m_pTitleItemList->EnableRemoveData(false);
	m_pTagInfoList->EnableRemoveData(false);
}

CSVReader::~CSVReader()
{
	for (int i = 0; i < m_pTitleItemList->GetTotal(); i++)
	{
		char* pCurInfo = (char*)m_pTitleItemList->Get(i);
		if (pCurInfo)
			delete pCurInfo;
	}

	delete m_pTitleItemList;

	for (int i = 0; i < m_pTagInfoList->GetTotal(); i++)
	{
		TagInfo* pCurInfo = (TagInfo*)m_pTagInfoList->Get(i);
		if (pCurInfo)
			delete pCurInfo;
	}

	delete m_pTagInfoList;

	if (m_pSFile)
		delete m_pSFile;
}

int CSVReader::Open(char* szFileName)
{
	bool bRet = m_pSFile->Open(szFileName,true,false);
	if (bRet)
	{
		m_pSFile->ReadToBuffer();
		m_bOpened = true;
		return 1;
	}
	m_bOpened = false;
	return 0;
}

void CSVReader::Close()
{
	m_pSFile->Close();
}

void CSVReader::ParseData()
{
	if (m_bOpened)
	{
		for (int i = 0; i < m_pSFile->GetTotal(); i++)
		{
			char* szCurItem = m_pSFile->GetNext();

			if (i == 0)
				ParseTitelItems(szCurItem);
			else
				ParseTagInfo(szCurItem);
		}
	}
}

void CSVReader::ParseTitelItems(char* szData)
{
	int iIndex = 0;
	bool bDo = true;
	char szItemData[256];
	int iLen = strlen(szData);
	char* szCurData = new char[iLen + 1];

	strcpy(szCurData, szData);

	m_pTitleItemList->Reset();

	while (bDo)
	{
		char szCurItem[256];
		char* szTmpItem = GetNextStr(szCurData, ',');
		if(strcmp(szTmpItem,"") == 0)
		//if (szCurItem == 0)
			bDo = false;
		else
		{
			char szTmpData[256];

			strcpy(szCurItem, szTmpItem);

			char* p = RemoveChar(szCurItem, '"', true);
			strcpy(szTmpData, p);
			p = RemoveChar(szTmpData, ' ', false);
			strcpy(szItemData, p);
			
			char* szNewItem = new char[strlen(szItemData) + 1];
			strcpy(szNewItem, szItemData);

			m_pTitleItemList->Add(szNewItem);
		}
	}
	delete szCurData;
}

void CSVReader::ParseTagInfo(char* szData)
{
	int iIndex = 0;
	bool bDo = true;
	char szItemData[256];
	int iLen = strlen(szData);
	char* szCurData = new char[iLen + 1];

	strcpy(szCurData, szData);

	TagInfo* pNewTagInfo = new TagInfo;
	int iCount = 0;

	for (int i = 0; i < _MAX_TAGS; i++)
		pNewTagInfo->bTags[i] = false;

	while (bDo)
	{
		char szCurItem[256] = "";
		char* szTmpItem = GetNextStr(szCurData, ',');
		if (strcmp(szTmpItem, "") == 0)
		//if (szTmpItem == 0)
			bDo = false;
		else
		{
			char szTmpData[256] = "";

			strcpy(szCurItem, szTmpItem);

			char* p = RemoveChar(szCurItem, '"', true);
			strcpy(szTmpData, p);
			p = RemoveChar(szTmpData, ' ', false);
			strcpy(szItemData, p);
			/*
			char szMsg[512];
			sprintf(szMsg,"ParseTagInfo - %d [%s]\n",  iCount, szItemData);
			OutputDebugStringA(szMsg);
			*/
			if(iCount == 0)
				strcpy(pNewTagInfo->szFileFolder, szItemData);
			else if (iCount == 1)
				strcpy(pNewTagInfo->szFileName, szItemData);
			else
			{
				if(strcmp(szItemData,"V") == 0)
					pNewTagInfo->bTags[iCount - 2] = true;
				else
					pNewTagInfo->bTags[iCount - 2] = false;
			}
			iCount++;
		}
	}

	m_pTagInfoList->Add(pNewTagInfo);
	delete szCurData;
}

char* CSVReader::GetTagString(int iIndex)
{
	strcpy(m_szTagString,"");

	int iCount = 0;
	if (m_pTagInfoList->GetTotal() > iIndex)
	{
		TagInfo* pCurInfo = (TagInfo*)m_pTagInfoList->Get(iIndex);
		if (pCurInfo)
		{
			int iTotalOfTags = m_pTitleItemList->GetTotal() - 2;
			for (int i = 0; i < iTotalOfTags; i++)
			{
				if (pCurInfo->bTags[i] == true)
				{
					if (iCount > 0)
						strcat(m_szTagString,",");

					char* szCurTag = (char*)m_pTitleItemList->Get(i + 2);
					strcat(m_szTagString, szCurTag);
					iCount++;
				}
			}

		}
	}
	return m_szTagString;
}

TagInfo* CSVReader::GetTagInfo(int iIndex)
{
	if (m_pTagInfoList->GetTotal() > iIndex)
	{
		return (TagInfo*)m_pTagInfoList->Get(iIndex);
	}
	return 0;
}

int CSVReader::GetTotal()
{
	return m_pTagInfoList->GetTotal();
}

int CSVReader::GetTotalOfTags()
{
	return m_pTitleItemList->GetTotal() - 2;
}

char* CSVReader::GetNextStr(char* szString, char ch)
{
	char* pTemp = strchr(szString, ch);
	if (pTemp)
	{
		int iLen = strlen(szString) - strlen(pTemp);
		memcpy(m_szTmpStr, szString, iLen);
		m_szTmpStr[iLen] = '\0';

		if (strlen(pTemp) > 1)
			strcpy(szString, pTemp + 1);
		else
			strcpy(szString, "");
	}
	else
	{
		strcpy(m_szTmpStr, szString);
		strcpy(szString, "");
	}
	return m_szTmpStr;
}

char* CSVReader::GetTagItemString(int iIndex)
{
	if(iIndex < m_pTitleItemList->GetTotal() - 2)
		return (char*)m_pTitleItemList->Get(iIndex + 2);
	return 0;
}

void CSVReader::Test()
{
	char szFileName[] = "D:\\noah_src\\TVWall\\Media-Tag-LUT.csv";
	int iRet = Open(szFileName);
	ParseData();
	Close();

	int iMax = 0;
	char szTag[512];
	for (int i = 0; i < GetTotal(); i++)
	{
		char* szCurTag = GetTagString(i);	
		int iLen = strlen(szCurTag);
		if (iLen > iMax)
		{
			iMax = iLen;
			strcpy(szTag, szCurTag);
		}
	}
	int kk = 0;
}