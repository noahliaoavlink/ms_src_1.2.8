// View1.cpp : implementation file
//

#include "stdafx.h"
//#include "StaticSplitTest.h"
#include "View1.h"


// CView1

IMPLEMENT_DYNCREATE(CView1, CView)

CView1::CView1()
{

}

CView1::~CView1()
{
}

BEGIN_MESSAGE_MAP(CView1, CView)
END_MESSAGE_MAP()


// CView1 drawing

void CView1::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here

	// create and select a solid blue brush
	CBrush brushBlue(RGB(0, 0, 255));
	CBrush* pOldBrush = pDC->SelectObject(&brushBlue);

	// create and select a thick, black pen
	CPen penBlack;
	penBlack.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&penBlack);

	// get our client rectangle
	CRect rect;
	GetClientRect(rect);

	// shrink our rect 20 pixels in each direction
	rect.DeflateRect(20, 20);

	// draw a thick black rectangle filled with blue
	pDC->Rectangle(rect);

	// put back the old objects
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}


// CView1 diagnostics

#ifdef _DEBUG
void CView1::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CView1::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CView1 message handlers
