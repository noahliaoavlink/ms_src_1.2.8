#pragma once

#include "Item_Class.h"

#include "IOSourceCtrlDll.h"
#include "TreeCtrlUI.h"
#include "FileListCtrl.h"
#include "FileInfoCtrl.h"
#include "CSVReader.h"
#include "ImageFile.h"
#include "TxtButtonCtrl.h"
#include "WaitingDlg.h"
#include "LogFileDll.h"
#include "UTipDll.h"

// CMediaCtrlDlg dialog

class CMediaCtrlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMediaCtrlDlg)

public:
	CMediaCtrlDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMediaCtrlDlg();

	void LoadCSV(bool bReset = false);
	void FreeUIObj();
	void DrawDisplayModeIcon();
	void DrawBMP(ImageFile* pImgFile, int x, int y, int w, int h);
	bool InRect(RECT* pRect, int x, int y);
	IOSourceCtrlDll* GetIOSourceCtrlDll();
	CSVReader* GetCSVReader();
	void Reset();
	void ReCreate();
	void ReCreate2();
	void InsertFolderNode(char* szFolder);
	void InsertFileNode(char* szFolder, char* szFileName);
	void SetLogFileDll(LogFileDll* pObj);
	void AddTip(wchar_t* wszTipID, int left, int top, int right, int bottom);
	void SetTip();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEMP_DLG };
#endif

protected:
	enum
	{
		LIST_MODE = WM_USER + 0x300,
		ICON_MODE,
	};

	CMenu_Class* m_FramePic;
	IOSourceCtrlDll* m_pIOSourceCtrlDll;
	TreeCtrlUI* m_pTreeCtrlUI;
	FileListCtrl* m_pFileListCtrl;
	FileInfoCtrl* m_pFileInfoCtrl;
	CComboBox m_TagList;
	CSVReader m_CSVReader;

	ImageFile m_IconMode;
	ImageFile m_ListMode;
	int m_iMode;
	RECT m_rTab;
	RECT m_rIcon;
	RECT m_rList;

	TxtButtonCtrl* m_pResetBtn;
	TxtButtonCtrl* m_pReCreateBtn;
	//CWaitingDlg m_WaitingDlg;

	LogFileDll* m_pLogFileDll;
	UTipDll* m_pUTipDll;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg LRESULT OnButtonClick(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};
