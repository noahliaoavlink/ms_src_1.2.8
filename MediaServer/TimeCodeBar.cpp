/********************************************************************\
Project Name: Timeline Prototype

File Name: TimeCodeBar.cpp

Definition of following classes:
TimeCodeBar

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
#include "TimeCode.h"
#include "TimeCodeBar.h"
#include "ServiceProvider.h"

#define ENGINNERING_MODE

using namespace Timeline;

/* ====================================================== *\
TimeCodeBar
\* ====================================================== */

TimeCodeBar::TimeCodeBar()
{
	m_rect.X = 0;
	m_rect.Y = 0;
	m_rect.Width = 1000;
	m_rect.Height = 30;
	m_horizontal_scroll_shift = 0;
}

TimeCodeBar::~TimeCodeBar()
{

}

void TimeCodeBar::SetOffset(int iOffset)
{
	m_horizontal_scroll_shift = iOffset;
}

int TimeCodeBar::GetOffset()
{
	return m_horizontal_scroll_shift;
}

void TimeCodeBar::MouseLeftButtonDown(const Gdiplus::Point& point)
{
	this->m_mouseEvent.MousePosition = point;

	bool hasCollision = this->HasCollision(m_mouseEvent.MousePosition);

	if (hasCollision)
	{
		this->Select();
		if (!this->IsDragged())
		{
			this->m_lastPoint = this->m_mouseEvent.MousePosition;
			this->SetMouseOffset(this->m_mouseEvent.MousePosition);
			this->Drag();
		}
		else
		{
			this->UnDragged();
		}
	}
	else
	{
		this->UnDragged();
		this->Deselect();
	}
}

void TimeCodeBar::MouseLeftButtonUp(const Gdiplus::Point& point)
{
	this->UnDragged();
	this->Deselect();
}

void TimeCodeBar::MouseMove(const Gdiplus::Point& point)
{
	if (this->IsDragged())
	{
		auto temp = m_horizontal_scroll_shift;
		temp += point.X - this->m_lastPoint.X;
		if (temp <= 0)
		{
			m_horizontal_scroll_shift = temp;

			auto service = ServiceProvider::Instance()->GetTimelineService();
			if (service != nullptr)
			{
				auto manager = service->GetTimelineManager();
				auto timecode = TimeCode(std::abs(m_horizontal_scroll_shift) * TimelineManager::PixelToMsFactor);
				manager->SetBaseTimeCode(timecode);
#ifdef ENGINNERING_MODE
#if false
				std::string dumpMsg = boost::str(boost::format("TimeCodeBar::TimeCode=%s") % manager->GetBaseTimeCode().ToString());
				DumpToLog(dumpMsg);
#endif
#endif // ENGINNERING_MODE
				
			}
		}

#ifdef ENGINNERING_MODE
#if false
		std::string dumpMsg = boost::str(boost::format("TimeCodeBar::m_horizontal_scroll_shift=%d") % m_horizontal_scroll_shift);
		DumpToLog(dumpMsg);
#endif
#endif // ENGINNERING_MODE
	}

	this->m_lastPoint = point;
}

void TimeCodeBar::Draw(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	Region region(m_rect);
	graphics.SetClip(&region, CombineModeReplace);

	// -------- Draw background -------- //
	SolidBrush brush(Color(255, 36, 36, 36));
	graphics.FillRectangle(&brush, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height);
	
	// -------- Draw timecode -------- //
	// TODO: It is a dummy timecode, it should be replace later.

#if 0
	TimeCode timecode(0, 0, 0, 0);
	for (int i = 0; i < 100; i++ )  //86400 , 100
	{
		DrawTimeCode(graphics,
			timecode.ToString(),
			Point(
				//m_rect.X + VisualConstant::BaseXShift + i * 100 + m_horizontal_scroll_shift,
				m_rect.X + i * 100 + m_horizontal_scroll_shift,
				m_rect.Y + VisualConstant::BaseYShift));
		timecode = timecode + TimeCode(0, 0, 1, 0);
	}
#else
	int iOffset = -(m_horizontal_scroll_shift / 100);
	int iHours = iOffset / 3600;
	int iTmp = iOffset - (iHours * 3600);
	int iMinutes = iTmp / 60;
	int iSecs = iOffset - (iHours * 3600) - iMinutes * 60;
	TimeCode timecode(iHours, iMinutes, iSecs, 0);
	for (int i = 0; i < 100; i++)  
	{
		DrawTimeCode(graphics,
			timecode.ToString(),
			Point(
				//m_rect.X + VisualConstant::BaseXShift + i * 100 + m_horizontal_scroll_shift,
				m_rect.X + i * 100,
				m_rect.Y + VisualConstant::BaseYShift));
		timecode = timecode + TimeCode(0, 0, 1, 0);
	}
#endif
	graphics.Restore(state);
}

void TimeCodeBar::DrawTimeCode(Gdiplus::Graphics& graphics, const std::string& text, const Gdiplus::Point& point)
{
	//Gdiplus::SolidBrush brush(Gdiplus::Color::SteelBlue);
	Gdiplus::SolidBrush brush(Gdiplus::Color(255,239,186,40));
	//Gdiplus::RectF rect(point.X - 25, point.Y - 5, 100, 10);
	Gdiplus::RectF rect(point.X, point.Y - 5, 100, 10);
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

	// Draw ruler line
	//Gdiplus::Pen pen(Gdiplus::Color::White, 1.0f);
	Gdiplus::Pen pen(Gdiplus::Color(255, 239, 186, 40), 1.0f);
	graphics.DrawLine(&pen, point.X, point.Y + 10, point.X, point.Y + 15);
}
