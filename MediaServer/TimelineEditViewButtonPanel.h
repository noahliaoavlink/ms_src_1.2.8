/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineEditViewButtonPanel.h

Declaration of following classes:
TimelineEditViewButton
TimelineEditViewButtonPanel

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
#include <boost/signals2.hpp>

// ==== Include local library ==== //
#include "TLDllExport.h"
#include "TLConstant.h"
#include "TLShape.h"
#include "TLButtonBase.h"


namespace Timeline
{

	/* ====================================================== *\
	TimelineEditViewButton
	\* ====================================================== */
	class TIMELINELIB_EXPORTS TimelineEditViewButton : public TLButtonBase
	{

	};

	class TIMELINELIB_EXPORTS TimelineEditViewButton2 : public TLBmpButtonBase
	{

	};

	/* ====================================================== *\
	TimelineEditViewButtonPanel
	\* ====================================================== */
	class TIMELINELIB_EXPORTS TimelineEditViewButtonPanel : public TLShape
	{
	public:
		using Signal = boost::signals2::signal<void(std::string)>;
		using PointerModeSignal = boost::signals2::signal<void(PointerMode)>;
		using LineModeSignal = boost::signals2::signal<void(LineMode)>;
		using UnitModeSignal = boost::signals2::signal<void(UnitMode)>;

	public:
		// ==== Constants ==== //
		class VisualConstant
		{
		public:
			const static int Height = 45;//80;
		};

		// ==== Constructor & Destructor ==== //
	public:
		TimelineEditViewButtonPanel();
		~TimelineEditViewButtonPanel();

	public:
		//========== Public member methods ========== //
		void SetRect(int x, int y, int width, int height);
		void SetRect(const Gdiplus::Rect& rect);

		bool MouseLeftButtonDown(const Gdiplus::Point& point);
		void MouseLeftButtonUp(const Gdiplus::Point& point);
		void Draw(Gdiplus::Graphics& graphics);

		bool CheckStatus(const Gdiplus::Point& point);
		// ==== Public signal ==== //
	public:
		Signal StartClick;
		Signal PauseClick;
		Signal StopClick;

		Signal BackClick;
		Signal NextClick;

		PointerModeSignal PointerModeChanged;
		LineModeSignal LineModeChanged;
		UnitModeSignal UnitModeChanged;

		int m_iGroupIndex;
		int m_iButtonIndex;

		//========== Public member methods ========== //
	public:
		PointerMode GetPointerMode();

		//========== Private member methods ========== //
	private:
		void AdjustLayout();

		//========== Private member fields ========== //
	private:
		std::vector<TimelineEditViewButton2*> m_buttonGroup1;
		//TimelineEditViewButton m_btnStop;
		//TimelineEditViewButton m_btnStart;
		//TimelineEditViewButton m_btnPause;
		TimelineEditViewButton2 m_btnBack;
		TimelineEditViewButton2 m_btnNext;

		std::vector<TimelineEditViewButton2*> m_buttonGroup2;
		TimelineEditViewButton2 m_btnEdit;
		TimelineEditViewButton2 m_btnInsert_Point;
		TimelineEditViewButton2 m_btnInsert_Video;
		TimelineEditViewButton2 m_btnMove;

		std::vector<TimelineEditViewButton2*> m_buttonGroup3;
		TimelineEditViewButton2 m_btnLineNormal;
		TimelineEditViewButton2 m_btnLineSoar;
		TimelineEditViewButton2 m_btnLinePlunge;
		TimelineEditViewButton2 m_btnLineFlatSoar;
		TimelineEditViewButton2 m_btnLineCurve;

		std::vector<TimelineEditViewButton*> m_buttonGroup4;
		TimelineEditViewButton m_btnUnit1;  //10 sec
		TimelineEditViewButton m_btnUnit2; //1 min
		TimelineEditViewButton m_btnUnit3; //10 mins

		std::shared_ptr<Gdiplus::Image> m_split_line_image;

		PointerMode m_pointerMode;
		LineMode m_LineMode;
		UnitMode m_UnitMode;
	};

	
}
