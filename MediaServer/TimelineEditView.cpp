/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineEditView.cpp

Definition of following classes:
CTimelineEditView

Copyright:
Media Server
(C) Copyright C&C TECHNIC TAIWAN CO., LTD.
All rights reserved.

Author:
Stone Chang, mail: stonechang.cctch@gmail.com

Other issue:
N/A

\********************************************************************/

// ==== Inlcude Microsoft MFC library ==== //
#include "stdafx.h"

// ==== Inlcude STL library ==== //
#include <vector>
#include <memory>
#include <string>
#include <boost/format.hpp>

// ==== Include local library ==== //
#include "TLUtility.h"
#include "TimelineEditView.h"
#include "DoubleBuffer.h"
#include "ServiceProvider.h"

#include "Panel.h"

#include "../../../Include/StrConv.h"
#include "MediaServer.h"

#define ENGINNERING_MODE

using namespace Timeline;

/* ====================================================== *\
CTimelineEditView
\* ====================================================== */

IMPLEMENT_DYNCREATE(CTimelineEditView, CView)

BEGIN_MESSAGE_MAP(CTimelineEditView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_COPYDATA()
	ON_WM_TIMER()
	//ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_MESSAGE(WM_TM_BEGIN, OnTMBegin)
	ON_MESSAGE(WM_TM_PLAY, OnTMPlay)
	ON_MESSAGE(WM_TM_PAUSE, OnTMPause)
	ON_MESSAGE(WM_TM_RETURN_FILENAME, OnTMReturnFileName)
	ON_MESSAGE(WM_TM_UPDATE_HAND,OnUpdateHand)
	ON_MESSAGE(WM_TM_SET_TIMEBAR_OFFSET, OnSetTimeBarOffset)
	ON_MESSAGE(WM_TM_SET_VERTICAL_SCROLL_SHIFT, OnSetVerticalScrollShift)
	ON_MESSAGE(WM_TM_REDRAW, OnRedraw)
	ON_MESSAGE(WM_TM_UPDATE_TIME_CODE,OnUpdateTimeCode)
	ON_MESSAGE(WM_TM_RESET_TIME_CODE, OnResetTimeCode)
	ON_MESSAGE(WM_TM_BACK_TL_TIEM, OnBackTLItem)
	ON_MESSAGE(WM_TM_NEXT_TL_TIEM, OnNextTLItem)
	ON_MESSAGE(WM_TM_PLAY2, OnTMPlay2)
END_MESSAGE_MAP()

// CTimelineEditView construction/destruction

CTimelineEditView::CTimelineEditView()
{
	m_facade.OnInitialize(this);

	m_bUpdateTrackContent = false;
	m_bUpdateButtonPanel = false;
	m_bUpdateCurTimeCode = false;
	m_bUpdateTimeCodeBar = false;

	m_bDoRedraw = false;
	m_bDoPlay = false;
	m_pUTipDll = 0;
}

CTimelineEditView::~CTimelineEditView()
{
	m_facade.OnTerminate();
}

#ifdef TIMELINEDEV
CRuntimeClass* CTimelineEditView::GetThisClass2()
{
	return CTimelineEditView::GetThisClass();
}
#endif

BOOL CTimelineEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

void CTimelineEditView::OnDraw(CDC* pDC)
{
	CRect rect;
	this->GetClientRect(&rect);

#if 0
	DoubleBuffer doubleBuffer(pDC, this);
	CDC* offScreenDC = doubleBuffer.InitBackBuffer();
	Gdiplus::Graphics graphics(offScreenDC->m_hDC);

	m_facade.Draw(graphics, CRectToRect(rect));
#else
	//Clear(0, 0, rect.right, rect.bottom);
	bool bReDrawAll = true;
	
	if (m_bUpdateButtonPanel)
	{
		bReDrawAll = false;
		m_bUpdateButtonPanel = false;
		Clear(m_rButtonPanelRegion.left, m_rButtonPanelRegion.top, m_rButtonPanelRegion.right - m_rButtonPanelRegion.left, m_rButtonPanelRegion.bottom - m_rButtonPanelRegion.top);
		m_facade.DrawButtonPanel(*m_pGraphics, CRectToRect(rect));

		::BitBlt(pDC->m_hDC, m_rButtonPanelRegion.left, m_rButtonPanelRegion.top, m_rButtonPanelRegion.right - m_rButtonPanelRegion.left, m_rButtonPanelRegion.bottom - m_rButtonPanelRegion.top, m_hBackDC, m_rButtonPanelRegion.left, m_rButtonPanelRegion.top, SRCCOPY);
	}

	if (m_bUpdateCurTimeCode)
	{
		bReDrawAll = false;
		m_bUpdateCurTimeCode = false;

		Clear(m_rCurTimeCodeRegion.left, m_rCurTimeCodeRegion.top, m_rCurTimeCodeRegion.right - m_rCurTimeCodeRegion.left, m_rCurTimeCodeRegion.bottom - m_rCurTimeCodeRegion.top);
		m_facade.DrawCurTimecode(*m_pGraphics, CRectToRect(rect));

		::BitBlt(pDC->m_hDC, m_rCurTimeCodeRegion.left, m_rCurTimeCodeRegion.top, m_rCurTimeCodeRegion.right - m_rCurTimeCodeRegion.left, m_rCurTimeCodeRegion.bottom - m_rCurTimeCodeRegion.top, m_hBackDC, m_rCurTimeCodeRegion.left, m_rCurTimeCodeRegion.top, SRCCOPY);
	}

	if (m_bUpdateTimeCodeBar)
	{
		bReDrawAll = false;
		m_bUpdateTimeCodeBar = false;

		Clear(m_rTimeCodeBarRegion.left, m_rTimeCodeBarRegion.top, m_rTimeCodeBarRegion.right - m_rTimeCodeBarRegion.left, m_rTimeCodeBarRegion.bottom - m_rTimeCodeBarRegion.top);
		m_facade.DrawTimecodeBar(*m_pGraphics, CRectToRect(rect));

		::BitBlt(pDC->m_hDC, m_rTimeCodeBarRegion.left, m_rTimeCodeBarRegion.top, m_rTimeCodeBarRegion.right - m_rTimeCodeBarRegion.left, m_rTimeCodeBarRegion.bottom - m_rTimeCodeBarRegion.top, m_hBackDC, m_rTimeCodeBarRegion.left, m_rTimeCodeBarRegion.top, SRCCOPY);
	}

	if (m_bUpdateTrackContent)
	{
		bReDrawAll = false;
		m_bUpdateTrackContent = false;
		Clear(m_rUpdateRegion.left, m_rUpdateRegion.top, m_rUpdateRegion.right - m_rUpdateRegion.left, m_rUpdateRegion.bottom - m_rUpdateRegion.top);
		m_facade.DrawTrackContent(*m_pGraphics, CRectToRect(rect));

		::BitBlt(pDC->m_hDC, m_rUpdateRegion.left, m_rUpdateRegion.top, m_rUpdateRegion.right - m_rUpdateRegion.left, m_rUpdateRegion.bottom - m_rUpdateRegion.top, m_hBackDC, m_rUpdateRegion.left, m_rUpdateRegion.top, SRCCOPY);
	}

	if (m_bDoRedraw)
	{
		m_bDoRedraw = false;
		bReDrawAll = true;
	}

	if(bReDrawAll)
	{
		Clear(0, 0, rect.right, rect.bottom);
		m_facade.Draw(*m_pGraphics, CRectToRect(rect));
		::BitBlt(pDC->m_hDC, 0, 0, rect.right, rect.bottom, m_hBackDC, 0, 0, SRCCOPY);
	}
#endif
}

void CTimelineEditView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
//	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CTimelineEditView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
	m_facade.OnRButtonUp(Point(point.x, point.y));
#ifndef SHARED_HANDLERS
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CTimelineEditView diagnostics

#ifdef _DEBUG
void CTimelineEditView::AssertValid() const
{
	CView::AssertValid();
}

void CTimelineEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG


// CTimelineEditView message handlers

int CTimelineEditView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	// m_pDC must be initialized here instead of the constructor
	// because the HWND isn't created until Create is called.
	CRect rect;
	this->GetClientRect(&rect);
	m_facade.OnCreate(CRectToRect(rect));

	CreateBackDC(rect.right, rect.bottom);

	m_pUTipDll = new UTipDll;
	m_pUTipDll->LoadLib();
	m_pUTipDll->_UT_Init(m_pUTipDll->GetUTObj());
	/*
	AddTip(L"Back", 10, 8, 31, 31);
	AddTip(L"Next", 150 , 8, 31, 31);
	AddTip(L"10S", 45, 8, 29, 29);
	AddTip(L"1M", 80, 8, 29, 29);
	AddTip(L"10M", 115, 8, 29, 29);

	int x = rect.right - (30 * 9) - 65;
	AddTip(L"Normal", x, 8, 30, 30);
	*/

	SetTimer(101,1,NULL);

	return 0;
}

void CTimelineEditView::OnDestroy()
{
	m_facade.OnDestroy();
	ReleaseBackDC();

	if (m_pUTipDll)
		delete m_pUTipDll;

	CView::OnDestroy();
}

BOOL CTimelineEditView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return CView::OnEraseBkgnd(pDC);
}

void CTimelineEditView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_facade.OnLButtonDown(Point(point.x, point.y));
	CView::OnLButtonDown(nFlags, point);
}

void CTimelineEditView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_facade.OnLButtonUp(Point(point.x, point.y));
	CView::OnLButtonUp(nFlags, point);
}


void CTimelineEditView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_facade.OnKeyDown(nChar);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTimelineEditView::OnMouseMove(UINT nFlags, CPoint point)
{
	m_facade.OnMouseMove(Point(point.x, point.y));

	m_pUTipDll->_UT_ShowTip(m_pUTipDll->GetUTObj(), this->GetSafeHwnd(), point.x, point.y);

	CView::OnMouseMove(nFlags, point);
}

BOOL CTimelineEditView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	m_facade.OnMouseWheel(zDelta, Point(point.x, point.y));
	return CView::OnMouseWheel(nFlags, zDelta, point);
}

void CTimelineEditView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	SetTip();
}


void CTimelineEditView::OnSizing(UINT fwSide, LPRECT pRect)
{
	CWnd::OnSizing(fwSide, pRect);

	// TODO: Add your message handler code here
	CRect rect;
	this->GetClientRect(&rect);
	m_facade.OnSizing(CRectToRect(rect));

	ReleaseBackDC();
	CreateBackDC(rect.right, rect.bottom);
}

LRESULT CTimelineEditView::OnTMBegin(WPARAM wParam, LPARAM lParam)
{
#ifdef _ENABLE_GPU
	auto tl_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	tl_manager->DoPlayback_AllOutDevices(PBC_CLOSE);
#endif

	//m_facade.Pause("");
	m_facade.SeekToBegin();
	auto m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	m_manager->ClearAll();
	return true;
}

LRESULT CTimelineEditView::OnTMPlay(WPARAM wParam, LPARAM lParam)
{

#ifdef _ENABLE_GPU
	extern CArray<CPanel*, CPanel*> g_PannelAry;
	/*
	DisplayManager* pDisplayManager = g_PannelAry[0]->GetDisplayManager();

	for (int x = 0; x < g_MediaStreamAry.size(); x++)
	{
		g_MediaStreamAry.at(x)->EnableGPU2(true, pDisplayManager->GetD3DDevice());
	}
	*/
	for (int x = 0; x < g_PannelAry.GetCount(); x++)
	{
		if (x > 0)
		{
			DisplayManager* pCurDisplayManager = g_PannelAry[x]->GetDisplayManager();
			if (pCurDisplayManager)
			{
				//g_MediaStreamAry.at(x)->EnableGPU2(true, 1 + x, pCurDisplayManager->GetD3DDevice());
				for (int y = 0; y < g_MediaStreamAry.size(); y++)
					g_MediaStreamAry.at(y)->EnableGPU2(true, x, pCurDisplayManager->GetD3DDevice());
			}
		}
	}
#endif

	m_facade.RemoveTimeCodeQueue();
	//m_facade.Stop("");

	m_facade.PreloadSource();
	m_facade.Start("");
	return true;
}

LRESULT CTimelineEditView::OnTMPause(WPARAM wParam, LPARAM lParam)
{
	m_facade.Pause("");
	return true;
}

LRESULT CTimelineEditView::OnTMReturnFileName(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 1)
	{
		m_facade.RetunFileName();
		SetFocus();
	}

	auto m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	::EnableWindow(m_manager->GetDisplayManager()->GetTimeLineHwnd(),true);

	return true;
}

LRESULT CTimelineEditView::OnUpdateHand(WPARAM wParam, LPARAM lParam)
{
	int iPos = wParam;
	/*
	char szMsg[512];
	sprintf(szMsg, "CTimelineEditView::OnUpdateHand - Pos [%d]\n", iPos);
	OutputDebugString(szMsg);
	*/

	m_facade.Pause("");

	int iTmp = -(iPos - (iPos % 10)) * 100;
	m_facade.RemoveTimeCodeQueue();

	m_facade.UpdateTimeCode3(iPos * 1000);
	m_facade.SetTimeBarOffset(iTmp);
	m_facade.UpdateHand(true);

	m_facade.Start2("");

	return true;
}

LRESULT CTimelineEditView::OnSetTimeBarOffset(WPARAM wParam, LPARAM lParam)
{
	unsigned long ulOffset = wParam;
	m_facade.SetTimeBarOffset(ulOffset);
	return true;
}

LRESULT CTimelineEditView::OnSetVerticalScrollShift(WPARAM wParam, LPARAM lParam)
{
	long lShift = wParam;
	m_facade.SetVerticalScrollShift(lShift);
	return true;
}

LRESULT CTimelineEditView::OnRedraw(WPARAM wParam, LPARAM lParam)
{
	m_bDoRedraw = true;
	Invalidate(false);
	return true;
}

LRESULT CTimelineEditView::OnUpdateTimeCode(WPARAM wParam, LPARAM lParam)
{
	m_facade.ProcessTimeCode();
	return true;
}

LRESULT CTimelineEditView::OnResetTimeCode(WPARAM wParam, LPARAM lParam)
{
	m_facade.RemoveTimeCodeQueue();
	m_facade.Stop("");
	return true;
}

LRESULT CTimelineEditView::OnBackTLItem(WPARAM wParam, LPARAM lParam)
{
	auto tl_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();

	tl_manager->FreezeShape(true);

	m_facade.SeekToBegin();
	tl_manager->ClearAll();
	tl_manager->DoPlayback_AllOutDevices(PBC_CLOSE);
	tl_manager->Back();
	m_facade.PreloadSource();
	m_facade.Start("");

	m_bDoRedraw = true;

	tl_manager->FreezeShape(false);
	return true;
}

LRESULT CTimelineEditView::OnNextTLItem(WPARAM wParam, LPARAM lParam)
{
	auto tl_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();

	tl_manager->FreezeShape(true);

	m_facade.RemoveTimeCodeQueue();
	m_facade.SeekToBegin();
	tl_manager->ClearAll();
	tl_manager->DoPlayback_AllOutDevices(PBC_CLOSE);
	tl_manager->Next();
	
	m_facade.PreloadSource();
	m_facade.Start("");

	m_bDoRedraw = true;

	tl_manager->FreezeShape(false);
	return true;
}

LRESULT CTimelineEditView::OnTMPlay2(WPARAM wParam, LPARAM lParam)
{
	/*
	TimelineManager* m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	CWnd* pEditViewWnd = m_manager->GetEditViewWnd();

	m_manager->FreezeShape(true);

	m_facade.RemoveTimeCodeQueue();
	m_facade.Stop("");

	m_manager->ClearAll();

	m_facade.PreloadSource();
	m_facade.Start("");


	m_manager->FreezeShape(false);
	*/

	m_bDoPlay = true;

	return true;
}

void CTimelineEditView::DoPlay()
{
//	if (m_bDoPlay)
	{
//		m_bDoPlay = false;

		TimelineManager* m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
		CWnd* pEditViewWnd = m_manager->GetEditViewWnd();

		m_manager->FreezeShape(true);

		m_facade.RemoveTimeCodeQueue();
		m_facade.Stop("");

//		m_manager->DoPlayback_AllOutDevices(PBC_CLOSE);
		m_manager->ClearAll();

		m_facade.PreloadSource();
		m_facade.Start("");


		m_manager->FreezeShape(false);
	}
}

BOOL CTimelineEditView::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	COPYDATASTRUCT * p = pCopyDataStruct;
	switch (p->dwData)
	{
		case UPDATE_TRACK_CONTENT:
			{
				RECT* pRect = (RECT*)p->lpData;
				m_rUpdateRegion.left = pRect->left;
				m_rUpdateRegion.top = pRect->top;
				m_rUpdateRegion.right = pRect->right;
				m_rUpdateRegion.bottom = pRect->bottom;

				//Clear(rRegion.left, rRegion.top, rRegion.right - rRegion.left, rRegion.bottom - rRegion.top);

				m_bUpdateTrackContent = true;
			}
			break;
		case UPDATE_BUTTON_PANEL:
			{
				RECT* pRect = (RECT*)p->lpData;
				m_rButtonPanelRegion.left = pRect->left;
				m_rButtonPanelRegion.top = pRect->top;
				m_rButtonPanelRegion.right = pRect->right;
				m_rButtonPanelRegion.bottom = pRect->bottom;
				m_bUpdateButtonPanel = true;
			}
			break;
		case UPDATE_CURTIMECODE:
			{
				RECT* pRect = (RECT*)p->lpData;
				m_rCurTimeCodeRegion.left = pRect->left;
				m_rCurTimeCodeRegion.top = pRect->top;
				m_rCurTimeCodeRegion.right = pRect->right;
				m_rCurTimeCodeRegion.bottom = pRect->bottom;
				m_bUpdateCurTimeCode = true;
			}
			break;
		case UPDATE_TIME_CODE_BAR:
			{
				RECT* pRect = (RECT*)p->lpData;
				m_rTimeCodeBarRegion.left = pRect->left;
				m_rTimeCodeBarRegion.top = pRect->top;
				m_rTimeCodeBarRegion.right = pRect->right;
				m_rTimeCodeBarRegion.bottom = pRect->bottom;
				m_bUpdateTimeCodeBar = true;
			}
			break;
		case UPDATE_TRACK_CONTENT2:
			{
				TrackContentEvent* pTCEvent = (TrackContentEvent*)p->lpData;
				pTCEvent->cur_time_code_rect;

				m_rUpdateRegion.left = pTCEvent->track_content_rect.left;
				m_rUpdateRegion.top = pTCEvent->track_content_rect.top;
				m_rUpdateRegion.right = pTCEvent->track_content_rect.right;
				m_rUpdateRegion.bottom = pTCEvent->track_content_rect.bottom;

				m_rCurTimeCodeRegion.left = pTCEvent->cur_time_code_rect.left;
				m_rCurTimeCodeRegion.top = pTCEvent->cur_time_code_rect.top;
				m_rCurTimeCodeRegion.right = pTCEvent->cur_time_code_rect.right;
				m_rCurTimeCodeRegion.bottom = pTCEvent->cur_time_code_rect.bottom;

				m_bUpdateCurTimeCode = true;
				m_bUpdateTrackContent = true;
			}
			break;
		case EFE_INSERT_EFFECT:
			{
				EffectInfo* pEffectInfo = (EffectInfo*)p->lpData;
				m_facade.InsertEffect(pEffectInfo);
			}
			break;
		case AUDIO_CONFIG_OK:
			m_facade.AudioConfigOK();
			break;
	}
	return true;
}

void CTimelineEditView::CreateBackDC(int iWidth, int iHeight)
{
	m_hTempDC = 0;

	memset(&bmpInfo, 0, sizeof(BITMAPINFOHEADER));
	bmpInfo.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.biWidth = iWidth;
	bmpInfo.biHeight = iHeight;
	bmpInfo.biPlanes = 1;
	bmpInfo.biBitCount = 24;
	bmpInfo.biCompression = BI_RGB;
	bmpInfo.biSizeImage = iWidth * iHeight * 3;
	bmpInfo.biSizeImage = 0;

	LPVOID lpBits;
	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = iWidth;
	bmiHeader.biHeight = iHeight;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 24;
	bmiHeader.biCompression = BI_RGB;
	bmiHeader.biSizeImage = 0;
	bmiHeader.biXPelsPerMeter = 0;
	bmiHeader.biYPelsPerMeter = 0;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;

	m_hBackDC = CreateCompatibleDC(NULL);
	m_hBitmap = CreateDIBSection(m_hBackDC, (LPBITMAPINFO)&bmiHeader, DIB_RGB_COLORS, &lpBits, NULL, NULL);
	m_hBackOldBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBitmap);
	BitBlt(m_hTempDC, 0, 0, iWidth, iHeight, m_hBackDC, 0, 0, SRCCOPY);

	m_hBlackBrush = CreateSolidBrush(RGB(0,0,0));

	m_pGraphics = new Gdiplus::Graphics(m_hBackDC);
	//m_pGraphics->SetInterpolationMode(InterpolationMode::InterpolationModeDefault);
}

void CTimelineEditView::ReleaseBackDC()
{
	SelectObject(m_hBackDC, m_hBackOldBitmap);
	::DeleteDC(m_hBackDC);
	DeleteObject(m_hBitmap);

	if (m_pGraphics)
		delete m_pGraphics;

	if (m_hBlackBrush)
		DeleteObject(m_hBlackBrush);
}

void CTimelineEditView::Clear(int x,int y,int w,int h)
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = rect.left + w;
	rect.bottom = rect.top + w;
	FillRect(m_hBackDC, &rect, m_hBlackBrush);
}

void CTimelineEditView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 

	if (m_bDoPlay)
	{
		m_bDoPlay = false;
		DoPlay();
	}
	
	CWnd::OnTimer(nIDEvent);
}

void CTimelineEditView::AddTip(wchar_t* wszTipID, int left, int top, int right, int bottom)
{
	wchar_t wszID[512];
	wchar_t wszData[512];
	wchar_t wszAppName[128];
	wchar_t wszKeyName[128];
	char szIniFile[512];
	wchar_t wszIniFile[512];
	sprintf(szIniFile, "%s\\Language\\timeline_%s.ini", theApp.m_strCurPath, theApp.m_strLanguage);

	wcscpy(wszIniFile, ANSIToUnicode(szIniFile));

	int iTotal = GetPrivateProfileInt("Base", "Total", 0, szIniFile);
	for (int i = 0; i < iTotal; i++)
	{
		swprintf(wszAppName, L"Item%d", i);
		GetPrivateProfileStringW(wszAppName, L"ID", L"", wszID, 512, wszIniFile);

		if (wcscmp(wszID, wszTipID) == 0)
		{
			GetPrivateProfileStringW(wszAppName, L"Str", L"", wszData, 512, wszIniFile);

			TipInfoW tip_info;
			tip_info.hWnd = this->GetSafeHwnd();
			wcscpy(tip_info.wszMsg, wszData);
			tip_info.rect.left = left;
			tip_info.rect.top = top;
			tip_info.rect.right = right;
			tip_info.rect.bottom = bottom;

			m_pUTipDll->_UT_Add(m_pUTipDll->GetUTObj(), tip_info.hWnd, tip_info.rect, tip_info.wszMsg, tip_info.ulTipID);

			break;
		}
	}
}

void Timeline::CTimelineEditView::SetTip()
{
	CRect rect;
	this->GetClientRect(&rect);
	m_facade.OnSize(CRectToRect(rect));

	ReleaseBackDC();
	CreateBackDC(rect.right, rect.bottom);

	m_pUTipDll->_UT_Reset(m_pUTipDll->GetUTObj());

	AddTip(L"Back", 10, 8, 31, 31);
	AddTip(L"Next", 150, 8, 31, 31);
	AddTip(L"10S", 45, 8, 29, 29);
	AddTip(L"1M", 80, 8, 29, 29);
	AddTip(L"10M", 115, 8, 29, 29);

	int x = rect.right - (30 * 9) - 65;
	AddTip(L"Normal", x, 8, 30, 30);

	x = rect.right - (30 * 8) - 60;
	AddTip(L"Insert", x, 8, 30, 30);

	x = rect.right - (30 * 7) - 55;
	AddTip(L"Insert Video", x, 8, 30, 30);

	x = rect.right - (30 * 6) - 50;
	AddTip(L"Move", x, 8, 30, 30);

}
