/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineService.cpp

Definition of following classes:
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


// ==== Inlcude Microsoft MFC library ==== //
#include "stdafx.h"

// ==== Inlcude STL library ==== //
#include <limits>
#include <sstream>

// ==== Include local library ==== //
#include "TimelineService.h"

using namespace Timeline;

/* ====================================================== *\
TimelineService
\* ====================================================== */

TimelineService::TimelineService()
{
}

TimelineService::~TimelineService()
{
}

TimeCodeEngine* TimelineService::GetTimeCodeEngine()
{
	return &m_engine;
}

TimelineManager* TimelineService::GetTimelineManager()
{
	return &m_timelineManager;
}