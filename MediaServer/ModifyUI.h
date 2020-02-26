#pragma once
#include "menu_class.h"

// CModifyUI ��ܤ��

class CModifyUI : public CDialog
{
	DECLARE_DYNAMIC(CModifyUI)

public:
	CModifyUI(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~CModifyUI();


	typedef struct typParmName
	{
		CString strParmName[6];

	}PARM_NAME;

	typedef enum
	{
		ENGLISH_TYE,
		CHINESS_TYE
	}LANGUAGE_TYPE;

// ��ܤ�����
	enum { IDD = IDD_MODIFYUI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

	CMenu_Class* m_FramePic;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	BOOL m_bShowDlg;
	BOOL m_b3DCtrl;
	LANGUAGE_TYPE m_iCurLang;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangePannelList();
	CArray<PARM_NAME, PARM_NAME> m_ParmNameAry;

	afx_msg void OnCbnSelchangeImgEffectList();
};
