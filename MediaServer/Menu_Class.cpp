#include "StdAfx.h"
#include "Menu_Class.h"
#include "Item_Class.h"
IMPLEMENT_DYNAMIC(CMenu_Class, CUI_Class)

BEGIN_MESSAGE_MAP(CMenu_Class, CUI_Class)
	ON_WM_MOUSEMOVE()
	ON_WM_SHOWWINDOW()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     CMenu_Class  
// INPUT:        ParetnWnd(父親視窗)、hwnd(DC來源)、iIDNum(ID Number)、strImageFileName(圖片檔名)、bHaveBGImage(是否有背景圖)
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  CMenu_Class的建構子
//////////////////////////////////////////////////////////////////////////////
CMenu_Class::CMenu_Class(CWnd* ParetnWnd, HWND hwnd, int iIDNum, CString strImageFileName, bool bHaveBGImage)
{	
	GetInfoFromImageFile(strImageFileName);
	m_hWnd = hwnd;
	m_bBGImage      = bHaveBGImage;
	m_bClickImage   = NoClickImage;
	m_bThroughImage = NoThroughtImage;
	m_CursorPos = CPoint(0, 0);
	m_ParentWnd = ParetnWnd;
	m_iID = iIDNum;
	m_iItemNum = 0;
	m_bMoveable = TRUE;
	m_iExceptID = -1;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     CreateMenu  
// INPUT:        ParetnWnd(父親視窗)、hwnd(DC來源)、iIDNum(ID Number)、strImageFileName(圖片檔名)、bHaveBGImage(是否有背景圖)
// OUTPUT:       int(執行結果)
// Author:       Jack Chen
// DESCRIPTION:  CMenu_Class的建構式
//////////////////////////////////////////////////////////////////////////////
int CMenu_Class::CreateMenu(CWnd* ParetnWnd, HWND hwnd, int iIDNum, CString strImageFileName, bool bHaveBGImage)
{
	GetInfoFromImageFile(strImageFileName);
	m_hWnd = hwnd;
	m_bBGImage      = bHaveBGImage;
	m_bClickImage   = NoClickImage;
	m_bThroughImage = NoThroughtImage;
	m_CursorPos = CPoint(0, 0);
	m_ParentWnd = ParetnWnd;
	m_iID = iIDNum;
	m_iItemNum = 0;
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     ~CMenu_Class  
// INPUT:        void
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  CMenu_Class的解構子
//////////////////////////////////////////////////////////////////////////////
CMenu_Class::~CMenu_Class(void)
{
	RemoveAll();
	//CUI_Class::~CUI_Class();
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     AddItem  
// INPUT:        pItem
// OUTPUT:       int(執行結果)
// Author:       Jack Chen
// DESCRIPTION:  增加一個已存在Item Class於這個Menu之內
//////////////////////////////////////////////////////////////////////////////
int CMenu_Class::AddItem(CUI_Class* pItem)
{
	//pItem->OffsetObject(CPoint(m_ObjectOffset.x, m_ObjectOffset.y)); //Item Object的Offset需加上 Menu的Offest	
	
	m_ItemList.AddHead(pItem);
	m_iItemNum++;
	if(m_region.m_hObject != NULL && pItem->GetRegion()->m_hObject != NULL)
	{
		m_region.CombineRgn(&m_region, pItem->GetRegion(), RGN_DIFF);
	}
	((CItem_Class*)pItem)->SetParentMenu(this);
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     DeleteItem
// INPUT:        int iIDnum
// OUTPUT:       int(執行結果)
// Author:       Jack Chen
// DESCRIPTION:  刪除一個指定ID的Item
//////////////////////////////////////////////////////////////////////////////
int CMenu_Class::DeleteItem(int iIDNum, BOOL bAddRegion)
{
	POSITION aPos = m_ItemList.GetHeadPosition();
	POSITION CurItemPos = aPos;
	CUI_Class *Item;
	while(aPos)
	{
		CurItemPos = aPos;
		Item = m_ItemList.GetNext(aPos);
		if(Item->GetIDNumber() == iIDNum) 
		{
			if (bAddRegion)
				AddRegion(Item->GetRegion());

			m_ItemList.RemoveAt(CurItemPos);
			delete Item;
			m_iItemNum--;
			return OK;
		}
	}
	return Err;
}


//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     RemoveAll  
// INPUT:        void
// OUTPUT:       int(執行結果)
// Author:       Jack Chen
// DESCRIPTION:  把Menu內所有的Item移除
//////////////////////////////////////////////////////////////////////////////
int CMenu_Class::RemoveAll()
{
	POSITION aPos = m_ItemList.GetHeadPosition();
	INT32 iCount = m_ItemList.GetCount();
	while (aPos)
	{
		CItem_Class *tempClass;
		tempClass = (CItem_Class*)m_ItemList.GetNext(aPos);
		if (!tempClass->m_WindowsItem)
			delete tempClass;
	}
	m_ItemList.RemoveAll();
	m_iItemNum = 0;
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     OnWindowMove  
// INPUT:        ix(X軸的位移), iy(Y軸的位移)
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  移動Parent Window的位置(供拖曳視窗之用)
//////////////////////////////////////////////////////////////////////////////
void CMenu_Class::OnWindowMove(int ix, int iy)
{
	CRect windowerect;
	m_ParentWnd->GetWindowRect(&windowerect);
	m_ParentWnd->MoveWindow( windowerect.left + ix, windowerect.top + iy,  windowerect.Width(), windowerect.Height(), true);	
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     OffsetObject  
// INPUT:        OffsetPt(Menu的位移量)
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  將Menu位置移動，並把之內的Item一起跟著移動
//////////////////////////////////////////////////////////////////////////////
void CMenu_Class::OffsetObject(CPoint OffsetPt)
{
	CUI_Class::OffsetObject(OffsetPt);
	POSITION aPos = m_ItemList.GetHeadPosition();
	CUI_Class *Item;
	while(aPos)
	{
		Item = m_ItemList.GetNext(aPos);
		Item->OffsetObject(OffsetPt);
	}
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     Initialize  
// INPUT:        void
// OUTPUT:       int(執行結果)
// Author:       Jack Chen
// DESCRIPTION:  將Menu初始化，並把之內的Item一起初始化
//////////////////////////////////////////////////////////////////////////////
int  CMenu_Class::Initialize()
{	
	POSITION aPos = m_ItemList.GetHeadPosition();
	CUI_Class *Item;
	CUI_Class::Initialize();
	while(aPos)
	{
		Item = m_ItemList.GetNext(aPos);
		Item->Initialize();
	}
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     GetRegion  
// INPUT:        void
// OUTPUT:       CRgn
// Author:       Jack Chen
// DESCRIPTION:  把Menu的Region(形狀)傳回去。並包含Item的形狀
//////////////////////////////////////////////////////////////////////////////
CRgn* CMenu_Class::GetRegion(void)
{
	CRgn* TotalRgn = new CRgn();
	TotalRgn->CreateRectRgn(0, 0, 0, 0);
	TotalRgn->CombineRgn(TotalRgn, &m_region, RGN_OR);
	POSITION aPos = m_ItemList.GetHeadPosition();
	CUI_Class *Item;
	while(aPos)
	{
		Item = m_ItemList.GetNext(aPos);
		TotalRgn->CombineRgn(TotalRgn, Item->GetRegion(), RGN_OR);
	}
	return TotalRgn;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     OnMouseMove  
// INPUT:        UINT nFlags, CPoint point
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  滑鼠移動
//////////////////////////////////////////////////////////////////////////////
void CMenu_Class::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if (!m_bMoveable)
		return;

	if(nFlags == MK_LBUTTON)
	{
		if(m_CursorPos != CPoint(0,0) && m_CursorPos != point)
		{
			OnWindowMove(point.x - m_CursorPos.x, point.y - m_CursorPos.y);
		}
		else
			m_CursorPos = point;
	}
	else
		m_CursorPos = CPoint(0,0);

	CUI_Class::OnMouseMove(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     OnZoom
// INPUT:        iStartX, iStartY, fScaleWidth, fScaleHeight
// OUTPUT:       int(執行結果)
// Author:       Jack Chen
// DESCRIPTION:  放大物件。長寬可不同比例
//////////////////////////////////////////////////////////////////////////////
int CMenu_Class::OnZoom(int iStartX, int iStartY, float fScaleWidth, float fScaleHeight)
{
	POSITION aPos = m_ItemList.GetHeadPosition();
	CUI_Class *Item;
	while(aPos)
	{
		Item = m_ItemList.GetNext(aPos);
		Item->OnZoom(iStartX, iStartY, fScaleWidth, fScaleHeight);
	}
	CUI_Class::OnZoom(iStartX, iStartY, fScaleWidth, fScaleHeight);
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     ChangeToOtherBKImage
// INPUT:        void
// OUTPUT:       int(執行結果)
// Author:       Jack Chen
// DESCRIPTION:  User可把背景圖換成想要的圖片，Menu需重設Effect Region
//////////////////////////////////////////////////////////////////////////////
int CMenu_Class::ChangeToOtherBKImage()
{
	int iResult = OK;
	iResult  = CUI_Class::ChangeToOtherBKImage();
	iResult |= ResetEffectRegion();
	return iResult;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     ResetEffectRegion
// INPUT:        void
// OUTPUT:       int(執行結果)
// Author:       Jack Chen
// DESCRIPTION:  重設Effect Region。(去掉Item的 Effect Region)
//////////////////////////////////////////////////////////////////////////////
int CMenu_Class::ResetEffectRegion()
{	
	int iResult = OK;
	if(m_region.m_hObject != NULL)
	{	
		POSITION aPos = m_ItemList.GetHeadPosition();
		CUI_Class *pItem;
		CRect RgnRect;
		while(aPos)
		{
			pItem = m_ItemList.GetNext(aPos);
			m_region.CombineRgn(&m_region, pItem->GetRegion(), RGN_DIFF);
			pItem->GetRegion()->GetRgnBox(&RgnRect);
		}
	}

	if(1)//m_strFileName == "..\\Menu Test\\MainDlg\\Play Menu\\FramePos.bmp") //暫時做法,待日後解
	{
		CRgn TempRgn;
		CRect RgnRect;
		m_region.GetRgnBox(&RgnRect);
		TempRgn.CreateRectRgn(0, 0, 0, 0);
		TempRgn.CombineRgn(&TempRgn, &m_region, RGN_OR);
		TempRgn.OffsetRgn(-RgnRect.left, -RgnRect.top);
		SetWindowRgn(TempRgn, true);
	}
	return iResult;	
}


//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     AddRegion
// INPUT:        AddRgn
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  將Menu的Region增加輸入的Region。用以回復被移除Item的範圍
//////////////////////////////////////////////////////////////////////////////
void CMenu_Class::AddRegion(CRgn* AddRgn)
{
	if(m_region.m_hObject != NULL)
m_region.CombineRgn(&m_region, AddRgn, RGN_OR);
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     OnShowWindow
// INPUT:        bShow, nStatus
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  將Menu的ShowWindow設定, 亦設給Menu內的所有Item
//////////////////////////////////////////////////////////////////////////////
void CMenu_Class::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CUI_Class::OnShowWindow(bShow, nStatus);

	ShowWindow(bShow);
	// TODO: 在此加入您的訊息處理常式程式碼
	POSITION aPos = m_ItemList.GetHeadPosition();
	CUI_Class *Item;
	while (aPos)
	{
		Item = m_ItemList.GetNext(aPos);
		if (Item->IsHaveInitialized())
			Item->ShowWindow(bShow);
	}
}

void   CMenu_Class::DoItemClickFunction(int iIDNum, int iParam)
{
	//if(!(iParam & Is_Option_Item)) return;
	if (iParam < 1) return;

	POSITION aPos = m_ItemList.GetHeadPosition();
	CUI_Class *Item;
	while (aPos)
	{
		Item = m_ItemList.GetNext(aPos);
		if (Item->IsHaveInitialized())
		{
			if (Item->GetIDNumber() != iIDNum && ((CItem_Class*)Item)->m_bIsOptionType
				&& ((CItem_Class*)Item)->m_iGroupIndex == iParam)
			{
				if (((CItem_Class*)Item)->m_bClicked)
				{
					((CItem_Class*)Item)->m_bClicked = false;
					Item->Redraw(true, Redraw_BGImage);
				}
			}
		}
	}
}

void CMenu_Class::OnPaint()
{
	CPaintDC dc(this);
	if (!m_bPassiveDraw)   Redraw(true, Redraw_BGImage);

	POSITION aPos = m_ItemList.GetHeadPosition();
	CUI_Class *Item;
	while (aPos)
	{
		Item = m_ItemList.GetNext(aPos);
		if (Item->IsHaveInitialized())
		{
			Item->Redraw(true, Redraw_BGImage);
		}
	}
}

CUI_Class* CMenu_Class::GetItemByID(INT32 iID)
{
	CUI_Class *Item;
	POSITION aPos = m_ItemList.GetHeadPosition();
	while (aPos)
	{
		Item = m_ItemList.GetNext(aPos);
		if (Item->IsHaveInitialized() && Item->GetIDNumber() == iID)
		{
			return Item;
		}
	}

	return NULL;
}

void CMenu_Class::SetGroupItem(CArray<INT32, INT32> iIDAry, INT32 iGroupIndex)
{
}

INT32 CMenu_Class::GetSelectedItem(INT32 iGroupIndex)
{
	CUI_Class *pItem;
	POSITION aPos = m_ItemList.GetHeadPosition();
	while (aPos)
	{
		pItem = m_ItemList.GetNext(aPos);
		if (pItem->IsHaveInitialized() && pItem->IsKindOf(RUNTIME_CLASS(CItem_Class)) &&
			((CItem_Class*)pItem)->m_bIsOptionType &&
			((CItem_Class*)pItem)->m_iGroupIndex == iGroupIndex)
		{

			if (((CItem_Class*)pItem)->m_bClicked)
			{
				return pItem->GetIDNumber();
			}
			
		}
	}

	return 0;
}

void CMenu_Class::EnableItem(BOOL bEnable, INT32 iExceptID)
{
	if (!bEnable)
		m_iExceptID = iExceptID;
	else
		m_iExceptID = -1;

	OnEnable(bEnable);
}

void CMenu_Class::OnEnable(BOOL bEnable)
{
	CUI_Class::OnEnable(bEnable);
	// TODO: 在此加入您的訊息處理常式程式碼
	
	CUI_Class *pItem;
	POSITION aPos = m_ItemList.GetHeadPosition();
	while (aPos)
	{
		pItem = m_ItemList.GetNext(aPos);
		if (pItem->IsHaveInitialized() && pItem->IsKindOf(RUNTIME_CLASS(CItem_Class)))
		{
			if (m_iExceptID == -1 || pItem->GetIDNumber () != m_iExceptID)
				pItem->OnEnable(bEnable);
		}
	}
}
