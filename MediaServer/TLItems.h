/********************************************************************\
Project Name: Timeline Prototype

File Name: TLItem.h

Declaration of following classes:
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

#pragma once

// ==== Inlcude Microsoft MFC library ==== //
#include "stdafx.h"

// ==== Inlcude STL library ==== //
#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>            // uuid class

// ==== Include local library ==== //
#include "TLDllExport.h"
#include "TimeCode.h"
#include "TLTracks.h"

namespace Timeline
{

	/* ====================================================== *\
			 TLInfo
	\* ====================================================== */
	class TLInfo;
	using TLInfoPtr = std::shared_ptr<TLInfo>;
	class TIMELINELIB_EXPORTS TLInfo
	{
		// ==== Constructor & Destructor ==== //
	public:
		TLInfo();
		virtual ~TLInfo();

		// ==== Public member methods ==== //
	public:
		unsigned long Id();
		void SetId(unsigned long id);

		std::string UUID();
		void SetUUID(std::string& uuid);

		std::string Name();
		void SetName(std::string& name);
		TimeCode CurrentTime();
		void SetCurrentTime(TimeCode timecode);
		TimeCode TotalTime();
		void SetTotalTime(TimeCode timecode);
		unsigned long GetProgress();
		TimeCode GetTimeLeft();

		std::string GetProgressString();
		std::string GetCurrentTimeString();
		std::string GetTotalTimeString();
		std::string GetTotalTimeString2();
		std::string GetTimeLeftString();

		void SetDisplayStartTime(unsigned long ulTime);
		unsigned long GetDisplayStartTime();
		void SetTrackVerticalScrollShift(long lValue);
		long GetTrackVerticalScrollShift();

		// ==== Private member fields ==== //
	private:
		unsigned long m_id;
		std::string m_uuid;

		std::string m_name;
		TimeCode m_currentTime;
		TimeCode m_totalTime;

		unsigned long m_ulDisplayStartTime;
		long m_lTrackVerticalScrollShift;
	};
	

	/* ====================================================== *\
			 TLPlayStatus
	\* ====================================================== */
	class TLPlayStatus;
	using TLPlayStatusPtr = std::shared_ptr<TLPlayStatus>;
	class TIMELINELIB_EXPORTS TLPlayStatus
	{
	public:
		// ==== Enumeration ==== //
		enum class PlayStatus
		{
			Run, Stop, Pause
		};

		// ==== Constructor & Destructor ==== //
	public:
		TLPlayStatus();
		virtual ~TLPlayStatus();

		// ==== Public member methods ==== //
	public:
		void Play();
		void Stop();
		void Pause();
		bool IsRunning();
		bool IsStop();
		bool IsPause();

		// ==== Private member fields ==== //
	private:
		PlayStatus m_playStatus;
	};
	


	/* ====================================================== *\
			 TLItem
	\* ====================================================== */
	class TLItem;
	using TLItemPtr = std::shared_ptr<TLItem>;
	using TLItemWeakPtr = std::weak_ptr<TLItem>;
	using ItemVector = std::vector<TLItemPtr>;
	using ItemVectorIter = ItemVector::iterator;
	using ItemPair = std::pair<std::string, TLItemPtr>;
	using ItemMap = std::map<std::string, TLItemPtr>;
	using ItemMapIter = ItemMap::iterator;
	class TIMELINELIB_EXPORTS TLItem
	{
		
		// ==== Constructor & Destructor ==== //
	public:
		TLItem();
		virtual ~TLItem();

		// ==== Public member methods ==== //
	public:
		TLInfo& GetInfo();
		TLPlayStatus& GetStatus();
		TLTrackPtr GetRootTrack();

		// ==== Private member fields ==== //
	private:
		TLInfo m_info;
		TLPlayStatus m_status;
		TLTrackPtr m_trackRoot;
	};

}
