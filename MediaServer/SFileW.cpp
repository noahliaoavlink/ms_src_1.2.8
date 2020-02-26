#include "stdafx.h"
#include "SFileW.h"

#include "..\\..\\..\\Include\\StrConv.h"

#define _FILE_BUFFER_SIZE 20480//2048

SFileW::SFileW()
{
	m_iRef = 0;
	m_pFile = 0;

	m_ulRead = _FILE_BUFFER_SIZE;
	m_nBufferSize = _FILE_BUFFER_SIZE;
	m_pBuffer = new wchar_t[_FILE_BUFFER_SIZE];
	m_pUtf8Buffer = new unsigned char[_FILE_BUFFER_SIZE*2];
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

SFileW::~SFileW()
{
	delete m_pBuffer;
	delete m_pUtf8Buffer;
	delete m_pDataBuffer;
	delete m_pMemBuffer;
}

bool SFileW::Open(const wchar_t* wszFileName,bool bTextMode,bool bWrite)
{
	wchar_t wszMode[10];
	if(bWrite)
	{
		if(bTextMode)
		    wcscpy(wszMode,L"w+t");
		else
			wcscpy(wszMode,L"w+b");
	}
	else
	{
		if(bTextMode)
		    wcscpy(wszMode,L"rt");
		else
			wcscpy(wszMode,L"rb");
	}

	if((m_pFile  = _wfopen(wszFileName, wszMode)) == 0)
		return false;
	return true;
}

bool SFileW::Open1(const wchar_t* wszFileName,wchar_t* wszMode)
{
	if((m_pFile  = _wfopen(wszFileName, wszMode)) == 0)
		return false;
	return true;
}

void SFileW::ReadToBuffer()
{
	wchar_t* wszBuffer;
	m_iCurIndex = 0;
    wszBuffer = new wchar_t[m_nBufferSize];
    wcscpy(wszBuffer,L"");
    //StrCpy(szBuffer,"");
    bool bDo = true;
	while(bDo)
	{
	    //int iReturn = GetNextLine(wszBuffer,m_nBufferSize);
		int iReturn = GetNextLine_Utf8(wszBuffer,m_nBufferSize);
	    if(iReturn > 0)
		{
			int iLen = wcslen(wszBuffer);
			if(iLen > 0)
			{
				wchar_t* wszData = new wchar_t[iLen + 10];
				wcscpy(wszData,wszBuffer);
				//StrCpy(szData,szBuffer);
				m_pDataBuffer->Add(wszData);
			}
		}
		else
		{
			if(wcscmp(wszBuffer,L"") != 0)
			{
				int iLen = wcslen(wszBuffer);
			    if(iLen > 0)
				{
					wchar_t* wszData = new wchar_t[iLen + 10];
					wcscpy(wszData,wszBuffer);
					//StrCpy(szData,szBuffer);
					m_pDataBuffer->Add(wszData);
				}
			}
			bDo = false;
		}
	}
	delete wszBuffer;
}

int SFileW::Read(void* pBuffer,int iBufferSize,int iOffset)
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
	{
#if 0
		int iLen = fread((void*) pBuffer,1,iBufferSize,m_pFile);
#else
		int iLen = fread((void*) m_pUtf8Buffer,1,iBufferSize*sizeof(wchar_t),m_pFile);
		wchar_t* pUtf8Data = UTF8ToUnicode((const char*)m_pUtf8Buffer);
		wcscpy((wchar_t*)pBuffer,pUtf8Data);
#endif
	    return iLen;
	}
}

void SFileW::Write(wchar_t* wszData,bool bNewLine)
{
	if(m_pFile)
	{
#if 0
       fwrite(wszData,1,wcslen(wszData)*sizeof(wchar_t),m_pFile);
	   if(bNewLine)
	      //fwrite("\r\n",1,strlen("\r\n"),m_pFile);
	      fwrite(L"\n",1,wcslen(L"\n")*sizeof(wchar_t),m_pFile);
#else
		std::string sData = UnicodeToUTF8(wszData);
		fwrite(sData.c_str(),1,sData.size(),m_pFile);
		if(bNewLine)
		{
			//std::string sNewLineData = UnicodeToUTF8(L"\n");
			fwrite("\n",1,strlen("\n"),m_pFile);
		}
#endif
	   fflush(m_pFile);
	}
}

void SFileW::Write(bool bNewLine)
{
	if(m_pFile)
	{
	    int iTotal = m_pDataBuffer->GetTotal();
	    ResetCurIndex();
	    for(int i = 0;i < iTotal;i++)
	    {
	        wchar_t* pData = GetNext();
	        Write(pData,bNewLine);
	    }
	}
}

void SFileW::Close()
{
	if(m_pFile)
	   fclose(m_pFile);
}

bool SFileW::IsEmpty(wchar_t* wszFileName)
{
	FILE* pFile;
	if((pFile = _wfopen(wszFileName, L"r")) == 0)
	   return true;
	fclose(pFile);
    return false;
}

unsigned long SFileW::GetNextLine(wchar_t* wszBuffer,unsigned long ulLine)
{
	wchar_t	*chTemp;
	bool	bStop = false;
	unsigned long	nOut;

	if(!m_bMemBufferMode && !m_pFile)
		return 0;

	//chTemp = (unsigned char*) wszBuffer;
	chTemp = wszBuffer;
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
#if 1
			    m_ulRead = fread((void*) m_pBuffer,1,m_nBufferSize*sizeof(wchar_t),m_pFile);
#else
				m_ulRead = fread((void*) m_pUtf8Buffer,1,m_nBufferSize*sizeof(wchar_t),m_pFile);
				if(m_ulRead > 0)
					wchar_t* p = UTF8ToUnicode1((const char*)m_pUtf8Buffer,m_pBuffer,m_ulRead);
#endif
			}

			m_ulIndex = 0;
			if(m_ulRead == 0)
			{
				bStop = true; //Error during readfile or END-OF-FILE encountered
				if(nOut > 0)
				{
					chTemp[nOut++] = (wchar_t) 0;
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
			switch((wchar_t)m_pBuffer[m_ulIndex])
			{
			case 0x0d://End of Line encountered
			case 0x0a:
				if((m_ulIndex + 1) < m_ulRead) // Check we're not on end of m_pBuffer ???
					if((wchar_t)m_pBuffer[m_ulIndex + 1] == '\n' || (wchar_t)m_pBuffer[m_ulIndex + 1] == '\r')
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

unsigned long SFileW::GetNextLine_Utf8(wchar_t* wszBuffer,unsigned long ulLine)
{
	char chTemp[_FILE_BUFFER_SIZE];
	bool	bStop = false;
	unsigned long	nOut;

	if(!m_bMemBufferMode && !m_pFile)
		return 0;

	//chTemp = (unsigned char*) wszBuffer;
	//chTemp = wszBuffer;
	//*chTemp = 0;
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
#if 0
			    m_ulRead = fread((void*) m_pBuffer,1,m_nBufferSize*sizeof(wchar_t),m_pFile);
#else
				m_ulRead = fread((void*) m_pUtf8Buffer,1,m_nBufferSize*sizeof(wchar_t),m_pFile);
#endif
			}

			m_ulIndex = 0;
			if(m_ulRead == 0)
			{
				bStop = true; //Error during readfile or END-OF-FILE encountered
				if(nOut > 0)
				{
					chTemp[nOut++] = 0;
					wchar_t* pwszTemp = UTF8ToUnicode(chTemp);
					wcscpy(wszBuffer,pwszTemp);
					return m_ulLine;	
				}
				else 
				{
					wcscpy(wszBuffer,L"");
					return m_ulLine = 0; //nix gelezen
				}
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
				wchar_t* pwszTemp = UTF8ToUnicode(chTemp);
				wcscpy(wszBuffer,pwszTemp);
				return m_ulLine;
			}
			switch(m_pUtf8Buffer[m_ulIndex])
			{
			case 0x0d://End of Line encountered
			case 0x0a:
				if((m_ulIndex + 1) < m_ulRead) // Check we're not on end of m_pBuffer ???
				{
					if(m_pUtf8Buffer[m_ulIndex + 1] == '\n' || m_pUtf8Buffer[m_ulIndex + 1] == '\r')
					{
						if(!*chTemp)
							m_ulLine++;
						m_ulIndex++;
					}
				}
				if(nOut > 0/**chTemp*/)
				{
					chTemp[nOut++] = '\0';
					m_ulLine++;
					wchar_t* pwszTemp = UTF8ToUnicode(chTemp);
					wcscpy(wszBuffer,pwszTemp);
					return m_ulLine;
				}
				break;
			default: chTemp[nOut++] = m_pUtf8Buffer[m_ulIndex];
			}
		}
	}
	if(nOut>0)
	{
		chTemp[nOut++] = '\0';

		wchar_t* pwszTemp = UTF8ToUnicode(chTemp);
		wcscpy(wszBuffer,pwszTemp);

		return m_ulLine;	
	}
	return m_ulLine = 0; //nix gelezen
}

void SFileW::ResetCurIndex()
{
	m_iCurIndex = 0;
}

void SFileW::SetCurIndex(int iIndex)
{
	m_iCurIndex = iIndex;
}

int SFileW::GetCurIndex()
{
     return m_iCurIndex;
}

unsigned long SFileW::GetTotal()
{
   return m_pDataBuffer->GetTotal();
}

wchar_t* SFileW::GetNext()
{
	int iTotal = m_pDataBuffer->GetTotal();

	if(m_iCurIndex < iTotal)
	    return (wchar_t*)m_pDataBuffer->Get(m_iCurIndex++);
	return L"";
}

void SFileW::AddDataToBuffer(wchar_t* wszData)
{
    wchar_t* wszNewData = new wchar_t[wcslen(wszData) + 256];
	wcscpy(wszNewData,wszData);
	m_pDataBuffer->Add(wszNewData);
}

void SFileW::InsertDataToBuffer(unsigned long ulIndex,wchar_t* wszData)
{
	wchar_t* wszNewData = new wchar_t[wcslen(wszData) + 256];
	wcscpy(wszNewData,wszData);
	m_pDataBuffer->Insert(ulIndex,wszNewData);
}

void SFileW::RemoveFromBuffer(unsigned long ulIndex)
{
	m_pDataBuffer->Delete(ulIndex);
}

void SFileW::CleanBuffer()
{
	m_pDataBuffer->Reset();
}

void SFileW::AddToMemBuffer(wchar_t* wszData)
{
	wchar_t* wszNewData = new wchar_t[wcslen(wszData) + 256];
	wcscpy(wszNewData,wszData);
	m_pMemBuffer->Add(wszNewData);
}

int SFileW::GetDataFromMemBuffer(void* pBuffer,int iIndex,int iBufferSize)
{
	int iStart = m_ulReadLen / m_iUnitSize;
	int iEnd = (m_ulReadLen + iBufferSize) / m_iUnitSize;
    int iCount = 0;
	unsigned long ulCurOffset;
    for(int i = iStart;i < iEnd;i++)
	{
		ulCurOffset = m_ulReadLen % m_iUnitSize;
		wchar_t* pData = (wchar_t*) m_pMemBuffer->Get(i);

		if(!pData)
			break;
			//return 0;

		int iCurLen = wcslen(pData);
		if(iBufferSize > iCurLen + iCount)
		{
		   memcpy((wchar_t*)pBuffer + iCount,pData + ulCurOffset,iCurLen);
		   iCount += (iCurLen - ulCurOffset);
		}
		else
		{
            iCurLen = iBufferSize - iCount;
			memcpy((wchar_t*)pBuffer + iCount,pData + ulCurOffset,iCurLen);
			iCount += iCurLen;
		}
	}

	m_ulReadLen += iBufferSize;
	m_iOutLen = iCount;
	m_iCurMemIndex++;
	return m_iOutLen;
}

void SFileW::SetMemBufferUnitSize(int iSize)
{
	m_iUnitSize = iSize;
}

void SFileW::EnableMemBufferMode(bool bEnable)
{
	m_bMemBufferMode = bEnable;
}