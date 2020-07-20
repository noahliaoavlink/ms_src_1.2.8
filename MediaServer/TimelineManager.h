/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineManager.h

Declaration of following classes:
TimelineManager

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
#include <queue>

// ==== Include local library ==== //
#include "TLDllExport.h"
#include "TLConstant.h"
#include "TLBaseType.h"
#include "TLItems.h"
#include "TLItemsVis.h"
#include "TLTracks.h"
#include "TLTracksVis.h"
#include "TimeCode.h"
#include "tinyxml2.h"

#include "TrackContentManager.h"
#include "TL_Out_Device.h"
#include "DisplayManager.h"
#include "TCAdapter.h"
#include "LogFileDll.h"

using namespace tinyxml2;

namespace Timeline
{

	/* ====================================================== *\
	TimelineManager
	\* ====================================================== */

	class TIMELINELIB_EXPORTS TimelineManager
	{
		using ItemToTrackMap = std::multimap<std::string, std::string>;
		using ItemToTrackPair = std::pair<std::string, std::string>;
		using StatusSignal = boost::signals2::signal<void(std::string)>;
		using BaseTimeCodeSignal = boost::signals2::signal<void(TimeCode)>;

		// ==== Constants ==== //
	public:
		const static int PixelToMsFactor = 10;
		
		// ==== Constructor & Destructor ==== //
	public:
		TimelineManager();
		virtual ~TimelineManager();

		// ==== Public member methods ==== //
	public:
		void AddItem();
		void DeleteItem();
		bool ItemClick(const Gdiplus::Point& point);
		void LButtonUp(const Gdiplus::Point& point);
		void OnMouseMove(const Gdiplus::Point& point);
		bool ItemDbClick(const Gdiplus::Point& point);
		bool TrackClick(const Gdiplus::Point& point);

		void LoadProject(const std::string& filename);
		void LoadProject(const std::string& filename, std::string szName, int iID);
		void SaveProject(const std::string& filename);

		TLItemPtr GetItem(const std::string& uuid);
		ItemVector GetItems();
		TLItemVisPtr GetItemVisual(const std::string& uuid);
		ItemVisVector GetItemVisuals();


		TLTrackPtr GetTrack(const std::string& uuid);
		TrackVector GetTracks();
		TrackVector GetTracks(const std::string& uuid);
		TLTrackVisPtr GetTrackVisual(const std::string& uuid);
		TrackVisVector GetTrackVisuals();
		TLTrackVisPtr GetCommandTrackVisual();
		TrackVisVector GetTrackVisualsWithoutRootAndCommand();
		TrackVisVector GetTrackVisualsOfCurrentItem();

		TLTrackVisSearchResult GetHitTrack(const Gdiplus::Point& point);

		std::string& GetSelectedItem(const std::string& uuid);

		void SetBaseTimeCode(TimeCode& timecode);
		TimeCode& GetBaseTimeCode();
		TimeCode XToTimeCode(int x);
		int TimeCodeToX(TimeCode timecode);

		void SetPointerMode(PointerMode mode);
		PointerMode GetPointerMode();
		void CreateSubNode(tinyxml2::XMLDocument* pDoc, XMLElement* pParent, std::shared_ptr<Timeline::TLTrack> pTrack, TrackVector& child_tracks);
		TimeCode GetTotalTimeOfCurrentItem();
		void UpdateTimeCode(TimeCode timecode);
		void SetItemViewWnd(CWnd* pWnd);
		void SetEditViewWnd(CWnd* pWnd);
		CWnd* GetEditViewWnd();
		CWnd* GetItemViewWnd();
		void SetMediaServerDlgWnd(CWnd* pWnd);
		CWnd* GetMediaServerDlgWnd();
		TrackContentManager* GetTrackContentManager();
		void SetLineMode(LineMode mode);
		LineMode GetLineMode();
		void SetUnitMode(UnitMode mode);
		UnitMode GetUnitMode();
		void SetTotalTimeOfCurrentItem(TimeCode timecode);
		void ChangeTo(char* szName);
		void ChangeTo(int iIndex);
		void Back();
		void Next();
		void UpdateTotalTime();
		//void ChangeExpandStatus(TLTrackPtr pTrack, int iStatus);
//		void SetPreviewWnd(HWND hWnd);
//		HWND GetPreviewWnd();
		void SetOutDevice(int iType, OutDevice* pObj);
		OutDevice* GetOutDevice(int iType);
		void DoPlayback_AllOutDevices(int iCmd);
		void SetVerticalScrollShift(int iValue);
		int GetSelItemIndex();
		void UpdateVideoEffectParamItem(int iIndex);
		bool CheckItemViewStatus();
		DisplayManager* GetDisplayManager();
		TCAdapter* GetTCAdapter();
		int GetPBStatus();
		void ClearAll();
		bool CheckItemName(char* szName);
		void SetLogFileDll(LogFileDll* pObj);
		void FreeObj();
		void UpdateAudioStatus();
		int GetTotalOfItems();
		void ResetItemStatus();
		void ResetEffectStatus();
		void FreezeShape(bool bFreeze);

		// ==== Private member methods ==== //
	private:
		void LoadXmlRecursively(tinyxml2::XMLElement* first, TLItemPtr& item, TLTrackPtr& parent, TrackMap& map);
		void CreateTrackVisualsRecursively(TLItemPtr& item, TLTrackPtr& track);

		// ==== Public signal ==== //
	public:
		StatusSignal StatusChanged;
		BaseTimeCodeSignal BaseTimeCodeChanged;

		// ==== Private member fields ==== //
	private:
		
		TrackContentManager* m_pTrackContentManager;

		ItemMap m_items;
		ItemVisMap m_itemVisuals;

		TrackMap m_tracks;
		TrackVisMap m_trackVisuals;

		int m_itemIdCount;
		int m_trackIdCount;

		std::string m_selectedItemUuid;
		std::string m_LastItemUuid;
		std::string m_selectedTrackUuid;

		ItemToTrackMap m_itemToTrackMap;

		PointerMode m_pointerMode;

		CWnd* m_pItemViewWnd;
		CWnd* m_pEditViewWnd;
		CWnd* m_pMediaServerDlgWnd;
		
//		HWND m_hPreviewWnd;
		TL_Out_Device* m_pTL_Out_Device;
		DisplayManager* m_pDisplayManager;

		TCAdapter* m_pTCAdapter;
		LogFileDll* m_pLogFileDll;

		LineMode m_LineMode;
		UnitMode m_UnitMode;

		TimeCode m_baseTimeCode;
		TimeCode m_timecode;
	};

}
