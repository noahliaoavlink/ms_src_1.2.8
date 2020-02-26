/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineItemFacade.h

Declaration of following classes:
TimelineItemFacade

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
#include "TimelineManager.h"


namespace Timeline
{

	/* ====================================================== *\
	TimelineItemFacade
	\* ====================================================== */
	class TIMELINELIB_EXPORTS TimelineItemFacade
	{
		// ==== Constructor & Destructor ==== //
	public:
		TimelineItemFacade();
		virtual ~TimelineItemFacade();

		//========== Public member methods ========== //
	public:
		void OnInitialize(CWnd* wnd);
		void OnTerminate();
		void Draw(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect);
		void OnCreate(const Gdiplus::Rect& rect);
		void OnDestroy();
		void OnSize(const Gdiplus::Rect& rect);
		void OnSizing(const Gdiplus::Rect& rect);

		void OnLButtonDown(const Gdiplus::Point& point);
		void OnLButtonUp(const Gdiplus::Point& point);
		void OnMouseMove(const Gdiplus::Point& point);
		void OnMouseWheel(short zDelta, const Gdiplus::Point& point);
		void OnLButtonDblClk(const Gdiplus::Point& point);
		void OnKeyDown(unsigned int key);
		void OnKillFocus();

		void OpenProjectFile();

		//========== Private member methods ========== //
	private:
		void Add(std::string msg);
		void Delete(std::string msg);
		void Open(std::string msg);
		void Save(std::string msg);
		void StatusChanged(std::string msg);
		void AdjustLayout(const Gdiplus::Rect& rect);

		//========== Private member fields ========== //
	private:
		CWnd* m_wnd;
		CClientDC *m_pDC;

		Gdiplus::Point m_mousePosition;
		Gdiplus::Rect m_clientRect;

		CRect m_ButtonPanelRect;

		int m_vertical_scroll_shift = 0;

		TimelineItemViewButtonPanel m_buttonPanel;

		TimelineManager* m_manager;

		char m_szProjectFileName[512];
	};

}