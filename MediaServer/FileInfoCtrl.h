#pragma once

#ifndef _FileInfoCtrl_H_
#define _FileInfoCtrl_H_

//#include "MyListCtrl.h"
#include "IOSourceCtrlDll.h"
#include "NWin.h"

#include "../../../Include/mediafilecommon.h"
//#include "mediafilecommon.h"

#define _BMP32 1

#ifdef _BMP32
#else
#include "DiBDraw.h"
#endif

class FileInfoCtrl : public WinMsgCallback
{
	IOSourceCtrlDll* m_pIOSourceCtrlDll;
	CNWin* m_pNWin;
#ifdef _BMP32
	YUVConverter m_yuv_converter;
	unsigned char* m_pRGB32Buf;
#else
	DiBDraw* m_pDiBDraw;
#endif
	int m_iMode;

	unsigned long m_ulID;

	HWND m_hParentWnd;

	HFONT m_hFont;
	HDC m_hDC;
	HBRUSH m_hBrush;

	//bool m_bSimpleMode;
protected:
public:
	FileInfoCtrl();
	~FileInfoCtrl();

	void Init();
	void SetIOSourceCtrl(IOSourceCtrlDll* pObj);
	void MoveWindow(int x, int y, int w, int h);
	void ShowWindow(bool bShow);
	void SetParent(HWND hWnd);
	void SetDisplayMode(int iMode);
	void DrawText(int x, int y, wchar_t* wszText);

	void UpdateInfo(unsigned long ulID);
	void ReDraw();

	//void SetSimpleMode(bool bEnable);

	virtual void* WinMsg(int iMsg, void* pParameter1, void* pParameter2);
};

#endif