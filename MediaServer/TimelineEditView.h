/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineEditView.h

Declaration of following classes:
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

#pragma once
// ==== Inlcude STL library ==== //
#include <memory>
#include <vector>

// ==== Include local library ==== //
#include "TLDllExport.h"
#include "TimelineEditFacade.h"
#include "UTipDll.h"

namespace Timeline
{
	/* ====================================================== *\
	CTimelineEditView
	\* ====================================================== */

	class TIMELINELIB_EXPORTS CTimelineEditView : public CView
	{
	public: // create from serialization only
		CTimelineEditView();
		DECLARE_DYNCREATE(CTimelineEditView)

		// Attributes
	public:
#ifdef TIMELINEDEV
		static CRuntimeClass* __stdcall GetThisClass2();
#endif
		// Operations
	public:

		// Overrides
	public:
		virtual void OnDraw(CDC* pDC);  // overridden to draw this view
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:

		// Implementation
	public:
		virtual ~CTimelineEditView();
#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
#endif

	protected:

		// Generated message map functions
	protected:
		afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
		DECLARE_MESSAGE_MAP()

		//========== Event handler ========== //
	public:
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnSizing(UINT fwSide, LPRECT pRect);

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);

		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

		afx_msg LRESULT OnTMBegin(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnTMPlay(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnTMPause(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnTMReturnFileName(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnUpdateHand(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnSetTimeBarOffset(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnSetVerticalScrollShift(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnRedraw(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnUpdateTimeCode(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnResetTimeCode(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnBackTLItem(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnNextTLItem(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnTMPlay2(WPARAM wParam, LPARAM lParam);
		afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

		afx_msg void OnTimer(UINT_PTR nIDEvent);

		void DoPlay();
		void CreateBackDC(int iWidth, int iHeight);
		void ReleaseBackDC();
		void Clear(int x, int y, int w, int h);
		void AddTip(wchar_t* wszTipID, int left, int top, int right, int bottom);
		void SetTip();
	protected:
		afx_msg void OnDestroy();

		//========== Private member fields ========== //
	private:
		TimelineEditFacade m_facade;
		Gdiplus::Graphics* m_pGraphics;
		HDC m_hTempDC;
		HBITMAP m_hBitmap;
		HDC m_hBackDC;
		HBITMAP m_hBackOldBitmap;
		BITMAPINFOHEADER bmpInfo;
		HBRUSH m_hBlackBrush;

		bool m_bUpdateTrackContent;
		bool m_bUpdateButtonPanel;
		bool m_bUpdateCurTimeCode;
		bool m_bUpdateTimeCodeBar;
		RECT m_rUpdateRegion;
		RECT m_rButtonPanelRegion;
		RECT m_rCurTimeCodeRegion;
		RECT m_rTimeCodeBarRegion;

		bool m_bDoRedraw;

		bool m_bDoPlay;

		UTipDll* m_pUTipDll;
	};


}
