#ifndef _FileTrimmer_H
#define _FileTrimmer_H

#include "..\\..\\Include\\SQList.h"

class FileTrimmer
{
        SQList* m_pFileList;
		SQList* m_pRemoveList;
		int m_iReserve;

		char m_szPath[512];
   public:
	    FileTrimmer();
        ~FileTrimmer();

		void SetPath(char* szPath);
		void SearchAllFiles(char* szFolder,SQList* pList);
		void Sort();
		int Compare(char* pAItem,char* pBItem);
		long FileToNum(char* szFileName);
		void MakeRemoveList();
		void RemoveFiles();

		void DoRemoveFiles();
};

#endif