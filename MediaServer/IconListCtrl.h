#pragma once

#ifndef _IconListCtrl_H_
#define _IconListCtrl_H_


class IconListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(IconListCtrl)

	CImageList m_ImageList;
	HBRUSH m_hBrush;

public:
	int m_iSelIdx;
	ULONG m_ulSelID;
	bool m_bListChg;

	IconListCtrl();
	~IconListCtrl();

	void Init();
	void Create(int x, int y, int w, int h, CWnd* pParent, unsigned long ulID);
	void Clean();
	void AddItem(char* szName, unsigned char* pIconBuf, unsigned long ulID);

protected:

	//{{AFX_MSG(IconListCtrl)
	//afx_msg void OnClicked();
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	COLORREF textBGColor;
	CBrush cBrush;
	COLORREF m_BGColor;
	COLORREF m_FGColor;
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
};



#endif
