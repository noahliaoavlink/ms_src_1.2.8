/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineEditViewButtonPanel.cpp

Definition of following classes:
TimelineEditViewButton
TimelineEditViewButtonPanel

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
#include "TimelineEditViewButtonPanel.h"
#include "ServiceProvider.h"
#include "MediaServer.h"

#define ENGINNERING_MODE

using namespace Timeline;

/* ====================================================== *\
TimelineEditViewButtonPanel
\* ====================================================== */
#define StartButtonText "Start"
#define PauseButtonText "Pause"
#define StopButtonText "Stop"
#define EditButtonText "Edit"
#define InsertPointBtnText "Insert_Point"
#define InsertVideoBtnText "Insert_Video"
#define MoveButtonText "Move"
#define BackButtonText "Back"
#define NextButtonText "Next"
#define LineNormalButtonText "Normal"
#define LineSoarButtonText "Soar"
#define LinePlungeButtonText "Plunge"
#define LineFlatSoarButtonText "FlatSoar"
#define LineCurveButtonText "Curve"

#define Unit1ButtonText "10s"
#define Unit2ButtonText "1m"
#define Unit3ButtonText "10m"

extern CMediaServerApp theApp;

TimelineEditViewButtonPanel::TimelineEditViewButtonPanel()
{
	m_iGroupIndex = 0;
	m_iButtonIndex = 0;

	/*
	// ==== Initializing button group1 ==== //
	m_btnStart.SetPosition(10, 10);
	m_btnStart.SetSize(30, 30);
	m_btnStart.SetText(StartButtonText);
	std::string szStart = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\start.png";
	m_btnStart.SetImage(szStart); //"start.png"

	m_btnPause.SetPosition(45, 10);
	m_btnPause.SetSize(30, 30);
	m_btnPause.SetText(PauseButtonText);
	std::string szPause = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\pause.png";
	m_btnPause.SetImage(szPause); //"pause.png"

	m_btnStop.SetPosition(80, 10);
	m_btnStop.SetSize(30, 30);
	m_btnStop.SetText(StopButtonText);
	std::string szStop = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\stop.png";
	m_btnStop.SetImage(szStop); //"stop.png"
	*/

	//TimelineEditViewButton m_btnBack;
	//TimelineEditViewButton m_btnNext;
	m_btnBack.SetPosition(10, 8);
	m_btnBack.SetSize(31, 31);
	m_btnBack.SetText(BackButtonText);
	std::string szBack = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\back.bmp";
	m_btnBack.SetImage(0,szBack);
	std::string szBack1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\back_t.bmp";
	m_btnBack.SetImage(1, szBack1);
	std::string szBack2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\back_c.bmp";
	m_btnBack.SetImage(2, szBack2);

	m_btnNext.SetPosition(150, 8);
	m_btnNext.SetSize(31, 31);
	m_btnNext.SetText(NextButtonText);
	std::string szNext = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\next.bmp";
	m_btnNext.SetImage(0,szNext);
	std::string szNext1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\next_t.bmp";
	m_btnNext.SetImage(1, szNext1);
	std::string szNext2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\next_c.bmp";
	m_btnNext.SetImage(2, szNext2);

//	m_buttonGroup1.push_back(&m_btnStart);
//	m_buttonGroup1.push_back(&m_btnStop);
//	m_buttonGroup1.push_back(&m_btnPause);
	m_buttonGroup1.push_back(&m_btnBack);
	m_buttonGroup1.push_back(&m_btnNext);

	// ==== Initializing button group2 ==== //
	m_btnEdit.SetPosition(10, 8);
	m_btnEdit.SetSize(31, 31);
	m_btnEdit.SetText(EditButtonText);
	std::string szEdit = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\mouse.bmp";
	m_btnEdit.SetImage(0,szEdit);  //"pointer.png"
	m_btnEdit.Select();
	std::string szEdit1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\mouse_t.bmp";
	m_btnEdit.SetImage(1,szEdit1);  //"pointer.png"
	std::string szEdit2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\mouse_c.bmp";
	m_btnEdit.SetImage(2, szEdit2);  //"pointer.png"

//#define InsertPointBtnText "Insert_Point"
//#define InsertVideoBtnText "Insert_Video"

	m_btnInsert_Point.SetPosition(150, 8);
	m_btnInsert_Point.SetSize(30, 30);
	m_btnInsert_Point.SetText(InsertPointBtnText);
	std::string szInsertPoint = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\add-point.bmp";
	m_btnInsert_Point.SetImage(0, szInsertPoint); //"command.png"
	std::string szInsertPoint1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\add-point_t.bmp";
	m_btnInsert_Point.SetImage(1, szInsertPoint1); //"command.png"
	std::string szInsertPoint2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\add-point_c.bmp";
	m_btnInsert_Point.SetImage(2, szInsertPoint2); //"command.png"

	//m_btnInsert_Video
	m_btnInsert_Video.SetPosition(150, 8);
	m_btnInsert_Video.SetSize(30, 30);
	m_btnInsert_Video.SetText(InsertVideoBtnText);
	std::string szInsertVideo = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\add-video.bmp";
	m_btnInsert_Video.SetImage(0, szInsertVideo); //"command.png"
	std::string szInsertVideo1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\add-video_t.bmp";
	m_btnInsert_Video.SetImage(1, szInsertVideo1); //"command.png"
	std::string szInsertVideo2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\add-video_c.bmp";
	m_btnInsert_Video.SetImage(2, szInsertVideo2); //"command.png"

	m_btnMove.SetPosition(150, 8);
	m_btnMove.SetSize(31, 31);
	m_btnMove.SetText(MoveButtonText);
	std::string szHand = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\hand.bmp";
	m_btnMove.SetImage(0,szHand); //"hand.png"
	std::string szHand1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\hand_t.bmp";
	m_btnMove.SetImage(1, szHand1); //"hand.png"
	std::string szHand2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\hand_c.bmp";
	m_btnMove.SetImage(2, szHand2); //"hand.png"

	m_buttonGroup2.push_back(&m_btnEdit);
	m_buttonGroup2.push_back(&m_btnInsert_Point);
	m_buttonGroup2.push_back(&m_btnInsert_Video);
	m_buttonGroup2.push_back(&m_btnMove);

	// ==== Initializing button group3 ==== //
	/*
	m_btnLineNormal.SetPosition(10, 8);
	m_btnLineNormal.SetSize(31, 31);
	m_btnLineNormal.SetText(LineNormalButtonText);
	std::string szLine = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\line.bmp";
	m_btnLineNormal.SetImage(0,szLine);
	m_btnLineNormal.Select();
	std::string szLine1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\line_t.bmp";
	m_btnLineNormal.SetImage(1, szLine1);
	std::string szLine2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\line_c.bmp";
	m_btnLineNormal.SetImage(2, szLine2);

	m_btnLineSoar.SetPosition(150, 8);
	m_btnLineSoar.SetSize(31, 31);
	m_btnLineSoar.SetText(LineSoarButtonText);
	std::string szSetup2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\step_up2.bmp";
	m_btnLineSoar.SetImage(0,szSetup2);
	std::string szSetup2_1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\step_up2_t.bmp";
	m_btnLineSoar.SetImage(1, szSetup2_1);
	std::string szSetup2_2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\step_up2_c.bmp";
	m_btnLineSoar.SetImage(2, szSetup2_2);

	m_btnLinePlunge.SetPosition(150, 8);
	m_btnLinePlunge.SetSize(31, 31);
	m_btnLinePlunge.SetText(LinePlungeButtonText);
	std::string szSetup3 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\step_up3.bmp";
	m_btnLinePlunge.SetImage(0,szSetup3);
	std::string szSetup3_1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\step_up3_t.bmp";
	m_btnLinePlunge.SetImage(1, szSetup3_1);
	std::string szSetup3_2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\step_up3_c.bmp";
	m_btnLinePlunge.SetImage(2, szSetup3_2);

	m_btnLineFlatSoar.SetPosition(150, 8);
	m_btnLineFlatSoar.SetSize(31, 31);
	m_btnLineFlatSoar.SetText(LineFlatSoarButtonText);
	std::string szSetup = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\step_up.bmp";
	m_btnLineFlatSoar.SetImage(0,szSetup);
	std::string szSetup_1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\step_up_t.bmp";
	m_btnLineFlatSoar.SetImage(1, szSetup_1);
	std::string szSetup_2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\step_up_c.bmp";
	m_btnLineFlatSoar.SetImage(2, szSetup_2);

	m_btnLineCurve.SetPosition(150, 8);
	m_btnLineCurve.SetSize(31, 31);
	m_btnLineCurve.SetText(LineCurveButtonText);
	std::string szBeiser = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\beiser.bmp";
	m_btnLineCurve.SetImage(0,szBeiser);
	std::string szBeiser1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\beiser_t.bmp";
	m_btnLineCurve.SetImage(1, szBeiser1);
	std::string szBeiser2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\beiser_c.bmp";
	m_btnLineCurve.SetImage(2, szBeiser2);

	m_buttonGroup3.push_back(&m_btnLineNormal);
	m_buttonGroup3.push_back(&m_btnLineSoar);
	m_buttonGroup3.push_back(&m_btnLinePlunge);
	m_buttonGroup3.push_back(&m_btnLineFlatSoar);
	m_buttonGroup3.push_back(&m_btnLineCurve);
	*/

	m_btnUnit1.SetPosition(45, 8);
	m_btnUnit1.SetSize(29, 29);
	m_btnUnit1.SetText(Unit1ButtonText);
	m_btnUnit1.Select();

	m_btnUnit2.SetPosition(80, 8);
	m_btnUnit2.SetSize(29, 29);
	m_btnUnit2.SetText(Unit2ButtonText);

	m_btnUnit3.SetPosition(115, 8);
	m_btnUnit3.SetSize(29, 29);
	m_btnUnit3.SetText(Unit3ButtonText);

	m_buttonGroup4.push_back(&m_btnUnit1);
	m_buttonGroup4.push_back(&m_btnUnit2);
	m_buttonGroup4.push_back(&m_btnUnit3);

	m_pointerMode = PointerMode::Edit;
	m_LineMode = LineMode::LM_NORMAL;

	/*
	//m_split_line_image;
	std::string szSplitLine = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\split_line.png";
	std::wstring widestr = std::wstring(szSplitLine.begin(), szSplitLine.end());
	m_split_line_image = std::make_shared<Gdiplus::Image>(widestr.c_str());
	*/
}

TimelineEditViewButtonPanel::~TimelineEditViewButtonPanel()
{
	m_buttonGroup1.clear();
	m_buttonGroup2.clear();
//	m_buttonGroup3.clear();
	m_buttonGroup4.clear();
}

void TimelineEditViewButtonPanel::SetRect(int x, int y, int width, int height)
{
	TLShape::SetRect(x, y, width, height);
	AdjustLayout();
}

void TimelineEditViewButtonPanel::SetRect(const Gdiplus::Rect& rect)
{
	this->SetRect(rect.X, rect.Y, rect.Width, rect.Height);
}

bool TimelineEditViewButtonPanel::MouseLeftButtonDown(const Gdiplus::Point& point)
{
	// ============== Group1 mouse event handler ===================//
	bool group1HasCollision = false;

	// Check objects collision
	for (auto obj : m_buttonGroup1)
	{
		if (obj->HasCollision(this->m_mouseEvent.MousePosition))
		{
			group1HasCollision = true;
			break;
		}
	}

	// If there have an object got hit by mouse, then send click message.
	if (group1HasCollision)
	{
		for (auto obj : m_buttonGroup1)
		{
			obj->Deselect();
			if (obj->HasCollision(this->m_mouseEvent.MousePosition))
			{
				obj->Select();

				if (obj->Text() == StartButtonText)
				{
					this->StartClick("StartClick");
				}

				if (obj->Text() == StopButtonText)
				{
					this->StopClick("StopClick");
				}

				if (obj->Text() == PauseButtonText)
				{
					this->PauseClick("PauseClick");
				}

				if (obj->Text() == BackButtonText)
				{
					this->BackClick("BackClick");
				}

				if (obj->Text() == NextButtonText)
				{
					this->NextClick("NextClick");
				}

			}
		}
	}
	else
	{
		for (auto obj : m_buttonGroup1)
		{
			obj->Deselect();
		}
	}

	// ============== Group2 mouse event handler ===================//
	bool group2HasCollision = false;

	// Check objects collision
	for (auto obj : m_buttonGroup2)
	{
		if (obj->HasCollision(this->m_mouseEvent.MousePosition))
		{
			group2HasCollision = true;
			break;
		}
	}

	// If there have an object got hit by mouse, then send click message.
	if (group2HasCollision)
	{
		for (auto obj : m_buttonGroup2)
		{
			obj->Deselect();
			if (obj->HasCollision(this->m_mouseEvent.MousePosition))
			{
				obj->Select();

				if (obj->Text() == EditButtonText)
				{
					this->m_pointerMode = PointerMode::Edit;
					this->PointerModeChanged(this->m_pointerMode);
				}

				if (obj->Text() == InsertPointBtnText)
				{
					this->m_pointerMode = PointerMode::Insert_Point;
					this->PointerModeChanged(this->m_pointerMode);
				}

				if (obj->Text() == InsertVideoBtnText)
				{
					this->m_pointerMode = PointerMode::Insert_Video;
					this->PointerModeChanged(this->m_pointerMode);
				}

				if (obj->Text() == MoveButtonText)
				{
					this->m_pointerMode = PointerMode::Move;
					this->PointerModeChanged(this->m_pointerMode);
				}
			}
		}
	}

	// ============== Group3 mouse event handler ===================//
	/*
	bool group3HasCollision = false;

	// Check objects collision
	for (auto obj : m_buttonGroup3)
	{
		if (obj->HasCollision(this->m_mouseEvent.MousePosition))
		{
			group3HasCollision = true;
			break;
		}
	}

	// If there have an object got hit by mouse, then send click message.
	/*
	if (group3HasCollision)
	{
		for (auto obj : m_buttonGroup3)
		{
			obj->Deselect();
			if (obj->HasCollision(this->m_mouseEvent.MousePosition))
			{
				obj->Select();

				if (obj->Text() == LineNormalButtonText)
				{
					m_LineMode = LineMode::LM_NORMAL;
					LineModeChanged(m_LineMode);
				}

				if (obj->Text() == LineSoarButtonText)
				{
					m_LineMode = LineMode::LM_SOAR;
					LineModeChanged(m_LineMode);
				}

				if (obj->Text() == LinePlungeButtonText)
				{
					m_LineMode = LineMode::LM_PLUNGE;
					LineModeChanged(m_LineMode);
				}

				if (obj->Text() == LineFlatSoarButtonText)
				{
					m_LineMode = LineMode::LM_FLAT_SOAR;
					LineModeChanged(m_LineMode);
				}

				if (obj->Text() == LineCurveButtonText)
				{
					m_LineMode = LineMode::LM_CURVE;
					LineModeChanged(m_LineMode);
				}
			}
		}
	}
	*/
	// ============== Group4 mouse event handler ===================//
	bool group4HasCollision = false;

	// Check objects collision
	for (auto obj : m_buttonGroup4)
	{
		if (obj->HasCollision(this->m_mouseEvent.MousePosition))
		{
			group4HasCollision = true;
			break;
		}
	}

	// If there have an object got hit by mouse, then send click message.
	if (group4HasCollision)
	{
		for (auto obj : m_buttonGroup4)
		{
			obj->Deselect();
			if (obj->HasCollision(this->m_mouseEvent.MousePosition))
			{
				obj->Select();

				if (obj->Text() == Unit1ButtonText)
				{
					m_UnitMode = UnitMode::UM_10_SEC;
					UnitModeChanged(m_UnitMode);
				}

				if (obj->Text() == Unit2ButtonText)
				{
					m_UnitMode = UnitMode::UM_1_MIN;
					UnitModeChanged(m_UnitMode);
				}

				if (obj->Text() == Unit3ButtonText)
				{
					m_UnitMode = UnitMode::UM_10_MINS;
					UnitModeChanged(m_UnitMode);
				}
			}
		}
	}

	//return group1HasCollision || group2HasCollision || group3HasCollision || group4HasCollision;
	return group1HasCollision || group2HasCollision || group4HasCollision;
}

void TimelineEditViewButtonPanel::MouseLeftButtonUp(const Gdiplus::Point& point)
{
	for (auto obj : m_buttonGroup1)
	{
		//obj->Deselect();
		if (obj->IsSelect())
		{
			obj->Deselect();
		}
	}
}

void TimelineEditViewButtonPanel::Draw(Gdiplus::Graphics& graphics)
{
	// Draw background
	Gdiplus::LinearGradientBrush brush(
		Gdiplus::Point(0, 0),
		//Gdiplus::Point(0, 150),
		Gdiplus::Point(0, 75),
		Gdiplus::Color(255, 86, 86, 86),
		Gdiplus::Color(255, 70, 70, 70));
	graphics.FillRectangle(&brush, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height);

	// Draw button group1
	for (auto o : m_buttonGroup1)
	{
		o->MouseLeftButtonDown(m_mouseEvent.MousePosition);
		o->Draw(graphics);
	}

	// Draw button group2
	for (auto o : m_buttonGroup2)
	{
		o->MouseLeftButtonDown(m_mouseEvent.MousePosition);
		o->Draw(graphics);
	}
	/*
	// Draw button group3
	for (auto o : m_buttonGroup3)
	{
		o->MouseLeftButtonDown(m_mouseEvent.MousePosition);
		o->Draw(graphics);
	}
	*/
	// Draw button group4
	for (auto o : m_buttonGroup4)
	{
		o->MouseLeftButtonDown(m_mouseEvent.MousePosition);
		o->Draw(graphics);
	}
/*
	int right = m_rect.X + m_rect.Width;
	Gdiplus::Image* image = image = m_split_line_image.get();
	graphics.DrawImage(image, right - 200, 9, image->GetWidth(), image->GetHeight());
	*/
}

PointerMode TimelineEditViewButtonPanel::GetPointerMode()
{
	return this->m_pointerMode;
}

void TimelineEditViewButtonPanel::AdjustLayout()
{
	int right = m_rect.X + m_rect.Width;

	// ==== Adjust button group1 ==== //

	// ==== Adjust button group2 ==== //
	m_btnEdit.SetPosition(right - m_btnEdit.Width() - m_btnInsert_Point.Width() - m_btnInsert_Video.Width() - m_btnMove.Width() - m_btnLineNormal.Width() - m_btnLineSoar.Width() - m_btnLinePlunge.Width() - m_btnLineFlatSoar.Width() - m_btnLineCurve.Width() - 65, m_rect.Y + 10);
	m_btnInsert_Point.SetPosition(right - m_btnInsert_Point.Width() - m_btnInsert_Video.Width() - m_btnMove.Width() - m_btnLineNormal.Width() - m_btnLineSoar.Width() - m_btnLinePlunge.Width() - m_btnLineFlatSoar.Width() - m_btnLineCurve.Width() - 60, m_rect.Y + 10);
	m_btnInsert_Video.SetPosition(right - m_btnInsert_Video.Width() - m_btnMove.Width() - m_btnLineNormal.Width() - m_btnLineSoar.Width() - m_btnLinePlunge.Width() - m_btnLineFlatSoar.Width() - m_btnLineCurve.Width() - 55, m_rect.Y + 10);
	m_btnMove.SetPosition(right - m_btnMove.Width() - m_btnLineNormal.Width() - m_btnLineSoar.Width() - m_btnLinePlunge.Width() - m_btnLineFlatSoar.Width() - m_btnLineCurve.Width() - 50, m_rect.Y + 10);

	// ==== Adjust button group3 ==== //
	m_btnLineNormal.SetPosition(right - m_btnLineNormal.Width() - m_btnLineSoar.Width() - m_btnLinePlunge.Width() - m_btnLineFlatSoar.Width() - m_btnLineCurve.Width() - 30, m_rect.Y + 10);
	m_btnLineSoar.SetPosition(right - m_btnLineSoar.Width() - m_btnLinePlunge.Width() - m_btnLineFlatSoar.Width() - m_btnLineCurve.Width() - 25, m_rect.Y + 10);
	m_btnLinePlunge.SetPosition(right - m_btnLinePlunge.Width() - m_btnLineFlatSoar.Width() - m_btnLineCurve.Width() - 20, m_rect.Y + 10);
	m_btnLineFlatSoar.SetPosition(right - m_btnLineFlatSoar.Width() - m_btnLineCurve.Width() - 15, m_rect.Y + 10);
	m_btnLineCurve.SetPosition(right - m_btnLineCurve.Width() - 10, m_rect.Y + 10);
}

bool TimelineEditViewButtonPanel::CheckStatus(const Gdiplus::Point& point)
{
	bool fRet = false;
	int iCurGroupIndex = 0;
	int iCurButtonIndex = 0;

	int iCount = 0;

	// Check objects collision
	for (auto obj : m_buttonGroup1)
	{
		if (obj->HasCollision(this->m_mouseEvent.MousePosition))
		{
			iCurGroupIndex = 1;
			iCurButtonIndex = iCount + 1;
			break;
		}
		iCount++;
	}

	iCount = 0;
	for (auto obj : m_buttonGroup2)
	{
		if (obj->HasCollision(this->m_mouseEvent.MousePosition))
		{
			iCurGroupIndex = 2;
			iCurButtonIndex = iCount + 1;
			break;
		}
		iCount++;
	}

	/*
	iCount = 0;
	for (auto obj : m_buttonGroup3)
	{
		if (obj->HasCollision(this->m_mouseEvent.MousePosition))
		{
			iCurGroupIndex = 3;
			iCurButtonIndex = iCount + 1;
			break;
		}
		iCount++;
	}
	*/

	iCount = 0;
	for (auto obj : m_buttonGroup4)
	{
		if (obj->HasCollision(this->m_mouseEvent.MousePosition))
		{
			iCurGroupIndex = 4;
			iCurButtonIndex = iCount + 1;
			break;
		}
		iCount++;
	}

	if (m_iGroupIndex != iCurGroupIndex || m_iButtonIndex != iCurButtonIndex)
	{
		m_iGroupIndex = iCurGroupIndex;
		m_iButtonIndex = iCurButtonIndex;
		fRet = true;
	}

	return fRet;
}
