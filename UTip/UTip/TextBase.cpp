#include "stdafx.h"
#include "TextBase.h"

TextBase::TextBase()
{
	for(int i = 0;i < _MAX_FONT;i++)
	    m_hFont[i] = 0;
	m_iCurSel = 0;

	m_ucBKColor[0] = 255;
	m_ucBKColor[1] = 255;
	m_ucBKColor[2] = 255;
	m_ucTextColor[0] = 100;
	m_ucTextColor[1] = 100;
	m_ucTextColor[2] = 100;
	m_bIsTransparent = false;
	m_ucColors = 0;
	m_iTotalOfColors = 0;
	m_bEnableOutline = false;
	m_ucOutlineColor[0] = 255;
	m_ucOutlineColor[1] = 255;
	m_ucOutlineColor[2] = 255;
}

TextBase::~TextBase()
{
	for(int i = 0;i < _MAX_FONT;i++)
	{
		if(m_hFont[i])
		{
			DeleteObject(m_hFont[i]);
			m_hFont[i] = 0;
		}
	}

	if(m_ucColors)
		delete m_ucColors;
}

void TextBase::SetFont(char* szName,int iSize,int iIndex)
{
	if(m_hFont[iIndex])
	{
		DeleteObject(m_hFont[iIndex]);
		m_hFont[iIndex] = 0;
	}

	m_hFont[iIndex] = CreateFontA(iSize,0,0,0,400,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
         DEFAULT_QUALITY,DEFAULT_PITCH|FF_MODERN,szName);
}

HFONT TextBase::GetFont(int iIndex)
{
	if(iIndex == -1)
		return m_hFont[m_iCurSel];
	return m_hFont[iIndex];
}

void TextBase::SetTextColor(unsigned char ucR,unsigned char ucG,unsigned char ucB)
{
	m_ucTextColor[0] = ucR;
	m_ucTextColor[1] = ucG;
	m_ucTextColor[2] = ucB;
}

void TextBase::SetBKColor(unsigned char ucR,unsigned char ucG,unsigned char ucB)
{
	m_ucBKColor[0] = ucR;
	m_ucBKColor[1] = ucG;
	m_ucBKColor[2] = ucB;
}

void TextBase::DrawText(HDC hDC,int x,int y,char* szData,bool bBKIsUntouched)
{
	BeginDraw(hDC,bBKIsUntouched,m_ucTextColor,m_ucBKColor);

	if(!szData)
		return ;

	if(m_bEnableOutline)
	{
		::SetTextColor(hDC,RGB(m_ucOutlineColor[0],m_ucOutlineColor[1],m_ucOutlineColor[2]));
		TextOutA(hDC,x - 1,y,szData,strlen(szData));
		TextOutA(hDC,x + 1,y,szData,strlen(szData));
		TextOutA(hDC,x,y - 1,szData,strlen(szData));
		TextOutA(hDC,x,y + 1,szData,strlen(szData));

		TextOutA(hDC,x - 1,y - 1,szData,strlen(szData));
		TextOutA(hDC,x + 1,y + 1,szData,strlen(szData));
		TextOutA(hDC,x + 1,y - 1,szData,strlen(szData));
		TextOutA(hDC,x - 1,y + 1,szData,strlen(szData));

		::SetTextColor(hDC,RGB(m_ucTextColor[0],m_ucTextColor[1],m_ucTextColor[2]));
	}

	if(szData)
		TextOutA(hDC,x,y,szData,strlen(szData));
	EndDraw(hDC);
}

void TextBase::DrawTextW(HDC hDC, int x, int y, wchar_t* wszData, bool bBKIsUntouched)
{
	BeginDraw(hDC, bBKIsUntouched, m_ucTextColor, m_ucBKColor);

	if (!wszData)
		return;

	if (m_bEnableOutline)
	{
		::SetTextColor(hDC, RGB(m_ucOutlineColor[0], m_ucOutlineColor[1], m_ucOutlineColor[2]));
		TextOut(hDC, x - 1, y, wszData, wcslen(wszData));
		TextOut(hDC, x + 1, y, wszData, wcslen(wszData));
		TextOut(hDC, x, y - 1, wszData, wcslen(wszData));
		TextOut(hDC, x, y + 1, wszData, wcslen(wszData));

		TextOut(hDC, x - 1, y - 1, wszData, wcslen(wszData));
		TextOut(hDC, x + 1, y + 1, wszData, wcslen(wszData));
		TextOut(hDC, x + 1, y - 1, wszData, wcslen(wszData));
		TextOut(hDC, x - 1, y + 1, wszData, wcslen(wszData));

		::SetTextColor(hDC, RGB(m_ucTextColor[0], m_ucTextColor[1], m_ucTextColor[2]));
	}

	if (wszData)
		TextOut(hDC, x, y, wszData, wcslen(wszData));
	EndDraw(hDC);
}

void TextBase::GetTextSize(HDC hDC,char* szData,SIZE* pSize)
{
    HFONT hTempFont = (HFONT)SelectObject(hDC,m_hFont[m_iCurSel]);
    GetTextExtentPoint32A(hDC,szData,strlen(szData),pSize);	
    SelectObject(hDC,hTempFont);
//    DeleteObject(hTempFont);
}

void TextBase::GetTextSizeW(HDC hDC, wchar_t* wszData, SIZE* pSize)
{
	HFONT hTempFont = (HFONT)SelectObject(hDC, m_hFont[m_iCurSel]);
	GetTextExtentPoint32(hDC, wszData, wcslen(wszData), pSize);
	SelectObject(hDC, hTempFont);
}

void TextBase::EnableTransparent(bool bEnable)
{
	m_bIsTransparent = bEnable;
}

void TextBase::DrawText(HDC hDC,int x,int y,_Region* pRect,char* szData,unsigned char* ucColor)
{
	BeginDraw(hDC,m_bIsTransparent,ucColor,m_ucBKColor);
	RECT rect;
    rect.left = pRect->left;
    rect.top = pRect->top;
    rect.right = pRect->right + pRect->left;
    rect.bottom = pRect->bottom + pRect->top;

	ExtTextOutA(hDC,x,y,ETO_CLIPPED,&rect,szData,strlen(szData),0);
	EndDraw(hDC);
}

void TextBase::DrawPasswordText(HDC hDC,int x,int y,_Region* pRect,char* szData,unsigned char* ucColor)
{
	char szPassword[512];
	int iLen = strlen(szData);
	for(int i = 0;i < iLen;i++)
		szPassword[i] = '*';
	szPassword[iLen] = '\0';

	DrawText(hDC,x,y,pRect,szPassword,ucColor);
}

void TextBase::BeginDraw(HDC hDC,bool bBKIsUntouched,unsigned char* ucTextColor,unsigned char* ucBKColor)
{
#ifdef _ENABLE_GDI_PLUS
#else
	if(bBKIsUntouched)
        SetBkMode(hDC,TRANSPARENT);
	else
	{
		SetBkMode(hDC,OPAQUE);
		::SetBkColor(hDC,RGB(ucBKColor[0],ucBKColor[1],ucBKColor[2]));
	}
	::SetTextColor(hDC,RGB(ucTextColor[0],ucTextColor[1],ucTextColor[2]));
	m_hTempFont = (HFONT)SelectObject(hDC,m_hFont[m_iCurSel]);
#endif
}

void TextBase::EndDraw(HDC hDC)
{
	SelectObject(hDC,m_hTempFont);
  //  DeleteObject(m_hTempFont);
}

void TextBase::DrawTextEx(HDC hDC,int x,int y,char* szData)
{
	_Region* pRect = new _Region;
	int iCurIndex = 0;
	int iNumber = 0;
	unsigned char ucColor[3];
	SIZE size;
	GetTextSize(hDC,szData,&size);

	float fSetpValue = (float)m_iTotalOfColors / (float)size.cy;
	float fCurValue = 0.0;
	for(int i = 0;i < size.cy;i++)
	{
		fCurValue += fSetpValue;
		if(fCurValue >= 1.0 || i == 0)
		{
		    iNumber = iCurIndex * 3;
		    ucColor[0] = m_ucColors[iNumber];
		    ucColor[1] = m_ucColors[iNumber + 1];
		    ucColor[2] = m_ucColors[iNumber + 2];
			iCurIndex++;

			if(fCurValue >= 1.0)
			    fCurValue -= 1.0;
		}
		pRect->left = x;
		pRect->top = y + i;
		pRect->right = size.cx;
		pRect->bottom = 1;
		DrawText(hDC,x,y,pRect,szData,ucColor);
	}
	delete pRect;
}

void TextBase::SetColors(unsigned char* ucColors,int iTotal)
{
	int iLen = iTotal * 3;
	if(m_ucColors)
		delete m_ucColors;

	m_ucColors = new unsigned char[iLen];
	m_iTotalOfColors = iTotal;

	memcpy(m_ucColors,ucColors,iLen);
}

void TextBase::SelFont(int iIndex)
{
	m_iCurSel = iIndex;
}

void TextBase::EnableOutline(bool bEnable)
{
	m_bEnableOutline = bEnable;
}

void TextBase::SetOutlineColor(unsigned char ucR,unsigned char ucG,unsigned char ucB)
{
    m_ucOutlineColor[0] = ucR;
	m_ucOutlineColor[1] = ucG;
	m_ucOutlineColor[2] = ucB;
}
