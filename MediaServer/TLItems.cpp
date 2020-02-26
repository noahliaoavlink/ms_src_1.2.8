/********************************************************************\
Project Name: Timeline Prototype

File Name: TLItem.cpp

Definition of following classes:
TLInfo
TLPlayStatus
TLItem

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
#include <boost/format.hpp>
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>

// ==== Include local library ==== //
#include "TLTracks.h"
#include "TLItems.h"

using namespace Timeline;

/* ====================================================== *\
         TLInfo
\* ====================================================== */

TLInfo::TLInfo()
{
	m_uuid = boost::uuids::to_string(boost::uuids::random_generator()());

	m_ulDisplayStartTime = 0;
	m_lTrackVerticalScrollShift = 0;
}

TLInfo::~TLInfo()
{
}

unsigned long TLInfo::Id()
{
	return m_id;
}
void TLInfo::SetId(unsigned long id)
{
	m_id = id;
}

std::string TLInfo::UUID()
{
	return m_uuid;
}

void TLInfo::SetUUID(std::string& uuid)
{
	m_uuid = uuid;
}

std::string TLInfo::Name()
{
	return m_name;
}

void TLInfo::SetName(std::string& name)
{
	m_name = name;
}

TimeCode TLInfo::CurrentTime()
{
	return m_currentTime;
}

void TLInfo::SetCurrentTime(TimeCode timecode)
{
	if (timecode >= m_totalTime)
	{
		m_currentTime = m_totalTime;
	}
	else
	{
		m_currentTime = timecode;
	}
}

TimeCode TLInfo::TotalTime()
{
	return m_totalTime;
}

void TLInfo::SetTotalTime(TimeCode timecode)
{
	m_totalTime = timecode;
}

unsigned long TLInfo::GetProgress()
{
	return static_cast<unsigned long>(((double)m_currentTime.TotalMilliseconds() / (double)m_totalTime.TotalMilliseconds()) * 100);
}

TimeCode TLInfo::GetTimeLeft()
{
	return m_totalTime - m_currentTime;
}

std::string TLInfo::GetProgressString()
{
	std::string output = boost::str(boost::format("%d%") % GetProgress());
	return output;
}

std::string TLInfo::GetCurrentTimeString()
{
	unsigned long currenttime = 0;

	currenttime = m_currentTime.TotalMilliseconds() / 1000;

#if 0
	unsigned long hour = currenttime / 3600;
	unsigned long minute = currenttime / 60;
	unsigned long second = currenttime % 60;
		
	std::string output = boost::str(boost::format("%02d:%02d:%02d") % hour % minute % second);
#else
	int iHour = currenttime / (60 * 60);

	double dNum = currenttime - 3600 * iHour;
	int iMinute = dNum / 60;

	double dNum1 = dNum - 60 * iMinute;
	int iSecond = dNum1;
	std::string output = boost::str(boost::format("%d:%02d:%02d") % iHour % iMinute % iSecond);
#endif

	return output;
}

std::string TLInfo::GetTotalTimeString()
{
	unsigned long totaltime = 0;

	totaltime = m_totalTime.TotalMilliseconds() / 1000;

#if 0
	unsigned long hour = totaltime / 3600;
	unsigned long minute = totaltime / 60;
	unsigned long second = totaltime % 60;

	std::string output = boost::str(boost::format("%d:%d:%d") % hour % minute % second);
#else
	int iHour = totaltime / (60 * 60);

	double dNum = totaltime - 3600 * iHour;
	int iMinute = dNum / 60;

	double dNum1 = dNum - 60 * iMinute;
	int iSecond = dNum1;
	std::string output = boost::str(boost::format("%d:%02d:%02d") % iHour % iMinute % iSecond);
#endif

	return output;
}

std::string TLInfo::GetTotalTimeString2()
{
	unsigned long totaltime = 0;

	totaltime = m_totalTime.TotalMilliseconds() / 1000;

#if 0
	unsigned long hour = totaltime / 3600;
	unsigned long minute = totaltime / 60;
	unsigned long second = totaltime % 60;

	std::string output = boost::str(boost::format("%d:%d:%d") % hour % minute % second);
#else
	int iHour = totaltime / (60 * 60);

	double dNum = totaltime - 3600 * iHour;
	int iMinute = dNum / 60;

	double dNum1 = dNum - 60 * iMinute;
	int iSecond = dNum1;
	int iMS = m_totalTime.TotalMilliseconds() % 1000;
	std::string output = boost::str(boost::format("%d:%02d:%02d.%03d") % iHour % iMinute % iSecond % iMS);
#endif

	return output;
}

std::string TLInfo::GetTimeLeftString()
{
	unsigned long timeleft = 0;
	
#if 0
	timeleft = GetTimeLeft().TotalMilliseconds() / 1000;
#else
	unsigned long totaltime = m_totalTime.TotalMilliseconds() / 1000;
	unsigned long currenttime = m_currentTime.TotalMilliseconds() / 1000;
	timeleft = totaltime - currenttime;
#endif


#if 0
	unsigned long hour = timeleft / 3600;
	unsigned long minute = timeleft / 60;
	unsigned long second = timeleft % 60;

	std::string output = boost::str(boost::format("%d:%d:%d") % hour % minute % second);
#else
	int iHour = timeleft / (60 * 60);

	double dNum = timeleft - 3600 * iHour;
	int iMinute = dNum / 60;

	double dNum1 = dNum - 60 * iMinute;
	int iSecond = dNum1;
	std::string output = boost::str(boost::format("%d:%02d:%02d") % iHour % iMinute % iSecond);
#endif

	
	return output;
}

void TLInfo::SetDisplayStartTime(unsigned long ulTime)
{
	m_ulDisplayStartTime = ulTime;
}

unsigned long TLInfo::GetDisplayStartTime()
{
	return m_ulDisplayStartTime;
}

void TLInfo::SetTrackVerticalScrollShift(long lValue)
{
	m_lTrackVerticalScrollShift = lValue;
}

long TLInfo::GetTrackVerticalScrollShift()
{
	return m_lTrackVerticalScrollShift;
}

/* ====================================================== *\
         TLPlayStatus
\* ====================================================== */

TLPlayStatus::TLPlayStatus()
{
	m_playStatus = PlayStatus::Pause;
}

TLPlayStatus::~TLPlayStatus()
{
}

void TLPlayStatus::Play()
{
	m_playStatus = PlayStatus::Run;
}

void TLPlayStatus::Stop()
{
	m_playStatus = PlayStatus::Stop;
}

void TLPlayStatus::Pause()
{
	m_playStatus = PlayStatus::Pause;
}

bool TLPlayStatus::IsRunning()
{
	return (m_playStatus == PlayStatus::Run);
}

bool TLPlayStatus::IsStop()
{
	return (m_playStatus == PlayStatus::Stop);
}

bool TLPlayStatus::IsPause()
{
	return (m_playStatus == PlayStatus::Pause);
}

/* ====================================================== *\
         TLItem
\* ====================================================== */

TLItem::TLItem()
{
	m_trackRoot = std::make_shared<TLTrack>();

	m_trackRoot->SetTrackType(TLTrackType::Undefine);
	m_trackRoot->SetId(0);
	m_trackRoot->SetName("ItemRoot");
}

TLItem::~TLItem()
{
}

TLInfo& TLItem::GetInfo()
{
	return m_info;
}

TLPlayStatus& TLItem::GetStatus()
{
	return m_status;
}

TLTrackPtr TLItem::GetRootTrack()
{
	return m_trackRoot;
}

