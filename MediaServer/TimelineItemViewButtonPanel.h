/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineItemViewButtonPanel.h

Declaration of following classes:
TimelineItemViewButton
TimelineItemViewButtonPanel

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
#include <boost/signals2.hpp>

// ==== Include local library ==== //
#include "TLDllExport.h"
#include "TLButtonBase.h"


namespace Timeline
{

	/* ====================================================== *\
	TimelineItemViewButton
	\* ====================================================== */
	class TIMELINELIB_EXPORTS TimelineItemViewButton : public TLButtonBase
	{
		
	};

	class TIMELINELIB_EXPORTS TimelineItemViewButton2 : public TLBmpButtonBase
	{

	};

	/* ====================================================== *\
	TimelineItemViewButtonPanel
	\* ====================================================== */
	class TIMELINELIB_EXPORTS TimelineItemViewButtonPanel : public TLShape
	{
		using Signal = boost::signals2::signal<void(std::string)>;
	public:
		// ==== Constants ==== //
		struct VisualConstant
		{
			const static int Height = 45;
		};

		// ==== Constructor & Destructor ==== //
	public:
		TimelineItemViewButtonPanel();
		~TimelineItemViewButtonPanel();

	public:
		//========== Public member methods ========== //
		void MouseLeftButtonDown(const Gdiplus::Point& point);
		void MouseLeftButtonUp(const Gdiplus::Point& point);

		void DeselectAll();

		void Draw(Gdiplus::Graphics& graphics);
		bool CheckStatus(const Gdiplus::Point& point);

		// ==== Public signal ==== //
	public:
		Signal OpenClick;
		Signal SaveClick;
		Signal AddClick;
		Signal DeleteClick;

		//========== Private member fields ========== //
	private:
		std::vector<TimelineItemViewButton2*> m_buttonCollection;
		
		TimelineItemViewButton2 m_btnOpen;
		TimelineItemViewButton2 m_btnSave;
		TimelineItemViewButton2 m_btnAdd;
		TimelineItemViewButton2 m_btnDelete;

		std::shared_ptr<Gdiplus::Image> m_logo_image;
		std::shared_ptr<Gdiplus::Image> m_split_line_image;

		int m_iGroupIndex;
		int m_iButtonIndex;
	};
}