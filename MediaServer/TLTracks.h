/********************************************************************\
Project Name: Timeline Prototype

File Name: TLTracks.h

Declaration of following classes:
TLTrackEventType
TLTrackEvent
TLTrackType
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

#pragma once

// ==== Inlcude Microsoft MFC library ==== //
#include "stdafx.h"

// ==== Inlcude STL library ==== //
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

// ==== Include local library ==== //
#include "TLDllExport.h"
#include "TimeCode.h"

namespace Timeline
{
	class TLTrack;
	using TLTrackPtr = std::shared_ptr<TLTrack>;
	using TLTrackWeakPtr = std::weak_ptr<TLTrack>;
	using TrackVector = std::vector<TLTrackPtr>;
	using TrackVectorIter = TrackVector::iterator;
	using TrackPair = std::pair<std::string, TLTrackPtr>;
	using TrackMap = std::map<std::string, TLTrackPtr>;
	using TrackMapIter = TrackMap::iterator;

	class TLTrackEvent;
	using TLTrackEventPtr = std::shared_ptr<TLTrackEvent>;
	using TLTrackEventWeakPtr = std::weak_ptr<TLTrackEvent>;
	using TrackEventVector = std::vector<TLTrackEventPtr>;
	using TrackEventVectorIter = TrackEventVector::iterator;
	using TrackEventPair = std::pair<std::string, TLTrackEventPtr>;
	using TrackEventMap = std::map<std::string, TLTrackEventPtr>;
	using TrackEventMapIter = TrackEventMap::iterator;

	/* ====================================================== *\
	TLTrackEventType
	\* ====================================================== */
	enum class TIMELINELIB_EXPORTS TLTrackEventType
	{
		Undefine = 0,
		Float = 1,
		Bool = 2,
		Int = 3,
		String = 4,
		Media = 5,
		Command = 6,
		Effect_Index = 7,
		Effect = 8,
		AudioConfig = 9,
		Ref
	};

	/* ====================================================== *\
	TLTrackEvent
	\* ====================================================== */
	class TIMELINELIB_EXPORTS TLTrackEvent : public std::enable_shared_from_this<TLTrackEvent>
	{
		// ==== Constructor & Destructor ==== //
	public:
		TLTrackEvent();
		virtual ~TLTrackEvent();

		// ==== Public member methods ==== //
	public:
		std::string UUID();
		void SetUUID(std::string& uuid);

		TLTrackEventType EventType();
		void SetEventType(TLTrackEventType type);

		TimeCode& GetTimeCode();
		void SetTimeCode(TimeCode& timecode);

		double GetValue();
		void SetValue(double value);

		TLTrackPtr& GetTrack();
		void SetTrack(TLTrackPtr& track);

		// ==== Private member fields ==== //
	private:

		// ==== Private member fields ==== //
	private:
		std::string m_uuid;
		TimeCode m_timecode;
		double m_value;
		TLTrackEventType m_eventType;
		TLTrackPtr m_track;
	};

	/* ====================================================== *\
			  TLTrackType
	\* ====================================================== */
	enum class TIMELINELIB_EXPORTS TLTrackType
	{
		Undefine = 0,
		Float = 1,
		Bool = 2,
		Int = 3,
		String = 4,
		Media = 5,
		Command = 6,
		Effect_Index = 7,
		Effect = 8,
		AudioConfig = 9,
	};

	/* ====================================================== *\
			  TLTrack
	\* ====================================================== */
	class TIMELINELIB_EXPORTS TLTrack : public std::enable_shared_from_this<TLTrack>
	{
		// ==== Constructor & Destructor ==== //
	public:
		TLTrack();
		virtual ~TLTrack();

		// ==== Public member methods ==== //
	public:
		unsigned long Id();
		void SetId(unsigned long id);

		std::string UUID();
		void SetUUID(std::string& uuid);

		std::string Name();
		void SetName(const std::string& name);

		std::string NickName();
		void SetNickName(const std::string& name);

		TLTrackType TrackType();
		void SetTrackType(TLTrackType type);

		TLTrackPtr& Parent();
		bool HasParent();
		void SetParent(TLTrackPtr& track);

		TrackVector& ChildTracks();
		bool HasChild();
		void AddChildTrack(TLTrackPtr& track);

		int Level();

		void RecursiveFindPath(TLTrackPtr& track, std::string& szFullPath);
		std::string GetFullPath();
		void SetVisible(bool bVisible);
		bool IsVisible();

		// ==== Public member fields ==== //
	private:
		int RecursiveFindLevel(TLTrackPtr& track);

		// ==== Private member fields ==== //
	private:
		unsigned long m_id;
		std::string m_uuid;

		std::string m_name;
		std::string m_nickname;

		TLTrackType m_trackType;
		TLTrackPtr m_parent;
		TrackVector m_childTracks;

		std::string m_fullpath;
		bool m_bVisible;
	};
	

}