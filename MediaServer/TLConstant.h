/********************************************************************\
Project Name: Timeline Prototype

File Name: TConstant.h

Declaration of following classes:
TLFont

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
#include <map>

// ==== Include local library ==== //
#include "TLDllExport.h"

namespace Timeline
{
	// ==== Enumeration ==== //
	/* ====================================================== *\
	PointerMode
	\* ====================================================== */
	enum class PointerMode
	{
		Edit = 1,
		Insert_Point = 2,
		Insert_Video = 3,
		Move = 4
	};

	enum class LineMode
	{
		LM_NORMAL = 1,
		LM_SOAR = 2,
		LM_PLUNGE = 3,
		LM_FLAT_SOAR = 4,
		LM_CURVE = 5,
	};

	enum class UnitMode
	{
		UM_10_SEC = 1,
		UM_1_MIN,
		UM_10_MINS,
	};

	/* ====================================================== *\
	TLFont
	\* ====================================================== */

	class TIMELINELIB_EXPORTS TLFont
	{
		// ==== Constructor & Destructor ==== //
	public:
		TLFont();
		virtual ~TLFont();

		// ==== Public member methods ==== //
	public:
		static TLFont& Instance();
		Gdiplus::Font* GetFont(int size);

		// ==== Private member methods ==== //
	private:
		static TLFont* m_this;
		std::map<int, std::shared_ptr<Gdiplus::Font> > m_fonts;
	};

}
