// RadioMenuDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MediaServer.h"
#include "RadioMenuDlg.h"
#include "afxdialogex.h"


// CRadioMenuDlg 對話方塊

IMPLEMENT_DYNAMIC(CRadioMenuDlg, CDialog)

CRadioMenuDlg::CRadioMenuDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TEMP_DLG, pParent)
{

}

CRadioMenuDlg::~CRadioMenuDlg()
{
}

void CRadioMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRadioMenuDlg, CDialog)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CRadioMenuDlg 訊息處理常式


BOOL CRadioMenuDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
#define WS_EX_LAYERED 0x00080000
	SetWindowLong(this->m_hWnd, GWL_EXSTYLE, GetWindowLong(this->m_hWnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);

	// TODO:  在此加入額外的初始化	
	
	//SetLayeredWindowAttributes(0, 150, LWA_ALPHA);
	m_FramePic = new CMenu_Class(this, m_hWnd, BAR_MENU, theApp.m_strCurPath + "\\UI Folder\\Menu Radio\\Bar-01.bmp");
	CItem_Class* pItem;
	pItem = new CItem_Class(this, m_hWnd, MEDIA_ITEM, theApp.m_strCurPath + "\\UI Folder\\Menu Radio\\F01.bmp", true, true, true);
	pItem->OffsetObject(CPoint(53, 0));
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, MIXER_ITEM, theApp.m_strCurPath + "\\UI Folder\\Menu Radio\\F02.bmp", true, true, true);
	pItem->OffsetObject(CPoint(153, 0));
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, SHAPE_ITEM, theApp.m_strCurPath + "\\UI Folder\\Menu Radio\\F03.bmp", true, true, true);
	pItem->OffsetObject(CPoint(256, 0));
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, TLINE_ITEM, theApp.m_strCurPath + "\\UI Folder\\Menu Radio\\F04.bmp", true, true, true);
	pItem->OffsetObject(CPoint(358, 0));
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, OUTPT_ITEM, theApp.m_strCurPath + "\\UI Folder\\Menu Radio\\F05.bmp", true, true, true);
	pItem->OffsetObject(CPoint(465, 0));
	m_FramePic->AddItem(pItem);
	pItem = new CItem_Class(this, m_hWnd, SYSTM_ITEM, theApp.m_strCurPath + "\\UI Folder\\Menu Radio\\F06.bmp", true, true, true);
	pItem->OffsetObject(CPoint(547, 0));
	m_FramePic->AddItem(pItem);

	m_FramePic->Initialize();

	CRect rectTemp;
	rectTemp = g_DispArys.GetAt(0);
	INT32 iWidth = rectTemp.Width();
	INT32 iHeight = rectTemp.Height();

	m_FramePic->OffsetObject(CPoint(626,985));

	if (iWidth != 1920 || iHeight != 1080)
		m_FramePic->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);

	CRgn NullRgn;
	NullRgn.CreateRectRgn(0, 0, 0, 0);
	m_FramePic->ResetEffectRegion();
	m_FramePic->SetDlgRegion(this, FALSE);
	m_FramePic->SetWindowRgn(NullRgn, true);
	m_FramePic->ShowWindow(SW_HIDE);
	m_FramePic->EnableMenuMove(FALSE);

	SetLayeredWindowAttributes(0, 70, LWA_ALPHA);
	m_DotPic = new CMenu_Class(this, m_hWnd, DOT_MENU, theApp.m_strCurPath + "\\UI Folder\\Menu Radio\\1.bmp");
	m_DotPic->Initialize();
	m_DotPic->OffsetObject(CPoint(935, 1055));
	m_DotPic->SetDlgRegion(this, FALSE);
	m_DotPic->SetWindowRgn(NullRgn, true);
	m_DotPic->EnableMenuMove(FALSE);
	m_bShowMenu = FALSE;

	if (iWidth != 1920 || iHeight != 1080)
	{
		m_FramePic->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);
		m_DotPic->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);
	}

	SetTimer(0, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CRadioMenuDlg::ShowMenu(BOOL bShowMenu)
{
	if (bShowMenu && !m_bShowMenu)
	{
		SetLayeredWindowAttributes(0, 150, LWA_ALPHA);
		m_FramePic->ShowWindow(SW_SHOW);
		m_FramePic->SetDlgRegion(this, FALSE);
		m_DotPic->ShowWindow(SW_HIDE);
		m_bShowMenu = TRUE;
	}
	else if (!bShowMenu && m_bShowMenu)
	{
		SetLayeredWindowAttributes(0, 70, LWA_ALPHA);
		m_DotPic->ShowWindow(SW_SHOW);
		m_DotPic->SetDlgRegion(this, FALSE);
		m_FramePic->ShowWindow(SW_HIDE);
		m_bShowMenu = FALSE;
	}
}


BOOL CRadioMenuDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別

	switch (message)
	{
		case MEDIA_ITEM:
		case MIXER_ITEM:
		case SHAPE_ITEM:
		case TLINE_ITEM:
		case OUTPT_ITEM:
		case SYSTM_ITEM:
		{
			GetParent()->SendMessage(message, wParam, lParam);
		}
		break;
	}

	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}


void CRadioMenuDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	CDialog::OnMouseMove(nFlags, point);
}


void CRadioMenuDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (nIDEvent == 0)
	{
		CPoint ptCursor;
		GetCursorPos(&ptCursor);
		if (m_bShowMenu && !m_FramePic->GetRegion()->PtInRegion(ptCursor))
		{
			KillTimer(0);
			ShowMenu(FALSE);
			SetTimer(0, 100, NULL);
		}
		else if (!m_bShowMenu && m_DotPic->GetRegion()->PtInRegion(ptCursor))
		{
			KillTimer(0);
			ShowMenu(TRUE);
			SetTimer(0, 100, NULL);
		}
	}

	CDialog::OnTimer(nIDEvent);
}


void CRadioMenuDlg::OnDestroy()
{
	CDialog::OnDestroy();
	// TODO: 在此加入您的訊息處理常式程式碼
	KillTimer(0);
}


void CRadioMenuDlg::OnCancel()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	return;
	CDialog::OnCancel();
}


void CRadioMenuDlg::OnOK()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	return;
	CDialog::OnOK();
}
