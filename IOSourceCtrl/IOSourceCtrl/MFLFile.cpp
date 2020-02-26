#include "stdafx.h"
#include "MFLFile.h"

MFLFile::MFLFile()
{
}

MFLFile::~MFLFile()
{
}

void MFLFile::WriteItem(MFLItem* pItem)
{
	fwrite(pItem,1,sizeof(MFLItem),m_pFile);
}

MFLItem* MFLFile::ReadItem()
{
	int iRet = fread((void*) &m_TmpMFLItem,1,sizeof(MFLItem),m_pFile);
	if(iRet != sizeof(MFLItem))
		return 0;
	return &m_TmpMFLItem;
}

MFLItem_V1* MFLFile::ReadItem_V1()
{
	int iRet = fread((void*)&m_TmpMFLItem_V1, 1, sizeof(MFLItem_V1), m_pFile);
	if (iRet != sizeof(MFLItem_V1))
		return 0;
	return &m_TmpMFLItem_V1;
}