#if !defined(AFX_BITSLIDER_H__F5B7057C_6520_4A86_B61A_345F62304099__INCLUDED_)
#define AFX_BITSLIDER_H__F5B7057C_6520_4A86_B61A_345F62304099__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Author	:	Jackey
//	Date	:	2004.02.19
//	File	:	BitSlider.h
//	Home	:	http://www.kuihua.net/
//	Contains:	Define the CBitSlider class...
//	History	:
//		1.0	:	2004.02.19 - First Version...
//	Mailto	:	Omega@Kuihua.net (Bug Report or Comments)
////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BitWnd.h"

class CBitItem;

class CBitSlider : public CSliderCtrl
{
public:
	CBitSlider(int MxPrwid=-1/*Allen add*/);
	virtual ~CBitSlider();
public:
	//{{AFX_VIRTUAL(CBitSlider)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CBitSlider)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void			SetLeftOffset(int nOffset)		{ m_nOffset = nOffset; }
	void			SetTopOffset(int nOffset)		{ m_nOffset = nOffset; }
	void			SetFlipCursor(HCURSOR hCursor)	{ m_hHand = hCursor; m_Thumb.SetFlipCursor(hCursor); }

	void			BuildBackItem(UINT nNormalRes, UINT nActiveRes);

#ifdef _ENABLE_BMP_FILE
	void			Create(int x, int y, int w, int h, CWnd* pParent, unsigned long ulID);
	void			CreateV(int x, int y, int w, int h, CWnd* pParent, unsigned long ulID);
	void			BuildThumbItem(char* szFileName, int cx, int cy, int iState);
	void			BuildBackItem(char* szNormalFileName, char* szActiveFileName);
	void			DrawBMP(ImageFile* pImgFile, int x, int y, int w, int h, int iSrcX, int iSrcY, int iSrcW, int iSrcH);
	void			SetPosV(int iPos);
	int				GetPosV();
#else
	void			BuildThumbItem(UINT nThumbRes, int cx, int cy);
	void			BuildThumbItem(CBitItem * lpBit);
#endif

	void			BuildBackItem(CBitItem * lpNBit, CBitItem * lpABit);

	void			DestroyBackItem();
private:
	void			UpdateToolTips();
	void			DrawSlider(CDC * pDC, BOOL bPaint);
	void			GetNewThumbRect(CRect & rcThumb);
private:
	CBitWnd			m_Thumb;
	int m_iMxPrwid;//Allen add

#ifdef _ENABLE_BMP_FILE
	ImageFile* m_pNormalBmpFile;
	ImageFile* m_pActiveBmpFile;

	HDC m_hMemDC;
#else
	CBitItem	*	m_lpActive;
	CBitItem	*	m_lpNormal;
#endif
	HCURSOR			m_hHand;

	BOOL			m_bAllocFlag;
	int				m_nOffset;

	CToolTipCtrl	m_ctlTips;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITSLIDER_H__F5B7057C_6520_4A86_B61A_345F62304099__INCLUDED_)