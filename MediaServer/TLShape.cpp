/********************************************************************\
Project Name: Timeline Prototype

File Name: TLShape.cpp

Definition of following classes:
TLShape

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
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>

// ==== Include local library ==== //
#include "TLShape.h"

using namespace Timeline;

/* ====================================================== *\
TLShape
\* ====================================================== */
TLShape::TLShape()
{
	m_uuid = boost::uuids::to_string(boost::uuids::random_generator()());
	m_rect.X = 0;
	m_rect.Y = 0;
	m_rect.Width = 100;
	m_rect.Height = 100;

	m_mouseEvent.EventType = MouseEventType::None;
	m_mouseEvent.MousePosition = Gdiplus::Point(0, 0);
	m_mouseEvent.MouseOffset = Gdiplus::Point(0, 0);
	m_mouseEvent.ZDelta = 0;

	m_isSelected = false;
	m_isDragged = false;
	m_isDraggable = false;
}

TLShape::~TLShape()
{

}

std::string TLShape::UUID() const
{
	return m_uuid;
}

void TLShape::SetUUID(const std::string& uuid)
{
	m_uuid = uuid;
}

std::string& TLShape::Text()
{
	return m_text;
}

void TLShape::SetText(const std::string& text)
{
	m_text = text;
}

void TLShape::MouseLeftButtonDown(int x, int y)
{
	this->m_mouseEvent.MousePosition.X = x;
	this->m_mouseEvent.MousePosition.Y = y;
	this->m_mouseEvent.EventType = MouseEventType::LeftButtonDown;
}

void TLShape::MouseLeftButtonDown(const Gdiplus::Point& point)
{
	this->MouseLeftButtonDown(point.X, point.Y);
}

void TLShape::MouseLeftButtonUp(int x, int y)
{
	this->m_mouseEvent.MousePosition.X = x;
	this->m_mouseEvent.MousePosition.Y = y;
	this->m_mouseEvent.EventType = MouseEventType::LeftButtonUp;
}

void TLShape::MouseLeftButtonUp(const Gdiplus::Point& point)
{
	this->MouseLeftButtonUp(point.X, point.Y);
}

void TLShape::MouseRightButtonDown(int x, int y)
{
	this->m_mouseEvent.MousePosition.X = x;
	this->m_mouseEvent.MousePosition.Y = y;
	this->m_mouseEvent.EventType = MouseEventType::RightButtonDown;
}

void TLShape::MouseRightButtonDown(const Gdiplus::Point& point)
{
	this->MouseRightButtonDown(point.X, point.Y);
}

void TLShape::MouseRightButtonUp(int x, int y)
{
	this->m_mouseEvent.MousePosition.X = x;
	this->m_mouseEvent.MousePosition.Y = y;
	this->m_mouseEvent.EventType = MouseEventType::RightButtonUp;
}

void TLShape::MouseRightButtonUp(const Gdiplus::Point& point)
{
	this->MouseRightButtonUp(point.X, point.Y);
}

void TLShape::MouseMiddleButtonDown(int x, int y)
{
	this->m_mouseEvent.MousePosition.X = x;
	this->m_mouseEvent.MousePosition.Y = y;
	this->m_mouseEvent.EventType = MouseEventType::MiddleButtonDown;
}

void TLShape::MouseMiddleButtonDown(const Gdiplus::Point& point)
{
	this->MouseMiddleButtonDown(point.X, point.Y);
}

void TLShape::MouseMiddleButtonUp(int x, int y)
{
	this->m_mouseEvent.MousePosition.X = x;
	this->m_mouseEvent.MousePosition.Y = y;
	this->m_mouseEvent.EventType = MouseEventType::MiddleButtonUp;
}

void TLShape::MouseMiddleButtonUp(const Gdiplus::Point& point)
{
	this->MouseMiddleButtonUp(point.X, point.Y);
}

void TLShape::MouseMove(int x, int y)
{
	this->m_mouseEvent.MousePosition.X = x;
	this->m_mouseEvent.MousePosition.Y = y;
	this->m_mouseEvent.EventType = MouseEventType::Move;
}

void TLShape::MouseMove(const Gdiplus::Point& point)
{
	this->MouseMove(point.X, point.Y);
}

void TLShape::MouseWheel(short zDelta, int x, int y)
{
	this->m_mouseEvent.MousePosition.X = x;
	this->m_mouseEvent.MousePosition.Y = y;
	this->m_mouseEvent.EventType = MouseEventType::Wheel;
	this->m_mouseEvent.ZDelta = zDelta;
}

void TLShape::MouseWheel(short zDelta, const Gdiplus::Point& point)
{
	this->MouseWheel(zDelta, point.X, point.Y);
}

Gdiplus::Point& TLShape::MouseOffset()
{
	return this->m_mouseEvent.MouseOffset;
}

void TLShape::SetMouseOffset(int x, int y)
{
	this->m_mouseEvent.MouseOffset.X = m_rect.X - x;
	this->m_mouseEvent.MouseOffset.Y = m_rect.Y - y;
}

void TLShape::SetMouseOffset(const Gdiplus::Point& point)
{
	this->SetMouseOffset(point.X, point.Y);
}

void TLShape::KeyDown(unsigned int key)
{

}

Gdiplus::Point TLShape::Position()
{
	return Point(m_rect.X, m_rect.Y);
}

void TLShape::SetPosition(int x, int y)
{
	m_rect.X = x;
	m_rect.Y = y;
}

void TLShape::SetPosition(const Gdiplus::Point& point)
{
	this->SetPosition(point.X, point.Y);
}


Rect& TLShape::GetRect()
{
	return m_rect;
}

void TLShape::SetRect(int x, int y, int width, int height)
{
	m_rect.X = x;
	m_rect.Y = y;
	m_rect.Width = width;
	m_rect.Height = height;
}

void TLShape::SetRect(const Gdiplus::Rect& rect)
{
	this->SetRect(rect.X, rect.Y, rect.Width, rect.Height);
}

int TLShape::Width()
{
	return m_rect.Width;
}

void TLShape::SetWidth(int width)
{
	m_rect.Width = width;
}

int TLShape::Height()
{
	return m_rect.Height;
}


void TLShape::SetHeight(int height)
{
	m_rect.Height = height;
}

void TLShape::SetSize(int width, int height)
{
	m_rect.Width = width;
	m_rect.Height = height;
}


bool TLShape::HasCollision(int x, int y)
{
	return this->HasCollision(Point(x, y));
}

bool TLShape::HasCollision(const Gdiplus::Point& point)
{
	if (m_rect.Contains(point))
	{
		return true;
	}
	return false;
}

void TLShape::Drag()
{
	m_isDragged = true;
}

void TLShape::UnDragged()
{
	m_isDragged = false;
}

bool TLShape::IsDragged()
{
	return m_isDragged;
}

bool TLShape::IsDraggable()
{
	return m_isDraggable;
}

void TLShape::EnableDrag()
{
	m_isDraggable = true;
}

void TLShape::DisableDrag()
{
	m_isDraggable = false;
}

void TLShape::Select()
{
	m_isSelected = true;
}

void TLShape::Deselect()
{
	m_isSelected = false;
}

bool TLShape::IsSelect()
{
	return m_isSelected;
}


std::string& TLShape::Tag()
{
	return m_tag;
}

void TLShape::SetTag(const std::string& tag)
{
	m_tag = tag;
}

void TLShape::Draw(Gdiplus::Graphics& graphics)
{

}

