#include "stdafx.h"
#include "FileTrimmer.h"
#include "..\\..\\Include\\SString.h"

FileTrimmer::FileTrimmer()
{
	m_pFileList = new SQList;
	m_pRemoveList = new SQList;
	m_iReserve = 30;//180;    //保留30天
}

FileTrimmer::~FileTrimmer()
{
	int i;
	for(i = 0;i < m_pFileList->GetTotal();i++)
	{
		char* pCurItem = (char*)m_pFileList->Get(i);
		delete pCurItem;
	}
	m_pFileList->EnableRemoveData(false);
	m_pFileList->Reset();
	delete m_pFileList;

	for(i = 0;i < m_pRemoveList->GetTotal();i++)
	{
		char* pCurItem = (char*)m_pRemoveList->Get(i);
		delete pCurItem;
	}
	m_pRemoveList->EnableRemoveData(false);
	m_pRemoveList->Reset();
	delete m_pRemoveList;

}

void FileTrimmer::SetPath(char* szPath)
{
	strcpy(m_szPath, szPath);
}

void FileTrimmer::SearchAllFiles(char* szFolder,SQList* pList)
{
	BOOL bFind = TRUE;
	WIN32_FIND_DATAA FindData;
	FindData.dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;
	char* szPath =(char*) new char[MAX_PATH];
	sprintf(szPath,"%s\\*.*",szFolder);
	HANDLE hFind = FindFirstFileA(szPath,&FindData);
	if(hFind == INVALID_HANDLE_VALUE)
	{
		//MessageBox(NULL,"Find first file error!!","error",MB_OK);
		return ;
	}
	else
	{
		bFind = FindNextFileA(hFind,&FindData);  //remove the .. sub folder
		while(bFind)
		{
		   bFind = FindNextFileA(hFind,&FindData);
		   if(bFind)
		   {
               //if(FindSubStr(_strupr(FindData.cFileName),_strupr(szFileName)))
			   //{
				   char* szNewFile = new char[strlen(FindData.cFileName) + 1];
				   strcpy(szNewFile,FindData.cFileName);
				   pList->Add(szNewFile);
			   //}
		   }
		   //Add(m_iCount++,FindData.cFileName);
		}
	}
	FindClose(hFind);
	delete szPath;
}

void FileTrimmer::Sort()
{
	char* pAItem;
	char* pBItem;
	char* pCurItem = new char[256];
	char* pNextItem = new char[256];
	char* pTempItem = new char[256];
    unsigned int i = 0; 
//	int iCurrent;
	int iIndex = 0;
	if(!m_pFileList->GetTotal())
		return ;
	for(i = 0;i < m_pFileList->GetTotal() - 1;i++)
	{
		//memcpy(pCurItem,(char*)m_pFileList->Get(i),sizeof(char));
		strcpy(pCurItem,(char*)m_pFileList->Get(i));
		iIndex = i;
		for(unsigned int j = i + 1;j < m_pFileList->GetTotal();j++)
		{
			//memcpy(pNextItem,(char*)m_pFileList->Get(j),sizeof(char));
			strcpy(pNextItem,(char*)m_pFileList->Get(j));
			if(Compare(pNextItem,pCurItem) == 1)
			{
				iIndex = j;
				//memcpy(pCurItem,pNextItem,sizeof(char));
				strcpy(pCurItem,pNextItem);
			}
		}
		//memcpy(pTempItem,(char*)m_pFileList->Get(i),sizeof(char));
		strcpy(pTempItem,(char*)m_pFileList->Get(i));
		pAItem = (char*)m_pFileList->Get(i);
		pBItem = (char*)m_pFileList->Get(iIndex);
		//memcpy(pAItem,pBItem,sizeof(char));
		//memcpy(pBItem,pTempItem,sizeof(char));
		strcpy(pAItem,pBItem);
		strcpy(pBItem,pTempItem);
	}
	delete pCurItem;
	delete pNextItem;
	delete pTempItem;
/*
	for(i = 0;i < m_pFileList->GetTotal();i++)
	{
		char* pCurItem = (char*)m_pFileList->Get(i);
	}
	*/
}

/*
比較最後修改時間
回傳值:
0  -> A = B
-1 -> A > B
1  -> A < B
*/
int FileTrimmer::Compare(char* pAItem,char* pBItem)
{
	long lNum1 = FileToNum(pAItem);
	long lNum2 = FileToNum(pBItem);

	if(lNum1 > lNum2)
		return -1;
	else if(lNum1 < lNum2)
		return 1;
	return 0;
}

long FileTrimmer::FileToNum(char* szFileName)
{
	int iYear,iMonth,iDay;
	char szText[10];
	sscanf(szFileName,"%d_%d_%d.%s",&iYear,&iMonth,&iDay,szText);

	return iYear * 400 + iMonth * 31 + iDay;
}

void FileTrimmer::MakeRemoveList()
{
	if(m_pFileList->GetTotal() > m_iReserve)
	{
		int iRemoveNum = m_pFileList->GetTotal() - m_iReserve;
		for(int i = 0;i < iRemoveNum;i++)
		{
			char* pCurItem = (char*)m_pFileList->Get(i);
			if(pCurItem)
			{
				int iLen = strlen(pCurItem);
				char* pNewItem = new char[iLen + 1];
				strcpy(pNewItem,pCurItem);
				m_pRemoveList->Add(pNewItem);
			}
		}
	}
}

void FileTrimmer::RemoveFiles()
{
	char szFileName[256];
	//char szCurDir[256];

	if(m_pRemoveList->GetTotal() == 0)
		return ;

	//GetExecutionPath(NULL,szCurDir);
	for(int i = 0;i < m_pRemoveList->GetTotal();i++)
	{
		sprintf(szFileName,"%s\\%s", m_szPath,m_pRemoveList->Get(i));
		::DeleteFileA(szFileName);
	}
}

void FileTrimmer::DoRemoveFiles()
{
	char szFolder[256];
	//char szCurDir[256];
	m_pFileList->Reset();
	m_pRemoveList->Reset();
    //GetExecutionPath(NULL,szCurDir); 
	sprintf(szFolder,"%s", m_szPath);
	SearchAllFiles(szFolder,m_pFileList);
	Sort();
	MakeRemoveList();
	RemoveFiles();
}

