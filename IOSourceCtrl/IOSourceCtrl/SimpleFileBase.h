#ifndef _SimpleFileBase_H
#define _SimpleFileBase_H

#include <stdio.h>

typedef struct
{
	unsigned long ulVersion;
	unsigned long ulTotalOfItems;
	unsigned long ulReserve[10];
}FileHeader;

class SimpleFileBase
{
		FileHeader m_FileHeader;
		unsigned long m_ulFileLen;
   protected:
			FILE* m_pFile;
   public:
	    SimpleFileBase();
        ~SimpleFileBase();

		virtual int Open(char* szFileName,bool bIsWritable = false);
		virtual void Close();
		virtual void WriteHeader(int iTotal);
		virtual FileHeader* ReadHeader();

		unsigned long GetLength();
};

#endif