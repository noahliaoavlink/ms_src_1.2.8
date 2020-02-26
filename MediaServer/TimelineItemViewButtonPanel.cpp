/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineItemViewButtonPanel.cpp

Definition of following classes:
TimelineItemViewButton
TimelineItemViewButtonPanel

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
#include "TimelineItemViewButtonPanel.h"
#include "TLUtility.h"
#include "ServiceProvider.h"
#include "MediaServer.h"

#define ENGINNERING_MODE


using namespace Timeline;

/* ====================================================== *\
TimelineItemViewButtonPanel
\* ====================================================== */
#define OpenButtonText "Open"
#define SaveButtonText "Save"
#define AddButtonText "Add"
#define DeleteButtonText "Delete"

extern CMediaServerApp theApp;

TimelineItemViewButtonPanel::TimelineItemViewButtonPanel()
{
	m_iGroupIndex = 0;
	m_iButtonIndex = 0;

	m_rect.X = 0;
	m_rect.Y = 0;
	m_rect.Width = 100;
	m_rect.Height = VisualConstant::Height;

	// ==== Initializing buttons ==== //
	m_btnOpen.SetPosition(145, 10);
	m_btnOpen.SetSize(30, 30);
	m_btnOpen.SetText(OpenButtonText);

	std::string szOpen = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\open.bmp";
	m_btnOpen.SetImage(0,szOpen); 
	std::string szOpen1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\open_t.bmp";
	m_btnOpen.SetImage(1, szOpen1); 
	std::string szOpen2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\open_c.bmp";
	m_btnOpen.SetImage(2, szOpen2);

	m_btnSave.SetPosition(190, 10);
	m_btnSave.SetSize(30, 30);
	m_btnSave.SetText(SaveButtonText);
	std::string szSave = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\save.bmp";
	m_btnSave.SetImage(0,szSave); 
	std::string szSave1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\save_t.bmp";
	m_btnSave.SetImage(1, szSave1);
	std::string szSave2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\save_c.bmp";
	m_btnSave.SetImage(2, szSave2);

	m_btnAdd.SetPosition(235, 10);
	m_btnAdd.SetSize(30, 30);
	m_btnAdd.SetText(AddButtonText);
	std::string szAdd = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\add.bmp";
	m_btnAdd.SetImage(0,szAdd); 
	std::string szAdd1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\add_t.bmp";
	m_btnAdd.SetImage(1, szAdd1); 
	std::string szAdd2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\add_c.bmp";
	m_btnAdd.SetImage(2, szAdd2);

	m_btnDelete.SetPosition(280, 10);
	m_btnDelete.SetSize(30, 30);
	m_btnDelete.SetText(DeleteButtonText);
	std::string szDelete = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\delete.bmp";
	m_btnDelete.SetImage(0,szDelete); 
	std::string szDelete1 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\delete_t.bmp";
	m_btnDelete.SetImage(1, szDelete1); 
	std::string szDelete2 = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\delete_c.bmp";
	m_btnDelete.SetImage(2, szDelete2);
		
	m_buttonCollection.push_back(&m_btnOpen);
	m_buttonCollection.push_back(&m_btnSave);
	m_buttonCollection.push_back(&m_btnAdd);
	m_buttonCollection.push_back(&m_btnDelete);

	//m_logo_image
	std::string szLogo = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\logo.png";
	std::wstring widestr = std::wstring(szLogo.begin(), szLogo.end());
	m_logo_image = std::make_shared<Gdiplus::Image>(widestr.c_str());

	//m_split_line_image;
	std::string szSplitLine = theApp.m_strCurPath + "\\UI Folder\\TimeLine\\split_line.png";
	widestr = std::wstring(szSplitLine.begin(), szSplitLine.end());
	m_split_line_image = std::make_shared<Gdiplus::Image>(widestr.c_str());
}

TimelineItemViewButtonPanel::~TimelineItemViewButtonPanel()
{
	m_buttonCollection.clear();
}

void TimelineItemViewButtonPanel::MouseLeftButtonDown(const Gdiplus::Point& point)
{
	bool hasCollision = false;

	// Check objects collision
	for (auto obj : m_buttonCollection)
	{
		if (obj->HasCollision(this->m_mouseEvent.MousePosition))
		{
			hasCollision = true;
			break;
		}
	}

	// If there have an object got hit by mouse, then send click message.
	if (hasCollision)
	{
		for (auto obj : m_buttonCollection)
		{
			obj->Deselect();
			if (obj->HasCollision(this->m_mouseEvent.MousePosition))
			{
				obj->Select();

				if (obj->Text() == OpenButtonText)
				{
					this->OpenClick("OpenClick");
				}

				if (obj->Text() == SaveButtonText)
				{
					this->SaveClick("SaveClick");
				}

				if (obj->Text() == AddButtonText)
				{
					this->AddClick("AddClick");
				}

				if (obj->Text() == DeleteButtonText)
				{
					this->DeleteClick("DeleteClick");
				}
			}
		}
	}
	else
	{
		for (auto obj : m_buttonCollection)
		{
			obj->Deselect();
		}
	}
}

void TimelineItemViewButtonPanel::MouseLeftButtonUp(const Gdiplus::Point& point)
{
	for (auto obj : m_buttonCollection)
	{
		obj->Deselect();
	}
}

void TimelineItemViewButtonPanel::DeselectAll()
{
	for (auto obj : m_buttonCollection)
	{
		obj->Deselect();
	}
}

void TimelineItemViewButtonPanel::Draw(Gdiplus::Graphics& graphics)
{
	// =========== Draw background ==================== //
	Gdiplus::LinearGradientBrush brush(
		Gdiplus::Point(0, 0),
		Gdiplus::Point(0, m_rect.Height),
		Gdiplus::Color(255, 86, 86, 86),
		Gdiplus::Color(255, 70, 70, 70));
	graphics.FillRectangle(&brush, m_rect.X, m_rect.Y, m_rect.Width, m_rect.Height);

	// Set the mouse position for each button
	auto& collection = m_buttonCollection;

	for (auto o : collection)
	{
		o->MouseLeftButtonDown(m_mouseEvent.MousePosition);
	}

	// Draw buttons
	for (auto o : collection)
	{
		o->Draw(graphics);
	}

	Gdiplus::Image* image = m_logo_image.get();
	graphics.DrawImage(image, 8, 12, image->GetWidth(), image->GetHeight());

	image = m_split_line_image.get();
	graphics.DrawImage(image, 118, 9, image->GetWidth(), image->GetHeight());
}

bool TimelineItemViewButtonPanel::CheckStatus(const Gdiplus::Point& point)
{
	bool fRet = false;
	int iCurGroupIndex = 0;
	int iCurButtonIndex = 0;

	int iCount = 0;

	for (auto obj : m_buttonCollection)
	{
		if (obj->HasCollision(this->m_mouseEvent.MousePosition))
		{
			iCurGroupIndex = 1;
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