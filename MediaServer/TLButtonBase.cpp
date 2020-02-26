/********************************************************************\
Project Name: Timeline Prototype

File Name: TLButtonBase.cpp

Definition of following classes:
TLButtonBase

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
#include "TLButtonBase.h"

#define ENGINNERING_MODE

using namespace Timeline;

/* ====================================================== *\
TLButtonBase
\* ====================================================== */

TLButtonBase::TLButtonBase()
{
	m_rect.X = 0;
	m_rect.Y = 0;
	m_rect.Width = 30;
	m_rect.Height = 30;

	m_hasImage = false;
}

TLButtonBase::~TLButtonBase()
{

}

void TLButtonBase::SetImage(const std::string& filename)
{
	std::wstring widestr = std::wstring(filename.begin(), filename.end());
	m_image = std::make_shared<Gdiplus::Image>(widestr.c_str());
	m_hasImage = true;
}

void TLButtonBase::Draw(Gdiplus::Graphics& graphics)
{
	auto& point = m_mouseEvent.MousePosition;

	Pen selectPen(Color::Orange, 1); //Red
	Pen deselectPen(Color(255,150, 150, 50), 1);
	SolidBrush selectBrush(Color::Orange); //Red
	SolidBrush deselectBrush(Color(255, 150, 150, 50)); //Color::SteelBlue

	Pen* pen = &deselectPen;
	SolidBrush* brush = &deselectBrush;

	if (IsSelect())
	{
		pen = &selectPen;
		brush = &selectBrush;
	}

	if (HasCollision(point.X, point.Y) || IsSelect())
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

TLBmpButtonBase::TLBmpButtonBase()
{
	m_rect.X = 0;
	m_rect.Y = 0;
	m_rect.Width = 30;
	m_rect.Height = 30;
}

TLBmpButtonBase::~TLBmpButtonBase()
{
}

void TLBmpButtonBase::SetImage(int iIndex,const std::string& filename)
{
	std::wstring widestr = std::wstring(filename.begin(), filename.end());
	switch (iIndex)
	{
		case 0:
			{
				m_normal_image = std::make_shared<Gdiplus::Image>(widestr.c_str());

				//Gdiplus::Image* image = m_normal_image.get();
				//m_rect.Width = image->GetWidth();
				//m_rect.Height = image->GetHeight();
			}
			break;
		case 1:
			m_active_image = std::make_shared<Gdiplus::Image>(widestr.c_str());
			break;
		case 2:
			m_down_image = std::make_shared<Gdiplus::Image>(widestr.c_str());
			break;
		case 3:
			m_disable_image = std::make_shared<Gdiplus::Image>(widestr.c_str());
			break;
	}

	//m_hasImage = true;
}

void TLBmpButtonBase::Draw(Gdiplus::Graphics& graphics)
{
	auto& point = m_mouseEvent.MousePosition;
	Gdiplus::Image* image;
	if (IsSelect())
		image = m_down_image.get();
	else if (HasCollision(point.X, point.Y))
		image = m_active_image.get();
	else
		image = m_normal_image.get();
	graphics.DrawImage(image, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height);
}

void TLBmpButtonBase::DrawActive(Gdiplus::Graphics& graphics)
{
	Gdiplus::Image* image = m_active_image.get();
	graphics.DrawImage(image, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height);
}

void TLBmpButtonBase::DrawDown(Gdiplus::Graphics& graphics)
{
	Gdiplus::Image* image = m_down_image.get();
	graphics.DrawImage(image, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height);
}

void TLBmpButtonBase::DrawNormal(Gdiplus::Graphics& graphics)
{
	Gdiplus::Image* image = m_normal_image.get();
	graphics.DrawImage(image, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height);
}