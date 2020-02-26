/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineService.h

Declaration of following classes:
TimelineService

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
#include "TimeCode.h"
#include "TimeCodeEngine.h"
#include "TimelineManager.h"


namespace Timeline
{

	/* ====================================================== *\
	TimelineService
	\* ====================================================== */

	class TIMELINELIB_EXPORTS TimelineService
	{
		// ==== Constructor & Destructor ==== //
	public:
		TimelineService();
		virtual ~TimelineService();

		// ==== Public member methods ==== //
	public:
		TimeCodeEngine* GetTimeCodeEngine();

		TimelineManager* GetTimelineManager();


		// ==== Private member fields ==== //
	private:
		TimeCode m_timecode;
		TimeCodeEngine m_engine;
		TimelineManager m_timelineManager;
	};

}