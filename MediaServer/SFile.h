#ifndef SFile_H
#define SFile_H
#include <stdio.h>
#include <string.h>
//#include "..\\Interface\\SIFile.h" 
#include "SQList.h"

//#define BUFFER_SIZE 2048

class SFile //: public SIFile
{
	   int m_iRef;

	   FILE* m_pFile;
	   unsigned char* m_pBuffer;
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
	   virtual bool Open(const char* szFileName,bool bTextMode = true,bool bWrite = false);
	   virtual bool Open(const char* szFileName,char* szMode);
	   virtual void ReadToBuffer();
	   virtual int Read(void* pBuffer,int iBufferSize,int iOffset);
	   virtual void Write(char* szData,bool bNewLine = false);
	   virtual void Write(bool bNewLine);
	   virtual void Close();
	   virtual bool IsEmpty(char* szFileName);
	   virtual unsigned long GetNextLine(char* szBuffer,unsigned long ulLine);
	   virtual void ResetCurIndex();
	   virtual void SetCurIndex(int iIndex);
	   virtual int GetCurIndex();
	   virtual unsigned long GetTotal();
	   virtual char* GetNext();
	   
	   virtual void AddDataToBuffer(char* szData);
       virtual void InsertDataToBuffer(unsigned long ulIndex,char* szData);
	   virtual void RemoveFromBuffer(unsigned long ulIndex);

	   virtual void CleanBuffer();
	   
	   //Memery buffer functions
	   virtual void AddToMemBuffer(char* szData);
	   virtual int GetDataFromMemBuffer(void* pBuffer,int iIndex,int iBufferSize);
	   virtual void SetMemBufferUnitSize(int iSize);
	   virtual void EnableMemBufferMode(bool bEnable);
	   
	   SFile();
	   ~SFile();
};

#endif