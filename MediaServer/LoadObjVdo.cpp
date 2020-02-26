// LoadObjVdo.cpp : 實作檔
//

#include "stdafx.h"
#include "MediaServer.h"
#include "LoadObjVdo.h"
#include "afxdialogex.h"
#include "../../../Include/SCreateWin.h"

// CLoadObjVdo 對話方塊

IMPLEMENT_DYNAMIC(CLoadObjVdo, CDialog)

CLoadObjVdo::CLoadObjVdo(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_LOADOBJ_DLG, pParent)
{
	m_bLockLoadObj = FALSE;
	m_bExtraVideo = FALSE;
	m_strVideoPath = "";
	m_strObjPath = "";
	m_iDefaultObjStatus = 0;

}

CLoadObjVdo::~CLoadObjVdo()
{
}

void CLoadObjVdo::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_OBJ_PATH_EDIT, m_ObjPathEdit);
	DDX_Control(pDX, IDC_VIDEO_PATH_COMB0, m_VideoPathBox);
	DDX_Control(pDX, IDC_ZOOM_COMBO, m_ZoomComboBox);
	DDX_Control(pDX, IDC_ZOOM_SLIDER, m_ZoomSildCtrl);

	/*if (m_ViewComboBox.GetCount() == 0)
	{
		CString strTemp;
		for (UINT x = 1; x <= m_iMaxViewNum; x++)
		{
			strTemp.Format("%i", x);
			m_ViewComboBox.AddString(strTemp);
		}
		m_ViewComboBox.SetCurSel(0);
	}
	m_iSelViewPort = m_ViewComboBox.GetCurSel() + 1;
	DDX_Control(pDX, IDC_VIEWPORT_COMBO, m_ViewComboBox);*/

	if (m_VideoPathBox.GetCount() == 0)
	{
		CString strTemp;
		CString strFilePath = "";
		//for (UINT x = 0; x < g_StreamMappingAry.GetCount(); x++)
		for (UINT x = 0; x < g_MediaStreamAry.size(); x++)
		{
			strFilePath = g_MediaStreamAry.at(x)->FilePath();
			if (!strFilePath.IsEmpty() && !g_MediaStreamAry.at(x)->IsAudioFile())
			{
				/*for (UINT y = 0; y < g_StreamMappingAry.GetCount(); y ++)
				{
					if (g_StreamMappingAry.GetAt(y).iMediaIdx == x)
					{
						strFilePath = g_StreamMappingAry.GetAt(y).strVideoPath;
						break;
					}
				}*/

				if (g_MediaStreamAry[x]->IsOpened())
				{
					if (strFilePath.Left(6).Find("Video ") == -1)
						strTemp.Format("Video %i (%s)", x + 1, strFilePath);
					else
						strTemp = strFilePath;
					m_VideoPathBox.AddString(strTemp);
				}
			}
		}
		m_VideoPathBox.SetCurSel(0);
	}

	if (m_VideoPathBox.GetCount() > 0)
	{
		m_VideoPathBox.EnableWindow();
		INT32 iSelIdx = m_VideoPathBox.GetCurSel();
		INT32 iStrLen = m_VideoPathBox.GetLBTextLen(iSelIdx);
		m_VideoPathBox.GetLBText(iSelIdx, m_strVideoPath);

		if (0)//m_strVideoPath.Left(6).Find("Video ") != -1)
		{
			INT32 iStrPos = 0, iVideoIdx = 0;
			CString strVideoIdx;			
			strVideoIdx = m_strVideoPath.Tokenize(" ", iStrPos);
			strVideoIdx = m_strVideoPath.Tokenize(" ", iStrPos);

			iVideoIdx = atoi(strVideoIdx);
			m_strVideoPath.Format("Video %i (%s)", iVideoIdx, g_MediaStreamAry.at(iVideoIdx - 1)->FileName());
		}
	}

	if (m_ZoomComboBox.GetCount() == 0)
	{
		CString strTemp;
		INT32 iIndex = 1;
		for (int x = 0; x <= 10; x++)
		{
			if (x < 5)
				strTemp.Format("1 / %.0f", pow(2, 5 - x));
			else
				strTemp.Format("x %.0f", pow(2, x - 5));
			
			m_ZoomComboBox.AddString(strTemp);
		}

		m_ZoomComboBox.SetCurSel(5);

		m_ZoomSildCtrl.SetRange(0, 10);
		m_ZoomSildCtrl.SetPos(5);
	}

	m_dZoomRatio = pow(2.0f, double(m_ZoomSildCtrl.GetPos() - 5));


	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoadObjVdo, CDialog)
	ON_BN_CLICKED(IDC_OPEN_VIDEO_PATH, &CLoadObjVdo::OnBnClickedOpenVideoPath)
	ON_BN_CLICKED(IDC_OPEN_OBJ_PATH, &CLoadObjVdo::OnBnClickedOpenObjPath)
	ON_CBN_SELCHANGE(IDC_VIDEO_PATH_COMB0, &CLoadObjVdo::OnCbnSelchangeVideoPathComb0)
	ON_CBN_SELCHANGE(IDC_ZOOM_COMBO, &CLoadObjVdo::OnCbnSelchangeZoomCombo)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_ZOOM_SLIDER, &CLoadObjVdo::OnNMCustomdrawZoomSlider)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_OPEN_VIDEO_PATH2, &CLoadObjVdo::OnBnClickedOpenVideoPath2)
	ON_BN_CLICKED(IDOK, &CLoadObjVdo::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SAVE_DEFAULT_CHK, &CLoadObjVdo::OnBnClickedSaveDefaultChk)
END_MESSAGE_MAP()


// CLoadObjVdo 訊息處理常式


BOOL CLoadObjVdo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化

	if (m_bLockLoadObj)
	{
		GetDlgItem(IDC_OBJ_PATH_EDIT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OPEN_OBJ_PATH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OBJ_FAPTH_STATIC)->ShowWindow(SW_HIDE);
	}

	if (m_iDefaultObjStatus == 0)
	{
		GetDlgItem(IDC_SAVE_DEFAULT_CHK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_INDEX_COMBO)->ShowWindow(SW_HIDE);
	}
	else
	{
		CString strTemp;
		for (int x = 0; x < 9; x++)
		{
			strTemp.Format("%i", x + 1);
			((CComboBox*)GetDlgItem(IDC_INDEX_COMBO))->AddString(strTemp);
		}

		((CComboBox*)GetDlgItem(IDC_INDEX_COMBO))->SetCurSel(0);

	}
	
	if (m_iDefaultObjStatus == 1)
	{
		GetDlgItem(IDC_SAVE_DEFAULT_CHK)->SetWindowTextA("Save To User Define");
		GetDlgItem(IDC_INDEX_COMBO)->EnableWindow(FALSE);
		GetDlgItem(IDC_INDEX_COMBO)->ShowWindow(SW_SHOW);

	}
	else if (m_iDefaultObjStatus == 2)
	{
		GetDlgItem(IDC_SAVE_DEFAULT_CHK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LOADDEF_STATIC)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_INDEX_COMBO)->ShowWindow(SW_SHOW);

	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CLoadObjVdo::OnBnClickedOpenVideoPath()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	if (m_VideoPathBox.GetCount() >= 10)
	{
		AfxMessageBox("Can't Get More Video", MB_TOPMOST);
		return;
	}

	CFileDialog fileVideoDlg(TRUE, "txt", "", OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_OVERWRITEPROMPT,
		"MP4 Files (*.mp4)|*.mp4|MKV Files (*.mkv)|*.mkv|MPEG Files (*.mpg)|*.mpg|MOV Files (*.mov)|*.mov|WMV Files (*.wmv)|*.wmv|VOB Files (*.vob)|*.vob|RMVB Files (*.rmvb)|*.rmvb|ALL Files (*.*)|*.*||");
	if (fileVideoDlg.DoModal() == IDOK)
	{
		m_strVideoPath = fileVideoDlg.GetPathName();
		m_VideoPathBox.EnableWindow();
		m_VideoPathBox.InsertString(0, m_strVideoPath);
		m_VideoPathBox.SetCurSel(0);
		m_bExtraVideo = TRUE;
	}
}


void CLoadObjVdo::OnBnClickedOpenObjPath()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CFileDialog fileObjDlg(TRUE, "", "", OFN_FILEMUSTEXIST, "*(*.obj)|*.obj|", this, sizeof(OPENFILENAME));
	if (fileObjDlg.DoModal() == IDOK)
	{
		m_strObjPath = fileObjDlg.GetPathName();
		m_ObjPathEdit.SetWindowText(m_strObjPath);
	}
}


void CLoadObjVdo::OnCbnSelchangeVideoPathComb0()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	INT32 iSelIdx = m_VideoPathBox.GetCurSel();
	INT32 iStrLen = m_VideoPathBox.GetLBTextLen(iSelIdx);
	m_VideoPathBox.GetLBText(iSelIdx, m_strVideoPath);
}


void CLoadObjVdo::OnCbnSelchangeZoomCombo()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	INT32 iZoomIdx = m_ZoomComboBox.GetCurSel();
	m_ZoomSildCtrl.SetPos(iZoomIdx);
}
  
void CLoadObjVdo::OnNMCustomdrawZoomSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	*pResult = 0;

	INT32 iZoomIdx = m_ZoomSildCtrl.GetPos();
	m_ZoomComboBox.SetCurSel(iZoomIdx);
}


void CLoadObjVdo::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (nIDEvent == 0)
	{
		if (!theApp.m_strSelVideoPath.IsEmpty())
		{
			if (theApp.m_strSelVideoPath.GetLength() > 5)
			{
				m_strVideoPath = theApp.m_strSelVideoPath;
				m_VideoPathBox.EnableWindow();
				m_VideoPathBox.InsertString(0, m_strVideoPath);
				m_VideoPathBox.SetCurSel(0);
			}
			KillTimer(0);
			theApp.m_strSelVideoPath = "";
			ShowWindow(SW_SHOW);
			SetFocus();
		}
	}

	CDialog::OnTimer(nIDEvent);
}


void CLoadObjVdo::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	KillTimer(0);

	CDialog::OnClose();
}


void CLoadObjVdo::OnBnClickedOpenVideoPath2()
{
	if (m_VideoPathBox.GetCount() >= 10)
	{
		AfxMessageBox("Can't Get More Video",MB_TOPMOST);
		return;
	}

	theApp.m_pMediaServerDlg->SendMessage(MSG_SELECT_MEDIA);
	ShowWindow(SW_HIDE);
	SetTimer(0, 100, NULL);
	return;
}


void CLoadObjVdo::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	CString strTemp;
	((CComboBox*)GetDlgItem(IDC_INDEX_COMBO))->GetWindowTextA(strTemp);
	strTemp = theApp.m_strCurPath + "\\Obj\\User Define" + strTemp + ".obj";

	if (m_iDefaultObjStatus == 1 && ((CButton*)GetDlgItem(IDC_SAVE_DEFAULT_CHK))->GetCheck())
	{
		CopyFile(m_strObjPath, strTemp, FALSE);
	}
	else if (m_iDefaultObjStatus == 2)
	{
		m_strObjPath = strTemp;
	}

	CDialog::OnOK();
}


void CLoadObjVdo::OnBnClickedSaveDefaultChk()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	if (((CButton*)GetDlgItem(IDC_SAVE_DEFAULT_CHK))->GetCheck())
		GetDlgItem(IDC_INDEX_COMBO)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_INDEX_COMBO)->EnableWindow(FALSE);
}
