/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineItemFacade.cpp

Definition of following classes:
TimelineItemFacade

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
#include "TimelineItemFacade.h"
#include "TLUtility.h"
#include "DoubleBuffer.h"
#include "ServiceProvider.h"


#define ENGINNERING_MODE


using namespace Timeline;

/* ====================================================== *\
TimelineItemFacade
\* ====================================================== */
TimelineItemFacade::TimelineItemFacade()
{}

TimelineItemFacade::~TimelineItemFacade()
{}

void TimelineItemFacade::OnInitialize(CWnd* wnd)
{
	m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	m_wnd = wnd;
	m_pDC = nullptr;

	m_manager->SetItemViewWnd(wnd);

	m_vertical_scroll_shift = 0;

	m_buttonPanel.AddClick.connect(boost::bind(&TimelineItemFacade::Add, this, _1));
	m_buttonPanel.DeleteClick.connect(boost::bind(&TimelineItemFacade::Delete, this, _1));
	m_buttonPanel.OpenClick.connect(boost::bind(&TimelineItemFacade::Open, this, _1));
	m_buttonPanel.SaveClick.connect(boost::bind(&TimelineItemFacade::Save, this, _1));

	m_manager->StatusChanged.connect(boost::bind(&TimelineItemFacade::StatusChanged, this, _1));
}

void TimelineItemFacade::OnTerminate()
{
	m_buttonPanel.AddClick.disconnect(boost::bind(&TimelineItemFacade::Add, this, _1));
	m_buttonPanel.DeleteClick.disconnect(boost::bind(&TimelineItemFacade::Delete, this, _1));
	m_buttonPanel.OpenClick.disconnect(boost::bind(&TimelineItemFacade::Open, this, _1));
	m_buttonPanel.SaveClick.disconnect(boost::bind(&TimelineItemFacade::Save, this, _1));

	m_manager->StatusChanged.disconnect(boost::bind(&TimelineItemFacade::StatusChanged, this, _1));
}

void TimelineItemFacade::Draw(Gdiplus::Graphics& graphics, const Gdiplus::Rect& rect)
{
	// =========== Draw background ==================== //
	Gdiplus::SolidBrush brush(Color(255, 32, 32, 32));
	graphics.FillRectangle(&brush, rect.X, rect.Y, rect.Width, rect.Height);

	m_ButtonPanelRect = CRect(0, 0, rect.Width, 32);

	// =========== Draw buttons ==================== //
	m_buttonPanel.Draw(graphics);

	// =========== Draw items ==================== //
	Region region(Rect(
		rect.X,
		TimelineItemViewButtonPanel::VisualConstant::Height + 5,
		rect.Width,
		rect.Height - TimelineItemViewButtonPanel::VisualConstant::Height - 5));
	graphics.SetClip(&region, CombineModeReplace);

	int x = 5;
	int y = 10 + TimelineItemViewButtonPanel::VisualConstant::Height + m_vertical_scroll_shift;

	auto visuals = m_manager->GetItemVisuals();
	if (visuals.size() > 0)
	{
		for (auto visual : visuals)
		{
			if (visual.get())
			{
				visual->SetPosition(x, y);
				visual->Draw(graphics);
				y += visual->Height() + 10;
			}
		}
	}
}

void TimelineItemFacade::OnCreate(const Gdiplus::Rect& rect)
{
	// m_pDC must be initialized here instead of the constructor
	// because the HWND isn't created until Create is called.
	m_pDC = new CClientDC(m_wnd);

	m_clientRect = rect;
	AdjustLayout(m_clientRect);
}

void TimelineItemFacade::OnDestroy()
{
	delete m_pDC;
	m_pDC = nullptr;
}

void TimelineItemFacade::OnSize(const Gdiplus::Rect& rect)
{
	m_clientRect = rect;
	AdjustLayout(m_clientRect);
	m_wnd->Invalidate(true);
}

void TimelineItemFacade::OnSizing(const Gdiplus::Rect& rect)
{
	m_clientRect = rect;
	AdjustLayout(m_clientRect);
	m_wnd->Invalidate(true);
}

void TimelineItemFacade::OnLButtonDown(const Gdiplus::Point& point)
{
	m_mousePosition = point;

	if (m_buttonPanel.HasCollision(m_mousePosition))
	{
		m_buttonPanel.MouseLeftButtonDown(m_mousePosition);
	}
	else
	{
		m_manager->ItemClick(m_mousePosition);
	}

	m_wnd->Invalidate(false);
}

void TimelineItemFacade::OnLButtonUp(const Gdiplus::Point& point)
{
	m_buttonPanel.MouseLeftButtonUp(m_mousePosition);
	m_manager->LButtonUp(m_mousePosition);
	m_wnd->Invalidate(false);
}

void TimelineItemFacade::OnMouseMove(const Gdiplus::Point& point)
{
	m_mousePosition = point;

	m_buttonPanel.MouseMove(m_mousePosition);

	if (m_ButtonPanelRect.PtInRect(CPoint(point.X, point.Y)))
	{
		bool bUpdate = m_buttonPanel.CheckStatus(point);

		if (bUpdate)
			m_wnd->Invalidate(false);

		return;
	}

	m_manager->OnMouseMove(m_mousePosition);

#ifdef ENGINNERING_MODE
#if false
	std::string output = boost::str(boost::format("TimelineItemFacade::OnMouseMove=(%d, %d)") % m_mousePosition.x % m_mousePosition.y);
	DumpToLog(output);
#endif
#endif // ENGINNERING_MODE

	if(m_manager->CheckItemViewStatus())
		m_wnd->Invalidate(false);
}

void TimelineItemFacade::OnMouseWheel(short zDelta, const Gdiplus::Point& point)
{
	auto temp = m_vertical_scroll_shift + zDelta / 4;
	if (temp <= 0)
	{
		m_vertical_scroll_shift = temp;
	}

	m_wnd->Invalidate(false);
}

void TimelineItemFacade::OnLButtonDblClk(const Gdiplus::Point& point)
{
	m_manager->ItemDbClick(m_mousePosition);
	m_wnd->Invalidate(false);
}

void TimelineItemFacade::OnKeyDown(unsigned int key)
{
	switch (key)
	{
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

void TimelineItemFacade::OnKillFocus()
{
	m_buttonPanel.DeselectAll();
	m_wnd->Invalidate(false);
}

void TimelineItemFacade::Add(std::string msg)
{
	m_manager->AddItem();
}

void TimelineItemFacade::Delete(std::string msg)
{
	m_manager->DeleteItem();
}

void TimelineItemFacade::Open(std::string msg)
{
#if 1

	//TCHAR szFilters[] = _T("Project Files (*.xml)|*.xml|All Files (*.*)|*.*||");
	//CFileDialog fileDlg(TRUE, _T("xml"), _T("*.xml"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	TCHAR szFilters[] = _T("Project Files (*.tlf)|*.tlf|(*.xml)|*.xml|All Files (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("tlf"), _T("*.tlf"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (fileDlg.DoModal() == IDOK)
	{
		CString filename = fileDlg.GetPathName();

		strcpy(m_szProjectFileName, std::string(filename.GetString()).c_str());

		/*
#ifdef TIMELINEDEV
		m_manager->LoadProject(std::string(CW2A(filename.GetString())));
#else
		m_manager->LoadProject(std::string(filename.GetString()));
#endif
*/
		CWnd* pWnd = m_manager->GetItemViewWnd();
		pWnd->PostMessage(WM_TM_OPEN_PROJECT_FILE,0,0);
		//WM_TM_OPEN_PROJECT_FILE;

	}

#else

	std::string sFileName = "D:\\test_video\\planet2\\test2.xml";
	m_manager->LoadProject(sFileName);
#endif

	m_buttonPanel.DeselectAll();
	m_wnd->Invalidate(false);
}

void TimelineItemFacade::Save(std::string msg)
{
	//TCHAR szFilters[] = _T("Project Files (*.xml)|*.xml|All Files (*.*)|*.*||");
	//CFileDialog fileDlg(FALSE, _T("xml"), _T("*.xml"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	TCHAR szFilters[] = _T("Project Files (*.tlf)|*.tlf|(*.xml)|*.xml|All Files (*.*)|*.*||");
	CFileDialog fileDlg(FALSE, _T("tlf"), _T("*.tlf"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (fileDlg.DoModal() == IDOK)
	{
		CString filename = fileDlg.GetPathName();

#ifdef TIMELINEDEV
		m_manager->LoadProject(std::string(CW2A(filename.GetString())));
#else

		if (PathFileExistsA(filename.GetBuffer()))
		{
			if (MessageBox(NULL, "The File Has Existed, Overwrite?", "Overwrite", MB_OKCANCEL | MB_TASKMODAL| MB_TOPMOST) == IDOK)
				//m_manager->LoadProject(std::string(filename.GetString()));
				m_manager->SaveProject(std::string(filename.GetString()));
		}
		else
			m_manager->SaveProject(std::string(filename.GetString()));
#endif
	}

	m_buttonPanel.DeselectAll();
	m_wnd->Invalidate(false);
}

void TimelineItemFacade::StatusChanged(std::string msg)
{
	m_wnd->Invalidate(false);
}

void TimelineItemFacade::AdjustLayout(const Gdiplus::Rect& rect)
{
	m_buttonPanel.SetPosition(rect.X, rect.Y);
	m_buttonPanel.SetSize(rect.Width, TimelineItemViewButtonPanel::VisualConstant::Height);
}

void TimelineItemFacade::OpenProjectFile()
{
	std::string sFileName = m_szProjectFileName;
	m_manager->LoadProject(sFileName);
}
