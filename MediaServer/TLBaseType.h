/********************************************************************\
Project Name: Timeline Prototype

File Name: TLBaseType.h

Declaration of following classes:
TLThickness
TLGeometricTransform
TLFadeOption

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
#include <string>
#include <vector>

// ==== Include local library ==== //
#include "TLDllExport.h"

namespace Timeline
{

	/* ====================================================== *\
			  TLThickness
	\* ====================================================== */

	class TIMELINELIB_EXPORTS TLThickness
	{
		// ==== Constructor & Destructor ==== //
	public:
		TLThickness();
		TLThickness(float thickness);
		TLThickness(float left, float top, float right, float bottom);
		virtual ~TLThickness();

		// ==== Public member methods ==== //
	public:

		// ==== Public member fields ==== //
	public:
		float Bottom;
		float Left;
		float Right;
		float Top;
	};

	/* ====================================================== *\
			 TLGeometricTransform
	\* ====================================================== */

	class TIMELINELIB_EXPORTS TLGeometricTransform
	{
		// ==== Public member methods ==== //
	public:
		static void Translate(Gdiplus::Point& pt, float x, float y);
		static void Translate(Gdiplus::Rect& rect, float x, float y);
	};

	/* ====================================================== *\
			  TLFadeOption
	\* ====================================================== */
	enum class TLFadeOption
	{
		None = 0,
		Linear = 1,
		Spline = 2,
		SnapStart = 3,
		SnapEnd = 4,
		SnapMiddle = 5
	};

}