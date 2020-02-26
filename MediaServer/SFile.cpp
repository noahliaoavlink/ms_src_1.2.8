#include "stdafx.h"
#include <io.h>
#include <fcntl.h>
#include "SFile.h"
#include "..\\..\\..\\Include\\SString.h"
#define _FILE_BUFFER_SIZE 20480//2048

/*
int CreateSIFile(SIFile** pFile)
{
	SFile* pTemp;
	int iRet;

	if(pFile == 0)
		return 0;

	pTemp = new SFile();
	if(pTemp == 0)
		return 0;

	iRet = pTemp->QueryInterface(IID_SIFILE,(void**)pFile);
	return iRet;
}

int SFile::QueryInterface(int iID,void** ppv)
{
	*ppv = 0;
	if(IID_SIFILE == iID || IID_SIBASE == iID)
		*ppv = this;
	if(*ppv == 0)
		return 0;
	((SIBase*)*ppv)->IncRef();
	return 1;
}

int SFile::IncRef()
{
	return ++m_iRef;
}

int SFile::DecRef()
{
	if(0 != --m_iRef)
		return m_iRef;
	delete this;
	return 0;
}
*/
SFile::SFile()
{
	m_iRef = 0;
	m_pFile = 0;

	m_ulRead = _FILE_BUFFER_SIZE;
	m_nBufferSize = _FILE_BUFFER_SIZE;
	m_pBuffer = new unsigned char[_FILE_BUFFER_SIZE];
	m_ulMasterIndex = 0;
	m_ulIndex = 0;
	m_ulLine = 0;
	m_iCurIndex = 0;
	//CreateSIQList((SIList**) &m_pDataBuffer);
    //CreateSIQList((SIList**) &m_pMemBuffer);
	m_pDataBuffer = new SQList;
	m_pMemBuffer = new SQList;

	//MemBuffer
	m_iUnitSize = 0;
	m_bMemBufferMode = false;
	m_iCurMemIndex = 0;
	m_ulReadLen = 0;
}

SFile::~SFile()
{
	delete m_pBuffer;
	delete m_pDataBuffer;
	delete m_pMemBuffer;
}

bool SFile::Open(const char* szFileName,bool bTextMode,bool bWrite)
{
	char szMode[10];
	if(bWrite)
	{
		if(bTextMode)
		    strcpy(szMode,"w+t");
		else
			strcpy(szMode,"w+b");
	}
	else
	{
		if(bTextMode)
		    strcpy(szMode,"rt");
		else
			strcpy(szMode,"rb");
	}

	if((m_pFile  = fopen(szFileName, szMode)) == 0)
		return false;
	return true;
}

bool SFile::Open(const char* szFileName,char* szMode)
{
	if((m_pFile  = fopen(szFileName, szMode)) == 0)
		return false;
	return true;
}

void SFile::ReadToBuffer()
{
#if 0
	char* szBuffer;
	m_iCurIndex = 0;
    szBuffer = new char[m_nBufferSize];
    //strcpy(szBuffer,"");
    StrCpy(szBuffer,"");
    bool bDo = true;
	while(bDo)
	{
	    int iReturn = GetNextLine(szBuffer,m_nBufferSize);
	    if(iReturn > 0)
		{
			char* szData = (char*)new char[strlen(szBuffer) + 10];
			//strcpy(szData,szBuffer);
			StrCpy(szData,szBuffer);
			m_pDataBuffer->Add(szData);
		}
		else
		{
			if(strcmp(szBuffer,"") != 0)
			{
			    char* szData = (char*)new char[strlen(szBuffer) + 10];
				//strcpy(szData,szBuffer);
				StrCpy(szData,szBuffer);
			    m_pDataBuffer->Add(szData);
			}
			bDo = false;
		}
	}
	delete szBuffer;
#else
	char* szBuffer;
	m_iCurIndex = 0;
    szBuffer = new char[m_nBufferSize];
    strcpy(szBuffer,"");
    //StrCpy(szBuffer,"");
    bool bDo = true;
	while(bDo)
	{
	    int iReturn = GetNextLine(szBuffer,m_nBufferSize);
	    if(iReturn > 0)
		{
			int iLen = strlen(szBuffer);
			if(iLen > 0)
			{
				char* szData = new char[iLen + 10];
				strcpy(szData,szBuffer);
				//StrCpy(szData,szBuffer);
				m_pDataBuffer->Add(szData);
			}
		}
		else
		{
			if(strcmp(szBuffer,"") != 0)
			{
				int iLen = strlen(szBuffer);
			    if(iLen > 0)
				{
					char* szData = new char[iLen + 10];
					strcpy(szData,szBuffer);
					//StrCpy(szData,szBuffer);
					m_pDataBuffer->Add(szData);
				}
			}
			bDo = false;
		}
	}
	delete szBuffer;
#endif
}

int SFile::Read(void* pBuffer,int iBufferSize,int iOffset)
{
	if(!m_pFile)
		return 0;

	if(iOffset != -1)
		fseek(m_pFile,iOffset,SEEK_SET);
	else
		fseek(m_pFile,0,SEEK_CUR);
	
	if(m_bMemBufferMode)
	    return GetDataFromMemBuffer((void*) pBuffer,m_iCurMemIndex,iBufferSize);
	else
	    return fread((void*) pBuffer,1,iBufferSize,m_pFile);
}

void SFile::Write(char* szData,bool bNewLine)
{
	if(m_pFile)
	{
       fwrite(szData,1,strlen(szData),m_pFile);
	   if(bNewLine)
	      //fwrite("\r\n",1,strlen("\r\n"),m_pFile);
	      fwrite("\n",1,strlen("\n"),m_pFile);
	   fflush(m_pFile);
	}
}

void SFile::Write(bool bNewLine)
{
	if(m_pFile)
	{
	    int iTotal = m_pDataBuffer->GetTotal();
	    ResetCurIndex();
	    for(int i = 0;i < iTotal;i++)
	    {
	        char* pData = GetNext();
	        Write(pData,bNewLine);
	    }
	}
}

void SFile::Close()
{
	if(m_pFile)
	   fclose(m_pFile);
}

bool SFile::IsEmpty(char* szFileName)
{
	FILE* pFile;
	if((pFile = fopen(szFileName, "r")) == 0)
	   return true;
	fclose(pFile);
    return false;
}

unsigned long SFile::GetNextLine(char* szBuffer,unsigned long ulLine)
{
	unsigned char	*chTemp;
	bool	bStop = false;
	unsigned long	nOut;

	if(!m_bMemBufferMode && !m_pFile)
		return 0;

	chTemp = (unsigned char*) szBuffer;
	*chTemp = 0;
	nOut = 0;
	while(!bStop)
	{
		if(!m_ulLine || m_ulIndex == m_ulRead)
		{
            if(m_bMemBufferMode)
			    m_ulRead = GetDataFromMemBuffer((void*) m_pBuffer,m_iCurMemIndex,m_nBufferSize);
			else
			{
				m_ulMasterIndex = fseek(m_pFile,0,SEEK_CUR);
			    m_ulRead = fread((void*) m_pBuffer,1,m_nBufferSize,m_pFile);
			}

			m_ulIndex = 0;
			if(m_ulRead == 0)
			{
				bStop = true; //Error during readfile or END-OF-FILE encountered
				if(nOut > 0)
				{
					chTemp[nOut++] = (char) 0;
					return m_ulLine;	
				}
				else 
					return m_ulLine = 0; //nix gelezen
			}
			else
			{
				if(m_ulRead != (unsigned long) m_nBufferSize)
					bStop = true;	//END-OF-FILE
			}
		}
		for(;m_ulIndex < m_ulRead;m_ulIndex++)
		{
			if((nOut + 1) == ulLine)
			{
				char szError[80];
				sprintf(szError,"m_pBuffer overflow in line %u (line length over %d chars)",++m_ulLine,ulLine);
				chTemp[nOut] = '\0';
				return m_ulLine;
			}
			switch(m_pBuffer[m_ulIndex])
			{
			case 0x0d://End of Line encountered
			case 0x0a:
				if((m_ulIndex + 1) < m_ulRead) // Check we're not on end of m_pBuffer ???
					if(m_pBuffer[m_ulIndex + 1] == '\n' || m_pBuffer[m_ulIndex + 1] == '\r')
					{
						if(!*chTemp)
							m_ulLine++;
						m_ulIndex++;
					}
				if(*chTemp)
				{
					chTemp[nOut++] = '\0';
					m_ulLine++;
					return m_ulLine;
				}
				break;
			default: chTemp[nOut++] = m_pBuffer[m_ulIndex];
			}
		}
	}
	if(nOut>0)
	{
		chTemp[nOut++] = '\0';
		return m_ulLine;	
	}
	return m_ulLine = 0; //nix gelezen
}

void SFile::ResetCurIndex()
{
	m_iCurIndex = 0;
}

void SFile::SetCurIndex(int iIndex)
{
	m_iCurIndex = iIndex;
}

int SFile::GetCurIndex()
{
     return m_iCurIndex;
}

unsigned long SFile::GetTotal()
{
   return m_pDataBuffer->GetTotal();
}

char* SFile::GetNext()
{
	int iTotal = m_pDataBuffer->GetTotal();

	if(m_iCurIndex < iTotal)
	    return (char*)m_pDataBuffer->Get(m_iCurIndex++);
	return "";
}

void SFile::AddDataToBuffer(char* szData)
{
    char* szNewData = (char*)new char[strlen(szData) + 256];
	strcpy(szNewData,szData);
	m_pDataBuffer->Add(szNewData);
}

void SFile::InsertDataToBuffer(unsigned long ulIndex,char* szData)
{
	char* szNewData = (char*)new char[strlen(szData) + 256];
	strcpy(szNewData,szData);
	m_pDataBuffer->Insert(ulIndex,szNewData);
}

void SFile::RemoveFromBuffer(unsigned long ulIndex)
{
	m_pDataBuffer->Delete(ulIndex);
}

void SFile::CleanBuffer()
{
	m_pDataBuffer->Reset();
}

//Memery buffer functions
void SFile::AddToMemBuffer(char* szData)
{
	char* szNewData = new char[strlen(szData) + 256];
#if 0
	StrCpy(szNewData,szData);
#else
	strcpy(szNewData,szData);
#endif
	m_pMemBuffer->Add(szNewData);
}

int SFile::GetDataFromMemBuffer(void* pBuffer,int iIndex,int iBufferSize)
{
#if 0
	int iCurIndex = 0;
	if(iBufferSize >= m_iUnitSize)
	{
		int iCount = 0;
		int iTotal = m_pMemBuffer->GetTotal();
		int iMultiple = iBufferSize / m_iUnitSize;
	    iCurIndex = iIndex * iMultiple;
		for(int i = 0;i < iMultiple;i++)
		{
			if(iTotal > iCurIndex + i)
			{
			    char* pData = (char*) m_pMemBuffer->Get(iCurIndex + i);
				int iCurLen = strlen(pData);
			    memcpy((char*)pBuffer + iCount,pData,iCurLen);
				iCount += iCurLen;
			}
		}
		m_iOutLen = iCount;
		m_iCurMemIndex++;
		return m_iOutLen;
	}
	else
	{
		//
	}
	return 0;
#else
	int iStart = m_ulReadLen / m_iUnitSize;
	int iEnd = (m_ulReadLen + iBufferSize) / m_iUnitSize;
    int iCount = 0;
	unsigned long ulCurOffset;
    for(int i = iStart;i < iEnd;i++)
	{
		ulCurOffset = m_ulReadLen % m_iUnitSize;
		char* pData = (char*) m_pMemBuffer->Get(i);

		if(!pData)
			break;
			//return 0;

		int iCurLen = strlen(pData);
		if(iBufferSize > iCurLen + iCount)
		{
		   memcpy((char*)pBuffer + iCount,pData + ulCurOffset,iCurLen);
		   iCount += (iCurLen - ulCurOffset);
		}
		else
		{
            iCurLen = iBufferSize - iCount;
			memcpy((char*)pBuffer + iCount,pData + ulCurOffset,iCurLen);
			iCount += iCurLen;
		}
	}

	m_ulReadLen += iBufferSize;
	m_iOutLen = iCount;
	m_iCurMemIndex++;
	return m_iOutLen;
#endif
}

void SFile::SetMemBufferUnitSize(int iSize)
{
	m_iUnitSize = iSize;
}

void SFile::EnableMemBufferMode(bool bEnable)
{
	m_bMemBufferMode = bEnable;
}