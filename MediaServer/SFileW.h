#ifndef _SFileW_H
#define _SFileW_H
#include <stdio.h>
#include <iostream>
#include <string.h>
//#include "..\\..\\..\\Include\\SQList.h"
#include "SQList.h"

//#define BUFFER_SIZE 2048

class SFileW //: public SIFile
{
	
	   int m_iRef;

	   FILE* m_pFile;
	   wchar_t* m_pBuffer;
	   unsigned char* m_pUtf8Buffer;
	   int m_nBufferSize;
	   unsigned long	m_ulRead;
	   unsigned long	m_ulLine;
	   unsigned long	m_ulMasterIndex;
	   unsigned long	m_ulIndex;
	   int m_iCurIndex;

	   int m_iOutLen;
	   SQList* m_pDataBuffer;
	   SQList* m_pMemBuffer;
	   int m_iUnitSize;
	   bool m_bMemBufferMode;
	   int m_iCurMemIndex;
	   unsigned long m_ulReadLen;
	
   public:  
	   /*
	   virtual int QueryInterface(int iID,void** ppv);
	   virtual int IncRef();
       virtual int DecRef();  
	   */
	   SFileW();
	   ~SFileW();

	   virtual bool Open(const wchar_t* wszFileName,bool bTextMode = true,bool bWrite = false);
	   virtual bool Open1(const wchar_t* wszFileName,wchar_t* wszMode);
	   virtual void ReadToBuffer();
	   virtual int Read(void* pBuffer,int iBufferSize,int iOffset);
	   virtual void Write(wchar_t* wszData,bool bNewLine = false);
	   virtual void Write(bool bNewLine);
	   virtual void Close();
	   virtual bool IsEmpty(wchar_t* wszFileName);
	   virtual unsigned long GetNextLine(wchar_t* wszBuffer,unsigned long ulLine);
	   virtual unsigned long GetNextLine_Utf8(wchar_t* wszBuffer,unsigned long ulLine);
	   virtual void ResetCurIndex();
	   virtual void SetCurIndex(int iIndex);
	   virtual int GetCurIndex();
	   virtual unsigned long GetTotal();
	   virtual wchar_t* GetNext();

	   virtual void AddDataToBuffer(wchar_t* wszData);
       virtual void InsertDataToBuffer(unsigned long ulIndex,wchar_t* wszData);
	   virtual void RemoveFromBuffer(unsigned long ulIndex);

	   virtual void CleanBuffer();

	   //Memery buffer functions
	   virtual void AddToMemBuffer(wchar_t* wszData);
	   virtual int GetDataFromMemBuffer(void* pBuffer,int iIndex,int iBufferSize);

	   virtual void SetMemBufferUnitSize(int iSize);
	   virtual void EnableMemBufferMode(bool bEnable);
	   
};

#endif