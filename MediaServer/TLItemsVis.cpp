/********************************************************************\
Project Name: Timeline Prototype

File Name: TLItemVis.cpp

Definition of following classes:
TLItemVis

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
#include <sstream>
#include <boost/format.hpp>

// ==== Include local library ==== //
#include "TLUtility.h"
#include "TLConstant.h"
#include "TLItemsVis.h"
#include "ServiceProvider.h"
#include "MediaServer.h"

#ifdef _NEW_TIME_LINE
#include "TimeLineSettingsDlg.h"
#endif

#define ENGINNERING_MODE

#define BeginButtonText "Begin"
#define StartButtonText "Start"
#define PauseButtonText "Pause"

using namespace Timeline;
extern CMediaServerApp theApp;

TLItemVisButton::TLItemVisButton()
{
	m_bActive = false;
	m_bUpdate = false;
}

void TLItemVisButton::SetRegion(Gdiplus::Rect rect)
{
	_Region = rect;
}

void TLItemVisButton::SetRegion(int x, int y, int w, int h)
{
	_Region.X = x;
	_Region.Y = y;
	_Region.Width = w;
	_Region.Height = h;
}

Gdiplus::Rect& TLItemVisButton::GetRegion()
{
	return _Region;
}

void TLItemVisButton::MouseLeftButtonDown(const Gdiplus::Point& point)
{
	if (_Region.Contains(point))
		Select();
	else
		Deselect();
}

void TLItemVisButton::MouseMove(const Gdiplus::Point& point)
{
	m_bUpdate = false;
	if (_Region.Contains(point))
	{
		if (!m_bActive)
			m_bUpdate = true;

		m_bActive = true;
	}
	else
	{
		if (m_bActive)
			m_bUpdate = true;
		m_bActive = false;
		Deselect();
	}
}

void TLItemVisButton::Draw(Gdiplus::Graphics& graphics)
{
	Pen selectPen(Color::Orange, 1); //Red
	Pen deselectPen(Color(255, 150, 150, 50), 1);//Color::SteelBlue
	SolidBrush selectBrush(Color::Orange); //Red
	SolidBrush deselectBrush(Color(255, 150, 150, 50)); //Color::SteelBlue

	Pen* pen = &deselectPen;
	SolidBrush* brush = &deselectBrush;

	if (IsSelect())
	{
		pen = &selectPen;
		brush = &selectBrush;
	}

	if (m_bActive)
	{
		graphics.DrawRectangle(pen, m_rect.X - 2, m_rect.Y - 2, m_rect.Width + 3, m_rect.Height + 3);
	}

	graphics.FillRectangle(brush, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height);

	// -------- Draw title -------- //
	Gdiplus::SolidBrush headerBrush(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::RectF layoutRect(m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

	std::string name = m_text;
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
		layoutRect,
		&format,
		&headerBrush);

	if (m_hasImage)
	{
		Gdiplus::Image* image = m_image.get();
		//graphics.DrawImage(image, m_rect.X + 1, m_rect.Y + 1, 29, 29);
		graphics.DrawImage(image, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height);
	}
}

bool TLItemVisButton::CheckStatus()
{
	if (m_bUpdate)
		return true;
	return false;
}

//=========//
TLItemVisButton2::TLItemVisButton2()
{
	m_bActive = false;
	m_bUpdate = false;
}

void TLItemVisButton2::SetRegion(Gdiplus::Rect rect)
{
	_Region = rect;
}

void TLItemVisButton2::SetRegion(int x, int y, int w, int h)
{
	_Region.X = x;
	_Region.Y = y;
	_Region.Width = w;
	_Region.Height = h;
}

Gdiplus::Rect& TLItemVisButton2::GetRegion()
{
	return _Region;
}

void TLItemVisButton2::MouseLeftButtonDown(const Gdiplus::Point& point)
{
	if (_Region.Contains(point))
		Select();
	else
		Deselect();
}

void TLItemVisButton2::MouseMove(const Gdiplus::Point& point)
{
	m_bUpdate = false;
	if (_Region.Contains(point))
	{
		if (!m_bActive)
			m_bUpdate = true;

		m_bActive = true;
	}
	else
	{
		if (m_bActive)
			m_bUpdate = true;
		m_bActive = false;
//		Deselect();
	}
}

void TLItemVisButton2::Draw(Gdiplus::Graphics& graphics)
{
	TLBmpButtonBase::Draw(graphics);

	if (IsSelect())
		DrawDown(graphics);
	else if (m_bActive)
		DrawActive(graphics);
	else
		DrawNormal(graphics);
	/*
	Pen selectPen(Color::Orange, 1); //Red
	Pen deselectPen(Color(255, 150, 150, 50), 1);//Color::SteelBlue
	SolidBrush selectBrush(Color::Orange); //Red
	SolidBrush deselectBrush(Color(255, 150, 150, 50)); //Color::SteelBlue

	Pen* pen = &deselectPen;
	SolidBrush* brush = &deselectBrush;

	if (IsSelect())
	{
		pen = &selectPen;
		brush = &selectBrush;
	}

	if (m_bActive)
	{
		graphics.DrawRectangle(pen, m_rect.X - 2, m_rect.Y - 2, m_rect.Width + 3, m_rect.Height + 3);
	}

	graphics.FillRectangle(brush, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height);

	// -------- Draw title -------- //
	Gdiplus::SolidBrush headerBrush(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::RectF layoutRect(m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

	std::string name = m_text;
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
		layoutRect,
		&format,
		&headerBrush);

	if (m_hasImage)
	{
		Gdiplus::Image* image = m_image.get();
		//graphics.DrawImage(image, m_rect.X + 1, m_rect.Y + 1, 29, 29);
		graphics.DrawImage(image, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height);
	}
	*/
}

bool TLItemVisButton2::CheckStatus()
{
	if (m_bUpdate)
		return true;
	return false;
}


/* ====================================================== *\
         TLItemVis
\* ====================================================== */
TLItemVis::TLItemVis()
{
#ifdef _NEW_TIME_LINE
	// ==== Initializing buttons ==== //
	m_btnBegin.SetPosition(0, 0);
	m_btnBegin.SetSize(38, 38);
	m_btnBegin.SetText(BeginButtonText);
	std::string szBackward = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\backward.bmp";
	m_btnBegin.SetImage(0,szBackward); 
	std::string szBackward1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\backward_t.bmp";
	m_btnBegin.SetImage(1, szBackward1);
	std::string szBackward2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\backward_c.bmp";
	m_btnBegin.SetImage(2, szBackward2);

	m_btnStart.SetPosition(0, 0);
	m_btnStart.SetSize(38, 38);
	m_btnStart.SetText(StartButtonText);
	std::string szStart = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\start.bmp";
	m_btnStart.SetImage(0,szStart);
	std::string szStart1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\start_t.bmp";
	m_btnStart.SetImage(1, szStart1);
	std::string szStart2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\start_c.bmp";
	m_btnStart.SetImage(2, szStart2);

	m_btnPause.SetPosition(0, 0);
	m_btnPause.SetSize(38, 38);
	m_btnPause.SetText(PauseButtonText);
	std::string szStop = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\pause.bmp";
	m_btnPause.SetImage(0,szStop);
	std::string szStop1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\pause_t.bmp";
	m_btnPause.SetImage(1, szStop1);
	std::string szStop2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\pause_c.bmp";
	m_btnPause.SetImage(2, szStop2);

	m_buttonCollection.push_back(&m_btnBegin);
	m_buttonCollection.push_back(&m_btnStart);
	m_buttonCollection.push_back(&m_btnPause);

#else
	std::wstring wszBackward = std::wstring(szBackward.begin(), szBackward.end());
	m_backwardImage = std::make_shared<Gdiplus::Image>(wszBackward.c_str()); //L"backward.png"

	std::wstring wszStart = std::wstring(szStart.begin(), szStart.end());
	m_startImage = std::make_shared<Gdiplus::Image>(wszStart.c_str()); //L"start.png"

	std::wstring wszStop = std::wstring(szStop.begin(), szStop.end());
	m_stopImage = std::make_shared<Gdiplus::Image>(wszStop.c_str()); //L"stop.png"
#endif

	_expandStatus = ExpandStatus::Expand;

	m_rect.X = 0;
	m_rect.Y = 0;
	m_rect.Width = VisualConstant::Width;
	m_rect.Height = VisualConstant::Height;

	m_iStatus = TLIS_BEGIN;

	SetExpandRegion();
}

TLItemVis::~TLItemVis()
{
#ifdef _NEW_TIME_LINE
	m_buttonCollection.clear();
#endif
}

void TLItemVis::Initialize(TLItemPtr item)
{
	_item = item;
	m_uuid = item->GetInfo().UUID();
}

TLItemPtr& TLItemVis::GetContext()
{
	return _item;
}

void TLItemVis::SetPosition(int x, int y)
{
	TLShape::SetPosition(x, y);
	if (IsExpand())
	{
		SetExpandRegion();
	}
	else 
	{
		SetCollapseRegion();
	}
}

void TLItemVis::MouseClick(const Gdiplus::Point& point)
{
	bool hasCollision = false;

	auto& collection = m_buttonCollection;

	for (auto o : collection)
	{
		o->MouseLeftButtonDown(point);
		if(o->IsSelect())
			hasCollision = true;
	}

	if (hasCollision)
	{
		for (auto o : collection)
		{
			if (o->IsSelect())
			{
				TimelineManager* m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
				CWnd* pEditViewWnd = m_manager->GetEditViewWnd();
				
				if (o->Text() == BeginButtonText)
				{
					this->BeginClick("BeginClick");

					SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_RESET_TIME_CODE, 0, 0);
					SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_BEGIN, 0, 0);

					TCAdapter* pTCAdapter = m_manager->GetTCAdapter();
					if (pTCAdapter)
						pTCAdapter->TC_Stop();

					m_manager->ClearAll();

#ifdef _ENABLE_SINGLE_EFFECT
					m_manager->GetDisplayManager()->EnableToDoEffect(false);
#endif

					m_iStatus = TLIS_BEGIN;
				}
				else if (o->Text() == StartButtonText)
				{
//					m_manager->FreezeShape(true);

					TimeCode total_time_code = _item->GetInfo().TotalTime();

					if (total_time_code.TotalMilliseconds() > 0)
					{
						this->StartClick("StartClick");

//						SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_RESET_TIME_CODE, 0, 0);

						if (m_iStatus == TLIS_START)
							return ;

						if (m_iStatus != TLIS_PAUSE)
						{
							SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PLAY2, 0, 0);
							//PostMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PAUSE, 0, 0);
							//PostMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PLAY, 0, 0);
						}
						else
							SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PLAY, 0, 0);
				
#ifdef _ENABLE_SINGLE_EFFECT
						m_manager->GetDisplayManager()->EnableToDoEffect(true);
#endif

						TCAdapter* pTCAdapter = m_manager->GetTCAdapter();
						if (pTCAdapter)
						{
							pTCAdapter->SetTotalOfTimeCode(total_time_code.TotalMilliseconds());
							pTCAdapter->TC_Start();
						}

						m_iStatus = TLIS_START;
					}
//					m_manager->FreezeShape(false);
				}
				else if (o->Text() == PauseButtonText)
				{
					this->PauseClick("PauseClick");
					SendMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PAUSE, 0, 0);
					m_iStatus = TLIS_PAUSE;
#ifdef _ENABLE_SINGLE_EFFECT
					m_manager->GetDisplayManager()->EnableToDoEffect(false);
#endif

					TCAdapter* pTCAdapter = m_manager->GetTCAdapter();
					if (pTCAdapter)
						pTCAdapter->TC_Pause();
				}
			}
		}
	}

	if (_expanderRegion.Contains(point))
	{
		this->ExpanderClick("ExpanderClick");

		if (IsExpand())
		{
			Collapse();
		}
		else
		{
			Expand();
		}
	}

	if (_titleRegion.Contains(point))
	{
		this->TitleClick("TitleClick");

#ifdef ENGINNERING_MODE
		/*auto service = ServiceProvider::Instance()->GetOutputService();
		if (service != nullptr)
		{
		std::string output = boost::str(boost::format("TLItemVis::_titleRegion=%s") % _item->GetInfo().Name());
		service->SendEvent(OutputEvent(OutputEvent::Type::Action, output));
		}*/
#endif // ENGINNERING_MODE
	}

	if (_idRegion.Contains(point))
	{
		this->IdClick("IdClick");

#ifdef ENGINNERING_MODE
		/*auto service = ServiceProvider::Instance()->GetOutputService();
		if (service != nullptr)
		{
		std::string output = boost::str(boost::format("TLItemVis::_idRegion=%s") % _item->GetInfo().Name());
		service->SendEvent(OutputEvent(OutputEvent::Type::Action, output));
		}*/
#endif // ENGINNERING_MODE
	}

	if (_controlRegion.Contains(point))
	{
		this->ControlClick("ControlClick");

#ifdef ENGINNERING_MODE
		/*auto service = ServiceProvider::Instance()->GetOutputService();
		if (service != nullptr)
		{
			std::string output = boost::str(boost::format("TLItemVis::_controlRegion=%s") % _item->GetInfo().Name());
			service->SendEvent(OutputEvent(OutputEvent::Type::Action, output));
		}*/
#endif // ENGINNERING_MODE
	}

	if (_totalTimeRegion.Contains(point))
	{
		this->TotalTimeClick("TotalTimeClick");

#ifdef ENGINNERING_MODE
		/*auto service = ServiceProvider::Instance()->GetOutputService();
		if (service != nullptr)
		{
			std::string output = boost::str(boost::format("TLItemVis::_totalTimeRegion=%s") % _item->GetInfo().Name());
			service->SendEvent(OutputEvent(OutputEvent::Type::Action, output));
		}*/
#endif // ENGINNERING_MODE
	}

	if (_progressRegion.Contains(point))
	{
		this->ProgressClick("ProgressClick");

#ifdef ENGINNERING_MODE
		/*auto service = ServiceProvider::Instance()->GetOutputService();
		if (service != nullptr)
		{
			std::string output = boost::str(boost::format("TLItemVis::_progressRegion=%s") % _item->GetInfo().Name());
			service->SendEvent(OutputEvent(OutputEvent::Type::Action, output));
		}*/
#endif // ENGINNERING_MODE
	}
}

void TLItemVis::LButtonUp(const Gdiplus::Point& point)
{
	auto& collection = m_buttonCollection;

	for (auto o : collection)
	{
		o->MouseLeftButtonUp(point);
	}
}

void TLItemVis::MouseMove(const Gdiplus::Point& point)
{
	auto& collection = m_buttonCollection;
	for (auto o : collection)
	{
		o->MouseMove(point);
	}
}

void TLItemVis::MouseDbClick(const Gdiplus::Point& point)
{
	char szCurItemName[512];
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();

	if (_titleRegion.Contains(point))
	{	
		if (manager->GetPBStatus() != PBC_PLAY)
		{
			CTimeLineSettingsDlg dlg;
			dlg.SetID(_item->GetInfo().Id());
			dlg.SetName((char*)_item->GetInfo().Name().c_str());

			strcpy(szCurItemName, _item->GetInfo().Name().c_str());

			if (dlg.DoModal() == IDOK)
			{
				std::string sName = dlg.GetName();
				int iID = dlg.GetID();

				_item->GetInfo().SetName(sName);
				_item->GetInfo().SetId(iID);

				TimelineManager* m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
				TrackContentManager* pTrackContentManager = m_manager->GetTrackContentManager();
				pTrackContentManager->ChangeItemName(szCurItemName, (char*)sName.c_str());
				m_manager->ResetItemStatus();
			}
		}
	}

	if (_idRegion.Contains(point))
	{
		if (manager->GetPBStatus() != PBC_PLAY)
		{
			CTimeLineSettingsDlg dlg;
			dlg.SetID(_item->GetInfo().Id());
			dlg.SetName((char*)_item->GetInfo().Name().c_str());
			dlg.DisableName();

			if (dlg.DoModal() == IDOK)
			{
				//std::string sName = dlg.GetName();
				int iID = dlg.GetID();
				_item->GetInfo().SetId(iID);
			}
		}
	}
}

void TLItemVis::Expand()
{
	_expandStatus = ExpandStatus::Expand;
	this->m_rect.Height = VisualConstant::Height;
	SetExpandRegion();
}

void TLItemVis::Collapse()
{
	_expandStatus = ExpandStatus::Collapse;
	this->m_rect.Height = VisualConstant::CollapseHeight;
	SetCollapseRegion();
}

bool TLItemVis::IsExpand()
{
	return (_expandStatus == ExpandStatus::Expand);
}

void TLItemVis::Draw(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	graphics.TranslateTransform(m_rect.X, m_rect.Y);
	
	if (IsExpand())
	{
		DrawBackgroundExpand(graphics);
		DrawExpanderExpand(graphics);
		DrawTitleExpand(graphics);
		DrawIdExpand(graphics);
		DrawControlExpand(graphics);
		DrawTotalTimeExpand(graphics);
		DrawProgressExpand(graphics);
	}
	else
	{
		DrawBackgroundCollapse(graphics);
		DrawExpanderCollapse(graphics);
		DrawTitleCollapse(graphics);
		DrawIdCollapse(graphics);
		DrawControlCollapse(graphics);
		DrawTotalTimeCollapse(graphics);
		DrawProgressCollapse(graphics);
	}

	if (IsSelect())
	{
		DrawFocusBorder(graphics);
	}

	graphics.Restore(state);
}

void TLItemVis::DrawBackgroundExpand(Gdiplus::Graphics& graphics)
{
#ifdef _NEW_TIME_LINE
	Gdiplus::SolidBrush brush(Gdiplus::Color(255, 80, 80, 80));
#else
	Gdiplus::SolidBrush brush(Gdiplus::Color(255, 180, 180, 180));
#endif
	graphics.FillRectangle(&brush, 0, 0, m_rect.Width + 1, m_rect.Height + 1);
}

void TLItemVis::DrawExpanderExpand(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	graphics.TranslateTransform(2, 2);

	// -------- Draw background -------- //
	Gdiplus::LinearGradientBrush brush1(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 30),
#ifdef _NEW_TIME_LINE
		Gdiplus::Color(255, 239, 186, 40),
		Gdiplus::Color(255, 239, 186, 40));
#else
		Gdiplus::Color(255, 116, 149, 192),
		Gdiplus::Color(255, 86, 119, 162));
#endif
	graphics.FillRectangle(&brush1, 0, 0, 30, 30);

	// -------- Draw mark -------- //
	Gdiplus::Pen pen(Gdiplus::Color(255, 255, 255, 255), 4.0f);
	// Draw "-"
	graphics.DrawLine(&pen, 4, 14, 26, 14);
	
	graphics.Restore(state);
}

void TLItemVis::DrawTitleExpand(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	graphics.TranslateTransform(34, 2);

	// -------- Draw background -------- //
	Gdiplus::LinearGradientBrush brush1(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 30),
#ifdef _NEW_TIME_LINE
		Gdiplus::Color(255, 239, 186, 40),
		Gdiplus::Color(255, 239, 186, 40));
#else
		Gdiplus::Color(255, 116, 149, 192),
		Gdiplus::Color(255, 86, 119, 162));
#endif
	graphics.FillRectangle(&brush1, 0, 0, 265, 30);
	
	// -------- Draw title -------- //
	Gdiplus::SolidBrush headerBrush(Gdiplus::Color(255, 0, 0, 0));
	Gdiplus::RectF layoutRect(0, 0, 265, 30);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

	std::string name = _item->GetInfo().Name();
	std::wstring widestr = std::wstring(name.begin(), name.end());
	std::wstring temp = L"...";
	if (name.size() > 20)
	{
		widestr = widestr.substr(0, 20) + temp;
	}
	
	graphics.DrawString(
		widestr.c_str(),
		widestr.length(),
		TLFont::Instance().GetFont(14),
		layoutRect,
		&format,
		&headerBrush);

	graphics.Restore(state);
}

void TLItemVis::DrawIdExpand(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	graphics.TranslateTransform(2, 34);

	// -------- Draw background -------- //
	Gdiplus::LinearGradientBrush brush1(
		Gdiplus::Point(60, 0),
		Gdiplus::Point(0, 0),
#ifdef _NEW_TIME_LINE
		Gdiplus::Color(255, 101, 101, 101),
		Gdiplus::Color(255, 101, 101, 101)
#else
		Gdiplus::Color(255, 208, 163, 209),
		Gdiplus::Color(255, 168, 123, 169)
#endif
		);
	graphics.FillRectangle(&brush1, 0, 0, 60, 60);

	// -------- Draw ID -------- //
	Gdiplus::SolidBrush headerBrush(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::RectF layoutRect(0, 0, 60, 60);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

	std::string content = std::to_string(_item->GetInfo().Id());
	std::wstring widestr = std::wstring(content.begin(), content.end());
	std::wstring temp = L"...";
	if (content.size() > 2)
	{
		widestr = widestr.substr(0, 3) + temp;
	}

	graphics.DrawString(
		widestr.c_str(),
		widestr.length(),
		TLFont::Instance().GetFont(20),
		layoutRect,
		&format,
		&headerBrush);

	graphics.Restore(state);
}

void TLItemVis::DrawControlExpand(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	graphics.TranslateTransform(2, 96);

	// -------- Draw background -------- //
	Gdiplus::LinearGradientBrush brush1(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 43),
#ifdef _NEW_TIME_LINE
		Gdiplus::Color(255, 145, 145, 145),
		Gdiplus::Color(255, 117, 117, 117)
#else
		Gdiplus::Color(255, 120, 120, 120),
		Gdiplus::Color(255, 200, 200, 200)
#endif
	);
	graphics.FillRectangle(&brush1, 0, 0, 171, 43);

	
#ifdef _NEW_TIME_LINE
	graphics.TranslateTransform(13, 4);
	m_btnBegin.Draw(graphics);

	graphics.TranslateTransform(42, 0);

	m_btnStart.Draw(graphics);

	graphics.TranslateTransform(62, 0);
	m_btnPause.Draw(graphics);
#else
	// -------- Draw backward button -------- //
	graphics.TranslateTransform(13, 4);
	Gdiplus::LinearGradientBrush brush2(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 38),
		Gdiplus::Color(255, 12, 46, 90),
		Gdiplus::Color(255, 112, 146, 190)
	);
	graphics.FillRectangle(&brush2, 0, 0, 38, 38);
	Gdiplus::Image* backwardImage = m_backwardImage.get();
	graphics.DrawImage(backwardImage, 0, 0, 38, 38);


	// -------- Draw play/pause button -------- //
	graphics.TranslateTransform(42, 0);
	Gdiplus::LinearGradientBrush brush3(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 38),
		Gdiplus::Color(255, 12, 46, 90),
		Gdiplus::Color(255, 112, 146, 190)
	);
	graphics.FillRectangle(&brush3, 0, 0, 38, 38);
	Gdiplus::Image* startImage = m_startImage.get();
	graphics.DrawImage(startImage, 0, 0, 38, 38);

	// -------- Draw stop button -------- //
	graphics.TranslateTransform(62, 0);
	Gdiplus::LinearGradientBrush brush4(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 38),
		Gdiplus::Color(255, 12, 46, 90),
		Gdiplus::Color(255, 112, 146, 190)
	);
	graphics.FillRectangle(&brush4, 0, 0, 38, 38);
	Gdiplus::Image* stopImage = m_stopImage.get();
	graphics.DrawImage(stopImage, 0, 0, 38, 38);
#endif

	graphics.Restore(state);
}

void TLItemVis::DrawTotalTimeExpand(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	graphics.TranslateTransform(174, 96);

	// -------- Draw background -------- //
	Gdiplus::LinearGradientBrush brush1(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 43),
#ifdef _NEW_TIME_LINE
		Gdiplus::Color(255, 206, 206, 206),
		Gdiplus::Color(255, 206, 206, 206)
#else
		Gdiplus::Color(255, 120, 120, 120),
		Gdiplus::Color(255, 200, 200, 200)
#endif
	);
	graphics.FillRectangle(&brush1, 0, 0, 126, 43);

	// -------- Draw total time -------- //
	Gdiplus::SolidBrush headerBrush(Gdiplus::Color(255, 0, 0, 0));
	Gdiplus::RectF layoutRect(0, 0, 125, 43);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

	std::string content = _item->GetInfo().GetTotalTimeString2();
	std::wstring widestr = std::wstring(content.begin(), content.end());
	std::wstring temp = L"...";
	if (content.size() > 8)
	{
		//widestr = widestr.substr(0, 3) + temp;
	}

	graphics.DrawString(
		widestr.c_str(),
		widestr.length(),
		TLFont::Instance().GetFont(14),
		layoutRect,
		&format,
		&headerBrush);

	graphics.Restore(state);
}

void TLItemVis::DrawProgressExpand(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	
	graphics.TranslateTransform(64, 34);

	// -------- Draw background -------- //
#ifdef _NEW_TIME_LINE
	Gdiplus::LinearGradientBrush brush1(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 60),
		Gdiplus::Color(255, 206, 206, 206),
		Gdiplus::Color(255, 206, 206, 206));
#else
	Gdiplus::SolidBrush brush1(Gdiplus::Color(255, 200, 200, 200));
#endif
	graphics.FillRectangle(&brush1, 0, 0, 235, 60);

	// -------- Draw progress bar -------- //
	Gdiplus::LinearGradientBrush brush2(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 80),
		Gdiplus::Color(255, 79, 147, 55),
		Gdiplus::Color(255, 179, 247, 155));
	int width = _item->GetInfo().GetProgress() * 233 / 100;
	graphics.FillRectangle(&brush2, 2, 2, width, 54);

	// -------- Draw current time and time left -------- //
	Gdiplus::SolidBrush headerBrush(Gdiplus::Color(255, 0, 0, 0));
	Gdiplus::RectF layoutRect(0, 0, 235, 60);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

	std::string content = _item->GetInfo().GetCurrentTimeString()
		+ "|" + _item->GetInfo().GetTimeLeftString();
	std::wstring widestr = std::wstring(content.begin(), content.end());
	std::wstring temp = L"...";
	if (content.size() > 20)
	{
		widestr = widestr.substr(0, 3) + temp;
	}

	graphics.DrawString(
		widestr.c_str(),
		widestr.length(),
		TLFont::Instance().GetFont(20),
		layoutRect,
		&format,
		&headerBrush);

	graphics.Restore(state);
}

void TLItemVis::DrawBackgroundCollapse(Gdiplus::Graphics& graphics)
{
#ifdef _NEW_TIME_LINE
	Gdiplus::SolidBrush brush(Gdiplus::Color(255, 80, 80, 80));
#else
	Gdiplus::SolidBrush brush(Gdiplus::Color(255, 180, 180, 180));
	//Gdiplus::SolidBrush brush(Gdiplus::Color(255, 200, 200, 200));
#endif
	graphics.FillRectangle(&brush, 0, 0, m_rect.Width + 1, m_rect.Height + 1);
}

void TLItemVis::DrawExpanderCollapse(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	graphics.TranslateTransform(2, 2);

	// -------- Draw background -------- //
	Gdiplus::LinearGradientBrush brush1(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 30),
#ifdef _NEW_TIME_LINE
		Gdiplus::Color(255, 239, 186, 40),
		Gdiplus::Color(255, 239, 186, 40));
#else
		Gdiplus::Color(255, 116, 149, 192),
		Gdiplus::Color(255, 86, 119, 162));
#endif
	graphics.FillRectangle(&brush1, 0, 0, 30, 30);

	// -------- Draw mark -------- //
	Gdiplus::Pen pen(Gdiplus::Color(255, 255, 255, 255), 4.0f);
	// Draw "+"
	graphics.DrawLine(&pen, 4, 14, 26, 14);
	graphics.DrawLine(&pen, 15, 4, 15, 26);

	graphics.Restore(state);
}

void TLItemVis::DrawTitleCollapse(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	graphics.TranslateTransform(34, 2);

	// -------- Draw background -------- //
	Gdiplus::LinearGradientBrush brush1(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 30),
#ifdef _NEW_TIME_LINE
		Gdiplus::Color(255, 239, 186, 40),
		Gdiplus::Color(255, 239, 186, 40));
#else
		Gdiplus::Color(255, 116, 149, 192),
		Gdiplus::Color(255, 86, 119, 162));
#endif
	graphics.FillRectangle(&brush1, 0, 0, 265, 30);

	// -------- Draw progress bar -------- //
	Gdiplus::LinearGradientBrush brush2(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 80),
		Gdiplus::Color(255, 79, 147, 55),
		Gdiplus::Color(255, 179, 247, 155));
	int width = _item->GetInfo().GetProgress() * 265 / 100;
	graphics.FillRectangle(&brush2, 1, 1, width, 28);

	// -------- Draw title -------- //
	Gdiplus::SolidBrush headerBrush(Gdiplus::Color(255, 0, 0, 0));
	Gdiplus::RectF layoutRect(0, 0, 265, 30);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

	std::string name = _item->GetInfo().Name();
	std::wstring widestr = std::wstring(name.begin(), name.end());
	std::wstring temp = L"...";
	if (name.size() > 20)
	{
		widestr = widestr.substr(0, 20) + temp;
	}

	graphics.DrawString(
		widestr.c_str(),
		widestr.length(),
		TLFont::Instance().GetFont(14),
		layoutRect,
		&format,
		&headerBrush);

	graphics.Restore(state);
}

void TLItemVis::DrawIdCollapse(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	graphics.TranslateTransform(2, 34);

	// -------- Draw background -------- //
	Gdiplus::LinearGradientBrush brush1(
		Gdiplus::Point(60, 0),
		Gdiplus::Point(0, 0),
#ifdef _NEW_TIME_LINE
		Gdiplus::Color(255, 101, 101, 101),
		Gdiplus::Color(255, 101, 101, 101)
#else
		Gdiplus::Color(255, 208, 163, 209),
		Gdiplus::Color(255, 168, 123, 169)
#endif
	);
	graphics.FillRectangle(&brush1, 0, 0, 60, 43);

	// -------- Draw ID -------- //
	Gdiplus::SolidBrush headerBrush(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::RectF layoutRect(0, 0, 60, 43);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

	std::string content = std::to_string(_item->GetInfo().Id());
	std::wstring widestr = std::wstring(content.begin(), content.end());
	std::wstring temp = L"...";
	if (content.size() > 2)
	{
		widestr = widestr.substr(0, 3) + temp;
	}

	graphics.DrawString(
		widestr.c_str(),
		widestr.length(),
		TLFont::Instance().GetFont(20),
		layoutRect,
		&format,
		&headerBrush);

	graphics.Restore(state);
}

void TLItemVis::DrawControlCollapse(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	graphics.TranslateTransform(62, 34);

	// -------- Draw background -------- //
	Gdiplus::LinearGradientBrush brush1(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 43),
#ifdef _NEW_TIME_LINE
		Gdiplus::Color(255, 145, 145, 145),
		Gdiplus::Color(255, 117, 117, 117)
#else
		Gdiplus::Color(255, 120, 120, 120),
		Gdiplus::Color(255, 200, 200, 200)
#endif
	);
	graphics.FillRectangle(&brush1, 0, 0, 235, 43);
	
#ifdef _NEW_TIME_LINE
	graphics.TranslateTransform(13, 4);
	m_btnBegin.Draw(graphics);

	graphics.TranslateTransform(42, 0);

	m_btnStart.Draw(graphics);

	graphics.TranslateTransform(62, 0);
	m_btnPause.Draw(graphics);
#else
	// -------- Draw backward button -------- //
	graphics.TranslateTransform(13, 4);
	Gdiplus::LinearGradientBrush brush2(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 38),
		Gdiplus::Color(255, 12, 46, 90),
		Gdiplus::Color(255, 112, 146, 190)
	);
	graphics.FillRectangle(&brush2, 0, 0, 38, 38);
	Gdiplus::Image* backwardImage = m_backwardImage.get();
	graphics.DrawImage(backwardImage, 0, 0, 38, 38);

	// -------- Draw play/pause button -------- //
	graphics.TranslateTransform(42, 0);
	Gdiplus::LinearGradientBrush brush3(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 38),
		Gdiplus::Color(255, 12, 46, 90),
		Gdiplus::Color(255, 112, 146, 190)
	);
	graphics.FillRectangle(&brush3, 0, 0, 38, 38);
	Gdiplus::Image* startImage = m_startImage.get();
	graphics.DrawImage(startImage, 0, 0, 38, 38);

	// -------- Draw stop button -------- //
	graphics.TranslateTransform(62, 0);
	Gdiplus::LinearGradientBrush brush4(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, 38),
		Gdiplus::Color(255, 12, 46, 90),
		Gdiplus::Color(255, 112, 146, 190)
	);
	graphics.FillRectangle(&brush4, 0, 0, 38, 38);
	Gdiplus::Image* stopImage = m_stopImage.get();
	graphics.DrawImage(stopImage, 0, 0, 38, 38);
#endif

	graphics.Restore(state);
}

void TLItemVis::DrawTotalTimeCollapse(Gdiplus::Graphics& graphics)
{
	// do nothing
}

void TLItemVis::DrawProgressCollapse(Gdiplus::Graphics& graphics)
{
	// do nothing
}

void TLItemVis::DrawFocusBorder(Gdiplus::Graphics& graphics)
{
	Gdiplus::Pen pen(Gdiplus::Color(255, 239, 186, 40), 3.0f);
	graphics.DrawRectangle(&pen, 0, 0, m_rect.Width + 3, m_rect.Height + 3);
}

void TLItemVis::SetExpandRegion()
{
	_expanderRegion.X = 0;
	_expanderRegion.Y = 0;
	_expanderRegion.Width = 30;
	_expanderRegion.Height = 30;

	_titleRegion.X = 30;
	_titleRegion.Y = 0;
	_titleRegion.Width = m_rect.Width - 30;
	_titleRegion.Height = 30;

	_idRegion.X = 0;
	_idRegion.Y = 30;
	_idRegion.Width = 60;
	_idRegion.Height = 60;

	_controlRegion.X = 0;
	_controlRegion.Y = 90;
	_controlRegion.Width = 171;
	_controlRegion.Height = m_rect.Height - 90;

	m_btnBegin.SetRegion(_controlRegion.X + 15 + m_rect.X, _controlRegion.Y + 4 + m_rect.Y,38,38);
	m_btnStart.SetRegion(_controlRegion.X + 57 + m_rect.X, _controlRegion.Y + 4 + m_rect.Y, 38, 38);
	m_btnPause.SetRegion(_controlRegion.X + 119 + m_rect.X, _controlRegion.Y + 4 + m_rect.Y, 38, 38);

	_totalTimeRegion.X = 171;
	_totalTimeRegion.Y = 90;
	_totalTimeRegion.Width = m_rect.Width - 171;
	_totalTimeRegion.Height = m_rect.Height - 90;

	_progressRegion.X = 60;
	_progressRegion.Y = 30;
	_progressRegion.Width = m_rect.Width - 60;
	_progressRegion.Height = 110;

	TLGeometricTransform::Translate(_expanderRegion, m_rect.X, m_rect.Y);
	TLGeometricTransform::Translate(_titleRegion, m_rect.X, m_rect.Y);
	TLGeometricTransform::Translate(_idRegion, m_rect.X, m_rect.Y);
	TLGeometricTransform::Translate(_controlRegion, m_rect.X, m_rect.Y);
	TLGeometricTransform::Translate(_totalTimeRegion, m_rect.X, m_rect.Y);
	TLGeometricTransform::Translate(_progressRegion, m_rect.X, m_rect.Y);
	
}

void TLItemVis::SetCollapseRegion()
{
	_expanderRegion.X = 0;
	_expanderRegion.Y = 0;
	_expanderRegion.Width = 30;
	_expanderRegion.Height = 30;

	_titleRegion.X = 30;
	_titleRegion.Y = 0;
	_titleRegion.Width = m_rect.Width - 30;
	_titleRegion.Height = 30;

	_idRegion.X = 0;
	_idRegion.Y = 30;
	_idRegion.Width = 60;
	_idRegion.Height = 43;

	_controlRegion.X = 60;
	_controlRegion.Y = 30;
	_controlRegion.Width = 60;
	_controlRegion.Height = m_rect.Height - 30;

	m_btnBegin.SetRegion(_controlRegion.X + 15 + m_rect.X, _controlRegion.Y + 4 + m_rect.Y, 38, 38);
	m_btnStart.SetRegion(_controlRegion.X + 57 + m_rect.X, _controlRegion.Y + 4 + m_rect.Y, 38, 38);
	m_btnPause.SetRegion(_controlRegion.X + 119 + m_rect.X, _controlRegion.Y + 4 + m_rect.Y, 38, 38);

	_totalTimeRegion.X = 0;
	_totalTimeRegion.Y = 0;
	_totalTimeRegion.Width = 0;
	_totalTimeRegion.Height = 0;

	_progressRegion.X = 0;
	_progressRegion.Y = 0;
	_progressRegion.Width = 0;
	_progressRegion.Height = 0;

	TLGeometricTransform::Translate(_expanderRegion, m_rect.X, m_rect.Y);
	TLGeometricTransform::Translate(_titleRegion, m_rect.X, m_rect.Y);
	TLGeometricTransform::Translate(_idRegion, m_rect.X, m_rect.Y);
	TLGeometricTransform::Translate(_controlRegion, m_rect.X, m_rect.Y);
	TLGeometricTransform::Translate(_totalTimeRegion, m_rect.X, m_rect.Y);
	TLGeometricTransform::Translate(_progressRegion, m_rect.X, m_rect.Y);
}

bool TLItemVis::CheckButtonStatus()
{
	auto& collection = m_buttonCollection;
	for (auto o : collection)
	{
		if (o->CheckStatus())
			return true;
	}
	return false;
}

int TLItemVis::GetStatus()
{
	return m_iStatus;
}

void TLItemVis::SetStatus(int iStatus)
{
	m_iStatus = iStatus;
}
