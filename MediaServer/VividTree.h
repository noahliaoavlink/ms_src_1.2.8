///////////////////////////////////////////////////////////////////////////
// VividTree.h : Definition of Class VividTree
///////////////////////////////////////////////////////////////////////////
//
// Copyright(C) 2005 Jim Alsup  All rights reserved
//              email: ja.alsup@gmail.com
//
// License: This code is provided "as is" with no expressed or implied 
//          warranty. You may use, or derive works from this file without
//          any restrictions except those listed below.
//
//        - This original header must be kept in the derived work.
//
//        - If your derived work is distributed in any form, you must
//          notify the author via the email address above and provide a 
//          short description of the product and intended audience.  
//
//        - You may not sell this code or a derived version of it as part of 
//          a comercial code library. 
//
//        - Offering the author a free licensed copy of any end product 
//          using this code is not required, but does endear you with a 
//          bounty of good karma.
//
///////////////////////////////////////////////////////////////////////////

#pragma once

#include "resource.h"
//#include "CamData.h"

#include "afxadv.h"
//#include "../../../Include/PluginObjMsg.h"
#include "../../../Include/MediaFileCommon.h"

#define SERVER_NODE 0
#define DVR_NODE 1
#define CAM_NODE 2

#define IDC_ADD_FOLDER 2001
#define IDC_ADD_FILE 2002
#define IDC_DEL 2003
#define IDC_EDIT_TAG 2004
#define IDC_SEARCH_TAG 2005
#define IDC_ADD_RTSP_URL 2006
#define IDC_TEST 2007

typedef struct
{
	int iType;
	int iID;
}NodeData;

// Helper Routine
extern void GradientFillRect( CDC *pDC, CRect &rect, COLORREF col_from, COLORREF col_to, bool vert_grad );


class VividTree : public CTreeCtrl
{
	DECLARE_DYNAMIC(VividTree)

public:
	VividTree();
	virtual ~VividTree();

// Attributes
	void SetBkGradients( COLORREF from, COLORREF to ){	m_gradient_bkgd_from = from; 
	                                                    m_gradient_bkgd_to = to; }
	COLORREF GetBkGradientFrom( ){	return m_gradient_bkgd_from; }
	COLORREF GetBkGradientTo( ){ return m_gradient_bkgd_to; }

	enum BkMode { BK_MODE_BMP = 0, BK_MODE_GRADIENT, BK_MODE_FILL };
	BkMode GetBkMode( ) { return m_bkgd_mode; }
	void SetBkMode( BkMode bkgd_mode ) { m_bkgd_mode = bkgd_mode; }

	bool GetBitmapTiledMode() { return m_bmp_tiled_mode;}
	void SetBitmapTiledMode( bool tiled ) {	m_bmp_tiled_mode = tiled; }
	void SetBitmapID( UINT id );

	bool GetGradientHorz() { return m_gradient_horz; }
	void SetGradientHorz( bool horz ) { m_gradient_horz = horz; } 

	
	virtual void  SetItemIcon( HICON icon ) { m_icon = icon; }; // Overridable
	virtual HICON GetItemIcon( HTREEITEM item ); // Overridable

// Operations
	bool ItemIsVisible( HTREEITEM item );

	void SetEventCallback(EventCallback* pObj);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	COLORREF m_gradient_bkgd_from;		// Gradient variables
	COLORREF m_gradient_bkgd_to;
	COLORREF m_gradient_bkgd_sel;
	bool     m_gradient_horz;			// horz or vertical gradient

	BkMode  m_bkgd_mode;				// Current Background mode
    CBitmap m_bmp_back_ground;			// Background bitmap image
	CBitmap m_bmp_tree_closed;			// Tree open image (marker)
	CBitmap m_bmp_tree_open;			// Tree closed image (marker)
	bool    m_bmp_tiled_mode;			// Tile background image

	CRect m_rect;						// The client rect when drawing
	int   m_h_offset;					// 0... -x (scroll offset)
	int   m_h_size;						// width of unclipped window
	int   m_v_offset;					// 0... -y (scroll offset)
	int   m_v_size;						// height of unclipped window

	HICON m_icon;

	void DrawBackGround( CDC* pDC );	// Background Paint code
	void DrawItems( CDC* pDC );  // Item Paint Code

	UINT       m_DragDropFormat;
	TVITEM tv_item;

	EventCallback* m_pEventCallback;
	POINT m_ptTmp;
	HTREEITEM m_hTmpTreeItem;

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNMRclickTreectrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnUpdateList(WPARAM wParam, LPARAM lParam);
//	afx_msg void OnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
};


