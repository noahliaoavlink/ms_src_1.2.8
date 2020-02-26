// ShapeUIDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MediaServer.h"
#include "ShapeUIDlg.h"
#include "afxdialogex.h"
#include "LoadObjVdo.h"
#include "tinyxml2.h"
#include "EdgeBlendDlg.h"
#include "../../../Include/StrConv.h"
#include "SFile.h"


//#define _DEBUG_EDGE_BLENDING 1

#ifdef _LOAD_OBJ_THREAD_MODE
unsigned int __stdcall _LoadObj_ThreadProc(void* lpvThreadParm);
unsigned int __stdcall _LoadProject_ThreadProc(void* lpvThreadParm);
#endif

// CShapeUIDlg 對話方塊

IMPLEMENT_DYNAMIC(CShapeUIDlg, CDialog)

CShapeUIDlg::CShapeUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TEMP_DLG, pParent)
{
	m_FramePic = NULL;
	m_iMaxView = 0;
	m_iObjNum = 0;
	m_iSelObjIdx = -1;
	m_iCurTempObjIdx = 0;
	m_bHadInit2DMap = FALSE;

	m_bPositionMode = false;

	m_bShow = false;
	m_pUTipDll = 0;

#ifdef _ENABLE_SPF
	m_pZLibDllObj = new ZLibDllObj;
	m_pZLibDllObj->LoadLib();
#endif

}

CShapeUIDlg::~CShapeUIDlg()
{
	KillTimer(0);

	if (m_FramePic)
		delete m_FramePic;

#ifdef _ENABLE_SPF
	if (m_pZLibDllObj)
		delete m_pZLibDllObj;
#endif
}

void CShapeUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CShapeUIDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_SHAP_SET_MODAL_NUM, OnSetModalNum)
END_MESSAGE_MAP()


// CShapeUIDlg 訊息處理常式


BOOL CShapeUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化

	m_pUTipDll = new UTipDll;
	m_pUTipDll->LoadLib();
	m_pUTipDll->_UT_Init(m_pUTipDll->GetUTObj());

#ifdef _ENABLE_SPF
	if(m_pZLibDllObj)
		m_pZLibDllObj->LoadLib();
#endif

	InitializeCriticalSection(&m_csFunBsy);
	SetWindowLong(this->m_hWnd, GWL_EXSTYLE, GetWindowLong(this->m_hWnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
	SetLayeredWindowAttributes(0, 160, LWA_ALPHA);

	CRgn NullRgn;
	NullRgn.CreateRectRgn(0, 0, 0, 0);
	m_FramePic = new CMenu_Class(this, m_hWnd, 1, theApp.m_strCurPath + "UI Folder\\Shape\\sample2.bmp");

	CItem_Class* pItem;
	pItem = new CItem_Class(this, m_hWnd, ITEM_MOVE, theApp.m_strCurPath + "UI Folder\\Shape\\LB-01.bmp", true, true, true, 1);
	pItem->OffsetObject(CPoint(4, 160));
	pItem->SetUTipDll(m_pUTipDll,this);
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_ROTATE, theApp.m_strCurPath + "UI Folder\\Shape\\LB-02.bmp", true, true, true, 1);
	pItem->OffsetObject(CPoint(4, 237));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_ZOOM, theApp.m_strCurPath + "UI Folder\\Shape\\LB-03.bmp", true, true, true, 1);
	pItem->OffsetObject(CPoint(4, 314));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);

#if 0
	pItem = new CItem_Class(this, m_hWnd, ITEM_MAP, theApp.m_strCurPath + "UI Folder\\Shape\\LB-05.bmp", true, true, true, 1);
	pItem->OffsetObject(CPoint(4, 476));
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_VIDEO, theApp.m_strCurPath + "UI Folder\\Shape\\LB-07.bmp", true, true, true, 2);
	pItem->OffsetObject(CPoint(4, 554));
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_LINE, theApp.m_strCurPath + "UI Folder\\Shape\\LB-06.bmp", true, true, true, 2);
	pItem->OffsetObject(CPoint(4, 632));
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_PTMOVE, theApp.m_strCurPath + "UI Folder\\Shape\\LB-04.bmp", true, true, true, 1);
	pItem->OffsetObject(CPoint(4, 398));
	m_FramePic->AddItem(pItem);
	
	if (!theApp.m_bSampleVersion && theApp.m_bEnableEdgeBlending)
	{
		pItem = new CItem_Class(this, m_hWnd, ITEM_2DMAP, theApp.m_strCurPath + "UI Folder\\Shape\\TB-03.bmp", true, true, true, 1);
		pItem->OffsetObject(CPoint(4, 710));
		m_FramePic->AddItem(pItem);
	}

	pItem = new CItem_Class(this, m_hWnd, ITEM_UNDO, theApp.m_strCurPath + "UI Folder\\Shape\\LB-08.bmp", true, true, true);
	pItem->OffsetObject(CPoint(4, 788));
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_REDO, theApp.m_strCurPath + "UI Folder\\Shape\\LB-09.bmp", true, true, true);
	pItem->OffsetObject(CPoint(4, 866));
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_MODEL_DEL, theApp.m_strCurPath + "UI Folder\\Shape\\LB-10.bmp", true, true, true);
	pItem->OffsetObject(CPoint(4, 944));
	m_FramePic->AddItem(pItem);
#else
	pItem = new CItem_Class(this, m_hWnd, ITEM_PTMOVE, theApp.m_strCurPath + "UI Folder\\Shape\\LB-04.bmp", true, true, true, 1);
	pItem->OffsetObject(CPoint(4, 398));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, ITEM_VIDEO, theApp.m_strCurPath + "UI Folder\\Shape\\LB-07.bmp", true, true, true, 2);
	pItem->OffsetObject(CPoint(4, 476));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_LINE, theApp.m_strCurPath + "UI Folder\\Shape\\LB-06.bmp", true, true, true, 2);
	pItem->OffsetObject(CPoint(4, 554));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);

	if (!theApp.m_bSampleVersion && theApp.m_bEnableEdgeBlending)
	{
		pItem = new CItem_Class(this, m_hWnd, ITEM_2DMAP, theApp.m_strCurPath + "UI Folder\\Shape\\TB-03.bmp", true, true, true, 1);
		pItem->OffsetObject(CPoint(4, 866));
		pItem->SetUTipDll(m_pUTipDll, this);
		m_FramePic->AddItem(pItem);
	}

	pItem = new CItem_Class(this, m_hWnd, ITEM_UNDO, theApp.m_strCurPath + "UI Folder\\Shape\\LB-08.bmp", true, true, true);
	pItem->OffsetObject(CPoint(4, 632));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_REDO, theApp.m_strCurPath + "UI Folder\\Shape\\LB-09.bmp", true, true, true);
	pItem->OffsetObject(CPoint(4, 710));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_MODEL_DEL, theApp.m_strCurPath + "UI Folder\\Shape\\LB-10.bmp", true, true, true);
	pItem->OffsetObject(CPoint(4, 788));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);
#endif

	//CPoint ptOffset = CPoint(3, 3);
	pItem = new CItem_Class(this, m_hWnd, ITEM_NEW, theApp.m_strCurPath + "UI Folder\\Shape\\TB-03.bmp", true, true, true);
	pItem->OffsetObject(CPoint(205, 1));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_OPEN, theApp.m_strCurPath + "UI Folder\\Shape\\TB-04.bmp", true, true, true);
	pItem->OffsetObject(CPoint(282, 1));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_SAVE, theApp.m_strCurPath + "UI Folder\\Shape\\TB-05.bmp", true, true, true);
	pItem->OffsetObject(CPoint(359, 1));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_LOADOBJ, theApp.m_strCurPath + "UI Folder\\Shape\\TB-02.bmp", true, true, true);
	pItem->OffsetObject(CPoint(474, 1));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_CUBE, theApp.m_strCurPath + "UI Folder\\Shape\\TB-07.bmp", true, true, true);
	pItem->OffsetObject(CPoint(549, 1));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_BALL, theApp.m_strCurPath + "UI Folder\\Shape\\TB-08.bmp", true, true, true);
	pItem->OffsetObject(CPoint(624, 1));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_4PYRAMID, theApp.m_strCurPath + "UI Folder\\Shape\\TB-09.bmp", true, true, true);
	pItem->OffsetObject(CPoint(699, 1));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_FIG, theApp.m_strCurPath + "UI Folder\\Shape\\TB-10.bmp", true, true, true);
	pItem->OffsetObject(CPoint(774, 1));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_FLAT, theApp.m_strCurPath + "UI Folder\\Shape\\TB-11.bmp", true, true, true);
	pItem->OffsetObject(CPoint(849, 1));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, ITEM_DISC, theApp.m_strCurPath + "UI Folder\\Shape\\TB-12.bmp", true, true, true);  //ITEM_USERDEFINE
	pItem->OffsetObject(CPoint(924, 1));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);
	
	pItem = new CItem_Class(this, m_hWnd, ITEM_CONE, theApp.m_strCurPath + "UI Folder\\Shape\\TB-15.bmp", true, true, true);
	pItem->OffsetObject(CPoint(999, 1));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, ITEM_FROMMIX, theApp.m_strCurPath + "UI Folder\\Shape\\TB-13.bmp", true, true, true);
	pItem->OffsetObject(CPoint(1074, 1));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, ITEM_BEAR, theApp.m_strCurPath + "UI Folder\\Shape\\TB-14.bmp", true, true, true);
	pItem->OffsetObject(CPoint(1149, 1));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, ITEM_USERDEFINE, theApp.m_strCurPath + "UI Folder\\Shape\\TB-16.bmp", true, true, true);
	pItem->OffsetObject(CPoint(1224, 1));
	pItem->SetUTipDll(m_pUTipDll, this);
	m_FramePic->AddItem(pItem);

	if (!theApp.m_bSampleVersion)
	{
		pItem = new CItem_Class(this, m_hWnd, ITEM_LOCK_UI, theApp.m_strCurPath + "UI Folder\\Shape\\lock.bmp", true, true, true, 12);
		pItem->OffsetObject(CPoint(1430, 1));
		pItem->SetUTipDll(m_pUTipDll, this);
		m_FramePic->AddItem(pItem);
	}

	m_FramePic->Initialize();

	CRect rectTemp;
	rectTemp = g_DispArys.GetAt(0);
	INT32 iWidth = rectTemp.Width();
	INT32 iHeight = rectTemp.Height();
	m_fZoomX = 1.0f;
	m_fZoomY = 1.0f;

	if (iWidth != 1920 || iHeight != 1080)
	{
		m_fZoomX = float(iWidth) / 1920;
		m_fZoomY = float(iHeight) / 1080;
		m_FramePic->OnZoom(0, 0, m_fZoomX, m_fZoomY);
	}

	m_FramePic->ResetEffectRegion();

#ifdef _LOAD_OBJ_THREAD_MODE
	m_FramePic->SetDlgRegion(this,false);
#else
	m_FramePic->SetDlgRegion(this);
#endif
	m_FramePic->SetWindowRgn(NullRgn, true);

	m_pNewFont = new CFont();
	m_pNewFont->CreateFontA(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Arial");

	m_VideoIdxCombox.Create(WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL | WS_VISIBLE,
		CRect(m_fZoomX * 1600, m_fZoomY * 700, m_fZoomX * 1640, m_fZoomY * 940), this, 0x3001);

	//m_AngleEdit.Create(WS_CHILD | SW_SHOW,CRect(m_fZoomX * 87, m_fZoomY * 310, m_fZoomX * 170,
		//               m_fZoomY * 370), this, 0x3001);

	m_sticVideoPath.Create("", WS_CHILD | WS_VISIBLE,
		CRect(m_fZoomX * 1650, m_fZoomY * 700, m_fZoomX * 1900, m_fZoomY * 740), this, 0x3002);

	g_PannelSetting.iNotifyAction = 0;

	m_dlgSetGrid.Create(IDD_SETGRID_DLG, this);
	m_dlgSetGrid.ShowWindow(SW_HIDE);
	SetTimer(0, 100, NULL);

	SetTip();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CShapeUIDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	m_bShow = bShow;

	if (m_FramePic)
	{
		if (bShow)
			m_FramePic->ShowWindow(SW_SHOW);
		else
			m_FramePic->ShowWindow(SW_HIDE);
	}

	// TODO: 在此加入您的訊息處理常式程式碼
}


BOOL CShapeUIDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	//g_PannelSetting.iSelIdx = 1;

	CString strTemp;
	strTemp.Format("CShape UI Dlg, %x, %x, %x", message, wParam, lParam);
	theApp.SaveLog(strTemp);
	static BOOL bBusy = FALSE;

	if (g_PannelSetting.iSelIdx == 0 && m_iMaxView != 0 || !IsWindowVisible())
		return CDialog::OnWndMsg(message, wParam, lParam, pResult);

	if ((m_iObjNum == 0 || m_iSelObjIdx == -1) && message >= ITEM_PTMOVE_SELECT && message <= ITEM_LAST && message != ITEM_2DMAP_EDGEBLEND)
	{
		return CDialog::OnWndMsg(message, wParam, lParam, pResult);
	}

	if (message >= ITEM_MOVE && message <= ITEM_LAST)
	{
		if (!bBusy)
			bBusy = TRUE;
		else
			return CDialog::OnWndMsg(message, wParam, lParam, pResult);
		EnterCriticalSection(&m_csFunBsy);
		g_PannelAry[0]->SetActiveWindow();
	}

	if ((message >= ITEM_MOVE && message <= ITEM_PTMOVE_SELECT) || (message < ITEM_LAST&& message > ITEM_PTMOVE_CANCEL))
	{
		if (g_PannelAry.GetCount() > 0)
		{
			for (int x = 0; x < g_PannelAry.GetCount(); x++)
				g_PannelAry[x]->CleanModifyPt();
		}
	}

	switch (message)
	{
		case ITEM_MAP:
			{
				if (lParam == 1)
				{
					m_bPositionMode = true;

					if(m_iObjNum > 0)
						EnableObjButtons(false);
				}
				else
				{
					m_bPositionMode = false;
					EnableObjButtons(true);
				}
			}
			break;
		case ITEM_MOVE:
		case ITEM_ROTATE:
		case ITEM_ZOOM:
		case ITEM_PTMOVE:
			{
				if (lParam == 1)
				{
					if (m_bPositionMode)
					{
						m_bPositionMode = false;
						EnableObjButtons(true);
					}
				}
			}
			break;
		case ITEM_LOADOBJ:
		case ITEM_BALL:
		case ITEM_CUBE:
		case ITEM_4PYRAMID:
		case ITEM_FIG:
		case ITEM_FLAT:
		case ITEM_USERDEFINE:
		case ITEM_BEAR:
		case ITEM_CONE:
		case ITEM_DISC:
		case ITEM_FROMMIX:
		{
#ifdef _TRAIL_VERSION
			if (m_iObjNum >= MAX_D3DNUM || g_StreamMappingAry.GetSize() >= 2)
#else
			if (m_iObjNum >= MAX_D3DNUM || g_StreamMappingAry.GetSize() > 25)
#endif
			{
				MessageBox("Can't Get More Object!", "Alarm", MB_TASKMODAL| MB_TOPMOST);
				GetParent()->SetFocus();
				break;
			}

#ifdef _LOAD_OBJ_THREAD_MODE
			LoadObj4Thread(message);
#else
			CLoadObjVdo dlgLoadObj;
			dlgLoadObj.SetVideoNum(g_PannelAry.GetSize() - 1);

			if (message == ITEM_LOADOBJ)
				dlgLoadObj.SetDefautlStatus(1);
			else
			{
				if (message == ITEM_USERDEFINE)
					dlgLoadObj.SetDefautlStatus(2);

				dlgLoadObj.LockLoadObj();
			}


			m_FramePic->OnEnable(FALSE);
			if (dlgLoadObj.DoModal() == IDOK)
			{
				CString strObjPath = dlgLoadObj.GetObjPath();
				CString strVideoPath = dlgLoadObj.GetVideoPath();
				DOUBLE dZoom = dlgLoadObj.GetZoolRatio();
				BOOL bExtraVideo = dlgLoadObj.IsExtraVideo();
				
				if (message != ITEM_LOADOBJ && message != ITEM_USERDEFINE)
				{
					strObjPath.Format("%sOBJ%i.obj", theApp.m_strCurPath + "Obj\\", message - ITEM_BALL + 1);
				}

				if (strObjPath.IsEmpty() || strVideoPath.IsEmpty())
				{
					m_FramePic->OnEnable(TRUE);
					GetParent()->SetFocus();
					break;
				}

				CString strNewPath;
				strNewPath.Format("%sMS_Shape%i.obj", theApp.m_strCurPath + "Obj\\", ++m_iCurTempObjIdx);
				CopyFile(strObjPath, strNewPath, FALSE);

				strObjPath = strNewPath;

				STREM_MAPPING StreamMap;
				StreamMap.strObjPath = strObjPath;
				StreamMap.strVideoPath = strVideoPath;
				StreamMap.bExtraVideo = bExtraVideo;

				if (strVideoPath.Left(6).Find("Video ") != -1)
				{
					StreamMap.iMediaIdx = atoi(strVideoPath.Right(strVideoPath.GetLength() - 6)) - 1;
					//StreamMap.iMediaIdx += DEF_GLOBAL_MEDIA;
					strVideoPath = "";
					StreamMap.iWidth = g_MediaStreamAry.at(StreamMap.iMediaIdx)->GetVideoWidth();
					StreamMap.iHeight = g_MediaStreamAry.at(StreamMap.iMediaIdx)->GetVideoHeight();
				}
				else
				{
					StreamMap.iMediaIdx = g_MediaStreamAry.size();

					for (int y = 0; y < g_MediaStreamAry.size(); y++)
					{
						if (g_MediaStreamAry.at(y)->FilePath().IsEmpty())
						{
							StreamMap.iMediaIdx = y;
							break;
						}
					}
				}

				StreamMap.iPanelIdx = g_PannelSetting.iSelIdx;
				StreamMap.iObjIdx = g_PannelAry[g_PannelSetting.iSelIdx]->GetObjCount();
				g_StreamMappingAry.Add(StreamMap);
				if (g_PannelAry.GetSize() > 1)
					g_PannelAry[0]->ShowTempVideo(strObjPath, "", dZoom);
				//g_PannelAry[dlgLoadObj.GetSelView()]->ShowTempVideo(strObjPath, strVideoPath);
				g_PannelAry[g_PannelSetting.iSelIdx]->ShowTempVideo(strObjPath, strVideoPath, dZoom, bExtraVideo);
							
				if (0)//strVideoPath.IsEmpty())
				{
					CString strTemp =	g_MediaStreamAry.at(StreamMap.iMediaIdx)->FilePath();
					g_StreamMappingAry.GetAt(g_StreamMappingAry.GetCount() - 1).strVideoPath = strTemp;
				}

				INT32 iObjNum = g_PannelAry[g_PannelSetting.iSelIdx]->GetObjCount();
				SetModalNum(iObjNum, TRUE);
				theApp.SaveShapeProject(theApp.m_strCurPath + "Mapping.xml");

				if (abs(dZoom - 1.0f) > 0.01f)
				{
					g_PannelAry[0]->SaveObjectSetting();
				}
			}
			m_FramePic->OnEnable(TRUE);
			GetParent()->SetFocus();
#endif
		}
		break;
//		case ITEM_FROMMIX:
	//		break;
		case ITEM_PTMOVE_CANCEL:
		{
			g_PannelAry[0]->CleanModifyPt();

			if (g_PannelSetting.iSelIdx != 0)
				g_PannelAry[g_PannelSetting.iSelIdx]->CleanModifyPt();

			CItem_Class* pItem = NULL;
			pItem = (CItem_Class*)m_FramePic->GetItemByID(ITEM_PTMOVE_SELECT);

			if (pItem)
				pItem->OnBnClicked();
		}
			break;
		case ITEM_PTMOVE_OK:
		{
			g_PannelAry[0]->ReleaseFocus(TRUE);
			if (MessageBox("Point Correct? This action can't undo", "Modify", MB_OKCANCEL | MB_TASKMODAL| MB_TOPMOST) == IDOK)
			{
				STREM_MAPPING strmCur = theApp.GetStreamMapByIdx(g_PannelSetting.iSelIdx, m_iSelObjIdx);
				g_PannelAry[0]->SaveModifyPt(strmCur.strObjPath);

				if (g_PannelSetting.iSelIdx != 0)
					g_PannelAry[g_PannelSetting.iSelIdx]->SaveModifyPt();

				CItem_Class* pItem = NULL;
				pItem = (CItem_Class*)m_FramePic->GetItemByID(ITEM_PTMOVE_SELECT);

				if (pItem)
					pItem->OnBnClicked();

			}
			g_PannelAry[0]->ReleaseFocus(FALSE);
		}
		break;
		case ITEM_MODEL_DEL:
		{
			g_PannelAry[0]->ReleaseFocus(TRUE);
			if (MessageBox("Delete Object? This action can't undo.", "Delete", MB_OKCANCEL | MB_TASKMODAL| MB_TOPMOST) == IDOK)
			{
				if (g_PannelSetting.iSelIdx != 0)
					g_PannelAry[g_PannelSetting.iSelIdx]->DeleteObject(m_iSelObjIdx);

				g_PannelAry[0]->DeleteObject(m_iSelObjIdx);
				SetModalNum(m_iObjNum - 1);

				GetParent()->SetFocus();
			}
			g_PannelAry[0]->ReleaseFocus(FALSE);
		}
		break;
		case ITEM_UNDO:
		{

			INT32 iResult = g_PannelAry[0]->UndoAction();
			
			if (iResult >= 0)
			{
				for (int x = 1; x < g_PannelAry.GetCount(); x++)
					g_PannelAry[x]->UndoAction(iResult);
			}

			
		}
		break;
		case ITEM_REDO:
		{
			INT32 iResult = g_PannelAry[0]->RedoAction();

			if (iResult >= 0)
			{
				for (int x = 1; x < g_PannelAry.GetCount(); x++)
					g_PannelAry[x]->RedoAction(iResult);
			}
		}
		break;
		case ITEM_NEW:
			NewProject(FALSE);
			break;
		case ITEM_OPEN:
#if 0
			if (NewProject(TRUE))
			{
				m_FramePic->OnEnable(FALSE);
				TCHAR szFilters[] = _T("Project Files (*.xml)|*.xml|All Files (*.*)|*.*||");
				CFileDialog fileDlg(TRUE, _T("xml"), _T("*.xml"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

				if (fileDlg.DoModal() == IDOK)
				{
					CPlaneScene3D::FileDecoder(fileDlg.GetPathName());

					if (1)
					{
						CFile sfSrc;
						CStdioFile sfDest;
						char cData[1] = "";
						BOOL bOpen = FALSE;
						CFileFind ffFinder;
						BOOL bError = FALSE;

						sfSrc.Open(fileDlg.GetPathName(), CFile::modeRead);
						strTemp = "";
						while (sfSrc.GetPosition() < sfSrc.GetLength())
						{
							sfSrc.Read(cData, 1);
							if (cData[0] != '\n')
								strTemp += cData[0];
							else
							{
								if (strTemp.Find("=== MS File Path:") != -1)
								{
									strTemp.Replace("=== MS File Path:", "");
									strTemp.Trim();

									if (bOpen)
										sfDest.Close();

									/*if (!ffFinder.FindFile(strTemp))
									{
										bError = TRUE;
										bOpen = FALSE;
										break;
									}*/

									if (strTemp.Right(11) == "Mapping.xml")
										strTemp = theApp.m_strCurPath + "Mapping.xml";
									else if (strTemp.Right(9) == "Shape.xml")
										strTemp = theApp.m_strCurPath + "Shape.xml";
									else if (strTemp.Find("\\Obj\\MS_Shape") != -1)
									{
										INT32 iFindPos = strTemp.Find("\\Obj\\MS_Shape");
										strTemp = strTemp.Right(strTemp.GetLength() - iFindPos);
										strTemp = theApp.m_strCurPath + strTemp;

									}
									else
									{
										bError = TRUE;
										bOpen = FALSE;
										break;
									}

									sfDest.Open(strTemp, CFile::modeCreate | CFile::modeReadWrite);
									bOpen = TRUE;
								}
								else
								{
									if (bOpen)
									{
										if (strTemp.Find("\\Obj\\MS_Shape") != -1)
										{
											INT32 iFindPos1 = strTemp.Find("Path=");
											INT32 iFindPos2 = strTemp.Find("\\Obj\\MS_Shape");

											strTemp.Format("%sPath=\"%s", strTemp.Left(iFindPos1), 
												theApp.m_strCurPath + 
												strTemp.Right(strTemp.GetLength() - iFindPos2 - 1));
										}
										sfDest.WriteString(strTemp + "\n");
									}
								}

								strTemp = "";
							}
						}

						sfSrc.Close();

						if (bOpen)
							sfDest.Close();

						if (!bOpen || bError)
						{
							CPlaneScene3D::FileEncoder(fileDlg.GetPathName());
							m_FramePic->OnEnable(TRUE);
							break;
						}

					}
					else
					{
						CopyFile(fileDlg.GetPathName(), theApp.m_strCurPath + "Mapping.xml", FALSE);
						CopyFile(fileDlg.GetPathName() + "shp", theApp.m_strCurPath + "Shape.xml", FALSE);
						CString strOrgObjPath;
						CFileFind fFind;
						INT32 iIndex;
						BOOL bWork = fFind.FindFile(fileDlg.GetPathName() + "Obj*");
						m_iCurTempObjIdx = 0;

						while (bWork)
						{
							bWork = fFind.FindNextFile();
							strOrgObjPath = fFind.GetFilePath();
							strTemp = strOrgObjPath.Right(3);
							strTemp.Replace("b", "");
							strTemp.Replace("j", "");
							iIndex = atoi(strTemp);

							strTemp.Format("%sMS_Shape%i.obj", theApp.m_strCurPath + "Obj\\", iIndex);
							CopyFile(strOrgObjPath, strTemp, FALSE);

							if (m_iCurTempObjIdx < iIndex)
								m_iCurTempObjIdx = iIndex;
						}
					}					

					CPlaneScene3D::FileEncoder(fileDlg.GetPathName());
					LoadProject("");
				}
				m_FramePic->OnEnable(TRUE);
			}
#else
			{
				OpenProject();

				if (m_iObjNum > 0 && m_bPositionMode == true)
					EnableObjButtons(false);
			}
#endif
			break;
		case ITEM_SAVE:
		{
			m_FramePic->OnEnable(FALSE);

			g_PannelAry[0]->SaveObjectSetting();

#ifdef _ENABLE_SPF
			TCHAR szFilters[] = _T("Project Files (*.spf)|*.spf|All Files (*.*)|*.*||");
			CFileDialog fileDlg(FALSE, _T("spf"), _T("*.spf"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters,this);

			if (fileDlg.DoModal() == IDOK)
			{
				theApp.SaveShapeProject(theApp.m_strCurPath + "Mapping.xml");

				CFileFind fFind;
				g_PannelAry[0]->ReleaseFocus(TRUE);
				if (fFind.FindFile(fileDlg.GetPathName()) && MessageBox("The File Has Existed, Overwrite?",
					"Overwrite", MB_OKCANCEL | MB_TASKMODAL| MB_TOPMOST) != IDOK)
				{
					g_PannelAry[0]->ReleaseFocus(FALSE);
					m_FramePic->OnEnable(TRUE);
					break;
				}

				g_PannelAry[0]->ReleaseFocus(FALSE);

#ifdef _ENABLE_SPF
				if (m_pZLibDllObj)
				{
					char szFileNameInZip[256];

					m_pZLibDllObj->CreateZip(fileDlg.GetPathName().GetBuffer());

					for (int x = -1; x <= m_iCurTempObjIdx; x++)
					{
						if (x == -1)
						{
							strTemp = theApp.m_strCurPath + "Mapping.xml";
							m_pZLibDllObj->AddFile("Mapping.xml", strTemp.GetBuffer());
						}
						else if (x == 0)
						{
							strTemp = theApp.m_strCurPath + "Shape.xml";
							m_pZLibDllObj->AddFile("Shape.xml", strTemp.GetBuffer());
						}
						else
						{
							strTemp.Format("%sMS_Shape%i.obj", theApp.m_strCurPath + "Obj\\", x);

							sprintf(szFileNameInZip,"Obj\\MS_Shape%i.obj",x);
							m_pZLibDllObj->AddFile(szFileNameInZip, strTemp.GetBuffer());
						}
					}

					m_pZLibDllObj->CloseZip();
				}
#endif
			}
#else
			TCHAR szFilters[] = _T("Project Files (*.xml)|*.xml|All Files (*.*)|*.*||");
			CFileDialog fileDlg(FALSE, _T("xml"), _T("*.xml"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

			if (fileDlg.DoModal() == IDOK)
			{
				if (fileDlg.GetFileName() == "\\Shape.xml")
				{
					AfxMessageBox("Can't be [Shape.xml]");
				}
				else
				{
					theApp.SaveShapeProject(theApp.m_strCurPath + "Mapping.xml");

					if (1)
					{
						CFileFind fFind;
						CFile sfSrc;
						CStdioFile sfDest;
						char cData[1] = "";

						g_PannelAry[0]->ReleaseFocus(TRUE);
						if (fFind.FindFile(fileDlg.GetPathName()) && MessageBox("The File Has Existed, Overwrite?",
							"Overwrite", MB_OKCANCEL | MB_TASKMODAL) != IDOK)
						{
							g_PannelAry[0]->ReleaseFocus(FALSE);
							m_FramePic->OnEnable(TRUE);
							break;
						}

						g_PannelAry[0]->ReleaseFocus(FALSE);

						sfDest.Open(fileDlg.GetPathName(), CFile::modeCreate | CFile::modeReadWrite);

						for (int x = -1; x <= m_iCurTempObjIdx; x++)
						{
							if (x == -1)
								strTemp = theApp.m_strCurPath + "Mapping.xml";
							else if (x == 0)
								strTemp = theApp.m_strCurPath + "Shape.xml";
							else
								strTemp.Format("%sMS_Shape%i.obj", theApp.m_strCurPath + "Obj\\", x);

							sfDest.WriteString("=== MS File Path:" + strTemp + "\n");
							sfSrc.Open(strTemp, CFile::modeRead);

							strTemp = "";
							while (sfSrc.GetPosition() < sfSrc.GetLength())
							{
								sfSrc.Read(cData, 1);
								if (cData[0] != '\n')
									strTemp += cData[0];
								else
								{
									sfDest.WriteString(strTemp + "\n");
									strTemp = "";
								}
							}

							sfSrc.Close();
						}

						sfDest.Close();
						CPlaneScene3D::FileEncoder(fileDlg.GetPathName());
					}
					else
					{
						CopyFile(theApp.m_strCurPath + "Mapping.xml", fileDlg.GetPathName(), FALSE);
						CopyFile(theApp.m_strCurPath + "Shape.xml", fileDlg.GetPathName() + "shp", FALSE);
						CString strNewObjPath;

						for (int x = 1; x <= m_iCurTempObjIdx; x++)
						{
							strTemp.Format("%sMS_Shape%i.obj", theApp.m_strCurPath + "Obj\\", x);
							strNewObjPath.Format("%sObj%i", fileDlg.GetPathName(), x);

							CopyFile(strTemp, strNewObjPath, FALSE);
						}

					}
				}
			}
#endif
			m_FramePic->OnEnable(TRUE);
		}
		break;
		case ITEM_2DMAP_SetGrid:
		{
			g_PannelAry[g_PannelSetting.iSelIdx]->Set2DFunction(1, 0);
		}
		break;
		case ITEM_2DMAP_SetAlpha:
		{
			g_PannelAry[0]->ReleaseFocus(TRUE);
			m_FramePic->OnEnable(FALSE);
			m_dlgSetGrid.ShowWindow(SW_SHOW);
		}
		break;
		case ITEM_2DMAP_SELECT:
		{
			g_PannelAry[g_PannelSetting.iSelIdx]->Set2DMapAction(MAP2D_SELECT_PT| MAP2D_SHOW_BK_PT);
		}
		break;
		case ITEM_2DMAP_MOVE:
		{
			g_PannelAry[g_PannelSetting.iSelIdx]->Set2DMapAction(MAP2D_MODIFY_PT | MAP2D_SHOW_BK_PT);
		}
		break;
		case ITEM_2DMAP_OK:
		{
		}
		break;
		case ITEM_2DMAP_CANCEL:
		{
		}
		break;
		case ITEM_2DMAP_EDGEBLEND:
		{
#ifdef _DEBUG_EDGE_BLENDING
#else
			if (g_DispArys.GetCount() > 2)
#endif
			{
				CEdgeBlendDlg dlgEgdeBlend;
				KillTimer(0);

				for (int x = 1; x < g_PannelAry.GetCount(); x++)
					g_PannelAry[x]->FreezeDisplay(TRUE);
				if (dlgEgdeBlend.DoModal())
				{

				}
				//for (int x = 1; x < g_PannelAry.GetCount(); x++)
					//g_PannelAry[x]->FreezeDisplay(FALSE);
				for (int x = 1; x < g_PannelAry.GetCount(); x++)
					g_PannelAry[x]->FreezeDisplay(FALSE);

				SetTimer(0, 100, NULL);
			}
		}
		break;
		case WM_SETGRID:
		{
			UINT iPannelIdx = (UINT)wParam;
			g_PannelAry[iPannelIdx]->Create2DMapping(m_dlgSetGrid.m_iGridNumX, m_dlgSetGrid.m_iGridNumY);
			g_PannelAry[iPannelIdx]->Set2DMapAction(MAP2D_NEED_UPDATE_BK);
		}
		break;
		case WM_SETCURVE:
		{
			UINT iPannelIdx = (UINT)wParam;
			UINT iAreaIdx = (UINT) lParam;
			g_PannelAry[iPannelIdx]->Set2DColor(m_dlgSetGrid.m_ColorAry, iAreaIdx, m_dlgSetGrid.m_ptDark);
		}
		break;
		case WM_SETGAMMA:
		{
			UINT iPannelIdx = (UINT)wParam;
			g_PannelAry[iPannelIdx]->Set2DFunction(2, m_dlgSetGrid.m_uGammaVal);
		}
		break;
		case WM_SETGAIN:
		{
			UINT iPannelIdx = (UINT)wParam;
			g_PannelAry[iPannelIdx]->Set2DFunction(3, m_dlgSetGrid.m_clrGain);
		}
		break;
		case WM_PRESETGAIN:
		{
			UINT iPannelIdx = (UINT)wParam;
			g_PannelAry[iPannelIdx]->PresetPanelGain(m_dlgSetGrid.m_uPanelGain[iPannelIdx]);
		}
		break;
		case WM_INIT2DMAPPING:
		{
			UINT iPannelIdx = (UINT)wParam;
			UINT iStatus = (UINT)lParam;

			if (iStatus == 2)
				m_bHadInit2DMap = TRUE;
			else if (!m_bHadInit2DMap)
			{
				g_PannelAry[iPannelIdx]->Init2DMapping(iStatus);
			}
		}
		break;
		case WM_SETTEXTURE:
		{
			UINT iPannelIdx = (UINT)wParam;
			UINT iIndex = (UINT)lParam;
			g_PannelAry[iPannelIdx]->Set2DFunction(0, iIndex);
		}
		break;
		case WM_SETGRID_CLOSE:
		{
			g_PannelAry[0]->ReleaseFocus(FALSE);
			m_FramePic->OnEnable(TRUE);
		}
		break;
		case WM_SYSCOMMAND:
			{
				if (SC_CLOSE == wParam)
					return true;
				else if (SC_KEYMENU == wParam)
					return true;
			}
		break;
		default:
			break;
	}

	if (message >= ITEM_MOVE && message <= ITEM_LAST)
	{
		bBusy = FALSE;
		LeaveCriticalSection(&m_csFunBsy);
	}

	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

void CShapeUIDlg::SetProjectorNum(INT32 iProjectorNum)
{
	if (m_iMaxView > 0 && m_iMaxView != iProjectorNum)
	{
		for (int x = 0; x < m_iMaxView; x++)
			m_FramePic->DeleteItem(ITEM_VIEW1 + x);
	}

	CRgn NullRgn;
	NullRgn.CreateRectRgn(0, 0, 0, 0);
	CItem_Class* pItem;

	CRect rectTemp;
	rectTemp = g_DispArys.GetAt(0);
	INT32 iWidth = rectTemp.Width();
	INT32 iHeight = rectTemp.Height();

	for (int x = 0; x < iProjectorNum; x++)
	{
		pItem = new CItem_Class(this, m_hWnd, ITEM_VIEW1 + x, theApp.m_strCurPath + "UI Folder\\Shape\\View.bmp", true, true, true, 4);
		pItem->OffsetObject(CPoint(1532 + x * 75, 2));
		pItem->SetUTipDll(m_pUTipDll, this);
		pItem->Initialize();
		if (iWidth != 1920 || iHeight != 1080)
			pItem->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);
		m_FramePic->AddItem(pItem);

		if (x == 0)
			pItem->OnBnClicked();
	}
	m_FramePic->ResetEffectRegion();

	if (m_bShow)
		m_FramePic->SetDlgRegion(this);
	else
		m_FramePic->SetDlgRegion(this,false);
	m_FramePic->OnPaint();
	m_FramePic->SetWindowRgn(NullRgn, true);
	m_iMaxView = iProjectorNum;
	g_PannelSetting.iSelIdx = 1;
}

void CShapeUIDlg::ShowSubItem()
{
	CRgn NullRgn;
	NullRgn.CreateRectRgn(0, 0, 0, 0);
	CItem_Class* pItem;
	CRect rectTemp;
	rectTemp = g_DispArys.GetAt(0);
	INT32 iWidth = rectTemp.Width();
	INT32 iHeight = rectTemp.Height();
	CString strTemp;

	static BOOL bPreShowMoveItem = FALSE;
	static BOOL bPreShowZOOMItem = FALSE;
	static BOOL bPreShowRotateItem = FALSE;
	static BOOL bPreSho2DMapItem = FALSE;
	static BOOL bPre3DAutoMapItem = FALSE;
	static INT32 iPreSelObjIdx = -1;
	BOOL bModelModify = ((g_PannelSetting.iModelAction & MODEL_MODIFY) > 0);
	BOOL bModelZoom = ((g_PannelSetting.iModelAction & MODEL_ZOOM) > 0);
	BOOL bRotateItem = ((g_PannelSetting.iModelAction & 0xf) == MODEL_ROTATE);
	BOOL b2DMapping = ((g_PannelSetting.iModelAction & MODEL_2DMAPPING) > 0);
	BOOL b3DAutoMap = ((g_PannelSetting.iModelAction & 0xf) == MODEL_MAP);

	//========== Point Modify Item ==============//
	for (int x = 0; x < 4; x++)
	{
		if (bModelModify && !bPreShowMoveItem)
		{
			strTemp.Format("UI Folder\\Shape\\PT MOVE-%i.bmp", x + 1);

			if (x < 2)
				pItem = new CItem_Class(this, m_hWnd, ITEM_PTMOVE_SELECT + x, theApp.m_strCurPath + strTemp, true, true, true, 6);
			else
				pItem = new CItem_Class(this, m_hWnd, ITEM_PTMOVE_SELECT + x, theApp.m_strCurPath + strTemp, true, true, true);
			pItem->OffsetObject(CPoint(82 + (x % 2) * 36, 400 + INT(x / 2) * 36));
			pItem->Initialize();
			if (iWidth != 1920 || iHeight != 1080)
				pItem->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);
			m_FramePic->AddItem(pItem);

			if (x == 0)
				pItem->OnBnClicked();
		}
		else if (!bModelModify && bPreShowMoveItem)
			m_FramePic->DeleteItem(ITEM_PTMOVE_SELECT + x, FALSE);
	}

	if ((!bModelModify && bPreShowMoveItem) || (!bPre3DAutoMapItem && b3DAutoMap))
	{
		if (g_PannelAry.GetCount() > 0)
		{
			for (int x = 0; x < g_PannelAry.GetCount(); x++)
				g_PannelAry[x]->CleanModifyPt();
		}
	}

	//========== Rotate Sub Item ==============//
	for (int x = 0; x < 4; x++)
	{
		if (bRotateItem && !bPreShowRotateItem)
		{
			strTemp.Format("UI Folder\\Shape\\Rotation%i.bmp", x + 1);

			pItem = new CItem_Class(this, m_hWnd, ITEM_ROTATE_X + x, theApp.m_strCurPath + strTemp, true, true, true, 8);
			
			pItem->OffsetObject(CPoint(82 + (x % 2) * 36, 237 + INT(x / 2) * 36));
			pItem->Initialize();
			if (iWidth != 1920 || iHeight != 1080)
				pItem->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);
			m_FramePic->AddItem(pItem);

			if (x == 0)
				pItem->OnBnClicked();
		}
		else if (!bRotateItem && bPreShowRotateItem)
			m_FramePic->DeleteItem(ITEM_ROTATE_X + x, FALSE);
	}


	if (0)//!theApp.m_bSampleVersion)
	{
		if (bRotateItem && !bPreShowRotateItem)
		{
		strTemp = "UI Folder\\Shape\\PT_ANGLE.bmp";
		pItem = new CItem_Class(this, m_hWnd, ITEM_ROTATE_ANGLE, theApp.m_strCurPath + strTemp,
		true, true, true, 15);

		//m_AngleEdit.SetWindowPos(NULL, 90, 200, 38, 38, SW_SHOW);
		pItem->OffsetObject(CPoint(82, 200));
		pItem->Initialize();
		if (iWidth != 1920 || iHeight != 1080)
		pItem->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);
		m_FramePic->AddItem(pItem);
		}
		else if (!bRotateItem && bPreShowRotateItem)
		m_FramePic->DeleteItem(ITEM_ROTATE_ANGLE, FALSE);
	}

	
	//========== Zoom Sub Item ==============//
	for (int x = 0; x < 2; x++)
	{
		if (bModelZoom && !bPreShowZOOMItem)
		{
			strTemp.Format("UI Folder\\Shape\\Zoom-%i.bmp", x + 1);

			pItem = new CItem_Class(this, m_hWnd, ITEM_ZOOM_ALL + x, theApp.m_strCurPath + strTemp, true, true, true, 7);
			pItem->OffsetObject(CPoint(82, 313 + x * 36));
			pItem->Initialize();
			if (iWidth != 1920 || iHeight != 1080)
				pItem->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);
			m_FramePic->AddItem(pItem);

			if (x == 0)
				pItem->OnBnClicked();
		}
		else if (!bModelZoom && bPreShowZOOMItem)
			m_FramePic->DeleteItem(ITEM_ZOOM_ALL + x, FALSE);
	}

	//========== 2D Mapping Item ==============//
	INT32 iIndex;
	for (int x = ITEM_2DMAP_SetGrid; x <= ITEM_2DMAP_SHOWPT; x++)
	{
		if (b2DMapping && !bPreSho2DMapItem)
		{
			iIndex = x - ITEM_2DMAP_SetGrid;

			if (iIndex < 2)
				strTemp.Format("UI Folder\\Shape\\Rotation%i.bmp", iIndex + 1);
			else if (iIndex >= 2 && iIndex <= 5)
				strTemp.Format("UI Folder\\Shape\\PT MOVE-%i.bmp", iIndex - 1);
			else
				strTemp.Format("UI Folder\\Shape\\Rotation%i.bmp", iIndex - 3);

			if (x == ITEM_2DMAP_SELECT || x == ITEM_2DMAP_MOVE)
				pItem = new CItem_Class(this, m_hWnd, x, theApp.m_strCurPath + strTemp, true, true, true, 10);
			else if (x == ITEM_2DMAP_SHOWPT)
				pItem = new CItem_Class(this, m_hWnd, x, theApp.m_strCurPath + strTemp, true, true, true, 11);
			else
				pItem = new CItem_Class(this, m_hWnd, x, theApp.m_strCurPath + strTemp, true, true, true);

			pItem->OffsetObject(CPoint(82 + (iIndex % 2) * 36, 710 + INT(iIndex / 2) * 36));
			pItem->Initialize();
			if (iWidth != 1920 || iHeight != 1080)
				pItem->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);
			m_FramePic->AddItem(pItem);

		}
		else if (!b2DMapping && bPreSho2DMapItem)
			m_FramePic->DeleteItem(x, FALSE);
	}

	//========== Model Sub Item ==============//
	
	if (0)//m_iSelObjIdx != iPreSelObjIdx)
	{
		for (int y = 0; y < 4; y++)
		{
			m_FramePic->DeleteItem(ITEM_MODEL_DEL + y, FALSE);
			strTemp.Format("UI Folder\\Shape\\PT MOVE-%i.bmp", y + 1);

			if (m_iSelObjIdx >= 0)
			{
				pItem = new CItem_Class(this, m_hWnd, ITEM_MODEL_DEL + y, theApp.m_strCurPath + strTemp, true, true, true);
				
				pItem->OffsetObject(CPoint(1666 + (y % 2) * 36, m_iSelObjIdx * 75 + INT(y / 2) * 36 + 172));
				pItem->Initialize();

				if (iWidth != 1920 || iHeight != 1080)
					pItem->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);
				m_FramePic->AddItem(pItem);
			}
		}
	}

	if (bPreShowZOOMItem != bModelZoom || bPreShowMoveItem != bModelModify || 
		bRotateItem != bPreShowRotateItem || iPreSelObjIdx != m_iSelObjIdx || bPreSho2DMapItem != b2DMapping)
	{
		//m_FramePic->ResetEffectRegion();
		m_FramePic->SetDlgRegion(this);
		//sm_FramePic->OnPaint();
		m_FramePic->SetWindowRgn(NullRgn, true);
	}

	bPreShowMoveItem = bModelModify;
	bPreShowZOOMItem = bModelZoom;
	bPreShowRotateItem = bRotateItem;
	bPreSho2DMapItem = b2DMapping;
	iPreSelObjIdx = m_iSelObjIdx;
	bPre3DAutoMapItem = b3DAutoMap;
}

void CShapeUIDlg::SetModalNum(INT32 iModalNum, BOOL bClickLastModel)
{

	CRgn NullRgn;
	NullRgn.CreateRectRgn(0, 0, 0, 0);
	CItem_Class* pItem;
	CRect rectTemp;
	rectTemp = g_DispArys.GetAt(0);
	INT32 iWidth = rectTemp.Width();
	INT32 iHeight = rectTemp.Height();


	if (m_iObjNum != iModalNum)
	{
		if (m_iObjNum > iModalNum)
		{
			for (int x = iModalNum; x < m_iObjNum; x++)
				m_FramePic->DeleteItem(ITEM_OBJ1 + x);
		}
		else
		{
			for (int x = m_iObjNum; x < iModalNum; x++)
			{
				pItem = new CItem_Class(this, m_hWnd, ITEM_OBJ1 + x, theApp.m_strCurPath + "UI Folder\\Shape\\TB-08.bmp", true, true, true, 5);
				pItem->OffsetObject(CPoint(1842, 85 + x * 77));
				pItem->SetUTipDll(m_pUTipDll, this);
				pItem->Initialize();
				if (iWidth != 1920 || iHeight != 1080)
					pItem->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);

				pItem->Initialize();
				m_FramePic->AddItem(pItem);

				if (bClickLastModel && x == iModalNum - 1)
					pItem->OnBnClicked();
			}
		}
		m_FramePic->ResetEffectRegion();
		m_FramePic->SetDlgRegion(this);
		m_FramePic->OnPaint();
		m_FramePic->SetWindowRgn(NullRgn, true);
		m_iObjNum = iModalNum;
	}

	if (!bClickLastModel)
	{
		m_iSelObjIdx = -1;
		g_PannelAry[g_PannelSetting.iSelIdx]->SetObjIdx(m_iSelObjIdx);
		g_PannelAry[0]->SetObjIdx(m_iSelObjIdx);
	}
	else
	{
		m_iSelObjIdx = iModalNum  -1;
		g_PannelAry[g_PannelSetting.iSelIdx]->SetObjIdx(m_iSelObjIdx);
		g_PannelAry[0]->SetObjIdx(m_iSelObjIdx);
	}
}


void CShapeUIDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此加入您的訊息處理常式程式碼
					   // 不要呼叫圖片訊息的 CDialog::OnPaint()

	if (m_bShow)
	{
		if (m_iMaxView == 0 && g_PannelAry.GetSize() > 1)
			SetProjectorNum(g_PannelAry.GetSize() - 1);
	}
}

BOOL CShapeUIDlg::NewProject(BOOL bOpen)
{
	BOOL bResult = FALSE;

	g_PannelAry[0]->ReleaseFocus(TRUE);

	if (bOpen)
		bResult = MessageBox("Open Another Project? It will lose any action without saveing.", 
			      "Open Project", MB_OKCANCEL | MB_TASKMODAL| MB_TOPMOST) == IDOK;
	else
		bResult = MessageBox("Open New Project? It will lose any action without saveing.",
			      "New Project", MB_OKCANCEL | MB_TASKMODAL| MB_TOPMOST) == IDOK;

	if (bResult)
	{
		for (int x = 0; x < g_PannelAry.GetCount(); x++)
			g_PannelAry[x]->DeleteObject(-1);

		SetModalNum(0);
		theApp.DeleteObject(-1, 0);

		for (int x = 0; x < g_MediaStreamAry.size(); x++)
		{
			g_MediaStreamAry.at(x)->Close(TRUE);
		}
		bResult = TRUE;
	}

	m_iCurTempObjIdx = 0;

	g_PannelAry[0]->ReleaseFocus(FALSE);

	return bResult;
}

void CShapeUIDlg::LoadProject(CString strProjectPath)
{
	if (m_iMaxView == 0 && g_PannelAry.GetSize() > 1)
		SetProjectorNum(g_PannelAry.GetSize() - 1);

	if (strProjectPath.IsEmpty())
		strProjectPath = theApp.m_strCurPath + "Mapping.xml";

	CFileFind fFind;
	if (!fFind.FindFile(strProjectPath))
		return;

	tinyxml2::XMLDocument doc;
	doc.LoadFile(strProjectPath);
	CString strTemp, strMSPath;
	STREM_MAPPING StreamMap;
	CString strVideoPath;
		   
	auto project = doc.FirstChildElement("Project");
	if (!project)
		return;

	auto mapping = project->FirstChildElement("Mapping");
	if (!mapping)
		return;

	auto pSubItem = mapping;
	for (auto o = mapping; o != 0; o = o->NextSiblingElement())
	{
		pSubItem = o->FirstChildElement("Video");
		strTemp = pSubItem->Attribute("Path");
		strVideoPath = StreamMap.strVideoPath = strTemp;
		strTemp = pSubItem->Attribute("Index");
		StreamMap.iMediaIdx = atoi(strTemp);
		strTemp = pSubItem->Attribute("Extra");
		StreamMap.bExtraVideo = atoi(strTemp);

		pSubItem = o->FirstChildElement("Object");
		strTemp = pSubItem->Attribute("Path");
		StreamMap.strObjPath = strTemp;

		if (strTemp.Right(15).Find("MS_Shape") != -1)
		{
			INT32 iStrPos = strTemp.Right(15).Find("MS_Shape");
			strTemp = strTemp.Right(15).Right(15 - iStrPos - 8);
			strTemp.Replace(".obj", "");
			m_iCurTempObjIdx = atoi(strTemp);
		}
		strTemp = pSubItem->Attribute("Index");
		StreamMap.iObjIdx = atoi(strTemp);

		pSubItem = o->FirstChildElement("Pannel");
		strTemp = pSubItem->Attribute("Index");
		StreamMap.iPanelIdx = atoi(strTemp);

		if (StreamMap.iPanelIdx > m_iMaxView || (StreamMap.iPanelIdx == 0 && m_iMaxView > 0))
			continue;
		else
			g_PannelSetting.iSelIdx = StreamMap.iPanelIdx;

		strMSPath = g_MediaStreamAry[StreamMap.iMediaIdx]->FilePath();

		if (strMSPath.IsEmpty()) //先不由Shape Load Media, 等日後軟體包再改
		{
			StreamMap.iWidth = 1920;
			StreamMap.iHeight = 1080;
		}

		if (strVideoPath.Find("!@#$%^") != -1)
			strVideoPath.Replace("!@#$%^", "");

		if (strMSPath.Find("!@#$%^") != -1)
			strMSPath.Replace("!@#$%^", "");

		if (strVideoPath.GetLength() < 6)
		{
			StreamMap.iMediaIdx = StreamMap.iMediaIdx;
			//StreamMap.iMediaIdx += DEF_GLOBAL_MEDIA;

			strVideoPath = StreamMap.strVideoPath = "";
			StreamMap.iWidth = g_MediaStreamAry[StreamMap.iMediaIdx]->GetVideoWidth();
			StreamMap.iHeight = g_MediaStreamAry[StreamMap.iMediaIdx]->GetVideoHeight();
		}
		else if (strVideoPath.Left(6).Find("Video ") != -1)
		{
			StreamMap.iMediaIdx = atoi(strVideoPath.Mid(6, 2).Trim()) - 1;
			//StreamMap.iMediaIdx += DEF_GLOBAL_MEDIA;

			strMSPath = g_MediaStreamAry[StreamMap.iMediaIdx]->FilePath();
			if (strMSPath.Find("!@#$%^") != -1)
				strMSPath.Replace("!@#$%^", "");

			strVideoPath = strVideoPath.Mid(9, strVideoPath.GetLength() - 10);

			//先不由Shape Load Media, 等日後軟體包再改
			if (0)//strMSPath.IsEmpty() || strMSPath.Find(strVideoPath) == -1)
			{
				INT32 iStrPos = 0;
				strTemp = StreamMap.strVideoPath;

				StreamMap.iMediaIdx = -1;
				for (int x = 0; x < g_MediaStreamAry.size(); x++)
				{
					if (g_MediaStreamAry.at(x)->FilePath().IsEmpty())
					{
						StreamMap.iMediaIdx = x;
						break;
					}
				}

				if (StreamMap.iMediaIdx == -1)
					continue;

				for (int x = 0; x < strTemp.GetLength(); x++)
				{
					if (strTemp.GetAt(x) == '(')
					{
						iStrPos = x + 1;
						break;
					}
				}

				if (iStrPos == 0)
					continue;

				strTemp = strTemp.Mid(iStrPos, strTemp.GetLength() - iStrPos - 1);

				if (!StreamMap.bExtraVideo && strTemp.Find("!@#$%^") != -1)
					strTemp.Replace("!@#$%^", "");

				strVideoPath = StreamMap.strVideoPath = strTemp;
			}
			else
			{			
				if (!g_MediaStreamAry[StreamMap.iMediaIdx]->IsOpened())
				{
					//continue;
					strVideoPath = "";
					StreamMap.iWidth = 1920;
					StreamMap.iHeight = 1080;
				}
				else
				{
					strVideoPath = "";
					StreamMap.iWidth = g_MediaStreamAry[StreamMap.iMediaIdx]->GetVideoWidth();
					StreamMap.iHeight = g_MediaStreamAry[StreamMap.iMediaIdx]->GetVideoHeight();
				}

			}
		}
		else if (strMSPath.IsEmpty() || strMSPath.Find(strVideoPath) == -1)
		{
			StreamMap.iMediaIdx = -1;
			for (int x = 0; x < g_MediaStreamAry.size(); x++)
			{
				if (g_MediaStreamAry.at(x)->FilePath().IsEmpty())
				{
					StreamMap.iMediaIdx = x;
					break;
				}
			}

			if (StreamMap.iMediaIdx == -1)
				continue;

			if (!StreamMap.bExtraVideo && strVideoPath.Find("!@#$%^") != -1)
				strVideoPath.Replace("!@#$%^", "");

			StreamMap.strVideoPath = strVideoPath;
		}

		g_StreamMappingAry.Add(StreamMap);

		if (g_PannelAry.GetSize() > 1)
			g_PannelAry[0]->ShowTempVideo(StreamMap.strObjPath, "", -1.0f);
		//g_PannelAry[dlgLoadObj.GetSelView()]->ShowTempVideo(strObjPath, strVideoPath);
		g_PannelAry[StreamMap.iPanelIdx]->ShowTempVideo(StreamMap.strObjPath, strVideoPath, -1.0f, StreamMap.bExtraVideo);

#ifdef _ENABLE_GPU
			Sleep(500);
#endif
	}
	
	INT32 iObjNum = g_PannelAry[g_PannelSetting.iSelIdx]->GetObjCount();
	//SetModalNum(iObjNum);
	SendMessageA(WM_SHAP_SET_MODAL_NUM, iObjNum, TRUE);
}


void CShapeUIDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (nIDEvent == 0)
	{
		EnterCriticalSection(&m_csFunBsy);
		//=========  Show Object Info ========//
		CFont *pOldFont;
		COLORREF colOldText;
		COLORREF colBkText;

		CString strTemp;

		CClientDC cdc(this);
		colOldText = cdc.SetTextColor(RGB(255, 255, 255));
		colBkText = cdc.SetBkColor(RGB(64, 64, 64));
		pOldFont = cdc.SelectObject(m_pNewFont);

		STREM_MAPPING strMapCur;
		if (0)
		{
			INT32 iPanelIdx = g_PannelSetting.iSelIdx;
			strTemp.Format("Porjector %i  (%ix%i)", iPanelIdx, 
				           g_DispArys.GetAt(iPanelIdx).Width(),
					       g_DispArys.GetAt(iPanelIdx).Height());
			cdc.TextOutA(1530 * m_fZoomX, 580 * m_fZoomY, strTemp);

			strMapCur = theApp.GetStreamMapByIdx(iPanelIdx, m_iSelObjIdx);
			
			INT32 iStrPos = 0;

			for (int x = 0; x < strMapCur.strObjPath.GetLength(); x++)
			{
				if (strMapCur.strObjPath.GetAt(strMapCur.strObjPath.GetLength() - x - 1) == '\\')
				{
					iStrPos = x;
					break;
				}
			} 

			strTemp.Format("Object:%i (%s)", m_iSelObjIdx, strMapCur.strObjPath.Right(iStrPos));
			cdc.TextOutA(1530 * m_fZoomX, 640 * m_fZoomY, strTemp);

			//cdc.TextOutA(1650 * m_fZoomX, 700 * m_fZoomY, g_MediaStreamAry.at(strMapCur.iMediaIdx)->FileName());
			m_sticVideoPath.GetWindowTextA(strTemp);
			if (strTemp.Trim() != g_MediaStreamAry.at(strMapCur.iMediaIdx)->FileName())
			{
				m_sticVideoPath.SetWindowTextA(g_MediaStreamAry.at(strMapCur.iMediaIdx)->FileName() +
						"                                                    ");

			}
			strTemp.Format("Media");
			cdc.TextOutA(1530 * m_fZoomX, 700 * m_fZoomY, strTemp);
			cdc.TextOutA(1678 * m_fZoomX, 760 * m_fZoomY, "Matrix");

			cdc.SelectObject(pOldFont);
			ShowObjMatrix(cdc, iPanelIdx, m_iSelObjIdx);
		}
		cdc.SetTextColor(colOldText);
		cdc.SetBkColor(colBkText);

		//=========  Check Notify Action ========//
		if (g_PannelSetting.iNotifyAction != 0)
		{
			CItem_Class* pItem = NULL;
			if (g_PannelSetting.iNotifyAction == MODEL_MODIFY_SELECT)
				pItem = (CItem_Class*)m_FramePic->GetItemByID(ITEM_PTMOVE_SELECT);
			else if (g_PannelSetting.iNotifyAction == MODEL_MODIFY_MOVE)
				pItem = (CItem_Class*)m_FramePic->GetItemByID(ITEM_PTMOVE_MOVE);

			if (pItem)
			{
				pItem->OnBnClicked();
				g_PannelSetting.iNotifyAction = 0;
			}
		}

		//=========  Check Selected Item ========//
		INT32 iIndex = 0;
		INT32 iTemp;


		//Select View
		iIndex = m_FramePic->GetSelectedItem(4);

		if (iIndex >= ITEM_VIEW1 && iIndex <= ITEM_VIEW8)
		{
			if (g_PannelSetting.iSelIdx != iIndex - ITEM_VIEW1 + 1)
			{
				//g_PannelAry[g_PannelSetting.iSelIdx]->SelectPannel(FALSE);
				g_PannelAry[g_PannelSetting.iSelIdx]->Invalidate();
				g_PannelSetting.iSelIdx = iIndex - ITEM_VIEW1 + 1;
				g_PannelAry[g_PannelSetting.iSelIdx]->Invalidate();
				//g_PannelAry[g_PannelSetting.iSelIdx]->SelectPannel(TRUE);
				INT32 iObjNum = g_PannelAry[g_PannelSetting.iSelIdx]->GetObjCount();
				SetModalNum(iObjNum);
			}
		}
		else if (g_PannelSetting.iSelIdx != 0)
		{
			g_PannelAry[g_PannelSetting.iSelIdx]->Invalidate();
			g_PannelSetting.iSelIdx = 0;
			SetModalNum(0);
			m_iSelObjIdx = -1;
		}
		
		if (g_PannelSetting.iSelIdx == 0 && m_iMaxView != 0)
		{
			LeaveCriticalSection(&m_csFunBsy);
			return;
		}

		//Object Action
		iIndex = m_FramePic->GetSelectedItem(1);

		if (iIndex == ITEM_MOVE)	iIndex = MODEL_MOVE;
		else if (iIndex == ITEM_ROTATE)	iIndex = MODEL_ROTATE;
		else if (iIndex == ITEM_ZOOM)	iIndex = MODEL_ZOOM;
		else if (iIndex == ITEM_MAP)	iIndex = MODEL_MAP;
		else if (iIndex == ITEM_PTMOVE)	iIndex = MODEL_MODIFY;
		else if (iIndex == ITEM_2DMAP)	g_PannelSetting.iModelAction = MODEL_2DMAPPING;
		else iIndex = 0;

		if (iIndex != ITEM_2DMAP)
		{
			g_PannelSetting.iModelAction = g_PannelSetting.iModelAction & (~MODEL_2DMAPPING);
			g_PannelSetting.iModelAction = g_PannelSetting.iModelAction & (~0xf) | iIndex;			
		}
		else //if (m_bHadInit2DMap)
		{
			g_PannelSetting.bNotShowModal = FALSE;
			g_PannelSetting.bHaveEndSurface = TRUE;
		}
	 			
		//Show Mode
		iIndex = m_FramePic->GetSelectedItem(2);

		if ((g_PannelSetting.iModelAction & MODEL_2DMAPPING) == 0)
		{
			g_PannelSetting.bNotShowModal = (iIndex == ITEM_VIDEO);
			g_PannelSetting.bHaveEndSurface = (iIndex == ITEM_VIDEO);
		}

		g_PannelSetting.iModelAction &= ~MODEL_SHOW_MODEL_LINE;

		if (iIndex == ITEM_LINE)
			g_PannelSetting.iModelAction |= MODEL_SHOW_MODEL_LINE;
		

		//Select Object
		iIndex = m_FramePic->GetSelectedItem(5);
		if (iIndex >= ITEM_OBJ1 && iIndex <= ITEM_OBJ10)
		{
			if (m_iSelObjIdx != iIndex - ITEM_OBJ1)
			{ 
				for (int x = 0; x < g_MediaStreamAry.size(); x++)
				{
					strTemp.Format("%i", x);
					iTemp = m_VideoIdxCombox.FindString(0, strTemp);

					if (!g_MediaStreamAry.at(x)->FileName().IsEmpty() && iTemp == -1)
					{
							m_VideoIdxCombox.AddString(strTemp);
					}
					else if (g_MediaStreamAry.at(x)->FileName().IsEmpty() && iTemp != -1)
					{ 						
							m_VideoIdxCombox.DeleteString(iTemp);
					}
				}
				m_iSelObjIdx = iIndex - ITEM_OBJ1;
				g_PannelAry[g_PannelSetting.iSelIdx]->SetObjIdx(m_iSelObjIdx);
				g_PannelAry[0]->SetObjIdx(m_iSelObjIdx);

				strMapCur = theApp.GetStreamMapByIdx(g_PannelSetting.iSelIdx, m_iSelObjIdx);

				strTemp.Format("%i", strMapCur.iMediaIdx);
				iTemp = m_VideoIdxCombox.FindString(0, strTemp);
				m_VideoIdxCombox.SetCurSel(iTemp);
			}
			else if (0)
			{
				strTemp.Format("%i", strMapCur.iMediaIdx);
				iTemp = m_VideoIdxCombox.FindString(0, strTemp);
				if (iTemp != m_VideoIdxCombox.GetCurSel())
				{
					m_VideoIdxCombox.GetLBText(m_VideoIdxCombox.GetCurSel(), strTemp);
					iTemp = atoi(strTemp);
					INT32 iObjCount = -1;
					for (int x = 0; x < g_StreamMappingAry.GetCount(); x++)
					{
						if (g_StreamMappingAry.GetAt(x).iPanelIdx == g_PannelSetting.iSelIdx)
						{
							iObjCount++;
						}

						if (iObjCount == m_iSelObjIdx)
						{
							g_StreamMappingAry.GetAt(x).iMediaIdx = iTemp;
							//SaveProject(theApp.m_strCurPath + "Mapping.xml");
							break;
						}
					}
				}
			}			
		}
		else
		{
			m_iSelObjIdx = -1;
			g_PannelAry[g_PannelSetting.iSelIdx]->SetObjIdx(m_iSelObjIdx);
			g_PannelAry[0]->SetObjIdx(m_iSelObjIdx);

		}

		//Zoom SubItem 
		if (g_PannelSetting.iModelAction & MODEL_ZOOM)
		{
			iIndex = m_FramePic->GetSelectedItem(7);
			if (iIndex == ITEM_ZOOM_ONE)
			{
				g_PannelSetting.iModelAction = g_PannelSetting.iModelAction & (~0xf) | MODEL_ZOOM_ONE;
			}
			else if (iIndex == ITEM_ZOOM_ALL)
			{
				g_PannelSetting.iModelAction = g_PannelSetting.iModelAction & (~0xf) | MODEL_ZOOM_ALL;
			}
		}
		
		//Modify SubItem 
		if (g_PannelSetting.iModelAction & MODEL_MODIFY)
		{
			iIndex = m_FramePic->GetSelectedItem(6);
			if (iIndex == ITEM_PTMOVE_SELECT)
			{
				g_PannelSetting.iModelAction = g_PannelSetting.iModelAction & (~0xf) | MODEL_MODIFY_SELECT;
			}
			else if (iIndex == ITEM_PTMOVE_MOVE)
			{
				g_PannelSetting.iModelAction = g_PannelSetting.iModelAction & (~0xf) | MODEL_MODIFY_MOVE;
			}
		}

		if ((g_PannelSetting.iModelAction &0xf) ==  MODEL_ROTATE)
		{
			iIndex = m_FramePic->GetSelectedItem(8);
			if (iIndex == ITEM_ROTATE_X)
			{
				g_PannelSetting.iModelAction = g_PannelSetting.iModelAction & (0x3FF) | MODEL_SUBITEM_1;
			}
			else if (iIndex == ITEM_ROTATE_Y)
			{
				g_PannelSetting.iModelAction = g_PannelSetting.iModelAction & (0x3FF) | MODEL_SUBITEM_2;
			}
			else if (iIndex == ITEM_ROTATE_Z)
			{
				g_PannelSetting.iModelAction = g_PannelSetting.iModelAction & (0x3FF) | MODEL_SUBITEM_3;
			}
			else if (iIndex == ITEM_ROTATE_ALL)
			{
				g_PannelSetting.iModelAction = g_PannelSetting.iModelAction & (0x3FF) | MODEL_SUBITEM_4;
			}
		}

		//Check Rotate Offset
		if ((g_PannelSetting.iModelAction & 0xf) == MODEL_ROTATE)
		{
			iIndex = m_FramePic->GetSelectedItem(15);

			if (iIndex == ITEM_ROTATE_ANGLE)
				g_PannelSetting.iModelAction |= MODEL_ROTATE_OFFSET;
			else
				g_PannelSetting.iModelAction &= ~MODEL_ROTATE_OFFSET;
		}
		else
			g_PannelSetting.iModelAction &= ~MODEL_ROTATE_OFFSET;

		static bool bShowBKPt = FALSE;
		iIndex = m_FramePic->GetSelectedItem(11);
		if (iIndex == ITEM_2DMAP_SHOWPT && !bShowBKPt)
		{
			//g_PannelAry[g_PannelSetting.iSelIdx]->Set2DMapAction(MAP2D_NEED_UPDATE_BK | MAP2D_SHOW_BK_PT);

			for (int x = 1; x < g_PannelAry.GetCount(); x++)
				g_PannelAry[x]->Set2DMapAction(MAP2D_NEED_UPDATE_BK | MAP2D_SHOW_BK_PT);

			bShowBKPt = !bShowBKPt;
		}
		else if (iIndex != ITEM_2DMAP_SHOWPT && bShowBKPt)
		{
			//g_PannelAry[g_PannelSetting.iSelIdx]->Set2DMapAction(MAP2D_NOACT);

			for (int x = 1; x < g_PannelAry.GetCount(); x++)
				g_PannelAry[x]->Set2DMapAction(MAP2D_NOACT);

			bShowBKPt = !bShowBKPt;
		}

		if (!theApp.m_bSampleVersion)
		{
			static BOOL bLockUI = FALSE;
			iIndex = m_FramePic->GetSelectedItem(12);
			if (iIndex == ITEM_LOCK_UI && !bLockUI)
			{
				INT32 iObjIdx = m_FramePic->GetSelectedItem(5);
				if (iObjIdx > 0)
				{
					CItem_Class* pItem = (CItem_Class*)m_FramePic->GetItemByID(iObjIdx);
					pItem->OnBnClicked();
				}
				m_FramePic->EnableItem(FALSE, ITEM_LOCK_UI);
				bLockUI = TRUE;
			}
			else if (iIndex != ITEM_LOCK_UI && bLockUI)
			{
				m_FramePic->EnableItem(TRUE);
				bLockUI = FALSE;
			}
		}


		ShowSubItem();
	}
	LeaveCriticalSection(&m_csFunBsy);
	CDialog::OnTimer(nIDEvent);
}

void CShapeUIDlg::ShowObjMatrix(CClientDC &cdc, INT32 iPanelIdx, INT32 iObjIdx, CString strProjectPath)
{
	CFileFind fFind;
	if (!fFind.FindFile(theApp.m_strCurPath + strProjectPath))
		return;

	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(theApp.m_strCurPath + strProjectPath);
	CString strTemp, strVal;

	auto pProject = xmlDoc.FirstChildElement("Project");
	auto pMatrixItem = pProject->FirstChildElement("Matrix");

	for (auto pNextMatrix = pMatrixItem; pNextMatrix != 0; pNextMatrix = pNextMatrix->NextSiblingElement())
	{
		//pNextMatrix
		if (atoi(pNextMatrix->Attribute("Id")) != iObjIdx)
			continue;

		auto rowItem = pNextMatrix->FirstChildElement("Row");
		cdc.TextOutA(1560 * m_fZoomX, 820 * m_fZoomY, rowItem->Attribute("Cell11"));
		cdc.TextOutA(1640 * m_fZoomX, 820 * m_fZoomY, rowItem->Attribute("Cell12"));
		cdc.TextOutA(1720 * m_fZoomX, 820 * m_fZoomY, rowItem->Attribute("Cell13"));
		cdc.TextOutA(1800 * m_fZoomX, 820 * m_fZoomY, rowItem->Attribute("Cell14"));

		rowItem = rowItem->NextSiblingElement();
		cdc.TextOutA(1560 * m_fZoomX, 880 * m_fZoomY, rowItem->Attribute("Cell21"));
		cdc.TextOutA(1640 * m_fZoomX, 880 * m_fZoomY, rowItem->Attribute("Cell22"));
		cdc.TextOutA(1720 * m_fZoomX, 880 * m_fZoomY, rowItem->Attribute("Cell23"));
		cdc.TextOutA(1800 * m_fZoomX, 880 * m_fZoomY, rowItem->Attribute("Cell24"));

		rowItem = rowItem->NextSiblingElement();
		cdc.TextOutA(1560 * m_fZoomX, 940 * m_fZoomY, rowItem->Attribute("Cell31"));
		cdc.TextOutA(1640 * m_fZoomX, 940 * m_fZoomY, rowItem->Attribute("Cell32"));
		cdc.TextOutA(1720 * m_fZoomX, 940 * m_fZoomY, rowItem->Attribute("Cell33"));
		cdc.TextOutA(1800 * m_fZoomX, 940 * m_fZoomY, rowItem->Attribute("Cell34"));

		rowItem = rowItem->NextSiblingElement();
		cdc.TextOutA(1560 * m_fZoomX, 1000 * m_fZoomY, rowItem->Attribute("Cell41"));
		cdc.TextOutA(1640 * m_fZoomX, 1000 * m_fZoomY, rowItem->Attribute("Cell42"));
		cdc.TextOutA(1720 * m_fZoomX, 1000 * m_fZoomY, rowItem->Attribute("Cell43"));
		cdc.TextOutA(1800 * m_fZoomX, 1000 * m_fZoomY, rowItem->Attribute("Cell44"));
	}
}

HBRUSH CShapeUIDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(RGB(64, 64, 64));
		pDC->SelectObject(m_pNewFont);
		pDC->SetTextColor(RGB(255, 255, 255));
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	else
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此變更 DC 的任何屬性

	// TODO:  如果預設值並非想要的，則傳回不同的筆刷
}


void CShapeUIDlg::OnOK()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	return;
	CDialog::OnOK();
}


void CShapeUIDlg::OnCancel()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別

	((CDialog*)GetParent())->PostMessageA(IDCANCEL);
	return;

	CDialog::OnCancel();
}

void CShapeUIDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_pUTipDll)
		delete m_pUTipDll;
}

void CShapeUIDlg::CreateDefultGama()
{
	g_PannelAry[0]->ReleaseFocus(TRUE);
	m_FramePic->OnEnable(FALSE);
	m_dlgSetGrid.CreateDefultGama();
}

LRESULT CShapeUIDlg::OnSetModalNum(WPARAM wParam, LPARAM lParam)
{
	SetModalNum(wParam, lParam);//TRUE
	return true;
}

void CShapeUIDlg::EnableObjButtons(bool bEnable)
{
	int i;
	CUI_Class* pCurItem = m_FramePic->GetItemByID(ITEM_LOADOBJ);
	pCurItem->OnEnable(bEnable);

	pCurItem = m_FramePic->GetItemByID(ITEM_CUBE);
	pCurItem->OnEnable(bEnable);

	pCurItem = m_FramePic->GetItemByID(ITEM_BALL);
	pCurItem->OnEnable(bEnable);

	pCurItem = m_FramePic->GetItemByID(ITEM_4PYRAMID);
	pCurItem->OnEnable(bEnable);

	pCurItem = m_FramePic->GetItemByID(ITEM_FIG);
	pCurItem->OnEnable(bEnable);

	pCurItem = m_FramePic->GetItemByID(ITEM_FLAT);
	pCurItem->OnEnable(bEnable);

	pCurItem = m_FramePic->GetItemByID(ITEM_USERDEFINE);
	pCurItem->OnEnable(bEnable);

	pCurItem = m_FramePic->GetItemByID(ITEM_NEW);
	pCurItem->OnEnable(bEnable);

	pCurItem = m_FramePic->GetItemByID(ITEM_OPEN);
	pCurItem->OnEnable(bEnable);

	pCurItem = m_FramePic->GetItemByID(ITEM_SAVE);
	pCurItem->OnEnable(bEnable);

	for (i = 0; i < 8; i++)
	{
		pCurItem = m_FramePic->GetItemByID(ITEM_VIEW1 + i);
		if(pCurItem)
			pCurItem->OnEnable(bEnable);
	}

	for (i = 0; i < 10; i++)
	{
		pCurItem = m_FramePic->GetItemByID(ITEM_OBJ1 + i);
		if (pCurItem)
			pCurItem->OnEnable(bEnable);
	}
}

void CShapeUIDlg::OpenProject()
{
	CString strTemp;

	if (NewProject(TRUE))
	{
#ifdef _ENABLE_SPF
		m_FramePic->OnEnable(FALSE);

		TCHAR szFilters[] = _T("SPF Project Files (*.spf)|*.spf|XML Project Files (*.xml)|*.xml|All Files (*.*)|*.*||");
		CFileDialog fileDlg(TRUE, _T("spf"), _T("*.spf"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters,this);

		if (fileDlg.DoModal() == IDOK)
		{
			//OpenProject2(fileDlg.GetPathName());

//			ShapeThreadItem thread_item;
			thread_item.pObj = this;
			thread_item.szProjectFileName = fileDlg.GetPathName();
			unsigned threadID1;
			HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, _LoadProject_ThreadProc, &thread_item, 0, &threadID1);


#if 0
			if(IsSPF(fileDlg.GetPathName().GetBuffer()))
				UnZipProjectFile(fileDlg.GetPathName().GetBuffer());
			else
			{
				CPlaneScene3D::FileDecoder(fileDlg.GetPathName());

				CFile sfSrc;
				CStdioFile sfDest;
				char cData[1] = "";
				BOOL bOpen = FALSE;
				CFileFind ffFinder;
				BOOL bError = FALSE;

				sfSrc.Open(fileDlg.GetPathName(), CFile::modeRead);
				strTemp = "";
				while (sfSrc.GetPosition() < sfSrc.GetLength())
				{
					sfSrc.Read(cData, 1);
					if (cData[0] != '\n')
						strTemp += cData[0];
					else
					{
						if (strTemp.Find("=== MS File Path:") != -1)
						{
							strTemp.Replace("=== MS File Path:", "");
							strTemp.Trim();

							if (bOpen)
								sfDest.Close();

							/*if (!ffFinder.FindFile(strTemp))
							{
							bError = TRUE;
							bOpen = FALSE;
							break;
							}*/

							if (strTemp.Right(11) == "Mapping.xml")
								strTemp = theApp.m_strCurPath + "Mapping.xml";
							else if (strTemp.Right(9) == "Shape.xml")
								strTemp = theApp.m_strCurPath + "Shape.xml";
							else if (strTemp.Find("\\Obj\\MS_Shape") != -1)
							{
								INT32 iFindPos = strTemp.Find("\\Obj\\MS_Shape");
								strTemp = strTemp.Right(strTemp.GetLength() - iFindPos);
								strTemp = theApp.m_strCurPath + strTemp;

							}
							else
							{
								bError = TRUE;
								bOpen = FALSE;
								break;
							}

							sfDest.Open(strTemp, CFile::modeCreate | CFile::modeReadWrite);
							bOpen = TRUE;
						}
						else
						{
							if (bOpen)
							{
								if (strTemp.Find("\\Obj\\MS_Shape") != -1)
								{
									INT32 iFindPos1 = strTemp.Find("Path=");
									INT32 iFindPos2 = strTemp.Find("\\Obj\\MS_Shape");

									strTemp.Format("%sPath=\"%s", strTemp.Left(iFindPos1),
										theApp.m_strCurPath +
										strTemp.Right(strTemp.GetLength() - iFindPos2 - 1));
								}
								sfDest.WriteString(strTemp + "\n");
							}
						}

						strTemp = "";
					}
				}

				sfSrc.Close();

				if (bOpen)
					sfDest.Close();

				if (!bOpen || bError)
				{
					CPlaneScene3D::FileEncoder(fileDlg.GetPathName());
					m_FramePic->OnEnable(TRUE);
					return;// break;
				}

				CPlaneScene3D::FileEncoder(fileDlg.GetPathName());
			}

			LoadProject("");
#endif

		}

		m_FramePic->OnEnable(TRUE);
#else
		m_FramePic->OnEnable(FALSE);
		TCHAR szFilters[] = _T("Project Files (*.xml)|*.xml|All Files (*.*)|*.*||");
		CFileDialog fileDlg(TRUE, _T("xml"), _T("*.xml"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

		if (fileDlg.DoModal() == IDOK)
		{
			CPlaneScene3D::FileDecoder(fileDlg.GetPathName());

			if (1)
			{
				CFile sfSrc;
				CStdioFile sfDest;
				char cData[1] = "";
				BOOL bOpen = FALSE;
				CFileFind ffFinder;
				BOOL bError = FALSE;

				sfSrc.Open(fileDlg.GetPathName(), CFile::modeRead);
				strTemp = "";
				while (sfSrc.GetPosition() < sfSrc.GetLength())
				{
					sfSrc.Read(cData, 1);
					if (cData[0] != '\n')
						strTemp += cData[0];
					else
					{
						if (strTemp.Find("=== MS File Path:") != -1)
						{
							strTemp.Replace("=== MS File Path:", "");
							strTemp.Trim();

							if (bOpen)
								sfDest.Close();

							/*if (!ffFinder.FindFile(strTemp))
							{
							bError = TRUE;
							bOpen = FALSE;
							break;
							}*/

							if (strTemp.Right(11) == "Mapping.xml")
								strTemp = theApp.m_strCurPath + "Mapping.xml";
							else if (strTemp.Right(9) == "Shape.xml")
								strTemp = theApp.m_strCurPath + "Shape.xml";
							else if (strTemp.Find("\\Obj\\MS_Shape") != -1)
							{
								INT32 iFindPos = strTemp.Find("\\Obj\\MS_Shape");
								strTemp = strTemp.Right(strTemp.GetLength() - iFindPos);
								strTemp = theApp.m_strCurPath + strTemp;

							}
							else
							{
								bError = TRUE;
								bOpen = FALSE;
								break;
							}

							sfDest.Open(strTemp, CFile::modeCreate | CFile::modeReadWrite);
							bOpen = TRUE;
						}
						else
						{
							if (bOpen)
							{
								if (strTemp.Find("\\Obj\\MS_Shape") != -1)
								{
									INT32 iFindPos1 = strTemp.Find("Path=");
									INT32 iFindPos2 = strTemp.Find("\\Obj\\MS_Shape");

									strTemp.Format("%sPath=\"%s", strTemp.Left(iFindPos1),
										theApp.m_strCurPath +
										strTemp.Right(strTemp.GetLength() - iFindPos2 - 1));
								}
								sfDest.WriteString(strTemp + "\n");
							}
						}

						strTemp = "";
					}
				}

				sfSrc.Close();

				if (bOpen)
					sfDest.Close();

				if (!bOpen || bError)
				{
					CPlaneScene3D::FileEncoder(fileDlg.GetPathName());
					m_FramePic->OnEnable(TRUE);
					return;// break;
				}

			}
			else
			{
				CopyFile(fileDlg.GetPathName(), theApp.m_strCurPath + "Mapping.xml", FALSE);
				CopyFile(fileDlg.GetPathName() + "shp", theApp.m_strCurPath + "Shape.xml", FALSE);
				CString strOrgObjPath;
				CFileFind fFind;
				INT32 iIndex;
				BOOL bWork = fFind.FindFile(fileDlg.GetPathName() + "Obj*");
				m_iCurTempObjIdx = 0;

				while (bWork)
				{
					bWork = fFind.FindNextFile();
					strOrgObjPath = fFind.GetFilePath();
					strTemp = strOrgObjPath.Right(3);
					strTemp.Replace("b", "");
					strTemp.Replace("j", "");
					iIndex = atoi(strTemp);

					strTemp.Format("%sMS_Shape%i.obj", theApp.m_strCurPath + "Obj\\", iIndex);
					CopyFile(strOrgObjPath, strTemp, FALSE);

					if (m_iCurTempObjIdx < iIndex)
						m_iCurTempObjIdx = iIndex;
				}
			}

			CPlaneScene3D::FileEncoder(fileDlg.GetPathName());
			LoadProject("");
		}
		m_FramePic->OnEnable(TRUE);
#endif
	}
}

void CShapeUIDlg::OpenProject2(CString strProjectPath)
{
	CString strTemp;

	if (IsSPF(strProjectPath.GetBuffer()))
		UnZipProjectFile(strProjectPath.GetBuffer());
	else
	{
		CPlaneScene3D::FileDecoder(strProjectPath);

		CFile sfSrc;
		CStdioFile sfDest;
		char cData[1] = "";
		BOOL bOpen = FALSE;
		CFileFind ffFinder;
		BOOL bError = FALSE;

		sfSrc.Open(strProjectPath.GetBuffer(), CFile::modeRead);
		strTemp = "";
		while (sfSrc.GetPosition() < sfSrc.GetLength())
		{
			sfSrc.Read(cData, 1);
			if (cData[0] != '\n')
				strTemp += cData[0];
			else
			{
				if (strTemp.Find("=== MS File Path:") != -1)
				{
					strTemp.Replace("=== MS File Path:", "");
					strTemp.Trim();

					if (bOpen)
						sfDest.Close();

					/*if (!ffFinder.FindFile(strTemp))
					{
					bError = TRUE;
					bOpen = FALSE;
					break;
					}*/

					if (strTemp.Right(11) == "Mapping.xml")
						strTemp = theApp.m_strCurPath + "Mapping.xml";
					else if (strTemp.Right(9) == "Shape.xml")
						strTemp = theApp.m_strCurPath + "Shape.xml";
					else if (strTemp.Find("\\Obj\\MS_Shape") != -1)
					{
						INT32 iFindPos = strTemp.Find("\\Obj\\MS_Shape");
						strTemp = strTemp.Right(strTemp.GetLength() - iFindPos);
						strTemp = theApp.m_strCurPath + strTemp;

					}
					else
					{
						bError = TRUE;
						bOpen = FALSE;
						break;
					}

					sfDest.Open(strTemp, CFile::modeCreate | CFile::modeReadWrite);
					bOpen = TRUE;
				}
				else
				{
					if (bOpen)
					{
						if (strTemp.Find("\\Obj\\MS_Shape") != -1)
						{
							INT32 iFindPos1 = strTemp.Find("Path=");
							INT32 iFindPos2 = strTemp.Find("\\Obj\\MS_Shape");

							strTemp.Format("%sPath=\"%s", strTemp.Left(iFindPos1),
								theApp.m_strCurPath +
								strTemp.Right(strTemp.GetLength() - iFindPos2 - 1));
						}
						sfDest.WriteString(strTemp + "\n");
					}
				}

				strTemp = "";
			}
		}

		sfSrc.Close();

		if (bOpen)
			sfDest.Close();

		if (!bOpen || bError)
		{
			CPlaneScene3D::FileEncoder(strProjectPath.GetBuffer());
			m_FramePic->OnEnable(TRUE);
			return;// break;
		}

		CPlaneScene3D::FileEncoder(strProjectPath.GetBuffer());
	}

	LoadProject("");
}

void CShapeUIDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_pUTipDll->_UT_ShowTip(m_pUTipDll->GetUTObj(), this->GetSafeHwnd(), point.x, point.y);

	CDialog::OnMouseMove(nFlags, point);
}

void CShapeUIDlg::AddTip(wchar_t* wszTipID, int left, int top, int right, int bottom)
{
	wchar_t wszID[512];
	wchar_t wszData[512];
	wchar_t wszAppName[128];
	wchar_t wszKeyName[128];
	char szIniFile[512];
	wchar_t wszIniFile[512];
	sprintf(szIniFile, "%s\\Language\\shape_%s.ini", theApp.m_strCurPath, theApp.m_strLanguage);

	wcscpy(wszIniFile, ANSIToUnicode(szIniFile));

	int iTotal = GetPrivateProfileInt("Base", "Total", 0, szIniFile);
	for (int i = 0; i < iTotal; i++)
	{
		swprintf(wszAppName, L"Item%d", i);
		GetPrivateProfileStringW(wszAppName, L"ID", L"", wszID, 512, wszIniFile);

		if (wcscmp(wszID, wszTipID) == 0)
		{
			GetPrivateProfileStringW(wszAppName, L"Str", L"", wszData, 512, wszIniFile);

			TipInfoW tip_info;
			tip_info.hWnd = this->GetSafeHwnd();
			wcscpy(tip_info.wszMsg, wszData);
			tip_info.rect.left = left;
			tip_info.rect.top = top;
			tip_info.rect.right = right;
			tip_info.rect.bottom = bottom;

			m_pUTipDll->_UT_Add(m_pUTipDll->GetUTObj(), tip_info.hWnd, tip_info.rect, tip_info.wszMsg, tip_info.ulTipID);

			break;
		}
	}
}

void CShapeUIDlg::SetTip()
{
	m_pUTipDll->_UT_Reset(m_pUTipDll->GetUTObj());

	AddTip(L"Move", 4 + 6, 160 + 6, 60, 60);
	AddTip(L"Rotate", 4 + 6, 237 + 6, 60, 60);
	AddTip(L"Zoom", 4 + 6, 314 + 6, 60, 60);
	AddTip(L"Select", 4 + 6, 398 + 6, 60, 60);
	AddTip(L"Play Video", 4 + 6, 476 + 6, 60, 60);
	AddTip(L"WireFrame", 4 + 6, 554 + 6, 60, 60);
	AddTip(L"UnDo", 4 + 6, 632 + 6, 60, 60);
	AddTip(L"ReDo", 4 + 6, 710 + 6, 60, 60);
	AddTip(L"Delete", 4 + 6, 788 + 6, 60, 60);

	AddTip(L"New Project", 205 + 6, 1 + 6, 60, 60);
	AddTip(L"Load Project", 282 + 6, 1 + 6, 60, 60);
	AddTip(L"Save Project", 359 + 6, 1 + 6, 60, 60);
	AddTip(L"Load Obj", 474 + 6, 1 + 6, 60, 60);
	AddTip(L"Load Cube", 549 + 6, 1 + 6, 60, 60);
	AddTip(L"Load Ball", 624 + 6, 1 + 6, 60, 60);
	AddTip(L"Load Pyramid", 699 + 6, 1 + 6, 60, 60);
	AddTip(L"Load Fig", 774 + 6, 1 + 6, 60, 60);
	AddTip(L"Load Flat", 849 + 6, 1 + 6, 60, 60);
	AddTip(L"Load Disc", 924 + 6, 1 + 6, 60, 60);
	AddTip(L"Load Cone", 999 + 6, 1 + 6, 60, 60);
	AddTip(L"Load Triangle", 1074 + 6, 1 + 6, 60, 60);
	AddTip(L"Load Bear", 1149 + 6, 1 + 6, 60, 60);
	AddTip(L"Load UserDefine", 1224 + 6, 1 + 6, 60, 60);
	AddTip(L"Lock", 1430 + 6, 1 + 6, 60, 60);

	wchar_t wszID[64];
	int i;
	for (i = 0; i < 4; i++)
	{
		swprintf(wszID, L"Panel%d", i + 1);
		AddTip(wszID, 1532 + 6 + i * 75, 2 + 6, 60, 60);
	}

	for (i = 0; i < 10; i++)
	{
		swprintf(wszID, L"Obj%d", i + 1);
		AddTip(wszID, 1842 + 6, 85 + 6 + i * 77, 60, 60);
	}
}

#ifdef _LOAD_OBJ_THREAD_MODE

void CShapeUIDlg::LoadObj4Thread(UINT message)
{
	m_CurMessage = message;
	unsigned threadID1;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, _LoadObj_ThreadProc, this, 0, &threadID1);
	//LoadObj();
}

void CShapeUIDlg::LoadObj()
{
	CLoadObjVdo dlgLoadObj(this);
	dlgLoadObj.SetVideoNum(g_PannelAry.GetSize() - 1);

	if (m_CurMessage == ITEM_LOADOBJ)
		dlgLoadObj.SetDefautlStatus(1);
	else
	{
		if (m_CurMessage == ITEM_USERDEFINE)
			dlgLoadObj.SetDefautlStatus(2);

		dlgLoadObj.LockLoadObj();
	}

	m_FramePic->OnEnable(FALSE);
	GetParent()->EnableWindow(false);
	if (dlgLoadObj.DoModal() == IDOK)
	{
		try
		{
			CString strObjPath = dlgLoadObj.GetObjPath();
			CString strVideoPath = dlgLoadObj.GetVideoPath();
			DOUBLE dZoom = dlgLoadObj.GetZoolRatio();
			BOOL bExtraVideo = dlgLoadObj.IsExtraVideo();

			if (m_CurMessage == ITEM_BEAR)
			{
				//strObjPath.Format("%sbearhead.obj", theApp.m_strCurPath + "Obj\\");
				strObjPath.Format("%sBearHead-Final.obj", theApp.m_strCurPath + "Obj\\");
			}
			else if (m_CurMessage == ITEM_CONE)
			{
				strObjPath.Format("%scone.obj", theApp.m_strCurPath + "Obj\\");
			}
			else if (m_CurMessage == ITEM_DISC)
			{
				strObjPath.Format("%sdisc.obj", theApp.m_strCurPath + "Obj\\");
			}
			else if(m_CurMessage == ITEM_FROMMIX)
				strObjPath.Format("%striangle.obj", theApp.m_strCurPath + "Obj\\");
			else if (m_CurMessage != ITEM_LOADOBJ && m_CurMessage != ITEM_USERDEFINE)
			{
				strObjPath.Format("%sOBJ%i.obj", theApp.m_strCurPath + "Obj\\", m_CurMessage - ITEM_BALL + 1);
			}

			if (strObjPath.IsEmpty() || strVideoPath.IsEmpty())
			{
				m_FramePic->OnEnable(TRUE);
				GetParent()->EnableWindow(true);
				GetParent()->SetFocus();
				return;//break;
			}

			bool bCheckRet = CheckObjFile(strObjPath);
			if (!bCheckRet)
			{
				MessageBoxA("LoadObj() Failed!!", "", MB_OK | MB_TOPMOST);

				m_FramePic->OnEnable(TRUE);
				GetParent()->EnableWindow(true);
				GetParent()->SetFocus();

				return;
			}

			CString strNewPath;
			strNewPath.Format("%sMS_Shape%i.obj", theApp.m_strCurPath + "Obj\\", ++m_iCurTempObjIdx);
			BOOL bRet = CopyFile(strObjPath, strNewPath, FALSE);
			if (bRet)
			{
				strObjPath = strNewPath;

				STREM_MAPPING StreamMap;
				StreamMap.strObjPath = strObjPath;
				StreamMap.strVideoPath = strVideoPath;
				StreamMap.bExtraVideo = bExtraVideo;

				if (strVideoPath.Left(6).Find("Video ") != -1)
				{
					StreamMap.iMediaIdx = atoi(strVideoPath.Right(strVideoPath.GetLength() - 6)) - 1;
					//StreamMap.iMediaIdx += DEF_GLOBAL_MEDIA;
					strVideoPath = "";
					StreamMap.iWidth = g_MediaStreamAry.at(StreamMap.iMediaIdx)->GetVideoWidth();
					StreamMap.iHeight = g_MediaStreamAry.at(StreamMap.iMediaIdx)->GetVideoHeight();
				}
				else
				{
					StreamMap.iMediaIdx = g_MediaStreamAry.size();

					for (int y = 0; y < g_MediaStreamAry.size(); y++)
					{
						if (g_MediaStreamAry.at(y)->FilePath().IsEmpty())
						{
							StreamMap.iMediaIdx = y;
							break;
						}
					}
				}

				StreamMap.iPanelIdx = g_PannelSetting.iSelIdx;
				StreamMap.iObjIdx = g_PannelAry[g_PannelSetting.iSelIdx]->GetObjCount();
				g_StreamMappingAry.Add(StreamMap);
				if (g_PannelAry.GetSize() > 1)
					g_PannelAry[0]->ShowTempVideo(strObjPath, "", dZoom, FALSE, true);
				//g_PannelAry[dlgLoadObj.GetSelView()]->ShowTempVideo(strObjPath, strVideoPath);
				g_PannelAry[g_PannelSetting.iSelIdx]->ShowTempVideo(strObjPath, strVideoPath, dZoom, bExtraVideo,true);

				if (0)//strVideoPath.IsEmpty())
				{
					CString strTemp = g_MediaStreamAry.at(StreamMap.iMediaIdx)->FilePath();
					g_StreamMappingAry.GetAt(g_StreamMappingAry.GetCount() - 1).strVideoPath = strTemp;
				}

				INT32 iObjNum = g_PannelAry[g_PannelSetting.iSelIdx]->GetObjCount();
				//SetModalNum(iObjNum, TRUE);
				SendMessageA(WM_SHAP_SET_MODAL_NUM, iObjNum, TRUE);
				theApp.SaveShapeProject(theApp.m_strCurPath + "Mapping.xml");

				if (abs(dZoom - 1.0f) > 0.01f)
				{
					g_PannelAry[0]->SaveObjectSetting();
				}
			}
			else
				MessageBoxA("CopyFile() Failed!!", "", MB_OK| MB_TOPMOST);
		}
		catch (...)
		{
			OutputDebugString("HEError - Catching an exception in LoadObj !! \n");
			MessageBoxA("LoadObj() Failed!!", "", MB_OK| MB_TOPMOST);
		}
	}
	m_FramePic->OnEnable(TRUE);
	GetParent()->EnableWindow(true);
	GetParent()->SetFocus();

	if (m_iObjNum > 0 && m_bPositionMode == true)
		EnableObjButtons(false);
}

bool CShapeUIDlg::CheckObjFile(CString strObjPath)
{
	SFile file;
	int iBufLen = 1024;
	bool bDo = true;
	int iUVCount = 0;
	char* pLineBuffer = new char[iBufLen];

	bool bRet = file.Open(strObjPath.GetString(), "rt");

	if (bRet)
	{
		while (bDo)
		{
			int iReadLen = file.GetNextLine(pLineBuffer, iBufLen);
			if (iReadLen > 0)
			{
				char* pRet = strstr(pLineBuffer, "vt ");
				if (pRet != NULL)
					iUVCount++;
			}
			else
				bDo = false;
		}
	}

	file.Close();

	if (pLineBuffer)
		delete pLineBuffer;

	if (iUVCount > 0)
		return true;
	return false;
}

#ifdef _ENABLE_SPF
void CShapeUIDlg::UnZipProjectFile(char* szFileName)
{
	if (m_pZLibDllObj)
	{
		m_pZLibDllObj->OpenUnZip(szFileName);
		m_pZLibDllObj->Extract(theApp.m_strCurPath.GetBuffer());
		m_pZLibDllObj->CloseUnZip();
	}
}

bool CShapeUIDlg::IsSPF(char* szFileName)
{
	char szTmp[64];
	char* pch = strrchr(szFileName,'.');
	strcpy(szTmp, strupr(pch));
	char* pch1 = strstr(szTmp, "SPF");
	if (pch1)
		return true;
	return false;
}
#endif

unsigned int __stdcall _LoadObj_ThreadProc(void* lpvThreadParm)
{
	CShapeUIDlg* pObj = (CShapeUIDlg *)lpvThreadParm;
	pObj->LoadObj();
	return 0;
}

unsigned int __stdcall _LoadProject_ThreadProc(void* lpvThreadParm)
{
	ShapeThreadItem* pItem = (ShapeThreadItem*)lpvThreadParm;
	((CShapeUIDlg*)pItem->pObj)->OpenProject2(pItem->szProjectFileName);
	return 0;
}
#endif
