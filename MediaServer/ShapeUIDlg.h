#pragma once


// CShapeUIDlg 對話方塊
#include "Item_Class.h"
#include "SetGridDlg.h"
#include "UTipDll.h"

#ifdef _ENABLE_SPF
#include "ZLibDllObj.h"
#endif

#define _LOAD_OBJ_THREAD_MODE 1

typedef struct
{
	void* pObj;
	CString szProjectFileName;
}ShapeThreadItem;

class CShapeUIDlg : public CDialog
{
	DECLARE_DYNAMIC(CShapeUIDlg)

public:
	CShapeUIDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CShapeUIDlg();

	enum
	{
		ITEM_MOVE = WM_USER + 0x300,
		ITEM_ROTATE,
		ITEM_ZOOM,
		ITEM_ZOOM_ALL,
		ITEM_ZOOM_ONE,
		ITEM_MAP,
		ITEM_VIDEO,
		ITEM_LINE,
		ITEM_PTMOVE,

		ITEM_LOADOBJ,
		ITEM_BALL,
		ITEM_CUBE,
		ITEM_4PYRAMID,
		ITEM_FIG,
		ITEM_FLAT,
		ITEM_USERDEFINE,
		ITEM_FROMMIX, //triangle
		ITEM_BEAR,
		ITEM_CONE,
		ITEM_DISC,
		ITEM_VIEW1,
		ITEM_VIEW2,
		ITEM_VIEW3,
		ITEM_VIEW4,
		ITEM_VIEW5,
		ITEM_VIEW6,
		ITEM_VIEW7,
		ITEM_VIEW8,
		ITEM_OBJ1,
		ITEM_OBJ2,
		ITEM_OBJ3,
		ITEM_OBJ4,
		ITEM_OBJ5,
		ITEM_OBJ6,
		ITEM_OBJ7,
		ITEM_OBJ8,
		ITEM_OBJ9,
		ITEM_OBJ10,
		ITEM_NEW,
		ITEM_OPEN,
		ITEM_SAVE,
		ITEM_PTMOVE_SELECT,
		ITEM_PTMOVE_MOVE,
		ITEM_PTMOVE_OK,
		ITEM_PTMOVE_CANCEL,
		ITEM_OBJ_DEL,
		ITEM_ROTATE_X,
		ITEM_ROTATE_Y,
		ITEM_ROTATE_Z,
		ITEM_ROTATE_ALL,
		ITEM_ROTATE_ANGLE,
		ITEM_MODEL_DEL,
		ITEM_CHANGE_VIDEO,
		ITEM_UNDO,
		ITEM_REDO,
		ITEM_2DMAP,
		ITEM_2DMAP_SetGrid,
		ITEM_2DMAP_SetAlpha,
		ITEM_2DMAP_SELECT,
		ITEM_2DMAP_MOVE,
		ITEM_2DMAP_OK,
		ITEM_2DMAP_CANCEL,
		ITEM_2DMAP_EDGEBLEND,
		ITEM_2DMAP_SHOWPT,
		ITEM_LOCK_UI,
		ITEM_LAST
	};

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEMP_DLG };
#endif

protected:
	CRITICAL_SECTION m_csFunBsy;
	CSetGridDlg m_dlgSetGrid;
	CMenu_Class* m_FramePic;
	INT32 m_iMaxView;
	INT32 m_iObjNum;
	INT32 m_iSelObjIdx;
	double m_fZoomX, m_fZoomY;
	INT32 m_iCurTempObjIdx;
	BOOL m_bHadInit2DMap;

#ifdef _LOAD_OBJ_THREAD_MODE
	bool m_bShow;
	UINT m_CurMessage;
#endif

	bool m_bPositionMode;

#ifdef _ENABLE_SPF
	ZLibDllObj* m_pZLibDllObj;
#endif

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	void SetProjectorNum(INT32 iProjectorNum);
	void ShowObjMatrix(CClientDC &cdc, INT32 iPanelIdx, INT32 iObjIdx, CString strProjectPath = "Shape.xml");
	CFont *m_pNewFont;
	CComboBox m_VideoIdxCombox;
	CEdit m_AngleEdit;
	CStatic m_sticVideoPath;

	UTipDll* m_pUTipDll;
	ShapeThreadItem thread_item;

	void ShowSubItem();
	BOOL NewProject(BOOL bOpen);

	DECLARE_MESSAGE_MAP()
public:
	void LoadProject(CString strProjectPath);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	void SetModalNum(INT32 iModalNum, BOOL bClickLastModel = FALSE);
	void CreateDefultGama();

	afx_msg LRESULT OnSetModalNum(WPARAM wParam, LPARAM lParam);

#ifdef _LOAD_OBJ_THREAD_MODE
	void LoadObj4Thread(UINT message);
	void LoadObj();
#endif

#ifdef _ENABLE_SPF
	void UnZipProjectFile(char* szFileName);
	bool IsSPF(char* szFileName);
#endif

	void EnableObjButtons(bool bEnable);
	void OpenProject();
	void OpenProject2(CString strProjectPath);
	void AddTip(wchar_t* wszTipID, int left, int top, int right, int bottom);
	void SetTip();
	bool CheckObjFile(CString strObjPath);
};
