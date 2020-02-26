#include "StdAfx.h"
#include "Resource.h"
#include "Moveable_Item.h"

BEGIN_MESSAGE_MAP(CMoveable_Item, CItem_Class)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


CMoveable_Item::CMoveable_Item(void)
{
	m_BeginPt = CPoint(0, 0);
	m_EndPt   = CPoint(0, 0);
}

CMoveable_Item::~CMoveable_Item(void)
{
}
CMoveable_Item::CMoveable_Item(CWnd* ParetnWnd, HWND hwnd, int iIDNum, CString strImageFileName, bool bHaveBGImage, 
						 bool bHaveClickImage, bool bHaveThroughImage, bool bIsOptionButtion)
{
	CreateItem(ParetnWnd, hwnd, iIDNum, strImageFileName, bHaveBGImage, bHaveClickImage, bHaveThroughImage, bIsOptionButtion);
	m_BeginPt = CPoint(0, 0);
	m_EndPt   = CPoint(0, 0);
	m_pItemImage = NULL;
}

CMoveable_Item::CMoveable_Item(CWnd* ParetnWnd, HWND hwnd, int iIDNum, CString strImageFileName, 
	                   CString strItemFileName, CPoint BeginPt, CPoint EndPt)
{
	m_hWnd = hwnd;
	m_ParentMenu = (CMenu_Class*)ParetnWnd;
	m_ParentWnd = ParetnWnd;
	GetInfoFromImageFile(strItemFileName);
	{
		m_pItemImage = m_pImage;
		m_pImage = NULL;
		m_ItemRegion.CreateRectRgn(0, 0, 1, 1);
		m_ItemRegion.CopyRgn(&m_region);
		m_region.DeleteObject();
	}
	GetInfoFromImageFile(strImageFileName);
	m_strItemPath = strItemFileName;
	m_bBGImage = true;
	m_bClickImage = false;
	m_bThroughImage = false;
	m_bIsOptionType = false;
	m_WindowsItem = false;
	m_iID = iIDNum;
	m_bClicked = false;

	m_BeginPt = CPoint(0, 0);
	m_EndPt = CPoint(0, 0);

	SetItemMoveLimit(BeginPt, EndPt);
	CPoint ptTemp;
	ptTemp.x = (BeginPt.x + EndPt.x)/2;
	ptTemp.y = (BeginPt.y + EndPt.y) / 2;
	SetToPos(ptTemp, TRUE);
}

void CMoveable_Item::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if (nFlags != MK_LBUTTON)
		SetMouseCursor(IDC_HAND_OPEN);
	else
		SetMouseCursor(IDC_HAND_CLOSE);

	CItem_Class::OnMouseMove(nFlags, point);

	if (nFlags == MK_LBUTTON)
	{
		SetCapture();

		if (m_pItemImage)
		{
			SetToPos(m_ObjectOffset - m_ParentMenu->GetObjectOffset() + point);
			Redraw(true, Redraw_ThroughImage);
			m_ParentWnd->SendMessage(m_iID, DEF_UI_CLICK_MSG);
		}
	}

	//CItem_Class::OnMouseMove(nFlags, point);
}

void CMoveable_Item::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	m_CursorPos = point;
	SetMouseCursor(IDC_HAND_CLOSE);
	m_ParentMenu->AddRegion(&m_region);
	SetCapture();

	CItem_Class::OnLButtonDown(nFlags, point);
}

void CMoveable_Item::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	//if(m_CursorPos != CPoint(0,0) && m_CursorPos != point)
	{
		//CPoint OffsetPt = point - m_CursorPos;
		//CPoint DestPt   = m_ObjectOffset - m_ParentMenu->GetObjectOffset() + OffsetPt;

		SetToPos(m_ObjectOffset - m_ParentMenu->GetObjectOffset() + point);
	}

	m_CursorPos = CPoint(0, 0);
	SetMouseCursor(IDC_HAND_OPEN);
	ReleaseCapture();

	CItem_Class::OnLButtonUp(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     SetMouseCursor  
// INPUT:        iCursorID
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  設滑鼠的圖示
//////////////////////////////////////////////////////////////////////////////
HCURSOR CMoveable_Item::SetMouseCursor(int iCursorID)
{
	HCURSOR hCursor;
	hCursor = AfxGetApp()->LoadCursor(iCursorID);
	::SetCursor(hCursor);
	return hCursor;

}
 
void CMoveable_Item::SetToPosPre(CPoint DestPt)
{
	m_ParentMenu->AddRegion(&m_region);
	if(m_BeginPt != CPoint(0, 0) || m_EndPt != CPoint(0, 0))
	{
		DestPt.x = DestPt.x > m_BeginPt.x ? DestPt.x : m_BeginPt.x;
		DestPt.x = DestPt.x < m_EndPt.x   ? DestPt.x : m_EndPt.x;
		DestPt.y = DestPt.y > m_EndPt.y ? DestPt.y : m_EndPt.y;
		DestPt.y = DestPt.y < m_BeginPt.y   ? DestPt.y : m_BeginPt.y;
	}

	CPoint OffsetPt = DestPt - (m_ObjectOffset - m_ParentMenu->GetObjectOffset());
	OffsetObject(OffsetPt);	
	
	//m_ParentMenu->ResetEffectRegion();
	/*CRect rectBox;
	m_region.GetRgnBox(&rectBox);
	m_ParentMenu->InvalidateRect(rectBox, false);*/
	m_ParentMenu->Invalidate(false);
}

void CMoveable_Item::SetToPos(CPoint DestPt, BOOL bInit)
{ 	
	if (m_MemoryDC == NULL)
	{
		CClientDC cdc(m_ParentWnd);
		m_MemoryDC = new CDC();
		m_MemoryDC->CreateCompatibleDC(&cdc);
		m_Bitmap.CreateCompatibleBitmap(&cdc, int(m_fScale_Width * m_pImage->GetWidth()), int(m_fScale_Height * m_pImage->GetHeight()));
		m_MemoryDC->SelectObject(&m_Bitmap);
		m_pGraphics = new Graphics(m_MemoryDC->m_hDC);
	}

	if (m_pImage)
		m_pGraphics->DrawImage(m_pImage, 0, 0, int(m_fScale_Width * m_pImage->GetWidth()), int(m_fScale_Height * m_pImage->GetHeight()));
	if (m_pItemImage)
	{
		if (!bInit)
			DestPt.x -= LONG(0.5*(m_fScale_Width * m_pItemImage->GetWidth()));
		if (m_BeginPt != CPoint(0, 0) || m_EndPt != CPoint(0, 0))
		{
			DestPt.x = DestPt.x > m_BeginPt.x ? DestPt.x : m_BeginPt.x;
			DestPt.x = DestPt.x < m_EndPt.x ? DestPt.x : m_EndPt.x;
			DestPt.y = DestPt.y > m_EndPt.y ? DestPt.y : m_EndPt.y;
			DestPt.y = DestPt.y < m_BeginPt.y ? DestPt.y : m_BeginPt.y;
		}
		m_ItemCurPt = DestPt - m_BeginPt;// +CPoint(0, int(m_pItemImage->GetHeight() / 4));

		CRgn rgnTemp;
		rgnTemp.CreateRectRgn(0, 0, 1, 1);
		m_ItemRegion.OffsetRgn(m_ItemCurPt);
		m_pGraphics->SetClip(m_ItemRegion);
		m_pGraphics->DrawImage(m_pItemImage, m_ItemCurPt.x, m_ItemCurPt.y, int(m_fScale_Width * m_pItemImage->GetWidth()), int(m_fScale_Height * m_pItemImage->GetHeight()));
		m_pGraphics->ResetClip();
		m_ItemRegion.OffsetRgn(-m_ItemCurPt);
		
	}
}
