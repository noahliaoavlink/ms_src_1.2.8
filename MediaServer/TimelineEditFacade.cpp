/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineEditFacade.cpp

Definition of following classes:
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

// ==== Inlcude Microsoft MFC library ==== //
#include "stdafx.h"

// ==== Inlcude STL library ==== //
#include <vector>
#include <memory>
#include <string>
#include <boost/format.hpp>

// ==== Include local library ==== //
#include "TLUtility.h"
#include "TLConstant.h"
#include "TimelineEditFacade.h"
#include "DoubleBuffer.h"
#include "ServiceProvider.h"

#include "Mmsystem.h"

#pragma comment(lib, "winmm.lib")

#define ENGINNERING_MODE

using namespace Timeline;

/* ====================================================== *\
TimelineEditFacade
\* ====================================================== */
TimelineEditFacade::TimelineEditFacade()
{
	m_bUpdateHand = false;

	m_iOldTimeCount = 0;
	m_iTimeCount = 0;

	m_OldmousePosition.X = -1;
	m_OldmousePosition.Y = -1;
	m_bDoRedraw = false;
	m_iRedrawCount = 0;

	m_pTimeCodeQueue = new SQQueue;
	m_pTimeCodeQueue->Alloc(sizeof(TimeCode),100);
	//m_pTimeCodeQueue->Add((unsigned char*)&timecode, sizeof(TimeCode));
}

TimelineEditFacade::~TimelineEditFacade()
{
	RemoveTimeCodeQueue();
	if (m_pTimeCodeQueue)
		delete m_pTimeCodeQueue;
}

void TimelineEditFacade::OnInitialize(CWnd* wnd)
{
	m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	m_wnd = wnd;
	m_pDC = nullptr;

	m_manager->SetEditViewWnd(wnd);

	m_buttonPanel.StartClick.connect(boost::bind(&TimelineEditFacade::Start, this, _1));
	m_buttonPanel.PauseClick.connect(boost::bind(&TimelineEditFacade::Pause, this, _1));
	m_buttonPanel.StopClick.connect(boost::bind(&TimelineEditFacade::Stop, this, _1));

	m_buttonPanel.BackClick.connect(boost::bind(&TimelineEditFacade::Back, this, _1));
	m_buttonPanel.NextClick.connect(boost::bind(&TimelineEditFacade::Next, this, _1));


	m_buttonPanel.PointerModeChanged.connect(boost::bind(&TimelineEditFacade::PointerModeChanged, this, _1));
	m_buttonPanel.LineModeChanged.connect(boost::bind(&TimelineEditFacade::LineModeChanged, this, _1));
	m_buttonPanel.UnitModeChanged.connect(boost::bind(&TimelineEditFacade::UnitModeChanged, this, _1));

	m_manager->StatusChanged.connect(boost::bind(&TimelineEditFacade::StatusChanged, this, _1));
	m_manager->BaseTimeCodeChanged.connect(boost::bind(&TimelineEditFacade::BaseTimeCodeChanged, this, _1));

	ThreadBase::Start();
}

void TimelineEditFacade::OnTerminate()
{
	ThreadBase::Stop();

	m_buttonPanel.StartClick.disconnect(boost::bind(&TimelineEditFacade::Start, this, _1));
	m_buttonPanel.PauseClick.disconnect(boost::bind(&TimelineEditFacade::Pause, this, _1));
	m_buttonPanel.StopClick.disconnect(boost::bind(&TimelineEditFacade::Stop, this, _1));

	m_buttonPanel.BackClick.disconnect(boost::bind(&TimelineEditFacade::Back, this, _1));
	m_buttonPanel.NextClick.disconnect(boost::bind(&TimelineEditFacade::Next, this, _1));

	m_buttonPanel.PointerModeChanged.disconnect(boost::bind(&TimelineEditFacade::PointerModeChanged, this, _1));
	m_buttonPanel.LineModeChanged.disconnect(boost::bind(&TimelineEditFacade::LineModeChanged, this, _1));
	m_buttonPanel.UnitModeChanged.disconnect(boost::bind(&TimelineEditFacade::UnitModeChanged, this, _1));

	m_manager->StatusChanged.disconnect(boost::bind(&TimelineEditFacade::StatusChanged, this, _1));
	m_manager->BaseTimeCodeChanged.disconnect(boost::bind(&TimelineEditFacade::BaseTimeCodeChanged, this, _1));
}

void TimelineEditFacade::Draw(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect)
{
	//--------------------------------------------------------//
	//                    Region1                             //
	//--------------------------------------------------------//
	// Region2 |           Region3                            //
	//--------------------------------------------------------//
	// Region4 |               Region5                        //
	//--------------------------------------------------------//
	//         |                                              //
	//         |                                              //
	// Region6 |               Region7                        //
	//         |                                              //
	//         |                                              //
	//--------------------------------------------------------//
	
	// Region1: button panel
	// Region2: timecode space transform information
	// Region3: timecode bar
	// Region4: command track's expander
	// Region5: command track's contents
	// Region6: other track's expander
	// Region7: other track's contents
	// Region8: tab page index (optional)

#if 0
	// =========== Draw button panel (Region1) ==================== //
	auto state1 = graphics.Save();
	Region region1(Rect(
		0,
		0,
		rect.Width,
		TimelineEditViewButtonPanel::VisualConstant::Height));
	graphics.SetClip(&region1, CombineModeReplace);
	m_buttonPanel.Draw(graphics);
	graphics.Restore(state1);

	// =========== Draw timecode space transform information (Region2) ==================== //
	auto state2 = graphics.Save();
	Region region2(Rect(
		0,
		TimelineEditViewButtonPanel::VisualConstant::Height,
		TLTrackVis::VisualConstant::ExpanderWidth,
		TimeCodeBar::VisualConstant::Height));
	graphics.SetClip(&region2, CombineModeReplace);
	this->DrawTimeCodeSpaceTransform(
		graphics,
		m_manager->XToTimeCode(m_mousePosition.X).ToString(),
		Gdiplus::Point(0, TimelineEditViewButtonPanel::VisualConstant::Height));
	this->DrawCurrentTimeCode(
		graphics,
		this->m_timecode.ToString(),
		Gdiplus::Point(0, TimelineEditViewButtonPanel::VisualConstant::Height));
	graphics.Restore(state2);

	// =========== Draw timecode bar (Region3) ==================== //
	auto state3 = graphics.Save();
	Region region3(Rect(
		TLTrackVis::VisualConstant::ExpanderWidth,
		TimelineEditViewButtonPanel::VisualConstant::Height,
		rect.Width - TLTrackVis::VisualConstant::ExpanderWidth,
		TimeCodeBar::VisualConstant::Height));
	graphics.SetClip(&region3, CombineModeReplace);
	m_timecodeBar.Draw(graphics);
	graphics.Restore(state3);
	

	// =========== Prepare the geometric information of the track ==================== //
	int y_shift_base = TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + 2;
	int y_shift = y_shift_base;

	auto visuals = m_manager->GetTrackVisualsOfCurrentItem();
	auto commandTrack = m_manager->GetCommandTrackVisual();
	auto cur_timecode = m_manager->GetTotalTimeOfCurrentItem();

	if (cur_timecode != 0)
	{
		auto service = ServiceProvider::Instance()->GetTimelineService();
		if (service != nullptr)
		{
			auto engine = service->GetTimeCodeEngine();
			engine->SetTotalTimecode(cur_timecode);
		}
	}

	if (!commandTrack)
	{
		return;
	}

	if (visuals.size() > 0)
	{
		if (commandTrack != nullptr)
		{
			commandTrack->SetPosition(0, y_shift);
			commandTrack->SetWidth(rect.Width);
			commandTrack->SetHeight(TLTrackVis::VisualConstant::CollapseHeight);
			y_shift += commandTrack->Height() + 2;
		}
	}

	y_shift += m_vertical_scroll_shift;

	if (visuals.size() > 0)
	{
		for (auto o : visuals)
		{
			if (o->GetContext()->IsVisible())
			{
				//
				o->SetPosition(0, y_shift);
				o->SetWidth(rect.Width);
				if (!o->GetContext()->HasChild())
				{
					o->SetHeight(TLTrackVis::VisualConstant::ExpandHeight);
				}
				else
				{
					o->SetHeight(TLTrackVis::VisualConstant::CollapseHeight);
				}
				y_shift += o->Height() + 2;
			}
		}
	}

	// =========== Draw command track's expander (Region4) ==================== //
	auto state4 = graphics.Save();
	Region region4(Rect(
		0,
		TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + 2,
		rect.Width,
		TLTrackVis::VisualConstant::Height));
	graphics.SetClip(&region4, CombineModeReplace);
	commandTrack->Draw(graphics);
	graphics.Restore(state4);

	// =========== Draw command track's contents (Region5) ==================== //

	auto state5 = graphics.Save();
	Region region5(Rect(
		TLTrackVis::VisualConstant::ExpanderWidth,
		TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + 2,
		rect.Width - TLTrackVis::VisualConstant::ExpanderWidth,
		TLTrackVis::VisualConstant::Height));
	graphics.SetClip(&region5, CombineModeReplace);
	commandTrack->DrawContent(graphics);

	if(m_iOldTimeCount != m_iTimeCount)
		commandTrack->UpdateTimeCode(graphics, this->m_timecode);
	graphics.Restore(state5);


	// =========== Draw other track's expander (Region6) ==================== //
	auto state6 = graphics.Save();
	Region region6(Rect(
		0,
		TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + TLTrackVis::VisualConstant::Height + 4,
		TLTrackVis::VisualConstant::ExpanderWidth,
		rect.Height - (TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + TLTrackVis::VisualConstant::Height + 2)));
	graphics.SetClip(&region6, CombineModeReplace);
	if (visuals.size() > 0)
	{
		for (auto o : visuals)
		{
			if (o->GetContext()->IsVisible())
				o->DrawExpander(graphics);
		}
	}
	graphics.Restore(state6);

	// =========== Draw other track's contents (Region7) ==================== //
	auto state7 = graphics.Save();
	Region region7(Rect(
		TLTrackVis::VisualConstant::ExpanderWidth,
		TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + TLTrackVis::VisualConstant::Height + 4,
		rect.Width - TLTrackVis::VisualConstant::ExpanderWidth,
		rect.Height - (TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + TLTrackVis::VisualConstant::Height + 2)));
	graphics.SetClip(&region7, CombineModeReplace);
	if (visuals.size() > 0)
	{
		for (auto o : visuals)
		{
			if (o->GetContext()->IsVisible())
				o->DrawContent(graphics);
			if (m_iOldTimeCount != m_iTimeCount)
				o->UpdateTimeCode(graphics, this->m_timecode);
			if (m_bUpdateHand == true)
				o->SetCheckRange(true);
		}
	}
	graphics.Restore(state7);

	// =========== Draw timeline playhead ==================== //
	
	auto service = ServiceProvider::Instance()->GetTimelineService();
	if (service != nullptr)
	{
		auto engine = service->GetTimeCodeEngine();
		engine->SetTotalTimecode(cur_timecode);

		if (engine->IsAlive())
		{
			if (m_timecode.TotalMilliseconds() < 10000)
				SeekTo(0);
			else
			{
				int iSec = m_timecode.TotalMilliseconds() / 1000;
				if (iSec % 10 == 0)
					SeekTo(iSec);

				if (m_bUpdateHand == true)
				{
					m_bUpdateHand = false;
					SeekTo(iSec);
				}
			}
		}
	}

	int iOffset = m_timecodeBar.GetOffset();

	Pen playheadPen(Color::LimeGreen, 2);
	double adjustFactor = 0.1;

	int iPos = m_timecode.TotalMilliseconds() * adjustFactor + 200 + iOffset;
	if (iPos >= 200)
	{
		graphics.DrawLine(&playheadPen,
			iPos,//m_timecode.TotalMilliseconds() * adjustFactor + 200 + iOffset,
			TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height,
			iPos,//m_timecode.TotalMilliseconds() * adjustFactor + 200 + iOffset,
			1000);
	}

	if (m_iOldTimeCount != m_iTimeCount)
		m_iOldTimeCount = m_iTimeCount;
#else
	DrawButtonPanel(graphics, rect);
	DrawCurTimecode(graphics, rect);
	DrawTimecodeBar(graphics, rect);
	DrawTrackExpander(graphics, rect);
	DrawTrackContent(graphics,rect);
#endif
}

void TimelineEditFacade::DrawButtonPanel(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect)
{
	// =========== Draw button panel (Region1) ==================== //
//	auto state1 = graphics.Save();

	m_ButtonPanelRect = CRect(0,0,rect.Width,TimelineEditViewButtonPanel::VisualConstant::Height);

	Region region1(Rect(
		0,
		0,
		rect.Width,
		TimelineEditViewButtonPanel::VisualConstant::Height));
	graphics.SetClip(&region1, CombineModeReplace);
	m_buttonPanel.Draw(graphics);
//	graphics.Restore(state1);
}

void TimelineEditFacade::DrawCurTimecode(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect)
{
	// =========== Draw timecode space transform information (Region2) ==================== //
//	auto state2 = graphics.Save();

	m_CurTimeCodeRect = CRect(
		0,
		TimelineEditViewButtonPanel::VisualConstant::Height,
		TLTrackVis::VisualConstant::ExpanderWidth,
		TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height);

	Region region2(Rect(
		0,
		TimelineEditViewButtonPanel::VisualConstant::Height,
		TLTrackVis::VisualConstant::ExpanderWidth,
		TimeCodeBar::VisualConstant::Height));
	graphics.SetClip(&region2, CombineModeReplace);
	this->DrawTimeCodeSpaceTransform(
		graphics,
		m_manager->XToTimeCode(m_mousePosition.X).ToString(),
		Gdiplus::Point(0, TimelineEditViewButtonPanel::VisualConstant::Height));
	this->DrawCurrentTimeCode(
		graphics,
		this->m_timecode.ToString(),
		Gdiplus::Point(0, TimelineEditViewButtonPanel::VisualConstant::Height));
//	graphics.Restore(state2);
}

void TimelineEditFacade::DrawTimecodeBar(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect)
{
	// =========== Draw timecode bar (Region3) ==================== //
//	auto state3 = graphics.Save();

	m_TimeCodeBarRect = CRect(
		TLTrackVis::VisualConstant::ExpanderWidth,
		TimelineEditViewButtonPanel::VisualConstant::Height,
		TLTrackVis::VisualConstant::ExpanderWidth + rect.Width - TLTrackVis::VisualConstant::ExpanderWidth,
		TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height);

	Region region3(Rect(
		TLTrackVis::VisualConstant::ExpanderWidth,
		TimelineEditViewButtonPanel::VisualConstant::Height,
		rect.Width - TLTrackVis::VisualConstant::ExpanderWidth,
		TimeCodeBar::VisualConstant::Height));
	graphics.SetClip(&region3, CombineModeReplace);
	m_timecodeBar.Draw(graphics);
//	graphics.Restore(state3);
}

void TimelineEditFacade::DrawTrackExpander(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect)
{
	auto commandTrack = m_manager->GetCommandTrackVisual();
	auto visuals = m_manager->GetTrackVisualsOfCurrentItem();
	auto cur_timecode = m_manager->GetTotalTimeOfCurrentItem();

	// =========== Prepare the geometric information of the track ==================== //
	int y_shift_base = TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + 2;
	int y_shift = y_shift_base;

	if (cur_timecode != 0)
	{
		auto service = ServiceProvider::Instance()->GetTimelineService();
		if (service != nullptr)
		{
			auto engine = service->GetTimeCodeEngine();
			engine->SetTotalTimecode(cur_timecode);
		}
	}

	if (!commandTrack)
	{
		return;
	}

	if (visuals.size() > 0)
	{
		if (commandTrack != nullptr)
		{
			commandTrack->SetPosition(0, y_shift);
			commandTrack->SetWidth(rect.Width);
			commandTrack->SetHeight(TLTrackVis::VisualConstant::CollapseHeight);
			y_shift += commandTrack->Height() + 2;
		}
	}

	y_shift += m_vertical_scroll_shift;

	if (visuals.size() > 0)
	{
		for (auto o : visuals)
		{
			if (o->GetContext()->IsVisible())
			{
				//
				o->SetPosition(0, y_shift);
				o->SetWidth(rect.Width);
				if (!o->GetContext()->HasChild())
				{
					o->SetHeight(TLTrackVis::VisualConstant::ExpandHeight);
				}
				else
				{
					o->SetHeight(TLTrackVis::VisualConstant::CollapseHeight);
				}
				y_shift += o->Height() + 2;
			}
		}
	}

	// =========== Draw command track's expander (Region4) ==================== //
//	auto state4 = graphics.Save();
	Region region4(Rect(
		0,
		TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + 2,
		rect.Width,
		TLTrackVis::VisualConstant::Height));
	graphics.SetClip(&region4, CombineModeReplace);
	commandTrack->Draw(graphics);
//	graphics.Restore(state4);

	// =========== Draw other track's expander (Region6) ==================== //
//	auto state6 = graphics.Save();
	Region region6(Rect(
		0,
		TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + TLTrackVis::VisualConstant::Height + 4,
		TLTrackVis::VisualConstant::ExpanderWidth,
		rect.Height - (TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + TLTrackVis::VisualConstant::Height + 2)));
	graphics.SetClip(&region6, CombineModeReplace);
	if (visuals.size() > 0)
	{
		for (auto o : visuals)
		{
			if (o->GetContext()->IsVisible())
				o->DrawExpander(graphics);
		}
	}
//	graphics.Restore(state6);
}

void TimelineEditFacade::DrawTrackContent(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect)
{
	auto visuals = m_manager->GetTrackVisualsOfCurrentItem();
	auto commandTrack = m_manager->GetCommandTrackVisual();
	auto cur_timecode = m_manager->GetTotalTimeOfCurrentItem();

	if (!commandTrack)
	{
		return;
	}

	// =========== Draw command track's contents (Region5) ==================== //
	Region region_track_content(Rect(
		TLTrackVis::VisualConstant::ExpanderWidth,
		TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + 2,
		rect.Width - TLTrackVis::VisualConstant::ExpanderWidth,
		rect.Height - (TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height)));

	graphics.SetClip(&region_track_content, CombineModeReplace);

	auto state5 = graphics.Save();
/*
	m_CmdTrackContentRect = Rect(
		TLTrackVis::VisualConstant::ExpanderWidth,
		TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + 2,
		rect.Width - TLTrackVis::VisualConstant::ExpanderWidth,
		TLTrackVis::VisualConstant::Height);
	*/	
	Region region5(Rect(
		TLTrackVis::VisualConstant::ExpanderWidth,
		TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + 2,
		rect.Width - TLTrackVis::VisualConstant::ExpanderWidth,
		TLTrackVis::VisualConstant::Height));
	graphics.SetClip(&region5, CombineModeReplace);
	commandTrack->DrawContent(graphics);
//	if (m_iOldTimeCount != m_iTimeCount)
	//	commandTrack->UpdateTimeCode(graphics, this->m_timecode);
	graphics.Restore(state5);

	// =========== Draw other track's contents (Region7) ==================== //

	m_TrackContentRect = CRect(
		TLTrackVis::VisualConstant::ExpanderWidth,
		TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height,
		rect.Width,
		rect.Height);

	auto state7 = graphics.Save();
	Region region7(Rect(
		TLTrackVis::VisualConstant::ExpanderWidth,
		TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + TLTrackVis::VisualConstant::Height + 4,
		rect.Width - TLTrackVis::VisualConstant::ExpanderWidth,
		rect.Height - (TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height + TLTrackVis::VisualConstant::Height + 2)));
	graphics.SetClip(&region7, CombineModeReplace);
	if (visuals.size() > 0)
	{
		for (auto o : visuals)
		{
			if (o->GetContext()->IsVisible())
				o->DrawContent(graphics);
			//if (m_iOldTimeCount != m_iTimeCount)
				//o->UpdateTimeCode(graphics, this->m_timecode);
			if (m_bUpdateHand == true)
				o->SetCheckRange(true);
		}
	}
	graphics.Restore(state7);

	// =========== Draw timeline playhead ==================== //

	auto service = ServiceProvider::Instance()->GetTimelineService();
	if (service != nullptr)
	{
		auto engine = service->GetTimeCodeEngine();
		engine->SetTotalTimecode(cur_timecode);

		if (engine->IsAlive())
		{
			int iSec = m_timecode.TotalMilliseconds() / 1000;

			if (m_timecode.TotalMilliseconds() < 10000)
			{
				if (iSec % 10 == 0 && m_iOldSec != iSec)
					SeekTo(0);
			}
			else
			{	
				if (iSec % 10 == 0 && m_iOldSec != iSec)
					SeekTo(iSec);

				if (m_bUpdateHand == true)
				{
					m_bUpdateHand = false;
					SeekTo(iSec);
				}
			}

			m_iOldSec = iSec;
		}
	}

	int iOffset = m_timecodeBar.GetOffset();

	Pen playheadPen(Color::LimeGreen, 2);
	double adjustFactor = 0.1;

	int iPos = m_timecode.TotalMilliseconds() * adjustFactor + 200 + iOffset;

	
	if (iPos >= 200)
	{
		graphics.DrawLine(&playheadPen,
			iPos,//m_timecode.TotalMilliseconds() * adjustFactor + 200 + iOffset,
			TimelineEditViewButtonPanel::VisualConstant::Height + TimeCodeBar::VisualConstant::Height,
			iPos,//m_timecode.TotalMilliseconds() * adjustFactor + 200 + iOffset,
			rect.GetBottom());
	}
	

//	if (m_iOldTimeCount != m_iTimeCount)
	//	m_iOldTimeCount = m_iTimeCount;
}

void TimelineEditFacade::OnCreate(const Gdiplus::Rect& rect)
{
	m_clientRect = rect;
	AdjustLayout(m_clientRect);
}

void TimelineEditFacade::OnDestroy()
{
	delete m_pDC;
	m_pDC = nullptr;
}

void TimelineEditFacade::OnSize(const Gdiplus::Rect& rect)
{
	m_clientRect = rect;
	AdjustLayout(m_clientRect);
	m_wnd->Invalidate(false);
}


void TimelineEditFacade::OnSizing(const Gdiplus::Rect& rect)
{
	m_clientRect = rect;
	AdjustLayout(m_clientRect);
	m_wnd->Invalidate(false);
}

void TimelineEditFacade::OnLButtonDown(const Gdiplus::Point& point)
{
	m_mousePosition = point;

	bool hasHit = m_buttonPanel.MouseLeftButtonDown(m_mousePosition);

	if (hasHit)
	{
		auto r = m_buttonPanel.GetRect();
		CRect rect;
		rect.left = r.X;
		rect.right = r.X + r.Width;
		rect.top = r.Y;
		rect.bottom = r.Y + r.Height;
		//m_wnd->InvalidateRect(&rect, false);
	}

	if (!m_buttonPanel.HasCollision(point))
	{
		m_timecodeBar.MouseLeftButtonDown(m_mousePosition);
		auto result = m_manager->GetHitTrack(m_mousePosition);
		if (result.HasResult)
		{
#ifdef ENGINNERING_MODE
#if true
			std::string output = boost::str(boost::format("TimelineEditFacade::Track=%s") % result.Context->GetContext()->Name());
			DumpToLog(output);
#endif
#endif // ENGINNERING_MODE
			//result.Context->MouseLeftButtonDown(m_mousePosition);
			m_manager->TrackClick(m_mousePosition);
		}
	}

	m_wnd->Invalidate(false);
}

void TimelineEditFacade::OnLButtonUp(const Gdiplus::Point& point)
{
	m_buttonPanel.MouseLeftButtonUp(m_mousePosition);
	m_timecodeBar.MouseLeftButtonUp(m_mousePosition);
	auto result = m_manager->GetHitTrack(m_mousePosition);
	if (result.HasResult)
	{
		result.Context->MouseLeftButtonUp(m_mousePosition);
	}

	m_wnd->Invalidate(false);
}

void TimelineEditFacade::OnMouseMove(const Gdiplus::Point& point)
{
	char szMsg[512];

	COPYDATASTRUCT cpd;
	RECT rect;

	m_mousePosition = point;
	

	m_buttonPanel.MouseMove(m_mousePosition);

	UpdateCurTimeCode();

	if (m_ButtonPanelRect.PtInRect(CPoint(point.X, point.Y)))
	{
		UpdateButtonPanel();

		bool bUpdate = m_buttonPanel.CheckStatus(point);

		if (bUpdate)
			m_wnd->Invalidate(false);

		return ;
	}

	m_timecodeBar.MouseMove(m_mousePosition);

	if (m_TimeCodeBarRect.PtInRect(CPoint(point.X, point.Y)))
	{
		UpdateTimeCodeBar();

		//m_wnd->Invalidate(false);

		return;
	}

	auto result = m_manager->GetHitTrack(m_mousePosition);
	if (result.HasResult)
	{
		result.Context->MouseMove(m_mousePosition);

		UpdateTrackContent();

		if (result.Context->CheckStatus())
		{
			m_wnd->Invalidate(false);
		}
	}

	/*
	if (m_bDoRedraw)
	{
		UpdateTrackContent();

		m_wnd->Invalidate(false);

		m_iRedrawCount--;
		if(m_iRedrawCount <= 0)
			m_bDoRedraw = false;
	}
	*/

	if (m_pTimeCodeQueue->GetTotal() == 0)
	{
		UpdateTrackContent();
		m_wnd->Invalidate(false);
	}

	//m_wnd->Invalidate(false);

#ifdef ENGINNERING_MODE
#if false
	TimeCode timecode = this->XToTimeCode(this->m_clientRect, this->m_mousePosition.X);
	int timecode_x = this->TimeCodeToX(this->m_clientRect, timecode);
	std::string output = boost::str(boost::format("TimelineEditFacade::OnMouseMove=(%d, %d, %s, %d)") % m_mousePosition.X % m_mousePosition.Y % timecode.ToString() %timecode_x);
	DumpToLog(output);
#endif
#endif // ENGINNERING_MODE


#ifdef ENGINNERING_MODE
#if false
	std::string output = boost::str(boost::format("TimelineEditFacade::OnMouseMove=(%d, %d)") % m_mousePosition.X % m_mousePosition.Y);
	DumpToLog(output);
#endif
#endif // ENGINNERING_MODE
}

void TimelineEditFacade::OnMouseWheel(short zDelta, const Gdiplus::Point& point)
{
	auto temp = m_vertical_scroll_shift + zDelta / 4;
	if (temp <= 0)
	{
		m_vertical_scroll_shift = temp;

		m_manager->SetVerticalScrollShift(m_vertical_scroll_shift);
	}

#ifdef ENGINNERING_MODE
#if false
	std::string dumpMessage = boost::str(boost::format("TimelineEditFacade::OnMouseWheel=(%d, %d)") % m_vertical_scroll_shift %zDelta);
	DumpToLog(dumpMessage);
#endif
#endif // ENGINNERING_MODE
	m_wnd->Invalidate(false);
}

void TimelineEditFacade::OnRButtonUp(const Gdiplus::Point& point)
{
	m_mousePosition = point;

	auto visuals = m_manager->GetTrackVisualsOfCurrentItem();

	for (auto o : visuals)
	{
		if (o->GetContext()->IsVisible() && o->HasCollision(point))
			;
		else
		{
			o->MouseRButtonUp(m_mousePosition);
			UpdateTrackContent();
		}
	}
	m_wnd->Invalidate(false);

	auto result = m_manager->GetHitTrack(m_mousePosition);
	if (result.HasResult)
	{
		result.Context->MouseRButtonUp(m_mousePosition);
		UpdateTrackContent();
		m_wnd->Invalidate(false);
		result.Context->MouseRButtonUp2(m_mousePosition);
	}
}

void TimelineEditFacade::OnKeyDown(unsigned int key)
{
	auto visuals = m_manager->GetTrackVisualsOfCurrentItem();

	if (visuals.size() > 0)
		for (auto o : visuals)
			o->OnKeyDown(key);

	switch (key)
	{
		case VK_DELETE:
		{
			//m_eventManager.KeyDownDelete();
			break;
		}
		case VK_INSERT:
		{
			/*
			if (m_eventManager.IsEnableAdd())
			{
				m_eventManager.DisableAdd();
			}
			else
			{
				m_eventManager.EnableAdd();
			}
			*/

			break;
		}
		case VK_UP:
		{
			break;
		}
		case VK_DOWN:
		{
			break;
		}
		case VK_PRIOR:
		{
			break;
		}
		case VK_NEXT:
		{
			break;
		}
		case VK_LEFT:
		{
			break;
		}
		case VK_RIGHT:
		{
			break;
		}
		case VK_HOME:
		{
			break;
		}
		case VK_END:
		{
			break;
		}
	}
}

void TimelineEditFacade::DrawTimeCodeSpaceTransform(Gdiplus::Graphics& graphics, const std::string& text, const Gdiplus::Point& point)
{
	Gdiplus::SolidBrush brush(Gdiplus::Color(255, 239, 186, 40));  //Gdiplus::Color::SteelBlue
	Gdiplus::RectF rect(point.X, point.Y, TLTrackVis::VisualConstant::ExpanderWidth, TimeCodeBar::VisualConstant::Height);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
	format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

	std::string name = text;
	std::wstring widestr = std::wstring(name.begin(), name.end());
	std::wstring temp = L"...";
	if (name.size() > 20)
	{
		widestr = widestr.substr(0, 20) + temp;
	}

	graphics.DrawString(
		widestr.c_str(),
		widestr.length(),
		TLFont::Instance().GetFont(9),
		rect,
		&format,
		&brush);
}

void TimelineEditFacade::DrawCurrentTimeCode(Gdiplus::Graphics& graphics, const std::string& text, const Gdiplus::Point& point)
{
	Gdiplus::SolidBrush brush(Gdiplus::Color(255, 239, 186, 40)); //Gdiplus::Color::White
	Gdiplus::RectF rect(point.X, point.Y, TLTrackVis::VisualConstant::ExpanderWidth, TimeCodeBar::VisualConstant::Height);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
	format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

	std::string name = text;
	std::wstring widestr = std::wstring(name.begin(), name.end());
	std::wstring temp = L"...";
	if (name.size() > 20)
	{
		widestr = widestr.substr(0, 20) + temp;
	}

	graphics.DrawString(
		widestr.c_str(),
		widestr.length(),
		TLFont::Instance().GetFont(9),
		rect,
		&format,
		&brush);
}

void TimelineEditFacade::Start(std::string msg)
{
	auto service = ServiceProvider::Instance()->GetTimelineService();
	if (service != nullptr)
	{

#ifdef ENGINNERING_MODE
#if false
	DumpToLog("TimelineEditFacade::Start");
#endif
#endif // ENGINNERING_MODE

		auto engine = service->GetTimeCodeEngine();

		engine->UiUpdate.disconnect(boost::bind(&TimelineEditFacade::UpdateTimeCode, this, _1));
		engine->UiUpdate2.disconnect(boost::bind(&TimelineEditFacade::UpdateTimeCode2, this, _1));
//		engine->StopTimeCodeUpdate();

		engine->UiUpdate.connect(boost::bind(&TimelineEditFacade::UpdateTimeCode, this, _1));
		engine->UiUpdate2.connect(boost::bind(&TimelineEditFacade::UpdateTimeCode2, this, _1));
		
		int iOffset = abs(m_timecodeBar.GetOffset());
		if (iOffset != 0)
		{
			if(m_timecode.TotalMilliseconds() < iOffset * 10)
				engine->SetTimecode(iOffset * 10);
		}
		
		engine->StartTimeCodeUpdate();

		m_manager->DoPlayback_AllOutDevices(PBC_PLAY);
	}
}

void TimelineEditFacade::Start2(std::string msg)
{
	auto service = ServiceProvider::Instance()->GetTimelineService();
	if (service != nullptr)
	{

#ifdef ENGINNERING_MODE
#if false
		DumpToLog("TimelineEditFacade::Start");
#endif
#endif // ENGINNERING_MODE

		auto engine = service->GetTimeCodeEngine();
		engine->UiUpdate.connect(boost::bind(&TimelineEditFacade::UpdateTimeCode, this, _1));
		engine->UiUpdate2.connect(boost::bind(&TimelineEditFacade::UpdateTimeCode2, this, _1));
		/*
		int iOffset = abs(m_timecodeBar.GetOffset());
		if (iOffset != 0)
		{
			if (m_timecode.TotalMilliseconds() < iOffset * 10)
				engine->SetTimecode(iOffset * 10);
		}
		*/

		engine->SetTimecode(m_timecode.TotalMilliseconds());

		engine->StartTimeCodeUpdate();

		m_manager->DoPlayback_AllOutDevices(PBC_PLAY);
	}
}

void TimelineEditFacade::Pause(std::string msg)
{
	auto service = ServiceProvider::Instance()->GetTimelineService();
	if (service != nullptr)
	{

#ifdef ENGINNERING_MODE
#if false
		DumpToLog("TimelineEditFacade::Pause");
#endif
#endif // ENGINNERING_MODE

		auto engine = service->GetTimeCodeEngine();
		engine->UiUpdate.disconnect(boost::bind(&TimelineEditFacade::UpdateTimeCode, this, _1));
		engine->UiUpdate2.disconnect(boost::bind(&TimelineEditFacade::UpdateTimeCode2, this, _1));
		engine->PauseTimeCodeUpdate();

		m_manager->DoPlayback_AllOutDevices(PBC_PAUSE);
	}
}

void TimelineEditFacade::Stop(std::string msg)
{
	auto service = ServiceProvider::Instance()->GetTimelineService();
	if (service != nullptr)
	{

#ifdef ENGINNERING_MODE
#if false
	DumpToLog("TimelineEditFacade::Stop");
#endif
#endif // ENGINNERING_MODE

		auto engine = service->GetTimeCodeEngine();
		engine->UiUpdate.disconnect(boost::bind(&TimelineEditFacade::UpdateTimeCode, this, _1));
		engine->UiUpdate2.disconnect(boost::bind(&TimelineEditFacade::UpdateTimeCode2, this, _1));
		engine->StopTimeCodeUpdate();

		m_manager->DoPlayback_AllOutDevices(PBC_STOP);
	}
	StopSource();
}

void TimelineEditFacade::Back(std::string msg)
{
	int iOffset = m_timecodeBar.GetOffset();
	UnitMode unit_mode = m_manager->GetUnitMode();
	auto item = m_manager->GetItem(m_manager->GetSelectedItem(""));

	int iUnit = 10;
	switch (unit_mode)
	{
		case UnitMode::UM_10_SEC:
			iUnit = 10;
			break;
		case UnitMode::UM_1_MIN:
			iUnit = 60;
			break;
		case UnitMode::UM_10_MINS:
			iUnit = 60*10;
			break;
	}

	if (iOffset < 0)
	{
		int iTmp = iOffset + 100 * iUnit;// 10;
		if (iTmp > 0)
			iTmp = 0;
		m_timecodeBar.SetOffset(iTmp);

		if(item.get())
			item->GetInfo().SetDisplayStartTime(iTmp);
		auto visuals = m_manager->GetTrackVisualsOfCurrentItem();
		if (visuals.size() > 0)
		{
			for (auto o : visuals)
			{
				o->SetOffset(iTmp);
			}
		}

		auto commandTrack = m_manager->GetCommandTrackVisual();
		if(commandTrack)
			commandTrack->SetOffset(iTmp);

		m_wnd->Invalidate(false);
		m_wnd->PostMessage(WM_TM_REDRAW, 0, 0);
	}
}

void TimelineEditFacade::Next(std::string msg)
{
	int iOffset = m_timecodeBar.GetOffset();

	UnitMode unit_mode = m_manager->GetUnitMode();

	auto item = m_manager->GetItem(m_manager->GetSelectedItem(""));

	int iUnit = 10;
	switch (unit_mode)
	{
	case UnitMode::UM_10_SEC:
		iUnit = 10;
		break;
	case UnitMode::UM_1_MIN:
		iUnit = 60;
		break;
	case UnitMode::UM_10_MINS:
		iUnit = 60 * 10;
		break;
	}

	if (iOffset > -86400 * 100) // -10000 //86400
	{
		int iTmp = iOffset - 100 * iUnit;// 10;
		if (iTmp < -86400 * 100)
			iTmp = -86400 * 100;
		m_timecodeBar.SetOffset(iTmp);

		if (item.get())
			item->GetInfo().SetDisplayStartTime(iTmp);

		auto visuals = m_manager->GetTrackVisualsOfCurrentItem();
		if (visuals.size() > 0)
		{
			for (auto o : visuals)
			{
				o->SetOffset(iTmp);
			}
		}

		auto commandTrack = m_manager->GetCommandTrackVisual();
		if(commandTrack)
			commandTrack->SetOffset(iTmp);

		m_wnd->Invalidate(false);
		m_wnd->PostMessage(WM_TM_REDRAW,0,0);
	}
}

void TimelineEditFacade::PointerModeChanged(PointerMode mode)
{
	m_manager->SetPointerMode(mode);
}

void TimelineEditFacade::LineModeChanged(LineMode mode)
{
	m_manager->SetLineMode(mode);
}

void TimelineEditFacade::UnitModeChanged(UnitMode mode)
{
	m_manager->SetUnitMode(mode);
}

void TimelineEditFacade::UpdateTimeCode(TimeCode timecode)
{
#if 0
	this->m_timecode = timecode;

	if (m_iTimeCount % 25 == 0)
		m_manager->UpdateTimeCode(timecode);

	ProcessTimeCode();

	if (m_iTimeCount % 3 == 0)
	{
		UpdateTrackContent();
		UpdateCurTimeCode();
		m_wnd->Invalidate(false);
	}

//	m_wnd->PostMessage(WM_TM_UPDATE_TIME_CODE,0,0);
//	m_iTimeCount++;
#else
	
	TCAdapter* pTCAdapter = m_manager->GetTCAdapter();
	if (pTCAdapter)
	{
		if (pTCAdapter->GetReceiveStatus() == CRS_HUNGER)
		{
			char szMsg[512];
			if (m_iTimeCount % 100 == 0)
			{
				sprintf(szMsg, "TC TimelineEditFacade::UpdateTimeCode - %d [CRS_HUNGER]\n", pTCAdapter->GetReceiveStatus());
				OutputDebugStringA(szMsg);
			}
		}

		if (pTCAdapter->GetMode() == TCM_CLIENT && pTCAdapter->IsWorking() && pTCAdapter->GetReceiveStatus() != CRS_HUNGER)
			return ;
	}
	
	AddTimeCodeToQueue(timecode);
#endif
}

void TimelineEditFacade::UpdateTimeCode1(TimeCode timecode)
{
	this->m_timecode = timecode;
	
	if (m_iTimeCount % 25 == 0)
		m_manager->UpdateTimeCode(timecode);

	ProcessTimeCode();

	if (m_iTimeCount % 3 == 0)
	{
//		UpdateTrackContent();
//		UpdateCurTimeCode();
		UpdateTrackContent2();
		m_wnd->Invalidate(false);
	}
}

void TimelineEditFacade::UpdateTimeCode2(TimeCode timecode)
{
	AddTimeCodeToQueue(timecode);
}

void TimelineEditFacade::UpdateTimeCode3(TimeCode timecode)
{
	this->m_timecode = timecode;
}

void TimelineEditFacade::ProcessTimeCode()
{
	bool bUpdateTimeCode = false;
	/*
	auto commandTrack = m_manager->GetCommandTrackVisual();
	if (m_iOldTimeCount != m_iTimeCount)
		commandTrack->UpdateTimeCode(m_timecode);
		*/
	auto visuals = m_manager->GetTrackVisualsOfCurrentItem();

	if (visuals.size() > 0)
	{
		for (auto o : visuals)
		{
			if (m_iOldTimeCount != m_iTimeCount)
				o->UpdateTimeCode(this->m_timecode);

			if (o->CheckPBStatus())
				bUpdateTimeCode = true;
		}
	}

	TCAdapter* pTCAdapter = m_manager->GetTCAdapter();
	if (pTCAdapter)
	{
		double dTimeCode = this->m_timecode.TotalMilliseconds() / 1000.0;
		if(bUpdateTimeCode)
			pTCAdapter->UpdateTimeCodeFromTCEngine(dTimeCode);
	}
	/*
	double dTimeCode = this->m_timecode.TotalMilliseconds() / 1000.0;
	for (int x = 0; x < g_MediaStreamAry.size(); x++)
	{
		if (g_MediaStreamAry.at(x)->IsOpened())
			g_MediaStreamAry.at(x)->UpdateGlobalTimeCode(dTimeCode);
	}
	*/

	auto commandTrack = m_manager->GetCommandTrackVisual();
	if (m_iOldTimeCount != m_iTimeCount)
		commandTrack->UpdateTimeCode(m_timecode);

	if (m_iOldTimeCount != m_iTimeCount)
		m_iOldTimeCount = m_iTimeCount;

	m_iTimeCount++;
}

void TimelineEditFacade::BaseTimeCodeChanged(TimeCode timecode)
{
	//UpdateTrackContent();
	//UpdateCurTimeCode();
	UpdateTrackContent2();

	m_wnd->Invalidate(false);
}

void TimelineEditFacade::StatusChanged(std::string msg)
{
	m_wnd->Invalidate(false);
}

void TimelineEditFacade::AdjustLayout(const Gdiplus::Rect& rect)
{
	m_buttonPanel.SetRect(rect.X, rect.Y, rect.Width, TimelineEditViewButtonPanel::VisualConstant::Height);
	m_timecodeBar.SetRect(Gdiplus::Rect(
		rect.X + TLTrackVis::VisualConstant::ExpanderWidth,
		rect.Y + TimelineEditViewButtonPanel::VisualConstant::Height,
		rect.Width - TLTrackVis::VisualConstant::ExpanderWidth,
		TimeCodeBar::VisualConstant::Height));
}

void TimelineEditFacade::SeekToBegin()
{
	Stop("");

	SeekTo(0);

	this->m_timecode = 0;
	m_manager->UpdateTimeCode(m_timecode);
	m_wnd->Invalidate(false);
}

void TimelineEditFacade::RetunFileName()
{
	auto visuals = m_manager->GetTrackVisualsOfCurrentItem();
	auto track_pos_info = m_manager->GetTrackContentManager()->GetTrackPosInfo();
	wchar_t* wszFilePath = m_manager->GetTrackContentManager()->GetFilePath();

	if (visuals.size() > 0)
	{
		for (auto o : visuals)
		{
			
			std::string szFullPath = o->GetFullPath();
			if (strcmp(szFullPath.c_str(), track_pos_info->szFullPath) == 0)
			{
				o->ReturnFileName(wszFilePath);
				m_wnd->Invalidate(false);
				break;
			}
		}
	}
}

void TimelineEditFacade::SeekTo(int iSec)
{
	//int iTmp = -iSec * 100;
	int iTmp = -(iSec - (iSec % 10)) * 100;

	m_timecodeBar.SetOffset(iTmp);

	auto visuals = m_manager->GetTrackVisualsOfCurrentItem();
	if (visuals.size() > 0)
	{
		for (auto o : visuals)
		{
			o->SetOffset(iTmp);
		}
	}

	auto commandTrack = m_manager->GetCommandTrackVisual();
	if(commandTrack)
		commandTrack->SetOffset(iTmp);

	//UpdateTrackContent();
	//UpdateCurTimeCode();
	UpdateTrackContent2();
	UpdateTimeCodeBar();

	m_wnd->Invalidate(false);
//	m_wnd->PostMessage(WM_TM_REDRAW, 0, 0);
}

void TimelineEditFacade::UpdateHand(bool bUpdate)
{
	m_bUpdateHand = bUpdate;
}

void TimelineEditFacade::SetTimeBarOffset(long lOffset)
{
	m_timecodeBar.SetOffset(lOffset);

	auto visuals = m_manager->GetTrackVisualsOfCurrentItem();
	if (visuals.size() > 0)
	{
		for (auto o : visuals)
		{
			o->SetOffset(lOffset);
		}
	}

	auto commandTrack = m_manager->GetCommandTrackVisual();
	commandTrack->SetOffset(lOffset);

	//UpdateTrackContent();
	//UpdateCurTimeCode();
	UpdateTrackContent2();
	UpdateTimeCodeBar();
	m_wnd->Invalidate(false);
}

void TimelineEditFacade::SetVerticalScrollShift(int iValue)
{
	m_vertical_scroll_shift = iValue;

	//UpdateTrackContent();
	//UpdateCurTimeCode();
	UpdateTrackContent2();

	m_wnd->Invalidate(false);
}

void TimelineEditFacade::UpdateCurTimeCode()
{
	COPYDATASTRUCT cpd;
	RECT rect, rect2;

	rect2.left = m_CurTimeCodeRect.left;
	rect2.top = m_CurTimeCodeRect.top;
	rect2.right = m_CurTimeCodeRect.right;
	rect2.bottom = m_CurTimeCodeRect.bottom;

	cpd.dwData = UPDATE_CURTIMECODE;
	cpd.cbData = sizeof(RECT);
	cpd.lpData = &rect2;

	m_wnd->SendMessage(WM_COPYDATA, 0, (LPARAM)(LPVOID)&cpd);
}

void TimelineEditFacade::UpdateTrackContent()
{
	COPYDATASTRUCT cpd;
	RECT rect, rect2;

	rect.left = m_TrackContentRect.left;
	rect.top = m_TrackContentRect.top;
	rect.right = m_TrackContentRect.right;
	rect.bottom = m_TrackContentRect.bottom;

	cpd.dwData = UPDATE_TRACK_CONTENT;
	cpd.cbData = sizeof(RECT);
	cpd.lpData = &rect;

	m_wnd->SendMessage(WM_COPYDATA, 0, (LPARAM)(LPVOID)&cpd);
/*
	rect2.left = m_CurTimeCodeRect.left;
	rect2.top = m_CurTimeCodeRect.top;
	rect2.right = m_CurTimeCodeRect.right;
	rect2.bottom = m_CurTimeCodeRect.bottom;

	cpd.dwData = UPDATE_CURTIMECODE;
	cpd.cbData = sizeof(RECT);
	cpd.lpData = &rect2;

	m_wnd->SendMessage(WM_COPYDATA, 0, (LPARAM)(LPVOID)&cpd);
	*/
}

void TimelineEditFacade::UpdateTrackContent2()
{
	COPYDATASTRUCT cpd;
	TrackContentEvent tc_event;

	tc_event.track_content_rect.left = m_TrackContentRect.left;
	tc_event.track_content_rect.top = m_TrackContentRect.top;
	tc_event.track_content_rect.right = m_TrackContentRect.right;
	tc_event.track_content_rect.bottom = m_TrackContentRect.bottom;

	tc_event.cur_time_code_rect.left = m_CurTimeCodeRect.left;
	tc_event.cur_time_code_rect.top = m_CurTimeCodeRect.top;
	tc_event.cur_time_code_rect.right = m_CurTimeCodeRect.right;
	tc_event.cur_time_code_rect.bottom = m_CurTimeCodeRect.bottom;

	cpd.dwData = UPDATE_TRACK_CONTENT2;
	cpd.cbData = sizeof(TrackContentEvent);
	cpd.lpData = &tc_event;

	m_wnd->SendMessage(WM_COPYDATA, 0, (LPARAM)(LPVOID)&cpd);
}

void TimelineEditFacade::UpdateButtonPanel()
{
	COPYDATASTRUCT cpd;
	RECT rect, rect2;

	rect.left = m_ButtonPanelRect.left;
	rect.top = m_ButtonPanelRect.top;
	rect.right = m_ButtonPanelRect.right;
	rect.bottom = m_ButtonPanelRect.bottom;

	cpd.dwData = UPDATE_BUTTON_PANEL;
	cpd.cbData = sizeof(RECT);
	cpd.lpData = &rect;

	m_wnd->SendMessage(WM_COPYDATA, 0, (LPARAM)(LPVOID)&cpd);
}

void TimelineEditFacade::UpdateTimeCodeBar()
{
	COPYDATASTRUCT cpd;
	RECT rect, rect2;

	rect.left = m_TimeCodeBarRect.left;
	rect.top = m_TimeCodeBarRect.top;
	rect.right = m_TimeCodeBarRect.right;
	rect.bottom = m_TimeCodeBarRect.bottom;

	cpd.dwData = UPDATE_TIME_CODE_BAR;
	cpd.cbData = sizeof(RECT);
	cpd.lpData = &rect;

	m_wnd->SendMessage(WM_COPYDATA, 0, (LPARAM)(LPVOID)&cpd);
}

void TimelineEditFacade::Redraw()
{
	m_bDoRedraw = true;
	m_iRedrawCount = 100;
}

void TimelineEditFacade::PreloadSource()
{
	auto visuals = m_manager->GetTrackVisualsOfCurrentItem();

	if (visuals.size() > 0)
	{
		for (auto o : visuals)
		{
			o->PreloadSource();
		}
	}
}

void TimelineEditFacade::StopSource()
{
	auto visuals = m_manager->GetTrackVisualsOfCurrentItem();

	if (visuals.size() > 0)
	{
		for (auto o : visuals)
		{
			o->StopSource();
		}
	}
}

void TimelineEditFacade::RemoveTimeCodeQueue()
{
	m_pTimeCodeQueue->Reset();
	/*
	int iTotal = m_TimeCodeQueue.size();
	for (int j = 0; j < iTotal; j++)
	{
		TimeCode* pCurTimeCode = &m_TimeCodeQueue.front();
		m_TimeCodeQueue.pop();
	}
	*/
}

void TimelineEditFacade::AddTimeCodeToQueue(TimeCode timecode)
{
	//m_TimeCodeQueue.push(timecode);
	m_pTimeCodeQueue->Add((unsigned char*)&timecode,sizeof(TimeCode));
}

TimeCode* TimelineEditFacade::GetTimeCodeFromeQueue()
{
	unsigned char* p = m_pTimeCodeQueue->GetNext();
	m_pTmpTimeCode = (TimeCode*)p;
	//m_pTmpTimeCode = &m_TimeCodeQueue.front();
	return m_pTmpTimeCode;
}

void TimelineEditFacade::RemoveTimeCodeItem()
{
	//m_TimeCodeQueue.pop();
}

void TimelineEditFacade::ThreadEvent()
{
	ThreadBase::Lock();

	//int iTotal = m_TimeCodeQueue.size();
	int iTotal = m_pTimeCodeQueue->GetTotal();
	if (iTotal > 0)
	{
		TimeCode* pCurTimeCode = GetTimeCodeFromeQueue();
		if (pCurTimeCode)
		{
			unsigned long ulCurTimeCode = pCurTimeCode->TotalMilliseconds();
			if (m_ulLastTimeCode != ulCurTimeCode)
			{
				m_ulLastTimeCode = ulCurTimeCode;
				UpdateTimeCode1(*pCurTimeCode);
			}
		}
		RemoveTimeCodeItem();
	}
	Sleep(4);
	ThreadBase::Unlock();
}

void TimelineEditFacade::InsertEffect(EffectInfo* pInfo)
{
	auto visuals = m_manager->GetTrackVisualsOfCurrentItem();
	auto track_pos_info = m_manager->GetTrackContentManager()->GetTrackPosInfo();
	//wchar_t* wszFilePath = m_manager->GetTrackContentManager()->GetFilePath();

	if (visuals.size() > 0)
	{
		for (auto o : visuals)
		{

			std::string szFullPath = o->GetFullPath();
			if (strcmp(szFullPath.c_str(), track_pos_info->szFullPath) == 0)
			{
				o->InsertEffectToTrack(pInfo);
				m_wnd->Invalidate(false);
				break;
			}
		}
	}
}

void TimelineEditFacade::AudioConfigOK()
{
	auto visuals = m_manager->GetTrackVisualsOfCurrentItem();
	auto track_pos_info = m_manager->GetTrackContentManager()->GetTrackPosInfo();
	//wchar_t* wszFilePath = m_manager->GetTrackContentManager()->GetFilePath();

	if (visuals.size() > 0)
	{
		for (auto o : visuals)
		{
			std::string szFullPath = o->GetFullPath();
			if (strcmp(szFullPath.c_str(), track_pos_info->szFullPath) == 0)
			{
				o->AudioConfigOK();
				m_wnd->Invalidate(false);
				break;
			}
		}
	}
}