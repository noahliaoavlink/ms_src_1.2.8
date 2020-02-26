#include "stdafx.h"
#include "FileInfoCtrl.h"
#include "GDICommon.h"

FileInfoCtrl::FileInfoCtrl()
{
	m_pNWin = 0;
	m_pIOSourceCtrlDll = 0;
	
	m_hFont = 0;
	m_hDC = 0;
	m_hBrush = 0;

	m_ulID = 0; 
	m_hParentWnd = NULL;

	//m_bSimpleMode = false;

	m_iMode = FIDM_ICON;

#ifdef _BMP32
	m_pRGB32Buf = new unsigned char[_THUMBNAIL_FRAME_W * _THUMBNAIL_FRAME_H * 4];
#else
	m_pDiBDraw = 0;
#endif
}

FileInfoCtrl::~FileInfoCtrl()
{
	
#ifdef _BMP32
	if (m_pRGB32Buf)
		delete m_pRGB32Buf;
#else
	if (m_pDiBDraw)
		delete m_pDiBDraw;
#endif

	if (m_pNWin)
		delete m_pNWin;

	if (m_hFont)
		DeleteObject(m_hFont);

	if (m_hBrush)
		DeleteObject(m_hBrush);

	if(m_hDC)
		ReleaseDC(m_pNWin->GetSafeHwnd(), m_hDC);
}

void FileInfoCtrl::Init()
{
	RECT rect = {10,10,200,100};
	m_pNWin = new CNWin("FileInfoCtrl", WT_NORMAL);

	m_hDC = ::GetDC(m_pNWin->GetSafeHwnd());
	m_hBrush = CreateSolidBrush(RGB(50, 50, 50));

#ifdef _BMP32
#else
	m_pDiBDraw = new DiBDraw;
	m_pDiBDraw->Init(m_pNWin->GetSafeHwnd(), rect);
#endif

	m_hFont = CreateFontA(14, 0, 0, 0, 400, 0, 0, 0, DEFAULT_CHARSET, 0, 0, DRAFT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Microsoft Sans Serif");

	m_pNWin->SetBKColor(50, 50, 50);
	m_pNWin->SetCallback(this);
}

void FileInfoCtrl::SetIOSourceCtrl(IOSourceCtrlDll* pObj)
{
	m_pIOSourceCtrlDll = pObj;
}

void FileInfoCtrl::MoveWindow(int x, int y, int w, int h)
{
	if(m_pNWin)
		m_pNWin->MoveWindow(x, y, w, h);
}

void FileInfoCtrl::ShowWindow(bool bShow)
{
	if (m_pNWin)
	{
		if (bShow)
		{
			m_pNWin->ShowWindow(SW_SHOW);
			ReDraw();
		}
		else
			m_pNWin->ShowWindow(SW_HIDE);
	}
}

void FileInfoCtrl::SetParent(HWND hWnd)
{
	if (m_pNWin)
	{
		m_hParentWnd = hWnd;
		::SetParent(m_pNWin->GetSafeHwnd(), hWnd);
	}
}
/*
void FileInfoCtrl::SetSimpleMode(bool bEnable)
{
	m_bSimpleMode = bEnable;
}
*/
void FileInfoCtrl::SetDisplayMode(int iMode)
{
	m_iMode = iMode;

	if (m_ulID != 0)
		UpdateInfo(m_ulID);
}

void FileInfoCtrl::UpdateInfo(unsigned long ulID)
{
	m_ulID = ulID;

	MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(ulID); 

	if (m_iMode == FIDM_ICON)
	{
		BrushRect(m_pNWin->GetSafeHwnd(), m_hDC, m_hBrush);

		if (pCurItem)
		{
			ThumbnailPicture* pPic = m_pIOSourceCtrlDll->GetPicture(pCurItem);

#ifdef _BMP32
			m_yuv_converter.I420ToARGB(pPic->pBuffer, m_pRGB32Buf, _THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H);
			
			RECT rect;
			HDC hdc;
			hdc = ::GetDC(m_hParentWnd);
			::GetWindowRect(m_pNWin->GetSafeHwnd(), &rect);

			// Ready for bmp header
			BITMAPINFO* pBitmapInfo;
			pBitmapInfo = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFOHEADER)];
			BITMAPINFOHEADER BitmapInfoHeader;
			BitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
			BitmapInfoHeader.biWidth = _THUMBNAIL_FRAME_W;
			BitmapInfoHeader.biHeight = _THUMBNAIL_FRAME_H;
			BitmapInfoHeader.biPlanes = 1;
			BitmapInfoHeader.biBitCount = 32;
			BitmapInfoHeader.biCompression = BI_RGB;
			BitmapInfoHeader.biSizeImage = (_THUMBNAIL_FRAME_W * 24 + 7) / 8 * _THUMBNAIL_FRAME_H;
			BitmapInfoHeader.biClrUsed = 0;
			BitmapInfoHeader.biClrImportant = 0;
			pBitmapInfo->bmiHeader = BitmapInfoHeader;

			// 實際影像輸出
			::StretchDIBits(hdc, rect.left, rect.top + (_THUMBNAIL_FRAME_H * 2), _THUMBNAIL_FRAME_W * 2, -(_THUMBNAIL_FRAME_H * 2), 0, 0, _THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H, m_pRGB32Buf, pBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		
			::ReleaseDC(m_hParentWnd, hdc);
#else
			if (pPic)
				m_pDiBDraw->Draw(pPic->pBuffer, _THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H);
#endif
		}
	}
	else
	{
		BrushRect(m_pNWin->GetSafeHwnd(), m_hDC, m_hBrush);

		if (pCurItem)
		{
			wchar_t wszText[256];
			swprintf(wszText, L"FileName:%s", m_pIOSourceCtrlDll->GetFileName(pCurItem->wszFileName));
			DrawText(100, 0, wszText);

			swprintf(wszText, L"Format:%s", pCurItem->wszFileFormat);
			DrawText(100, 20, wszText);

			swprintf(wszText, L"Video:%s", pCurItem->wszVideoCodecName);
			DrawText(100, 40, wszText);

			swprintf(wszText, L"Audio:%s", pCurItem->wszAudioCodecName);
			DrawText(100, 60, wszText);

			swprintf(wszText, L"Len:%s", pCurItem->wszLen);
			DrawText(100, 80, wszText);

			swprintf(wszText, L"Tags:%s", pCurItem->wszTag);
			DrawText(100, 100, wszText);
		}
	}
}

void FileInfoCtrl::DrawText(int x,int y,wchar_t* wszText)
{
	HFONT hTempFont = (HFONT)SelectObject(m_hDC, m_hFont);
	SetBkMode(m_hDC, TRANSPARENT);
	SetTextColor(m_hDC, RGB(200, 200, 0));

	TextOutW(m_hDC, x, y ,wszText, wcslen(wszText));

	SelectObject(m_hDC, hTempFont);
}

void FileInfoCtrl::ReDraw()
{
	m_pNWin->Invalidate();
//	m_pNWin->UpdateWindow();
}

void* FileInfoCtrl::WinMsg(int iMsg, void* pParameter1, void* pParameter2)
{
	switch (iMsg)
	{
		case WM_KEYDOWN:
		{
			if ((int)pParameter1 == 27)
			{
				//if(m_bSimpleMode)
					//::ShowWindow(m_hParentWnd, SW_HIDE);
				//else
					::PostMessage(m_hParentWnd, WM_CLOSE, 0, 0);
			}
		}
		break;
	}

	return 0;
}
