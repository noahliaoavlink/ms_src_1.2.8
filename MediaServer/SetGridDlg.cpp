// SetGridDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MediaServer.h"
#include "SetGridDlg.h"
#include "afxdialogex.h"


// CSetGridDlg 對話方塊

IMPLEMENT_DYNAMIC(CSetGridDlg, CDialog)

CSetGridDlg::CSetGridDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SETGRID_DLG, pParent)
{
	m_DirectionDC = NULL;
	m_MemoryDC = NULL;
	m_iTextureIdx = 0;
	m_iSelCurvePt = -1;
	m_bClickDirectionPt = FALSE;
	m_clrGain = 0;

	for (int x = 0; x < 5; x++)
		m_uPanelGain[x] = 0;
}

CSetGridDlg::~CSetGridDlg()
{
	if (m_MemoryDC)  delete m_MemoryDC;
	if (m_DirectionDC)  delete m_DirectionDC;

	m_MemoryDC = NULL;
	m_DirectionDC = NULL;
}

void CSetGridDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetGridDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSetGridDlg::OnBnClickedOk)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_SETCURVE_BTN, &CSetGridDlg::OnBnClickedSetcurveBtn)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_SETGAIN_BTN, &CSetGridDlg::OnBnClickedSetgainBtn)
	ON_BN_CLICKED(IDC_SETGRID_BTN, &CSetGridDlg::OnBnClickedSetgridBtn)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_GAMMA_COMBO, &CSetGridDlg::OnCbnSelchangeGammaCombo)
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_CBN_SELCHANGE(IDC_PANNEL_COMBO, &CSetGridDlg::OnCbnSelchangePannelCombo)
	ON_CBN_SELCHANGE(IDC_AREA_COMBO, &CSetGridDlg::OnCbnSelchangeAreaCombo)
END_MESSAGE_MAP()


// CSetGridDlg 訊息處理常式


void CSetGridDlg::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	GetParent()->SendMessage(WM_SETGRID_CLOSE);
	CDialog::OnOK();
}


void CSetGridDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (0)
	{
		CClientDC cdc(this);
		CString strTemp;
		strTemp.Format("(%i, %i)", point.x, point.y);
		cdc.TextOutA(0, 300, strTemp);
	}

	if (m_bAddCurPt)
	{
		CPoint ptAdd = m_CurvePtAry.GetAt(m_CurvePtAry.GetCount() - 1);
		ptAdd.y = point.y - m_rectChart.top;

		if (m_CurvePtAry.GetCount() >= 3)
		{
			if (ptAdd.x < 0) ptAdd.x = 0;
			if (ptAdd.x > m_rectChart.Width()) ptAdd.x = m_rectChart.Width();
			ptAdd.x = point.x - m_rectChart.left;
		}
		else
		{
			if (ptAdd.y < 0) ptAdd.y = 0;
			if (ptAdd.y > m_rectChart.Height()) ptAdd.y = m_rectChart.Height();
		}

		m_CurvePtAry.GetAt(m_CurvePtAry.GetCount() - 1) = ptAdd;

		RedrawMemoryDC();
	}
	else if (m_CurvePtAry.GetCount() == 4)
	{
		if (m_iSelCurvePt >= 0)
		{
			CPoint ptTemp = m_CurvePtAry.GetAt(m_iSelCurvePt);
			ptTemp.y = point.y - m_rectChart.top;

			if (m_iSelCurvePt >= 2)
				ptTemp.x = point.x - m_rectChart.left;
			else
			{
				if (ptTemp.y < 0) ptTemp.y = 0;
				if (ptTemp.y > m_rectChart.Height()) ptTemp.y = m_rectChart.Height();
			}

			if (ptTemp.x < 0) ptTemp.x = 0;
			if (ptTemp.x > m_rectChart.Width()) ptTemp.x = m_rectChart.Width();

			if (m_CurvePtAry.GetAt(m_iSelCurvePt) != ptTemp)
			{
				m_CurvePtAry.GetAt(m_iSelCurvePt) = ptTemp;
				RedrawMemoryDC();
			}

		}
		else if (nFlags != MK_LBUTTON)
		{
			INT iClosePtIdx = -1;
			CPoint ptTemp;
			HCURSOR hCursor;
			HCURSOR hCurCursor;

			for (int x = 0; x < 4; x++)
			{
				ptTemp = m_CurvePtAry.GetAt(x);

				if (pow(ptTemp.x - (point.x - m_rectChart.left), 2) < 25)
				//if (pow(ptTemp.x - (point.x - m_rectChart.left), 2) + pow(ptTemp.y - (point.y - m_rectChart.top), 2) < 25)
				{
					iClosePtIdx = x;
					break;
				}
			}

			if (iClosePtIdx >= 0)
			{
				if (nFlags != MK_LBUTTON)
					hCursor = AfxGetApp()->LoadCursor(IDC_HAND_OPEN);

				hCurCursor = GetCursor();

				m_iSelCurvePt = -10;
				if (hCurCursor != hCursor)
					SetCursor(hCursor);
			}
			else
				m_iSelCurvePt = -1;

		}
	}

	if (m_bClickDirectionPt && nFlags == MK_LBUTTON)
	{
		if (SetDarkPt(point))
		{
			RedrawMemoryDC();
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}


void CSetGridDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此加入您的訊息處理常式程式碼
					   // 不要呼叫圖片訊息的 CDialog::OnPaint()
	RedrawMemoryDC();
}

void CSetGridDlg::RedrawMemoryDC()
{
	CRect rectTemp;
	GetClientRect(&rectTemp);
	CClientDC cdc(this);
	CBrush brshGray(RGB(128, 128, 128));
	CPen penGreen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen penRed(PS_SOLID, 2, RGB(255, 0, 0));
	CPen penBlue(PS_SOLID, 2, RGB(0, 0, 255));
	CPen penGray(PS_SOLID, 2, RGB(130, 130, 130));

	if (m_MemoryDC)
	{
		m_MemoryDC->FillRect(rectTemp, &brshGray);
		m_MemoryDC->SelectObject(&penGreen);
		for (int x = 0; x <= m_rectChart.Width(); x+= m_rectChart.Width()/8)
		{
			m_MemoryDC->MoveTo(x, 0);
			m_MemoryDC->LineTo(x, m_rectChart.Width());

			m_MemoryDC->MoveTo(0, x);
			m_MemoryDC->LineTo(m_rectChart.Width(), x);
		}

		m_MemoryDC->SelectObject(&penRed);

		if (m_CurvePtAry.GetCount() == 1)
		{
			m_MemoryDC->MoveTo(0, m_CurvePtAry.GetAt(0).y);
			m_MemoryDC->LineTo(m_rectChart.Width(), m_CurvePtAry.GetAt(0).y);
		}
		else if (m_CurvePtAry.GetCount() == 2)
		{
			m_MemoryDC->MoveTo(m_CurvePtAry.GetAt(0));
			m_MemoryDC->LineTo(m_CurvePtAry.GetAt(1));
		}
		else if (m_CurvePtAry.GetCount() == 3)
		{
			CPoint pt[4] = { m_CurvePtAry.GetAt(0), m_CurvePtAry.GetAt(2), m_CurvePtAry.GetAt(2), m_CurvePtAry.GetAt(1)};
			m_MemoryDC->PolyBezier(pt, 4);
		}
		else if (m_CurvePtAry.GetCount() == 4)
		{
			CPoint pt[4] = { m_CurvePtAry.GetAt(0), m_CurvePtAry.GetAt(2), m_CurvePtAry.GetAt(3), m_CurvePtAry.GetAt(1) };
			m_MemoryDC->PolyBezier(pt, 4);
		}

		cdc.BitBlt(m_rectChart.left, m_rectChart.top, m_rectChart.Width(), m_rectChart.Height(), m_MemoryDC, 0, 0, SRCCOPY);

		cdc.TextOutA(178, 20, "Color"); 
		cdc.TextOutA(180, 420, "Dark");
		cdc.TextOutA(600, 427, "Light");

		if (m_CurvePtAry.GetCount() == 4)
		{
			CBrush brshBlack(RGB(0, 0, 0));
			CRect rectPt;

			for (int x = 0; x < 4; x++)
			{
				rectPt = CRect(m_CurvePtAry.GetAt(x).x - 2 + m_rectChart.left, m_CurvePtAry.GetAt(x).y - 2 + m_rectChart.top,
							   m_CurvePtAry.GetAt(x).x + 2 + m_rectChart.left, m_CurvePtAry.GetAt(x).y + 2 + m_rectChart.top);

				if (rectPt.left < m_rectChart.left)
					rectPt.OffsetRect(m_rectChart.left - rectPt.left, 0);
				if (rectPt.right > m_rectChart.right)
					rectPt.OffsetRect(m_rectChart.right - rectPt.right, 0);
				if (rectPt.top < m_rectChart.top)
					rectPt.OffsetRect(0, m_rectChart.top - rectPt.top);
				if (rectPt.bottom > m_rectChart.bottom)
					rectPt.OffsetRect(0, m_rectChart.bottom - rectPt.bottom);

				cdc.FillRect(&rectPt, &brshBlack);
			}
		}
	}

	if (m_DirectionDC)
	{
		CRgn rgnTemp, rgnCircle;
		COLORREF clrPreBK;
		CRect rectArea(0, 0, m_rectDirection.Width(), m_rectDirection.Height());
		CBrush bushWhite(RGB(255, 255, 255)), bushBlack(RGB(0, 0, 0)), bushBlue(RGB(0, 0, 255));
		CBrush brushBackGround(RGB(240, 240, 240)), burshCircle;
		rgnTemp.CreateEllipticRgn(0, 0, 12, 12);
		rgnCircle.CreateEllipticRgn(0, 0, 100, 100);
		burshCircle.CreateHatchBrush(HS_CROSS, RGB(60, 0, 0));

		clrPreBK = m_DirectionDC->SetBkColor(RGB(150, 200, 150));
		m_DirectionDC->FillRect(&rectArea, &brushBackGround);
		m_DirectionDC->FillRgn(&rgnCircle, &burshCircle);
		m_DirectionDC->FrameRgn(&rgnCircle, &bushBlue, 1, 1);
		m_DirectionDC->SelectObject(&penGray);
		m_DirectionDC->MoveTo(CPoint(50, 50));
		m_DirectionDC->LineTo(m_ptDark);

		rgnTemp.OffsetRgn(44, 44);
		m_DirectionDC->FillRgn(&rgnTemp, &bushWhite);
		rgnTemp.OffsetRgn(m_ptDark.x - 50, m_ptDark.y - 50);
		m_DirectionDC->FillRgn(&rgnTemp, &bushBlack);

		cdc.BitBlt(m_rectDirection.left, m_rectDirection.top, m_rectDirection.Width(), m_rectDirection.Height(), 
			       m_DirectionDC, 0, 0, SRCCOPY);
		cdc.TextOutA(m_rectDirection.left, m_rectDirection.top - 20, "Blending Direction");

		m_DirectionDC->SetBkColor(clrPreBK);
		rgnCircle.DeleteObject();
		burshCircle.DeleteObject();
		rgnTemp.DeleteObject();
	}
}


BOOL CSetGridDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化

	CRect rectTemp;
	GetClientRect(&rectTemp);
	CClientDC cdc(this);
	m_rectChart = CRect(220, 20, 620, 420);
	m_rectDirection = CRect(630, 170, 730, 270);
	m_ptDark = CPoint(6, 50);

	for (int x = 0; x < 5; x++)
	for (int y = 0; y < MAX_PTAREA; y++)
	{
		m_ptDarkTable[x][y] = CPoint(6, 50);
	}

	if (m_MemoryDC == NULL)
	{
		m_MemoryDC = new CDC();
		m_MemoryDC->CreateCompatibleDC(&cdc);
		m_Bitmap.CreateCompatibleBitmap(&cdc, m_rectChart.Width() + 1, m_rectChart.Height() + 1);
		m_MemoryDC->SelectObject(&m_Bitmap);
	}

	if (m_DirectionDC == NULL)
	{
		m_DirectionDC = new CDC();
		m_DirectionDC->CreateCompatibleDC(&cdc);
		m_DirectionBMP.CreateCompatibleBitmap(&cdc, m_rectDirection.Width(), m_rectDirection.Height());
		m_DirectionDC->SelectObject(&m_DirectionBMP);
	}

	m_bAddCurPt = FALSE;

	/*pGammaComb->AddString("0.455");
	pGammaComb->AddString("0.476");
	pGammaComb->AddString("0.5");
	pGammaComb->AddString("0.526");
	pGammaComb->AddString("0.556");
	pGammaComb->AddString("1");
	pGammaComb->AddString("1.8");
	pGammaComb->AddString("1.9");
	pGammaComb->AddString("2.0");
	pGammaComb->AddString("2.1");
	pGammaComb->AddString("2.2");	
	*/

	CComboBox* pTempComb = (CComboBox*)GetDlgItem(IDC_GAMMA_COMBO);
	CString strTemp;

	for (int x = 4; x <= 10; x++)
	{
		strTemp.Format("%.1f", double(x) / 10);
		pTempComb->AddString(strTemp);
	}

	pTempComb = (CComboBox*)GetDlgItem(IDC_PANNEL_COMBO);
	for (int x = 1; x <= g_PannelAry.GetCount(); x++)
	{
		strTemp.Format("%i", x);
		pTempComb->AddString(strTemp);
	}


	((CButton*)GetDlgItem(IDC_VIDEO_RADIO))->SetCheck(BST_CHECKED);
	SetTimer(0, 100, NULL);

	LoadPtAry();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}


void CSetGridDlg::OnBnClickedSetcurveBtn()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	if (m_CurvePtAry.GetCount() < 4)
	{
		m_CurvePtAry.RemoveAll();

		/*m_bAddCurPt = TRUE;
		CPoint ptTemp(0, m_rectChart.Height());

		m_CurvePtAry.Add(ptTemp);
		ptTemp = CPoint(m_rectChart.Width(), 0);
		m_CurvePtAry.Add(ptTemp);
		ptTemp = CPoint(0, m_rectChart.Height());
		m_CurvePtAry.Add(ptTemp);*/

		m_CurvePtAry.Add(CPoint(0, 332));
		m_CurvePtAry.Add(CPoint(400, 0));
		m_CurvePtAry.Add(CPoint(113, 152));
		m_CurvePtAry.Add(CPoint(183, 13));
		PtToAry(TRUE);
	}
	else
	{
		PtToAry(TRUE);
	}
}

void CSetGridDlg::PtToAry(BOOL bNeedSave)
{
	if (m_MemoryDC)
	{
		RedrawMemoryDC();
		DOUBLE dCurveVal;
		for (int x = 0; x < 100; x++)
		{
			m_ColorAry[x] = 0;
			for (int y = 0; y <= m_rectChart.Height(); y++)
			{
				COLORREF clrTemp = m_MemoryDC->GetPixel(x *m_rectChart.Width() / 99, m_rectChart.Height() - y);

				if (GetRValue(clrTemp) == 255 && GetGValue(clrTemp) == 0 && GetBValue(clrTemp) == 0) // Curve Color
				{
					dCurveVal = DOUBLE(y) / m_rectChart.Height();

					if (dCurveVal > 1.0f)
						dCurveVal = 1.0f;
					else if (dCurveVal < 0.f)
						dCurveVal = 0.f;

					m_ColorAry[x] = dCurveVal;
					break;
				}
			}
		}

		if (bNeedSave)
		{
			m_CurvePtTable[m_iSelPannel][m_iSelArea].RemoveAll();

			for (int x = 0; x < m_CurvePtAry.GetCount(); x++)
			{
				m_CurvePtTable[m_iSelPannel][m_iSelArea].Add(m_CurvePtAry.GetAt(x));
			}

			SavePtAry();
			GetParent()->SendMessage(WM_SETCURVE, m_iSelPannel, m_iSelArea);
		}
	}
}


void CSetGridDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (m_bAddCurPt)
	{
		CPoint ptAdd;
		if (m_CurvePtAry.GetCount() == 1)
		{
			ptAdd.y = point.y - m_rectChart.top;
			ptAdd.x = m_rectChart.Width();

			if (ptAdd.y < 0) ptAdd.y = 0;
			if (ptAdd.y > m_rectChart.Height()) ptAdd.y = m_rectChart.Height();

			m_CurvePtAry.Add(ptAdd);
		}
		else if (m_CurvePtAry.GetCount() < 4)
		{
			ptAdd.y = point.y - m_rectChart.top;
			ptAdd.x = point.x - m_rectChart.left;

			if (ptAdd.x < 0) ptAdd.x = 0;
			if (ptAdd.x > m_rectChart.Width()) ptAdd.x = m_rectChart.Width();
			if (ptAdd.y < 0) ptAdd.y = 0;
			if (ptAdd.y > m_rectChart.Height()) ptAdd.y = m_rectChart.Height();

			m_CurvePtAry.Add(ptAdd);
		}
		else if (m_CurvePtAry.GetCount() == 4)
		{
			m_bAddCurPt = FALSE;

			PtToAry(TRUE);
		}
	}
	else if (m_CurvePtAry.GetCount() == 4)
	{
		CPoint ptTemp;

		for (int x = 0; x < 4; x++)
		{
			ptTemp = m_CurvePtAry.GetAt(x);

			//if (pow(ptTemp.x - (point.x - m_rectChart.left), 2) < 25)
			if (pow(ptTemp.x - (point.x - m_rectChart.left), 2) + pow(ptTemp.y - (point.y - m_rectChart.top), 2) < 25)
			{
				m_iSelCurvePt = x;
				break;
			}
		}

		if (m_iSelCurvePt >= 0)
		{
			HCURSOR hCursor;
			HCURSOR hCurCursor;

			hCursor = AfxGetApp()->LoadCursor(IDC_HAND_CLOSE);
			hCurCursor = GetCursor();

			if (hCurCursor != hCursor)
				SetCursor(hCursor);

			SetCapture();
		}
	}

	if (m_rectDirection.PtInRect(point))
	{
		if (SetDarkPt(point))
		{
			m_bClickDirectionPt = TRUE;
			SetCapture();
			RedrawMemoryDC();
		}
	}
	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CSetGridDlg::SetDarkPt(CPoint ptMouse)
{
	BOOL bResult = FALSE;

	INT32 iOldLength, iNewLength = 44;
	CPoint ptDirtAreaCenter(m_rectDirection.left + m_rectDirection.Width() / 2,
		m_rectDirection.top + m_rectDirection.Height() / 2);

	DOUBLE dDegree;
	CPoint ptVector = ptMouse - ptDirtAreaCenter;

	iOldLength = pow(pow(ptVector.x, 2) + pow(ptVector.y, 2), 0.5);

	if (iOldLength > 0)
	{
		m_ptDark = CPoint(50 + ptVector.x *iNewLength / iOldLength, 50 + ptVector.y *iNewLength / iOldLength);
		bResult = TRUE;

		m_ptDarkTable[m_iSelPannel][m_iSelArea] = m_ptDark;
	}

	return bResult;
}


void CSetGridDlg::OnBnClickedSetgainBtn()
{
	// TODO: 在此加入控制項告知處理常式程式碼


	CColorDialog colorDlg;

	if (colorDlg.DoModal() == IDOK)
	{
		m_clrGain = UINT(colorDlg.GetColor());
		m_uPanelGain[m_iSelPannel] = m_clrGain;
		GetParent()->SendMessage(WM_SETGAIN, m_iSelPannel);
		PtToAry(TRUE);
	}
}


void CSetGridDlg::OnBnClickedSetgridBtn()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	CString strTemp;
	GetDlgItem(IDC_GRIDX_EDIT)->GetWindowTextA(strTemp);
	m_iGridNumX = atoi(strTemp);
	GetDlgItem(IDC_GRIDY_EDIT)->GetWindowTextA(strTemp);
	m_iGridNumY = atoi(strTemp);

	if (m_iGridNumX <= 0 || m_iGridNumX > 500 || m_iGridNumY <= 0 || m_iGridNumY > 500)
	{
		AfxMessageBox("Grid Number Range is 1 to 500", MB_TOPMOST);
		return;
	}
	else
	{
		GetParent()->SendMessage(WM_SETGRID, m_iSelPannel);
	}
}


void CSetGridDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (nIDEvent == 0)
	{
		static INT32 iPreIdx = 0;

		CString strTemp;
		CComboBox* pTempComb = (CComboBox*)GetDlgItem(IDC_PANNEL_COMBO);

		if (pTempComb->GetCount() == 0 && IsWindowVisible())
		{
			m_iSelArea = 0;
			if (g_PannelAry.GetCount() == 1)
			{
				pTempComb->AddString("0");
				m_iSelPannel = 0;
			}
			else
			{
				for (int x = 1; x < g_PannelAry.GetCount(); x++)
				{
					strTemp.Format("%i", x);
					pTempComb->AddString(strTemp);
				}
				m_iSelPannel = 1;
			}

			pTempComb->SetCurSel(0);

			pTempComb = (CComboBox*)GetDlgItem(IDC_AREA_COMBO);
			for (int x = 1; x <= 6; x++)
			{
				strTemp.Format("%i", x);
				pTempComb->AddString(strTemp);
			}

			for (int x = 0; x < g_PannelAry.GetCount(); x++)
			{
				if (g_PannelAry.GetCount() > 1 && x == 0)
					continue;

				GetParent()->SendMessage(WM_INIT2DMAPPING, x, 0);
				GetParent()->SendMessage(WM_PRESETGAIN, x, 0);
				for (int y = 0; y < MAX_PTAREA; y++)
				{
					if (m_CurvePtTable[x][y].GetCount() > 0)
					{
						m_CurvePtAry.RemoveAll();

						for (int z = 0; z < m_CurvePtTable[x][y].GetCount(); z++)
						{
							m_CurvePtAry.Add(m_CurvePtTable[x][y].GetAt(z));
						}

						m_ptDark = m_ptDarkTable[x][y];
						PtToAry(FALSE);
						GetParent()->SendMessage(WM_SETCURVE, x, y + 100);
					}
				}
				GetParent()->SendMessage(WM_INIT2DMAPPING, x, 1);
			}
			GetParent()->SendMessage(WM_INIT2DMAPPING, 0, 2);

			pTempComb->SetCurSel(0);
			OnCbnSelchangePannelCombo();
		}

		m_iTextureIdx = 0;
		for (int x = IDC_VIDEO_RADIO; x <= IDC_BLUE_RADIO; x++)
		{
			if (((CButton*)GetDlgItem(x))->GetCheck() == BST_CHECKED)
			{
				m_iTextureIdx = x - IDC_VIDEO_RADIO;
			}

		}

		if (iPreIdx != m_iTextureIdx)
		{
			iPreIdx = m_iTextureIdx;
			GetParent()->SendMessage(WM_SETTEXTURE, m_iSelPannel, m_iTextureIdx);
		}
	}

	CDialog::OnTimer(nIDEvent);
}


void CSetGridDlg::OnCbnSelchangeGammaCombo()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CComboBox* pGammaComb = (CComboBox*)GetDlgItem(IDC_GAMMA_COMBO);
	CString strGammaVal;
	pGammaComb->GetLBText(pGammaComb->GetCurSel(), strGammaVal);

	if (!strGammaVal.IsEmpty())
	{
		m_uGammaVal = UINT(atof(strGammaVal) * 100);
		GetParent()->SendMessage(WM_SETGAMMA, m_iSelPannel);
	}
}


void CSetGridDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	m_iSelCurvePt = -1;
	ReleaseCapture();
	m_bClickDirectionPt = FALSE;

	CDialog::OnLButtonUp(nFlags, point);
}


BOOL CSetGridDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	HCURSOR hCursor;
	HCURSOR hCurCursor;
	if (m_iSelCurvePt >= 0)
	{
		hCursor = AfxGetApp()->LoadCursor(IDC_HAND_CLOSE);
		RedrawMemoryDC();

		hCurCursor = GetCursor();

		if (hCurCursor != hCursor)
			SetCursor(hCursor);

		return TRUE;
	}
	else if (m_iSelCurvePt == -10)
	{
		hCursor = AfxGetApp()->LoadCursor(IDC_HAND_OPEN);
		hCurCursor = GetCursor();

		m_iSelCurvePt = -10;
		if (hCurCursor != hCursor)
			SetCursor(hCursor);
	}
	else
		return CDialog::OnSetCursor(pWnd, nHitTest, message);
}


void CSetGridDlg::OnCbnSelchangePannelCombo()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	CComboBox* pTempComb = (CComboBox*)GetDlgItem(IDC_PANNEL_COMBO);
	CString strTemp;
	pTempComb->GetLBText(pTempComb->GetCurSel(), strTemp);
	m_iSelPannel = atoi(strTemp);

	pTempComb = (CComboBox*)GetDlgItem(IDC_AREA_COMBO);
	pTempComb->GetLBText(pTempComb->GetCurSel(), strTemp);
	m_iSelArea = atoi(strTemp) - 1;

	m_CurvePtAry.RemoveAll();

	for (int x = 0; x < m_CurvePtTable[m_iSelPannel][m_iSelArea].GetCount(); x++)
	{
		m_CurvePtAry.Add(m_CurvePtTable[m_iSelPannel][m_iSelArea].GetAt(x));
	}

	m_ptDark = m_ptDarkTable[m_iSelPannel][m_iSelArea];
	m_bAddCurPt = FALSE;
	RedrawMemoryDC();
}


void CSetGridDlg::OnCbnSelchangeAreaCombo()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	CComboBox* pTempComb = (CComboBox*)GetDlgItem(IDC_AREA_COMBO);
	CString strTemp;
	pTempComb->GetLBText(pTempComb->GetCurSel(), strTemp);
	m_iSelArea = atoi(strTemp) - 1;

	m_CurvePtAry.RemoveAll();

	for (int x = 0; x < m_CurvePtTable[m_iSelPannel][m_iSelArea].GetCount(); x++)
	{
		m_CurvePtAry.Add(m_CurvePtTable[m_iSelPannel][m_iSelArea].GetAt(x));
	}

	m_ptDark = m_ptDarkTable[m_iSelPannel][m_iSelArea];

	m_bAddCurPt = FALSE;
	RedrawMemoryDC();
}

void CSetGridDlg::SavePtAry()
{
	CStdioFile sfFile;
	CString strData;
	CPoint ptTemp;

	sfFile.Open(theApp.m_strCurPath + "\\TempShape\\PtAry.map", CFile::modeCreate | CFile::modeReadWrite);

	// m_CurvePtTable[5][MAX_PTAREA]

	for (int x = 0; x < 5; x++)
	{
		strData.Format("G:%i,%i\n", x, m_uPanelGain[x]);
		sfFile.WriteString(strData);

		for (int y = 0; y < MAX_PTAREA; y++)
		{
			for (int iPtIdx = 0; iPtIdx < m_CurvePtTable[x][y].GetCount(); iPtIdx++)
			{
				if (iPtIdx == 0)
				{
					ptTemp = m_ptDarkTable[x][y];
					strData.Format("==%i,%i,%i,%i\n", x, y, ptTemp.x, ptTemp.y);
					sfFile.WriteString(strData);
				}

				ptTemp = m_CurvePtTable[x][y].GetAt(iPtIdx);
				strData.Format("%i,%i,%i,%i\n", x, y, ptTemp.x, ptTemp.y);
				sfFile.WriteString(strData);
			}
		}
	}

	sfFile.Close();
}

void CSetGridDlg::LoadPtAry()
{
	CStdioFile sfFile;
	CString strData, strTemp;
	CPoint ptTemp;
	CFileFind ffFinder;
	INT32 iPannelIdx, iAreaIdx, iStrPos;

	if (!ffFinder.FindFile(theApp.m_strCurPath + "\\TempShape\\PtAry.map"))
		return;

	for (int x = 0; x < 5; x++)
	for (int y = 0; y < MAX_PTAREA; y++)
	{
		m_CurvePtTable[x][y].RemoveAll();
	}

	sfFile.Open(theApp.m_strCurPath + "\\TempShape\\PtAry.map", CFile::modeRead);

	while (sfFile.GetPosition() < sfFile.GetLength())
	{
		sfFile.ReadString(strData);

		iStrPos = 0;

		if (strData.Left(2) == "G:")
		{
			strData.Replace("G:", "");
			strTemp = strData.Tokenize(",", iStrPos);
			iPannelIdx = atoi(strTemp);

			strTemp = strData.Tokenize(",", iStrPos);
			m_uPanelGain[iPannelIdx] = atoi(strTemp);
		}
		else if (strData.Left(2) == "==")
		{
			strData.Replace("==", "");
			strTemp = strData.Tokenize(",", iStrPos);
			iPannelIdx = atoi(strTemp);

			strTemp = strData.Tokenize(",", iStrPos);
			iAreaIdx = atoi(strTemp);

			strTemp = strData.Tokenize(",", iStrPos);
			ptTemp.x = atoi(strTemp);

			strTemp = strData.Tokenize(",", iStrPos);
			ptTemp.y = atoi(strTemp);

			m_ptDarkTable[iPannelIdx][iAreaIdx] = ptTemp;

		}
		else
		{
			strTemp = strData.Tokenize(",", iStrPos);
			iPannelIdx = atoi(strTemp);

			strTemp = strData.Tokenize(",", iStrPos);
			iAreaIdx = atoi(strTemp);

			strTemp = strData.Tokenize(",", iStrPos);
			ptTemp.x = atoi(strTemp);

			strTemp = strData.Tokenize(",", iStrPos);
			ptTemp.y = atoi(strTemp);

			m_CurvePtTable[iPannelIdx][iAreaIdx].Add(ptTemp);
		}

	}
}

void CSetGridDlg::OnCancel()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	GetParent()->SendMessage(WM_SETGRID_CLOSE);
	CDialog::OnOK();

	CDialog::OnCancel();
}

void CSetGridDlg::CreateDefultGama()
{
	CString strTemp;
	CComboBox* pTempComb = (CComboBox*)GetDlgItem(IDC_PANNEL_COMBO);

	if (!IsWindowVisible())
		ShowWindow(SW_SHOW);

	KillTimer(0);

	if (pTempComb->GetCount() == 0)
	{
		m_iSelArea = 0;
		if (g_PannelAry.GetCount() == 1)
		{
			pTempComb->AddString("0");
			m_iSelPannel = 0;
		}
		else
		{
			for (int x = 1; x < g_PannelAry.GetCount(); x++)
			{
				strTemp.Format("%i", x);
				pTempComb->AddString(strTemp);
			}
			m_iSelPannel = 1;
		}

		pTempComb->SetCurSel(0);

		pTempComb = (CComboBox*)GetDlgItem(IDC_AREA_COMBO);
		for (int x = 1; x <= 6; x++)
		{
			strTemp.Format("%i", x);
			pTempComb->AddString(strTemp);
		}
	}

	for (int x = 0; x < 5; x++)
	for (int y = 0; y < MAX_PTAREA; y++)
	{
		m_CurvePtTable[x][y].RemoveAll();
	}

	for (int x = 0; x < g_PannelAry.GetCount(); x++)
	{
		if (g_PannelAry.GetCount() > 1 && x == 0)
			continue;

		m_iSelPannel = x;

		m_uPanelGain[x] = 0;
		CPoint ptCurveAry[4] = {CPoint(0, 400), CPoint(400, 0), CPoint(113, 152), CPoint(183, 13)};
		CPoint ptDarkAry[4] = { CPoint(6, 50), CPoint(50, 6), CPoint(94, 50), CPoint(50, 94) };

		for (int y = 0; y < 4; y++)
		{
			m_ptDarkTable[x][y] = ptDarkAry[y];

			for (int z = 0; z < 4; z++)
				m_CurvePtTable[x][y].Add(ptCurveAry[z]);
		}

		for (int y = 0; y < MAX_PTAREA; y++)
		{
			m_iSelArea = y;
			if (m_CurvePtTable[x][y].GetCount() > 0)
			{
				m_CurvePtAry.RemoveAll();

				for (int z = 0; z < m_CurvePtTable[x][y].GetCount(); z++)
				{
					m_CurvePtAry.Add(m_CurvePtTable[x][y].GetAt(z));
				}

				m_ptDark = m_ptDarkTable[x][y];
				PtToAry(TRUE);
			}
		}
	}

	pTempComb->SetCurSel(0);
	OnCbnSelchangePannelCombo();

	SetTimer(0, 100, NULL);
}
