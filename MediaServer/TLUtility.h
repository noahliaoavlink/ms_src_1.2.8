/********************************************************************\
Project Name: Timeline Prototype

File Name: TLUtility.h

Declaration of following classes:

Declaration of following functions:
towstring
ShowMessageBox
DumpToLog
DumpToEvent
DumpToAction

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

// ==== Include local library ==== //
#include "TLDllExport.h"

namespace Timeline
{
	TIMELINELIB_EXPORTS std::wstring towstring(const std::string& v);

	TIMELINELIB_EXPORTS void ShowMessageBox(const std::string& msg);
	TIMELINELIB_EXPORTS void DumpToLog(const std::string& msg);
	TIMELINELIB_EXPORTS void DumpToEvent(const std::string& msg);
	TIMELINELIB_EXPORTS void DumpToAction(const std::string& msg);
	//TIMELINELIB_EXPORTS void DumpToHistory(std::string& msg);

	TIMELINELIB_EXPORTS Gdiplus::Rect CRectToRect(const CRect& rect);
}
