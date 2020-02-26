/********************************************************************\
Project Name: Timeline Prototype

File Name: TLTracks.cpp

Definition of following classes:
TLTrackEvent
TLTrack

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
#include <memory>
#include <sstream>
#include <boost/uuid/uuid_io.hpp>

// ==== Include local library ==== //
#include "TLTracks.h"

using namespace Timeline;

/* ====================================================== *\
TLTrackEvent
\* ====================================================== */

TLTrackEvent::TLTrackEvent()
{
	m_uuid = boost::uuids::to_string(boost::uuids::random_generator()());
}

TLTrackEvent::~TLTrackEvent()
{
}

std::string TLTrackEvent::UUID()
{
	return m_uuid;
}

void TLTrackEvent::SetUUID(std::string& uuid)
{
	m_uuid = uuid;
}

TLTrackEventType TLTrackEvent::EventType()
{
	return m_eventType;
}

void TLTrackEvent::SetEventType(TLTrackEventType type)
{
	m_eventType = type;
}

TimeCode& TLTrackEvent::GetTimeCode()
{
	return m_timecode;
}

void TLTrackEvent::SetTimeCode(TimeCode& timecode)
{
	m_timecode = timecode;
}

double TLTrackEvent::GetValue()
{
	return m_value;
}

void TLTrackEvent::SetValue(double value)
{
	m_value = value;
}

TLTrackPtr& TLTrackEvent::GetTrack()
{
	return m_track;
}

void TLTrackEvent::SetTrack(TLTrackPtr& track)
{
	m_track = track;
}


/* ====================================================== *\
          TLTrack
\* ====================================================== */

TLTrack::TLTrack()
{
	m_uuid = boost::uuids::to_string(boost::uuids::random_generator()());

	m_bVisible = true;
}

TLTrack::~TLTrack()
{
}

unsigned long TLTrack::Id()
{
	return m_id;
}
void TLTrack::SetId(unsigned long id)
{
	m_id = id;
}

std::string TLTrack::UUID()
{
	return m_uuid;
}

void TLTrack::SetUUID(std::string& uuid)
{
	m_uuid = uuid;
}

std::string TLTrack::Name()
{
	return m_name;
}

void TLTrack::SetName(const std::string& name)
{
	m_name = name;
}

std::string TLTrack::NickName()
{
	return m_nickname;
}

void TLTrack::SetNickName(const std::string& name)
{
	m_nickname = name;
}

TLTrackType TLTrack::TrackType()
{
	return m_trackType;
}

void TLTrack::SetTrackType(TLTrackType type)
{
	m_trackType = type;
}

TLTrackPtr& TLTrack::Parent()
{
	return m_parent;
}

bool TLTrack::HasParent()
{
	if (this->m_parent != nullptr)
	{
		return true;
	}
	
	return false;
}

void TLTrack::SetParent(TLTrackPtr& track)
{
	m_parent = track;
}

TrackVector& TLTrack::ChildTracks()
{
	return m_childTracks;
}

bool TLTrack::HasChild()
{
	return m_childTracks.size() > 0;
}

void TLTrack::AddChildTrack(TLTrackPtr& track)
{
	track->SetParent(shared_from_this());
	m_childTracks.push_back(track);
}

int TLTrack::Level()
{
	return RecursiveFindLevel(shared_from_this());
}

int TLTrack::RecursiveFindLevel(TLTrackPtr& track)
{
	int value = 0;
	if (track->HasParent())
	{
		value = RecursiveFindLevel(track->Parent()) + 1;
	}
	else
	{
		value = 1;
	}

	return value;
}

void TLTrack::RecursiveFindPath(TLTrackPtr& track, std::string& szFullPath)
{
	if (track->HasParent())
	{
		if (strcmp(szFullPath.c_str(), "") == 0)
		{
			if(track->m_nickname != "")
				szFullPath = track->m_nickname;
			else
				szFullPath = track->m_name;
		}
		else
		{
			std::string szTmp = szFullPath;
			szFullPath = track->m_name + "/" + szTmp;
		}
		RecursiveFindPath(track->Parent(), szFullPath);
	}
}

std::string TLTrack::GetFullPath()
{
	m_fullpath = "";
	RecursiveFindPath(shared_from_this(), m_fullpath);
	return m_fullpath;
}

void TLTrack::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
}

bool TLTrack::IsVisible()
{
	return m_bVisible;
}