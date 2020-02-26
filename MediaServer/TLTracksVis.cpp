/********************************************************************\
Project Name: Timeline Prototype

File Name: TLTrackVis.cpp

Definition of following classes:
TLTrackVis

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
#include <string>
#include <boost/format.hpp>

// ==== Include local library ==== //
#include "TLUtility.h"
#include "TLConstant.h"
#include "TLTracksVis.h"
#include "ServiceProvider.h"
#include "EditCommandDlg.h"
#include "EditEffectIndexDlg.h"
#include "MediaServer.h"
#include "MoveToDlg.h"
#include "UIController.h"
#include "AudioConfigDlg.h"

#define ENGINNERING_MODE

using namespace Timeline;

extern UIController* g_pUIController;

enum MovingOperate
{
	MOP_NONE = 0,
	MOP_ADD,
	MOP_SUB,
	MOP_ANY,
};

/* ====================================================== *\
TLTrackEventVis
\* ====================================================== */
TLTrackEventVis::TLTrackEventVis()
{
	m_rect.X = 0;
	m_rect.Y = 0;
	m_rect.Width = VisualConstant::Width;
	m_rect.Height = VisualConstant::Height;
}

TLTrackEventVis::~TLTrackEventVis()
{
}

void TLTrackEventVis::Initialize(TLTrackEventPtr& event)
{
	m_event = event;
	m_uuid = event->UUID();
}

TLTrackEventPtr& TLTrackEventVis::GetContext()
{
	return m_event;
}

/*
Gdiplus::Point& TLTrackEventVis::Position()
{
return Point(m_rect.X + m_rect.Width / 2, m_rect.Y + m_rect.Height / 2);
}
*/

void TLTrackEventVis::Draw(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();

	Pen selectPen(Color::Red, 1);
	Pen deselectPen(Color::White, 1);
	SolidBrush selectBrush(Color::Red);
	SolidBrush deselectBrush(Color::White);

	Pen* pen = &deselectPen;
	SolidBrush* brush = &deselectBrush;

	if (IsSelect())
	{
		pen = &selectPen;
		brush = &selectBrush;
	}

	if (HasCollision(m_mouseEvent.MousePosition))
	{
		graphics.DrawEllipse(pen, m_rect.X - 2, m_rect.Y - 2, m_rect.Width + 4, m_rect.Height + 4);
	}

	graphics.FillEllipse(brush, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height);

	graphics.Restore(state);
}

/* ====================================================== *\
          TLTrackVis
\* ====================================================== */
TLTrackVis::TLTrackVis()
{
	m_expandStatus = ExpandStatus::Expand;

	m_rect.X = 0;
	m_rect.Y = 0;
	m_rect.Width = VisualConstant::Width;
	m_rect.Height = VisualConstant::ExpandHeight;

	m_iOffset = 0;
	m_iActiveIndex = -1;
	m_iSelectIndex = -1;
	m_bDragged = false;
	m_bResized = false;

//	strcpy(m_track_pos_info.szFullPath, "");
//	m_track_pos_info.fUnit = 0.0;

	m_hArrow = 0;
	m_hResize_H = 0;
	m_hMove = 0;

	m_bUpdate = false;
	m_bIsPressed = false;

	strcpy(m_szTrackFullPath, "");

	SetExpandRegion();
}

TLTrackVis::~TLTrackVis()
{
	if(m_hArrow)
		DestroyCursor(m_hArrow);

	if (m_hResize_H)
		DestroyCursor(m_hResize_H);

	if (m_hMove)
		DestroyCursor(m_hMove);
}

void TLTrackVis::Initialize(TLTrackPtr& track)
{
	m_track = track;
	m_uuid = track->UUID();

	auto track_type = m_track->TrackType();
	std::string sFullPath = TLTrackVis::GetFullPath();

	//strcpy(m_szFullPath, (char*)sFullPath.c_str());

	m_ContentProcessor.Init((char*)sFullPath.c_str(),track_type, m_contentRegion.X);

	m_hArrow = LoadCursor(NULL, IDC_ARROW);

	std::string szResizeCursorFile = theApp.m_strCurPath + "\\UI Folder\\Media\\cursor\\resize_h.cur";
	m_hResize_H = (HCURSOR)LoadCursorFromFile(szResizeCursorFile.c_str());

	std::string szMoveCursorFile = theApp.m_strCurPath + "\\UI Folder\\Media\\cursor\\move.cur";
	m_hMove = (HCURSOR)LoadCursorFromFile(szMoveCursorFile.c_str());
}

TLTrackPtr& TLTrackVis::GetContext()
{
	return m_track;
}

void TLTrackVis::SetPosition(int x, int y)
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

void TLTrackVis::SetOffset(int iValue)
{
	m_iOffset = iValue;
}

void TLTrackVis::MouseLeftButtonDown(const Gdiplus::Point& point)
{
#if 0

	auto pointerMode = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager()->GetPointerMode();

	this->m_mouseEvent.MousePosition = point;

	if (m_expanderRegion.Contains(this->m_mouseEvent.MousePosition))
	{
		this->Select();
	}

	//if (m_contentRegion.Contains(this->m_mouseEvent.MousePosition) && pointerMode == PointerMode::Edit)
	if (m_contentRegion.Contains(this->m_mouseEvent.MousePosition))
	{
		auto trackSpaceMousePosition = MapToTrackVisSpace(this->m_mouseEvent.MousePosition);

		bool hasCollision = this->HasEventCollision(trackSpaceMousePosition);

		std::string name = this->m_track->Name();

		// If there is an object hit by the mouse, then execute the selection or drag/undrag.
		// If there is no object to be hit, then deselect and undrag it all.
		if (hasCollision)
		{

			for (auto obj : m_events)
			{
				obj->Deselect();
				if (obj->HasCollision(trackSpaceMousePosition))
				{
					obj->Select();
					if (!obj->IsDragged())
					{
						obj->SetMouseOffset(trackSpaceMousePosition);
						obj->Drag();
					}
					else
					{
						obj->UnDragged();
					}
				}
			}
		}
		else
		{
			auto track_type = m_track->TrackType();

			// If the add event feature is enabled, a new TLEvent is created.
			if (pointerMode == PointerMode::Insert && (track_type == TLTrackType::Float || track_type == TLTrackType::Int))
			{
				//auto obj = std::make_shared<TLTrackEventVis>();

				auto vis = std::make_shared<TLTrackEventVis>();
				vis->MouseLeftButtonDown(trackSpaceMousePosition);
				vis->SetPosition(trackSpaceMousePosition.X - 5, trackSpaceMousePosition.Y - 5);
				vis->SetSize(10, 10);
#ifdef ENGINNERING_MODE
#if true
				std::string output = boost::str(boost::format("TLTrackVis::(%d, %d, %d, %d)") % vis->Position().X % vis->Position().Y % trackSpaceMousePosition.X % trackSpaceMousePosition.Y);
				DumpToLog(output);
#endif
#endif // ENGINNERING_MODE

				m_events.push_back(vis);
				//std::sort(m_events.begin(), m_events.end(), [](TLTrackEventVisPtr& a, TLTrackEventVisPtr& b) -> bool { return a->Position().X < b->Position().Y; });
			}

			for (auto obj : m_events)
			{
				obj->UnDragged();
				obj->Deselect();
			}
		}
	}

#else
	if (m_expanderRegion.Contains(point))
	{
		if (IsExpand())
			Collapse();
		else
			Expand();

		ChangeExpandStatus();
	}

	_LBDown(point);
#endif
}

void TLTrackVis::MouseLeftButtonUp(const Gdiplus::Point& point)
{
#if 0

	this->m_mouseEvent.MousePosition = point;

	for (auto obj : m_events)
	{
		obj->UnDragged();
	}

#else
	_LBUp(point);
#endif
}

void TLTrackVis::MouseMove(const Gdiplus::Point& point)
{
#if 0
	this->m_mouseEvent.MousePosition = point;
	auto trackSpaceMousePosition = MapToTrackVisSpace(this->m_mouseEvent.MousePosition);

	for (auto obj : m_events)
	{
		if (obj->IsDragged())
		{
			Gdiplus::Point pt(
				trackSpaceMousePosition.X + obj->MouseOffset().X,
				trackSpaceMousePosition.Y + obj->MouseOffset().Y);
			obj->SetPosition(pt);

		}
	}
#else
	_MouseMove(point);
#endif
}

void TLTrackVis::_LBDown(const Gdiplus::Point& point)
{
	if (m_bIsPressed)
		return;

	m_bIsPressed = true;

	m_iClickX = point.X;
	m_iClickY = point.X;
	
	auto pointerMode = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager()->GetPointerMode();
	auto lineMode = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager()->GetLineMode();
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	//std::string fullpath = m_track->GetFullPath();
	std::string fullpath = GetFullPath();
	this->m_mouseEvent.MousePosition = point;

	if (m_expanderRegion.Contains(this->m_mouseEvent.MousePosition))
		this->Select();

	if (m_contentRegion.Contains(this->m_mouseEvent.MousePosition))
	{
		auto track_type = m_track->TrackType();

		int iItemIndex = -1, iTrackIndex = -1;
		int iTackIndexRet = manager->GetTrackContentManager()->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);

		// If the add event feature is enabled, a new TLEvent is created.
		if (pointerMode == PointerMode::Insert_Point) 
		{
			if((track_type == TLTrackType::Float || track_type == TLTrackType::Int))
			{
				int iPos = point.X - m_contentRegion.X;
				float fUnit = (iPos - m_iOffset) / 100.0;

				int iValue = 0;
				int iYPos = point.Y - m_contentRegion.Y;
				if (iYPos == 51)
					iValue = 0;
				else if (iYPos > 51)
					iValue = -(iYPos - 51) * 2;
				else if (iYPos < 51)
					//iValue = iYPos * 2;
					iValue = 100 - iYPos * 2;

				float fLastTime = manager->GetTrackContentManager()->GetLastTime(iItemIndex, iTrackIndex);

				if (fLastTime <= fUnit)
				{
					if (manager->GetPBStatus() != PBC_PLAY)
					{
						manager->GetTrackContentManager()->Add((char*)fullpath.c_str(), fUnit, iValue, (LinkType)lineMode, -90);
						UpdateItemTotalTime();
					}
				}
			}
			else if (track_type == TLTrackType::Effect_Index)
			{
				int iPos = point.X - m_contentRegion.X;
				float fUnit = (iPos - m_iOffset) / 100.0;

				int iIndex = manager->GetTrackContentManager()->FindNode((char*)fullpath.c_str(), fUnit);

				if (iIndex >= 0)
				{
					EffectIndexNode* pCurInxNode = (EffectIndexNode*)manager->GetTrackContentManager()->GetNodeData(iItemIndex, iTrackIndex, iIndex);
					if (pCurInxNode)
					{
						manager->UpdateVideoEffectParamItem(pCurInxNode->iIndex);
					}
				}
			}
			else if (track_type == TLTrackType::Effect)
			{
				int iPos = point.X - m_contentRegion.X;
				float fUnit = (iPos - m_iOffset) / 100.0;

				int iIndex = manager->GetTrackContentManager()->FindNode((char*)fullpath.c_str(), fUnit);

				if (iIndex >= 0)
				{
					EffectNode* pCurEffectNode = (EffectNode*)manager->GetTrackContentManager()->GetNodeData(iItemIndex, iTrackIndex, iIndex);
					if (pCurEffectNode)
					{
						int iRightPos = (pCurEffectNode->fTime + pCurEffectNode->fDuration) * 100.0;
						int iX = point.X - m_contentRegion.X - m_iOffset;
						if (iX >= iRightPos - 2 && iX <= iRightPos + 2)
						{
							if (manager->GetPBStatus() != PBC_PLAY)
							{
								m_bResized = true;
								m_iActiveIndex = iIndex;
								m_iSelectIndex = iIndex;
								m_fNodeTimeOffset = pCurEffectNode->fTime + pCurEffectNode->fDuration - fUnit;
								SetCursor(m_hResize_H);

								CWnd* pEditViewWnd = manager->GetEditViewWnd();
								::SetCapture(pEditViewWnd->GetSafeHwnd());
							}
						}
						else
						{
							m_bResized = false;
							SetCursor(m_hArrow);
						}
					}
					m_iSelectIndex = iIndex;
				}
				else
					m_iSelectIndex = -1;
			}
		}
		else if (pointerMode == PointerMode::Insert_Video)
		{
			if (track_type == TLTrackType::Media)
			{
				int iPos = point.X - m_contentRegion.X;
				float fUnit = (iPos - m_iOffset) / 100.0;

				int iIndex = manager->GetTrackContentManager()->FindNode((char*)fullpath.c_str(), fUnit);

				if (iIndex >= 0)
				{
					MediaNode* pCurMediaNode = (MediaNode*)manager->GetTrackContentManager()->GetNodeData(iItemIndex, iTrackIndex, m_iActiveIndex);
					if (pCurMediaNode)
					{
						int iRightPos = (pCurMediaNode->fTime + pCurMediaNode->fLen) * 100.0;
						int iX = point.X - m_contentRegion.X - m_iOffset;
						if (iX >= iRightPos - 2 && iX <= iRightPos + 2)
						{
							if (manager->GetPBStatus() != PBC_PLAY)
							{
								m_bResized = true;
								m_iActiveIndex = iIndex;
								m_iSelectIndex = iIndex;
								m_fNodeTimeOffset = pCurMediaNode->fTime + pCurMediaNode->fLen - fUnit;
								SetCursor(m_hResize_H);

								CWnd* pEditViewWnd = manager->GetEditViewWnd();
								::SetCapture(pEditViewWnd->GetSafeHwnd());
							}
						}
						else
						{
							m_bResized = false;
							SetCursor(m_hArrow);
						}
					}
					m_iSelectIndex = iIndex;
				}
				else
					m_iSelectIndex = -1;
			}
		}
		else if (pointerMode == PointerMode::Move)
		{
			if (track_type == TLTrackType::Float || track_type == TLTrackType::Int || track_type == TLTrackType::Media || track_type == TLTrackType::Effect)
			{
				int iPos = point.X - m_contentRegion.X;
				float fUnit = (iPos - m_iOffset) / 100.0;

				//int iIndex = manager->GetTrackContentManager()->FindNode((char*)fullpath.c_str(), fUnit);
				int iIndex = manager->GetTrackContentManager()->FindNode_Backward((char*)fullpath.c_str(), fUnit);

				if (track_type == TLTrackType::Media)
				{
					//int iItemIndex = -1, iTrackIndex = -1;
					//int iTackIndexRet = manager->GetTrackContentManager()->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);
					MediaNode* pCurMediaNode = (MediaNode*)manager->GetTrackContentManager()->GetNodeData(iItemIndex, iTrackIndex, iIndex);
					if(pCurMediaNode)
						m_fNodeTimeOffset = pCurMediaNode->fTime - fUnit;
				}
				else if (track_type == TLTrackType::Effect)
				{
					EffectNode* pCurEffectNode = (EffectNode*)manager->GetTrackContentManager()->GetNodeData(iItemIndex, iTrackIndex, iIndex);
					if (pCurEffectNode)
						m_fNodeTimeOffset = pCurEffectNode->fTime - fUnit;
				}

				if (iIndex >= 0)
				{
					m_iActiveIndex = -1;//iIndex;
					m_iSelectIndex = iIndex;
					m_bDragged = true;
					SetCursor(m_hMove);
					CWnd* pEditViewWnd = manager->GetEditViewWnd();
					::SetCapture(pEditViewWnd->GetSafeHwnd());
				}
				else
				{
					m_iActiveIndex = -1;
					m_iSelectIndex = -1;
					m_bDragged = false;
					SetCursor(m_hArrow);
				}
			}
		}
	}
	else
	{
		m_iSelectIndex = -1;
		m_iActiveIndex = -1;
	}
	m_bIsPressed = false;
}

void TLTrackVis::_LBUp(const Gdiplus::Point& point)
{
	auto pointerMode = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager()->GetPointerMode();
	auto lineMode = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager()->GetLineMode();
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	//std::string fullpath = m_track->GetFullPath();
	std::string fullpath = GetFullPath();

	/*
	if (m_contentRegion.Contains(this->m_mouseEvent.MousePosition))
	{
		if (pointerMode == PointerMode::Move)
		{
			if (m_bDragged)
				m_bDragged = false;
		}
	}
	*/

	if (m_bDragged)
	{
		if (m_iClickX != point.X || m_iClickY != point.X)
		{
			int iItemIndex = -1, iTrackIndex = -1;
			auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
			auto track_content_manager = manager->GetTrackContentManager();
			int iTackIndexRet = track_content_manager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);
			track_content_manager->Sort(iItemIndex, iTrackIndex);
		}
	}

	m_bDragged = false;
	m_bResized = false;
	ReleaseCapture();
}

void TLTrackVis::_MouseMove(const Gdiplus::Point& point)
{
	auto pointerMode = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager()->GetPointerMode();
	auto lineMode = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager()->GetLineMode();
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	//std::string fullpath = m_track->GetFullPath();
	std::string fullpath = GetFullPath();

	m_bUpdate = false;

	if (m_contentRegion.Contains(this->m_mouseEvent.MousePosition))
	{
		int iItemIndex = -1, iTrackIndex = -1;
		auto track_content_manager = manager->GetTrackContentManager();

		int iPos = point.X - m_contentRegion.X;
		float fUnit = (iPos - m_iOffset) / 100.0;
		
		int iTackIndexRet = track_content_manager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);
		int iIndex = track_content_manager->FindNode((char*)fullpath.c_str(), fUnit);

		if (m_iActiveIndex != iIndex)
			m_bUpdate = true;

		auto track_type = m_track->TrackType();
		if (pointerMode == PointerMode::Insert_Point)
		{
			if (track_type == TLTrackType::Float || track_type == TLTrackType::Int /*|| track_type == TLTrackType::Media */|| track_type == TLTrackType::Effect && !m_bResized)
			{
				if (iIndex >= 0)
					m_iActiveIndex = iIndex;
				else
					m_iActiveIndex = -1;
			}

			if (m_bResized)
			{
				if (track_type == TLTrackType::Effect)
				{
					EffectNode* pCurEffectNode = (EffectNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, m_iActiveIndex);
					if (pCurEffectNode)
					{
						pCurEffectNode->fDuration = fUnit - pCurEffectNode->fTime + m_fNodeTimeOffset;
						UpdateItemTotalTime();
					}
				}
			}

			if (iIndex >= 0)
			{
				if (track_type == TLTrackType::Effect)
				{
					EffectNode* pCurEffectNode = (EffectNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, m_iActiveIndex);
					if (pCurEffectNode)
					{
						int iRightPos = (pCurEffectNode->fTime + pCurEffectNode->fDuration) * 100.0;
						int iX = point.X - m_contentRegion.X - m_iOffset;
						if (iX >= iRightPos - 2 && iX <= iRightPos + 2)
						{
							if (manager->GetPBStatus() != PBC_PLAY)
								SetCursor(m_hResize_H);
						}
						else
							SetCursor(m_hArrow);
					}
				}
			}
			
		}
		else if (pointerMode == PointerMode::Insert_Video)
		{
			if (track_type == TLTrackType::Media)
			{
				if (!m_bResized)
				{
					if (iIndex >= 0)
						m_iActiveIndex = iIndex;
					else
						m_iActiveIndex = -1;
				}

				if (m_bResized)
				{
					MediaNode* pCurMediaNode = (MediaNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, m_iActiveIndex);
					if (pCurMediaNode)
					{
						pCurMediaNode->fLen = fUnit - pCurMediaNode->fTime + m_fNodeTimeOffset;
						UpdateItemTotalTime();
					}
				}

				if (iIndex >= 0)
				{
					MediaNode* pCurMediaNode = (MediaNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, m_iActiveIndex);
					if (pCurMediaNode)
					{
						int iRightPos = (pCurMediaNode->fTime + pCurMediaNode->fLen) * 100.0;
						int iX = point.X - m_contentRegion.X - m_iOffset;
						if (iX >= iRightPos - 2 && iX <= iRightPos + 2)
						{
							if (manager->GetPBStatus() != PBC_PLAY)
								SetCursor(m_hResize_H);
						}
						else
							SetCursor(m_hArrow);
					}
				}

			}
		}
		else if (pointerMode == PointerMode::Move)
		{
			if (m_bDragged)
			{
				if ((track_type == TLTrackType::Float || track_type == TLTrackType::Int))
				{
					int iValue = 0;
					int iYPos = point.Y - m_contentRegion.Y;
					if (iYPos == 51)
						iValue = 0;
					else if (iYPos > 51)
						iValue = -(iYPos - 51) * 2;
					else if (iYPos < 51)
						//iValue = iYPos * 2;
						iValue = 100 - iYPos * 2;

					//int iRet = track_content_manager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);
					if (iTackIndexRet != -1)
					{
						if (manager->GetPBStatus() != PBC_PLAY)
						{
							int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
							if (iTotalOfNodes > m_iActiveIndex && m_iActiveIndex != -1)
							{
								NumberNode* pCurNumberNode = (NumberNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, m_iActiveIndex);
								if (pCurNumberNode)
								{
									NumberNode* pPreviousNumberNode = 0;
									NumberNode* pNextNumberNode = 0;
									if (m_iActiveIndex > 0)
										pPreviousNumberNode = (NumberNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, m_iActiveIndex - 1);

									if (m_iActiveIndex < iTotalOfNodes)
										pNextNumberNode = (NumberNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, m_iActiveIndex + 1);

									if (pPreviousNumberNode && pPreviousNumberNode->fTime > fUnit)
											return;

									if (pNextNumberNode && pNextNumberNode->fTime < fUnit)
										return;

									pCurNumberNode->fTime = fUnit;
									pCurNumberNode->iValue = iValue;
									//(LinkType)lineMode, -90;
									UpdateItemTotalTime();
								}
							}
						}
					}
				}
				else if (track_type == TLTrackType::Media)
				{
					if (iTackIndexRet != -1)
					{
						if (manager->GetPBStatus() != PBC_PLAY)
						{
							int iIndex = track_content_manager->FindNode_Backward((char*)fullpath.c_str(), fUnit);

							int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
							if (iTotalOfNodes > m_iSelectIndex && m_iSelectIndex != -1)
							{
									MediaNode* pCurMediaNode = (MediaNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, m_iSelectIndex);
									if (pCurMediaNode && fUnit + m_fNodeTimeOffset >= 0.0)
									pCurMediaNode->fTime = fUnit + m_fNodeTimeOffset;

								UpdateItemTotalTime();
							}
						}
					}
				}
				else if (track_type == TLTrackType::Effect)
				{
					if (iTackIndexRet != -1)
					{
						if (manager->GetPBStatus() != PBC_PLAY)
						{
							int iIndex = track_content_manager->FindNode_Backward((char*)fullpath.c_str(), fUnit);

							int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
							if (iTotalOfNodes > m_iSelectIndex && m_iSelectIndex != -1)
							{
								EffectNode* pCurEffectNode = (EffectNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, m_iSelectIndex);
								if (pCurEffectNode && fUnit + m_fNodeTimeOffset >= 0.0)
									pCurEffectNode->fTime = fUnit + m_fNodeTimeOffset;

								UpdateItemTotalTime();
							}
						}
					}
				}
			}
			else
			{
				if (iIndex >= 0)
				{
					m_iActiveIndex = iIndex;
					SetCursor(m_hMove);
				}
				else
				{
					m_iActiveIndex = -1;
					SetCursor(m_hArrow);
				}
			}
		}
	}
	else
	{
		m_iActiveIndex = -1;
	}
}

void TLTrackVis::MouseRButtonUp(const Gdiplus::Point& point)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto pointerMode = manager->GetPointerMode();
	std::string name = this->m_track->Name();
	auto track_type = m_track->TrackType();
	//std::string fullpath = m_track->GetFullPath();
	std::string fullpath = GetFullPath();
	int iPos = point.X - m_contentRegion.X;
	float fUnit = (iPos - m_iOffset) / 100.0;
	TrackContentManager* pTrackContentManager = manager->GetTrackContentManager();
	int iItemIndex, iTrackIndex;
	int iRet = pTrackContentManager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);
	pTrackContentManager->Sort(iItemIndex, iTrackIndex);
	int iIndex = pTrackContentManager->FindNode((char*)fullpath.c_str(), fUnit);

	if (m_contentRegion.Contains(point))
	{
		if (pointerMode == PointerMode::Insert_Point)
		{
			if ((track_type == TLTrackType::Float || track_type == TLTrackType::Int))
			{
				bool bIsCurve = false;
				//int iItemIndex, iTrackIndex;
				int iRet = pTrackContentManager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);
				if (iRet != -1)
				{
					NumberNode* pCurNumberNode = (NumberNode*)pTrackContentManager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
					if (pCurNumberNode)
					{
						if (pCurNumberNode->iLinkType == LT_CURVE)
							bIsCurve = true;
					}
				}

				if (iIndex >= 0)
				{
					/*
					if (bIsCurve)
						PopupMenu(PMT_DEL_POINT2, point);
					else
						PopupMenu(PMT_DEL_POINT, point);
						*/
				}
			}
			else if (track_type == TLTrackType::Command)
			{
				/*
				if (iIndex >= 0)
					PopupMenu(PMT_DEL_CMD, point);
				else
					PopupMenu(PMT_INSERT_CMD, point);
					*/
			}
			else if (track_type == TLTrackType::Effect_Index)
			{
				/*
				if (iIndex >= 0)
					PopupMenu(PMT_DEL_EFFECT_INDEX, point);
				else
					PopupMenu(PMT_INSERT_EFFECT_INDEX, point);
					*/
			}
		}
		else if (pointerMode == PointerMode::Insert_Video)
		{
			if (strcmp(name.c_str(), "Source") == 0 || track_type == TLTrackType::Media)
			{
				TrackPosInfo track_pos_info;
				strcpy(track_pos_info.szFullPath, fullpath.c_str());
				track_pos_info.fUnit = fUnit;

				pTrackContentManager->SetTrackPosInfo(&track_pos_info);

				m_iSelectIndex = iIndex;
				/*
				if (iIndex >= 0)
				PopupMenu(PMT_DEL_SRC, point);
				else
				PopupMenu(PMT_INSERT_SRC, point);
				*/
			}
		}
		else if (pointerMode == PointerMode::Move)
		{
			if (strcmp(name.c_str(), "Source") == 0 || track_type == TLTrackType::Media)
			{
				TrackPosInfo track_pos_info;
				strcpy(track_pos_info.szFullPath, fullpath.c_str());
				track_pos_info.fUnit = fUnit;

				pTrackContentManager->SetTrackPosInfo(&track_pos_info);

				m_iSelectIndex = iIndex;
				/*
				if (iIndex >= 0)
					//PopupMenu(PMT_MOVE_TO_BEGINNING, point);
					PopupMenu(PMT_MOVE_TO, point);
					*/
			}
		}
	}
	else
	{
		m_iSelectIndex = -1;
		m_iActiveIndex = -1;
	}
}

void TLTrackVis::MouseRButtonUp2(const Gdiplus::Point& point)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto pointerMode = manager->GetPointerMode();
	std::string name = this->m_track->Name();
	auto track_type = m_track->TrackType();
	//std::string fullpath = m_track->GetFullPath();
	std::string fullpath = GetFullPath();
	int iPos = point.X - m_contentRegion.X;
	float fUnit = (iPos - m_iOffset) / 100.0;
	TrackContentManager* pTrackContentManager = manager->GetTrackContentManager();

	int iItemIndex, iTrackIndex;
	int iRet = pTrackContentManager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);
	int iIndex = pTrackContentManager->FindNode((char*)fullpath.c_str(), fUnit);

	if (m_contentRegion.Contains(point))
	{
		if (pointerMode == PointerMode::Insert_Point)
		{
			if ((track_type == TLTrackType::Float || track_type == TLTrackType::Int))
			{
				bool bIsCurve = false;
				//int iItemIndex, iTrackIndex;
				int iRet = pTrackContentManager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);
				if (iRet != -1)
				{
					NumberNode* pCurNumberNode = (NumberNode*)pTrackContentManager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
					if (pCurNumberNode)
					{
						if (pCurNumberNode->iLinkType == LT_CURVE)
							bIsCurve = true;
					}
				}

				if (iIndex >= 0)
				{
					if (bIsCurve)
						PopupMenu(PMT_DEL_POINT2, point);
					else
						PopupMenu(PMT_DEL_POINT, point);
				}
			}
			else if (track_type == TLTrackType::Command)
			{
				if (iIndex >= 0)
					PopupMenu(PMT_DEL_CMD, point);
				else
					PopupMenu(PMT_INSERT_CMD, point);
			}
			else if (track_type == TLTrackType::Effect_Index)
			{
				if (iIndex >= 0)
					PopupMenu(PMT_DEL_EFFECT_INDEX, point);
				else
					PopupMenu(PMT_INSERT_EFFECT_INDEX, point);
			}
			else if (track_type == TLTrackType::Effect)
			{
				TrackPosInfo track_pos_info;
				strcpy(track_pos_info.szFullPath, fullpath.c_str());
				track_pos_info.fUnit = fUnit;

				pTrackContentManager->SetTrackPosInfo(&track_pos_info);

				if (iIndex >= 0)
					PopupMenu(PMT_DEL_EFFECT, point);
				else
					PopupMenu(PMT_INSERT_EFFECT, point);
			}
			else if (track_type == TLTrackType::AudioConfig)
			{
				TrackPosInfo track_pos_info;
				strcpy(track_pos_info.szFullPath, fullpath.c_str());
				track_pos_info.fUnit = fUnit;

				pTrackContentManager->SetTrackPosInfo(&track_pos_info);

				PopupMenu(PMT_CONFIG_AUDIO, point);
			}
		}
		else if (pointerMode == PointerMode::Insert_Video)
		{
			if (strcmp(name.c_str(), "Source") == 0 || track_type == TLTrackType::Media)
			{
				TrackPosInfo track_pos_info;
				strcpy(track_pos_info.szFullPath, fullpath.c_str());
				track_pos_info.fUnit = fUnit;

				pTrackContentManager->SetTrackPosInfo(&track_pos_info);

				m_iSelectIndex = iIndex;

				if (iIndex >= 0)
					PopupMenu(PMT_DEL_SRC, point);
				else
					PopupMenu(PMT_INSERT_SRC, point);
			}
			else if (track_type == TLTrackType::AudioConfig)
			{
				TrackPosInfo track_pos_info;
				strcpy(track_pos_info.szFullPath, fullpath.c_str());
				track_pos_info.fUnit = fUnit;

				pTrackContentManager->SetTrackPosInfo(&track_pos_info);

				PopupMenu(PMT_CONFIG_AUDIO, point);
			}
		}
		else if (pointerMode == PointerMode::Move)
		{
			if (strcmp(name.c_str(), "Source") == 0 || track_type == TLTrackType::Media)
			{
				TrackPosInfo track_pos_info;
				strcpy(track_pos_info.szFullPath, fullpath.c_str());
				track_pos_info.fUnit = fUnit;

				pTrackContentManager->SetTrackPosInfo(&track_pos_info);

				if (iIndex >= 0)
					//PopupMenu(PMT_MOVE_TO_BEGINNING, point);
					PopupMenu(PMT_MOVE_TO, point);
			}
			else if (track_type == TLTrackType::Effect)
			{
				if (iIndex >= 0)
					PopupMenu(PMT_EFFECT_MOVE_TO, point);
			}
		}
	}
}

void TLTrackVis::Expand()
{
	m_expandStatus = ExpandStatus::Expand;
	m_rect.Height = VisualConstant::Height;
	SetExpandRegion();
}

void TLTrackVis::Collapse()
{
	m_expandStatus = ExpandStatus::Collapse;
	m_rect.Height = VisualConstant::CollapseHeight;
	SetCollapseRegion();
}

bool TLTrackVis::IsExpand()
{
	return (m_expandStatus == ExpandStatus::Expand);
}

void TLTrackVis::KeyDownDelete()
{
	auto& collection = m_events;
	TrackEventVisVector temp; // Storage for objects who will be deleted;

							  // Remove elements from event collection.
	for (auto iter = collection.begin(); iter != collection.end(); iter++)
	{
		TLTrackEventVisPtr obj = *iter;
		if (obj->IsSelect())
		{
			temp.push_back(obj);
			--(iter = collection.erase(iter)); // Must write erase like this. Or it will get run-time error!
		}
	}
}

void TLTrackVis::DeselectAllEvents()
{
	for (auto o : m_events)
	{
		o->Deselect();
	}
}

void TLTrackVis::DrawExpander(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	graphics.TranslateTransform(m_rect.X, m_rect.Y);

	if (IsExpand())
	{
		DrawExpanderExpand(graphics);
		//DrawContentExpand(graphics);
	}
	else
	{
		DrawExpanderCollapse(graphics);
		//DrawContentCollapse(graphics);
	}

	if (IsSelect())
	{
		DrawFocusBorder(graphics);
	}

	//DrawEvents(graphics);

	graphics.Restore(state);
}

void TLTrackVis::DrawContent(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	graphics.TranslateTransform(m_rect.X + m_iOffset, m_rect.Y);

	DrawContentExpand(graphics);
	//DrawEvents(graphics);
	DrawLines(graphics);
	DrawSourcePics(graphics);
	DrawCommandIcon(graphics);
	DrawEffectIndex(graphics);
	DrawEffect(graphics);

	graphics.Restore(state);
}

void TLTrackVis::Draw(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	graphics.TranslateTransform(m_rect.X, m_rect.Y);
	//graphics.TranslateTransform(m_rect.X + m_iOffset, m_rect.Y);

	if (IsExpand())
	{
		DrawExpanderExpand(graphics);
		//DrawContentExpand(graphics);
	}
	else
	{
		//DrawExpanderCollapse(graphics);
		//DrawContentCollapse(graphics);
	}

	if (IsSelect())
	{
		DrawFocusBorder(graphics);
	}

	//DrawEvents(graphics);
	DrawLines(graphics);
	DrawSourcePics(graphics);
	DrawCommandIcon(graphics);
	DrawEffectIndex(graphics);
	DrawEffect(graphics);

	graphics.Restore(state);
}

void TLTrackVis::DrawExpanderExpand(Gdiplus::Graphics& graphics)
{
	auto level = m_track->Level() - 1;
	auto shift = level * VisualConstant::LevelShift;

	auto state = graphics.Save();
	graphics.TranslateTransform(0, 0);

	// -------- Draw background -------- //
	Gdiplus::LinearGradientBrush brush1(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, Height()),
		Gdiplus::Color(255, 48, 48, 48),
		Gdiplus::Color(255, 32, 32, 32));
	graphics.FillRectangle(&brush1, shift, 0, VisualConstant::ExpanderWidth - shift, Height());

	// -------- Draw mark -------- //
	if (m_track != nullptr && m_track->HasChild())
	{
		Gdiplus::Pen pen(Gdiplus::Color(255, 180, 180, 180), 2.0f);
		// Draw "-"
		graphics.DrawLine(&pen, 4 + shift, 14, 26 + shift, 14);
	}


	// -------- Draw name -------- //
	Gdiplus::SolidBrush headerBrush(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::RectF layoutRect(shift, 0, VisualConstant::ExpanderWidth - shift - 10, Height());
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
	format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

	std::string name = m_track->Name();
	std::wstring widestr = std::wstring(name.begin(), name.end());
	std::wstring temp = L"...";
	if (name.size() > 20)
	{
		widestr = widestr.substr(0, 20) + temp;
	}

	graphics.DrawString(
		widestr.c_str(),
		widestr.length(),
		TLFont::Instance().GetFont(10),
		layoutRect,
		&format,
		&headerBrush);

	graphics.Restore(state);
}

void TLTrackVis::DrawContentExpand(Gdiplus::Graphics& graphics)
{
	auto level = m_track->Level() - 1;
	auto shift = level * VisualConstant::LevelShift;

	auto state = graphics.Save();
	graphics.TranslateTransform(0, 0);

	// -------- Draw background -------- //
	Gdiplus::SolidBrush brush(Gdiplus::Color(255, 48, 48, 48));
	graphics.FillRectangle(&brush, VisualConstant::ExpanderWidth, 0, m_rect.Width - VisualConstant::ExpanderWidth - m_iOffset, m_rect.Height);

	graphics.Restore(state);
}

void TLTrackVis::DrawExpanderCollapse(Gdiplus::Graphics& graphics)
{
	auto level = m_track->Level() - 1;
	auto shift = level * VisualConstant::LevelShift;

	auto state = graphics.Save();
	graphics.TranslateTransform(0, 0);

	// -------- Draw background -------- //
	Gdiplus::LinearGradientBrush brush1(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, Height()),
		Gdiplus::Color(255, 48, 48, 48),
		Gdiplus::Color(255, 32, 32, 32));
	graphics.FillRectangle(&brush1, shift, 0, VisualConstant::ExpanderWidth - shift, Height());

	// -------- Draw mark -------- //
	if (m_track != nullptr && m_track->HasChild())
	{
		Gdiplus::Pen pen(Gdiplus::Color(255, 180, 180, 180), 2.0f);
		// Draw "-"
		graphics.DrawLine(&pen, 4 + shift, 14, 26 + shift, 14);
		graphics.DrawLine(&pen, 15 + shift, 4, 15 + shift, 26);
	}

	// -------- Draw name -------- //
	Gdiplus::SolidBrush headerBrush(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::RectF layoutRect(shift, 0, VisualConstant::ExpanderWidth - shift - 10, Height());
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
	format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

	std::string name = m_track->Name();
	std::wstring widestr = std::wstring(name.begin(), name.end());
	std::wstring temp = L"...";
	if (name.size() > 20)
	{
		widestr = widestr.substr(0, 20) + temp;
	}

	graphics.DrawString(
		widestr.c_str(),
		widestr.length(),
		TLFont::Instance().GetFont(10),
		layoutRect,
		&format,
		&headerBrush);

	graphics.Restore(state);
}

void TLTrackVis::DrawContentCollapse(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();
	graphics.TranslateTransform(VisualConstant::ExpanderWidth, 0);

	// -------- Draw background -------- //
	Gdiplus::SolidBrush brush(Gdiplus::Color(255, 48, 48, 48));
	graphics.FillRectangle(&brush, 0, 0, m_rect.Width, m_rect.Height);

	graphics.Restore(state);
}

void TLTrackVis::DrawEvents(Gdiplus::Graphics& graphics)
{
	auto state = graphics.Save();

	auto& collection = m_events;

	for (auto o : collection)
	{
		auto pt = MapToTrackVisSpace(m_mouseEvent.MousePosition);
		o->MouseLeftButtonDown(pt);
	}

	// Draw link lines
	if (collection.size() > 1)
	{
		PointF* pts = new PointF[collection.size()];

		for (int i = 0; i<collection.size(); i++)
		{
			auto obj = collection.at(i);
			pts[i].X = obj->Position().X + obj->Width() / 2;
			pts[i].Y = obj->Position().Y + obj->Height() / 2;
		}

		Pen pen(Color::White);

		graphics.DrawLines(&pen, pts, collection.size());
	}

	// Draw events
	for (auto o : collection)
	{
		o->Draw(graphics);
	}

	graphics.Restore(state);
}

void TLTrackVis::DrawCues(Gdiplus::Graphics& graphics)
{

}

void TLTrackVis::DrawFocusBorder(Gdiplus::Graphics& graphics)
{
	auto level = m_track->Level() - 1;
	auto shift = level * VisualConstant::LevelShift;

	Gdiplus::SolidBrush brush(Gdiplus::Color::White);
	graphics.FillRectangle(&brush, shift, 0, 5, m_rect.Height);
}

void TLTrackVis::DrawLines(Gdiplus::Graphics& graphics)
{
	auto track_type = m_track->TrackType();
	track_type;

	if (TLTrackType::Float == track_type || TLTrackType::Int == track_type)
	{
		int iItemIndex, iTrackIndex;
		std::string sFullPath = TLTrackVis::GetFullPath();

		m_ContentProcessor.SetFullPath((char*)sFullPath.c_str());

		auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
		auto track_content_manager = manager->GetTrackContentManager();

		int iRet = track_content_manager->GetItemTackIndex((char*)sFullPath.c_str(), iItemIndex, iTrackIndex);
		if (iRet != -1)
		{
			POINT last_point;
			last_point.x;
			last_point.y;
			int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
			for (int i = 0; i < iTotalOfNodes; i++)
			{
				NumberNode* pCurNumberNode = (NumberNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, i);
				if (pCurNumberNode)
				{
					int x = pCurNumberNode->fTime * 100.0 + m_contentRegion.X;
					int y;

					//int y = pCurNumberNode->iValue + m_contentRegion.Y;
					if (pCurNumberNode->iValue == 0)
						y = 51;
					else if (pCurNumberNode->iValue > 0)
					{
						//int iOffset = pCurNumberNode->iValue / 2;
						int iOffset = 50 - (pCurNumberNode->iValue / 2);
						y = iOffset;
					}
					else if (pCurNumberNode->iValue < 0)
					{
						int iOffset = pCurNumberNode->iValue / 2;
						y = 51 - iOffset;
					}
					

					//pCurNumberNode->iLinkType;
					//pCurNumberNode->iAngle;

					Pen selectPen(Color::Orange, 1); //Red
					Pen activePen(Color::Yellow, 1); //Orange
					Pen deselectPen(Color(255,150,150,50), 1);  //White
					SolidBrush deselectBrush(Color(255, 150, 150, 50)); //White
					SolidBrush selectBrush(Color::Orange); //Red
					SolidBrush* brush = &deselectBrush;
					Pen* pen = &deselectPen;

					//int m_ContentProcessor.GetCurIndex();

					if (m_iActiveIndex == i)
					{
						brush = &selectBrush;
						pen = &selectPen;

						graphics.DrawEllipse(pen, x - 4, y - 4, 5 + 4, 5 + 4);
					}
					/*
					if (m_ContentProcessor.GetCurIndex() + 1 == i)
					{
						pen = &activePen;
					}
					*/

					graphics.FillEllipse(brush, x - 2, y - 2, 5, 5);

					if (i > 0)
					{
						//Pen pen(Color::White);
						switch (pCurNumberNode->iLinkType)
						{
							case LT_NORMAL:
								{
									graphics.DrawLine(pen, last_point.x, last_point.y, x, y);
								}
								break;
							case LT_SOAR:
								{
									graphics.DrawLine(pen, last_point.x, last_point.y, last_point.x, y);
									graphics.DrawLine(pen, last_point.x, y, x, y);
								}
								break;
							case LT_PLUNGE:
								{
									graphics.DrawLine(pen, last_point.x, last_point.y, x, last_point.y);
									graphics.DrawLine(pen, x, last_point.y,x,y);
								}
								break;
							case LT_FLAT_SOAR:
								{
									POINT p1, p2;
									p2.x = p1.x = last_point.x + (x - last_point.x) / 2;
									p1.y = last_point.y;
									p2.y = y;
									graphics.DrawLine(pen, last_point.x, last_point.y, p1.x, p1.y);
									graphics.DrawLine(pen, p1.x, p1.y, p2.x, p2.y);
									graphics.DrawLine(pen, p2.x, p2.y,x,y);
								}
								break;
							case LT_CURVE:
								{
									Point p1, p2, p3, p4;
									p1.X = last_point.x;
									p1.Y = last_point.y;

									switch (pCurNumberNode->iAngle)
									{
										case -45:
											{
												//-45
												p2.X = x - (x - last_point.x) / 4;
												p2.Y = last_point.y - (last_point.y - y) / 4;

												p3.X = x - (x - last_point.x) / 4;
												p3.Y = last_point.y - (last_point.y - y) / 4;
											}
											break;
										case -90:
											{
												// -90
												p2.X = x;
												p2.Y = last_point.y;

												p3.X = x;
												p3.Y = last_point.y;
											}
											break;
										case 45:
											{
												p2.X = last_point.x + (x - last_point.x) / 4;
												p2.Y = y + (last_point.y - y) / 4;

												p3.X = last_point.x + (x - last_point.x) / 4;
												p3.Y = y + (last_point.y - y) / 4;
											}
											break;
										case 90:
											{
												p2.X = last_point.x;
												p2.Y = y;

												p3.X = last_point.x;
												p3.Y = y;
											}
											break;
									}

									p4.X = x;
									p4.Y = y;
									graphics.DrawBezier(pen, p1, p2, p3, p4);
								}
								break;
						}
					}
					
#if ENABLE_INTERSECTION_POINT
					if (m_ContentProcessor.GetCurIndex() != -1)
					{
						POINT last_pt;
						for (int i = 0; i < m_ContentProcessor.GetTotalOfPoints(); i++)
						{
							POINT pt = m_ContentProcessor.GetPoint(i);

							if(i > 0)
								graphics.DrawLine(&activePen, last_pt.x, last_pt.y, pt.x, pt.y);

							last_pt.x = pt.x;
							last_pt.y = pt.y;
						}

						//graphics.DrawLine(&activePen, p1.x, p1.y, p2.x, p2.y);
					}

					POINT ptIntersection = m_ContentProcessor.GetIntersectionPoint();
					if (ptIntersection.x != -1 && ptIntersection.y != -1)
					{
						SolidBrush spotBrush(Color::YellowGreen); //Yellow

						graphics.FillEllipse(&spotBrush, ptIntersection.x - 4, ptIntersection.y - 4, 8, 8);

///jack
/*
if (GetContext()->Parent()->Name() == "VideoEffect" && 
							GetContext()->Name() == "Index" && g_MediaStreamAry.size() > 0)
						{
							g_MediaStreamAry.at(0)->m_iEffectIdx = INT32((ptIntersection.y+50)/5);
						}
						*/
///jack
					}
#endif
					

					last_point.x = x;
					last_point.y = y;
				}
			}
		}
	}
}

void TLTrackVis::DrawSourcePics(Gdiplus::Graphics& graphics)
{
	std::string name = this->m_track->Name();
	auto track_type = m_track->TrackType();
	if (strcmp(name.c_str(), "Source") == 0 || track_type == TLTrackType::Media)
	{
		wchar_t wszFileName[512];
		int iItemIndex, iTrackIndex;
		std::string sFullPath = TLTrackVis::GetFullPath();

		Pen selectPen(Color::Orange, 1); //Red
		Pen activePen(Color(255, 200, 200, 50), 1); 
		Pen normalPen(Color(255, 150, 150, 50), 1); //Color::White

		auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
		auto track_content_manager = manager->GetTrackContentManager();

		int iRet = track_content_manager->GetItemTackIndex((char*)sFullPath.c_str(), iItemIndex, iTrackIndex);
		if (iRet != -1)
		{
			int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
			for (int i = 0; i < iTotalOfNodes; i++)
			{
				MediaNode* pCurMediaNode = (MediaNode*)track_content_manager->GetNodeData(iItemIndex,iTrackIndex, i);
				if (pCurMediaNode)
				{
					int x = pCurMediaNode->fTime * 100.0 + m_contentRegion.X;
					int y = (101 - _THUMBNAIL_FRAME_H) / 2;

					wcscpy(wszFileName, track_content_manager->Char2WChar(pCurMediaNode->szPath));
					Gdiplus::Bitmap* pPic = manager->GetTrackContentManager()->GetPicture(wszFileName);
					//graphics.DrawImage(pPic, x, y, _THUMBNAIL_FRAME_W, -_THUMBNAIL_FRAME_H);
					graphics.DrawImage(pPic, x, y + _THUMBNAIL_FRAME_H, _THUMBNAIL_FRAME_W, -_THUMBNAIL_FRAME_H);

#if _ENABLE_MEDIA_LEN
					int w = pCurMediaNode->fLen * 100.0;
		
					if (m_iSelectIndex == i)
						graphics.DrawRectangle(&selectPen, x - 1, y - 1, w + 1, _THUMBNAIL_FRAME_H + 1);
				//	else if (m_iActiveIndex == i)
					//	graphics.DrawRectangle(&activePen, x - 1, y - 1, w + 1, _THUMBNAIL_FRAME_H + 1);
					else
						graphics.DrawRectangle(&normalPen, x - 1, y - 1, w + 1, _THUMBNAIL_FRAME_H + 1);

					graphics.DrawImage(pPic, x + w - _THUMBNAIL_FRAME_W, y + _THUMBNAIL_FRAME_H, _THUMBNAIL_FRAME_W, -_THUMBNAIL_FRAME_H);
#else

					if (m_iActiveIndex == i)
						graphics.DrawRectangle(&selectPen, x - 1, y - 1, _THUMBNAIL_FRAME_W + 1, _THUMBNAIL_FRAME_H + 1);
#endif
				}
			}
		}
	}
}

void TLTrackVis::DrawCommandIcon(Gdiplus::Graphics& graphics)
{
	auto track_type = m_track->TrackType();
	if (track_type == TLTrackType::Command)
	{
		int iItemIndex, iTrackIndex;
		std::string sFullPath = TLTrackVis::GetFullPath();

		Pen selectPen(Color::Yellow, 1);

		auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
		auto track_content_manager = manager->GetTrackContentManager();

		int iRet = track_content_manager->GetItemTackIndex((char*)sFullPath.c_str(), iItemIndex, iTrackIndex);
		if (iRet != -1)
		{
			int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
			for (int i = 0; i < iTotalOfNodes; i++)
			{
				CommandNode* pCurCmdNode = (CommandNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, i);
				if (pCurCmdNode)
				{
					int x = pCurCmdNode->fTime * 100.0 + m_contentRegion.X;
					int y = (32 - 20) / 2;

					std::string szText;
					
					switch (pCurCmdNode->iType)
					{
						case CT_NONE:
							szText = "E";
							break;
						case CT_STOP:
							szText = "S";
							break;
						case CT_JUMP:
							szText = "J";
							break;
					}

					graphics.DrawRectangle(&selectPen, x, y, 20, 20);

					Gdiplus::SolidBrush brush(Gdiplus::Color::Orange);
					Gdiplus::RectF rect(x, y, 20, 20);
					Gdiplus::StringFormat format;
					format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
					format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

					std::string name = szText;
					std::wstring widestr = std::wstring(name.begin(), name.end());

					graphics.DrawString(
						widestr.c_str(),
						widestr.length(),
						TLFont::Instance().GetFont(9),
						rect,
						&format,
						&brush);

				}
			}
		}
	}
}

void TLTrackVis::DrawEffectIndex(Gdiplus::Graphics& graphics)
{
	char szIndexText[80];
	auto track_type = m_track->TrackType();
	if (track_type == TLTrackType::Effect_Index)
	{
		int iItemIndex, iTrackIndex;
		std::string sFullPath = TLTrackVis::GetFullPath();

		Pen selectPen(Color::Yellow, 1);

		auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
		auto track_content_manager = manager->GetTrackContentManager();

		int iRet = track_content_manager->GetItemTackIndex((char*)sFullPath.c_str(), iItemIndex, iTrackIndex);
		if (iRet != -1)
		{
			int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
			for (int i = 0; i < iTotalOfNodes; i++)
			{
				EffectIndexNode* pCurInxNode = (EffectIndexNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, i);
				if (pCurInxNode)
				{
					int x = pCurInxNode->fTime * 100.0 + m_contentRegion.X;
					int y = (32 - 20) / 2;

					sprintf(szIndexText, "%s"/*"%02d %s", pCurInxNode->iIndex*/, g_effect_param_items[pCurInxNode->iIndex].szEffectName);

					std::string szText = szIndexText;

					graphics.DrawRectangle(&selectPen, x, y, 90, 90);

					Gdiplus::SolidBrush brush(Gdiplus::Color::Orange);
					Gdiplus::RectF rect(x, y, 90, 90);
					Gdiplus::StringFormat format;
					format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
					format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

					std::string name = szText;
					std::wstring widestr = std::wstring(name.begin(), name.end());

					graphics.DrawString(
						widestr.c_str(),
						widestr.length(),
						TLFont::Instance().GetFont(9),
						rect,
						&format,
						&brush);
				}
			}
		}
	}
}

void TLTrackVis::DrawEffect(Gdiplus::Graphics& graphics)
{
	std::string name = this->m_track->Name();
	auto track_type = m_track->TrackType();
	if (track_type == TLTrackType::Effect)
	{
		wchar_t wszFileName[512];
		int iItemIndex, iTrackIndex;
		std::string sFullPath = TLTrackVis::GetFullPath();

		Pen selectPen(Color::Orange, 1); //Red
		Pen activePen(Color(255, 200, 200, 50), 1);
		Pen normalPen(Color(255, 150, 150, 50), 1); //Color::White

		auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
		auto track_content_manager = manager->GetTrackContentManager();

		int iRet = track_content_manager->GetItemTackIndex((char*)sFullPath.c_str(), iItemIndex, iTrackIndex);
		if (iRet != -1)
		{
			int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
			for (int i = 0; i < iTotalOfNodes; i++)
			{
				EffectNode* pCurEffectNode = (EffectNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, i);
				if (pCurEffectNode)
				{
					int x = pCurEffectNode->fTime * 100.0 + m_contentRegion.X;
					int y = (101 - _THUMBNAIL_FRAME_H) / 2;
					int w = pCurEffectNode->fDuration * 100.0;

					if (m_iSelectIndex == i)
						graphics.DrawRectangle(&selectPen, x - 1, y - 1, w + 1, _THUMBNAIL_FRAME_H + 1);
					else
						graphics.DrawRectangle(&normalPen, x - 1, y - 1, w + 1, _THUMBNAIL_FRAME_H + 1);

					Gdiplus::SolidBrush brush(Gdiplus::Color::Orange);
					Gdiplus::RectF rect(x, y, w, _THUMBNAIL_FRAME_H);
					Gdiplus::StringFormat format;
					format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
					format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

					std::string name = pCurEffectNode->szName;
					std::wstring widestr = std::wstring(name.begin(), name.end());

					graphics.DrawString(
						widestr.c_str(),
						widestr.length(),
						TLFont::Instance().GetFont(9),
						rect,
						&format,
						&brush);
				}
			}
		}
	}
}

void TLTrackVis::SetExpandRegion()
{
	m_expanderRegion.X = 0;
	m_expanderRegion.Y = 0;
	m_expanderRegion.Width = VisualConstant::ExpanderWidth;
	m_expanderRegion.Height = VisualConstant::ExpandHeight;

	m_contentRegion.X = VisualConstant::ExpanderWidth;
	m_contentRegion.Y = 0;
	m_contentRegion.Width = m_rect.Width - VisualConstant::ExpanderWidth;
	m_contentRegion.Height = VisualConstant::ExpandHeight;

	TLGeometricTransform::Translate(m_expanderRegion, m_rect.X, m_rect.Y);
	TLGeometricTransform::Translate(m_contentRegion, m_rect.X, m_rect.Y);
}

void TLTrackVis::SetCollapseRegion()
{
	m_expanderRegion.X = 0;
	m_expanderRegion.Y = 0;
	m_expanderRegion.Width = VisualConstant::ExpanderWidth;
	m_expanderRegion.Height = VisualConstant::CollapseHeight;

	m_contentRegion.X = VisualConstant::ExpanderWidth;
	m_contentRegion.Y = 0;
	m_contentRegion.Width = m_rect.Width - VisualConstant::ExpanderWidth;
	m_contentRegion.Height = VisualConstant::ExpandHeight; //VisualConstant::CollapseHeight;

	TLGeometricTransform::Translate(m_expanderRegion, m_rect.X, m_rect.Y);
	TLGeometricTransform::Translate(m_contentRegion, m_rect.X, m_rect.Y);
}

bool TLTrackVis::HasEventCollision(const Gdiplus::Point& point)
{
	bool hasCollision = false;

	// Check objects collision
	for (auto obj : m_events)
	{
		if (obj->HasCollision(point))
		{
			hasCollision = true;
			break;
		}
	}
	return hasCollision;
}

Gdiplus::Point TLTrackVis::MapToTrackVisSpace(const Gdiplus::Point& point)
{
	//auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	//int shift_x = manager->TimeCodeToX(m_contentRegion, manager->GetBaseTimeCode());
	return Gdiplus::Point(point.X - m_rect.X, point.Y - m_rect.Y);
}

void TLTrackVis::InsertSource(const Gdiplus::Point& point)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	CWnd* pWnd = manager->GetMediaServerDlgWnd();

	TrackContentManager* pTrackContentManager = manager->GetTrackContentManager();
	CopyDataInfo copydata_info;

	copydata_info.pObj = pTrackContentManager;

	COPYDATASTRUCT cpd;

	cpd.dwData = MSG_SELECT_MEDIA;
	cpd.cbData = sizeof(CopyDataInfo);
	cpd.lpData = &copydata_info;

	pWnd->SendMessage(WM_COPYDATA,0, (LPARAM)(LPVOID)&cpd);
//	UpdateItemTotalTime();
}

void TLTrackVis::DeleteSource(const Gdiplus::Point& point)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	TrackContentManager* pTrackContentManager = manager->GetTrackContentManager();

	//std::string fullpath = m_track->GetFullPath();
	std::string fullpath = GetFullPath();

	int iPos = point.X - m_contentRegion.X;
	float fUnit = (iPos - m_iOffset) / 100.0;

	pTrackContentManager->Delete((char*)fullpath.c_str(),fUnit);
	UpdateItemTotalTime();

	int iStreamIndex = m_ContentProcessor.CheckStreamIndex((char*)fullpath.c_str());
	OutDevice* pOutDevice = manager->GetOutDevice(iStreamIndex);
	if (pOutDevice)
	{
		if (pOutDevice->IsOpened())
		{
			pOutDevice->Close();

			DisplayManager* pDisplayManager = manager->GetDisplayManager();
			pDisplayManager->Clear(iStreamIndex - ST_STREAM0,800,600);
		}
	}
}

void TLTrackVis::DeletePoint(const Gdiplus::Point& point)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	TrackContentManager* pTrackContentManager = manager->GetTrackContentManager();

	//std::string fullpath = m_track->GetFullPath();
	std::string fullpath = GetFullPath();

	int iPos = point.X - m_contentRegion.X;
	float fUnit = (iPos - m_iOffset) / 100.0;

	pTrackContentManager->Delete((char*)fullpath.c_str(), fUnit);
	UpdateItemTotalTime();
}

void TLTrackVis::ChangeAngle(const Gdiplus::Point& point,int iAngle)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	TrackContentManager* pTrackContentManager = manager->GetTrackContentManager();
	//std::string fullpath = m_track->GetFullPath();
	std::string fullpath = GetFullPath();

	int iPos = point.X - m_contentRegion.X;
	float fUnit = (iPos - m_iOffset) / 100.0;

	int iItemIndex, iTrackIndex;
	int iRet = pTrackContentManager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);
	int iTotalOfNodes = pTrackContentManager->GetTotalOfNodes(iItemIndex, iTrackIndex);

	int iIndex = pTrackContentManager->FindNode((char*)fullpath.c_str(), fUnit);

	if (iIndex >= 0)
	{
		NumberNode* pCurNumberNode = (NumberNode*)pTrackContentManager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
		if (pCurNumberNode)
			pCurNumberNode->iAngle = iAngle;	
	}
}

void TLTrackVis::Unlink(const Gdiplus::Point& point)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	TrackContentManager* pTrackContentManager = manager->GetTrackContentManager();
	//std::string fullpath = m_track->GetFullPath();
	std::string fullpath = GetFullPath();

	int iPos = point.X - m_contentRegion.X;
	float fUnit = (iPos - m_iOffset) / 100.0;

	int iItemIndex, iTrackIndex;
	int iRet = pTrackContentManager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);
	int iTotalOfNodes = pTrackContentManager->GetTotalOfNodes(iItemIndex, iTrackIndex);

	int iIndex = pTrackContentManager->FindNode((char*)fullpath.c_str(), fUnit);

	if (iIndex >= 0)
	{
		NumberNode* pCurNumberNode = (NumberNode*)pTrackContentManager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
		if (pCurNumberNode)
			pCurNumberNode->iLinkType = LT_NONE;
	}
}

void TLTrackVis::InsertCommand(const Gdiplus::Point& point)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	CWnd* pWnd = manager->GetMediaServerDlgWnd();
	//std::string item_uuid = manager->GetSelectedItem("");
	//auto item = manager->GetItem(item_uuid);
	//std::string item_name = item->GetInfo().Name();

	std::string fullpath = GetFullPath(); //item_name + "/" + m_track->GetFullPath();

	TrackContentManager* pTrackContentManager = manager->GetTrackContentManager();

	CEditCommandDlg dlg;
	dlg.SetFullPath((char*)fullpath.c_str());
	dlg.SetTrackContentManager(pTrackContentManager);
	if (dlg.DoModal() == IDOK)
	{
		int iPos = point.X - m_contentRegion.X;
		float fUnit = (iPos - m_iOffset) / 100.0;

		if (strcmp(dlg.GetName(), "") == 0)
		{
			MessageBoxA(NULL, "Command name cannot be empty!", "Warning", MB_OK| MB_TOPMOST);
			return;
		}

		
		int iItemIndex, iTrackIndex;
		int iRet = pTrackContentManager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);
		if (iRet != -1)
		{
			int iTotalOfNodes = pTrackContentManager->GetTotalOfNodes(iItemIndex, iTrackIndex);
			for (int i = 0; i < iTotalOfNodes; i++)
			{
				CommandNode* pCurCmdNode = (CommandNode*)pTrackContentManager->GetNodeData(iItemIndex, iTrackIndex, i);
				if (pCurCmdNode)
				{
					if (strcmp(pCurCmdNode->szName, dlg.GetName()) == 0)
					{
						MessageBox(NULL, "Cannot have the same name!!", "Warning", MB_OK| MB_TOPMOST);
						return;
					}
				}
			}
		}

		pTrackContentManager->Add((char*)fullpath.c_str(), fUnit, dlg.GetType(), dlg.GetName(), dlg.GetTargetName(), dlg.GetJumpType());
	}
//	UpdateItemTotalTime();
}

void TLTrackVis::DeleteCommand(const Gdiplus::Point& point)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	CWnd* pWnd = manager->GetMediaServerDlgWnd();
	//std::string fullpath = m_track->GetFullPath();
	std::string fullpath = GetFullPath();

	TrackContentManager* pTrackContentManager = manager->GetTrackContentManager();

	int iPos = point.X - m_contentRegion.X;
	float fUnit = (iPos - m_iOffset) / 100.0;

	int iIndex = pTrackContentManager->FindNode((char*)fullpath.c_str(), fUnit);

	if (iIndex >= 0)
	{
		pTrackContentManager->Delete((char*)fullpath.c_str(), fUnit);
	}
//	UpdateItemTotalTime();
}

void TLTrackVis::InsertEffectIndex(const Gdiplus::Point& point)
{
	CEditEffectIndexDlg dlg;

	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	std::string fullpath = GetFullPath();

	TrackContentManager* pTrackContentManager = manager->GetTrackContentManager();

	if (dlg.DoModal() == IDOK)
	{
		int iIndex = dlg.GetIndex();

		int iPos = point.X - m_contentRegion.X;
		float fUnit = (iPos - m_iOffset) / 100.0;
		pTrackContentManager->Add((char*)fullpath.c_str(), fUnit,iIndex);
	}
	UpdateItemTotalTime();
}

void TLTrackVis::DeleteEffectIndex(const Gdiplus::Point& point)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	std::string fullpath = GetFullPath();

	TrackContentManager* pTrackContentManager = manager->GetTrackContentManager();

	int iPos = point.X - m_contentRegion.X;
	float fUnit = (iPos - m_iOffset) / 100.0;

	int iIndex = pTrackContentManager->FindNode((char*)fullpath.c_str(), fUnit);

	if (iIndex >= 0)
	{
		pTrackContentManager->Delete((char*)fullpath.c_str(), fUnit);
	}
	UpdateItemTotalTime();
}

void TLTrackVis::MoveToBeginning(const Gdiplus::Point& point)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	std::string fullpath = GetFullPath();

	int iItemIndex = -1, iTrackIndex = -1;
	auto track_content_manager = manager->GetTrackContentManager();

	int iPos = point.X - m_contentRegion.X;
	float fUnit = (iPos - m_iOffset) / 100.0;
	int iIndex = track_content_manager->FindNode((char*)fullpath.c_str(), fUnit);
	int iTackIndexRet = track_content_manager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);

	if (iTackIndexRet != -1)
	{
		int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
		if (iTotalOfNodes > iIndex && iIndex != -1)
		{
			MediaNode* pCurMediaNode = (MediaNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
			if (pCurMediaNode)
				pCurMediaNode->fTime = 0.0;

			UpdateItemTotalTime();
		}
	}
}

void TLTrackVis::CloseToTheFront(const Gdiplus::Point& point)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	std::string fullpath = GetFullPath();

	int iItemIndex = -1, iTrackIndex = -1;
	auto track_content_manager = manager->GetTrackContentManager();

	int iPos = point.X - m_contentRegion.X;
	float fUnit = (iPos - m_iOffset) / 100.0;

	int iTackIndexRet = track_content_manager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);
//	track_content_manager->Sort(iItemIndex, iTrackIndex);
	int iIndex = track_content_manager->FindNode((char*)fullpath.c_str(), fUnit);

	if (iTackIndexRet != -1)
	{
		int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
		if (iTotalOfNodes == 1)
		{
			if (iTotalOfNodes > iIndex && iIndex != -1)
			{
				MediaNode* pCurMediaNode = (MediaNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
				if (pCurMediaNode)
					pCurMediaNode->fTime = 0.0;
			}
		}
		else
		{
			if (iTotalOfNodes > 1)
			{
				if (iIndex == 0)
				{
					MediaNode* pCurMediaNode = (MediaNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
					if (pCurMediaNode)
						pCurMediaNode->fTime = 0.0;
				}
				else
				{
					float fOffset = 0.0;
					if (iIndex - 1 >= 0)
					{
						MediaNode* pFrontMediaNode = (MediaNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, iIndex - 1);
						if (pFrontMediaNode)
							fOffset = pFrontMediaNode->fTime + pFrontMediaNode->fLen + 0.03;

						MediaNode* pCurMediaNode = (MediaNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
						if (pCurMediaNode)
							pCurMediaNode->fTime = fOffset;
					}
				}
			}
		}

		UpdateItemTotalTime();
	}
}

void TLTrackVis::MoveTo(const Gdiplus::Point& point,int iOP,int iSec)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	std::string fullpath = GetFullPath();

	int iItemIndex = -1, iTrackIndex = -1;
	auto track_content_manager = manager->GetTrackContentManager();

	int iPos = point.X - m_contentRegion.X;
	float fUnit = (iPos - m_iOffset) / 100.0;
	int iIndex = track_content_manager->FindNode((char*)fullpath.c_str(), fUnit);
	int iTackIndexRet = track_content_manager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);

	if (iTackIndexRet != -1)
	{
		int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
		if (iTotalOfNodes > iIndex && iIndex != -1)
		{
			MediaNode* pCurMediaNode = (MediaNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
			if (pCurMediaNode)
			{
				switch (iOP)
				{
				case MOP_NONE:
					pCurMediaNode->fTime = iSec;
					break;
				case MOP_ADD:
					pCurMediaNode->fTime += iSec;
					break;
				case MOP_SUB:
					{
						float fNewTime = pCurMediaNode->fTime - iSec;
						if(fNewTime < 0)
							pCurMediaNode->fTime = 0.0;
						else
							pCurMediaNode->fTime = fNewTime;
					}
					break;
				case MOP_ANY:
					CMoveToDlg dlg;
					if (dlg.DoModal() == IDOK)
					{
						pCurMediaNode->fTime = dlg.GetTime();
					}
					break;
				}
			}

			UpdateItemTotalTime();
		}
	}
}

void TLTrackVis::Effect_CloseToTheFront(const Gdiplus::Point& point)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	std::string fullpath = GetFullPath();

	int iItemIndex = -1, iTrackIndex = -1;
	auto track_content_manager = manager->GetTrackContentManager();

	int iPos = point.X - m_contentRegion.X;
	float fUnit = (iPos - m_iOffset) / 100.0;

	int iTackIndexRet = track_content_manager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);
//	track_content_manager->Sort(iItemIndex, iTrackIndex);
	int iIndex = track_content_manager->FindNode((char*)fullpath.c_str(), fUnit);

	if (iTackIndexRet != -1)
	{
		int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
		if (iTotalOfNodes == 1)
		{
			if (iTotalOfNodes > iIndex && iIndex != -1)
			{
				EffectNode* pCurEffectNode = (EffectNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
				if (pCurEffectNode)
					pCurEffectNode->fTime = 0.0;
			}
		}
		else
		{
			if (iTotalOfNodes > 1)
			{
				if (iIndex == 0)
				{
					EffectNode* pCurEffectNode = (EffectNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
					if (pCurEffectNode)
						pCurEffectNode->fTime = 0.0;
				}
				else
				{
					float fOffset = 0.0;
					if (iIndex - 1 >= 0)
					{
						EffectNode* pFrontEffectNode = (EffectNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, iIndex - 1);
						if (pFrontEffectNode)
							fOffset = pFrontEffectNode->fTime + pFrontEffectNode->fDuration + 0.03;

						EffectNode* pCurEffectNode = (EffectNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
						if (pCurEffectNode)
							pCurEffectNode->fTime = fOffset;
					}
				}
			}
		}
		UpdateItemTotalTime();
	}
}

void TLTrackVis::Effect_MoveTo(const Gdiplus::Point& point, int iOP, int iSec)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	std::string fullpath = GetFullPath();

	int iItemIndex = -1, iTrackIndex = -1;
	auto track_content_manager = manager->GetTrackContentManager();

	int iPos = point.X - m_contentRegion.X;
	float fUnit = (iPos - m_iOffset) / 100.0;
	int iIndex = track_content_manager->FindNode((char*)fullpath.c_str(), fUnit);
	int iTackIndexRet = track_content_manager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);

	if (iTackIndexRet != -1)
	{
		int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
		if (iTotalOfNodes > iIndex && iIndex != -1)
		{
			EffectNode* pCurEffectNode = (EffectNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
			if (pCurEffectNode)
			{
				switch (iOP)
				{
				case MOP_NONE:
					pCurEffectNode->fTime = iSec;
					break;
				case MOP_ADD:
					pCurEffectNode->fTime += iSec;
					break;
				case MOP_SUB:
				{
					float fNewTime = pCurEffectNode->fTime - iSec;
					if (fNewTime < 0)
						pCurEffectNode->fTime = 0.0;
					else
						pCurEffectNode->fTime = fNewTime;
				}
				break;
				case MOP_ANY:
					CMoveToDlg dlg;
					if (dlg.DoModal() == IDOK)
					{
						pCurEffectNode->fTime = dlg.GetTime();
					}
					break;
				}
			}

			UpdateItemTotalTime();
		}
	}
}

void TLTrackVis::InsertEffect(const Gdiplus::Point& point)
{
	g_pUIController->OpenEffectSelector();
}

void TLTrackVis::DeleteEffect(const Gdiplus::Point& point)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	std::string fullpath = GetFullPath();

	TrackContentManager* pTrackContentManager = manager->GetTrackContentManager();

	int iPos = point.X - m_contentRegion.X;
	float fUnit = (iPos - m_iOffset) / 100.0;

	int iIndex = pTrackContentManager->FindNode((char*)fullpath.c_str(), fUnit);

	if (iIndex >= 0)
	{
		pTrackContentManager->Delete((char*)fullpath.c_str(), fUnit);

		int iStreamIndex = m_ContentProcessor.GetEffectStreamIndex((char*)fullpath.c_str());
		m_ContentProcessor.ResetEffect(iStreamIndex);
	}
	UpdateItemTotalTime();
}

void TLTrackVis::ConfigAudio(const Gdiplus::Point& point)
{
	int iItemIndex = -1, iTrackIndex = -1;
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	std::string fullpath = GetFullPath();

	TrackContentManager* pTrackContentManager = manager->GetTrackContentManager();
	int iTackIndexRet = pTrackContentManager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);

#if 0
	CAudioConfigDlg dlg;

	for (int i = 0; i < _MAX_SOURCES; i++)
	{
		bool bEnable = pTrackContentManager->AudioIsEnabled(iItemIndex, i);
		dlg.EnableAudio(i, bEnable);
	}

	if (dlg.DoModal() == IDOK)
	{
		for (int j = 0; j < _MAX_SOURCES; j++)
		{
			bool bEnable = dlg.AudioIsEnabled(j);
			pTrackContentManager->EnableAudio(iItemIndex,j, bEnable);

			OutDevice* pOutDevice = manager->GetOutDevice(ST_STREAM0 + j);
			if (pOutDevice)
			{
				pOutDevice->EnableAudio(bEnable);
			}
		}
	}
#else

	AudioConfigWin* pAudioConfigWin = g_pUIController->GetAudioConfigWin();

	for (int i = 0; i < _MAX_SOURCES; i++)
	{
		bool bEnable = pTrackContentManager->AudioIsEnabled(iItemIndex, i);
		pAudioConfigWin->SetCheck(i, bEnable);

		int iVolume = pTrackContentManager->GetVolume(iItemIndex, i);
		pAudioConfigWin->SetVolume(i, iVolume);
	}

	g_pUIController->OpenAudioConfig();
#endif
}

void TLTrackVis::ChangeEffectLength(const Gdiplus::Point& point)
{
	CMoveToDlg dlg;
	dlg.SetType(DT_CHANGE_LEN);
	if (dlg.DoModal() == IDOK)
	{
		float fLength = dlg.GetTime();

		auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
		std::string fullpath = GetFullPath();

		int iItemIndex = -1, iTrackIndex = -1;
		auto track_content_manager = manager->GetTrackContentManager();

		int iPos = point.X - m_contentRegion.X;
		float fUnit = (iPos - m_iOffset) / 100.0;
		int iIndex = track_content_manager->FindNode((char*)fullpath.c_str(), fUnit);
		int iTackIndexRet = track_content_manager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);

		if (iTackIndexRet != -1)
		{
			int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
			if (iTotalOfNodes > iIndex && iIndex != -1)
			{
				EffectNode* pCurEffectNode = (EffectNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
				if (pCurEffectNode)
				{
					pCurEffectNode->fDuration = fLength;
				}
			}
		}

		UpdateItemTotalTime();
	}
}

void TLTrackVis::PopupMenu(int iType, const Gdiplus::Point& point)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();

	if (manager->GetPBStatus() == PBC_PLAY)
		return;

	CMenu *menu = new CMenu;
	menu->CreatePopupMenu();

	switch (iType)
	{
		case PMT_INSERT_SRC:
			menu->AppendMenu(MF_STRING, IDC_POP_INSERT_SRC, "Insert");
			break;
		case PMT_DEL_SRC:
			menu->AppendMenu(MF_STRING, IDC_POP_DEL_SRC, "Remove");
			break;
		case PMT_DEL_POINT:
			menu->AppendMenu(MF_STRING, IDC_POP_DEL_POINT, "Remove");
			menu->AppendMenu(MF_STRING, IDC_POP_UNLINK, "Unlink");
			break;
		case PMT_DEL_POINT2:
			{
				menu->AppendMenu(MF_STRING, IDC_POP_DEL_POINT, "Remove");
				menu->AppendMenu(MF_STRING, IDC_POP_ANGLE1, "-45¢X");
				menu->AppendMenu(MF_STRING, IDC_POP_ANGLE2, "-90¢X");
				menu->AppendMenu(MF_STRING, IDC_POP_ANGLE3, "45¢X");
				menu->AppendMenu(MF_STRING, IDC_POP_ANGLE4, "90¢X");
			}
			break;
		case PMT_INSERT_CMD:
			menu->AppendMenu(MF_STRING, IDC_POP_INSERT_CMD, "Insert");
			break;
		case PMT_DEL_CMD:
			menu->AppendMenu(MF_STRING, IDC_POP_DEL_CMD, "Remove");
			break;
		case PMT_INSERT_EFFECT_INDEX:
			menu->AppendMenu(MF_STRING, IDC_POP_INSERT_EFFECT_INDEX, "Insert");
			break;
		case PMT_DEL_EFFECT_INDEX:
			menu->AppendMenu(MF_STRING, IDC_POP_DEL_EFFECT_INDEX, "Remove");
			break;
		case PMT_MOVE_TO_BEGINNING:
			menu->AppendMenu(MF_STRING, IDC_POP_MOVE_TO_BEGINNING, "Move to Begining");
			break;
		case PMT_CLOSE_TO_THE_FRONT:
			menu->AppendMenu(MF_STRING, IDC_POP_CLOSE_TO_THE_FRONT, "Close to the front");
			break;
		case PMT_MOVE_TO:
			//menu->AppendMenu(MF_STRING, IDC_POP_MOVE_TO_BEGINNING, "Move to Begining");
			menu->AppendMenu(MF_STRING, IDC_POP_CLOSE_TO_THE_FRONT, "Close to the front");
			menu->AppendMenu(MF_STRING, IDC_POP_MOVE_TO_1,"Move to 0:01");
			menu->AppendMenu(MF_STRING, IDC_POP_MOVE_TO_2, "Move to 0:02");
			menu->AppendMenu(MF_STRING, IDC_POP_MOVE_TO_5, "Move to 0:05");
			menu->AppendMenu(MF_STRING, IDC_POP_MOVE_TO_10, "Move to 0:10");

			menu->AppendMenu(MF_STRING, IDC_POP_ADD_1_SEC, "+1 Sec");
			menu->AppendMenu(MF_STRING, IDC_POP_ADD_2_SECS, "+2 Secs");
			menu->AppendMenu(MF_STRING, IDC_POP_ADD_5_SECS, "+5 Secs");
			menu->AppendMenu(MF_STRING, IDC_POP_ADD_10_SECS, "+10 Secs");
			menu->AppendMenu(MF_STRING, IDC_POP_SUB_1_SEC, "-1 Sec");
			menu->AppendMenu(MF_STRING, IDC_POP_SUB_2_SECS, "-2 Secs");
			menu->AppendMenu(MF_STRING, IDC_POP_SUB_5_SECS, "-5 Secs");
			menu->AppendMenu(MF_STRING, IDC_POP_SUB_10_SECS, "-10 Secs");
			menu->AppendMenu(MF_STRING, IDC_POP_MOVE_TO, "Move to ...");
			break;
		case PMT_INSERT_EFFECT:
			menu->AppendMenu(MF_STRING, IDC_POP_INSERT_EFFECT, "Insert");
			break;
		case PMT_DEL_EFFECT:
			menu->AppendMenu(MF_STRING, IDC_POP_DEL_EFFECT, "Remove");
			menu->AppendMenu(MF_STRING, IDC_POP_CHANG_EEFFECT_LEN, "Change length");
			break;
		case PMT_EFFECT_MOVE_TO:
			menu->AppendMenu(MF_STRING, IDC_POP_EFFECT_CLOSE_TO_THE_FRONT, "Close to the front");
			menu->AppendMenu(MF_STRING, IDC_POP_EFFECT_MOVE_TO, "Move to ...");
			break;
		case PMT_CONFIG_AUDIO:
			menu->AppendMenu(MF_STRING, IDC_POP_CONFIG_AUDIO, "Config Audio");
			break;
	}

//	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();

	CPoint client_point;
	client_point.x = point.X;
	client_point.y = point.Y;
	manager->GetEditViewWnd()->ClientToScreen(&client_point);

	UINT nCode = menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, client_point.x, client_point.y, manager->GetEditViewWnd());
	delete menu;

	switch (nCode)
	{
		case IDC_POP_INSERT_SRC:
			InsertSource(point);
			break;
		case IDC_POP_DEL_SRC:
			DeleteSource(point);
			break;
		case IDC_POP_DEL_POINT:
			DeletePoint(point);
			break;
		case IDC_POP_ANGLE1:
			ChangeAngle(point, -45);
			break;
		case IDC_POP_ANGLE2:
			ChangeAngle(point, -90);
			break;
		case IDC_POP_ANGLE3:
			ChangeAngle(point, 45);
			break;
		case IDC_POP_ANGLE4:
			ChangeAngle(point, 90);
			break;
		case IDC_POP_UNLINK:
			Unlink(point);
			break;
		case IDC_POP_INSERT_CMD:
			InsertCommand(point);
			break;
		case IDC_POP_DEL_CMD:
			DeleteCommand(point);
			break;
		case  IDC_POP_INSERT_EFFECT_INDEX:
			InsertEffectIndex(point);
			break;
		case  IDC_POP_DEL_EFFECT_INDEX:
			DeleteEffectIndex(point);
			break;
		case IDC_POP_MOVE_TO_BEGINNING:
			MoveToBeginning(point);
			break;
		case IDC_POP_MOVE_TO_1:
			MoveTo(point, MOP_NONE, 1);
			break;
		case IDC_POP_MOVE_TO_2:
			MoveTo(point, MOP_NONE, 2);
			break;
		case IDC_POP_MOVE_TO_5:
			MoveTo(point, MOP_NONE, 5);
			break;
		case IDC_POP_MOVE_TO_10:
			MoveTo(point, MOP_NONE, 10);
			break;
		case IDC_POP_ADD_1_SEC:
			MoveTo(point, MOP_ADD, 1);
			break;
		case IDC_POP_ADD_2_SECS:
			MoveTo(point, MOP_ADD, 2);
			break;
		case IDC_POP_ADD_5_SECS:
			MoveTo(point, MOP_ADD, 5);
			break;
		case IDC_POP_ADD_10_SECS:
			MoveTo(point, MOP_ADD, 10);
			break;
		case IDC_POP_SUB_1_SEC:
			MoveTo(point, MOP_SUB, 1);
			break;
		case IDC_POP_SUB_2_SECS:
			MoveTo(point, MOP_SUB, 2);
			break;
		case IDC_POP_SUB_5_SECS:
			MoveTo(point, MOP_SUB, 5);
			break;
		case IDC_POP_SUB_10_SECS:
			MoveTo(point, MOP_SUB, 10);
			break;
		case IDC_POP_MOVE_TO:
			MoveTo(point, MOP_ANY, 0);
			break;
		case IDC_POP_CLOSE_TO_THE_FRONT:
			CloseToTheFront(point);
			break;
		case IDC_POP_INSERT_EFFECT:
			InsertEffect(point);
			break;
		case IDC_POP_DEL_EFFECT:
			DeleteEffect(point);
			break;
		case IDC_POP_EFFECT_CLOSE_TO_THE_FRONT:
			Effect_CloseToTheFront(point);
			break;
		case IDC_POP_EFFECT_MOVE_TO:
			Effect_MoveTo(point, MOP_ANY, 0);
			break;
		case IDC_POP_CONFIG_AUDIO:
			ConfigAudio(point);
			break;
		case IDC_POP_CHANG_EEFFECT_LEN:
			ChangeEffectLength(point);
			break;
	}
}
/*
std::string TLTrackVis::GetFullPath()
{
	return m_track->GetFullPath();
}
*/
void TLTrackVis::ReturnFileName(wchar_t* wszFileName)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto track_content_manager = manager->GetTrackContentManager();
	TrackPosInfo* pTrack_Pos_Info = track_content_manager->GetTrackPosInfo();
	char* szFileName = track_content_manager->WChar2Char(wszFileName);
	//char* szTime = PosToTimeInfo(pTrack_Pos_Info->fUnit,true);
	//float fPos = TimeInfoToPos(szTime);

#if _ENABLE_MEDIA_LEN
	auto pIOSourceCtrlDll = track_content_manager->GetIOSourceCtrlDll();
	auto pMediaFileInfo = pIOSourceCtrlDll->GetMediaFileInfo(wszFileName);
	
	float fOffset = 0.0;
	int iItemIndex = -1, iTrackIndex = -1;
	std::string fullpath = GetFullPath();
	int iTackIndexRet = track_content_manager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);

	if (iTackIndexRet != -1)
	{
		int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);

		MediaNode* pCurMediaNode = (MediaNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, iTotalOfNodes - 1);
		if (pCurMediaNode)
			fOffset = pCurMediaNode->fTime + pCurMediaNode->fLen + 0.03;
	}
/*	
	if (fOffset != 0.0)
		track_content_manager->Add(pTrack_Pos_Info->szFullPath, fOffset, szFileName, pMediaFileInfo->dLen);
	else 
	*/
		track_content_manager->Add(pTrack_Pos_Info->szFullPath, pTrack_Pos_Info->fUnit, szFileName, pMediaFileInfo->dLen);

		track_content_manager->Sort(iItemIndex, iTrackIndex);
#else
	manager->GetTrackContentManager()->Add(pTrack_Pos_Info->szFullPath, pTrack_Pos_Info->fUnit, szFileName);
#endif
	UpdateItemTotalTime();
}

void TLTrackVis::UpdateItemTotalTime()
{
	std::string fullpath = TLTrackVis::GetFullPath();
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	TrackContentManager* pTrackContentManager = manager->GetTrackContentManager();

	int iItemIndex, iTrackIndex;
	int iRet = pTrackContentManager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);
	float fTime = pTrackContentManager->GetMaximumTime(iItemIndex);

	manager->SetTotalTimeOfCurrentItem(TimeCode(fTime * 1000));
}

void TLTrackVis::UpdateTimeCode(Gdiplus::Graphics& graphics, TimeCode timecode)
{
#if 0
	std::string sFullPath = TLTrackVis::GetFullPath();
	m_ContentProcessor.SetFullPath((char*)sFullPath.c_str());
#else
	char* szFullPath = TLTrackVis::GetFullPath2();
	m_ContentProcessor.SetFullPath(szFullPath);
#endif
	m_ContentProcessor.UpdateTimeCode(graphics, timecode);
}

void TLTrackVis::UpdateTimeCode(TimeCode timecode)
{
#if 0
	std::string sFullPath = TLTrackVis::GetFullPath();
	m_ContentProcessor.SetFullPath((char*)sFullPath.c_str());
#else
	char* szFullPath = TLTrackVis::GetFullPath2();
	if (strcmp(szFullPath, "") == 0)
		return;
	m_ContentProcessor.SetFullPath(szFullPath);
#endif
	m_ContentProcessor.UpdateTimeCode(timecode);
}

std::string TLTrackVis::GetFullPath()
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	//CWnd* pWnd = manager->GetMediaServerDlgWnd();
	std::string item_uuid = manager->GetSelectedItem("");

	if (strcmp(item_uuid.c_str(), "") == 0)
		return "";

	auto item = manager->GetItem(item_uuid);
	if (!item.get())
		return "";

	std::string item_name = item->GetInfo().Name();

	if (strcmp(m_szTrackFullPath, "") == 0)
	{
		std::string sTrackPath = m_track->GetFullPath();
		strcpy(m_szTrackFullPath, sTrackPath.c_str());
	}

	m_TmpFullPath = item_name + "/" + m_szTrackFullPath;// m_track->GetFullPath();
	
	return m_TmpFullPath;
}

char* TLTrackVis::GetFullPath2()
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	//CWnd* pWnd = manager->GetMediaServerDlgWnd();
	std::string item_uuid = manager->GetSelectedItem("");

	if (strcmp(item_uuid.c_str(), "") == 0)
		return "";

	auto item = manager->GetItem(item_uuid);
	if (!item.get())
		return "";

	std::string item_name = item->GetInfo().Name();

	if (strcmp(m_szTrackFullPath, "") == 0)
	{
		std::string sTrackPath = m_track->GetFullPath();
		strcpy(m_szTrackFullPath, sTrackPath.c_str());
	}


	//m_TmpFullPath = item_name + "/" + m_szTrackFullPath;// m_track->GetFullPath();

	sprintf(m_szTmpFullPath, "%s/%s", item_name.c_str(), m_szTrackFullPath);
	return m_szTmpFullPath;
}
/*
char* TLTrackVis::PosToTimeInfo(float fPos, bool bEnableMS)
{
	int iHour = fPos / (60 * 60);

	double dNum = fPos - 3600 * iHour;
	int iMinute = dNum / 60;

	double dNum1 = dNum - 60 * iMinute;
	int iSecond = dNum1;

	if (bEnableMS)
	{
		double dNum2 = dNum1 - iSecond;
		int iMSec = dNum2 * 1000;

		if (iHour > 0)
			sprintf(m_szTime, "%02d:%02d:%02d.%03d", iHour, iMinute, iSecond, iMSec);
		else
			sprintf(m_szTime, "%02d:%02d:%02d.%03d",0, iMinute, iSecond, iMSec);
	}
	else
	{
		if (iHour > 0)
			sprintf(m_szTime, "%02d:%02d:%02d", iHour, iMinute, iSecond);
		else
			sprintf(m_szTime, "%02d:%02d:%02d", 0,iMinute, iSecond);
	}
	return m_szTime;
}

float TLTrackVis::TimeInfoToPos(char* szTime)
{
	float fPos = 0.0;
	char szHour[10];
	char szMinute[10];
	int iHour, iMinute, iSecond;
	char* pch = strchr(szTime, '.');
	int iMS = atoi(pch + 1);

	char* pch1 = strchr(szTime, ':');
	if (pch1)
	{
		int iHourLen = strlen(szTime) - strlen(pch1);
		memcpy(szHour, szTime, iHourLen);
		szHour[iHourLen] = '\0';

		iHour = atoi(szHour);
	}

	char* pch2 = strchr(pch1 + 1, ':');
	if (pch2)
	{
		int iMinuteLen = strlen(pch1) - strlen(pch2);
		memcpy(szMinute, pch1, iMinuteLen);
		szMinute[iMinuteLen] = '\0';

		iMinute = atoi(szMinute);
		iSecond = atoi(pch2 + 1);
	}

	fPos = iHour * 3600 + iMinute * 60 + iSecond + iMS / 1000.0;

	return fPos;
}
*/

void TLTrackVis::ChangeExpandStatus()
{
	if (m_track->HasChild())
	{
		bool bVisible = true;
		if (m_expandStatus == ExpandStatus::Collapse)
			bVisible = false;

		for (auto child : m_track->ChildTracks())
		{
			child->SetVisible(bVisible);
			if (child->HasChild())
				SetVisibleRecursively(child, bVisible);
		}
	}
}

void TLTrackVis::SetVisibleRecursively(TLTrackPtr& track, bool bVisible)
{
	for (auto child : track->ChildTracks())
	{
		child->SetVisible(bVisible);
		if (child->HasChild())
			SetVisibleRecursively(child, bVisible);	
	}
}

void TLTrackVis::SetCheckRange(bool bEnable)
{
	m_ContentProcessor.SetCheckRange(bEnable);
}

bool TLTrackVis::CheckStatus()
{
	if (m_bUpdate)
		return true;
	return false;
}

void TLTrackVis::OnKeyDown(unsigned int key)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto pointerMode = manager->GetPointerMode();
	auto track_content_manager = manager->GetTrackContentManager();
	auto track_type = m_track->TrackType();
	std::string name = this->m_track->Name();
	std::string fullpath = GetFullPath();

	int iItemIndex = -1, iTrackIndex = -1;
	int iTackIndexRet = manager->GetTrackContentManager()->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);

	MediaNode* pCurMediaNode = 0;
	int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
	if (iTotalOfNodes > m_iActiveIndex && m_iActiveIndex != -1)
	{
		pCurMediaNode = (MediaNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, m_iActiveIndex);
	}

	if (IsSelect())
	{
		switch (key)
		{
			case 189://VK_LEFT:
			{
				if (pointerMode == PointerMode::Move && pCurMediaNode)
				{
					if (strcmp(name.c_str(), "Source") == 0 || track_type == TLTrackType::Media)
					{
						float fTimeOffset = -0.5;

						if (pCurMediaNode->fTime != 0)
						{
							if (pCurMediaNode->fTime + fTimeOffset > 0.0)
								pCurMediaNode->fTime += fTimeOffset;
							else
								pCurMediaNode->fTime = 0.0;
							UpdateItemTotalTime();
						}
					}
				}
				break;
			}
			case 187://VK_RIGHT:
			{
				if (pointerMode == PointerMode::Move && pCurMediaNode)
				{
					if (strcmp(name.c_str(), "Source") == 0 || track_type == TLTrackType::Media)
					{
						float fTimeOffset = 0.5;

						if (pCurMediaNode->fTime + fTimeOffset < 604800.0)
						{
							pCurMediaNode->fTime += fTimeOffset;
							UpdateItemTotalTime();
						}
					}
				}
				break;
			}
		}
	}
}

void TLTrackVis::PreloadSource()
{
	std::string sFullPath = TLTrackVis::GetFullPath();
	m_ContentProcessor.SetFullPath((char*)sFullPath.c_str());

	m_ContentProcessor.PreloadSource();
}

bool TLTrackVis::CheckPBStatus()
{
	return m_ContentProcessor.CheckPBStatus();
}

void TLTrackVis::StopSource()
{
	m_ContentProcessor.StopVideoSource();
}

void TLTrackVis::InsertEffectToTrack(EffectInfo* pInfo)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto track_content_manager = manager->GetTrackContentManager();
	TrackPosInfo* pTrack_Pos_Info = track_content_manager->GetTrackPosInfo();

	float fOffset = 0.0;
	int iItemIndex = -1, iTrackIndex = -1;
	std::string fullpath = GetFullPath();
	int iTackIndexRet = track_content_manager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);

	int iParam[5] = { pInfo->iParameter1 ,pInfo->iParameter2 ,pInfo->iParameter3 ,pInfo->iParameter4 ,pInfo->iParameter5};
	track_content_manager->Add(pTrack_Pos_Info->szFullPath, pTrack_Pos_Info->fUnit, pInfo->iEffectID, pInfo->szName, pInfo->fDuration, pInfo->iLevel, iParam);
	track_content_manager->Sort(iItemIndex, iTrackIndex);
}

void TLTrackVis::AudioConfigOK()
{
	int iItemIndex = -1, iTrackIndex = -1;
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	std::string fullpath = GetFullPath();

	TrackContentManager* pTrackContentManager = manager->GetTrackContentManager();
	int iTackIndexRet = pTrackContentManager->GetItemTackIndex((char*)fullpath.c_str(), iItemIndex, iTrackIndex);

	AudioConfigWin* pAudioConfigWin = g_pUIController->GetAudioConfigWin();

	//pAudioConfigWin->IsChecked(int iIndex);
	//GetVolume();

	for (int j = 0; j < _MAX_SOURCES; j++)
	{
		bool bEnable = pAudioConfigWin->IsChecked(j);
		int iVolume = pAudioConfigWin->GetVolume(j);
		pTrackContentManager->EnableAudio(iItemIndex, j, bEnable);
		pTrackContentManager->SetVolume(iItemIndex, j, iVolume);

		OutDevice* pOutDevice = manager->GetOutDevice(ST_STREAM0 + j);
		if (pOutDevice)
		{
			pOutDevice->EnableAudio(bEnable);

			pOutDevice->SetVolume(iVolume);
		}
	}
}