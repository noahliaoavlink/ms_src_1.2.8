/********************************************************************\
Project Name: Timeline Prototype

File Name: TimeCodeBar.h

Declaration of following classes:
TimeCodeBar

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

// ==== Include local library ==== //
#include "TLDllExport.h"
#include "TLShape.h"

namespace Timeline
{

	/* ====================================================== *\
	TimeCodeBar
	\* ====================================================== */
	class TIMELINELIB_EXPORTS TimeCodeBar : public TLShape
	{
	public:
		// ==== Constants ==== //
		class VisualConstant
		{
		public:
			const static int Height = 30;
			const static int BaseXShift = 30;
			const static int BaseYShift = 15;
		};

		// ==== Constructor & Destructor ==== //
	public:
		TimeCodeBar();
		~TimeCodeBar();

		// ==== Public member methods ==== //
	public:
		void SetOffset(int iOffset);
		int GetOffset();

		void MouseLeftButtonDown(const Gdiplus::Point& point);
		void MouseLeftButtonUp(const Gdiplus::Point& point);
		void MouseMove(const Gdiplus::Point& point);
		void Draw(Gdiplus::Graphics& graphics);
		void DrawTimeCode(Gdiplus::Graphics& graphics, const std::string& text, const Gdiplus::Point& point);

	private:
		Gdiplus::Point m_lastPoint;
		int m_horizontal_scroll_shift = 0;
	};

}