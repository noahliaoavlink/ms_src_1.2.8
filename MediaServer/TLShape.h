/********************************************************************\
Project Name: Timeline Prototype

File Name: TLShape.h

Declaration of following classes:
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

#pragma once

// ==== Inlcude Microsoft MFC library ==== //
#include "stdafx.h"

// ==== Inlcude STL library ==== //
#include <string>
#include <stack>

// ==== Include local library ==== //
#include "TLDllExport.h"

namespace Timeline
{

	/* ====================================================== *\
	TLShape
	\* ====================================================== */
	class TIMELINELIB_EXPORTS TLShape
	{
		// ==== Enumeration ==== //
		enum class MouseEventType
		{
			None = 0,
			LeftButtonDown = 1,
			LeftButtonUp = 2,
			RightButtonDown = 3,
			RightButtonUp = 4,
			MiddleButtonDown = 5,
			MiddleButtonUp = 6,
			Wheel = 7,
			Move = 8
		};
	public:
		// ==== Constants ==== //
		struct MouseEvent
		{
			MouseEventType EventType;
			Gdiplus::Point MousePosition;
			Gdiplus::Point MouseOffset;
			short ZDelta;
		};

		// ==== Constructor & Destructor ==== //
	public:
		TLShape();
		virtual ~TLShape();

		// ==== Public member methods ==== //
	public:
		virtual std::string UUID() const;
		virtual void SetUUID(const std::string& uuid);

		virtual std::string& Text();
		virtual void SetText(const std::string& text);

		void MouseLeftButtonDown(int x, int y);
		void MouseLeftButtonDown(const Gdiplus::Point& point);
		void MouseLeftButtonUp(int x, int y);
		void MouseLeftButtonUp(const Gdiplus::Point& point);

		void MouseRightButtonDown(int x, int y);
		void MouseRightButtonDown(const Gdiplus::Point& point);
		void MouseRightButtonUp(int x, int y);
		void MouseRightButtonUp(const Gdiplus::Point& point);

		void MouseMiddleButtonDown(int x, int y);
		void MouseMiddleButtonDown(const Gdiplus::Point& point);
		void MouseMiddleButtonUp(int x, int y);
		void MouseMiddleButtonUp(const Gdiplus::Point& point);

		void MouseMove(int x, int y);
		void MouseMove(const Gdiplus::Point& point);

		void MouseWheel(short zDelta, int x, int y);
		void MouseWheel(short zDelta, const Gdiplus::Point& point);

		virtual Gdiplus::Point& MouseOffset();
		virtual void SetMouseOffset(int x, int y);
		virtual void SetMouseOffset(const Gdiplus::Point& point);

		void KeyDown(unsigned int key);

		virtual Gdiplus::Point Position();
		virtual void SetPosition(int x, int y);
		virtual void SetPosition(const Gdiplus::Point& point);

		virtual Rect& GetRect();
		virtual void SetRect(int x, int y, int width, int height);
		virtual void SetRect(const Gdiplus::Rect& rect);

		virtual int Width();
		virtual void SetWidth(int width);
		virtual int Height();
		virtual void SetHeight(int height);
		virtual void SetSize(int width, int height);

		virtual bool HasCollision(int x, int y);
		virtual bool HasCollision(const Gdiplus::Point& point);

		virtual void Drag();
		virtual void UnDragged();
		virtual bool IsDragged();
		virtual bool IsDraggable();
		virtual void EnableDrag();
		virtual void DisableDrag();

		virtual void Select();
		virtual void Deselect();
		virtual bool IsSelect();

		virtual std::string& Tag();
		virtual void SetTag(const std::string& tag);

		virtual void Draw(Gdiplus::Graphics& graphics);

		// ==== Protected member fields ==== //
	protected:
		std::string m_uuid;
		Gdiplus::Rect m_rect;
		MouseEvent m_mouseEvent;

		bool m_isSelected;
		bool m_isDragged;
		bool m_isDraggable;

		std::string m_text;
		std::string m_tag;

		std::stack<MouseEvent> m_eventStack;
	};

}