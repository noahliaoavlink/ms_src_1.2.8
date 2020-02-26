#include "stdafx.h"
#include "SimpleFileBase.h"

SimpleFileBase::SimpleFileBase()
{
	m_pFile = 0;
	m_FileHeader.ulVersion = 2;
	m_FileHeader.ulTotalOfItems = 0;
	for(int i = 0;i < 10;i++)
		m_FileHeader.ulReserve[i] = 0;
}

SimpleFileBase::~SimpleFileBase()
{
}

int SimpleFileBase::Open(char* szFileName,bool bIsWritable)
{
	char szMode[10];

	if(bIsWritable)
		strcpy(szMode,"wb");
	else
		strcpy(szMode,"rb");

	m_pFile  = fopen(szFileName, szMode);

	if(m_pFile == 0)
		return -1;
	return 1;
}

void SimpleFileBase::Close()
{
	if(m_pFile)
		fclose(m_pFile);
}

void SimpleFileBase::WriteHeader(int iTotal)
{
	m_FileHeader.ulTotalOfItems = iTotal;
	fwrite(&m_FileHeader,1,sizeof(FileHeader),m_pFile);
}

FileHeader* SimpleFileBase::ReadHeader()
{
	int iRet = fread((void*) &m_FileHeader,1,sizeof(FileHeader),m_pFile);
	if(iRet != sizeof(FileHeader))
		return 0;
	return &m_FileHeader;
}

unsigned long SimpleFileBase::GetLength()
{
	if (m_pFile)
	{
		unsigned long ulCurPos = ftell(m_pFile);
		fseek(m_pFile, 0, SEEK_END);
		m_ulFileLen = ftell(m_pFile);
		fseek(m_pFile, ulCurPos, SEEK_SET);
		return m_ulFileLen;
	}
	return 0;
}