/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineEditFacade.h

Declaration of following classes:
TimelineEditFacade

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
// ==== Inlcude STL library ==== //
#include <memory>
#include <vector>

// ==== Include local library ==== //
#include "TLDllExport.h"
#include "TimelineManager.h"
#include "TimelineEditViewButtonPanel.h"
#include "TimeCodeBar.h"
#include "ThreadBase.h"
#include <queue>          // std::queue
#include "../../../Include/SQQueue.h"

/*
#define UPDATE_TRACK_CONTENT 1
#define UPDATE_BUTTON_PANEL 2
#define UPDATE_CURTIMECODE 3
#define UPDATE_TIME_CODE_BAR 4
#define UPDATE_TRACK_CONTENT2 5
*/

enum TLEditFacadeEvent
{
	UPDATE_TRACK_CONTENT = 1,
	UPDATE_BUTTON_PANEL,
	UPDATE_CURTIMECODE,
	UPDATE_TIME_CODE_BAR,
	UPDATE_TRACK_CONTENT2,
	EFE_INSERT_EFFECT,
	AUDIO_CONFIG_OK,
};

typedef struct
{
	RECT track_content_rect;
	RECT cur_time_code_rect;
}TrackContentEvent;

namespace Timeline
{
	/* ====================================================== *\
	TimelineEditFacade
	\* ====================================================== */

	class TIMELINELIB_EXPORTS TimelineEditFacade : public ThreadBase
	{
		// ==== Constructor & Destructor ==== //
	public:
		TimelineEditFacade();
		virtual ~TimelineEditFacade();

		//========== Public member methods ========== //
	public:
		void OnInitialize(CWnd* wnd);
		void OnTerminate();
		void Draw(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect);
		void DrawButtonPanel(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect);
		void DrawCurTimecode(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect);
		void DrawTimecodeBar(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect);
		void DrawTrackExpander(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect);
		void DrawTrackContent(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect);
		void OnCreate(const Gdiplus::Rect& rect);
		void OnDestroy();
		void OnSize(const Gdiplus::Rect& rect);
		void OnSizing(const Gdiplus::Rect& rect);

		void OnLButtonDown(const Gdiplus::Point& point);
		void OnLButtonUp(const Gdiplus::Point& point);
		void OnMouseMove(const Gdiplus::Point& point);
		void OnMouseWheel(short zDelta, const Gdiplus::Point& point);
		void OnRButtonUp(const Gdiplus::Point& point);
		void OnKeyDown(unsigned int key);
		
		void Start(std::string msg);
		void Start2(std::string msg);
		void Pause(std::string msg);
		void Stop(std::string msg);
		void SeekToBegin();
		void RetunFileName();

		void Back(std::string msg);
		void Next(std::string msg);

		void SeekTo(int iSec);
		void UpdateHand(bool bUpdate);
		void SetTimeBarOffset(long lOffset);
		void SetVerticalScrollShift(int iValue);
		void ProcessTimeCode();

		void UpdateTrackContent();
		void UpdateTrackContent2();
		void UpdateCurTimeCode();
		void UpdateButtonPanel();
		void UpdateTimeCodeBar();
		void Redraw();
		void PreloadSource();
		void StopSource();

		void RemoveTimeCodeQueue();
		void AddTimeCodeToQueue(TimeCode timecode);
		TimeCode* GetTimeCodeFromeQueue();
		void RemoveTimeCodeItem();
		void UpdateTimeCode1(TimeCode timecode);
		void UpdateTimeCode2(TimeCode timecode);
		void UpdateTimeCode3(TimeCode timecode);
		virtual void ThreadEvent();
		void InsertEffect(EffectInfo* pInfo);
		void AudioConfigOK();
		//========== Private member methods ========== //
	private:
		void DrawTimeCodeSpaceTransform(Gdiplus::Graphics& graphics, const std::string& text, const Gdiplus::Point& point);
		void DrawCurrentTimeCode(Gdiplus::Graphics& graphics, const std::string& text, const Gdiplus::Point& point);
		
		void PointerModeChanged(PointerMode mode);
		void LineModeChanged(LineMode mode);
		void UnitModeChanged(UnitMode mode);
		void UpdateTimeCode(TimeCode timecode);
		void BaseTimeCodeChanged(TimeCode timecode);
		void StatusChanged(std::string msg);
		void AdjustLayout(const Gdiplus::Rect& rect);

		//========== Private member fields ========== //
	private:
		CClientDC *m_pDC;
		CWnd* m_wnd;

		Gdiplus::Point m_mousePosition;
		Gdiplus::Point m_OldmousePosition;
		Gdiplus::Rect m_clientRect;

		//Rect m_CmdTrackContentRect;
		CRect m_TrackContentRect;
		CRect m_ButtonPanelRect;
		CRect m_CurTimeCodeRect;
		CRect m_TimeCodeBarRect;

		int m_iOldTimeCount;
		int m_iTimeCount;

		int m_vertical_scroll_shift = 0;

		TimelineEditViewButtonPanel m_buttonPanel;

		TimeCodeBar m_timecodeBar;
		TimeCode m_timecode;
		bool m_bUpdateHand;
		int m_iOldSec;
		bool m_bDoRedraw;
		int m_iRedrawCount;

		TimelineManager* m_manager;

		std::queue<TimeCode> m_TimeCodeQueue;
		TimeCode* m_pTmpTimeCode;
		SQQueue* m_pTimeCodeQueue;

		unsigned long m_ulLastTimeCode;

	};
}
