/********************************************************************\
Project Name: Timeline Prototype

File Name: TimelineManager.cpp

Definition of following classes:
TimelineManager

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
#include <limits>
#include <sstream>
#include <boost/format.hpp>

// ==== Include local library ==== //
#include "tinyxml2.h"
#include "TimelineManager.h"
#include "TLUtility.h"
#include "MediaServer.h"
#include "ServiceProvider.h"

#ifdef _NEW_TIME_LINE
#include "TimeLineSettingsDlg.h"
#endif

#define ENGINNERING_MODE

using namespace Timeline;
extern CMediaServerApp theApp;

/* ====================================================== *\
TimelineManager
\* ====================================================== */

TimelineManager::TimelineManager()
{
	this->m_itemIdCount = 0;
	this->m_trackIdCount = 0;

	this->m_pointerMode = PointerMode::Edit;
	m_LineMode = LineMode::LM_NORMAL;
	m_UnitMode = UnitMode::UM_10_SEC;

	m_selectedItemUuid = "";
	m_pItemViewWnd = 0;
	m_pEditViewWnd = 0;
	m_pMediaServerDlgWnd = 0;

	m_pTrackContentManager = new TrackContentManager;
	m_pTL_Out_Device = new TL_Out_Device;
	m_pDisplayManager = new DisplayManager;
	m_pTCAdapter = new TCAdapter;

#ifdef _NEW_TIME_LINE

#else

#ifdef ENGINNERING_MODE
#if true
	LoadProject("timeline_project.xml");
#endif
#endif // ENGINNERING_MODE
	
#endif
}

TimelineManager::~TimelineManager()
{
	if (m_pTrackContentManager)
		delete m_pTrackContentManager;

	m_items.clear();
	m_itemVisuals.clear();

	m_tracks.clear();
	m_trackVisuals.clear();

	if (m_pTL_Out_Device)
		delete m_pTL_Out_Device;

	if (m_pDisplayManager)
		delete m_pDisplayManager;

	if (m_pTCAdapter)
		delete m_pTCAdapter;
}

void TimelineManager::FreeObj()
{
	if (m_pTCAdapter)
		m_pTCAdapter->FreeObj();
}

void TimelineManager::SetItemViewWnd(CWnd* pWnd)
{
	m_pItemViewWnd = pWnd;
}

void TimelineManager::SetEditViewWnd(CWnd* pWnd)
{
	m_pEditViewWnd = pWnd;

	m_pTrackContentManager->SetEditViewWnd(pWnd);
}

CWnd* TimelineManager::GetEditViewWnd()
{
	return m_pEditViewWnd;
}

CWnd* TimelineManager::GetItemViewWnd()
{
	return m_pItemViewWnd;
}

TrackContentManager* TimelineManager::GetTrackContentManager()
{
	return m_pTrackContentManager;
}
/*
void TimelineManager::SetPreviewWnd(HWND hWnd)
{
	m_hPreviewWnd = hWnd;
}

HWND TimelineManager::GetPreviewWnd()
{
	return m_hPreviewWnd;
}
*/

void TimelineManager::SetOutDevice(int iType, OutDevice* pObj)
{
	m_pTL_Out_Device->SetOutDevice(iType, pObj);
}

OutDevice* TimelineManager::GetOutDevice(int iType)
{
	return m_pTL_Out_Device->GetOutDevice(iType);
}

void TimelineManager::AddItem()
{
	// todo: Modify the AddItem operation to replace the current dummy code.
	// Should be as follows:
	//     Open a dialog and enter some Name/Time, then create a timeline item.
	char szCurItemName[128];

	if (GetPBStatus() == PBC_PLAY || GetPBStatus() == PBC_PAUSE)
	{
		PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_PAUSE, 0, 0);
	}

#ifdef _NEW_TIME_LINE
	CTimeLineSettingsDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		if (GetPBStatus() == PBC_PLAY || GetPBStatus() == PBC_PAUSE)
		{
			SendMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_RESET_TIME_CODE, 0, 0);
			//m_pDisplayManager->ClearAll();
			ClearAll();
			m_pDisplayManager->ResetAllD3DSurfaces();
			DoPlayback_AllOutDevices(PBC_CLOSE);
		}

		std::string sName = dlg.GetName();
		int iID = dlg.GetID();

		strcpy(szCurItemName, sName.c_str());

		if (iID == -1)
		{
			MessageBox(NULL,"Input ID was invalidated!!", "Warning", MB_OK| MB_TOPMOST);
			return;
		}

		for (auto it = m_items.begin(); it != m_items.end(); ++it)
		{
			auto item = it->second;
			if (item.get())
			{
				if (strcmp(item->GetInfo().Name().c_str(), sName.c_str()) == 0)
				{
					MessageBox(NULL, "Cannot have the same name!!", "Warning", MB_OK| MB_TOPMOST);
					return;
				}
			}
		}
		//Cannot have the same name

#ifdef	MULTI_STREAM
		//bool bSampleVersion = GetPrivateProfileInt("System", "Sample Version", 0, theApp.m_strCurPath + "Setting.ini");

		std::string szProjectFile;

#ifdef _TRAIL_VERSION
		szProjectFile = theApp.m_strCurPath + "trail_timeline_prj.xml";
#else  //_TRAIL_VERSION

#ifdef _ENABLE_VIDEO_WALL
		szProjectFile = theApp.m_strCurPath + "timeline_project.xml";
#else  //_ENABLE_VIDEO_WALL
		if(theApp.m_bSampleVersion)
			szProjectFile = theApp.m_strCurPath + "timeline_project2.xml";
		else
		{
#ifdef _ENABLE_SINGLE_EFFECT
			szProjectFile = theApp.m_strCurPath + "timeline_project1.xml";
#else     //_ENABLE_SINGLE_EFFECT
			szProjectFile = theApp.m_strCurPath + "timeline_project3.xml";
#endif    //_ENABLE_SINGLE_EFFECT
		}
#endif  //_ENABLE_VIDEO_WALL

#endif //_TRAIL_VERSION

#else  //MULTI_STREAM
		std::string szProjectFile = theApp.m_strCurPath + "timeline_project.xml";

#endif //MULTI_STREAM

		LoadProject(szProjectFile.c_str(), sName, iID);

		PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_REDRAW, 0, 0);
		//m_pEditViewWnd->Invalidate(false);
	}
	else
		return;

	//m_pDisplayManager->ClearAll();
	ClearAll();

	TLItemVisPtr pLastItem = 0;
	auto visuals = this->GetItemVisuals();
	for (auto o : visuals)
	{
		if (o.get())
		{
			if (strcmp(o->GetContext()->GetInfo().Name().c_str(), szCurItemName) == 0)
				pLastItem = o;
			o->Deselect();
		}
	}

	if (pLastItem)
	{
		pLastItem->Select();
		m_selectedItemUuid = pLastItem->UUID();
	}
	
#else
	// Create timeline item
	auto item = std::make_shared<TLItem>();
	item->GetInfo().SetId(m_itemIdCount);
	item->GetInfo().SetName("Timeline" + std::to_string(m_itemIdCount));
	item->GetInfo().SetTotalTime(16730000);
	item->GetInfo().SetCurrentTime(16730000/2);
	this->m_items.insert(std::make_pair(item->GetInfo().UUID(), item));

	// Create timeline item visual
	auto itemVisual = std::make_shared<TLItemVis>();
	itemVisual->Initialize(item);
	itemVisual->SetPosition(0, 0);
	itemVisual->Collapse();
	this->m_itemVisuals.insert(std::make_pair(itemVisual->UUID(), itemVisual));

	// todo: It should be linked by weak_ptr
	// Add a root track for each new item.
	auto rootTrack = item->GetRootTrack();
	this->m_tracks.insert(std::make_pair(rootTrack->UUID(), rootTrack));

	auto trackVisual = std::make_shared<TLTrackVis>();
	trackVisual->Initialize(rootTrack);
	trackVisual->SetPosition(0, 0);
	trackVisual->Collapse();
	this->m_trackVisuals.insert(std::make_pair(trackVisual->UUID(), trackVisual));

	m_itemIdCount++;

#ifdef ENGINNERING_MODE
#if true
	std::string output = boost::str(boost::format("TimelineManager::AddItem::%s m_items.size=%d m_itemVisuals.size=%d m_tracks.size=%d m_trackVisuals.size=%d")
		% itemVisual->UUID()
		% m_items.size()
		% m_itemVisuals.size()
		% m_tracks.size()
		% m_trackVisuals.size()
	);
	DumpToLog(output);
#endif
#endif // ENGINNERING_MODE

	this->StatusChanged("AddItem");
	
#endif
}

void TimelineManager::DeleteItem()
{
//	if (GetPBStatus() == PBC_PLAY || GetPBStatus() == PBC_PAUSE)
	{
		SendMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_RESET_TIME_CODE, 0, 0);
		PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_BEGIN, 0, 0);
		//m_pDisplayManager->ClearAll();
		ClearAll();
		m_pDisplayManager->ResetAllD3DSurfaces();
		DoPlayback_AllOutDevices(PBC_CLOSE);
	}

	auto item = GetItem(m_selectedItemUuid);
	if (!item)
		return;
	m_pTrackContentManager->DeleteItem((char*)item->GetInfo().Name().c_str());

	int result = this->m_itemVisuals.erase(m_selectedItemUuid);
	m_items.erase(m_selectedItemUuid);

	if (m_itemVisuals.size() == 0)
	{
		m_trackVisuals.clear();
		m_itemToTrackMap.clear();
		m_items.clear();
	}

	if (result == 1)
	{
#ifdef ENGINNERING_MODE
#if true
		DumpToLog(boost::str(boost::format("TimelineManager::DeleteItem::%s deleted! ") % m_selectedItemUuid));
#endif
#endif // ENGINNERING_MODE
	}

#ifdef ENGINNERING_MODE
#if true
	std::string output = boost::str(boost::format("TimelineManager::DeleteItem::m_items.size=%d m_itemVisuals.size=%d m_tracks.size=%d m_trackVisuals.size=%d")
		% m_items.size()
		% m_itemVisuals.size()
		% m_tracks.size()
		% m_trackVisuals.size()
	);
	DumpToLog(output);
#endif
#endif // ENGINNERING_MODE

	this->StatusChanged("DeleteItem");
	//PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_PAINT, 0, 0);
	PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_REDRAW, 0, 0);
	//m_pEditViewWnd->Invalidate(false);

	//m_pDisplayManager->ClearAll();
	ClearAll();

	auto visuals = this->GetItemVisuals();
	for (auto o : visuals)
	{
		if (o.get())
		{
			o->Select();
			m_selectedItemUuid = o->UUID();
			break;
		}
	}

	auto item2 = GetItem(m_selectedItemUuid);
	if (item2.get())
	{
		if (m_selectedItemUuid != m_LastItemUuid)
		{
			unsigned long ulOffset = item2->GetInfo().GetDisplayStartTime();
			PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_SET_TIMEBAR_OFFSET, ulOffset, 0);

			long lShift = item2->GetInfo().GetTrackVerticalScrollShift();
			PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_SET_VERTICAL_SCROLL_SHIFT, lShift, 0);

			PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_REDRAW, 0, 0);

			auto cur_time = item2->GetInfo().CurrentTime();

			auto time_code_engine = ServiceProvider::Instance()->GetTimelineService()->GetTimeCodeEngine();
			time_code_engine->SetTimecode(cur_time);
		}
	}
}

bool TimelineManager::ItemClick(const Gdiplus::Point& point)
{
	auto visuals = this->GetItemVisuals();

	bool hasCollision = false;

	int iSelCount = 0;
	m_LastItemUuid = m_selectedItemUuid;
	
//	PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_RESET_TIME_CODE, 0, 0);
//	m_pDisplayManager->ClearAll();
//	m_pDisplayManager->ResetAllD3DSurfaces();
//	DoPlayback_AllOutDevices(PBC_CLOSE);
	
	for (auto o : visuals)
	{
		if (o.get())
		{
			if (o->IsSelect())
				iSelCount = 1;

			o->Deselect();
		}
	}

	int iCount = 0;

	FreezeShape(true);

	for (auto o : visuals)
	{
		if (o.get())
		{
			if (o->HasCollision(point))
			{
				hasCollision = true;
				o->Select();

				m_selectedItemUuid = o->UUID();
				
				if (m_selectedItemUuid != m_LastItemUuid)
				{
					if (iCount == 0)
					{
						SendMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_RESET_TIME_CODE, 0, 0);
						//m_pDisplayManager->ClearAll();
						
						DoPlayback_AllOutDevices(PBC_STOP);

						m_pDisplayManager->ResetAllD3DSurfaces();
						DoPlayback_AllOutDevices(PBC_CLOSE);

						ClearAll();
					}

					o->SetStatus(TLIS_CLOSE);

					iCount++;
				}
				
				auto item = GetItem(m_selectedItemUuid);
				if (m_selectedItemUuid != m_LastItemUuid)
				{
					unsigned long ulOffset = item->GetInfo().GetDisplayStartTime();
					PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_SET_TIMEBAR_OFFSET, ulOffset, 0);

					long lShift = item->GetInfo().GetTrackVerticalScrollShift();
					PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_SET_VERTICAL_SCROLL_SHIFT, lShift, 0);

					PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_REDRAW, 0, 0);

					auto cur_time = item->GetInfo().CurrentTime();

					auto time_code_engine = ServiceProvider::Instance()->GetTimelineService()->GetTimeCodeEngine();
					time_code_engine->SetTimecode(cur_time);
				}

				this->StatusChanged("ItemSelectionChanged");
				ResetEffectStatus();

				o->MouseClick(point);

				break;
			}
		}
	}

	FreezeShape(false);
	
	if (m_selectedItemUuid != m_LastItemUuid)
	{
//		PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_RESET_TIME_CODE, 0, 0);
		//m_pDisplayManager->ClearAll();
//		ClearAll();

//		m_pDisplayManager->ResetAllD3DSurfaces();
//		DoPlayback_AllOutDevices(PBC_CLOSE);
	}
	

	if (!hasCollision && iSelCount > 0)
	{
		m_selectedItemUuid = m_LastItemUuid;

		UpdateAudioStatus();
		
		auto item_vis = GetItemVisual(m_selectedItemUuid);
		if (item_vis.get())
			item_vis->Select();
	}

//	if (m_selectedItemUuid != m_LastItemUuid)
	//	m_pDisplayManager->ClearAll();
	
	return hasCollision;
}

void TimelineManager::LButtonUp(const Gdiplus::Point& point)
{
	auto visuals = this->GetItemVisuals();

	bool hasCollision = false;
	int iSelCount = 0;

	m_LastItemUuid = m_selectedItemUuid;

	for (auto o : visuals)
	{
		if (o.get())
		{
			if (o->IsSelect())
				iSelCount = 1;

			o->Deselect();
		}
	}

	for (auto o : visuals)
	{
		if (o.get())
		{
			if (o->HasCollision(point))
			{
				hasCollision = true;
				o->Select();
				o->LButtonUp(point);
				//			m_selectedItemUuid = o->UUID();
				//			this->StatusChanged("ItemSelectionChanged");
				break;
			}
		}
	}
	
	if (!hasCollision && iSelCount > 0)
	{
		m_selectedItemUuid = m_LastItemUuid;

		auto item_vis = GetItemVisual(m_selectedItemUuid);
		if(item_vis.get())
			item_vis->Select();
	}
	
}

void TimelineManager::OnMouseMove(const Gdiplus::Point& point)
{
	auto visuals = this->GetItemVisuals();
	for (auto o : visuals)
	{
		if (o.get())
		{
			o->MouseMove(point);
		}
	}
}

bool TimelineManager::ItemDbClick(const Gdiplus::Point& point)
{
	auto visuals = this->GetItemVisuals();

	bool hasCollision = false;
	int iSelCount = 0;
	m_LastItemUuid = m_selectedItemUuid;

	for (auto o : visuals)
	{
		if (o.get())
		{
			if (o->IsSelect())
				iSelCount = 1;

			o->Deselect();
		}
	}

	for (auto o : visuals)
	{
		if (o.get())
		{
			if (o->HasCollision(point))
			{
				hasCollision = true;
				o->Select();
				o->MouseDbClick(point);
				m_selectedItemUuid = o->UUID();
				//			this->StatusChanged("ItemSelectionChanged");
				break;
			}
		}
	}
	
	if (!hasCollision && iSelCount > 0)
	{
		m_selectedItemUuid = m_LastItemUuid;

		auto item_vis = GetItemVisual(m_selectedItemUuid);
		if (item_vis.get())
			item_vis->Select();
	}

	return hasCollision;
}

bool TimelineManager::TrackClick(const Gdiplus::Point& point)
{
	auto visuals = this->GetTrackVisualsOfCurrentItem();

	bool hasCollision = false;

	for (auto o : visuals)
	{
		o->Deselect();
	}

	for (auto o : visuals)
	{
#if 0
		if (o->GetContext()->IsVisible() && o->HasCollision(point))
		{
			hasCollision = true;
			o->Select();
			o->MouseLeftButtonDown(point);
			m_selectedTrackUuid = o->UUID();
			this->StatusChanged("TrackSelectionChanged");

			break;
		}
#else
		if (o->GetContext()->IsVisible() && o->HasCollision(point))
		{
			hasCollision = true;
			o->Select();
			o->MouseLeftButtonDown(point);
			m_selectedTrackUuid = o->UUID();
			this->StatusChanged("TrackSelectionChanged");

		//	break;
		}
		else
			o->MouseLeftButtonDown(point);
#endif
	}

	return hasCollision;
}

void TimelineManager::LoadProject(const std::string& filename)
{
#ifdef ENGINNERING_MODE
#if false
	DumpToLog(filename);
#endif
#endif // ENGINNERING_MODE
	ItemVisVector firstSelectItems;

//	if (GetPBStatus() == PBC_PLAY || GetPBStatus() == PBC_PAUSE)
	{
		SendMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_RESET_TIME_CODE, 0, 0);
		PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_BEGIN, 0, 0);
		//m_pDisplayManager->ClearAll();
		ClearAll();
		m_pDisplayManager->ResetAllD3DSurfaces();
		DoPlayback_AllOutDevices(PBC_CLOSE);
	}

	char szFileName[512];
	strcpy(szFileName, filename.c_str());

	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename.c_str());

	m_items.clear();
	m_itemVisuals.clear();
	m_pTrackContentManager->RemoveAll();

	auto project = doc.FirstChildElement("TimelineProject");
	if (!project)
		return;
	auto item = project->FirstChildElement("Item");
	for (auto o = item; o != 0; o = o->NextSiblingElement())
	{
		auto item = std::make_shared<TLItem>();
		item->GetInfo().SetId(o->IntAttribute("Id"));
		item->GetInfo().SetName(std::string(o->Attribute("Name")));
		item->GetInfo().SetTotalTime(TimeCode(o->IntAttribute("TotalTime")));
		item->GetInfo().SetCurrentTime(TimeCode(o->IntAttribute("TotalTime")));
		m_items.insert(std::make_pair(item->GetInfo().UUID(), item));

		std::string szCurItemName = o->Attribute("Name");
		m_pTrackContentManager->AddItem((char*)szCurItemName.c_str());

		auto track = o->FirstChildElement("Track");
		LoadXmlRecursively(track, item, item->GetRootTrack(), m_tracks);
	}

	for (auto pair : m_items)
	{
		auto item = pair.second;
		if (item.get())
		{
			auto itemVisual = std::make_shared<TLItemVis>();
			itemVisual->Initialize(item);
			itemVisual->SetPosition(0, 0);
			itemVisual->Expand();
			this->m_itemVisuals.insert(std::make_pair(itemVisual->UUID(), itemVisual));

			firstSelectItems.push_back(itemVisual);
			CreateTrackVisualsRecursively(item, item->GetRootTrack());
		}
	}

	if (firstSelectItems.size() > 0)
	{
		std::sort(firstSelectItems.begin(), firstSelectItems.end(), [](TLItemVisPtr& a, TLItemVisPtr& b) -> bool { return a->GetContext()->GetInfo().Id() < b->GetContext()->GetInfo().Id(); });
		auto item = firstSelectItems[0];
		item->Select();
		m_selectedItemUuid = item->UUID();
	}

	this->StatusChanged("LoadProject");

	int iLen = strlen(szFileName);
	strcpy(szFileName + iLen - 3, "txt");

	m_pTrackContentManager->Load(szFileName);

	UpdateTotalTime();

	UpdateAudioStatus();
	ResetEffectStatus();

	//PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_PAINT, 0, 0);
	PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_REDRAW, 0, 0);
	//m_pEditViewWnd->Invalidate(false);

#ifdef ENGINNERING_MODE
#if false
	std::string output = boost::str(boost::format("TimelineManager::LoadProject::m_items.size=%d m_itemVisuals.size=%d m_tracks.size=%d m_trackVisuals.size=%d")
		% m_items.size()
		% m_itemVisuals.size()
		% m_tracks.size()
		% m_trackVisuals.size()
	);
	DumpToLog(output);
#endif
#endif // ENGINNERING_MODE
}

void TimelineManager::LoadProject(const std::string& filename, std::string szName,int iID)
{
#ifdef ENGINNERING_MODE
#if false
	DumpToLog(filename);
#endif
#endif // ENGINNERING_MODE
	ItemVisVector firstSelectItems;

	if (GetPBStatus() == PBC_PLAY || GetPBStatus() == PBC_PAUSE)
	{
		SendMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_RESET_TIME_CODE, 0, 0);
		PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_BEGIN, 0, 0);
		//m_pDisplayManager->ClearAll();
		ClearAll();
		m_pDisplayManager->ResetAllD3DSurfaces();
		DoPlayback_AllOutDevices(PBC_CLOSE);
	}

	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename.c_str());

	int iCount = 0;

	auto project = doc.FirstChildElement("TimelineProject");
	auto item = project->FirstChildElement("Item");
	for (auto o = item; o != 0; o = o->NextSiblingElement())
	{
		auto item = std::make_shared<TLItem>();
		
		if (iCount == 0)
		{
			item->GetInfo().SetId(iID);
			item->GetInfo().SetName(szName);
			//item->GetInfo().SetTotalTime(TimeCode(10000 * (iID + 1)));
			item->GetInfo().SetTotalTime(TimeCode(0));
			item->GetInfo().SetCurrentTime(TimeCode(0));

			m_pTrackContentManager->AddItem((char*)szName.c_str());
		}
		else
		{
			item->GetInfo().SetId(o->IntAttribute("Id"));
			item->GetInfo().SetName(std::string(o->Attribute("Name")));
			item->GetInfo().SetTotalTime(TimeCode(o->IntAttribute("TotalTime")));
			item->GetInfo().SetCurrentTime(TimeCode(o->IntAttribute("TotalTime")));

			std::string szCurItemName = o->Attribute("Name");
			m_pTrackContentManager->AddItem((char*)szCurItemName.c_str());
		}
		m_items.insert(std::make_pair(item->GetInfo().UUID(), item));

		auto track = o->FirstChildElement("Track");
		LoadXmlRecursively(track, item, item->GetRootTrack(), m_tracks);

		iCount++;
	}

	for (auto pair : m_items)
	{
		auto item = pair.second;
		if (item.get())
		{
			auto itemVisual = std::make_shared<TLItemVis>();
			itemVisual->Initialize(item);
			itemVisual->SetPosition(0, 0);
			itemVisual->Expand();
			this->m_itemVisuals.insert(std::make_pair(itemVisual->UUID(), itemVisual));

			firstSelectItems.push_back(itemVisual);
			CreateTrackVisualsRecursively(item, item->GetRootTrack());
		}
	}

	if (firstSelectItems.size() > 0)
	{
		std::sort(firstSelectItems.begin(), firstSelectItems.end(), [](TLItemVisPtr& a, TLItemVisPtr& b) -> bool { return a->GetContext()->GetInfo().Id() < b->GetContext()->GetInfo().Id(); });
		auto item = firstSelectItems[0];
		item->Select();
		m_selectedItemUuid = item->UUID();
	}

	this->StatusChanged("LoadProject");

	UpdateAudioStatus();
	ResetEffectStatus();

#ifdef ENGINNERING_MODE
#if false
	std::string output = boost::str(boost::format("TimelineManager::LoadProject::m_items.size=%d m_itemVisuals.size=%d m_tracks.size=%d m_trackVisuals.size=%d")
		% m_items.size()
		% m_itemVisuals.size()
		% m_tracks.size()
		% m_trackVisuals.size()
	);
	DumpToLog(output);
#endif
#endif // ENGINNERING_MODE
}

void TimelineManager::SaveProject(const std::string& filename)
{
	tinyxml2::XMLDocument xmlDoc;

	char szFileName[512];
	strcpy(szFileName, filename.c_str());

	XMLElement* pProject = xmlDoc.NewElement("TimelineProject");
	pProject->SetAttribute("Version", "1.0");
	xmlDoc.InsertFirstChild(pProject);
	
	for (auto it = m_items.begin(); it != m_items.end(); ++it)
	{
		std::string sID = it->first;
		auto item = it->second;
		if (item.get())
		{
			XMLElement* pCurItem = xmlDoc.NewElement("Item");

			pCurItem->SetAttribute("Name", item->GetInfo().Name().c_str());
			pCurItem->SetAttribute("Id", (int)item->GetInfo().Id());
			pCurItem->SetAttribute("TotalTime", item->GetInfo().GetTotalTimeString().c_str());

			std::string curItemUuid = item->GetInfo().UUID();
			TrackVector tracks = GetTracks(curItemUuid);

			for (auto o : tracks)
			{
				if (o->Level() == 2)
				{
					XMLElement * pCurTrack = xmlDoc.NewElement("Track");

					std::string track_name;
					if (o->NickName() != "")
						track_name = o->NickName();
					else
						track_name = o->Name();
					TLTrackType type = o->TrackType();

					pCurTrack->SetAttribute("Name", track_name.c_str());
					pCurTrack->SetAttribute("Type", (int)type);
					pCurTrack->SetAttribute("Id", (int)o->Id());
					pCurTrack->SetAttribute("Level", (int)o->Level());

					if (o->HasChild())
						CreateSubNode(&xmlDoc, pCurTrack, o, o->ChildTracks());

					pCurItem->InsertEndChild(pCurTrack);
				}
			}

			pProject->InsertEndChild(pCurItem);
		}
	}

	XMLError eResult = xmlDoc.SaveFile(filename.c_str());

	int iLen = strlen(szFileName);
	strcpy(szFileName + iLen - 3,"txt");

	m_pTrackContentManager->Save(szFileName);
}

void TimelineManager::CreateSubNode(tinyxml2::XMLDocument* pDoc, XMLElement* pParent, std::shared_ptr<Timeline::TLTrack> pTrack, TrackVector& child_tracks)
{
	for (auto o : child_tracks)
	{
		XMLElement * pCurTrack = pDoc->NewElement("Track");

		std::string track_name = o->Name();
		TLTrackType type = o->TrackType();
		
		pCurTrack->SetAttribute("Name", track_name.c_str());
		pCurTrack->SetAttribute("Type", (int)type);
		pCurTrack->SetAttribute("Id", (int)o->Id());
		pCurTrack->SetAttribute("Level", (int)o->Level());

		if (o->HasChild())
			CreateSubNode(pDoc, pCurTrack, o, o->ChildTracks());

		pParent->InsertEndChild(pCurTrack);
	}
}

TLItemPtr TimelineManager::GetItem(const std::string& uuid)
{
	return m_items[uuid];
}

ItemVector TimelineManager::GetItems()
{
	ItemVector output;
#if 0
	std::transform(m_items.begin(), m_items.end(), std::back_inserter(output), [](const ItemMap::value_type& val) {return val.second; });
	std::sort(output.begin(), output.end(), [](TLItemPtr& a, TLItemPtr& b) -> bool { return a->GetInfo().Id() < b->GetInfo().Id(); });
#else
	ItemVector temp;
	std::transform(m_items.begin(), m_items.end(), std::back_inserter(temp), [](const ItemMap::value_type& val) {return val.second; });
	for (auto o : temp)
	{
		if (o.get())
			output.push_back(o);
	}
	std::sort(output.begin(), output.end(), [](TLItemPtr& a, TLItemPtr& b) -> bool { return a->GetInfo().Id() < b->GetInfo().Id(); });
#endif
	return output;
}

TLItemVisPtr TimelineManager::GetItemVisual(const std::string& uuid)
{
	return m_itemVisuals[uuid];
}

ItemVisVector TimelineManager::GetItemVisuals()
{
	ItemVisVector output;
#if 0
	std::transform(m_itemVisuals.begin(), m_itemVisuals.end(), std::back_inserter(output), [](const ItemVisMap::value_type& val) {return val.second; });
	std::sort(output.begin(), output.end(), [](TLItemVisPtr& a, TLItemVisPtr& b) -> bool { return a->GetContext()->GetInfo().Id() < b->GetContext()->GetInfo().Id(); });
#else
	ItemVisVector temp;
	std::transform(m_itemVisuals.begin(), m_itemVisuals.end(), std::back_inserter(temp), [](const ItemVisMap::value_type& val) {return val.second; });

	for (auto o : temp)
	{
		if (o.get())
			output.push_back(o);
	}
	std::sort(output.begin(), output.end(), [](TLItemVisPtr& a, TLItemVisPtr& b) -> bool { return a->GetContext()->GetInfo().Id() < b->GetContext()->GetInfo().Id(); });
#endif
	return output;
}

TLTrackPtr TimelineManager::GetTrack(const std::string& uuid)
{
	return m_tracks[uuid];
}

TrackVector TimelineManager::GetTracks()
{
	TrackVector output;
	std::transform(m_tracks.begin(), m_tracks.end(), std::back_inserter(output), [](const TrackMap::value_type& val) {return val.second; });
	return output;
}

TrackVector TimelineManager::GetTracks(const std::string& uuid)
{
	TrackVector output;
	TrackVector temp;

	//auto visuals = GetTrackVisualsWithoutRootAndCommand();
	auto match_track_uuids = m_itemToTrackMap.equal_range(uuid);
	
	while (match_track_uuids.first != match_track_uuids.second)
	{
		auto uuid = match_track_uuids.first->second;
		temp.push_back(m_tracks[uuid]);
		++(match_track_uuids.first);
	}
	
	
	for (auto o : temp)
	{
		//auto name = o->GetContext()->Name();
		//if (name != "Command" && name != "ItemRoot")
		{
			output.push_back(o);
		}
	}
	
	//std::sort(output.begin(), output.end(), [](TLTrackPtr& a, TLTrackPtr& b) -> bool { return a->GetContext()->Id() < b->GetContext()->Id(); });

	return output;
}

TLTrackVisPtr TimelineManager::GetTrackVisual(const std::string& uuid)
{
	return m_trackVisuals[uuid];
}

TrackVisVector TimelineManager::GetTrackVisuals()
{
	TrackVisVector output;
#if 0
	std::transform(m_trackVisuals.begin(), m_trackVisuals.end(), std::back_inserter(output), [](const TrackVisMap::value_type& val) {return val.second; });
	std::sort(output.begin(), output.end(), [](TLTrackVisPtr& a, TLTrackVisPtr& b) -> bool { return a->GetContext()->Id() < b->GetContext()->Id(); });
#else
	TrackVisVector temp;
	std::transform(m_trackVisuals.begin(), m_trackVisuals.end(), std::back_inserter(temp), [](const TrackVisMap::value_type& val) {return val.second; });

	for (auto o : temp)
	{
		if (o.get())
			output.push_back(o);
	}

	std::sort(output.begin(), output.end(), [](TLTrackVisPtr& a, TLTrackVisPtr& b) -> bool { return a->GetContext()->Id() < b->GetContext()->Id(); });
#endif
	return output;
}

TLTrackVisPtr TimelineManager::GetCommandTrackVisual()
{
	TLTrackVisPtr result = nullptr;

	TrackVisVector output;
	std::transform(m_trackVisuals.begin(), m_trackVisuals.end(), std::back_inserter(output), [](const TrackVisMap::value_type& val) {return val.second; });

	for (auto o : output)
	{
		if (o.get())
		{
			if (o->GetContext()->Name() == "Command")
			{
				result = o;
			}
		}
	}

	return result;
}

TrackVisVector TimelineManager::GetTrackVisualsWithoutRootAndCommand()
{
	TrackVisVector output;
	TrackVisVector temp;
	std::transform(m_trackVisuals.begin(), m_trackVisuals.end(), std::back_inserter(temp), [](const TrackVisMap::value_type& val) {return val.second; });

	for (auto o : temp)
	{
		if (o.get())
		{
			auto name = o->GetContext()->Name();
			if (name != "Command" && name != "ItemRoot")
			{
				output.push_back(o);
			}
		}
	}

	std::sort(output.begin(), output.end(), [](TLTrackVisPtr& a, TLTrackVisPtr& b) -> bool { return a->GetContext()->Id() < b->GetContext()->Id(); });

	return output;
}

TrackVisVector TimelineManager::GetTrackVisualsOfCurrentItem()
{
	TrackVisVector output;
	TrackVisVector temp;

	auto visuals = GetTrackVisualsWithoutRootAndCommand();
	auto match_track_uuids = m_itemToTrackMap.equal_range(m_selectedItemUuid);

	while (match_track_uuids.first != match_track_uuids.second)
	{
		auto uuid = match_track_uuids.first->second;
		temp.push_back(m_trackVisuals[uuid]);
		++(match_track_uuids.first);
	}

	for (auto o : temp)
	{
		if (o.get())
		{
			auto name = o->GetContext()->Name();
			if (name != "Command" && name != "ItemRoot")
			{
				output.push_back(o);
			}
		}
	}

	std::sort(output.begin(), output.end(), [](TLTrackVisPtr& a, TLTrackVisPtr& b) -> bool { return a->GetContext()->Id() < b->GetContext()->Id(); });

	return output;
}

TLTrackVisSearchResult TimelineManager::GetHitTrack(const Gdiplus::Point& point)
{
	TLTrackVisSearchResult result;

	result.HasResult = false;

	auto cmd = GetCommandTrackVisual();
	if (cmd.get() && cmd->HasCollision(point))
	{
		result.Context = cmd;
		result.HasResult = true;
	}

	if (result.HasResult != true)
	{
		auto visuals = this->GetTrackVisualsOfCurrentItem();

		for (auto o : visuals)
		{
			if (o->GetContext()->IsVisible() && o->HasCollision(point))
			{
				result.Context = o;
				result.HasResult = true;
				break;
			}
		}
	}

	return result;
}

std::string& TimelineManager::GetSelectedItem(const std::string& uuid)
{
	return m_selectedItemUuid;
}

void TimelineManager::LoadXmlRecursively(tinyxml2::XMLElement* first, TLItemPtr& item, TLTrackPtr& parent, TrackMap& map)
{
	std::vector<TLTrackType> types = {
		TLTrackType::Undefine,
		TLTrackType::Float,
		TLTrackType::Bool,
		TLTrackType::Int,
		TLTrackType::String,
		TLTrackType::Media,
		TLTrackType::Command,
		TLTrackType::Effect_Index,
		TLTrackType::Effect,
		TLTrackType::AudioConfig
	};

	for (auto element = first; element != 0; element = element->NextSiblingElement())
	{
		auto track = std::make_shared<TLTrack>();
		track->SetName(element->Attribute("Name"));
		track->SetTrackType(types[element->IntAttribute("Type")]);
		track->SetId(element->IntAttribute("Id"));

		parent->AddChildTrack(track);

		if (strcmp(track->Parent()->Name().c_str(), "VideoEffect") == 0)
			track->SetNickName(element->Attribute("Name"));
		else
			track->SetNickName("");

		map.insert(std::make_pair(track->UUID(), track));
		m_itemToTrackMap.insert(std::make_pair(item->GetInfo().UUID(), track->UUID()));

		auto firstChild = element->FirstChildElement();
		if (firstChild != 0)
		{
			LoadXmlRecursively(firstChild, item, track, map);
		}
	}
}

void TimelineManager::CreateTrackVisualsRecursively(TLItemPtr& item, TLTrackPtr& track)
{
	auto trackVisual = std::make_shared<TLTrackVis>();
	trackVisual->SetPosition(0, 0);
	trackVisual->Expand();
	trackVisual->Initialize(track);

	m_trackVisuals.insert(std::make_pair(trackVisual->UUID(), trackVisual));

#if _TRAIL_VERSION
	int iSourceCount = 0;

	if (track->HasChild())
	{
		for (auto child : track->ChildTracks())
		{
			auto item_name = child->Name();
			char* pch = strstr((char*)item_name.c_str(), "Source");
			if (pch)
			{
				iSourceCount++;

				if (iSourceCount > 1)
					continue;
			}

			char* pch1 = strstr((char*)item_name.c_str(), "Effect");
			if (pch1)
			{
				continue;
			}

			CreateTrackVisualsRecursively(item, child);
		}
	}
#else
	
	if (track->HasChild())
	{
		for (auto child : track->ChildTracks())
		{
			CreateTrackVisualsRecursively(item, child);
		}
	}
#endif
}

void TimelineManager::SetBaseTimeCode(TimeCode& timecode)
{
	m_baseTimeCode = timecode;
	this->BaseTimeCodeChanged(m_baseTimeCode);
}

TimeCode& TimelineManager::GetBaseTimeCode()
{
	return m_baseTimeCode;
}

TimeCode TimelineManager::GetTotalTimeOfCurrentItem()
{
	if (strcmp(m_selectedItemUuid.c_str(), "") == 0)
		return 0;

	TLItemPtr pCurItem = GetItem(m_selectedItemUuid);
	if(pCurItem)
		return pCurItem->GetInfo().TotalTime();
	return 0;
}

void TimelineManager::SetTotalTimeOfCurrentItem(TimeCode timecode)
{
	if (strcmp(m_selectedItemUuid.c_str(), "") == 0)
		return ;

	TLItemPtr pCurItem = GetItem(m_selectedItemUuid);
	if (pCurItem)
		pCurItem->GetInfo().SetTotalTime(timecode);
	this->StatusChanged("SetTotalTimeOfCurrentItem");
}

TimeCode TimelineManager::XToTimeCode(int x)
{
	TimeCode result;
	TimeCode baseTimeCode = this->GetBaseTimeCode();

	int shift = x - TLTrackVis::VisualConstant::ExpanderWidth;
	double timecodeDelta = (double)shift * (double)TimelineManager::PixelToMsFactor;
	result = baseTimeCode + TimeCode((long)timecodeDelta);

	return result;
}

int TimelineManager::TimeCodeToX(TimeCode timecode)
{
	int result = 0;

	TimeCode baseTimeCode = this->GetBaseTimeCode();

	TimeCode shift = timecode - baseTimeCode;
	int xDelta = (double)shift.TotalMilliseconds() / (double)TimelineManager::PixelToMsFactor;
	result = TLTrackVis::VisualConstant::ExpanderWidth + xDelta;

	return result;
}

void TimelineManager::SetPointerMode(PointerMode mode)
{
	this->m_pointerMode = mode;
#ifdef ENGINNERING_MODE
#if true
	DumpToLog(boost::str(boost::format("TimelineManager::SetPointerMode::%d") % (int)mode));
#endif
#endif // ENGINNERING_MODE

	if (mode == PointerMode::Edit)
	{
		auto visuals = this->GetTrackVisualsOfCurrentItem();

		for (auto o : visuals)
		{
			o->DisableDrag();
		}
	}
}

PointerMode TimelineManager::GetPointerMode()
{
	return this->m_pointerMode;
}

void TimelineManager::SetLineMode(LineMode mode)
{
	m_LineMode = mode;
}

LineMode TimelineManager::GetLineMode()
{
	return m_LineMode;
}

void TimelineManager::SetUnitMode(UnitMode mode)
{
	m_UnitMode = mode;
}

UnitMode TimelineManager::GetUnitMode()
{
	return m_UnitMode;
}

void TimelineManager::UpdateTimeCode(TimeCode timecode)
{

	if (strcmp(m_selectedItemUuid.c_str(), "") == 0)
		return ;

	TLItemPtr pCurItem = GetItem(m_selectedItemUuid);
	if (pCurItem)
	{
		int iTmp = ((timecode.TotalMilliseconds() / 1000) / 10) * 10;
		int iStartTime = iTmp * -100;
		pCurItem->GetInfo().SetDisplayStartTime(iStartTime);
		pCurItem->GetInfo().SetCurrentTime(timecode);
		if(m_pItemViewWnd)
			m_pItemViewWnd->Invalidate(false);
	}
}

void TimelineManager::SetMediaServerDlgWnd(CWnd* pWnd)
{
	m_pMediaServerDlgWnd = pWnd;
}

CWnd* TimelineManager::GetMediaServerDlgWnd()
{
	return m_pMediaServerDlgWnd;
}

void TimelineManager::ChangeTo(char* szName)
{
	auto item_list = GetItems();

	FreezeShape(true);

	/*
	TLItemPtr pCurItem = GetItem(m_selectedItemUuid);
	if (pCurItem)
	{
		pCurItem->GetInfo().SetCurrentTime(0);
	}
	*/

	for (auto item : item_list)
	{
		if (item.get())
		{
			if (strcmp(item->GetInfo().Name().c_str(), szName) == 0)
			{
				auto visuals = this->GetItemVisuals();

				for (auto o : visuals)
					o->Deselect();

				for (auto o : visuals)
				{
					if (strcmp(o->UUID().c_str(), item->GetInfo().UUID().c_str()) == 0)
					{
						o->Select();

						SendMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_RESET_TIME_CODE, 0, 0);
//						ClearAll();
						DoPlayback_AllOutDevices(PBC_CLOSE);

						m_selectedItemUuid = o->UUID();
						this->StatusChanged("ItemSelectionChanged");
						break;
					}
				}
				break;
			}
		}
	}

	TLItemPtr pCurItem = GetItem(m_selectedItemUuid);
	if (pCurItem)
	{
		UpdateAudioStatus();
		ResetEffectStatus();
		pCurItem->GetInfo().SetCurrentTime(0);
		PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_PLAY, 0, 0);
	}

	FreezeShape(false);
}

void TimelineManager::Back()
{
	auto item_list = GetItems();
	if (item_list.size() > 1)
	{
		m_LastItemUuid = m_selectedItemUuid;

		int iIndex = -1;
		int iTotal = 0;
		int iCount = 0;
		for (auto item : item_list)
		{
			if (item.get())
			{
				if (m_selectedItemUuid == item->GetInfo().UUID())
				{
					iIndex = iCount;
				}
				iCount++;
			}
		}

		iTotal = iCount;

		int iNewIndex = iIndex - 1;
		if (iNewIndex < 0)
			iNewIndex = iTotal - 1;
		
		auto visuals = this->GetItemVisuals();
		for (auto o : visuals)
			o->Deselect();

		iCount = 0;
		for (auto item : item_list)
		{
			if (item.get())
			{
				if (iCount == iNewIndex)
				{
					for (auto o : visuals)
					{
						if (strcmp(o->UUID().c_str(), item->GetInfo().UUID().c_str()) == 0)
						{
							o->Select();
							m_selectedItemUuid = o->UUID();
							this->StatusChanged("ItemSelectionChanged");
							break;
						}
					}
					break;
				}
				iCount++;
			}
		}

		if (m_selectedItemUuid != m_LastItemUuid)
		{
			//PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_RESET_TIME_CODE, 0, 0);
			//m_pDisplayManager->ClearAll();
			m_pDisplayManager->ResetAllD3DSurfaces();
			DoPlayback_AllOutDevices(PBC_CLOSE);

			ClearAll();
		}
	}
}

void TimelineManager::Next()
{
	auto item_list = GetItems();
	if (item_list.size() > 1)
	{
		m_LastItemUuid = m_selectedItemUuid;

		int iIndex = -1;
		int iTotal = 0;
		int iCount = 0;
		for (auto item : item_list)
		{
			if (item.get())
			{
				if (m_selectedItemUuid == item->GetInfo().UUID())
				{
					iIndex = iCount;
				}
				iCount++;
			}
		}

		iTotal = iCount;

		int iNewIndex = iIndex + 1;
		if (iNewIndex >= iTotal)
			iNewIndex = 0;

		auto visuals = this->GetItemVisuals();

		for (auto o : visuals)
			o->Deselect();

		iCount = 0;
		for (auto item : item_list)
		{
			if (item.get())
			{
				if (iCount == iNewIndex)
				{
					for (auto o : visuals)
					{
						if (strcmp(o->UUID().c_str(), item->GetInfo().UUID().c_str()) == 0)
						{
							o->Select();
							m_selectedItemUuid = o->UUID();
							this->StatusChanged("ItemSelectionChanged");
							break;
						}
					}

					break;
				}
				iCount++;
			}
		}

		if (m_selectedItemUuid != m_LastItemUuid)
		{
			//PostMessage(m_pEditViewWnd->GetSafeHwnd(), WM_TM_RESET_TIME_CODE, 0, 0);
			//m_pDisplayManager->ClearAll();
			m_pDisplayManager->ResetAllD3DSurfaces();
			DoPlayback_AllOutDevices(PBC_CLOSE);

			ClearAll();
		}
	}
}

void TimelineManager::UpdateTotalTime()
{
	auto item_list = GetItems();

	for (int i = 0; i < m_pTrackContentManager->GetTotalOfItems(); i++)
	{
		char* szCurItemName = m_pTrackContentManager->GetItemName(i);
		float fTime = m_pTrackContentManager->GetMaximumTime(i);

		for (auto item : item_list)
		{
			if (item.get())
			{
				if (strcmp(item->GetInfo().Name().c_str(), szCurItemName) == 0)
				{
					item->GetInfo().SetTotalTime(fTime * 1000.0);
					break;
				}
			}
		}
	}
}

void TimelineManager::SetVerticalScrollShift(int iValue)
{
	TLItemPtr pCurItem = GetItem(m_selectedItemUuid);
	if (pCurItem)
	{
		pCurItem->GetInfo().SetTrackVerticalScrollShift(iValue);
	}
}

void TimelineManager::DoPlayback_AllOutDevices(int iCmd)
{
	char szMsg[512];

	if (PBC_CLOSE == iCmd)
	{
		m_pDisplayManager->WaitForHDEffectFinished();
	}

	for (int i = 0; i < _TL_MAX_LAYER; i++)
	{
		OutDevice* pCurDevice = m_pTL_Out_Device->GetOutDevice(ST_LAYER1 + i);
		if (pCurDevice)
		{
			switch (iCmd)
			{
				case PBC_PAUSE:
					{
						pCurDevice->Pause();

//						sprintf(szMsg, "TimelineManager::DoPlayback_AllOutDevices - PBC_PAUSE \n");
//						OutputDebugString(szMsg);
					}
					break;
				case PBC_PLAY:
					{
//						sprintf(szMsg, "TimelineManager::DoPlayback_AllOutDevices - PBC_PLAY 0\n");
//						OutputDebugString(szMsg);

						if (m_pTL_Out_Device->GetLayerPBStatus(ST_LAYER1 + i) == PBC_PAUSE || m_pTL_Out_Device->GetLayerPBStatus(ST_LAYER1 + i) == PBC_STOP)
						{
							pCurDevice->Play();

//							sprintf(szMsg, "TimelineManager::DoPlayback_AllOutDevices - PBC_PLAY 1\n");
//							OutputDebugString(szMsg);
						}
					}
					break;
				case PBC_STOP:
					{
					/*
						if (GetTCAdapter()->GetMode() == TCM_CLIENT)
							pCurDevice->Stop2();
						else
						*/
							pCurDevice->Stop();

						//pCurDevice->Close();

//						sprintf(szMsg, "TimelineManager::DoPlayback_AllOutDevices - PBC_STOP \n");
//						OutputDebugString(szMsg);
					}
					break;
				case PBC_CLOSE:
					pCurDevice->Close();
					break;
			}
			m_pTL_Out_Device->SetLayerPBStatus(ST_LAYER1 + i, iCmd);
		}
	}

	OutDevice* pMainDevice = m_pTL_Out_Device->GetOutDevice(ST_MAIN);
	if (pMainDevice)
	{
		switch (iCmd)
		{
		case PBC_PAUSE:
			pMainDevice->Pause();
			break;
		case PBC_PLAY:
			{
				if (m_pTL_Out_Device->GetMainStreamPBStatus() == PBC_PAUSE || m_pTL_Out_Device->GetMainStreamPBStatus() == PBC_STOP)
					pMainDevice->Play();
			}
			break;
		case PBC_STOP:
			pMainDevice->Stop();
			break;
		}
		m_pTL_Out_Device->SetMainStreamPBStatus(iCmd);
	}
}

void TimelineManager::UpdateVideoEffectParamItem(int iIndex)
{
	auto visuals = this->GetTrackVisualsOfCurrentItem();

	for (auto o : visuals)
	{
		if (o->GetContext()->Parent()->Name() == "VideoEffect")
		{
			if(o->GetContext()->NickName() == "Param1")
			{
				o->GetContext()->SetName(g_effect_param_items[iIndex].szParam[1]);
			}
			else if (o->GetContext()->NickName() == "Param2")
			{
				o->GetContext()->SetName(g_effect_param_items[iIndex].szParam[2]);
			}
			else if (o->GetContext()->NickName() == "Param3")
			{
				o->GetContext()->SetName(g_effect_param_items[iIndex].szParam[3]);
			}
			else if (o->GetContext()->NickName() == "Param4")
			{
				o->GetContext()->SetName(g_effect_param_items[iIndex].szParam[4]);
			}
			else if (o->GetContext()->NickName() == "Param5")
			{
				o->GetContext()->SetName(g_effect_param_items[iIndex].szParam[5]);
			}
		}
	}

	if(m_pEditViewWnd)
		m_pEditViewWnd->Invalidate(false);	
}

bool TimelineManager::CheckItemViewStatus()
{
	auto visuals = this->GetItemVisuals();
	for (auto o : visuals)
	{
		if (o.get())
		{
			if (o->CheckButtonStatus())
				return true;
		}
	}
	return false;
}

DisplayManager* TimelineManager::GetDisplayManager()
{
	return m_pDisplayManager;
}

TCAdapter* TimelineManager::GetTCAdapter()
{
	return m_pTCAdapter;
}

int TimelineManager::GetPBStatus()
{
	int iPlayingCount = 0;
	int iPauseCount = 0;

	for (int i = 0; i < _TL_MAX_LAYER; i++)
	{
		int iPBStatus = m_pTL_Out_Device->GetLayerPBStatus(ST_LAYER1 + i);
		if (iPBStatus == PBC_PLAY)
			iPlayingCount++;
		else if (iPBStatus == PBC_PAUSE)
			iPauseCount++;
	}

	if (iPlayingCount > 0)
		return PBC_PLAY;
	else if (iPauseCount > 0)
		return PBC_PAUSE;
	return m_pTL_Out_Device->GetLayerPBStatus(ST_STREAM0 + 0);
}

void TimelineManager::ClearAll()
{
	m_pDisplayManager->ClearAll();

	for (int x = 0; x < g_MediaStreamAry.size(); x++)
	{
		if(g_MediaStreamAry.at(x))
			g_MediaStreamAry.at(x)->Clear();
	}
}

bool TimelineManager::CheckItemName(char* szName)
{
	TLItemPtr pCurItem = GetItem(m_selectedItemUuid);
	if (pCurItem)
	{
		if (strcmp(pCurItem->GetInfo().Name().c_str(), szName) == 0)
			return true;
	}
	return false;
}

void TimelineManager::SetLogFileDll(LogFileDll* pObj)
{
	m_pLogFileDll = pObj;

	m_pTCAdapter->SetLogFileDll(pObj);
	m_pTCAdapter->CheckLogStatus();
}

void TimelineManager::UpdateAudioStatus()
{
	TLItemPtr pCurItem = GetItem(m_selectedItemUuid);
	if (pCurItem)
	{
		int iItemIndex = m_pTrackContentManager->FindItem((char*)pCurItem->GetInfo().Name().c_str());
		if (iItemIndex != -1)
		{
			for (int i = 0; i < 10; i++)
			{
				bool bEnable = m_pTrackContentManager->AudioIsEnabled(iItemIndex, i);
				int iVolume = m_pTrackContentManager->GetVolume(iItemIndex, i);

				OutDevice* pOutDevice = GetOutDevice(ST_STREAM0 + i);
				if (pOutDevice)
				{
					pOutDevice->EnableAudio(bEnable);
					pOutDevice->SetVolume(iVolume);
				}
			}
		}
	}
}

int TimelineManager::GetTotalOfItems()
{
	auto item_list = GetItems();
	return item_list.size();
}

void TimelineManager::ResetItemStatus()
{
	TLItemPtr pCurItem = GetItem(m_selectedItemUuid);
	if (pCurItem)
	{
		ClearAll();

		m_pDisplayManager->ResetAllD3DSurfaces();
		DoPlayback_AllOutDevices(PBC_CLOSE);
	}
}

void TimelineManager::ResetEffectStatus()
{
	for (int i = 0; i < _TL_MAX_LAYER; i++)
	{
		OutDevice* pCurDevice = m_pTL_Out_Device->GetOutDevice(ST_LAYER1 + i);
		if (pCurDevice)
		{
			pCurDevice->SetEffectIndex(0);
			pCurDevice->SetEffectLevel(0);
		}
	}
}

void TimelineManager::FreezeShape(bool bFreeze)
{
	for (int x = 1; x < g_PannelAry.GetCount(); x++)
		g_PannelAry[x]->FreezeDisplay(bFreeze);
}