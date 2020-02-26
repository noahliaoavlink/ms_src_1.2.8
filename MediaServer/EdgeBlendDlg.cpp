// EdgeBlendDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MediaServer.h"
#include "EdgeBlendDlg.h"
#include "afxdialogex.h"


// CEdgeBlendDlg 對話方塊

IMPLEMENT_DYNAMIC(CEdgeBlendDlg, CDialog)

CEdgeBlendDlg::CEdgeBlendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_EDGEBLEND_DLG, pParent)
{
}

CEdgeBlendDlg::~CEdgeBlendDlg()
{
}

void CEdgeBlendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEdgeBlendDlg, CDialog)
	ON_BN_CLICKED(IDC_NEXT_BTN, &CEdgeBlendDlg::OnBnClickedNextBtn)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_EBS_SET_BTN, &CEdgeBlendDlg::OnBnClickedEbsSetBtn)
	ON_BN_CLICKED(IDC_LEFT_CHK, &CEdgeBlendDlg::OnBnClickedLeftChk)
	ON_BN_CLICKED(IDC_TOP_CHK, &CEdgeBlendDlg::OnBnClickedTopChk)
	ON_BN_CLICKED(IDC_RIGHT_CHK, &CEdgeBlendDlg::OnBnClickedRightChk)
	ON_BN_CLICKED(IDC_DOWN_CHK, &CEdgeBlendDlg::OnBnClickedDownChk)
	ON_BN_CLICKED(IDC_PRE_BTN, &CEdgeBlendDlg::OnBnClickedPreBtn)
END_MESSAGE_MAP()


// CEdgeBlendDlg 訊息處理常式


BOOL CEdgeBlendDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化
	CClientDC cdc(this);
	m_MemoryDC = new CDC();
	m_MemoryDC->CreateCompatibleDC(&cdc);
	m_rectDrawArea = CRect(0, 120, 576, 444);
	m_Bitmap.CreateCompatibleBitmap(&cdc, m_rectDrawArea.Width(), m_rectDrawArea.Height());
	m_MemoryDC->SelectObject(&m_Bitmap);

	m_BlendSetting.iBlendWidth = 0;
	m_BlendSetting.iBlendHeight = 0;
	m_BlendSetting.uGridH = 0;
	m_BlendSetting.uGridV = 0;
	m_BlendSetting.iStatus = 1;

	for (int x = 0; x < 4; x++)
	{
		m_BlendSetting.iPanelIdxAry[x] = 0;

		for (int y = 0; y < 4; y++)
			m_BlendSetting.bBlendArea[x][y] = FALSE;
	}

	m_strPanelArrange = "";
	m_uCurStep = 0;
	SetStepUI(0);
	SetTimer(0, 100, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}


void CEdgeBlendDlg::OnBnClickedNextBtn()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	if (m_uCurStep < 4)
	{
		m_uCurStep++;
		SetStepUI(m_uCurStep);
	}
	else
	{
		OnOK();
		m_BlendSetting.iStatus = 2;
		for (int x = 1; x < g_PannelAry.GetCount(); x++)
			g_PannelAry.GetAt(x)->SetBlendSetting(m_BlendSetting, TRUE);

		g_PannelAry.GetAt(0)->SetBlendSetting(m_BlendSetting, TRUE);
	}
}

void CEdgeBlendDlg::OnBnClickedPreBtn()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	if (m_uCurStep > 0)
	{
		m_uCurStep--;
		SetStepUI(m_uCurStep);
	}
}

void CEdgeBlendDlg::CalcDispArea()
{
	CRect rectTemp;
	INT32 iDispIdx = 0;

	INT32 iBlendWidth = m_BlendSetting.iBlendWidth * m_rectDrawArea.Width() / 1920;
	INT32 iBlendHeight = m_BlendSetting.iBlendHeight * m_rectDrawArea.Height() / 1080;

	//if (m_BlendSetting.uHNum == m_BlendSetting.uVNum)
		rectTemp = CRect(0, 0, m_rectDrawArea.Width() / m_BlendSetting.uHNum + iBlendWidth/2,
					     m_rectDrawArea.Height() / m_BlendSetting.uVNum + iBlendHeight/2);
	/*else if (m_BlendSetting.uHNum > m_BlendSetting.uVNum)
	{
		if (m_BlendSetting.uHNum > 2)
			rectTemp = CRect(0, 0, (m_rectDrawArea.Width() - iBlendWidth) / m_BlendSetting.uHNum + iBlendWidth,
					     m_rectDrawArea.Height() / m_BlendSetting.uHNum + iBlendHeight/2);
		else
			rectTemp = CRect(0, 0, m_rectDrawArea.Width()/ m_BlendSetting.uHNum + iBlendWidth/2,
							 m_rectDrawArea.Height() / m_BlendSetting.uHNum + iBlendHeight / 2);
	}
	else
	{
		if (m_BlendSetting.uVNum > 2)
			rectTemp = CRect(0, 0, m_rectDrawArea.Width() / m_BlendSetting.uVNum + iBlendWidth/2,
						 (m_rectDrawArea.Height() - iBlendHeight) / m_BlendSetting.uVNum + iBlendHeight);
		else
			rectTemp = CRect(0, 0, m_rectDrawArea.Width() / m_BlendSetting.uVNum + iBlendWidth / 2,
						 m_rectDrawArea.Height() / m_BlendSetting.uVNum + iBlendHeight/2);
	}*/

	for (int y = 0; y < m_BlendSetting.uVNum; y++)
	{
		for (int x = 0; x < m_BlendSetting.uHNum; x++)
		{
			m_rectDispAry[iDispIdx] = rectTemp;


			m_BlendSetting.rectDispAry[m_BlendSetting.iPanelIdxAry[iDispIdx] - 1] = CRect(rectTemp.left * 1920 / m_rectDrawArea.Width(),
				rectTemp.top * 1080 / m_rectDrawArea.Height(), rectTemp.right * 1920 / m_rectDrawArea.Width(),
				rectTemp.bottom * 1080 / m_rectDrawArea.Height());

			iDispIdx++;
			rectTemp.OffsetRect(rectTemp.Width() - iBlendWidth, 0);

		}
		rectTemp.OffsetRect(-(rectTemp.Width() - iBlendWidth) * m_BlendSetting.uHNum, 0);
		rectTemp.OffsetRect(0, rectTemp.Height() - iBlendHeight);
	}
}

void CEdgeBlendDlg::SetStepUI(UINT uStep)
{

	for (int x = IDC_EBS_COMBO; x < IDC_EBS_STATIC; x++)
	{
		GetDlgItem(x)->ShowWindow(SW_HIDE);
	}
	GetDlgItem(IDC_NEXT_BTN)->ShowWindow(SW_SHOW); 
	GetDlgItem(IDC_NEXT_BTN)->EnableWindow(FALSE);
	GetDlgItem(IDC_PRE_BTN)->EnableWindow(TRUE);

	if (uStep != 4)
		GetDlgItem(IDC_NEXT_BTN)->SetWindowTextA("Next Step");
	else
		GetDlgItem(IDC_NEXT_BTN)->SetWindowTextA("Modify Point");

	if (uStep == 0)
	{
		GetDlgItem(IDC_PRE_BTN)->EnableWindow(FALSE);
		SetWindowPos(NULL, 700, 490, 555, 155, SWP_SHOWWINDOW);
		CComboBox *pCombBox = (CComboBox*)GetDlgItem(IDC_EBS_COMBO);
		pCombBox->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EBS_STATIC)->SetWindowTextA("Pannel Arrange(H x V)");

		if (pCombBox->GetCount() != 0)
		{
			INT32 iCount = pCombBox->GetCount();
			for (int x = 0; x < iCount; x++)
				pCombBox->DeleteString(0);
		}

		if (g_PannelAry.GetCount() == 3)
		{
			pCombBox->AddString("2 x 1");
			pCombBox->AddString("1 x 2");
		}
		else if (g_PannelAry.GetCount() == 4)
		{
			pCombBox->AddString("3 x 1");
			pCombBox->AddString("1 x 3");
		}
		else if (g_PannelAry.GetCount() == 5)
		{
			pCombBox->AddString("1 x 4");
			pCombBox->AddString("2 x 2");
			pCombBox->AddString("4 x 1");
		}
	}
	else if (uStep == 1)
	{
		SetWindowPos(NULL, 700, 290, 600, 500, SWP_SHOWWINDOW);
		GetDlgItem(IDC_EBS_STATIC)->SetWindowTextA("Click Pannel 1");
		
		m_iCurPannelIdx = 1;
		m_BlendSetting.uHNum = m_strPanelArrange.GetAt(0) - '0';
		m_BlendSetting.uVNum = m_strPanelArrange.GetAt(4) - '0';

		CalcDispArea();

		INT32 iDispIdx = 0;
		for (int y = 0; y < m_BlendSetting.uVNum; y++)
		{
			for (int x = 0; x < m_BlendSetting.uHNum; x++)
			{
				m_BlendSetting.bBlendArea[iDispIdx][0] = (x > 0 && m_BlendSetting.uHNum != 1);
				m_BlendSetting.bBlendArea[iDispIdx][1] = (y > 0 && m_BlendSetting.uVNum != 1);
				m_BlendSetting.bBlendArea[iDispIdx][2] = (x < m_BlendSetting.uHNum - 1 && 
					                                      m_BlendSetting.uHNum != 1);
				m_BlendSetting.bBlendArea[iDispIdx][3] = (y < m_BlendSetting.uVNum - 1 && 
					                                      m_BlendSetting.uVNum != 1);

				iDispIdx++;
			}
		}

		if (m_iCurPannelIdx < g_PannelAry.GetCount())
		{
			g_PannelAry[g_PannelSetting.iSelIdx]->Invalidate();
			g_PannelSetting.iSelIdx = m_iCurPannelIdx;
			g_PannelAry[g_PannelSetting.iSelIdx]->Invalidate();
		}
	}
	else if (uStep == 2)
	{
		SetWindowPos(NULL, 700, 290, 600, 500, SWP_SHOWWINDOW);
		GetDlgItem(IDC_EBS_STATIC)->SetWindowTextA("Set Blending Area");
		GetDlgItem(IDC_EBS_W_EDIT)->ShowWindow(SW_SHOW); 
		GetDlgItem(IDC_EBS_H_EDIT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EBS_W_STATIC)->SetWindowTextA("Width");
		GetDlgItem(IDC_EBS_H_STATIC)->SetWindowTextA("Height");
		GetDlgItem(IDC_EBS_W_STATIC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EBS_H_STATIC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EBS_SET_BTN)->ShowWindow(SW_SHOW);

		CString strTemp;
		strTemp.Format("%i", m_BlendSetting.iBlendWidth);
		GetDlgItem(IDC_EBS_W_EDIT)->SetWindowTextA(strTemp);
		strTemp.Format("%i", m_BlendSetting.iBlendHeight);
		GetDlgItem(IDC_EBS_H_EDIT)->SetWindowTextA(strTemp);
	}
	else if (uStep == 3)
	{
		SetWindowPos(NULL, 700, 290, 750, 500, SWP_SHOWWINDOW);
		GetDlgItem(IDC_EBS_STATIC)->SetWindowTextA("Blending Area");
		GetDlgItem(IDC_BLEND_GROUP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LEFT_CHK)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TOP_CHK)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RIGHT_CHK)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DOWN_CHK)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_NEXT_BTN)->EnableWindow(TRUE);

		CComboBox *pCombBox = (CComboBox*)GetDlgItem(IDC_EBS_COMBO);
		pCombBox->ShowWindow(SW_SHOW);

		if (pCombBox->GetCount() != 0)
		{
			INT32 iCount = pCombBox->GetCount();
			for (int x = 0; x < iCount; x++)
				pCombBox->DeleteString(0);
		}

		CString strTemp;
		for (int x = 0; x < g_PannelAry.GetCount() - 1; x++)
		{
			strTemp.Format("Panel %i", x + 1);
			pCombBox->AddString(strTemp);
		}
		pCombBox->SetCurSel(0);

		m_iCurPannelIdx = 1;

		if (m_iCurPannelIdx < g_PannelAry.GetCount())
		{
			g_PannelAry[g_PannelSetting.iSelIdx]->Invalidate();
			g_PannelSetting.iSelIdx = m_iCurPannelIdx;
			g_PannelAry[g_PannelSetting.iSelIdx]->Invalidate();
		}
	}
	else if (uStep == 4)
	{
		SetWindowPos(NULL, 700, 290, 750, 500, SWP_SHOWWINDOW);
		GetDlgItem(IDC_EBS_STATIC)->SetWindowTextA("Set Panel Grid Number");
		SetWindowPos(NULL, 700, 290, 600, 500, SWP_SHOWWINDOW);
		GetDlgItem(IDC_EBS_W_EDIT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EBS_H_EDIT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EBS_W_STATIC)->SetWindowTextA("Grid H");
		GetDlgItem(IDC_EBS_H_STATIC)->SetWindowTextA("Grid V");
		GetDlgItem(IDC_EBS_W_STATIC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EBS_H_STATIC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EBS_SET_BTN)->ShowWindow(SW_SHOW);

		CString strTemp;
		strTemp.Format("%i", m_BlendSetting.uGridH);
		GetDlgItem(IDC_EBS_W_EDIT)->SetWindowTextA(strTemp);
		strTemp.Format("%i", m_BlendSetting.uGridV);
		GetDlgItem(IDC_EBS_H_EDIT)->SetWindowTextA(strTemp);
	}
}

void CEdgeBlendDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	CClientDC cdc(this);
	CString strTemp;

	strTemp.Format("(%03i, %03i)", point.x, point.y);
	cdc.TextOutA(0, 0, strTemp);

	CDialog::OnMouseMove(nFlags, point);
}

void CEdgeBlendDlg::DrawMemoryDC()
{
	if (m_uCurStep > 0)
	{
		CClientDC cdc(this);
		INT32 iDispIdx = 0, iTempVal = 0;
		CRect rectArea(0, 0, m_rectDrawArea.Width(), m_rectDrawArea.Height());
		CRect rectTemp;
		CBrush brshWhite(RGB(255, 255, 255));
		CBrush brshBlack(RGB(0, 0, 0));
		CBrush brshRed(RGB(255, 0, 0));
		CPen penBlue(PS_SOLID, 1, RGB(0, 0, 255));
		CPen penBlack(PS_SOLID, 1, RGB(0, 0, 0));
		CString strTemp;
		m_MemoryDC->FillRect(rectArea, &brshWhite);

		INT32 iBlendWidth = m_BlendSetting.iBlendWidth * m_rectDrawArea.Width() / 1920;
		INT32 iBlendHeight = m_BlendSetting.iBlendHeight * m_rectDrawArea.Height() / 1080;

		for (int y = 0; y < m_BlendSetting.uVNum; y++)
		{
			for (int x = 0; x < m_BlendSetting.uHNum; x++)
			{
				m_MemoryDC->FrameRect(m_rectDispAry[iDispIdx], &brshBlack);

				for (int z = 0; z < 4; z++)
				{
					rectTemp = m_rectDispAry[iDispIdx];
					
					if (z == 0) rectTemp.right = rectTemp.left + iBlendWidth;
					else if (z == 1) rectTemp.bottom = rectTemp.top + iBlendHeight;
					else if (z == 2) rectTemp.left = rectTemp.right - iBlendWidth;
					else if (z == 3) rectTemp.top = rectTemp.bottom - iBlendHeight;

					if (m_BlendSetting.bBlendArea[iDispIdx][z])
					{
						m_MemoryDC->FillRect(rectTemp, &brshRed);
					}
				}

				m_MemoryDC->SelectObject(&penBlue);

				for (int z = 1; z < m_BlendSetting.uGridH; z++)
				{
					iTempVal = m_rectDispAry[iDispIdx].left + m_rectDispAry[iDispIdx].Width() 
						       * z / m_BlendSetting.uGridH;
					m_MemoryDC->MoveTo(iTempVal, m_rectDispAry[iDispIdx].top);
					m_MemoryDC->LineTo(iTempVal, m_rectDispAry[iDispIdx].bottom);					
				}
				for (int z = 1; z < m_BlendSetting.uGridV; z++)
				{
					iTempVal = m_rectDispAry[iDispIdx].top + m_rectDispAry[iDispIdx].Height()
						* z / m_BlendSetting.uGridV;
					m_MemoryDC->MoveTo(m_rectDispAry[iDispIdx].left, iTempVal);
					m_MemoryDC->LineTo(m_rectDispAry[iDispIdx].right, iTempVal);
				}

				m_MemoryDC->SelectObject(&penBlack);

				if (m_BlendSetting.iPanelIdxAry[iDispIdx] != 0)
				{
					strTemp.Format("%i", m_BlendSetting.iPanelIdxAry[iDispIdx]);
					m_MemoryDC->TextOutA(m_rectDispAry[iDispIdx].left + 10,
						                 m_rectDispAry[iDispIdx].top + 10, strTemp);
				}
				iDispIdx++;
			}
		}

		cdc.BitBlt(m_rectDrawArea.left, m_rectDrawArea.top, m_rectDrawArea.Width(), m_rectDrawArea.Height(),
				   m_MemoryDC, 0, 0, SRCCOPY);
	}
}

void CEdgeBlendDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此加入您的訊息處理常式程式碼
					   // 不要呼叫圖片訊息的 CDialog::OnPaint()

	DrawMemoryDC();
}


void CEdgeBlendDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (m_uCurStep == 1)
	{
		if (m_rectDrawArea.PtInRect(point))
		{
			CString strTemp;
			point -= CPoint(m_rectDrawArea.left, m_rectDrawArea.top);
			for (int x = 0; x < 4; x++)
			{
				if (m_rectDispAry[x].PtInRect(point))
				{
					m_BlendSetting.iPanelIdxAry[x] = m_iCurPannelIdx;
					m_iCurPannelIdx++;

					if (m_iCurPannelIdx >= g_PannelAry.GetCount())
						m_iCurPannelIdx = 1;

					strTemp.Format("Click Pannel %i", m_iCurPannelIdx);
					GetDlgItem(IDC_EBS_STATIC)->SetWindowTextA(strTemp);
					DrawMemoryDC();

					if (m_iCurPannelIdx < g_PannelAry.GetCount())
					{
						INT32 iPreIdx = g_PannelSetting.iSelIdx;
						g_PannelSetting.iSelIdx = m_iCurPannelIdx;
						g_PannelAry[iPreIdx]->OnPaint();
						g_PannelAry[g_PannelSetting.iSelIdx]->OnPaint();
					}
					break;
				}
			}
		}
	}
	else if (0)//m_uCurStep == 3)
	{
		if (m_rectDrawArea.PtInRect(point))
		{
			CString strTemp;
			point -= CPoint(m_rectDrawArea.left, m_rectDrawArea.top);
			for (int x = 0; x < 4; x++)
			{
				if (m_rectDispAry[x].PtInRect(point))
				{
					m_iCurPannelIdx++;

					if (m_iCurPannelIdx > 4)
						m_iCurPannelIdx = 1;

					strTemp.Format("Click Pannel %i", m_iCurPannelIdx);
					GetDlgItem(IDC_EBS_STATIC)->SetWindowTextA(strTemp);
					DrawMemoryDC();

					if (m_iCurPannelIdx < g_PannelAry.GetCount())
					{
						g_PannelAry[g_PannelSetting.iSelIdx]->Invalidate();
						g_PannelSetting.iSelIdx = m_iCurPannelIdx;
						g_PannelAry[g_PannelSetting.iSelIdx]->Invalidate();
					}
					break;
				}
			}
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}


void CEdgeBlendDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (nIDEvent == 0)
	{
		if (m_uCurStep == 0)
		{
			INT32 iIndex = ((CComboBox*)GetDlgItem(IDC_EBS_COMBO))->GetCurSel();
			((CComboBox*)GetDlgItem(IDC_EBS_COMBO))->GetLBText(iIndex, m_strPanelArrange);

			if (!m_strPanelArrange.IsEmpty())
				GetDlgItem(IDC_NEXT_BTN)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_NEXT_BTN)->EnableWindow(FALSE);

		}
		else if (m_uCurStep == 1)
		{
			INT32 iPannelNum = m_BlendSetting.uVNum * m_BlendSetting.uHNum;
			BOOL bFind = TRUE;

			for (int x = 1; x <= iPannelNum && bFind; x++)
			{
				bFind = FALSE;
				for (int y = 0; y < 4; y++)
				{
					if (m_BlendSetting.iPanelIdxAry[y] == x)
					{
						bFind = TRUE;
						break;
					}
				}
			}

			if (bFind)
				GetDlgItem(IDC_NEXT_BTN)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_NEXT_BTN)->EnableWindow(FALSE);
		}
		else if (m_uCurStep == 2)
		{
			if (m_BlendSetting.iBlendWidth < 1920 && m_BlendSetting.iBlendWidth > -960 && 
				m_BlendSetting.iBlendHeight < 1080 && m_BlendSetting.iBlendHeight > -540)
				GetDlgItem(IDC_NEXT_BTN)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_NEXT_BTN)->EnableWindow(FALSE);
		}
		else if (m_uCurStep == 3)
		{

			INT32 iIndex = ((CComboBox*)GetDlgItem(IDC_EBS_COMBO))->GetCurSel() + 1;

			if (iIndex != m_iCurPannelIdx)
			{
				m_iCurPannelIdx = iIndex;

				if (m_iCurPannelIdx < g_PannelAry.GetCount())
				{
					g_PannelAry[g_PannelSetting.iSelIdx]->Invalidate();
					g_PannelSetting.iSelIdx = m_iCurPannelIdx;
					g_PannelAry[g_PannelSetting.iSelIdx]->Invalidate();
				}

			}


			((CButton*)GetDlgItem(IDC_LEFT_CHK))->SetCheck(m_BlendSetting.bBlendArea[m_iCurPannelIdx - 1][0]);
			((CButton*)GetDlgItem(IDC_TOP_CHK))->SetCheck(m_BlendSetting.bBlendArea[m_iCurPannelIdx - 1][1]);
			((CButton*)GetDlgItem(IDC_RIGHT_CHK))->SetCheck(m_BlendSetting.bBlendArea[m_iCurPannelIdx - 1][2]);
			((CButton*)GetDlgItem(IDC_DOWN_CHK))->SetCheck(m_BlendSetting.bBlendArea[m_iCurPannelIdx - 1][3]);
		}
		else if (m_uCurStep == 4)
		{
			if (m_BlendSetting.uGridH > 1 && m_BlendSetting.uGridH < 100 && 
				m_BlendSetting.uGridV > 1 && m_BlendSetting.uGridV < 100)
				GetDlgItem(IDC_NEXT_BTN)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_NEXT_BTN)->EnableWindow(FALSE);
		}
	}

	CDialog::OnTimer(nIDEvent);
}


BOOL CEdgeBlendDlg::DestroyWindow()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	KillTimer(0);

	return CDialog::DestroyWindow();
}


void CEdgeBlendDlg::OnBnClickedEbsSetBtn()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	if (m_uCurStep == 2)
	{
		INT32 iWidth, iHeight;
		CString strTemp;

		GetDlgItem(IDC_EBS_W_EDIT)->GetWindowTextA(strTemp);
		iWidth = atoi(strTemp);
		GetDlgItem(IDC_EBS_H_EDIT)->GetWindowTextA(strTemp);
		iHeight = atoi(strTemp);

		if (iWidth < 1920 && iWidth > -960 && iHeight < 1080 && iHeight > -540)
		{
			m_BlendSetting.iBlendWidth = iWidth;
			m_BlendSetting.iBlendHeight = iHeight;

			CalcDispArea();
			DrawMemoryDC();

			for (int x = 1; x < g_PannelAry.GetCount(); x++)
				g_PannelAry.GetAt(x)->SetBlendSetting(m_BlendSetting);
		}
		else
		{
			AfxMessageBox("Width range is -960 ~ 1920, Height range is -540 ~ 1080", MB_TOPMOST);
			m_BlendSetting.iBlendWidth = m_BlendSetting.iBlendHeight = -2000;
		}

		strTemp.Format("%i", iWidth);
		GetDlgItem(IDC_EBS_W_EDIT)->SetWindowTextA(strTemp);
		strTemp.Format("%i", iHeight);
		GetDlgItem(IDC_EBS_H_EDIT)->SetWindowTextA(strTemp);
	}
	else if (m_uCurStep == 4)
	{
		INT32 iGridH, iGridV;
		CString strTemp;

		GetDlgItem(IDC_EBS_W_EDIT)->GetWindowTextA(strTemp);
		iGridH = atoi(strTemp);
		GetDlgItem(IDC_EBS_H_EDIT)->GetWindowTextA(strTemp);
		iGridV = atoi(strTemp);

		if (iGridH > 1 && iGridH < 100 && iGridV > 1 && iGridV < 100)
		{
			m_BlendSetting.uGridH = iGridH;
			m_BlendSetting.uGridV = iGridV;

			DrawMemoryDC();

			for (int x = 1; x < g_PannelAry.GetCount(); x++)
				g_PannelAry.GetAt(x)->SetBlendSetting(m_BlendSetting);
		}
		else
		{
			AfxMessageBox("Grid Number should over 1 and under 100", MB_TOPMOST);
			m_BlendSetting.uGridH = m_BlendSetting.uGridV = 0;
		}

		strTemp.Format("%i", iGridH);
		GetDlgItem(IDC_EBS_W_EDIT)->SetWindowTextA(strTemp);
		strTemp.Format("%i", iGridV);
		GetDlgItem(IDC_EBS_H_EDIT)->SetWindowTextA(strTemp);
	}
}

void CEdgeBlendDlg::SetBlendArea(INT iItemID)
{
	KillTimer(0);
	m_BlendSetting.bBlendArea[m_iCurPannelIdx - 1][iItemID - IDC_LEFT_CHK] = ((CButton*)GetDlgItem(iItemID))->GetCheck();
	DrawMemoryDC();
	for (int x = 1; x < g_PannelAry.GetCount(); x++)
		g_PannelAry.GetAt(x)->SetBlendSetting(m_BlendSetting);
	SetTimer(0, 100, NULL);
}

void CEdgeBlendDlg::OnBnClickedLeftChk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	SetBlendArea(IDC_LEFT_CHK);
}

void CEdgeBlendDlg::OnBnClickedTopChk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	SetBlendArea(IDC_TOP_CHK);
}

void CEdgeBlendDlg::OnBnClickedRightChk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	SetBlendArea(IDC_RIGHT_CHK);
}

void CEdgeBlendDlg::OnBnClickedDownChk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	SetBlendArea(IDC_DOWN_CHK);
}