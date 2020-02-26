/********************************************************************\
Project Name: Timeline Prototype

File Name: TLItemVis.h

Declaration of following classes:
TLItemVis

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
#include <boost/signals2.hpp>

// ==== Include local library ==== //
#include "TLDllExport.h"
#include "TLItems.h"
#include "TLShape.h"
#include "TLButtonBase.h"

enum TLItemStatus
{
	TLIS_BEGIN,
	TLIS_START,
	TLIS_PAUSE,
	TLIS_CLOSE,
};

namespace Timeline
{

#ifdef _NEW_TIME_LINE
	/* ====================================================== *\
	TLItemVisButton
	\* ====================================================== */
	class TIMELINELIB_EXPORTS TLItemVisButton : public TLButtonBase
	{
			Gdiplus::Rect _Region;
			bool m_bActive;
			bool m_bUpdate;
		public:
			TLItemVisButton();

			void SetRegion(Gdiplus::Rect rect);
			void SetRegion(int x, int y, int w, int h);
			Gdiplus::Rect& GetRegion();
			void MouseLeftButtonDown(const Gdiplus::Point& point);
			void MouseMove(const Gdiplus::Point& point);
			void Draw(Gdiplus::Graphics& graphics);
			bool CheckStatus();
	};

	class TIMELINELIB_EXPORTS TLItemVisButton2 : public TLBmpButtonBase
	{
		Gdiplus::Rect _Region;
		bool m_bActive;
		bool m_bUpdate;
	public:
		TLItemVisButton2();

		void SetRegion(Gdiplus::Rect rect);
		void SetRegion(int x, int y, int w, int h);
		Gdiplus::Rect& GetRegion();
		void MouseLeftButtonDown(const Gdiplus::Point& point);
		void MouseMove(const Gdiplus::Point& point);
		void Draw(Gdiplus::Graphics& graphics);
		bool CheckStatus();
	};
#endif

	/* ====================================================== *\
			 TLItemVis
	\* ====================================================== */
	class TLItemVis;
	using TLItemVisPtr = std::shared_ptr<TLItemVis>;
	using TLItemVisWeakPtr = std::weak_ptr<TLItemVis>;
	using ItemVisVector = std::vector<TLItemVisPtr>;
	using ItemVisVectorIter = ItemVisVector::iterator;
	using ItemVisPair = std::pair<std::string, TLItemVisPtr>;
	using ItemVisMap = std::map<std::string, TLItemVisPtr>;
	using ItemVisMapIter = ItemVisMap::iterator;
	class TIMELINELIB_EXPORTS TLItemVis : public TLShape
	{
		using Signal = boost::signals2::signal<void(std::string)>;

	public:
		// ==== Constants ==== //
		class VisualConstant
		{
		public:
			const static int Width = 300;
			const static int Height = 140;
			const static int CollapseHeight = 80;
		};

		// ==== Enumeration ==== //
		enum class ExpandStatus
		{
			Expand, Collapse
		};

		// ==== Constructor & Destructor ==== //
	public:
		TLItemVis();
		virtual ~TLItemVis();

		// ==== Public member methods ==== //
	public:
		void SetPosition(int x, int y);

		void Initialize(TLItemPtr item);
		TLItemPtr& GetContext();

		void MouseClick(const Gdiplus::Point& point);
		void LButtonUp(const Gdiplus::Point& point);
		void MouseMove(const Gdiplus::Point& point);
		void MouseDbClick(const Gdiplus::Point& point);

		void Expand();
		void Collapse();
		bool IsExpand();

		void Draw(Gdiplus::Graphics& graphics);
		bool CheckButtonStatus();
		int GetStatus();
		void SetStatus(int iStatus);

		// ==== Private member methods ==== //
	private:
		void DrawBackgroundExpand(Gdiplus::Graphics& graphics);
		void DrawExpanderExpand(Gdiplus::Graphics& graphics);
		void DrawTitleExpand(Gdiplus::Graphics& graphics);
		void DrawIdExpand(Gdiplus::Graphics& graphics);
		void DrawControlExpand(Gdiplus::Graphics& graphics);
		void DrawTotalTimeExpand(Gdiplus::Graphics& graphics);
		void DrawProgressExpand(Gdiplus::Graphics& graphics);


		void DrawBackgroundCollapse(Gdiplus::Graphics& graphics);
		void DrawExpanderCollapse(Gdiplus::Graphics& graphics);
		void DrawTitleCollapse(Gdiplus::Graphics& graphics);
		void DrawIdCollapse(Gdiplus::Graphics& graphics);
		void DrawControlCollapse(Gdiplus::Graphics& graphics);
		void DrawTotalTimeCollapse(Gdiplus::Graphics& graphics);
		void DrawProgressCollapse(Gdiplus::Graphics& graphics);

		void DrawFocusBorder(Gdiplus::Graphics& graphics);

		void SetExpandRegion();
		void SetCollapseRegion();

		// ==== Public signal ==== //
	public:
		Signal ExpanderClick;
		Signal TitleClick;
		Signal IdClick;
		Signal ProgressClick;
		Signal ControlClick;
		Signal TotalTimeClick;

#ifdef _NEW_TIME_LINE
		Signal BeginClick;
		Signal StartClick;
		Signal PauseClick;
#endif

		// ==== Private member fields ==== //
	private:
		TLItemPtr _item;

		ExpandStatus _expandStatus;
		
		Gdiplus::Rect _expanderRegion;
		Gdiplus::Rect _titleRegion;
		Gdiplus::Rect _idRegion;
		Gdiplus::Rect _controlRegion;
		Gdiplus::Rect _totalTimeRegion;
		Gdiplus::Rect _progressRegion;

		std::shared_ptr<Gdiplus::Image> m_backwardImage;
		std::shared_ptr<Gdiplus::Image> m_startImage;
		std::shared_ptr<Gdiplus::Image> m_pauseImage;

		int m_iStatus;

#ifdef _NEW_TIME_LINE
		std::vector<TLItemVisButton2*> m_buttonCollection;
		TLItemVisButton2 m_btnBegin;
		TLItemVisButton2 m_btnStart;
		TLItemVisButton2 m_btnPause;
#endif

	};

}
