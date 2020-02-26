#pragma once

#ifndef _FileListCtrl_H_
#define _FileListCtrl_H_

//#include "MyListCtrl.h"
#include "IOSourceCtrlDll.h"
#include "FileInfoCtrl.h"
#include "YUVConverter.h"

#define EF_EVT_SEL_CHANGED 2001 //SelChanged
#define MF_EVT_SEL_CHANGED 2002 //SelChanged

class NListCtrl : public CListCtrl , public WinMsgCallback
{
		CNWin* m_pNWin;
		HWND m_hWnd;
		HWND m_hParentWnd;
		bool m_bSimpleMode;
	public:
		NListCtrl();
		~NListCtrl();

		void Create(char* szName, int iID, int w, int h);
		void SetParent(HWND hWnd);
		void MoveWindow(int x, int y, int w, int h);
		void ShowWindow(bool bShow);
		void Reset();
		int GetNextSelectItem(int iSel);
		void ChangeStyle(int iStyle);
		void ReDraw();
		HWND GetParentWnd() { return m_hParentWnd; };

		void SetSimpleMode(bool bEnable);

		void* WinMsg(int iMsg, void* pParameter1, void* pParameter2);

protected:
	//afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//afx_msg void OnPaint();
	//afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnNMRclickTreectrl(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
	
	//DECLARE_MESSAGE_MAP()
};

class FileListCtrl : public NListCtrl //MyListCtrl
{
	IOSourceCtrlDll* m_pIOSourceCtrlDll;
	FileInfoCtrl* m_pFileInfoCtrl;
	int m_iMode;

	CImageList m_ImageList;
	YUVConverter m_yuv_converter;
	unsigned char* m_pRGB32Buf;

	HBRUSH m_hBrush;

	EventCallback* m_pEventCallback;
protected:
public:
	FileListCtrl();
	~FileListCtrl();
	
	void Init();
	void SetIOSourceCtrl(IOSourceCtrlDll* pObj);
	void SetFileInfoCtrl(FileInfoCtrl* pObj);
	void Clean();
	void InsertItem(int iIndex, wchar_t* wszFileName);
	void SetDisplayMode(int iMode);
	wchar_t* GetCurSelFileName();
	wchar_t* GetFirstFileName();
	void MoveWindow(int x, int y, int w, int h);

	void SetSimpleMode(bool bEnable);
	void SetEventCallback(void* pObj);

protected:
	//afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//afx_msg void OnPaint();
	//afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnNMRclickTreectrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomdrawMyList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
};

typedef struct
{
	char szName[128];
	char szEffectName[128];
	int iEffectID;
	int iLevel;
	int iParameter1;
	int iParameter2;
	int iParameter3;
	int iParameter4;
	int iParameter5;
}SMEffectItem;

class EffectListCtrl : public NListCtrl
{
	HBRUSH m_hBrush;
	SMEffectItem m_SMEffectItem;

	EventCallback* m_pEventCallback;
protected:
public:
	EffectListCtrl();
	~EffectListCtrl();

//	void Create(char* szName, int iID, int w, int h, CWnd* pParent);
	void MoveWindow(int x, int y, int w, int h);

	void Init();
	void ReDraw();
	void AddItem(SMEffectItem* pItem);
	void InsertItem(int iIndex, SMEffectItem* pItem);
	void UpdateItem(int iIndex, SMEffectItem* pItem);
	//void DeleteItem(int iIndex);
	SMEffectItem* GetItem(int iIndex);
	//void SetSimpleMode(bool bEnable);

	void SetEventCallback(void* pObj);

protected:

	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
};

#endif
