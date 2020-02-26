/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineItemView.h

Declaration of following classes:
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

#pragma once
// ==== Inlcude Microsoft MFC library ==== //
#include "stdafx.h"

// ==== Inlcude STL library ==== //
#include <memory>
#include <vector>

// ==== Include local library ==== //
#include "TLDllExport.h"
#include "TimelineItemViewButtonPanel.h"
#include "TimelineItemFacade.h"
#include "TimelineManager.h"
#include "UTipDll.h"

namespace Timeline
{

	/* ====================================================== *\
	CTimelineItemView
	\* ====================================================== */
	class TIMELINELIB_EXPORTS CTimelineItemView : public CView
	{
	public:
		CTimelineItemView();
		DECLARE_DYNCREATE(CTimelineItemView)

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
		virtual ~CTimelineItemView();
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

		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnKillFocus(CWnd* pNewWnd);

		afx_msg LRESULT OnOpenProjectFile(WPARAM wParam, LPARAM lParam);

		void AddTip(wchar_t* wszTipID, int left, int top, int right, int bottom);
		void SetTip();
	protected:
		afx_msg void OnDestroy();

		

		//========== Private member fields ========== //
	private:
		TimelineItemFacade m_facade;
		UTipDll* m_pUTipDll;
	};

}
