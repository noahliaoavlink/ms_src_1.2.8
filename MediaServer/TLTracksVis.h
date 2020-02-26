/********************************************************************\
Project Name: Timeline Prototype

File Name: TLTrackVis.h

Declaration of following classes:
TLTrackVis
TLTrackVisSearchResult

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
#include <boost/signals2.hpp>

// ==== Include local library ==== //
#include "TLDllExport.h"
#include "TLShape.h"
#include "TLTracks.h"
#include "TLBaseType.h"
#include "ContentProcessor.h"

//#include "SimpleMediaCtrl.h"

enum PopMenuType
{
	PMT_INSERT_SRC,
	PMT_DEL_SRC,
	PMT_DEL_POINT,
	PMT_DEL_POINT2,
	PMT_INSERT_CMD,
	PMT_DEL_CMD,
	PMT_INSERT_EFFECT_INDEX,
	PMT_DEL_EFFECT_INDEX,
	PMT_MOVE_TO_BEGINNING,
	PMT_MOVE_TO,
	PMT_CLOSE_TO_THE_FRONT,
	PMT_INSERT_EFFECT,
	PMT_DEL_EFFECT,
	PMT_EFFECT_MOVE_TO,
	PMT_CONFIG_AUDIO,
};

#define IDC_POP_INSERT_SRC 3001
#define IDC_POP_DEL_SRC 3002
#define IDC_POP_DEL_POINT 3003
#define IDC_POP_ANGLE1 3004  //-45
#define IDC_POP_ANGLE2 3005  //-90
#define IDC_POP_ANGLE3 3006  //45
#define IDC_POP_ANGLE4 3007  //90
#define IDC_POP_UNLINK 3008 //Unlink
#define IDC_POP_INSERT_CMD 3009
#define IDC_POP_DEL_CMD 3010
#define IDC_POP_INSERT_EFFECT_INDEX 3011
#define IDC_POP_DEL_EFFECT_INDEX 3012
#define IDC_POP_MOVE_TO_BEGINNING 3013
#define IDC_POP_MOVE_TO_1 3014
#define IDC_POP_MOVE_TO_2 3015
#define IDC_POP_MOVE_TO_5 3016
#define IDC_POP_MOVE_TO_10 3017
#define IDC_POP_ADD_1_SEC 3018
#define IDC_POP_ADD_2_SECS 3019
#define IDC_POP_ADD_5_SECS 3020
#define IDC_POP_ADD_10_SECS 3021
#define IDC_POP_SUB_1_SEC 3022
#define IDC_POP_SUB_2_SECS 3023
#define IDC_POP_SUB_5_SECS 3024
#define IDC_POP_SUB_10_SECS 3025
#define IDC_POP_MOVE_TO 3026
#define IDC_POP_CLOSE_TO_THE_FRONT 3027
#define IDC_POP_INSERT_EFFECT 3028
#define IDC_POP_DEL_EFFECT 3029
#define IDC_POP_EFFECT_MOVE_TO 3030
#define IDC_POP_EFFECT_CLOSE_TO_THE_FRONT 3031
#define IDC_POP_CONFIG_AUDIO 3032
#define IDC_POP_CHANG_EEFFECT_LEN 3033

namespace Timeline
{
	class TLTrackVis;
	using TLTrackVisPtr = std::shared_ptr<TLTrackVis>;
	using TLTrackVisWeakPtr = std::weak_ptr<TLTrackVis>;
	using TrackVisVector = std::vector<TLTrackVisPtr>;
	using TrackVisVectorIter = TrackVisVector::iterator;
	using TrackVisPair = std::pair<std::string, TLTrackVisPtr>;
	using TrackVisMap = std::map<std::string, TLTrackVisPtr>;
	using TrackVisMapIter = TrackVisMap::iterator;

	class TLTrackEventVis;
	using TLTrackEventVisPtr = std::shared_ptr<TLTrackEventVis>;
	using TLTrackEventVisWeakPtr = std::weak_ptr<TLTrackEventVis>;
	using TrackEventVisVector = std::vector<TLTrackEventVisPtr>;
	using TrackEventVisVectorIter = TrackEventVisVector::iterator;
	using TrackEventVisPair = std::pair<std::string, TLTrackEventVisPtr>;
	using TrackEventVisMap = std::map<std::string, TLTrackEventVisPtr>;
	using TrackEventVisMapIter = TrackEventVisMap::iterator;

	/* ====================================================== *\
	TLTrackEventVis
	\* ====================================================== */
	class TIMELINELIB_EXPORTS TLTrackEventVis : public TLShape
	{
	public:
		// ==== Constants ==== //
		class VisualConstant
		{
		public:
			const static int Width = 5;
			const static int Height = 5;
		};

		// ==== Constructor & Destructor ==== //
	public:
		TLTrackEventVis();
		virtual ~TLTrackEventVis();

		// ==== Public member methods ==== //
	public:
		void Initialize(TLTrackEventPtr& event);
		TLTrackEventPtr& GetContext();
		//Gdiplus::Point& Position();
		void Draw(Gdiplus::Graphics& graphics);

		// ==== Private member methods ==== //
	private:

		// ==== Public signal ==== //
	public:

		// ==== Private member fields ==== //
	private:
		TLTrackEventPtr m_event;
	};

	/* ====================================================== *\
			  TLTrackVis
	\* ====================================================== */
	
	class TIMELINELIB_EXPORTS TLTrackVis : public TLShape
	{
		using Signal = boost::signals2::signal<void(std::string)>;

	public:
		// ==== Constants ==== //
		class VisualConstant
		{
		public:
			const static int Width = 1000;
			const static int Height = 32;
			const static int ExpanderWidth = 200;
			const static int CollapseHeight = 32;
			const static int ExpandHeight = 101;//92;
			const static int LevelShift = 10;
		};

		// ==== Enumeration ==== //
		enum class ExpandStatus
		{
			Expand = 1, Collapse = 2
		};

		// ==== Constructor & Destructor ==== //
	public:
		TLTrackVis();
		virtual ~TLTrackVis();

		// ==== Public member methods ==== //
	public:
		void Initialize(TLTrackPtr& track);
		TLTrackPtr& GetContext();

		void SetPosition(int x, int y);
		void MouseLeftButtonDown(const Gdiplus::Point& point);
		void MouseLeftButtonUp(const Gdiplus::Point& point);
		void MouseMove(const Gdiplus::Point& point);
		void MouseRButtonUp(const Gdiplus::Point& point);
		void MouseRButtonUp2(const Gdiplus::Point& point);
		void _LBDown(const Gdiplus::Point& point);
		void _LBUp(const Gdiplus::Point& point);
		void _MouseMove(const Gdiplus::Point& point);
		void OnKeyDown(unsigned int key);

		void Expand();
		void Collapse();
		bool IsExpand();

		void KeyDownDelete();

		void DeselectAllEvents();

		void DrawExpander(Gdiplus::Graphics& graphics);
		void DrawContent(Gdiplus::Graphics& graphics);
		void Draw(Gdiplus::Graphics& graphics);
		void DrawSourcePics(Gdiplus::Graphics& graphics);
		void DrawLines(Gdiplus::Graphics& graphics);
		void DrawCommandIcon(Gdiplus::Graphics& graphics);
		void DrawEffectIndex(Gdiplus::Graphics& graphics);
		void DrawEffect(Gdiplus::Graphics& graphics);

		void SetOffset(int iValue);
		void InsertSource(const Gdiplus::Point& point);
		void DeleteSource(const Gdiplus::Point& point);
		void DeletePoint(const Gdiplus::Point& point);
		void ChangeAngle(const Gdiplus::Point& point, int iAngle);
		void Unlink(const Gdiplus::Point& point);
		void InsertCommand(const Gdiplus::Point& point);
		void DeleteCommand(const Gdiplus::Point& point);
		void InsertEffectIndex(const Gdiplus::Point& point);
		void DeleteEffectIndex(const Gdiplus::Point& point);
		void InsertEffect(const Gdiplus::Point& point);
		void DeleteEffect(const Gdiplus::Point& point);
		void ConfigAudio(const Gdiplus::Point& point);
		void PopupMenu(int iType, const Gdiplus::Point& point);

		//std::string GetFullPath();
		void ReturnFileName(wchar_t* wszFileName);
		void UpdateItemTotalTime();
		void UpdateTimeCode(Gdiplus::Graphics& graphics, TimeCode timecode);
		void UpdateTimeCode(TimeCode timecode);
		std::string GetFullPath();
		//char* PosToTimeInfo(float fPos, bool bEnableMS);
		//float TimeInfoToPos(char* szTime);
		void ChangeExpandStatus();
		void SetVisibleRecursively(TLTrackPtr& track, bool bVisible);
		void SetCheckRange(bool bEnable);
		bool CheckStatus();

		char* GetFullPath2();
		void MoveToBeginning(const Gdiplus::Point& point);
		void CloseToTheFront(const Gdiplus::Point& point);
		void MoveTo(const Gdiplus::Point& point, int iOP, int iSec);
		void PreloadSource();
		bool CheckPBStatus();
		void StopSource();
		void InsertEffectToTrack(EffectInfo* pInfo);
		void Effect_CloseToTheFront(const Gdiplus::Point& point);
		void Effect_MoveTo(const Gdiplus::Point& point, int iOP, int iSec);
		void AudioConfigOK();
		void ChangeEffectLength(const Gdiplus::Point& point);

		// ==== Private member methods ==== //
	private:
		void DrawExpanderExpand(Gdiplus::Graphics& graphics);
		void DrawContentExpand(Gdiplus::Graphics& graphics);

		void DrawExpanderCollapse(Gdiplus::Graphics& graphics);
		void DrawContentCollapse(Gdiplus::Graphics& graphics);

		void DrawEvents(Gdiplus::Graphics& graphics);
		void DrawCues(Gdiplus::Graphics& graphics);
		void DrawFocusBorder(Gdiplus::Graphics& graphics);

		void SetExpandRegion();
		void SetCollapseRegion();

		bool HasEventCollision(const Gdiplus::Point& point);
		Gdiplus::Point MapToTrackVisSpace(const Gdiplus::Point& point);

		// ==== Public signal ==== //
	public:
		Signal ExpanderClick;

		// ==== Private member fields ==== //
	private:
		ExpandStatus m_expandStatus;
		TLTrackPtr m_track;

		Gdiplus::Rect m_expanderRegion;
		Gdiplus::Rect m_contentRegion;

		TrackEventVisVector m_events;

//		TrackPosInfo m_track_pos_info;
		int m_iOffset;
		char m_szTime[64];
		int m_iActiveIndex;
		int m_iSelectIndex;
		bool m_bDragged;
		bool m_bResized;
		float m_fNodeTimeOffset;

		std::string m_TmpFullPath;
		HCURSOR m_hArrow;
		HCURSOR m_hResize_H;
		HCURSOR m_hMove;

		bool m_bUpdate;
		bool m_bIsPressed;

		int m_iClickX;
		int m_iClickY;

		ContentProcessor m_ContentProcessor;

		char m_szTrackFullPath[512];
		char m_szTmpFullPath[512];
	};


	/* ====================================================== *\
	TLTrackVisSearchResult
	\* ====================================================== */
	struct TIMELINELIB_EXPORTS TLTrackVisSearchResult
	{
		// ==== Public member fields ==== //
	public:
		bool HasResult = false;
		TLTrackVisPtr Context = nullptr;
	};

}


//TLTrackSearchResult
