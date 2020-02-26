#ifndef _SIniFileW_H
#define _SIniFileW_H

#include "SFileW.h"

typedef struct
{
	wchar_t wszName[256];
	int iStart;
	int iEnd;
}SectionInfoW;

class SIniFileW : public SFileW
{	
		wchar_t* m_wszResult;
		wchar_t* m_wszCurrentSection;
		wchar_t* m_wszCurrentItem;
		wchar_t* m_wszCurrentData;
		wchar_t* m_wszFileName;

		int m_iCurSectionStartIndex;
		int m_iCurSectionEndIndex;

		SQList* m_pSectionInfo;
		int m_iRet;
   public:  
	   
		SIniFileW();
		~SIniFileW();

		bool Open(wchar_t* szFileName,bool bTextMode = true,bool bWrite = true);
		void Close();
		virtual void ReadToBuffer();
		int ParseData(wchar_t* wszInData,wchar_t* wszOutCommand,wchar_t* wszOutData);
		bool GetSection(wchar_t* wszSection);
		wchar_t* GetItemValue(wchar_t* wszSection,wchar_t* wszItem,wchar_t* wszDefault = L"");
		wchar_t* GetData(wchar_t* wszItem,wchar_t* wszDefault);
		bool GetSectionIndex(wchar_t* wszSection,int* iStart,int* iEnd);
		void SetCurSectionStartIndex(int iIndex);
		int GetCurSectionStartIndex();
		void SetCurSectionEndIndex(int iIndex);
		int GetCurSectionEndIndex();
		bool SetItemData(wchar_t* wszSection,wchar_t* wszItem,wchar_t* wszNewData);
		void WriteNewSection(wchar_t* wszSection);
		void WriteNewItem(wchar_t* wszItem,wchar_t* wszData,bool bInsert = false);
		void AddNewSectionInfo(wchar_t* wszSection,int iStart,int iEnd);
		void ShiftSectionInfo(wchar_t* wszSection);
		void Write(bool bTextMode = true);

		void ParseSectionInfo();
		bool IsSection(wchar_t* szData);
};

#endif