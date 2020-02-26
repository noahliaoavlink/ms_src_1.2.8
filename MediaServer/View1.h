#pragma once


// CView1 view

class CView1 : public CView
{
	DECLARE_DYNCREATE(CView1)

public:
	CView1();           // protected constructor used by dynamic creation
	virtual ~CView1();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


