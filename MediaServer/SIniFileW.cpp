#include "stdafx.h"
#include "SIniFileW.h"

//#include "..\\..\\..\\Include\\SString.h"

#define _INI_BUFFER_SIZE 2048

#define _STR_BUFFER_SIZE 2048
static wchar_t wszResult[_STR_BUFFER_SIZE];
__inline wchar_t* RemoveCharW(wchar_t* wszData, wchar_t cChar, bool bAll = false)
{
	int iCount = 0;
	wcscpy(wszResult, L"");
	int iTotal = wcslen(wszData);

	bool bRemove = true;

	for (int i = 0; i < iTotal; i++)
	{
		if (bAll)
		{
			if (wszData[i] != cChar)
				wszResult[iCount++] = wszData[i];
		}
		else
		{
			if (bRemove)
			{
				if (wszData[i] != cChar)
				{
					wszResult[iCount++] = wszData[i];
					bRemove = false;
				}
			}
			else
				wszResult[iCount++] = wszData[i];
		}
	}

	if (iCount > 0)
		wszResult[iCount] = L'\0';

	return wszResult;
}


SIniFileW::SIniFileW()
{
	m_wszResult = new wchar_t[_INI_BUFFER_SIZE];
	m_wszCurrentSection = new wchar_t[_INI_BUFFER_SIZE];
	m_wszFileName = new wchar_t[_INI_BUFFER_SIZE];
	m_wszCurrentItem = new wchar_t[_INI_BUFFER_SIZE];
	m_wszCurrentData = new wchar_t[_INI_BUFFER_SIZE];
	wcscpy(m_wszCurrentSection,L"");
	wcscpy(m_wszCurrentItem,L"");
	wcscpy(m_wszCurrentData,L"");

	m_iCurSectionStartIndex = 0;
	m_iCurSectionEndIndex = 0;
	m_iRet = 0;

	m_pSectionInfo = new SQList;
}

SIniFileW::~SIniFileW()
{
	delete m_wszResult;
	delete m_wszCurrentSection;
	delete m_wszFileName;
	delete m_wszCurrentItem;
	delete m_wszCurrentData;

	delete m_pSectionInfo;
}

bool SIniFileW::Open(wchar_t* szFileName,bool bTextMode,bool bWrite)
{
    bool bOK = SFileW::Open(szFileName,bTextMode,bWrite);
    wcscpy(m_wszFileName,szFileName);

	if(!bOK)
		return false;
	//if(!bWrite)
	//SFile::ReadToBuffer();
	//ParseSectionInfo();
	ReadToBuffer();

	//if(bOK)
	return true;
	//return false;
}

void SIniFileW::ReadToBuffer()
{
	SFileW::ReadToBuffer();
	ParseSectionInfo();
}

void SIniFileW::Close()
{
	wcscpy(m_wszCurrentSection,L"");
	wcscpy(m_wszCurrentItem,L"");
	SFileW::CleanBuffer();
    SFileW::Close();
}

void SIniFileW::ParseSectionInfo()
{
	m_pSectionInfo->Reset();
	unsigned long ulTotal = GetTotal();
	ResetCurIndex();
	int iCount = 0;
	for(unsigned long i = 0;i < ulTotal;i++)
	{
#if 1
		wchar_t* wszData = GetNext();

		wchar_t wszCurData[512];
		//wcscpy(wszCurData,RemoveCharW(wszData,' ',true));
		wcscpy(wszCurData, wszData);

		if(IsSection(wszCurData))
		{
			SectionInfoW* pSectionInfo = new SectionInfoW;

			int iLen = wcslen(wszCurData) - 2;
			//strcpy(pSectionInfo->szName,szData);
			if(wszCurData[0] == 65279 && iLen > 1)
			{
				memcpy(pSectionInfo->wszName,wszCurData + 2,iLen*sizeof(wchar_t));
				iLen -= 1;
			}
			else
				memcpy(pSectionInfo->wszName,wszCurData + 1,iLen*sizeof(wchar_t));
			pSectionInfo->wszName[iLen] = L'\0';
			pSectionInfo->iStart = i;
			pSectionInfo->iEnd = 0;
			m_pSectionInfo->Add(pSectionInfo);

			if(iCount > 0)
			{
                SectionInfoW* pPreviousSectionInfo = (SectionInfoW*)m_pSectionInfo->Get(iCount - 1);
				pPreviousSectionInfo->iEnd = i - 1;
			}
            iCount++;
#else
		wchar_t* wszData = GetNext();
		if(IsSection(wszData))
		{
			SectionInfo* pSectionInfo = new SectionInfo;

			int iLen = wcslen(wszData) - 2;
			//strcpy(pSectionInfo->szName,szData);
			memcpy(pSectionInfo->wszName,wszData + 1,iLen*sizeof(wchar_t));
			pSectionInfo->wszName[iLen] = L'\0';
			pSectionInfo->iStart = i;
			pSectionInfo->iEnd = 0;
			m_pSectionInfo->Add(pSectionInfo);

			if(iCount > 0)
			{
                SectionInfo* pPreviousSectionInfo = (SectionInfo*)m_pSectionInfo->Get(iCount - 1);
				pPreviousSectionInfo->iEnd = i - 1;
			}
            iCount++;
#endif
		}
	}

	if(iCount > 0)
	{
	    SectionInfoW* pLastSectionInfo = (SectionInfoW*)m_pSectionInfo->Get(iCount - 1);
	    pLastSectionInfo->iEnd = ulTotal;
	}
	ResetCurIndex();
}

bool SIniFileW::IsSection(wchar_t* wszData)
{
	wchar_t wszOutCommand[_INI_BUFFER_SIZE];
	wchar_t wszOutData[_INI_BUFFER_SIZE];
	int iRet = ParseData(wszData,wszOutCommand,wszOutData);
	if(iRet == 2)
		return true;
	return false;
}

int SIniFileW::ParseData(wchar_t* wszInData,wchar_t* wszOutCommand,wchar_t* wszOutData)
{
	int iInLen = wcslen(wszInData);
    if(wszInData[0] == L';')
	    return 0;
	else if(wszInData[0] == 65279)
	{
		if(wszInData[1] == L'[' && wszInData[iInLen - 1] == L']')
		{
			int iLen = wcslen(wszInData);
			wcsncpy(wszOutCommand,wszInData + 2,iLen - 2);
			wszOutCommand[iLen - 2] = '\0';
			return 2;
		}
	}
	else if(wszInData[0] == L'[' && wszInData[iInLen - 1] == L']')
	{
		int iLen = wcslen(wszInData);
		wcsncpy(wszOutCommand,wszInData + 1,iLen - 2);
		wszOutCommand[iLen - 2] = '\0';
		return 2;
	}
	else
	{
        //char* pData = strrchr(szInData,'=');
		wchar_t* pData = wcsrchr(wszInData,L'=');
        if(pData == 0)
			return 0;

		wchar_t* wszTemp = new wchar_t[_INI_BUFFER_SIZE];

		int iLen = wcslen(pData);
		int iTotalLen = wcslen(wszInData);

		if(iTotalLen - iLen <= 0)
		    return 0;

		wcsncpy(wszTemp,wszInData,iTotalLen - iLen);
		
		wszTemp[iTotalLen - iLen] = L'\0';
		
		wcscpy(wszOutCommand,RemoveCharW(wszTemp,' ',true));
		wcscpy(wszTemp,pData + 1);
		wcscpy(wszOutData,RemoveCharW(wszTemp,' ',false));

		delete wszTemp;

		return 1;
	}
	return 0;
}

bool SIniFileW::GetSection(wchar_t* wszSection)
{
	int iStart;
	int iEnd;

	if(GetSectionIndex(wszSection,&iStart,&iEnd))
	{
		SetCurSectionStartIndex(iStart);
        SetCurSectionEndIndex(iEnd);

		SetCurIndex(iStart + 1);
		wcscpy(m_wszCurrentSection,wszSection);
		return true;
	}
	return false;
}

wchar_t* SIniFileW::GetData(wchar_t* wszItem,wchar_t* wszDefault)
{
	int iLen = 0;
    wchar_t wszOutCommand[_INI_BUFFER_SIZE];
    wchar_t wszOutData[_INI_BUFFER_SIZE];

	int iCurIndex = GetCurIndex();
	if(iCurIndex < m_iCurSectionStartIndex)
		SetCurIndex(m_iCurSectionStartIndex);
	else if(iCurIndex >= m_iCurSectionEndIndex)
		SetCurIndex(m_iCurSectionStartIndex);

    wchar_t* wszData = GetNext();

	iLen = wcslen(wszData);

	if(!wszData || iLen >= _INI_BUFFER_SIZE)
	{
		m_iRet = -1;
		return wszDefault;
		//return "";
	}

    if(wcscmp(wszData,L"") == 0)
		ResetCurIndex();

	ParseData(wszData,wszOutCommand,wszOutData);
	if(wcscmp(wszOutCommand,wszItem) == 0)
	{
		wcscpy(m_wszResult,wszOutData);
		m_iRet = 0;
		return m_wszResult;
	}
	else
		SetCurIndex(m_iCurSectionStartIndex);

    for(int i = m_iCurSectionStartIndex;i <= m_iCurSectionEndIndex;i++)
	{
		SetCurIndex(i);
		wchar_t* wszData = GetNext();

		iLen = wcslen(wszData);

		if(!wszData || iLen >= _INI_BUFFER_SIZE)
		{
			m_iRet = -1;
			return wszDefault;
			//return "";
		}

		if(wcscmp(wszData,L"") != 0)
		{
	       ParseData(wszData,wszOutCommand,wszOutData);
	       if(wcscmp(wszOutCommand,wszItem) == 0)
		   {
			   wcscpy(m_wszResult,wszOutData);
			   m_iRet = 0;
		       return m_wszResult;
		   }
		}
	}
	m_iRet = -1;
	//return "";
	return wszDefault;
}

bool SIniFileW::GetSectionIndex(wchar_t* wszSection,int* iStart,int* iEnd)
{
	int iTotal = m_pSectionInfo->GetTotal();
	for(int i = 0;i < iTotal;i++)
	{
       SectionInfoW* pSectionInfo = (SectionInfoW*)m_pSectionInfo->Get(i);
	   if(wcscmp(pSectionInfo->wszName,wszSection) == 0)
	   {
		   *iStart = pSectionInfo->iStart;
		   *iEnd = pSectionInfo->iEnd;
		   return true;
	   }
	}
	return false;
}

void SIniFileW::SetCurSectionStartIndex(int iIndex)
{
	m_iCurSectionStartIndex = iIndex;
}

int SIniFileW::GetCurSectionStartIndex()
{
	return m_iCurSectionStartIndex;
}

void SIniFileW::SetCurSectionEndIndex(int iIndex)
{
	m_iCurSectionEndIndex = iIndex;
}

int SIniFileW::GetCurSectionEndIndex()
{
	return m_iCurSectionEndIndex;
}

wchar_t* SIniFileW::GetItemValue(wchar_t* wszSection,wchar_t* wszItem,wchar_t* wszDefault)
{
	/*
    if(wcscmp(m_wszCurrentSection,wszSection) == 0 
		&& wcscmp(m_wszCurrentItem,wszItem) == 0 
		&& wcscmp(m_wszCurrentData, L"") != 0)
	    return m_wszCurrentData;
    else */if(wcscmp(m_wszCurrentSection,wszSection) == 0)
    {
		wcscpy(m_wszCurrentItem,wszItem);
		wcscpy(m_wszCurrentData,GetData(wszItem,wszDefault));
	    return m_wszCurrentData;
    }
	else
	{
		if(!GetSection(wszSection))
		{
			m_iRet = -1;
			return wszDefault;
			//return "";
		}

		//else
		//{
			wcscpy(m_wszCurrentItem,wszItem);
			wcscpy(m_wszCurrentData,GetData(wszItem,wszDefault));
            return m_wszCurrentData;
		//}
	}
}

bool SIniFileW::SetItemData(wchar_t* wszSection,wchar_t* wszItem,wchar_t* wszNewData)
{
    wchar_t* pItemData = GetItemValue(wszSection,wszItem);
	if(wcscmp(pItemData,L"") == 0 && m_iRet == -1)
	//if(strcmp(pItemData,"") == 0)
	{
		if(!GetSection(wszSection))
		{
	        unsigned long ulTotal = GetTotal();
		    WriteNewSection(wszSection);
		    WriteNewItem(wszItem,wszNewData);
		    AddNewSectionInfo(wszSection,ulTotal,ulTotal + 2);
		}
		else
		{
			WriteNewItem(wszItem,wszNewData,true);
			ShiftSectionInfo(wszSection);
		}
		return true;
	}
    else if(pItemData)
    {
        int iIndex = GetCurIndex();
    	SetCurIndex(iIndex - 1);
    	wchar_t* wszData = GetNext();
		int iLen = wcslen(wszData);
		int iCurLen = wcslen(wszItem) + wcslen(wszNewData) + 3;
		if(iCurLen > iLen)
		{
			RemoveFromBuffer(iIndex - 1);
			wchar_t* wszNewBuffer = new wchar_t[iCurLen + 10];
			swprintf(wszNewBuffer,L"%s = %s",wszItem,wszNewData);
			InsertDataToBuffer(iIndex - 1,wszNewBuffer);
			delete wszNewBuffer;
		}
		else
    	    swprintf(wszData,L"%s = %s",wszItem,wszNewData);
    	return true;
    }
    return false;
}

void SIniFileW::WriteNewSection(wchar_t* wszSection)
{
	wchar_t wszNewSection[_INI_BUFFER_SIZE];
	swprintf(wszNewSection,L"[%s]",wszSection);
    SFileW::AddDataToBuffer(wszNewSection);
}

void SIniFileW::WriteNewItem(wchar_t* wszItem,wchar_t* wszData,bool bInsert)
{
	wchar_t wszNewItem[_INI_BUFFER_SIZE];
	swprintf(wszNewItem,L"%s = %s",wszItem,wszData);
	if(bInsert)
	{
	    SFileW::InsertDataToBuffer(m_iCurSectionEndIndex,wszNewItem);
		SetCurSectionEndIndex(GetCurSectionEndIndex() + 1);
	}
	else
	{
		SFileW::AddDataToBuffer(wszNewItem);
		//SetCurSectionEndIndex(GetCurSectionEndIndex() + 1);
	}
}

void SIniFileW::AddNewSectionInfo(wchar_t* wszSection,int iStart,int iEnd)
{
	SectionInfoW* pSectionInfo = new SectionInfoW;
	wcscpy(pSectionInfo->wszName,wszSection);
	pSectionInfo->iStart = iStart;
	pSectionInfo->iEnd = iEnd;
	m_pSectionInfo->Add(pSectionInfo);
}

void SIniFileW::ShiftSectionInfo(wchar_t* wszSection)
{
	bool bFind = false;
	int iTotal = m_pSectionInfo->GetTotal();
	for(int i = 0;i < iTotal;i++)
	{
        SectionInfoW* pSectionInfo = (SectionInfoW*)m_pSectionInfo->Get(i);

		if(bFind)
		{
			pSectionInfo->iStart++;
            pSectionInfo->iEnd++;
		}
		else if(wcscmp(pSectionInfo->wszName,wszSection) == 0)
		{
            bFind = true;
			pSectionInfo->iEnd++;
		}

	}
}

void SIniFileW::Write(bool bTextMode)
{
    SFileW::Close();
	if(bTextMode)
        SFileW::Open1(m_wszFileName,L"wt");
	else
		SFileW::Open1(m_wszFileName,L"wb");
    SFileW::Write(true);
}