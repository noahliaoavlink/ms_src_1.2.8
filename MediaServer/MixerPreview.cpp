#include "stdafx.h"
#include "MixerPreview.h"

void MixerPreview::SetImageStartLT(const CPoint & OriLT, const CPoint & OriRB, const CPoint & ZoomLT, const CPoint & ZoomRB)
{
	CRect ZoomedRect(ZoomLT, ZoomRB);

	if ((OriLT.x <= ZoomLT.x) && (ZoomLT.x <= OriRB.x))
		m_ImageStartLT.x = 0;
	else //only occur on OriLT.x > ZoomLT.x
		m_ImageStartLT.x = LONG(float(m_iWidth*(OriLT.x - ZoomLT.x)) / ZoomedRect.Width());

	if ((OriLT.y <= ZoomLT.y) && (ZoomLT.y <= OriRB.y))
		m_ImageStartLT.y = 0;
	else //only occur on OriLT.y > ZoomLT.y
		m_ImageStartLT.y = LONG(float(m_iHeight*(OriLT.y - ZoomLT.y)) / ZoomedRect.Height());
}

void MixerPreview::SetZoomedRealLT(const CPoint & OriLT, const CPoint & OriRB, const CPoint & ZoomLT, const CPoint & ZoomRB)
{
	int IntVal_x, IntVal_y;

	if (ZoomLT.x<OriLT.x)
		IntVal_x = 0;
	else
		IntVal_x = abs(ZoomLT.x - OriLT.x);

	if (ZoomLT.y<OriLT.y)
		IntVal_y = 0;
	else
		IntVal_y = abs(ZoomLT.y - OriLT.y);

	m_ZoomedRealLT.x = int(float(m_iWidth*IntVal_x) / (OriRB.x - OriLT.x));
	m_ZoomedRealLT.y = int(float(m_iHeight*IntVal_y) / (OriRB.y - OriLT.y));
}

MixerPreview::MixerPreview()
{
	m_iHeight = m_iWidth = 0;
	m_ZoomedRect = { 0, 0, 0, 0 };
	m_iListIdx = -1;
	m_pDisplayManager = NULL;
	m_bPlaying = FALSE;
	m_fZoomRate = 1.0;
	m_pMediaStream = NULL;
}

MixerPreview::~MixerPreview()
{
	if (m_pDisplayManager)
	{
		m_pDisplayManager->UsingD3D(0);
		delete m_pDisplayManager;
		m_pDisplayManager = NULL;
	}

	if (m_pD3DRender)
		delete m_pD3DRender;

}

void MixerPreview::RenderSurface(IDirect3DSurface9* pBuffer, int iWidth, int iHeight)
{
	if (pBuffer && iWidth > 0 && iHeight > 0)
	{
		m_pD3DRender->RenderFrame1(pBuffer, iWidth, iHeight);
		m_pD3DRender->Flip();
	}
}

void MixerPreview::RenderFrame(unsigned char* pBuffer, int iWidth, int iHeight)
{
	if (pBuffer)
	{
		m_pD3DRender->RenderFrame(pBuffer, iWidth, iHeight);
		m_pD3DRender->Flip();
	}
}

void MixerPreview::CalImageSiteLT(const CRect & NoZoomedSym, const CRect & ZoomedSym)
{
	CPoint tmpShowLT, tmpShowRB;

	//set LeftTop x,y of showed image
	if (ZoomedSym.left < NoZoomedSym.left)
		tmpShowLT.x = NoZoomedSym.left;
	else
		tmpShowLT.x = ZoomedSym.left;

	if (ZoomedSym.top < NoZoomedSym.top)
		tmpShowLT.y = NoZoomedSym.top;
	else
		tmpShowLT.y = ZoomedSym.top;

	//get RightBottom x,y of showed image
	if (ZoomedSym.right > NoZoomedSym.right)
		tmpShowRB.x = NoZoomedSym.right;
	else
		tmpShowRB.x = ZoomedSym.right;

	if (ZoomedSym.bottom > NoZoomedSym.bottom)
		tmpShowRB.y = NoZoomedSym.bottom;
	else
		tmpShowRB.y = ZoomedSym.bottom;

	m_ZoomedRect.SetRect(tmpShowLT.x, tmpShowLT.y, tmpShowRB.x, tmpShowRB.y);

	//src bottom right

	if ((NoZoomedSym.left <= ZoomedSym.right) && (ZoomedSym.right <= NoZoomedSym.right))
		m_iZoomedSrcWidth = m_iWidth;
	else //only occur on ZoomedSym.right > NoZoomedSym.right
		m_iZoomedSrcWidth = int(float(m_iWidth*m_ZoomedRect.Width()) / ZoomedSym.Width());

	if ((NoZoomedSym.top <= ZoomedSym.bottom) && (ZoomedSym.bottom <= NoZoomedSym.bottom))
		m_iZoomedSrcHeight = m_iHeight;
	else //only occur on zoomedSym.bottom > NoZoomedSym.bottom
		m_iZoomedSrcHeight = int(float(m_iHeight*m_ZoomedRect.Height()) / ZoomedSym.Height());


	//dst bottom right
	m_iZoomedDstWidth = int(float(m_iWidth*(m_ZoomedRect.right - NoZoomedSym.left)) / NoZoomedSym.Width());
	m_iZoomedDstHeight = int(float(m_iHeight*(m_ZoomedRect.bottom - NoZoomedSym.top)) / NoZoomedSym.Height());


	//dst top left

	SetZoomedRealLT(NoZoomedSym.TopLeft(), NoZoomedSym.BottomRight(), ZoomedSym.TopLeft(), ZoomedSym.BottomRight());

	//src top left

	SetImageStartLT(NoZoomedSym.TopLeft(), NoZoomedSym.BottomRight(), ZoomedSym.TopLeft(), ZoomedSym.BottomRight());
}

void MixerPreview::SetWH(int width, int height)
{
	m_iHeight = height;
	m_iWidth = width;
}

#if 0
//MixerPreview inherit from CStatic
void MixerPreview::Create(const CRect & area, CWnd * Parent, int IID)
{
	m_pParentWnd = Parent;
    CStatic::Create(_T("A bitmap static control"), WS_CHILD | WS_BORDER | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE | SS_NOTIFY, area, Parent, IID);
}
BEGIN_MESSAGE_MAP(MixerPreview, CStatic)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void MixerPreview::OnLButtonDown(UINT nFlags, CPoint point)
{
	/*
	RECT rect;
	HDC hDC = ::GetDC(m_hWnd);
	
	::GetWindowRect(m_hWnd, &rect); //get world coordination of cstatic in MixerDlg
	*/
	
	CPen penGreen(PS_SOLID, 1, RGB(0, 255, 0));

	HDC hDC = ::GetDC(m_hWnd);

	SetROP2(hDC, R2_XORPEN);

	switch (GetDlgCtrlID())
	{
		case IDC_STATIC_PREVIEW1:
			DrawRect(m_hWnd, hDC, penGreen.operator HPEN());
			break;
		case IDC_STATIC_PREVIEW2:break;
		case IDC_STATIC_MIXER:break;
	}

	::ReleaseDC(m_hWnd, hDC);
	
	CStatic::OnLButtonDown(nFlags, point);
}
/*
畫出充滿紅色的矩形

CClientDC cdc(this);
CRect rectTemp(0, 0, 200, 200);
CBrush redBursh(RGB(255, 0, 0));
cdc.FillRect(&rectTemp, &redBursh);

畫線成功

CPen penGreen(PS_SOLID, 5, RGB(0, 255, 0));
cdc.SelectObject(&penGreen);
cdc.MoveTo(CPoint(0, 0));
cdc.LineTo(CPoint(200, 200));

畫線成功 但無法指定顏色 顏色為黑 故不見

HDC hDC = ::GetDC(m_hWnd);

DrawLine(hDC, 50, 0, 50, 500);

::ReleaseDC(m_hWnd, hDC);

直接在CStatic外圍畫框
DrawRect(m_hWnd, penGreen.operator HPEN());


實驗路徑D:\SvnMs\x64\Debug\Obj 是否為唯讀
CFile file;
BOOL F = file.Open(_T("D:\\SvnMs\\x64\\Debug\\Obj\\Temp.xml"), CFile::modeReadWrite | CFile::modeNoTruncate | CFile::modeCreate);
file.Write("a", 1);
file.Close();

*/

#else
//MixerPreview composite CStatic
void MixerPreview::Create(const CRect & area, CWnd * Parent, int IID, CBitSlider* video_slider)
{
	m_pVideoSlider = video_slider;

	m_pParentWnd = Parent;
	m_PicCtrl.Create(_T("A bitmap static control"), WS_CHILD | WS_BORDER | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE, area, m_pParentWnd, IID);

	m_pDisplayManager = new CShapeDispManager;

	m_pD3DRender = new D3DRender;
	m_pD3DRender->Init(m_PicCtrl.m_hWnd, VPF_RGB32, 1920, 1080);
	m_pD3DRender->Clear();
	m_pD3DRender->Flip();

	m_pDisplayManager->Init(m_PicCtrl.m_hWnd, m_pD3DRender->GetD3DDevice());
}
#endif


