// Mixer2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "MediaServer.h"
#include "Mixer2Dlg.h"
#include "afxdialogex.h"
#include "../../../Include/StrConv.h"

//Component iID
#define IDC_XEDIT_ID 		 1007
#define IDC_YEDIT_ID 		 1008
#define IDC_EDIT_WIDTH_ID  1009
#define IDC_EDIT_HEIGHT_ID 1010

#define IDC_COMBO_ID 		 9000
#define IDC_STATIC_LEVEL_ID  9005
#define IDC_STATIC_PARAM1_ID 9006
#define IDC_STATIC_PARAM2_ID 9007
#define IDC_STATIC_PARAM3_ID 9008
#define IDC_STATIC_PARAM4_ID 9009
#define IDC_STATIC_PARAM5_ID 9010


//Effect UI location
#define LEVEL_SLIDER_X_BEGIN 1070
#define LEVEL_SLIDER_X_END 1354
#define PARAM_SLIDER_X_BEGIN 1080
#define PARAM_SLIDER_X_END 1349

#define LEVEL_SLIDER_Y 590
#define PARAM1_SLIDER_Y 645
#define PARAM2_SLIDER_Y 702
#define PARAM3_SLIDER_Y 762
#define PARAM4_SLIDER_Y 822
#define PARAM5_SLIDER_Y 881

#define LEVEL_SLIDER_LEN (LEVEL_SLIDER_X_END-LEVEL_SLIDER_X_BEGIN)
#define PARAM_SLIDER_LEN (PARAM_SLIDER_X_END-PARAM_SLIDER_X_BEGIN)

#define LEVEL_PERCENT float(((CMoveable_Item*)(m_FramePic->GetItemByID(LEVEL_SLIDER)))->GetRelativePos().x)/LEVEL_SLIDER_LEN
#define EFFEC_PERCENT(ID) float(((CMoveable_Item*)(m_FramePic->GetItemByID(ID)))->GetRelativePos().x)/PARAM_SLIDER_LEN

//reset for Level, Param Maximum Setting Value(Capcity), it vary for future program set
#define MAX_LEVEL_VAL      100
#define MAX_PARAM_VAL      100

CArray<PARM_NAME, PARM_NAME> g_EffectParmNameAry;
LANGUAGE_TYPE g_iCurLang = ENGLISH_TYE;

// CMixer2Dlg dialog

IMPLEMENT_DYNAMIC(CMixer2Dlg, CDialogEx)

void CMixer2Dlg::DrawActivePnt(int previewID)
{
	CPen GreenPen(PS_SOLID, 3, RGB(0, 255, 0));

	// for remove previous green frame {
	if (m_iImageID != -1)
	{
		DrawRect(m_pMxPrw[m_iImageID]->m_PicCtrl.m_hWnd, m_cdc->m_hDC, GreenPen.operator HPEN());

		// for remove previous zoom icon if painted while change active window in mixers
		if (m_pMxPrw[m_iImageID]->m_bZmPaintBit) //m_iImageID now still remain previous active window (old) index
		{
			m_pMxPrw[m_iImageID]->m_ZoomLTpnt = m_pMxPrw[m_iImageID]->m_PreZmRectPos.TopLeft();
			m_pMxPrw[m_iImageID]->m_ZoomRBpnt = m_pMxPrw[m_iImageID]->m_PreZmRectPos.BottomRight();
			DrawZoomRect();
		}
	}
	m_iImageID = previewID;
	// for remove previous green frame }

	DrawRect(m_pMxPrw[m_iImageID]->m_PicCtrl.m_hWnd, m_cdc->m_hDC, GreenPen.operator HPEN());

	if (m_pMxPrw[m_iImageID]->StreamExist())
	{
		m_sWidth.Format("%d", m_pMxPrw[m_iImageID]->MixerStream().GetVideoWidth());
		m_sHeight.Format("%d", m_pMxPrw[m_iImageID]->MixerStream().GetVideoHeight());

		//m_EffectCombox.EnableWindow(TRUE);
		if (!m_pMxPrw[m_iImageID]->m_bZmPaintBit) DrawZoomRect();

		UpdateData(FALSE);
	}
	else
	{
		m_sWidth.Format("%s", "");
		m_sHeight.Format("%s", "");
		UpdateData(FALSE);

		//m_EffectCombox.EnableWindow(FALSE);
		m_pX_EditCtrl->SetWindowText("");
		m_pY_EditCtrl->SetWindowText("");

		SetFocus();
	}
	
}

void CMixer2Dlg::DrawRect(HWND hWnd, HDC hDC, HPEN hPen)
{
	RECT rect;
	::GetWindowRect(hWnd, &rect);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	MoveToEx(hDC, rect.left, rect.top, NULL);
	LineTo(hDC, rect.right - 1, rect.top);
	LineTo(hDC, rect.right - 1, rect.bottom - 1);
	LineTo(hDC, rect.left, rect.bottom - 1);
	LineTo(hDC, rect.left, rect.top);

	SelectObject(hDC, hOldPen);
}

void CMixer2Dlg::DrawZoomRect()
{
	if (m_iImageID != -1 && m_pMxPrw[m_iImageID]->StreamExist())
	{
		COLORREF penColor;

		if (m_pMxPrw[m_iImageID]->m_fZoomRate < 1.f)		 penColor = RGB(255, 0, 0);
		else if (m_pMxPrw[m_iImageID]->m_fZoomRate == 1.f)   penColor = RGB(0, 255, 0);
		else                                                 penColor = RGB(0, 0, 255);

		CPen myPen(PS_DASH | PS_ENDCAP_SQUARE, 2, penColor), *oldPen;

		oldPen = m_cdc->SelectObject(&myPen);

		//--------------------Rebuild four point of paint rectangle----------------------

		CPoint paintLTpnt, paintRBpnt;

		if (m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x >= m_ZoomOriLTpnt.x &&
			m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x <= m_ZoomOriRBpnt.x)
		{
			paintLTpnt.x = m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x;
		}
		else if (m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x < m_ZoomOriLTpnt.x ||
			m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x > m_ZoomOriRBpnt.x)
		{
			paintLTpnt.SetPoint(0, 0);
			paintRBpnt.SetPoint(0, 0);
		}
		else
		{
			paintLTpnt.x = m_ZoomOriLTpnt.x;
		}

		if (paintLTpnt != CPoint(0, 0) &&
			!(m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y < m_ZoomOriLTpnt.y) &&
			!(m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y > m_ZoomOriRBpnt.y))
		{
			if (m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y >= m_ZoomOriLTpnt.y &&
				m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y <= m_ZoomOriRBpnt.y)
			{
				paintLTpnt.y = m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y;
			}
			else if (m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y < m_ZoomOriLTpnt.y)
			{
				paintLTpnt.y = m_ZoomOriLTpnt.y;
			}
		}

		if (paintLTpnt != CPoint(0, 0))
		{
			if (m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x >= m_ZoomOriLTpnt.x &&
				m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x <= m_ZoomOriRBpnt.x)
			{
				paintRBpnt.x = m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x;
			}
			else if (m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x > m_ZoomOriRBpnt.x)
			{
				paintRBpnt.x = m_ZoomOriRBpnt.x;
			}

			if (m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y >= m_ZoomOriLTpnt.y &&
				m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y <= m_ZoomOriRBpnt.y)
			{
				paintRBpnt.y = m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y;
			}
			else if (m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y > m_ZoomOriRBpnt.y)
			{
				paintRBpnt.y = m_ZoomOriRBpnt.y;
			}
		}

		//--------------------paint edge of rectangle or not----------------------

		if (paintLTpnt != CPoint(0, 0))
		{
			if (m_pMxPrw[m_iImageID]->m_fZoomRate == 1.f &&
				paintLTpnt == m_ZoomOriLTpnt &&
				paintRBpnt == m_ZoomOriRBpnt)
			{
				m_cdc->MoveTo(m_pMxPrw[m_iImageID]->m_ZoomLTpnt);
				m_cdc->LineTo(m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x, m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y);
				m_cdc->LineTo(m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x, m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y);
				m_cdc->LineTo(m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x, m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y);
				m_cdc->LineTo(m_pMxPrw[m_iImageID]->m_ZoomLTpnt);
			}
			else
			{
				m_cdc->MoveTo(paintLTpnt);

				if (paintLTpnt.y != m_ZoomOriLTpnt.y)
					m_cdc->LineTo(paintRBpnt.x, paintLTpnt.y);
				else
					m_cdc->MoveTo(paintRBpnt.x, paintLTpnt.y);

				if (paintRBpnt.x != m_ZoomOriRBpnt.x)
					m_cdc->LineTo(paintRBpnt.x, paintRBpnt.y);
				else
					m_cdc->MoveTo(paintRBpnt.x, paintRBpnt.y);

				if (paintRBpnt.y != m_ZoomOriRBpnt.y)
					m_cdc->LineTo(paintLTpnt.x, paintRBpnt.y);
				else
					m_cdc->MoveTo(paintLTpnt.x, paintRBpnt.y);

				if (paintLTpnt.x != m_ZoomOriLTpnt.x)
					m_cdc->LineTo(paintLTpnt.x, paintLTpnt.y);
			}
		}
		m_cdc->SelectObject(oldPen);

		m_pMxPrw[m_iImageID]->m_PreZmRectPos.SetRect(m_pMxPrw[m_iImageID]->m_ZoomLTpnt, m_pMxPrw[m_iImageID]->m_ZoomRBpnt);
		m_pMxPrw[m_iImageID]->m_bZmPaintBit ^= 1;
	}
}
void CMixer2Dlg::AddItem(int iIDNum)
{
	CMoveable_Item *pMoveItem;

	switch (iIDNum)
	{
	case LEVEL_SLIDER:
		pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Level_Silder.bmp",
			theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parameter-L-b.bmp",
			CPoint(LEVEL_SLIDER_X_BEGIN, LEVEL_SLIDER_Y), CPoint(LEVEL_SLIDER_X_END, LEVEL_SLIDER_Y));
		break;
	case PARAM1_SLIDER:
		pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parm_Silder.bmp",
			theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parameter-s01-b.bmp",
			CPoint(PARAM_SLIDER_X_BEGIN, PARAM1_SLIDER_Y), CPoint(PARAM_SLIDER_X_END, PARAM1_SLIDER_Y));
		break;
	case PARAM2_SLIDER:
		pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parm_Silder.bmp",
			theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parameter-s01-b.bmp",
			CPoint(PARAM_SLIDER_X_BEGIN, PARAM2_SLIDER_Y), CPoint(PARAM_SLIDER_X_END, PARAM2_SLIDER_Y));
		break;
	case PARAM3_SLIDER:
		pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parm_Silder.bmp",
			theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parameter-s01-b.bmp",
			CPoint(PARAM_SLIDER_X_BEGIN, PARAM3_SLIDER_Y), CPoint(PARAM_SLIDER_X_END, PARAM3_SLIDER_Y));
		break;
	case PARAM4_SLIDER:
		pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parm_Silder.bmp",
			theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parameter-s01-b.bmp",
			CPoint(PARAM_SLIDER_X_BEGIN, PARAM4_SLIDER_Y), CPoint(PARAM_SLIDER_X_END, PARAM4_SLIDER_Y));
		break;
	case PARAM5_SLIDER:
		pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parm_Silder.bmp",
			theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parameter-s01-b.bmp",
			CPoint(PARAM_SLIDER_X_BEGIN, PARAM5_SLIDER_Y), CPoint(PARAM_SLIDER_X_END, PARAM5_SLIDER_Y));
		break;
	}

	switch (iIDNum)
	{
	case LEVEL_SLIDER:
		pMoveItem->OffsetObject(CPoint(LEVEL_SLIDER_X_BEGIN, LEVEL_SLIDER_Y));
		break;
	case PARAM1_SLIDER:
		pMoveItem->OffsetObject(CPoint(PARAM_SLIDER_X_BEGIN, PARAM1_SLIDER_Y));
		break;
	case PARAM2_SLIDER:
		pMoveItem->OffsetObject(CPoint(PARAM_SLIDER_X_BEGIN, PARAM2_SLIDER_Y));
		break;
	case PARAM3_SLIDER:
		pMoveItem->OffsetObject(CPoint(PARAM_SLIDER_X_BEGIN, PARAM3_SLIDER_Y));
		break;
	case PARAM4_SLIDER:
		pMoveItem->OffsetObject(CPoint(PARAM_SLIDER_X_BEGIN, PARAM4_SLIDER_Y));
		break;
	case PARAM5_SLIDER:
		pMoveItem->OffsetObject(CPoint(PARAM_SLIDER_X_BEGIN, PARAM5_SLIDER_Y));
		break;
	}
	pMoveItem->SetParentMenu(m_FramePic);
	m_FramePic->AddItem(pMoveItem);
	pMoveItem->Initialize();
}

void CMixer2Dlg::RefreshEffectCtrl()
{
	INT iSelEffect = m_pMxPrw[m_iImageID]->m_cEffect.GetEffectID();

	if (iSelEffect >= 0 && iSelEffect < g_EffectParmNameAry.GetCount())
	{
		BOOL bEmptyParm;

		m_pMxPrw[m_iImageID]->m_uEffItemBmp = 0;

		for (int x = 0; x<6; x++)
		{
			bEmptyParm = g_EffectParmNameAry[iSelEffect].strParmName[x].IsEmpty();

			if (!bEmptyParm)
			{
				m_pMxPrw[m_iImageID]->m_uEffItemBmp |= 0x01 << x;
			}
		}

		CPoint ptBtn;
		int Ptmp, PsliderY = PARAM1_SLIDER_Y;

		for (int x = 0; x < 6; x++)
		{
			int slider_id[] = { LEVEL_SLIDER, PARAM1_SLIDER, PARAM2_SLIDER, PARAM3_SLIDER, PARAM4_SLIDER, PARAM5_SLIDER };

			m_FramePic->DeleteItem(slider_id[x]);

			if (((m_pMxPrw[m_iImageID]->m_uEffItemBmp >> x) & 0x01) == 1)
			{
				AddItem(slider_id[x]);

				Ptmp = m_pMxPrw[m_iImageID]->MixerStream().GetEffectParam(x);
				m_sParmVal[x].Format("%d", Ptmp);
				UpdateData(FALSE);

				if (slider_id[x] == LEVEL_SLIDER) //for Level
					ptBtn = CPoint(LEVEL_SLIDER_X_BEGIN + Ptmp*LEVEL_SLIDER_LEN / MAX_LEVEL_VAL, LEVEL_SLIDER_Y);
				else //for PARAMs
					ptBtn = CPoint(PARAM_SLIDER_X_BEGIN + Ptmp*PARAM_SLIDER_LEN / MAX_PARAM_VAL, PsliderY);
				
				((CMoveable_Item*)m_FramePic->GetItemByID(slider_id[x]))->SetToPos(ptBtn, TRUE);
				((CMoveable_Item*)m_FramePic->GetItemByID(slider_id[x]))->RedrawBGImage(TRUE);

				m_ParamStatic[x].ShowWindow(SW_SHOW);
			}
			else
				m_ParamStatic[x].ShowWindow(SW_HIDE);
		}
	}
}
void CMixer2Dlg::RefreshEffectText()
{
#if 0
	if (m_iImageID != -1)
	{
		INT iSelEffect = m_pMxPrw[m_iImageID]->m_cEffect.GetEffectID();

		if (iSelEffect >= 0 && iSelEffect < g_EffectParmNameAry.GetCount())
		{
			for (int x = 0; x < 6; x++)
			{
				m_cdc->SetBkMode(TRANSPARENT);
				m_cdc->SetTextColor(RGB(255, 255, 255));

				int y_axis_pos[] = { 599, 645, 704, 763, 824, 883, 0 };

				m_cdc->TextOut(980, y_axis_pos[x], g_EffectParmNameAry[iSelEffect].strParmName[x]);
			}
		}
	}
#else
	//char szMsg[512];
	//RECT rect;
	HDC hDC;
	hDC = ::GetDC(this->GetSafeHwnd());

	SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, RGB(255, 255, 255));

	if (m_iImageID != -1)
	{
		INT iSelEffect = m_pMxPrw[m_iImageID]->m_cEffect.GetEffectID();

		//sprintf(szMsg, "CMixer2Dlg::RefreshEffectText() - iSelEffect = %d\n", iSelEffect);
		//OutputDebugStringA(szMsg);

		if (iSelEffect >= 0 && iSelEffect < g_EffectParmNameAry.GetCount())
		{
			for (int x = 0; x < 6; x++)
			{
				int y_axis_pos[] = { 599, 645, 704, 763, 824, 883, 0 };

				TextOut(hDC, 980, y_axis_pos[x], g_EffectParmNameAry[iSelEffect].strParmName[x], strlen(g_EffectParmNameAry[iSelEffect].strParmName[x]));

				//sprintf(szMsg, "CMixer2Dlg::RefreshEffectText() - strParmName = %s [%d]\n", g_EffectParmNameAry[iSelEffect].strParmName[x], iSelEffect);
				//OutputDebugStringA(szMsg);
			}
		}
	}
	::ReleaseDC(this->GetSafeHwnd(), hDC);
#endif
}

int CMixer2Dlg::VolToPos(int vol)
{
	int pos = 0;

	if (vol <= 49)
		pos = vol*2.6 + 230;
	else// >=50
		pos = (vol - 50)*2.6;
	return pos;
}

CMixer2Dlg::CMixer2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TEMP_DLG, pParent)
{
	m_bInitOK = FALSE;
	m_cdc = NULL;
	m_iImageID = -1;
	ZeroMemory(m_pMxPrw, sizeof(m_pMxPrw));
	m_FramePic = NULL;
	m_pVolumeCtrl = NULL;
	m_pRGB32Buf = new unsigned char[_THUMBNAIL_FRAME_W * _THUMBNAIL_FRAME_H * 4];

	for (int x = 0; x < 6; x++)
		m_sParmVal[x] = _T("50");
}

CMixer2Dlg::~CMixer2Dlg()
{
	if (m_pRGB32Buf)
		delete m_pRGB32Buf;
}

void CMixer2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	if (m_bInitOK)
	{
		#if 1
		DDX_Text(pDX, IDC_EDIT_WIDTH_ID, m_sWidth);
		DDX_Text(pDX, IDC_EDIT_HEIGHT_ID, m_sHeight);

		if (pDX->m_bSaveAndValidate == FALSE) //execute only for UpdateData(FALSE) to generate indirect data
		{
			CRect zoomOriRect(m_ZoomOriLTpnt, m_ZoomOriRBpnt);

			m_iXedit = (m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x - m_ZoomOriLTpnt.x)*_ttoi(m_sWidth) / zoomOriRect.Width();
			m_iYedit = (m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y - m_ZoomOriLTpnt.y)*_ttoi(m_sHeight) / zoomOriRect.Height();
		}
		//for Zoom In-Out
		DDX_Text(pDX, IDC_XEDIT_ID, m_iXedit);
		DDV_MinMaxInt(pDX, m_iXedit, 0-_ttoi(m_sWidth), _ttoi(m_sWidth));
		DDX_Text(pDX, IDC_YEDIT_ID, m_iYedit);
		DDV_MinMaxInt(pDX, m_iYedit, 0-_ttoi(m_sHeight), _ttoi(m_sHeight));
		#else
		DDX_Text(pDX, IDC_XEDIT_ID, m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x);
		DDV_MinMaxInt(pDX, m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x, m_ZoomOriLTpnt.x - 100, m_ZoomOriLTpnt.x + 100);
		DDX_Text(pDX, IDC_YEDIT_ID, m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y);
		DDV_MinMaxInt(pDX, m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y, m_ZoomOriLTpnt.y - 100, m_ZoomOriLTpnt.y + 100);

		DDX_Text(pDX, IDC_EDIT_WIDTH_ID, m_sWidth);
		DDX_Text(pDX, IDC_EDIT_HEIGHT_ID, m_sHeight);
		#endif

		//for Effect Parameters
		for (int x = 0; x < 6; x++)
		{
			DDX_Text(pDX, IDC_STATIC_LEVEL_ID + x, m_sParmVal[x]);
		}
	}
}


BEGIN_MESSAGE_MAP(CMixer2Dlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_MESSAGE(WM_MIXER_SEEK, OnSeek)
	ON_MESSAGE(WM_MIXER_VOL_NOTIFY, OnVolChgNotify)
	ON_MESSAGE(WM_MIXER_CLR_EXIST_ZOOM, OnClrExistZoomRect)
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_CBN_SELCHANGE(IDC_COMBO_ID, &CMixer2Dlg::OnSelchange)
END_MESSAGE_MAP()


// CMixer2Dlg message handlers
BOOL CMixer2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化

	m_hMemDC = CreateCompatibleDC(NULL);

	m_FramePic = new CMenu_Class(this, m_hWnd, 1, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\BG1.bmp");

	CItem_Class *pItem;

	//video1
	pItem = new CItem_Class(this, m_hWnd, V1_PAUSE_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Media-01.bmp", true, true, true);
	pItem->OffsetObject(CPoint(235, 453));
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, V1_TO_START_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Media-02.bmp", true, true, true);
	pItem->OffsetObject(CPoint(275, 453));
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, V1_PLAY_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Media-03.bmp", true, true, true);
	pItem->OffsetObject(CPoint(315, 453));
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, V1_TO_END_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Media-04.bmp", true, true, true);
	pItem->OffsetObject(CPoint(355, 453));
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, V1_STOP_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Media-05.bmp", true, true, true);
	pItem->OffsetObject(CPoint(395, 453));
	m_FramePic->AddItem(pItem);
	/*
	pMoveItem = new CMoveable_Item(this, m_hWnd, V1_SLIDER, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer-slider.bmp",
		theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer.bmp",
		CPoint(440, 460), CPoint(743, 460));
	pMoveItem->SetParentMenu(m_FramePic);
	pMoveItem->OffsetObject(CPoint(440, 460));
	m_FramePic->AddItem(pMoveItem);
	*/
	m_pVideo1Slider = new CBitSlider(LAYER1);
	m_pVideo1Slider->Create(440, 460, 328, 25, this, 1004);

	std::string sVideoSlider_layer = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer.bmp";
	m_pVideo1Slider->BuildThumbItem((char*)sVideoSlider_layer.c_str(), 9, 22, 0);
	std::string sVideoSlider_layer_t = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer_t.bmp";
	m_pVideo1Slider->BuildThumbItem((char*)sVideoSlider_layer_t.c_str(), 9, 22, 1);
	std::string sVideoSlider_layer_c = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer_c.bmp";
	m_pVideo1Slider->BuildThumbItem((char*)sVideoSlider_layer_c.c_str(), 9, 22, 2);

	std::string sVideoSlider = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer-slider.bmp";
	std::string sVideoSlider_Active = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer-slider_active.bmp";
	m_pVideo1Slider->BuildBackItem((char*)sVideoSlider.c_str(), (char*)sVideoSlider_Active.c_str());
	m_pVideo1Slider->SetTopOffset(1);
	m_pVideo1Slider->SetPosV(0);

	//video2
	pItem = new CItem_Class(this, m_hWnd, V2_PAUSE_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Media-01.bmp", true, true, true);
	pItem->OffsetObject(CPoint(805, 453));
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, V2_TO_START_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Media-02.bmp", true, true, true);
	pItem->OffsetObject(CPoint(845, 453));
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, V2_PLAY_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Media-03.bmp", true, true, true);
	pItem->OffsetObject(CPoint(885, 453));
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, V2_TO_END_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Media-04.bmp", true, true, true);
	pItem->OffsetObject(CPoint(925, 453));
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, V2_STOP_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Media-05.bmp", true, true, true);
	pItem->OffsetObject(CPoint(965, 453));
	m_FramePic->AddItem(pItem);
	/*
	pMoveItem = new CMoveable_Item(this, m_hWnd, V2_SLIDER, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer-slider.bmp",
		theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer.bmp",
		CPoint(1015, 460), CPoint(1318, 460));
	pMoveItem->SetParentMenu(m_FramePic);
	pMoveItem->OffsetObject(CPoint(1015, 460));
	m_FramePic->AddItem(pMoveItem);
	*/

	m_pVideo2Slider = new CBitSlider(LAYER2);
	m_pVideo2Slider->Create(1015, 460, 328, 25, this, 1005);

	m_pVideo2Slider->BuildThumbItem((char*)sVideoSlider_layer.c_str(), 9, 22, 0);
	m_pVideo2Slider->BuildThumbItem((char*)sVideoSlider_layer_t.c_str(), 9, 22, 1);
	m_pVideo2Slider->BuildThumbItem((char*)sVideoSlider_layer_c.c_str(), 9, 22, 2);
	m_pVideo2Slider->BuildBackItem((char*)sVideoSlider.c_str(), (char*)sVideoSlider_Active.c_str());
	m_pVideo2Slider->SetTopOffset(1);
	m_pVideo2Slider->SetPosV(0);

	//video3
	pItem = new CItem_Class(this, m_hWnd, V3_PAUSE_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Media-01.bmp", true, true, true);
	pItem->OffsetObject(CPoint(1370, 453));
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, V3_TO_START_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Media-02.bmp", true, true, true);
	pItem->OffsetObject(CPoint(1410, 453));
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, V3_PLAY_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Media-03.bmp", true, true, true);
	pItem->OffsetObject(CPoint(1450, 453));
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, V3_TO_END_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Media-04.bmp", true, true, true);
	pItem->OffsetObject(CPoint(1490, 453));
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, V3_STOP_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Media-05.bmp", true, true, true);
	pItem->OffsetObject(CPoint(1530, 453));
	m_FramePic->AddItem(pItem);

	/*
	pMoveItem = new CMoveable_Item(this, m_hWnd, V3_SLIDER, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer-slider.bmp",
		theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer.bmp",
		CPoint(1580, 460), CPoint(1883, 460));
	pMoveItem->SetParentMenu(m_FramePic);
	pMoveItem->OffsetObject(CPoint(1580, 460));
	m_FramePic->AddItem(pMoveItem);
	*/

	m_pVideo3Slider = new CBitSlider(MIXER);
	m_pVideo3Slider->Create(1580, 460, 328, 25, this, 1006);

	m_pVideo3Slider->BuildThumbItem((char*)sVideoSlider_layer.c_str(), 9, 22, 0);
	m_pVideo3Slider->BuildThumbItem((char*)sVideoSlider_layer_t.c_str(), 9, 22, 1);
	m_pVideo3Slider->BuildThumbItem((char*)sVideoSlider_layer_c.c_str(), 9, 22, 2);
	m_pVideo3Slider->BuildBackItem((char*)sVideoSlider.c_str(), (char*)sVideoSlider_Active.c_str());
	m_pVideo3Slider->SetTopOffset(1);
	m_pVideo3Slider->SetPosV(0);

	pItem = new CItem_Class(this, m_hWnd, UP_BUTTON, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Physics-up.bmp", true, true, true);
	pItem->OffsetObject(CPoint(484, 581));
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, DOWN_BUTTON, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Physics-down.bmp", true, true, true);
	pItem->OffsetObject(CPoint(484, 871));
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, LEFT_BUTTON, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Physics-left.bmp", true, true, true);
	pItem->OffsetObject(CPoint(276, 719));
	m_FramePic->AddItem(pItem);

	pItem = new CItem_Class(this, m_hWnd, RIGHT_BUTTON, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Physics-right.bmp", true, true, true);
	pItem->OffsetObject(CPoint(706, 719));
	m_FramePic->AddItem(pItem);

	/*pItem = new CItem_Class(this, m_hWnd, ROTATE, theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Rotate.bmp", true, true, true);
	pItem->OffsetObject(CPoint(680, 907));
	m_FramePic->AddItem(pItem);*/ //?

	m_FramePic->Initialize();

	CRect rectTemp;
	rectTemp = g_DispArys.GetAt(0);
	INT32 iWidth = rectTemp.Width();
	INT32 iHeight = rectTemp.Height();

	if (iWidth != 1920 || iHeight != 1080)
		m_FramePic->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);

	CRgn NullRgn;
	NullRgn.CreateRectRgn(0, 0, 0, 0);
	m_FramePic->SetWindowRgn(NullRgn, true);

	std::string sZoomCtrlBmp = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Physics-zoom.bmp";
	m_ZoomCtrlBmp.Load((char*)sZoomCtrlBmp.c_str());

	std::string sVolumeCtrlBmp = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Vol.bmp";
	m_VolumeCtrlBmp.Load((char*)sVolumeCtrlBmp.c_str());

	std::string sVolumeCtrl = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Vol_Round.bmp";
	m_pVolumeCtrl = new CRoundSliderCtrl;
	m_pVolumeCtrl->SetBGFileName((char*)sVolumeCtrl.c_str());
	m_pVolumeCtrl->Create(1753,910,109,109,this,1002);

	m_pVolumeSlider = new CBitSlider;
	m_pVolumeSlider->CreateV(1794, 570, 25, 328, this, 1003);

	std::string sVolumeSlider_layer = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer_v.bmp";
	m_pVolumeSlider->BuildThumbItem((char*)sVolumeSlider_layer.c_str(), 22, 9, 0);
	std::string sVolumeSlider_layer_t = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer_t_v.bmp";
	m_pVolumeSlider->BuildThumbItem((char*)sVolumeSlider_layer_t.c_str(), 22, 9, 1);
	std::string sVolumeSlider_layer_c = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer_c_v.bmp";
	m_pVolumeSlider->BuildThumbItem((char*)sVolumeSlider_layer_c.c_str(), 22, 9, 2);

	std::string sVolumeSlider = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer-slider_v.bmp";
	std::string sVolumeSlider_Active = theApp.m_strCurPath + "\\UI Folder\\Mixer2\\Layer-slider_active_v.bmp";
	m_pVolumeSlider->BuildBackItem((char*)sVolumeSlider.c_str(), (char*)sVolumeSlider_Active.c_str());
	m_pVolumeSlider->SetTopOffset(1);
	m_pVolumeSlider->SetPosV(50);

	m_pX_EditCtrl = new CEditCtrl;
	m_pX_EditCtrl->Create(290, 955, 60, 20, this, IDC_XEDIT_ID);

	m_pY_EditCtrl = new CEditCtrl;
	m_pY_EditCtrl->Create(290, 1000, 60, 20, this, IDC_YEDIT_ID);

	m_pWidth_EditCtrl = new CEditCtrl;
	m_pWidth_EditCtrl->Create(475, 955, 60, 20, this, IDC_EDIT_WIDTH_ID);

	m_pHeight_EditCtrl = new CEditCtrl;
	m_pHeight_EditCtrl->Create(475, 1000, 60, 20, this, IDC_EDIT_HEIGHT_ID);

	//m_pAngle_EditCtrl = new CEditCtrl; ?
	//m_pAngle_EditCtrl->Create(650, 1000, 60, 20, this, 1011); ?
	
	m_pIconListCtrl = new IconListCtrl;
	m_pIconListCtrl->Create(8, 90, 210, 980, this, 1012);

	m_rPreviewRgn[LAYER1].SetRect(223, 86, 784, 444);
	m_rPreviewRgn[LAYER2].SetRect(790, 86, 1351, 444);
	m_rPreviewRgn[MIXER].SetRect(1357, 86, 1919, 444);

	m_ParamStatic[0].Create("50", WS_CHILD | SS_CENTER, CRect(1450, 599, 1475, 615), this, IDC_STATIC_LEVEL_ID);
	m_ParamStatic[1].Create("50", WS_CHILD | SS_CENTER, CRect(1450, 645, 1475, 661), this, IDC_STATIC_PARAM1_ID);
	m_ParamStatic[2].Create("50", WS_CHILD | SS_CENTER, CRect(1450, 704, 1475, 720), this, IDC_STATIC_PARAM2_ID);
	m_ParamStatic[3].Create("50", WS_CHILD | SS_CENTER, CRect(1450, 763, 1475, 779), this, IDC_STATIC_PARAM3_ID);
	m_ParamStatic[4].Create("50", WS_CHILD | SS_CENTER, CRect(1450, 824, 1475, 840), this, IDC_STATIC_PARAM4_ID);
	m_ParamStatic[5].Create("50", WS_CHILD | SS_CENTER, CRect(1450, 883, 1475, 899), this, IDC_STATIC_PARAM5_ID);

	m_EffectCombox.Create(WS_CHILD | CBS_DROPDOWNLIST | CBS_SORT | CBS_HASSTRINGS | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,
		CRect(1498/*1598*/, 594, 1665/*1765*/, 794), this, IDC_COMBO_ID);

	m_EffectCombox.InsertString(0, "31. Mirage");
	m_EffectCombox.InsertString(0, "30. Blur");
	m_EffectCombox.InsertString(0, "29. Single Color");
	m_EffectCombox.InsertString(0, "28. Dot Mosaics");
	m_EffectCombox.InsertString(0, "27. Plane Rotate");
	m_EffectCombox.InsertString(0, "26. Cylinder Rotate");
	m_EffectCombox.InsertString(0, "25. Circle Center");
	m_EffectCombox.InsertString(0, "24. Kaleidoscope");
	m_EffectCombox.InsertString(0, "23. WaterColor");
	m_EffectCombox.InsertString(0, "22. Wave");
	m_EffectCombox.InsertString(0, "21. Edge");
	m_EffectCombox.InsertString(0, "20. Wood Noise");
	m_EffectCombox.InsertString(0, "19. Halftone");
	m_EffectCombox.InsertString(0, "18. Fish Eye");
	m_EffectCombox.InsertString(0, "17. Ripple");
	m_EffectCombox.InsertString(0, "16. Glass Windows");
	m_EffectCombox.InsertString(0, "15. Color Modify");
	m_EffectCombox.InsertString(0, "14. Neon Light Texture");
	m_EffectCombox.InsertString(0, "13. Blind Texture");
	m_EffectCombox.InsertString(0, "12. Swirl Texture");
	m_EffectCombox.InsertString(0, "11. Cross Hatching");
	m_EffectCombox.InsertString(0, "10. Rotate Scale");
	m_EffectCombox.InsertString(0, "09. Pixelate");
	m_EffectCombox.InsertString(0, "08. Multi Movie");
	m_EffectCombox.InsertString(0, "07. Freeze Movie");
	m_EffectCombox.InsertString(0, "06. Rectangle Bright Dark");
	m_EffectCombox.InsertString(0, "05. Sleazy Emboss");
	m_EffectCombox.InsertString(0, "04. Brightness Flicker");
	m_EffectCombox.InsertString(0, "03. RGB Color");
	m_EffectCombox.InsertString(0, "02. Gray");
	m_EffectCombox.InsertString(0, "01. Negative");
	m_EffectCombox.InsertString(0, "00. No Effect");
	m_EffectCombox.SetCurSel(0);

	if (g_PannelSetting.iTVWallMode)
	{
		g_EffectParmNameAry.Add({ "Left","Top","Right","GapX", "GapY" });
	}
	else if (m_iCurLang == ENGLISH_TYE)
	{
		g_EffectParmNameAry.Add({ "Mirage", "", "","","","","" });
		g_EffectParmNameAry.Add({ "Blur", "Level", "","","","","" });
		g_EffectParmNameAry.Add({ "Single Color", "Level", "Contrast","","","","" });
		g_EffectParmNameAry.Add({ "Dot Mosaics", "Level", "Red Shift","Green Shift","Blue Shift","","" });
		g_EffectParmNameAry.Add({ "Plane Rotate", "Level", "Frequency","Amplitude","","","" });
		g_EffectParmNameAry.Add({ "Cylinder Rotate", "Level", "Contast","","","","" });
		g_EffectParmNameAry.Add({ "Circle Center", "Level", "Bright Shift","Left","Right","Top", "Down" });
		g_EffectParmNameAry.Add({ "Kaleidoscope", "Freeze", "","","","" });
		g_EffectParmNameAry.Add({ "WaterColor", "Multi", "H Shift","V Shift","Mirror","" });
		g_EffectParmNameAry.Add({ "Wave", "Pixel Size", "","","","" });
		g_EffectParmNameAry.Add({ "Edge", "Angle", "Width Zoom","Heigth Zoom","H Shfit","V Shift" });
		g_EffectParmNameAry.Add({ "Wood Noise", "Level", "Density","","","" });
		g_EffectParmNameAry.Add({ "Halftone", "Amount", "Radius","","","" });
		g_EffectParmNameAry.Add({ "Fish Eye", "Level", "Density","Contrast","","" });
		g_EffectParmNameAry.Add({ "Ripple", "Level", "Light Size","H Move","V Move","" });
		g_EffectParmNameAry.Add({ "Glass Windows", "Level", "Brightness","Hue","Saturation","" });
		g_EffectParmNameAry.Add({ "Color Modify", "Level", "Pattern","","","" });
		g_EffectParmNameAry.Add({ "Neon Light Texture", "Peak", "Speed","Wavelength","H Shift","V Shift" });
		g_EffectParmNameAry.Add({ "Blind Texture", "Curvature", "Layer","","","" });
		g_EffectParmNameAry.Add({ "Swirl Texture", "Density", "Brightness","","","" });
		g_EffectParmNameAry.Add({ "Cross Hatching", "R", "G","B","H Num","V Num" });
		g_EffectParmNameAry.Add({ "Rotate Scale", "Edge Width", "","","","" });
		g_EffectParmNameAry.Add({ "Pixelate", "Wave Number", "Frequency","Twist","", "" });
		g_EffectParmNameAry.Add({ "Multi Movie", "Level", "Saturation","","","" });
		g_EffectParmNameAry.Add({ "Freeze Movie", "Level", "Speed","","","" });
		g_EffectParmNameAry.Add({ "Rectangle Bright Dark", "Number", "Angle1","Rotate1","Angle2","Rotate2" });
		g_EffectParmNameAry.Add({ "Sleazy Emboss", "Number", "Rotate","Shift","","" });
		g_EffectParmNameAry.Add({ "Brightness Flicker", "Plane Num", "Rotate","X Move","Z Move","Distance" });
		g_EffectParmNameAry.Add({ "RGB Color", "Dot Size", "Brigtness","Soft","","" });
		g_EffectParmNameAry.Add({ "Gray", "Color Range", "Red", "Green", "Blue","Saturation" });
		g_EffectParmNameAry.Add({ "Negative", "Frame Num", "", "", "","" });
		g_EffectParmNameAry.Add({ "No Effect", "Frequency", "Angle", "Range", "Amplitude","Y Offset" });
	}

	for (int id = 0; id < PREVIEW_NUM; id++)
	{
		if (!m_pMxPrw[id])
		{
			m_pMxPrw[id] = new MixerPreview;
			m_pMxPrw[id]->Create(m_rPreviewRgn[id], this, IDC_STATIC_PREVIEW1 + id, id==LAYER1? m_pVideo1Slider: (id==LAYER2? m_pVideo2Slider: m_pVideo3Slider));
			//m_pMxPrw[id]->m_iSoundPos = m_pVolumeCtrl->GetPos();
			m_pMxPrw[id]->m_fZoomRate = 1.0;
		}
	}

	m_cdc = new CClientDC(this);

	//init original zoom rect position
	m_ZoomOriLTpnt.x = 287 + 18;
	m_ZoomOriLTpnt.y = 594 + 18;
	m_ZoomOriRBpnt.x = 287 + 431 - 18;
	m_ZoomOriRBpnt.y = 594 + 291 - 18;

	//after init ok, DDX data change activate correctly
	m_bInitOK = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CMixer2Dlg::OnCancel()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別	
	GetParent()->SendMessage(MSG_MAINUI);
	ShowWindow(SW_HIDE);
	return;

	CDialog::OnCancel();
}

void CMixer2Dlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此加入您的訊息處理常式程式碼

	if (m_FramePic)
		delete m_FramePic;

	if (m_hMemDC)
	{
		DeleteDC(m_hMemDC);
		m_hMemDC = 0;
	}

	if (m_pVolumeCtrl)
		delete m_pVolumeCtrl;

	if (m_pVolumeSlider)
		delete m_pVolumeSlider;

	if (m_pVideo1Slider)
		delete m_pVideo1Slider;

	if (m_pVideo2Slider)
		delete m_pVideo2Slider;

	if (m_pVideo3Slider)
		delete m_pVideo3Slider;

	if (m_pX_EditCtrl)
		delete m_pX_EditCtrl;

	if (m_pY_EditCtrl)
		delete m_pY_EditCtrl;

	if (m_pWidth_EditCtrl)
		delete m_pWidth_EditCtrl;

	if (m_pHeight_EditCtrl)
		delete m_pHeight_EditCtrl;

	//if (m_pAngle_EditCtrl) ?
		//delete m_pAngle_EditCtrl; ?

	if (m_pIconListCtrl)
		delete m_pIconListCtrl;

	if (m_cdc)
		delete m_cdc;

	for (int i = 0; i < PREVIEW_NUM; i++)
		if (m_pMxPrw[i])
		{
			delete m_pMxPrw[i];
			m_pMxPrw[i] = NULL;
		}
//	FreeUIObj();
}

void CMixer2Dlg::OnPaint()
{

	//m_bLBClicked = FALSE;
	m_FramePic->OnPaint();

	//Invalidate(); //for effect UI refresh used, can be removed to verify its meanings by changing selection on effect combox
	//UpdateWindow(); //these two line will cause OnPaint() to be recursively called, so let's be commented

	DrawUI();
	DrawText();

	if (m_iImageID != -1)
	{
		CPen GreenPen(PS_SOLID, 3, RGB(0, 255, 0));

		m_pMxPrw[m_iImageID]->m_bZmPaintBit = 0;
		DrawZoomRect();
		DrawRect(m_pMxPrw[m_iImageID]->m_PicCtrl.m_hWnd, m_cdc->m_hDC, GreenPen.operator HPEN());
	}

	RefreshEffectText();

	RedrawDlgUICtrl();
	
	CDialog::OnPaint();
}

void CMixer2Dlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
//	if (bShow)
	//	bool bRet = RegisterHotKey(this->GetSafeHwnd(), _ESCAPE_KEY_ID, 0, VK_ESCAPE);
	if (bShow)
	{
		m_pIconListCtrl->Clean();
		//m_pIconListCtrl->DeleteAllItems();
		//m_ImageList.DeleteImageList();
		//bool bRet = m_ImageList.Create(_THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H, ILC_COLOR32 | ILC_MASK, 0, 1);

		CString tmpStr;
		wchar_t wszKeyword[256];
		//unsigned char* pRGBBuf = new unsigned char[128 * 96 * 4];
		//memset(pRGBBuf, 128, 128 * 96 * 4);

		for (int j = 0;j < _MAX_SOURCE;j++)
			m_iStreamIndexMapping[j] = -1;

		int iSourceCount = 0;

		for (int i = 0; i < g_MediaStreamAry.size(); i++)
			if (!(tmpStr = g_MediaStreamAry.at(i)->FilePath()).IsEmpty())
			{
				//tmpStr = g_MediaStreamAry.at(i)->FileName();
				tmpStr.Replace("!@#$%^", "");

				if (m_pIOSourceCtrlDll)
				{
					wcscpy(wszKeyword, ANSIToUnicode(tmpStr.GetBuffer()));
					MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfo(wszKeyword);

					if (pCurItem)
					{
						wchar_t* pwszShortFileName = m_pIOSourceCtrlDll->GetFileName(pCurItem->wszFileName);
						//m_pIconListCtrl->InsertItem(i, WCharToChar(pwszShortFileName), m_ImageList.GetImageCount());

						ThumbnailPicture* pPic = m_pIOSourceCtrlDll->GetPicture(pCurItem);
						if (pPic)
						{
							m_yuv_converter.I420ToARGB(pPic->pBuffer, m_pRGB32Buf, _THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H);
							m_pIconListCtrl->AddItem(WCharToChar(pwszShortFileName), m_pRGB32Buf, pCurItem->ulID);
							/*
							CBitmap bmp;
							bool bRet = bmp.CreateBitmap(_THUMBNAIL_FRAME_W, _THUMBNAIL_FRAME_H, 1, 32, m_pRGB32Buf);
							int iRet = m_ImageList.Add(&bmp, RGB(255, 0, 255));
							bmp.DeleteObject();
							*/
						}

						m_iStreamIndexMapping[iSourceCount] = i;
						iSourceCount++;
					}
				}
				//m_pIconListCtrl->AddItem(tmpStr.GetBuffer(), pRGBBuf, 128, 96);
			}
		//m_pIconListCtrl->SetImageList(&m_ImageList, LVSIL_NORMAL);
		//delete pRGBBuf;

		//init cdc mode		
		m_cdc->SetROP2(R2_XORPEN);

		//timer enable
		SetTimer(1, 100, NULL);

		//init status
		m_bUpdateVol = false;
		m_bPreviewChg = false;
		m_pIconListCtrl->m_bListChg = false;
		m_pIconListCtrl->m_ulSelID = -1;
		m_pX_EditCtrl->SetWindowText("");
		m_pY_EditCtrl->SetWindowText("");
		m_pWidth_EditCtrl->SetWindowText("");
		m_pHeight_EditCtrl->SetWindowText("");
		m_EffectCombox.EnableWindow(FALSE);
		m_iXedit = 0;
		m_iYedit = 0;

		CPoint zeroPnt(0, 0);
		
		for (int i = 0; i<PREVIEW_NUM; i++)
		{
			//init Zoom Rect Paint bit
			m_pMxPrw[i]->m_bZmPaintBit = 0;

			//init zoomed rect position as original zoom rect position at start
			if (m_pMxPrw[i]->m_ZoomLTpnt == zeroPnt && m_pMxPrw[i]->m_ZoomRBpnt == zeroPnt)
			{
				m_pMxPrw[i]->m_ZoomLTpnt = m_ZoomOriLTpnt;
				m_pMxPrw[i]->m_ZoomRBpnt = m_ZoomOriRBpnt;
				m_pMxPrw[i]->ToNoZoomSym(CRect(m_ZoomOriLTpnt, m_ZoomOriRBpnt));
			}
		}
	}
	else
	{
		KillTimer(1);

		m_EffectCombox.SetCurSel(0);

		for (int id = LEVEL_SLIDER; id <= PARAM5_SLIDER; id++)
		{
			m_FramePic->DeleteItem(id);
			m_ParamStatic[id - LEVEL_SLIDER].ShowWindow(SW_HIDE);
		}
	}

	m_iImageID = -1; //put here for avoid crashing while execute OnWndMsg{if (m_iImageID!=-1 && m_pMxPrw[m_iImageID]->StreamExist())} after OnDestroy {delete m_pMxPrw[i];} at condition of m_iImage!=-1

	::PostMessage(this->GetSafeHwnd(), WM_PAINT, 0, 0);

	CDialog::OnShowWindow(bShow, nStatus);
}

void CMixer2Dlg::DrawBMP(ImageFile* pImgFile, int x, int y, int w, int h,int iColorKeyR, int iColorKeyG, int iColorKeyB)
{
	BITMAPINFOHEADER bmpInfo;
	memset(&bmpInfo, 0, sizeof(BITMAPINFOHEADER));
	bmpInfo.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.biWidth = pImgFile->GetWidth();
	bmpInfo.biHeight = pImgFile->GetHeight();
	bmpInfo.biPlanes = 1;
	bmpInfo.biBitCount = 24;

	bmpInfo.biCompression = BI_RGB;
	bmpInfo.biSizeImage = pImgFile->GetWidth() * pImgFile->GetHeight() * 3;
	bmpInfo.biSizeImage = 0;

	HDC hdc;
	//RECT rect;
	hdc = ::GetDC(this->GetSafeHwnd());

	if (iColorKeyR == -1 && iColorKeyG == -1 && iColorKeyB == -1)
	{
		::SetStretchBltMode(hdc, COLORONCOLOR);
		StretchDIBits(hdc,
			x,
			y,
			w,
			h,
			0,
			0,
			pImgFile->GetWidth(),
			pImgFile->GetHeight() - 1,
			pImgFile->GetBuffer(),//pBuffer,
			(BITMAPINFO*)&bmpInfo,
			DIB_RGB_COLORS,
			SRCCOPY);
	}
	else
	{
		COLORREF ColorKey = RGB(iColorKeyR, iColorKeyG, iColorKeyB);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, pImgFile->GetHBITMAP());

		TransparentBlt(hdc,
			x,
			y,
			w,
			h,
			m_hMemDC,
			0,
			0,
			pImgFile->GetWidth(),
			pImgFile->GetHeight(),
			ColorKey);  //透明
		

		if (hOldBitmap)
			SelectObject(m_hMemDC, hOldBitmap);
	}
	::ReleaseDC(this->GetSafeHwnd(), hdc);
}

void CMixer2Dlg::DrawUI()
{
	DrawBMP(&m_ZoomCtrlBmp, 287, 594, 432, 292,255,255,255);

	//1722,910  //m_VolumeCtrl
	DrawBMP(&m_VolumeCtrlBmp, 1722, 910, 173, 109, 255, 255, 255);

	//each Invalidate() will invoke "OnPaint() event function of relevant control item"

	//for resolve "List Ctrl icon disappear at second invoke Mixer dialog after mouse click on it"
	m_pIconListCtrl->Invalidate();

	//for resolve video slider disappear at first Mixer show window
	m_pVideo1Slider->Invalidate();
	m_pVideo2Slider->Invalidate();
	m_pVideo3Slider->Invalidate();

	//for resolve vol round slider text(50) disappear at Mixer show window
	m_pVolumeCtrl->Invalidate();

	//for resolve vol slider disappear at first Mixer show window
	m_pVolumeSlider->Invalidate();
}

void CMixer2Dlg::DrawText()
{
	HDC hDC;
	//RECT rect;
	hDC = ::GetDC(this->GetSafeHwnd());

	SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, RGB(255, 255, 255));

	TextOut(hDC, 88, 61, "Video", strlen("Video"));
	TextOut(hDC, 243, 61, "Video1(1920x1080)", strlen("Video1(1920x1080)"));
	TextOut(hDC, 810, 61, "Video2(1920x1080)", strlen("Video2(1920x1080)"));
	TextOut(hDC, 1715, 509, "System Sound", strlen("System Sound"));

	TextOut(hDC, 270, 955, "X:", strlen("X:"));
	TextOut(hDC, 270, 1000, "Y:", strlen("Y:"));

	TextOut(hDC, 420, 955, "Width:", strlen("Width:"));
	TextOut(hDC, 420, 1000, "Height:", strlen("Height:"));
	//TextOut(hDC, 590, 1000, "Angle:", strlen("Angle:")); ?
	
	::ReleaseDC(this->GetSafeHwnd(), hDC);
}

void CMixer2Dlg::SetIOSourceCtrlDll(IOSourceCtrlDll * pObj)
{
	m_pIOSourceCtrlDll = pObj;
}

LRESULT CMixer2Dlg::OnVolChgNotify(WPARAM wParam, LPARAM lParam)
{
	//if (m_iImageID != -1 && m_pMxPrw[m_iImageID]->StreamExist())
	//{
		if (wParam > 100) 
			wParam = 100;

		m_pMxPrw[m_iImageID]->MixerStream().SetVolume(wParam);
		//m_pMxPrw[m_iImageID]->m_iSoundPos = lParam;
	//}

	return LRESULT();
}

LRESULT CMixer2Dlg::OnSeek(WPARAM wParam, LPARAM lParam)
{
	if (lParam == -1) //system vol slider
	{
		if (m_iImageID != -1 && wParam <= 100 && m_pMxPrw[m_iImageID]->StreamExist()) //when LButtonUp
			m_pMxPrw[m_iImageID]->MixerStream().SetSystemVolume(wParam);
	}
	else              //preview slider
	{
		if (m_pMxPrw[lParam]->StreamExist())
		{
			if (wParam > 100) //when LButtonDown
				m_pMxPrw[lParam]->m_bPlaying = FALSE;
			else              //when LButtonUp
			{
				m_pMxPrw[lParam]->MixerStream().Seek(m_pMxPrw[lParam]->MixerStream().GetTotalPos()*wParam/100.f);
				m_pMxPrw[lParam]->m_bPlaying = TRUE;
			}
		}
		else
		{
			switch (lParam)
			{
				case LAYER1:
					m_pVideo1Slider->SetPosV(0);
					break;
				case LAYER2:
					m_pVideo2Slider->SetPosV(0);
					break;
				case MIXER:
					m_pVideo3Slider->SetPosV(0);
					break;
			}
		}
	}
	return true;
}

LRESULT CMixer2Dlg::OnClrExistZoomRect(WPARAM wParam, LPARAM lParam)
{
	if (m_iImageID >= LAYER1)
	{
		MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(lParam);

		if (m_pMxPrw[m_iImageID]->m_bZmPaintBit &&								//if zoom rect UI is painted
			wcscmp(pCurItem->wszFileName, m_pMxPrw[m_iImageID]->m_sFileName))   //if not the same display file
		{
			DrawZoomRect();
		}
	}

	m_bPreviewChg = false;

	return LRESULT();
}

void CMixer2Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	int clicked = -1;

	for (int i = LAYER1; i <= MIXER; i++)
	{
		if (m_rPreviewRgn[i].PtInRect(point))
		{
			clicked = i;
			break;
		}
	}

	switch (clicked)
	{
		case LAYER1:
		case LAYER2:
		case MIXER:
		{
			DrawActivePnt(clicked);

			if (m_pIconListCtrl->GetFirstSelectedItemPosition()) //if IconList focused
				m_bPreviewChg = true;

			m_bUpdateVol = true;
			//m_EffectCombox.SetCurSel(m_pMxPrw[m_iImageID]->m_cEffect.GetEffectID());
		}
	}

	CDialogEx::OnLButtonDown(nFlags, point);

	m_pIconListCtrl->SetItemState(-1, 0, LVIS_SELECTED | LVIS_FOCUSED); //remove foucs item in list ctrl

	//click Zoom In-Out Region detect
	if (m_iImageID != -1 && m_pMxPrw[m_iImageID]->StreamExist())
	{
		CRect zoomOriRect(m_ZoomOriLTpnt, m_ZoomOriRBpnt);

		if (zoomOriRect.PtInRect(point))
		{
			m_OldMousePnt = point;
		}
	}
}


void CMixer2Dlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1 && IsWindowVisible())
	{
		if (m_iImageID != -1 && m_pMxPrw[m_iImageID]->StreamExist())
				m_pMxPrw[m_iImageID]->MixerStream().SetEffectIndex(m_EffectCombox.GetCurSel());

		if (m_pIconListCtrl->m_bListChg && m_bPreviewChg)
		{
			if (m_iImageID >= LAYER1 && m_pIconListCtrl->m_ulSelID != -1)
			{
				MediaFileInfo* pCurItem = m_pIOSourceCtrlDll->GetMediaFileInfobyID(m_pIconListCtrl->m_ulSelID);

				if (wcscmp(pCurItem->wszFileName, m_pMxPrw[m_iImageID]->m_sFileName)) //display file not equal
				{
					m_pMxPrw[m_iImageID]->m_iListIdx = m_pIconListCtrl->m_iSelIdx;
					wcscpy(m_pMxPrw[m_iImageID]->m_sFileName, pCurItem->wszFileName);

					m_bUpdateVol = true; //reset vol value
				}
			}
			m_bPreviewChg = false;
			m_pIconListCtrl->m_bListChg = false;
		}


		int ms_idx;
		static unsigned char quarter_sec = 0;
		
		for (int i = 0; i < PREVIEW_NUM; i++)
		{
			if (m_pMxPrw[i]->m_iListIdx != -1)
			{
				ms_idx = m_iStreamIndexMapping[m_pMxPrw[i]->m_iListIdx];

				if (m_bUpdateVol && i != m_iImageID && ms_idx == m_pMxPrw[m_iImageID]->m_iListIdx)
				{
					m_pMxPrw[i]->m_iListIdx = -1;
					m_pMxPrw[i]->m_bPlaying = FALSE;
					m_pMxPrw[i]->MxD3DRender()->ClearTarget();

					//------------------------------------------will cause crash to survey
					//for (int Idx = 0; Idx < 6; Idx++)
						//m_pMxPrw[m_iImageID]->MixerStream().SetEffectParam(Idx, m_pMxPrw[i]->MixerStream().GetEffectParam(Idx));
					//------------------------------------------


					m_pMxPrw[i]->InitMixerStream(NULL);
					m_pMxPrw[i]->m_pVideoSlider->SetPosV(0);
					m_pMxPrw[i]->m_sFileName[0] = L'\0';

					m_pMxPrw[m_iImageID]->m_fZoomRate = m_pMxPrw[i]->m_fZoomRate;
					m_pMxPrw[m_iImageID]->m_ZoomLTpnt = m_pMxPrw[i]->m_ZoomLTpnt;
					m_pMxPrw[m_iImageID]->m_ZoomRBpnt = m_pMxPrw[i]->m_ZoomRBpnt;
					m_pMxPrw[m_iImageID]->m_cEffect.SetEffectID(m_pMxPrw[i]->m_cEffect.GetEffectID());

					m_pMxPrw[i]->m_fZoomRate = 1.f;
					m_pMxPrw[i]->m_ZoomLTpnt = m_ZoomOriLTpnt;
					m_pMxPrw[i]->m_ZoomRBpnt = m_ZoomOriRBpnt;
					m_pMxPrw[i]->ToNoZoomSym(CRect(m_ZoomOriLTpnt, m_ZoomOriRBpnt));
					m_pMxPrw[i]->m_cEffect.SetEffectID(0); //to "No effect Index"
			
					continue;
				}

				if (g_MediaStreamAry.at(ms_idx)->IsOpened() && !g_MediaStreamAry.at(ms_idx)->SetDisplayManager(m_pMxPrw[i]->GetDispManager(), 100 + i)) //Mixer Using 100 Index
				{
					g_MediaStreamAry.at(ms_idx)->CreateDispMixer(m_pMxPrw[i]->GetD3DDevice(), m_pMxPrw[i]->GetDesktopD3DFormat(), 1920, 1080);
				}
				m_pMxPrw[i]->InitMixerStream(g_MediaStreamAry.at(ms_idx));

				m_pMxPrw[i]->m_bPlaying = TRUE;

				INT32 iWidth = g_MediaStreamAry.at(ms_idx)->GetVideoWidth();
				INT32 iHeight = g_MediaStreamAry.at(ms_idx)->GetVideoHeight();

				//for Zoom In-Out
				if (quarter_sec % 10 == 0) //up to 1 sec interval
				{
					if (/*m_iImageID != -1 &&*/ m_pMxPrw[i]->m_bPlaying)
					{
						if (m_pMxPrw[i]->m_bZmPaintBit)
						{
							m_pMxPrw[i]->MxD3DRender()->Clear();

							CRect NoZoomedSym(m_ZoomOriLTpnt, m_ZoomOriRBpnt), ZoomedSym(m_pMxPrw[i]->m_ZoomLTpnt, m_pMxPrw[i]->m_ZoomRBpnt);

							m_pMxPrw[i]->SetWH(iWidth, iHeight);
							m_pMxPrw[i]->CalImageSiteLT(NoZoomedSym, ZoomedSym);

							g_MediaStreamAry.at(ms_idx)->SetSrcArea(CRect(m_pMxPrw[i]->ImgSrc().x, m_pMxPrw[i]->ImgSrc().y, m_pMxPrw[i]->SrcImgWidth(), m_pMxPrw[i]->SrcImgHeight()));
							g_MediaStreamAry.at(ms_idx)->SetDestArea(CRect(m_pMxPrw[i]->ImgDst().x, m_pMxPrw[i]->ImgDst().y, m_pMxPrw[i]->DstImgWidth(), m_pMxPrw[i]->DstImgHeight()));
						}
					}
				}

				IDirect3DSurface9* pCurSurface = g_MediaStreamAry.at(ms_idx)->GetSurface(100 + i);
				m_pMxPrw[i]->RenderSurface(pCurSurface, iWidth, iHeight);
				
				//m_pMxPrw[i]->GetDispManager()->Clear(100 + i, iWidth, iHeight);
				
				if (quarter_sec % 2 == 0 && m_pMxPrw[i]->m_bPlaying) //1/4 sec to refresh video slider position
					m_pMxPrw[i]->m_pVideoSlider->SetPosV(int(m_pMxPrw[i]->MixerStream().GetCurPos()*100 / m_pMxPrw[i]->MixerStream().GetTotalPos()));
			}
		}
		quarter_sec++;

		if (m_bUpdateVol)
		{
			if (m_pMxPrw[m_iImageID]->StreamExist())
			{
				//update vol of system and video
				int video_vol = m_pMxPrw[m_iImageID]->MixerStream().GetVolume();
				m_pVolumeCtrl->SetPos(VolToPos(video_vol));
				m_pVolumeCtrl->Invalidate();

				int sysVol = m_pMxPrw[m_iImageID]->MixerStream().GetSystemVolume();
				m_pMxPrw[m_iImageID]->MixerStream().SetSystemVolume(sysVol);
				m_pVolumeSlider->SetPosV(sysVol);

				//update width, height info
				m_sWidth.Format("%d", m_pMxPrw[m_iImageID]->MixerStream().GetVideoWidth());
				m_sHeight.Format("%d", m_pMxPrw[m_iImageID]->MixerStream().GetVideoHeight());

				//update Zoom UI
				if (!m_pMxPrw[m_iImageID]->m_bZmPaintBit) DrawZoomRect();

				UpdateData(FALSE);

				m_EffectCombox.EnableWindow(TRUE);
			}
			else
				m_EffectCombox.EnableWindow(FALSE);

			m_EffectCombox.SetCurSel(m_pMxPrw[m_iImageID]->m_cEffect.GetEffectID());
			RefreshEffectCtrl();

			//for (int i= LEVEL_SLIDER; i<= PARAM5_SLIDER; i++)
			//::PostMessage(this->GetSafeHwnd(), i, )

			m_bUpdateVol = false;
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


BOOL CMixer2Dlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	//sample of add filter conditions to intercept calling relative event handler function at front end
	switch (message)
	{
		case WM_MIXER_VOL_NOTIFY:
		{
			if (m_iImageID == -1 || !m_pMxPrw[m_iImageID]->StreamExist())
				return false;
		}
		break;
		case WM_COMMAND:
		{
			if (wParam == IDOK && m_iImageID == -1)
				return false;
		}
		break;

		case WM_MOUSEMOVE:
		case WM_RBUTTONDOWN:
		case WM_MOUSEWHEEL: 
		{
			if (m_iImageID == -1 || !m_pMxPrw[m_iImageID]->m_bZmPaintBit)
				return false;
		}
		break;
		//case WM_LBUTTONDOWN:
			//...etc.
	}

	//play list function on Layer1, Layer2, Mixer preview

	if (m_pMxPrw[LAYER1]/*exist after OnInitDialog, before OnDestroy*/ && m_pMxPrw[LAYER1]->StreamExist())
		switch (message)
		{
			case V1_PAUSE_ITEM:
			{
				m_pMxPrw[LAYER1]->m_bPlaying = FALSE;
				m_pMxPrw[LAYER1]->MixerStream().Pause();
			}
			break;
			case V1_TO_START_ITEM:
				m_pMxPrw[LAYER1]->MixerStream().Seek(0);
				break;
			case V1_PLAY_ITEM:
			{
				m_pMxPrw[LAYER1]->m_bPlaying = TRUE;

				if (m_pMxPrw[LAYER1]->MixerStream().GetPlaybackMode() != MFPM_BACKWARD && m_pMxPrw[LAYER1]->MixerStream().GetPlaybackMode() != MFPM_PAUSE)
					m_pMxPrw[LAYER1]->MixerStream().Open(m_pMxPrw[LAYER1]->MixerStream().FilePath().GetBuffer());
					m_pMxPrw[LAYER1]->MixerStream().Play();
			}
			break;
			case V1_TO_END_ITEM:
				m_pMxPrw[LAYER1]->MixerStream().Seek(m_pMxPrw[LAYER1]->MixerStream().GetTotalPos());
			break;
			case V1_STOP_ITEM:
			{
				m_pMxPrw[LAYER1]->m_bPlaying = FALSE;

				m_pMxPrw[LAYER1]->MixerStream().Stop();
				m_pMxPrw[LAYER1]->MixerStream().Close();

				//reset slider UI
				m_pVideo1Slider->SetPosV(0);
			}
			break;
		}

	if (m_pMxPrw[LAYER2] && m_pMxPrw[LAYER2]->StreamExist())
		switch (message)
		{
			case V2_PAUSE_ITEM:
			{
				m_pMxPrw[LAYER2]->m_bPlaying = FALSE;
				m_pMxPrw[LAYER2]->MixerStream().Pause();
			}
			break;
			case V2_TO_START_ITEM:
				m_pMxPrw[LAYER2]->MixerStream().Seek(0);
				break;
			case V2_PLAY_ITEM:
			{
				m_pMxPrw[LAYER2]->m_bPlaying = TRUE;

				if (m_pMxPrw[LAYER2]->MixerStream().GetPlaybackMode() != MFPM_BACKWARD && m_pMxPrw[LAYER2]->MixerStream().GetPlaybackMode() != MFPM_PAUSE)
					m_pMxPrw[LAYER2]->MixerStream().Open(m_pMxPrw[LAYER2]->MixerStream().FilePath().GetBuffer());
					m_pMxPrw[LAYER2]->MixerStream().Play();
			}
			break;
			case V2_TO_END_ITEM:
				m_pMxPrw[LAYER2]->MixerStream().Seek(m_pMxPrw[LAYER2]->MixerStream().GetTotalPos());
			break;
			case V2_STOP_ITEM:
			{
				m_pMxPrw[LAYER2]->m_bPlaying = FALSE;

				m_pMxPrw[LAYER2]->MixerStream().Stop();
				m_pMxPrw[LAYER2]->MixerStream().Close();

				//reset slider UI
				m_pVideo2Slider->SetPosV(0);
			}
			break;
		}

	if (m_pMxPrw[MIXER] && m_pMxPrw[MIXER]->StreamExist())
		switch (message)
		{
			case V3_PAUSE_ITEM:
			{
				m_pMxPrw[MIXER]->m_bPlaying = FALSE;
				m_pMxPrw[MIXER]->MixerStream().Pause();
			}
			break;
			case V3_TO_START_ITEM:
				m_pMxPrw[MIXER]->MixerStream().Seek(0);
				break;
			case V3_PLAY_ITEM:
			{
				m_pMxPrw[MIXER]->m_bPlaying = TRUE;

				if (m_pMxPrw[MIXER]->MixerStream().GetPlaybackMode() != MFPM_BACKWARD && m_pMxPrw[MIXER]->MixerStream().GetPlaybackMode() != MFPM_PAUSE)
					m_pMxPrw[MIXER]->MixerStream().Open(m_pMxPrw[MIXER]->MixerStream().FilePath().GetBuffer());
					m_pMxPrw[MIXER]->MixerStream().Play();
			}
			break;
			case V3_TO_END_ITEM:
				m_pMxPrw[MIXER]->MixerStream().Seek(m_pMxPrw[MIXER]->MixerStream().GetTotalPos());
			break;
			case V3_STOP_ITEM:
			{
				m_pMxPrw[MIXER]->m_bPlaying = FALSE;

				m_pMxPrw[MIXER]->MixerStream().Stop();
				m_pMxPrw[MIXER]->MixerStream().Close();

				//reset slider UI
				m_pVideo3Slider->SetPosV(0);
			}
			break;
		}
	
	if (m_iImageID!=-1 && m_pMxPrw[m_iImageID]->StreamExist())
	{
		switch (message)
		{
			//Zoom In-Out function -- refresh zoom In-Out UI			

		case UP_BUTTON:
			if (m_pMxPrw[m_iImageID]->m_bZmPaintBit && m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y>(m_ZoomOriLTpnt.y + 1)) //+1 for avoid crashing while touch critical edge
			{
				DrawZoomRect();
				m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y -= 1;
				m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y -= 1;
				DrawZoomRect();
				UpdateData(FALSE);
			}
			break;
		case DOWN_BUTTON:
			if (m_pMxPrw[m_iImageID]->m_bZmPaintBit && (m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y + 1)<m_ZoomOriRBpnt.y) //+1 for avoid crashing while touch critical edge
			{
				DrawZoomRect();
				m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y += 1;
				m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y += 1;
				DrawZoomRect();
				UpdateData(FALSE);
			}
			break;
		case LEFT_BUTTON:
			if (m_pMxPrw[m_iImageID]->m_bZmPaintBit && m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x>(m_ZoomOriLTpnt.x + 1)) //+1 for avoid crashing while touch critical edge
			{
				DrawZoomRect();
				m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x -= 1;
				m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x -= 1;
				DrawZoomRect();
				UpdateData(FALSE);
			}
			break;
		case RIGHT_BUTTON:
			if (m_pMxPrw[m_iImageID]->m_bZmPaintBit && (m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x + 1)<m_ZoomOriRBpnt.x) //+1 for avoid crashing while touch critical edge
			{
				DrawZoomRect();
				m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x += 1;
				m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x += 1;
				DrawZoomRect();
				UpdateData(FALSE);
			}
			break;
		//Effec Slider
		case LEVEL_SLIDER:
		case PARAM1_SLIDER:
		case PARAM2_SLIDER:
		case PARAM3_SLIDER:
		case PARAM4_SLIDER:
		case PARAM5_SLIDER:
			{
				INT32 iParmIdx = message - LEVEL_SLIDER;
				INT32 iParmVal;

				if (message == LEVEL_SLIDER)
				{
					iParmVal = int(MAX_LEVEL_VAL*LEVEL_PERCENT);
				}
				else
				{
					iParmVal = int(MAX_PARAM_VAL*EFFEC_PERCENT(message));
				}
				m_pMxPrw[m_iImageID]->MixerStream().SetEffectParam(iParmIdx, iParmVal);
				m_sParmVal[iParmIdx].Format("%d", iParmVal);
				UpdateData(FALSE);
			}
			break;
		}
	}
	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}


void CMixer2Dlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if (m_pMxPrw[m_iImageID]->m_bZmPaintBit)
	{
		CRect zoomOriRect(m_ZoomOriLTpnt, m_ZoomOriRBpnt);

		if (zoomOriRect.PtInRect(point))
		{
			DrawZoomRect();
			m_pMxPrw[m_iImageID]->m_fZoomRate = 1.f;
			m_pMxPrw[m_iImageID]->m_ZoomLTpnt = m_ZoomOriLTpnt;
			m_pMxPrw[m_iImageID]->m_ZoomRBpnt = m_ZoomOriRBpnt;
			m_pMxPrw[m_iImageID]->ToNoZoomSym(CRect(m_ZoomOriLTpnt, m_ZoomOriRBpnt));
			DrawZoomRect();
			UpdateData(FALSE);
		}
	}

	CDialogEx::OnRButtonDown(nFlags, point);
}


void CMixer2Dlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (!(nFlags & MK_LBUTTON)) return;

	//for move zoomed image arbitrarily by clicking mouse and moving at the same time in the boundary of zoomed rectangle
	if (m_pMxPrw[m_iImageID]->StreamExist() && m_pMxPrw[m_iImageID]->m_bZmPaintBit)
	{
		CRect zoomedRect(m_pMxPrw[m_iImageID]->m_ZoomLTpnt, m_pMxPrw[m_iImageID]->m_ZoomRBpnt);

		if (zoomedRect.PtInRect(point))
		{
			DrawZoomRect();

			if (point.x != m_OldMousePnt.x)
			{
				int diff = point.x - m_OldMousePnt.x;

				if ((m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x + diff) + 1 < m_ZoomOriRBpnt.x &&
					(m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x + diff) > m_ZoomOriLTpnt.x + 1)
				{
					m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x += diff;
					m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x += diff;
				}
			}
			if (point.y != m_OldMousePnt.y)
			{
				int diff = point.y - m_OldMousePnt.y;

				if ((m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y + diff) + 1 < m_ZoomOriRBpnt.y &&
					(m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y + diff) > m_ZoomOriLTpnt.y + 1)
				{
					m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y += diff;
					m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y += diff;
				}
			}

			DrawZoomRect();

			UpdateData(FALSE);

			m_OldMousePnt = point;
		}
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CMixer2Dlg::OnOK()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	if (m_pMxPrw[m_iImageID]->m_bZmPaintBit)
	{
		UINT uCtrlid = GetFocus()->GetDlgCtrlID();

		switch (uCtrlid)
		{
			case IDC_XEDIT_ID:
			{
				DrawZoomRect();

				int ori_Xval = m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x;

#if 1
				if (UpdateData())//update  m_iEditX
				{
					CRect zoomOriRect(m_ZoomOriLTpnt, m_ZoomOriRBpnt);

					m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x = m_iXedit*zoomOriRect.Width()/_ttoi(m_sWidth) + m_ZoomOriLTpnt.x;
					m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x += (m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x - ori_Xval);	//update m_zoomRBpnt.x with the same direction offset of m_zoomLTpnt.x
				}
#else
				if (UpdateData())//update  m_zoomLTpnt.x
					m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x += (m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x - ori_Xval);	//update m_zoomRBpnt.x with the same direction offset of m_zoomLTpnt.x
#endif
				else
				{
					m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x = ori_Xval;
					UpdateData(FALSE);
				}

				DrawZoomRect();
			}
			break;
			case IDC_YEDIT_ID:
			{
				DrawZoomRect();

				int ori_Yval = m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y;

#if 1
				if (UpdateData())//update  m_iEditY
				{
					CRect zoomOriRect(m_ZoomOriLTpnt, m_ZoomOriRBpnt);

					m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y = m_iYedit*zoomOriRect.Height() / _ttoi(m_sHeight) + m_ZoomOriLTpnt.y;
					m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y += (m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y - ori_Yval);  //update m_zoomRBpnt.y with the same direction offset of m_zoomLTpnt.y
				}
#else		
				if (UpdateData())//update  m_zoomLTpnt.y
					m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y += (m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y - ori_Yval);  //update m_zoomRBpnt.y with the same direction offset of m_zoomLTpnt.y
#endif
				else
				{
					m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y = ori_Yval;
					UpdateData(FALSE);
				}

				DrawZoomRect();
			}
			break;
		}
	}
	else
		UpdateData(FALSE);

	//CDialogEx::OnOK();
}

BOOL CMixer2Dlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if (m_pMxPrw[m_iImageID]->m_bZmPaintBit)
	{
		LONG offset_H = 2;
		LONG offset_V = 2;

		CRect zoomOriRect(m_ZoomOriLTpnt, m_ZoomOriRBpnt), zoomRect(m_pMxPrw[m_iImageID]->m_ZoomLTpnt, m_pMxPrw[m_iImageID]->m_ZoomRBpnt);

		if ((zDelta > 0) && ((zoomOriRect.Width() - zoomRect.Width()) >= (zoomOriRect.Width() - offset_H * 2) || (zoomOriRect.Height() - zoomRect.Height()) >= zoomOriRect.Height() - offset_V * 2))
			return 0;
		if ((zDelta < 0) && ((zoomRect.Width() - zoomOriRect.Width()) >= 100 || (zoomRect.Height() - zoomOriRect.Height()) >= 100))
			return 0;

		DrawZoomRect();

		if ((m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x + offset_H) + 2 < m_ZoomOriRBpnt.x &&
			(m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x + offset_H) > m_ZoomOriLTpnt.x + 3 &&
			(m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y + offset_V) + 2 < m_ZoomOriRBpnt.y &&
			(m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y + offset_V) > m_ZoomOriLTpnt.y + 4)
		{
			if (zDelta > 0)
			{
				m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x += offset_H;
				m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y += offset_V;
				m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x -= offset_H;
				m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y -= offset_V;
			}
			else
			{
				m_pMxPrw[m_iImageID]->m_ZoomLTpnt.x -= offset_H;
				m_pMxPrw[m_iImageID]->m_ZoomLTpnt.y -= offset_V;
				m_pMxPrw[m_iImageID]->m_ZoomRBpnt.x += offset_H;
				m_pMxPrw[m_iImageID]->m_ZoomRBpnt.y += offset_V;
			}
		}

		m_pMxPrw[m_iImageID]->m_fZoomRate = float(zoomRect.Width()) / zoomOriRect.Width();

		DrawZoomRect();

		UpdateData(FALSE);
	}
	//return 0;
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

void CMixer2Dlg::OnSelchange()
{
	if (m_iImageID != -1)
	{
		m_pMxPrw[m_iImageID]->m_cEffect.SetEffectID(((CComboBox*)GetDlgItem(IDC_COMBO_ID))->GetCurSel());
		RefreshEffectCtrl();

		::PostMessage(this->GetSafeHwnd(), WM_PAINT, 0, 0);
	}
}

void CMixer2Dlg::RedrawDlgUICtrl()
{
	CComboBox* pEffectSelector = (CComboBox*)GetDlgItem(IDC_COMBO_ID);
	pEffectSelector->Invalidate();
	pEffectSelector->UpdateWindow();

	for (int i = 0; i < sizeof(m_ParamStatic) / sizeof(CStatic); i++)
		m_ParamStatic[i].Invalidate();
	
	m_pX_EditCtrl->Invalidate();
	m_pY_EditCtrl->Invalidate();
	m_pWidth_EditCtrl->Invalidate();
	m_pHeight_EditCtrl->Invalidate();
}
