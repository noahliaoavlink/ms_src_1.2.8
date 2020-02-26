#ifndef TextBase_H
#define TextBase_H

#include "..\\..\\Include\\UIObjInfo.h"
//#include "ControlBase.h"

#define _MAX_FONT 3

class TextBase
{
	   HFONT m_hFont[_MAX_FONT];
	   int m_iCurSel;
	   int m_iFontSize;
	   unsigned char m_ucBKColor[3];
	   unsigned char m_ucTextColor[3];
	   bool m_bIsTransparent;
	   HFONT m_hTempFont;
	   unsigned char* m_ucColors;
	   int m_iTotalOfColors;
	   bool m_bEnableOutline;
       unsigned char m_ucOutlineColor[3];
   public:  
	   TextBase();
	   ~TextBase();

	   virtual void SetFont(char* szName,int iSize,int iIndex = 0);
	   virtual HFONT GetFont(int iIndex = -1);
	   virtual void SetTextColor(unsigned char ucR,unsigned char ucG,unsigned char ucB);
	   virtual void SetBKColor(unsigned char ucR,unsigned char ucG,unsigned char ucB);
	   virtual void DrawText(HDC hDC,int x,int y,char* szData,bool bBKIsUntouched = true);
	   virtual void DrawTextW(HDC hDC, int x, int y, wchar_t* wszData, bool bBKIsUntouched = true);
	   virtual void GetTextSize(HDC hDC,char* szData,SIZE* pSize);
	   virtual void GetTextSizeW(HDC hDC, wchar_t* wszData, SIZE* pSize);

	   virtual void EnableTransparent(bool bEnable);
	   virtual void DrawText(HDC hDC,int x,int y,_Region* pRect,char* szData,unsigned char* ucColor);
	   virtual void DrawPasswordText(HDC hDC,int x,int y,_Region* pRect,char* szData,unsigned char* ucColor);
	   virtual void DrawTextEx(HDC hDC,int x,int y,char* szData);

	   void BeginDraw(HDC hDC,bool bBKIsUntouched,unsigned char* ucTextColor,unsigned char* ucBKColor);
	   void EndDraw(HDC hDC);

	   virtual void SetColors(unsigned char* ucColors,int iTotal);
	   virtual void SelFont(int iIndex);
	   virtual void EnableOutline(bool bEnable);
	   virtual void SetOutlineColor(unsigned char ucR,unsigned char ucG,unsigned char ucB);
};
#endif