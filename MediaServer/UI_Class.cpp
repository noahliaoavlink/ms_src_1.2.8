// UI_Class.cpp : ��@��
//

#include "stdafx.h"
#include "UI_Class.h"

// CUI_Class

IMPLEMENT_DYNAMIC(CUI_Class, CButton)

BEGIN_MESSAGE_MAP(CUI_Class, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_CONTROL_REFLECT(BN_CLICKED, &CUI_Class::OnBnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, &CUI_Class::OnBnDoubleclicked)
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

CUI_Class::CUI_Class()
: m_pImage(NULL)
, m_cliprgn(NULL)
, m_strFileName(_T(""))
, m_ObjectOffset(CPoint(0,0))
, m_ImageOffset(CPoint(0,0))
, m_fScale_Width(1.0)
, m_fScale_Height(1.0)
, m_BGEraseColor(Color(255, 255, 255))
, m_MemoryDC(NULL)
, m_pGraphics(NULL)
, m_SacleRgn(NULL)
, m_bHaveInitialized(false)
, m_bPassiveDraw(false)
{
	m_pUTipDll = 0;
}

CUI_Class::~CUI_Class()
{
	if(m_pImage)    delete m_pImage;
	if(m_MemoryDC)  delete m_MemoryDC;
	if(m_pGraphics) delete m_pGraphics;
	if(m_SacleRgn)  delete m_SacleRgn;

	m_pImage = NULL;
	m_MemoryDC = NULL;
	m_pGraphics = NULL;
	m_SacleRgn = NULL;
}

void CUI_Class::SetUTipDll(UTipDll* pObj, CWnd* pParetnWnd)
{
	m_pUTipDll = pObj;
	m_ParentWnd = pParetnWnd;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     IsPtInside  
// INPUT:        CurrentPt(��m)
// OUTPUT:       BOOL(�O�_�b�䤤)
// Author:       Jack Chen
// DESCRIPTION:  �P�_��J����m�O�_�b��UI����
//////////////////////////////////////////////////////////////////////////////
BOOL CUI_Class::IsPtInside(CPoint CurrentPt)
{
	return m_region.PtInRegion(CurrentPt.x, CurrentPt.y);
}

int CUI_Class::GetInfoFromImageFile(CString strFileName)
{
	m_strFileName = strFileName;
	if(m_strFileName == "") 
	{
		return OK;
	}

	WCHAR* pWCharPath = AsWideString(strFileName, true);
	m_pImage    = new Bitmap(pWCharPath);
	delete pWCharPath;
	int iWidth  = m_pImage->GetWidth();
	int iHeight = m_pImage->GetHeight();
	
	m_region.CreateRectRgn(0, 0, iWidth, iHeight);

	Color InfoColor;
	m_pImage->GetPixel(0, 0, &InfoColor);

	if(0)//InfoColor.GetValue() != m_BGEraseColor.GetValue())
	{																				//                                Msb                  Lsb
		m_ObjectOffset.x = (InfoColor.GetRed() & 0xf0)<< 4 | InfoColor.GetGreen();  // x = 4Bit Msb Red + 8Bit Green (r1r2r3r4g1g2g3g4g5g6g7g8)   1:is Msb  8: is Lsb
		m_ObjectOffset.y = (InfoColor.GetRed() & 0x0f)<< 8 | InfoColor.GetBlue();   // y = 4BIt Lsb Red + 8Bit Blue  (r5r6r7r8b1b2b3b4b5b6b7b8)   1:is Msb  8: is Lsb
	}

	
	CRgn RgnTemp;
	m_ImageOffset = CPoint(iWidth, iHeight);
	int iStartX = -1; 

	for(int y = 0; y < iHeight; y++)
	for(int x = 0; x < iWidth;  x++)
	{
		if(x == 0 && y == 0)
		{
			m_pImage->GetPixel(x + 1, y, &InfoColor);
			if(InfoColor.GetValue() == m_BGEraseColor.GetValue())
			{
				RgnTemp.CreateRectRgn(x, y, x + 1, y + 1);
				m_region.CombineRgn(&m_region, &RgnTemp, RGN_DIFF);	
				RgnTemp.DeleteObject();
			}
			continue;
		}

		if(x == 0 && iStartX != -1)
		{
			RgnTemp.CreateRectRgn(iStartX, y - 1, iWidth, y);
			m_region.CombineRgn(&m_region, &RgnTemp, RGN_DIFF);	
			RgnTemp.DeleteObject();
			iStartX = -1;			
		}

		m_pImage->GetPixel(x, y, &InfoColor);
		if(InfoColor.GetValue() == m_BGEraseColor.GetValue()) 
		{
			if(iStartX == -1) iStartX = x;
		}
		else
		{
			if(iStartX != -1)
			{
				RgnTemp.CreateRectRgn(iStartX, y, x, y + 1);
				m_region.CombineRgn(&m_region, &RgnTemp, RGN_DIFF);	
				RgnTemp.DeleteObject();
				iStartX = -1;				
			}

			if(x < m_ImageOffset.x)   m_ImageOffset.x = x;
			if(y < m_ImageOffset.y)	  m_ImageOffset.y = y;
		}

		if(x == iWidth - 1 && y == iHeight - 1 && iStartX != -1)
		{
			RgnTemp.CreateRectRgn(iStartX, y, x + 1, y + 1);
			m_region.CombineRgn(&m_region, &RgnTemp, RGN_DIFF);	
			RgnTemp.DeleteObject();
			iStartX = -1;		
		}
	}

	m_region.OffsetRgn(m_ObjectOffset.x - m_ImageOffset.x, m_ObjectOffset.y - m_ImageOffset.y);

	
	if(strFileName.Right(6) == "_r.bmp")  //�ȨѨ�Region����
	{
		m_strFileName = strFileName.Left(strFileName.GetLength() - 6) + ".bmp";
		delete m_pImage;
		m_pImage = NULL;
	}
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     RedrawBGImage  
// INPUT:        bErase(�w�d)
// OUTPUT:       BOOL(�O�_���\����)
// Author:       Jack Chen
// DESCRIPTION:  ø�s��UI���I����
//////////////////////////////////////////////////////////////////////////////
int CUI_Class::RedrawBGImage(bool bErase)
{
	CClientDC cdc(m_ParentWnd);

	if(m_pImage == NULL)
	{
		WCHAR* pWCharPath = AsWideString(m_strFileName, true);
		m_pImage    = new Bitmap(pWCharPath);
		delete pWCharPath;

		if(m_pImage->GetFlags() == NULL)
		{
			delete m_pImage;
			m_pImage = NULL;
			AfxMessageBox("No Resource Picture" , MB_TOPMOST);
			return Err;
		}
	}
	if(m_cliprgn)
		cdc.SelectClipRgn(m_cliprgn);
	else
		cdc.SelectClipRgn(GetSacleRegion(true));


	if(m_MemoryDC == NULL) 
	{
		CClientDC cdc(m_ParentWnd);
		m_MemoryDC = new CDC();
		m_MemoryDC->CreateCompatibleDC(&cdc);
		m_Bitmap.CreateCompatibleBitmap(&cdc, int(m_fScale_Width * m_pImage->GetWidth()), int(m_fScale_Height * m_pImage->GetHeight()));
		m_MemoryDC->SelectObject(&m_Bitmap);
		m_pGraphics = new Graphics(m_MemoryDC->m_hDC);
		m_pGraphics->DrawImage(m_pImage, 0, 0, int(m_fScale_Width * m_pImage->GetWidth()), int(m_fScale_Height * m_pImage->GetHeight()));
	}


	int iStartX = m_ObjectOffset.x - int(m_fScale_Width * m_ImageOffset.x);
	int iStartY = m_ObjectOffset.y - int(m_fScale_Height * m_ImageOffset.y);
	cdc.BitBlt(iStartX, iStartY, int(m_fScale_Width * m_pImage->GetWidth()), int(m_fScale_Height * m_pImage->GetHeight()), m_MemoryDC, 0, 0, SRCCOPY);
	cdc.SelectClipRgn(NULL);
	return OK;	
}
int CUI_Class::DrawSpecifyImage(CString strImgPath)
{
	WCHAR* pWCharPath = AsWideString(strImgPath, true);
	Bitmap* pTempImage = new Bitmap(pWCharPath);
	delete pWCharPath;

	if (pTempImage->GetFlags() == NULL)
	{
		delete pTempImage;
		AfxMessageBox("No Resource Picture",MB_TOPMOST);
		return Err;
	}

	CClientDC cdc(m_ParentWnd);
	CDC memorydc;
	CBitmap bitmap;
	memorydc.CreateCompatibleDC(&cdc);
	bitmap.CreateCompatibleBitmap(&cdc, int(pTempImage->GetWidth() * m_fScale_Width), int(pTempImage->GetHeight() * m_fScale_Height));
	memorydc.SelectObject(&bitmap);

	Graphics pGraphics(memorydc.m_hDC);

	int iStartX = m_ObjectOffset.x - int(m_ImageOffset.x* m_fScale_Width);
	int iStartY = m_ObjectOffset.y - int(m_ImageOffset.y* m_fScale_Height);
	pGraphics.DrawImage(pTempImage, 0, 0, int(pTempImage->GetWidth()* m_fScale_Width), int(pTempImage->GetHeight() * m_fScale_Height));
	cdc.BitBlt(iStartX, iStartY, int(pTempImage->GetWidth()* m_fScale_Width), int(pTempImage->GetHeight()* m_fScale_Height), &memorydc, 0, 0, SRCCOPY);
	delete pTempImage;
	pTempImage = NULL;
	memorydc.DeleteDC();
	bitmap.DeleteObject();
	return OK;

}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     Redraw  
// INPUT:        bErase(�w�d), DrawType(�e���覡)
// OUTPUT:       BOOL(�O�_���\����)
// Author:       Jack Chen
// DESCRIPTION:  ø�s��UI���I����
//               DrawType: 
//               Redraw_BGImage      �e�I����
//               Redraw_ClickImage   �e�ƹ����U��
//               Redraw_ThroughImage �e�ƹ��g�L��
//////////////////////////////////////////////////////////////////////////////
int CUI_Class::Redraw(bool bErase, int DrawType)
{
	CString strFileName = m_strFileName;
	strFileName.MakeLower();
	if(m_strFileName == "") return OK;
	
	if(DrawType == Redraw_ClickImage   &&  !m_bClickImage)   DrawType = Redraw_BGImage;
	if(DrawType == Redraw_ThroughImage &&  !m_bThroughImage) DrawType = Redraw_BGImage;

	if(DrawType == Redraw_BGImage) return RedrawBGImage(bErase);

	switch(DrawType)
	{
		case Redraw_BGImage:      //�e�I����(�ɦW�����t _c�� _t���r��)
			if(!m_bBGImage)      return Err_No_BGImage;
			if(strFileName.Right(6) == "_c.bmp" || strFileName.Right(6) == "_t.bmp")  //�ɦW�h��_c ��_t
				strFileName = strFileName.Left(strFileName.GetLength() - 6) + ".bmp";
				break;
			break;
		case Redraw_ClickImage:   //�e�ƹ����U��(�ɦW�O�H _c.bmp����)
			if(strFileName.Right(6) != "_c.bmp")
			{
				if(strFileName.Right(6) == "_t.bmp")
					strFileName = strFileName.Left(strFileName.GetLength() - 6) + "_c.bmp";  //�ɦW�h��_t
				else
					strFileName = strFileName.Left(strFileName.GetLength() - 4) + "_c.bmp";
			}
			break;
		case Redraw_ThroughImage: //�e�ƹ��g�L��(�ɦW�O�H _t.bmp����)
			if(strFileName.Right(6) != "_t.bmp")
			{
				if(strFileName.Right(6) == "_c.bmp") 
					strFileName = strFileName.Left(strFileName.GetLength() - 6) + "_t.bmp"; //�ɦW�h��_c
				else
					strFileName = strFileName.Left(strFileName.GetLength() - 4) + "_t.bmp";
			}
			break;

	}

	if(m_pImage != NULL) delete m_pImage;
	WCHAR* pWCharPath = AsWideString(strFileName, true);
	m_pImage    = new Bitmap(pWCharPath);
	delete pWCharPath;

	if(m_pImage->GetFlags() == NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
		AfxMessageBox("No Resource Picture", MB_TOPMOST);
		return Err;
	}


	CClientDC cdc(m_ParentWnd);
	CDC memorydc;
	CBitmap bitmap;
	memorydc.CreateCompatibleDC(&cdc);
	bitmap.CreateCompatibleBitmap(&cdc, int(m_pImage->GetWidth() * m_fScale_Width), int(m_pImage->GetHeight() * m_fScale_Height));
	memorydc.SelectObject(&bitmap);

	if(m_cliprgn)
		cdc.SelectClipRgn(m_cliprgn);
	else if (m_SacleRgn)
		cdc.SelectClipRgn(m_SacleRgn);
	else
		cdc.SelectClipRgn(&m_region);

	Graphics pGraphics(memorydc.m_hDC);

	int iStartX = m_ObjectOffset.x - int(m_ImageOffset.x* m_fScale_Width); 
	int iStartY = m_ObjectOffset.y - int(m_ImageOffset.y* m_fScale_Height); 
	pGraphics.DrawImage(m_pImage, 0, 0, int(m_pImage->GetWidth()* m_fScale_Width), int(m_pImage->GetHeight() * m_fScale_Height));
	cdc.BitBlt(iStartX, iStartY, int(m_pImage->GetWidth()* m_fScale_Width), int(m_pImage->GetHeight()* m_fScale_Height), &memorydc, 0, 0, SRCCOPY);
	cdc.SelectClipRgn(NULL);
	delete m_pImage;
	m_pImage = NULL;
	return OK;	
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     AsWideString  
// INPUT:        const char* str (CString���A)
// OUTPUT:       WCHAR*
// Author:       Jack Chen
// DESCRIPTION:  �NCString �ରWCHAR* ���A��X
//////////////////////////////////////////////////////////////////////////////
WCHAR* CUI_Class::AsWideString(const char* str, bool bChangeToAbsolutePath)
{
	if( str == NULL )
	return NULL;

	CString strTemp = str;

	if(bChangeToAbsolutePath)
	{
		strTemp = ChangeToAbsolutePath(str);
	}
	 
	int nBufSize = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, strTemp, -1, NULL, 0 );
	WCHAR* wideString = new WCHAR[nBufSize+1];
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, strTemp, -1, wideString, nBufSize );
	 
	return wideString;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     OnMouseMove  
// INPUT:        UINT nFlags, CPoint point
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  �ƹ�����
//////////////////////////////////////////////////////////////////////////////
void CUI_Class::OnMouseMove(UINT nFlags, CPoint point)
{
	CRgn ClientRgn;
	ClientRgn.CreateRectRgn(0, 0, 0, 0);

	if (m_SacleRgn)
		ClientRgn.CombineRgn(&ClientRgn, m_SacleRgn, RGN_OR);
	else
		ClientRgn.CombineRgn(&ClientRgn, &m_region, RGN_OR);
	ClientRgn.OffsetRgn(-m_ObjectOffset.x, -m_ObjectOffset.y);


	BOOL bFlag = (GetCapture() != this);
	bFlag = ClientRgn.PtInRegion(point);
	bFlag = GetForegroundWindow() == this;


	if(GetCapture() != this && ClientRgn.PtInRegion(point))// && (GetForegroundWindow() == this || m_strFileName.Find("..\\Menu Test\\MainDlg\\") != -1))
	//Menu Add Item��, Region�]�w�����T��Bug. �ȮɥΦ��k��
	{
		SetCapture();
		Redraw(true, Redraw_ThroughImage);
	}
	else if(GetCapture() == this && !ClientRgn.PtInRegion(point)) 
	{
		ReleaseCapture();
		Redraw(true, Redraw_BGImage);
	}

	if (m_pUTipDll)
		m_pUTipDll->_UT_ShowTip(m_pUTipDll->GetUTObj(), m_ParentWnd->GetSafeHwnd(), point.x + m_ObjectOffset.x, point.y + m_ObjectOffset.y);
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     OnPaint  
// INPUT:        void
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  �N����쥻�I�s��OnPaint�禡�A���N��Redraw�禡
//////////////////////////////////////////////////////////////////////////////
void CUI_Class::OnPaint()
{
	CPaintDC dc(this);
	if(!m_bPassiveDraw)   Redraw(true, Redraw_BGImage);
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     Initialize  
// INPUT:        void
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  ���󪺪�l��
//////////////////////////////////////////////////////////////////////////////
int  CUI_Class::Initialize()
{
	if (m_bHaveInitialized == true) return OK; //Allen
		
	if(m_strFileName == "") 
	{
		m_bHaveInitialized = true;
		return OK;
	}
	CRect RgnBox;
	CRgn  TempRgn;
	m_region.GetRgnBox(&RgnBox);

	while(m_ParentWnd->IsKindOf(RUNTIME_CLASS(CUI_Class)))
	{
		if(((CUI_Class*)m_ParentWnd)->m_strFileName == "") 
			m_ParentWnd = ((CUI_Class*)m_ParentWnd)->GetParentWnd();
		else 
			break;
	}
	CWnd::Create(NULL, NULL, WS_CHILD|WS_VISIBLE | BS_OWNERDRAW, RgnBox, m_ParentWnd, m_iID);
	
	TempRgn.CreateRectRgn(0, 0, 0, 0);
	TempRgn.CombineRgn(&TempRgn, &m_region, RGN_OR);
	TempRgn.OffsetRgn(-RgnBox.left, -RgnBox.top);
	SetWindowRgn(TempRgn, 1);
	m_bHaveInitialized = true;
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     OnBnClicked()�A OnBnDoubleclicked()�A  OnLButtonDown()�A OnChildNotify()
// INPUT:        void
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  ���F���N�즳�ʧ@�C�ҥH�мg�o�Ǩ禡
//////////////////////////////////////////////////////////////////////////////
void CUI_Class::OnBnClicked(){}
void CUI_Class::OnBnDoubleclicked(){}
void CUI_Class::OnLButtonDown(UINT nFlags, CPoint point){}
BOOL CUI_Class::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult){ return OK;}



//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     OnZoom
// INPUT:        iStartX, iStartY, fScaleWidth, fScaleHeight
// OUTPUT:       int(���浲�G)
// Author:       Jack Chen
// DESCRIPTION:  ��j����C���e�i���P���
//////////////////////////////////////////////////////////////////////////////
int CUI_Class::OnZoom(int iStartX, int iStartY, float fScaleWidth, float fScaleHeight)
{
	m_ObjectOffset.x = int(fScaleWidth * m_ObjectOffset.x) + iStartX;
	m_ObjectOffset.y = int(fScaleHeight * m_ObjectOffset.y) + iStartY;

	m_fScale_Width  *= fScaleWidth;
	m_fScale_Height *= fScaleHeight;
	
	if(m_region.m_hObject == NULL) return OK;
	

	CRect RegRect;
	WINDOWPLACEMENT WndPlacement;
	GetWindowPlacement(&WndPlacement);
	RegRect = WndPlacement.rcNormalPosition;
	RegRect = CRect(m_ObjectOffset.x, m_ObjectOffset.y, int(m_ObjectOffset.x + fScaleHeight * RegRect.Width()), int(m_ObjectOffset.y + fScaleHeight * RegRect.Height()));
	WndPlacement.rcNormalPosition = RegRect;
	SetWindowPlacement(&WndPlacement);

	RECT rectTemp;
	GetSacleRegion(false)->GetRgnBox(&rectTemp);

	CRgn  TempRgn;
	TempRgn.CreateRectRgn(0, 0, 0, 0);
	TempRgn.CombineRgn(&TempRgn, m_SacleRgn, RGN_OR);
	TempRgn.OffsetRgn(-rectTemp.left, -rectTemp.top);
	SetWindowRgn(TempRgn, 1);

	//SetWindowRgn(*GetSacleRegion(false), 1);

	delete m_MemoryDC;
	delete m_pGraphics;
	m_Bitmap.DeleteObject();
	m_pGraphics = NULL;
	m_MemoryDC = NULL;
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     SetDlgRegion
// INPUT:        DlgWnd
// OUTPUT:       int(���浲�G)
// Author:       Jack Chen
// DESCRIPTION:  �NDlgWnd��Region(�Ϊ�)�A�]����UI��Region(�Ϊ�)
//////////////////////////////////////////////////////////////////////////////
int CUI_Class::SetDlgRegion(CWnd* DlgWnd, BOOL bSetPlacement)
{
	if(GetRegion()->m_hObject != NULL)	
	{
		CRgn  TempRgn;
		CRect RegRect;
		if(m_fScale_Width == 1 && m_fScale_Height == 1) 
			GetRegion()->GetRgnBox(&RegRect);
		else
			GetSacleRegion(true)->GetRgnBox(&RegRect);

		if (bSetPlacement)
		{
			WINDOWPLACEMENT WndPlacement;
			DlgWnd->GetWindowPlacement(&WndPlacement);

			WndPlacement.rcNormalPosition = RegRect;
			DlgWnd->SetWindowPlacement(&WndPlacement);
		}
		TempRgn.CreateRectRgn(0, 0, 0, 0);

		if(m_fScale_Width == 1 && m_fScale_Height == 1) 
			TempRgn.CombineRgn(&TempRgn, GetRegion(), RGN_OR);
		else			
			TempRgn.CombineRgn(&TempRgn, GetSacleRegion(true), RGN_OR);

		return DlgWnd->SetWindowRgn(TempRgn, 1);
	}	
	else 
		return ERROR;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     GetSacleRegion
// INPUT:        bContainItems (�O�_�n�t�Ҧ���Item)
// OUTPUT:       CRgn*
// Author:       Jack Chen
// DESCRIPTION:  �o�� Scale�᪺ CRgn�A�i�̿�J�� bContaionItems
//               �ӨM�w��X��CRgn�O�_�n�]�tItem�C(�� CMenu_Class��)
//////////////////////////////////////////////////////////////////////////////
CRgn* CUI_Class::GetSacleRegion(bool bContainItems)
{
	if(m_fScale_Width == 1.0f && m_fScale_Height == 1.0f)
	{
		if(bContainItems)
			return GetRegion();
		else
			return &m_region;
	}

	if(m_SacleRgn != NULL) 
	{
		delete m_SacleRgn;
		m_SacleRgn = NULL;
	}
	m_SacleRgn = new CRgn();
	m_SacleRgn->CreateRectRgn(0, 0, 0, 0);
	Region* TempRgn;
	if(bContainItems)
		TempRgn = new Region(HRGN(*GetRegion()));
	else
		TempRgn = new Region(HRGN(m_region));
	Matrix matrix(m_fScale_Width, 0, 0, m_fScale_Height, 0, 0);
	TempRgn->Transform(&matrix);

	HRGN hRegion;
	CDC aDC;
	Graphics graphics(aDC);
	
	hRegion = TempRgn->GetHRGN(&graphics);
	Sleep(10);
	m_SacleRgn->CombineRgn(m_SacleRgn, CRgn::FromHandle(hRegion), RGN_OR);	
	
	delete TempRgn;	
	TempRgn = NULL;
	return m_SacleRgn;
}


//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     ChangeToOtherBKImage
// INPUT:        void
// OUTPUT:       int(���浲�G)
// Author:       Jack Chen
// DESCRIPTION:  User�i��I���ϴ����Q�n���Ϥ�
//////////////////////////////////////////////////////////////////////////////
int CUI_Class::ChangeToOtherBKImage()
{
	int iResult = OK;
	char filters[] = "JPEG(*.jpg)|*.jpg|BMP(*.bmp)|*.bmp|TIF(*.tif)|*.tif|PNG(*.png)|*.png|GIF(*.gif)|*.gif|All File(*.*)|*.*|";
	CFileDialog fileDlg(TRUE, "", "", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filters, NULL, sizeof(OPENFILENAME));		
	if(fileDlg.DoModal() == IDOK)
	{
		delete m_pImage;
		m_region.DeleteObject();
		delete m_MemoryDC;
		delete m_pGraphics;
		m_Bitmap.DeleteObject();
		m_pGraphics = NULL;
		m_MemoryDC = NULL;

		GetInfoFromImageFile(fileDlg.GetPathName());

		CRect RgnBox;
		CRgn  TempRgn;
		m_region.GetRgnBox(&RgnBox);
		TempRgn.CreateRectRgn(0, 0, 0, 0);
		TempRgn.CombineRgn(&TempRgn, &m_region, RGN_OR);
		TempRgn.OffsetRgn(-RgnBox.left, -RgnBox.top);
		SetWindowRgn(TempRgn, 1);
	}
	else
		iResult = Err;

	return iResult;
}
void CUI_Class::OffsetObject(CPoint OffsetPt)
{
	m_ObjectOffset.Offset(OffsetPt);     
	if(m_region.m_hObject != NULL)   
		m_region.OffsetRgn(OffsetPt);

	if(m_bHaveInitialized && m_strFileName != "")
	{
		CRect WindowRect, ParentRect;
		m_ParentWnd->GetWindowRect(&ParentRect);
		GetWindowRect(&WindowRect);
		WindowRect.OffsetRect(-ParentRect.left, -ParentRect.top);
		MoveWindow(WindowRect.left + OffsetPt.x, WindowRect.top + OffsetPt.y,
			       WindowRect.Width(), WindowRect.Height(), true);
	}

}

void CUI_Class::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);
	// TODO: �b���[�J�z���T���B�z�`���{���X
}

CString CUI_Class::ChangeToAbsolutePath(CString strFile)
{	
	int nBufferLength = 300;
	static char CurPath[300] = "";

	if(strcmp(CurPath, "") == 0)
	GetCurrentDirectory(nBufferLength, CurPath);

	CString strFilePath = CurPath;
	if(strFile.Find("..\\") != -1)
	{
		strFilePath.MakeReverse();
		strFilePath = strFilePath.Right(strFilePath.GetLength() - (strFilePath.Find("\\") + 1));
		strFilePath.MakeReverse();
		strFilePath += strFile.Right(strFile.GetLength() - (strFile.Find("..\\") + 2));
	}
	else
		strFilePath = strFile;

	return strFilePath;
}

void CUI_Class::OnEnable(BOOL bEnable)
{
	CWnd::OnEnable(bEnable);

	// TODO: �b���[�J�z���T���B�z�`���{���X
	EnableWindow(bEnable);

	if (bEnable)
	{
		LPMSG lpMsg = NULL;
		while (PeekMessage(lpMsg, m_hWnd, NULL, NULL, PM_REMOVE))
		{
			if (!PreTranslateMessage(lpMsg))
			{
				TranslateMessage(lpMsg);
				DispatchMessage(lpMsg);
			}
		}
	}
}
