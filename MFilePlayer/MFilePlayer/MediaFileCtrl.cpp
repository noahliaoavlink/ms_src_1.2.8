#include "stdafx.h"
#include "MediaFileCtrl.h"

MediaFileCtrl::MediaFileCtrl()
{
	m_pFFMediaFileList = new SQList;
	m_pFFMediaFileList->EnableRemoveData(false);

	m_pCurSelObj = 0;
}

MediaFileCtrl::~MediaFileCtrl()
{
	CloseAllFiles();

	int iTotal = m_pFFMediaFileList->GetTotal();
	for (int i = 0; i < iTotal; i++)
	{
		FFMediaFileDll* pCurItem = (FFMediaFileDll*)m_pFFMediaFileList->Get(i);
		if (pCurItem)
			delete pCurItem;
	}
	delete m_pFFMediaFileList;
}

void MediaFileCtrl::CloseAllFiles()
{
	int iTotal = m_pFFMediaFileList->GetTotal();
	for (int i = 0; i < iTotal; i++)
	{
		FFMediaFileDll* pCurItem = (FFMediaFileDll*)m_pFFMediaFileList->Get(i);
		if (pCurItem)
			pCurItem->Close();
	}
}

int MediaFileCtrl::SearchFileName(char* szFileName)
{
	for (int i = 0; i < m_pFFMediaFileList->GetTotal(); i++)
	{
		FFMediaFileDll* pCurItem = (FFMediaFileDll*)m_pFFMediaFileList->Get(i);
		if (pCurItem)
		{
			if (strcmp(pCurItem->GetFileName(), szFileName) == 0)
				return i;
		}
	}
	return -1;
}

int MediaFileCtrl::Open(char* szFileName)
{
	return m_pCurSelObj->Open(szFileName);
	/*
	int iIndex = SearchFileName(szFileName);
	if (iIndex == -1)
		return CreateNewObj(szFileName);
	else
		m_pCurSelObj = (FFMediaFileDll*)m_pFFMediaFileList->Get(iIndex);
	return 1;
	*/
}

void MediaFileCtrl::Close()
{
	//skip
}

FFMediaFileDll* MediaFileCtrl::GetCurSelObj()
{
	return m_pCurSelObj;
}

int MediaFileCtrl::CreateNewObj()
{
	FFMediaFileDll* pNewItem = new FFMediaFileDll;
	pNewItem->LoadLib();

	m_pFFMediaFileList->Add(pNewItem);

	m_pCurSelObj = pNewItem;
	return 1;
//	return pNewItem->Open(szFileName);
}

void MediaFileCtrl::SetTarget(int iIndex)
{
	FFMediaFileDll* pCurItem = (FFMediaFileDll*)m_pFFMediaFileList->Get(iIndex);
	m_pCurSelObj = pCurItem;
}
