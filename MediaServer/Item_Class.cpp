#include "StdAfx.h"
#include "Item_Class.h"
#include "UI_Class.h"

BEGIN_MESSAGE_MAP(CItem_Class, CUI_Class)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CItem_Class, CUI_Class)

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     CMenu_Class  
// INPUT:        ParetnWnd(父親視窗)、hwnd(DC來源)、iIDNum(ID Number)、strImageFileName(圖片檔名)、bHaveBGImage(是否有背景圖)
//               bHaveClickImage(是否有Click圖), bHaveThroughImage(是否有Through圖), bIsOptionButtion(是否是Option的Button)
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  CMenu_Class的建構子
//////////////////////////////////////////////////////////////////////////////
CItem_Class::CItem_Class(CWnd* ParetnWnd, HWND hwnd, int iIDNum, CString strImageFileName, bool bHaveBGImage, 
						 bool bHaveClickImage, bool bHaveThroughImage, UINT iGroupOption) : m_bClicked(false)
{
	m_hWnd = hwnd;
	m_ParentMenu = (CMenu_Class*)ParetnWnd;
	m_ParentWnd = ParetnWnd;
	GetInfoFromImageFile(strImageFileName);
	m_bBGImage      = bHaveBGImage;
	m_bClickImage   = bHaveClickImage;
	m_bThroughImage = bHaveThroughImage;
	m_bIsOptionType = (iGroupOption > 0);
	m_iGroupIndex = iGroupOption;
	m_WindowsItem = false;
	m_iID = iIDNum;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     CMenu_Class  
// INPUT:        ParetnWnd(父親視窗)、hwnd(DC來源)、iIDNum(ID Number)、strImageFileName(圖片檔名)、bHaveBGImage(是否有背景圖)
//               bHaveClickImage(是否有Click圖), bHaveThroughImage(是否有Through圖), bIsOptionButtion(是否是Option的Button)
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  CMenu_Class的建構式
//////////////////////////////////////////////////////////////////////////////
int CItem_Class::CreateItem(CWnd* ParetnWnd, HWND hwnd, int iIDNum, CString strImageFileName, bool bHaveBGImage, 
							bool bHaveClickImage, bool bHaveThroughImage, bool bIsOptionButtion)
{
	m_hWnd = hwnd;
	m_ParentMenu = (CMenu_Class*)ParetnWnd;
	m_ParentWnd = ParetnWnd;
	GetInfoFromImageFile(strImageFileName);
	m_bBGImage      = bHaveBGImage;
	m_bClickImage   = bHaveClickImage;
	m_bThroughImage = bHaveThroughImage;
	m_bIsOptionType = bIsOptionButtion;
	m_WindowsItem = false;
	m_iID = iIDNum;
	m_bClicked = false;
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     CreateWindowsItem  
// INPUT:        ParetnWnd(父親視窗)、hwnd(DC來源)、iIDNum(ID Number)、WindowsRect(物件的Rectangel大小)
// OUTPUT:       int(執行結果)
// Author:       Jack Chen
// DESCRIPTION:  CMenu_Class的建構式
//////////////////////////////////////////////////////////////////////////////
int CItem_Class::CreateWindowsItem(CWnd* ParentWnd, HWND hwnd, int iIDNum, CRect WindowsRect)
{
	m_hWnd = hwnd;
	m_ParentMenu = (CMenu_Class*)ParentWnd;
	m_ParentWnd = ParentWnd;
	m_bBGImage      = true;
	m_bClickImage   = false;
	m_bThroughImage = false;
	m_bIsOptionType = false;
	m_iID = iIDNum;
	m_strFileName = "";
	m_pImage = NULL;
	m_WindowsItem = true;
	m_ObjectOffset.x = WindowsRect.left;
	m_ObjectOffset.y = WindowsRect.top;
	
	CWnd::Create(NULL, NULL, WS_CHILD|WS_VISIBLE | BS_OWNERDRAW, WindowsRect, ParentWnd, iIDNum);
	return m_region.CreateRectRgn(WindowsRect.left, WindowsRect.top, WindowsRect.right, WindowsRect.bottom);
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     ~CItem_Class  
// INPUT:        void
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  CItem_Class的解構子
//////////////////////////////////////////////////////////////////////////////
CItem_Class::~CItem_Class(void)
{
	//CUI_Class::~CUI_Class();
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     Redraw  
// INPUT:        bErase(預留), DrawType(畫的方式)
// OUTPUT:       BOOL(是否成功執行)
// Author:       Jack Chen
// DESCRIPTION:  繪製此UI的背景圖，如果本身是MFC的 Item則呼叫UpdateWindow的函式
//               DrawType: 
//               Redraw_BGImage      畫背景圖
//               Redraw_ClickImage   畫滑鼠按下圖
//               Redraw_ThroughImage 畫滑鼠經過圖
//////////////////////////////////////////////////////////////////////////////
int CItem_Class::Redraw(bool bErase, int DrawType)
{
	if(m_WindowsItem)
	{
		UpdateWindow();
		return OK;
	}

	if(m_bClicked)
		return CUI_Class::Redraw(bErase, Redraw_ClickImage);
	else
		return CUI_Class::Redraw(bErase, DrawType);	
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     OnLButtonDown  
// INPUT:        UINT nFlags, CPoint point
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  滑鼠按下
//////////////////////////////////////////////////////////////////////////////
void CItem_Class::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	OnBnClicked();

	//CUI_Class::OnLButtonDown(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     OnLButtonUp  
// INPUT:        UINT nFlags, CPoint point
// OUTPUT:       void
// Author:       Jack Chen
// DESCRIPTION:  滑鼠彈起
//////////////////////////////////////////////////////////////////////////////
void CItem_Class::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if(!m_bIsOptionType)  //如果Item是Option形態且已按下則不彈起
	{
		m_bClicked = false;
	}

	if(m_bClicked == false) Redraw(true, Redraw_ThroughImage);

	m_ParentWnd->SendMessage(m_iID, DEF_UI_CLICK_MSG, m_bClicked);

	//m_ParentMenu->DoItemClickFunction(m_iID, Cur_ItemStatus);

	CUI_Class::OnLButtonUp(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:     Initialize  
// INPUT:        void
// OUTPUT:       int(執行結果)
// Author:       Jack Chen
// DESCRIPTION:  將Item初始化
//////////////////////////////////////////////////////////////////////////////
int  CItem_Class::Initialize()
{
	/*while(m_ParentWnd->IsKindOf(RUNTIME_CLASS(CUI_Class)))
	{
		m_ParentWnd = m_ParentWnd->GetParent();
	}*/
	int iRet = CUI_Class::Initialize();
	return iRet;
}

void CItem_Class::OnBnClicked()
{ 
	if (m_bIsOptionType)
		m_bClicked = !m_bClicked;
	else
		m_bClicked = true;

	if (m_bClickImage && m_bClicked)
		Redraw(true, Redraw_ClickImage);
	else
		Redraw(true, Redraw_BGImage);

	if (m_bClicked && m_iGroupIndex > 0)
		m_ParentMenu->DoItemClickFunction(m_iID, m_iGroupIndex);//Cur_ItemStatus
}

void CItem_Class::OnEnable(BOOL bEnable)
{
	CUI_Class::OnEnable(bEnable);

	// TODO: 在此加入您的訊息處理常式程式碼
}
