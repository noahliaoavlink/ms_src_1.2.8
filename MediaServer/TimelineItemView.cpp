/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineItemView.cpp

Definition of following classes:
CTimelineItemView

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
#include <sstream>
#include <boost/format.hpp>

// ==== Include local library ==== //
#include "TimelineItemView.h"
#include "TLUtility.h"
#include "DoubleBuffer.h"
#include "ServiceProvider.h"
#include "../../../Include/StrConv.h"
#include "MediaServer.h"

#define ENGINNERING_MODE


using namespace Timeline;

/* ====================================================== *\
CTimelineItemView
\* ====================================================== */

IMPLEMENT_DYNCREATE(CTimelineItemView, CView)

BEGIN_MESSAGE_MAP(CTimelineItemView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_SIZING()
	//ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_TM_OPEN_PROJECT_FILE, OnOpenProjectFile)
END_MESSAGE_MAP()


CTimelineItemView::CTimelineItemView()
{
	m_pUTipDll = 0;

	m_facade.OnInitialize(this);
}

CTimelineItemView::~CTimelineItemView()
{
	m_facade.OnTerminate();
}

#ifdef TIMELINEDEV
CRuntimeClass* CTimelineItemView::GetThisClass2()
{
	return CTimelineItemView::GetThisClass();
}
#endif

BOOL CTimelineItemView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

void CTimelineItemView::OnDraw(CDC* pDC)
{
	CRect rect;
	this->GetClientRect(&rect);
	
	DoubleBuffer doubleBuffer(pDC, this);
	CDC* offScreenDC = doubleBuffer.InitBackBuffer();
	Gdiplus::Graphics graphics(offScreenDC->m_hDC);

	m_facade.Draw(graphics, CRectToRect(rect));
}

void CTimelineItemView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CTimelineItemView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CTimelineItemView diagnostics

#ifdef _DEBUG
void CTimelineItemView::AssertValid() const
{
	CView::AssertValid();
}

void CTimelineItemView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG


// CTimelineItemView message handlers

int CTimelineItemView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	m_pUTipDll = new UTipDll;
	m_pUTipDll->LoadLib();
	m_pUTipDll->_UT_Init(m_pUTipDll->GetUTObj());

	SetTip();

	return 0;
}

void CTimelineItemView::OnDestroy()
{
	m_facade.OnDestroy();

	if (m_pUTipDll)
		delete m_pUTipDll;

	CView::OnDestroy();
}

void CTimelineItemView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_facade.OnLButtonDown(Point(point.x, point.y));
	CWnd::OnLButtonDown(nFlags, point);
}

void CTimelineItemView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_facade.OnLButtonUp(Point(point.x, point.y));
	CWnd::OnLButtonUp(nFlags, point);
}

void CTimelineItemView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_facade.OnKeyDown(nChar);
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTimelineItemView::OnMouseMove(UINT nFlags, CPoint point)
{
	m_facade.OnMouseMove(Point(point.x, point.y));

	m_pUTipDll->_UT_ShowTip(m_pUTipDll->GetUTObj(), this->GetSafeHwnd(), point.x, point.y);

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CTimelineItemView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	m_facade.OnMouseWheel(zDelta, Point(point.x, point.y));
	return CWnd::OnMouseWheel(nFlags, zDelta, point);
}

void CTimelineItemView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_facade.OnLButtonDblClk(Point(point.x, point.y));
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CTimelineItemView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CRect rect;
	this->GetClientRect(&rect);
	m_facade.OnSize(CRectToRect(rect));
}

void CTimelineItemView::OnSizing(UINT fwSide, LPRECT pRect)
{
	CWnd::OnSizing(fwSide, pRect);

	// TODO: Add your message handler code here
	CRect rect;
	this->GetClientRect(&rect);
	m_facade.OnSizing(CRectToRect(rect));
}

void CTimelineItemView::OnKillFocus(CWnd* pNewWnd)
{
	m_facade.OnKillFocus();
}

LRESULT CTimelineItemView::OnOpenProjectFile(WPARAM wParam, LPARAM lParam)
{
	m_facade.OpenProjectFile();
	return true;
}

void CTimelineItemView::AddTip(wchar_t* wszTipID, int left, int top, int right, int bottom)
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

void Timeline::CTimelineItemView::SetTip()
{
	m_pUTipDll->_UT_Reset(m_pUTipDll->GetUTObj());

	AddTip(L"Load Project", 145, 10, 30, 30);
	AddTip(L"Save Project", 190, 10, 30, 30);
	AddTip(L"New Item", 235, 10, 30, 30);
	AddTip(L"Delete Item", 280, 10, 30, 30);
}
