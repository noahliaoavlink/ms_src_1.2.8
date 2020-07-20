// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MediaServer.h"
#include "SettingsDlg.h"
#include "afxdialogex.h"
#include "TreeCtrlUI.h"
#include "../../../Include/SCreateWin.h"
#include "ServiceProvider.h"
#include "../../../Include/sstring.h"
#include "MediaServerDlg.h"

// CSettingsDlg dialog

DOItem g_OrderItems[] = {
	{ 0,"0,1,2,3" },
	{ 1,"0,1,3,2" },
	{ 2,"0,2,1,3" },
	{ 3,"0,2,3,1" },
	{ 4,"0,3,2,1" },
	{ 5,"0,3,1,2" },
	{ 6,"1,0,2,3" },
	{ 7,"1,0,3,2" },
	{ 8,"1,2,0,3" },
	{ 9,"1,2,3,0" },
	{ 10,"1,3,2,0" },
	{ 11,"1,3,0,2" },
	{ 12,"2,0,1,3" },
	{ 13,"2,0,3,1" },
	{ 14,"2,1,0,3" },
	{ 15,"2,1,3,0" },
	{ 16,"2,3,1,0" },
	{ 17,"2,3,0,1" },
	{ 18,"3,0,1,2" },
	{ 19,"3,0,2,1" },
	{ 20,"3,1,2,0" },
	{ 21,"3,1,0,2" },
	{ 22,"3,2,0,1" },
	{ 23,"3,2,1,0" },
};

FPSItem g_TLItems[] = {
	{10,100,"10"},//100
	{ 5,200,"5" },//200
	{ 2,500,"2" },//500
	{ 1,1000,"1" },//1000
};

FPSItem g_MPItems[] = {
	{ 15,50,"15" },//66
	{ 20,50,"20" },//50
	{ 25,40,"25" },//40
	{ 30,33,"30" },//33
	{ 60,16,"60" },//33
};

IMPLEMENT_DYNAMIC(CSettingsDlg, CDialogEx)

CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SETTINGS_DLG, pParent)
{

}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BROWSE, &CSettingsDlg::OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_BROWSE2, &CSettingsDlg::OnBnClickedBrowse2)
	ON_BN_CLICKED(IDC_RESET, &CSettingsDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDOK, &CSettingsDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_ENABLE_MAPPING_PF, &CSettingsDlg::OnBnClickedEnableMappingPf)
	ON_BN_CLICKED(IDC_ENABLE_TIMELINE_PF, &CSettingsDlg::OnBnClickedEnableTimelinePf)
	ON_BN_CLICKED(IDCANCEL, &CSettingsDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BROWSE3, &CSettingsDlg::OnBnClickedBrowse3)
	ON_BN_CLICKED(IDC_BROWSE4, &CSettingsDlg::OnBnClickedBrowse4)
	ON_BN_CLICKED(IDC_BROWSE5, &CSettingsDlg::OnBnClickedBrowse5)
	ON_BN_CLICKED(IDC_BROWSE6, &CSettingsDlg::OnBnClickedBrowse6)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSettingsDlg message handlers
BOOL CSettingsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	int i;
	char szFileName[1024];
	char szCurDisplayOrder[16];
	bool bEnableMappingPf = GetPrivateProfileInt("Mapping_Setting", "EnablePreloadProjectFile", 0, theApp.m_strCurPath + "Setting.ini");
	CButton* pEnableMappingPFBtn = (CButton*)GetDlgItem(IDC_ENABLE_MAPPING_PF);
	pEnableMappingPFBtn->SetCheck(bEnableMappingPf);
	
	bool bEnableTimelinePf = GetPrivateProfileInt("Timeline_Setting", "EnablePreloadProjectFile", 0, theApp.m_strCurPath + "Setting.ini");
	CButton* pEnableTimelinePFBtn = (CButton*)GetDlgItem(IDC_ENABLE_TIMELINE_PF);
	pEnableTimelinePFBtn->SetCheck(bEnableTimelinePf);

	GetPrivateProfileString("Mapping_Setting", "ProjectFile", "", szFileName, 1024, theApp.m_strCurPath + "Setting.ini");
	SetDlgItemText(IDC_MAPPING_PROJECT_FILE, szFileName);

	GetPrivateProfileString("Timeline_Setting", "ProjectFile", "", szFileName, 1024, theApp.m_strCurPath + "Setting.ini");
	SetDlgItemText(IDC_TIMELINE_PROJECT_FILE, szFileName);
	/*
	int iTimeCodeType = GetPrivateProfileInt("Timeline_Setting", "TimeCodeType", 0, theApp.m_strCurPath + "Setting.ini");
	if (iTimeCodeType == 0)
	{
		((CButton *)GetDlgItem(IDC_GOGAL_TIME_CODE))->SetCheck(true);
		((CButton *)GetDlgItem(IDC_LOCAL_TIME_CODE))->SetCheck(false);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_GOGAL_TIME_CODE))->SetCheck(false);
		((CButton *)GetDlgItem(IDC_LOCAL_TIME_CODE))->SetCheck(true);
	}
	*/
	OnBnClickedEnableMappingPf();
	OnBnClickedEnableTimelinePf();

	// language
	int tipID = GetPrivateProfileInt("UI", "TipLang", 0, theApp.m_strCurPath + "Setting.ini");
	sprintf(szFileName, "%s\\Language\\lang_config.ini", theApp.m_strCurPath);
	int iTotal = GetPrivateProfileInt("Base", "Total", 0, szFileName);
	CComboBox *bLanguageCombo = (CComboBox*)GetDlgItem(IDC_LANGUAGE);
	for (int i = 0; i < iTotal; ++i) {
		char sLangName[100];
		sprintf(sLangName, "Lang%d", i);
		GetPrivateProfileString(sLangName, "Name", "", sLangName, 100, szFileName);
		bLanguageCombo->AddString(sLangName);
	}
	bLanguageCombo->SetCurSel(tipID);


	bool bSync = GetPrivateProfileInt("Mapping_Setting", "Sync", 0, theApp.m_strCurPath + "Setting.ini");
	CButton* pSyncBtn = (CButton*)GetDlgItem(IDC_SYNC);
	pSyncBtn->SetCheck(bSync);

	int iTLFPS = GetPrivateProfileInt("Timeline_Setting", "FPS", 10, theApp.m_strCurPath + "Setting.ini");
	int iSelTLFPS = 0;
	CComboBox* pTL_FPS = (CComboBox*)GetDlgItem(IDC_TL_FPS);

	int iTotalOfTLFPSItems = sizeof(g_TLItems) / sizeof(FPSItem);
	for (i = 0; i < iTotalOfTLFPSItems; i++)
	{
		pTL_FPS->AddString(g_TLItems[i].szText);

		if (g_TLItems[i].iFPS == iTLFPS)
			iSelTLFPS = i;
	}
	pTL_FPS->SetCurSel(iSelTLFPS);

	bool bRepeat = GetPrivateProfileInt("Timeline_Setting", "Repeat", 0, theApp.m_strCurPath + "Setting.ini");
	CButton* pRepeatBtn = (CButton*)GetDlgItem(IDC_REPEAT);
	pRepeatBtn->SetCheck(bRepeat);

	int iMPFPS = GetPrivateProfileInt("Mapping_Setting", "FPS", 25, theApp.m_strCurPath + "Setting.ini");
	int iSelMPFPS = 0;
	CComboBox* pMP_FPS = (CComboBox*)GetDlgItem(IDC_MP_FPS);
	int iTotalOfMPFPSItems = sizeof(g_MPItems) / sizeof(FPSItem);
	for (i = 0; i < iTotalOfMPFPSItems; i++)
	{
		pMP_FPS->AddString(g_MPItems[i].szText);

		if (g_MPItems[i].iFPS == iMPFPS)
			iSelMPFPS = i;
	}
	pMP_FPS->SetCurSel(iSelMPFPS);

	bool bRunNet = GetPrivateProfileInt("Network_Setting", "Run", 0, theApp.m_strCurPath + "Setting.ini");
	CButton* pRunNetBtn = (CButton*)GetDlgItem(IDC_RUN_NETWORK);
	pRunNetBtn->SetCheck(bRunNet);

	bool bMaster = GetPrivateProfileInt("Network_Setting", "Master", 0, theApp.m_strCurPath + "Setting.ini");
	CButton* pMasterBtn = (CButton*)GetDlgItem(IDC_MASTER);
	pMasterBtn->SetCheck(bMaster);

#ifdef _ENABLE_VIDEO_WALL
	//mask
	bool bEnableMask = GetPrivateProfileInt("Mask", "Enable", 0, theApp.m_strCurPath + "Setting.ini");
	CButton* pEnableMaskBtn = (CButton*)GetDlgItem(IDC_ENABLE_MASK);
	pEnableMaskBtn->SetCheck(bEnableMask);

	//panel1
	GetPrivateProfileString("Mask", "Panel1_File", "", szFileName, 1024, theApp.m_strCurPath + "Setting.ini");
	SetDlgItemText(IDC_PANEL1_MASK_FILE, szFileName);

	int x = GetPrivateProfileInt("Mask", "Panel1_X", 0, theApp.m_strCurPath + "Setting.ini");
	SetDlgItemInt(IDC_P1_MAKS_X,x);
	int y = GetPrivateProfileInt("Mask", "Panel1_Y", 0, theApp.m_strCurPath + "Setting.ini");
	SetDlgItemInt(IDC_P1_MAKS_Y, y);

	//panel2
	GetPrivateProfileString("Mask", "Panel2_File", "", szFileName, 1024, theApp.m_strCurPath + "Setting.ini");
	SetDlgItemText(IDC_PANEL2_MASK_FILE, szFileName);

	x = GetPrivateProfileInt("Mask", "Panel2_X", 0, theApp.m_strCurPath + "Setting.ini");
	SetDlgItemInt(IDC_P2_MAKS_X, x);
	y = GetPrivateProfileInt("Mask", "Panel2_Y", 0, theApp.m_strCurPath + "Setting.ini");
	SetDlgItemInt(IDC_P2_MAKS_Y, y);

	//panel3
	GetPrivateProfileString("Mask", "Panel3_File", "", szFileName, 1024, theApp.m_strCurPath + "Setting.ini");
	SetDlgItemText(IDC_PANEL3_MASK_FILE, szFileName);

	x = GetPrivateProfileInt("Mask", "Panel3_X", 0, theApp.m_strCurPath + "Setting.ini");
	SetDlgItemInt(IDC_P3_MAKS_X, x);
	y = GetPrivateProfileInt("Mask", "Panel3_Y", 0, theApp.m_strCurPath + "Setting.ini");
	SetDlgItemInt(IDC_P3_MAKS_Y, y);

	//panel4
	GetPrivateProfileString("Mask", "Panel4_File", "", szFileName, 1024, theApp.m_strCurPath + "Setting.ini");
	SetDlgItemText(IDC_PANEL4_MASK_FILE, szFileName);

	x = GetPrivateProfileInt("Mask", "Panel4_X", 0, theApp.m_strCurPath + "Setting.ini");
	SetDlgItemInt(IDC_P4_MAKS_X, x);
	y = GetPrivateProfileInt("Mask", "Panel4_Y", 0, theApp.m_strCurPath + "Setting.ini");
	SetDlgItemInt(IDC_P4_MAKS_Y, y);


	int iMarkMode = GetPrivateProfileInt("Mark", "Mode", 0, theApp.m_strCurPath + "Setting.ini");
	CComboBox* pMarkMode = (CComboBox*)GetDlgItem(IDC_MARK_MODE);
	pMarkMode->AddString("None");
	pMarkMode->AddString("Line");
	pMarkMode->AddString("ColorBar");
	pMarkMode->SetCurSel(iMarkMode);

	int iTimeCodeInterval = GetPrivateProfileInt("System", "TimeCodeInterval", 20, theApp.m_strCurPath + "Setting.ini");
	CComboBox* pTimeCodeInterval = (CComboBox*)GetDlgItem(IDC_TIME_CODE_INTERVAL);
	pTimeCodeInterval->AddString("20");
	pTimeCodeInterval->AddString("40");  // 1/2
	pTimeCodeInterval->AddString("80");  // 1/4
	pTimeCodeInterval->AddString("160");  // 1/8
	pTimeCodeInterval->AddString("320");  // 1/16
	pTimeCodeInterval->AddString("640");  // 1/32
	pTimeCodeInterval->AddString("1280");  // 1/64
	if(iTimeCodeInterval == 20)
		pTimeCodeInterval->SetCurSel(0);
	else if (iTimeCodeInterval == 40)
		pTimeCodeInterval->SetCurSel(1);
	else if (iTimeCodeInterval == 80)
		pTimeCodeInterval->SetCurSel(2);
	else if (iTimeCodeInterval == 160)
		pTimeCodeInterval->SetCurSel(3);
	else if (iTimeCodeInterval == 320)
		pTimeCodeInterval->SetCurSel(4);
	else if (iTimeCodeInterval == 640)
		pTimeCodeInterval->SetCurSel(5);
	else if (iTimeCodeInterval == 1280)
		pTimeCodeInterval->SetCurSel(6);

	
	GetPrivateProfileString("Media", "DisplayOrder", "0,1,2,3", szCurDisplayOrder, 16, theApp.m_strCurPath + "Setting.ini");
	CComboBox* pDisplayOrder = (CComboBox*)GetDlgItem(IDC_DISPLAY_ORDER);

	int iSelODIndex = -1;
	int iTotalOfOrderItems = sizeof(g_OrderItems) / sizeof(DOItem);
	for (int i = 0; i < iTotalOfOrderItems; i++)
	{
		pDisplayOrder->AddString(g_OrderItems[i].szText);
		if (strcmp(g_OrderItems[i].szText, szCurDisplayOrder) == 0)
			iSelODIndex = i;
	}
	pDisplayOrder->SetCurSel(iSelODIndex);
	
#else
	/*
	GetDlgItem(IDC_ENABLE_MASK)->EnableWindow(false);
	GetDlgItem(IDC_PANEL1_MASK_FILE)->EnableWindow(false);
	GetDlgItem(IDC_PANEL2_MASK_FILE)->EnableWindow(false);
	GetDlgItem(IDC_PANEL3_MASK_FILE)->EnableWindow(false);
	GetDlgItem(IDC_PANEL4_MASK_FILE)->EnableWindow(false);

	GetDlgItem(IDC_P1_MAKS_X)->EnableWindow(false);
	GetDlgItem(IDC_P1_MAKS_Y)->EnableWindow(false);
	GetDlgItem(IDC_P2_MAKS_X)->EnableWindow(false);
	GetDlgItem(IDC_P2_MAKS_Y)->EnableWindow(false);
	GetDlgItem(IDC_P3_MAKS_X)->EnableWindow(false);
	GetDlgItem(IDC_P3_MAKS_Y)->EnableWindow(false);
	GetDlgItem(IDC_P4_MAKS_X)->EnableWindow(false);
	GetDlgItem(IDC_P4_MAKS_Y)->EnableWindow(false);
	GetDlgItem(IDC_MARK_MODE)->EnableWindow(false);
	GetDlgItem(IDC_TIME_CODE_INTERVAL)->EnableWindow(false);

	GetDlgItem(IDC_BROWSE3)->EnableWindow(false);
	GetDlgItem(IDC_BROWSE4)->EnableWindow(false);
	GetDlgItem(IDC_BROWSE5)->EnableWindow(false);
	GetDlgItem(IDC_BROWSE6)->EnableWindow(false);

	GetDlgItem(IDC_DISPLAY_ORDER)->EnableWindow(false);
	*/
#endif

	MoveToCenter(this->GetSafeHwnd());

	SetTimer(101,100,NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSettingsDlg::SetMainUIHandle(HWND hWnd)
{
	m_hMSMainUIWnd = hWnd;
}

void CSettingsDlg::OnBnClickedBrowse()
{
	// TODO: Add your control notification handler code here
#ifdef _ENABLE_SPF
	TCHAR szFilters[] = _T("Project Files (*.spf)|*.spf|All Files (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("spf"), _T("*.spf"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER, szFilters, this);
#else
	TCHAR szFilters[] = _T("Project Files (*.xml)|*.xml|All Files (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("xml"), _T("*.xml"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER, szFilters,this);
#endif

	if (fileDlg.DoModal() == IDOK)
	{
		CString filename = fileDlg.GetPathName();
		SetDlgItemText(IDC_MAPPING_PROJECT_FILE, filename.GetBuffer());
	}
}


void CSettingsDlg::OnBnClickedBrowse2()
{
	// TODO: Add your control notification handler code here
	TCHAR szFilters[] = _T("Project Files (*.tlf)|*.tlf|(*.xml)|*.xml|All Files (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("tlf"), _T("*.tlf"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER, szFilters,this);

	if (fileDlg.DoModal() == IDOK)
	{
		CString filename = fileDlg.GetPathName();
		SetDlgItemText(IDC_TIMELINE_PROJECT_FILE, filename.GetBuffer());
	}
}


void CSettingsDlg::OnBnClickedReset()
{
	// TODO: Add your control notification handler code here
	//Shape(.xml)?ÅMapping(.xml)?©ÂÄãÊ?Ê°àÔ??™Èô§TempShapeÂ≠êÁõÆ??
	int i = 0;
	BOOL bRet;
	CString strFileName = theApp.m_strCurPath + "Shape.xml";
	bRet = DeleteFile(strFileName);

	strFileName = theApp.m_strCurPath + "Mapping.xml";
	bRet = DeleteFile(strFileName);

	SQList* pFileList = new SQList;
	CString strFolderName = theApp.m_strCurPath + "TempShape";

	TreeCtrlUI treectrl_ui;
	treectrl_ui.SearchAllFiles(strFolderName.GetBuffer(), pFileList,0);

	for (i = 0; i < pFileList->GetTotal(); i++)
	{
		char* pCurFile = (char*)pFileList->Get(i);
		DeleteFile(pCurFile);
	}

	RemoveDirectory(strFolderName.GetBuffer());

	for (i = 0; i < pFileList->GetTotal(); i++)
	{
		char* pCurItem = (char*)pFileList->Get(i);
		delete pCurItem;
	}
	pFileList->EnableRemoveData(false);
	pFileList->Reset();
	delete pFileList;
}


void CSettingsDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	char szFileName[512];
	char szData[64];
	int iTimeCodeType = 0;
	int iEnableMask = 0;
	int x, y;
	GetDlgItemText(IDC_MAPPING_PROJECT_FILE, szFileName,512);
	WritePrivateProfileStringA("Mapping_Setting", "ProjectFile", szFileName, theApp.m_strCurPath + "Setting.ini");

	GetDlgItemText(IDC_TIMELINE_PROJECT_FILE, szFileName, 512);
	WritePrivateProfileStringA("Timeline_Setting", "ProjectFile", szFileName, theApp.m_strCurPath + "Setting.ini");

	CButton* pEnableMappingPFBtn = (CButton*)GetDlgItem(IDC_ENABLE_MAPPING_PF);
	sprintf(szData,"%d", pEnableMappingPFBtn->GetCheck());
	WritePrivateProfileStringA("Mapping_Setting", "EnablePreloadProjectFile", szData, theApp.m_strCurPath + "Setting.ini");

	CButton* pEnableTimelinePFBtn = (CButton*)GetDlgItem(IDC_ENABLE_TIMELINE_PF);
	sprintf(szData, "%d", pEnableTimelinePFBtn->GetCheck());
	WritePrivateProfileStringA("Timeline_Setting", "EnablePreloadProjectFile", szData, theApp.m_strCurPath + "Setting.ini");
	/*
	if (((CButton *)GetDlgItem(IDC_GOGAL_TIME_CODE))->GetCheck())
		iTimeCodeType = 0;
	else
		iTimeCodeType = 1;

	sprintf(szData, "%d", iTimeCodeType);
	WritePrivateProfileStringA("Timeline_Setting", "TimeCodeType", szData, theApp.m_strCurPath + "Setting.ini");
	*/

	CButton* pRepeatBtn = (CButton*)GetDlgItem(IDC_REPEAT);
	sprintf(szData, "%d", pRepeatBtn->GetCheck());
	WritePrivateProfileStringA("Timeline_Setting", "Repeat", szData, theApp.m_strCurPath + "Setting.ini");

	CButton* pSyncBtn = (CButton*)GetDlgItem(IDC_SYNC);
	sprintf(szData, "%d", pSyncBtn->GetCheck());
	WritePrivateProfileStringA("Mapping_Setting", "Sync", szData, theApp.m_strCurPath + "Setting.ini");

	char szValue[20];
	CComboBox* pTLFPS = (CComboBox*)GetDlgItem(IDC_TL_FPS);
	int iSel = pTLFPS->GetCurSel();
	pTLFPS->GetLBText(iSel, szValue);
	WritePrivateProfileStringA("Timeline_Setting", "FPS", szValue, theApp.m_strCurPath + "Setting.ini");

	CComboBox* pMFPS = (CComboBox*)GetDlgItem(IDC_MP_FPS);
	iSel = pMFPS->GetCurSel();
	pMFPS->GetLBText(iSel, szValue);
	WritePrivateProfileStringA("Mapping_Setting", "FPS", szValue, theApp.m_strCurPath + "Setting.ini");

	//language
	int tipID = GetPrivateProfileInt("UI", "TipLang", 0, theApp.m_strCurPath + "Setting.ini");
	CComboBox *bLanguageCombo = (CComboBox*)GetDlgItem(IDC_LANGUAGE);
	if (tipID != bLanguageCombo->GetCurSel()) {
		sprintf(szData, "%d", bLanguageCombo->GetCurSel());
		WritePrivateProfileStringA("UI", "TipLang", szData, theApp.m_strCurPath + "Setting.ini");
		sprintf(szData, "Lang%d", bLanguageCombo->GetCurSel());
		GetPrivateProfileString(szData, "Code", "", szData, 64, theApp.m_strCurPath + "\\Language\\lang_config.ini");
		theApp.m_strLanguage = szData;

		CMediaServerDlg *pFather = (CMediaServerDlg *)GetParent();
		pFather->SetTip();
	}

	CButton* pRunNetBtn = (CButton*)GetDlgItem(IDC_RUN_NETWORK);
	sprintf(szData, "%d", pRunNetBtn->GetCheck());
	WritePrivateProfileStringA("Network_Setting", "Run", szData, theApp.m_strCurPath + "Setting.ini");

	CButton* pMasterBtn = (CButton*)GetDlgItem(IDC_MASTER);
	sprintf(szData, "%d", pMasterBtn->GetCheck());
	WritePrivateProfileStringA("Network_Setting", "Master", szData, theApp.m_strCurPath + "Setting.ini");

	for (int i = 0; i < g_PannelAry.GetCount(); i++)
	{
		g_PannelAry[i]->ApplySetting();
	}

	auto tl_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	tl_manager->GetDisplayManager()->ApplySetting();

	auto service = ServiceProvider::Instance()->GetTimelineService();
	if (service != nullptr)
	{
		auto engine = service->GetTimeCodeEngine();
		engine->ApplySetting();
	}

#ifdef _ENABLE_VIDEO_WALL
	//mask
	if (((CButton *)GetDlgItem(IDC_ENABLE_MASK))->GetCheck())
		iEnableMask = 1;
	else
		iEnableMask = 0;

	sprintf(szData, "%d", iEnableMask);
	WritePrivateProfileStringA("Mask", "Enable", szData, theApp.m_strCurPath + "Setting.ini");

	//panel1
	GetDlgItemText(IDC_PANEL1_MASK_FILE, szFileName, 512);
	WritePrivateProfileStringA("Mask", "Panel1_File", szFileName, theApp.m_strCurPath + "Setting.ini");

	x = GetDlgItemInt(IDC_P1_MAKS_X);
	y = GetDlgItemInt(IDC_P1_MAKS_Y);

	sprintf(szData, "%d", x);
	WritePrivateProfileStringA("Mask", "Panel1_X", szData, theApp.m_strCurPath + "Setting.ini");
	sprintf(szData, "%d", y);
	WritePrivateProfileStringA("Mask", "Panel1_Y", szData, theApp.m_strCurPath + "Setting.ini");

	//panel2
	GetDlgItemText(IDC_PANEL2_MASK_FILE, szFileName, 512);
	WritePrivateProfileStringA("Mask", "Panel2_File", szFileName, theApp.m_strCurPath + "Setting.ini");

	x = GetDlgItemInt(IDC_P2_MAKS_X);
	y = GetDlgItemInt(IDC_P2_MAKS_Y);

	sprintf(szData, "%d", x);
	WritePrivateProfileStringA("Mask", "Panel2_X", szData, theApp.m_strCurPath + "Setting.ini");
	sprintf(szData, "%d", y);
	WritePrivateProfileStringA("Mask", "Panel2_Y", szData, theApp.m_strCurPath + "Setting.ini");

	//panel3
	GetDlgItemText(IDC_PANEL3_MASK_FILE, szFileName, 512);
	WritePrivateProfileStringA("Mask", "Panel3_File", szFileName, theApp.m_strCurPath + "Setting.ini");

	x = GetDlgItemInt(IDC_P3_MAKS_X);
	y = GetDlgItemInt(IDC_P3_MAKS_Y);

	sprintf(szData, "%d", x);
	WritePrivateProfileStringA("Mask", "Panel3_X", szData, theApp.m_strCurPath + "Setting.ini");
	sprintf(szData, "%d", y);
	WritePrivateProfileStringA("Mask", "Panel3_Y", szData, theApp.m_strCurPath + "Setting.ini");

	//panel4
	GetDlgItemText(IDC_PANEL4_MASK_FILE, szFileName, 512);
	WritePrivateProfileStringA("Mask", "Panel4_File", szFileName, theApp.m_strCurPath + "Setting.ini");

	x = GetDlgItemInt(IDC_P4_MAKS_X);
	y = GetDlgItemInt(IDC_P4_MAKS_Y);

	sprintf(szData, "%d", x);
	WritePrivateProfileStringA("Mask", "Panel4_X", szData, theApp.m_strCurPath + "Setting.ini");
	sprintf(szData, "%d", y);
	WritePrivateProfileStringA("Mask", "Panel4_Y", szData, theApp.m_strCurPath + "Setting.ini");

	//GetPrivateProfileString("Mask", szKeyName, "1.0", szData, 256, theApp.m_strCurPath + "Setting.ini");
	//m_fMaskScale = atof(szData);

	for (int i = 0; i < g_PannelAry.GetCount(); i++)
	{
		g_PannelAry[i]->ApplyMask();
	}

	CComboBox* pMarkMode = (CComboBox*)GetDlgItem(IDC_MARK_MODE);
	int iMarkMode = pMarkMode->GetCurSel();
	sprintf(szData, "%d", iMarkMode);
	WritePrivateProfileStringA("Mark", "Mode", szData, theApp.m_strCurPath + "Setting.ini");

	for (int y = 0; y < g_MediaStreamAry.size(); y++)
	{
		g_MediaStreamAry[y]->SetMarkMode(iMarkMode);
	}

	char szValue[20];
	CComboBox* pTimeCodeInterval = (CComboBox*)GetDlgItem(IDC_TIME_CODE_INTERVAL);
	int iSel = pTimeCodeInterval->GetCurSel();
	pTimeCodeInterval->GetLBText(iSel, szValue);
	
	WritePrivateProfileStringA("System", "TimeCodeInterval", szValue, theApp.m_strCurPath + "Setting.ini");

	auto service = ServiceProvider::Instance()->GetTimelineService();
	if (service != nullptr)
	{
		auto engine = service->GetTimeCodeEngine();
		engine->ApplySetting();
	}

	char szCurDisplayOrder[16];
	GetPrivateProfileString("Media", "DisplayOrder", "0,1,2,3", szCurDisplayOrder, 16, theApp.m_strCurPath + "Setting.ini");
	CComboBox* pDisplayOrder = (CComboBox*)GetDlgItem(IDC_DISPLAY_ORDER);
	int iSelODIndex = pDisplayOrder->GetCurSel();

	char szDOData[16];
	int iDisplayOrder[4];

	strcpy(szDOData, g_OrderItems[iSelODIndex].szText);

	for (int i = 0; i < 4; i++)
		iDisplayOrder[i] = GetNextNumberFromString(',', szDOData);

	WritePrivateProfileStringA("Media", "DisplayOrder", g_OrderItems[iSelODIndex].szText, theApp.m_strCurPath + "Setting.ini");

	for (int y = 0; y < g_MediaStreamAry.size(); y++)
	{
		for (int i = 0; i < 4; i++)
			g_MediaStreamAry[y]->SetDisplayOrder(i, iDisplayOrder[i]);
	}
#endif

	if (m_hMSMainUIWnd)
		::SendMessage(m_hMSMainUIWnd, MSG_MAINUI, 0, 0);

	::EnableWindow(m_hMSMainUIWnd, true);

	CDialogEx::OnOK();
}


void CSettingsDlg::OnBnClickedEnableMappingPf()
{
	// TODO: Add your control notification handler code here
	CButton* pEnableMappingPFBtn = (CButton*)GetDlgItem(IDC_ENABLE_MAPPING_PF);

	if (pEnableMappingPFBtn->GetCheck())
	{
#ifdef _ENABLE_SPF
		GetDlgItem(IDC_MAPPING_PROJECT_FILE)->EnableWindow(true);
		GetDlgItem(IDC_BROWSE)->EnableWindow(true);
#else
		GetDlgItem(IDC_MAPPING_PROJECT_FILE)->EnableWindow(false);
		GetDlgItem(IDC_BROWSE)->EnableWindow(false);
#endif
	}
	else
	{
		GetDlgItem(IDC_MAPPING_PROJECT_FILE)->EnableWindow(false);
		GetDlgItem(IDC_BROWSE)->EnableWindow(false);
	}
}


void CSettingsDlg::OnBnClickedEnableTimelinePf()
{
	// TODO: Add your control notification handler code here
	CButton* pEnableTimelinePFBtn = (CButton*)GetDlgItem(IDC_ENABLE_TIMELINE_PF);
	
	if (pEnableTimelinePFBtn->GetCheck())
	{
		GetDlgItem(IDC_TIMELINE_PROJECT_FILE)->EnableWindow(true);
		GetDlgItem(IDC_BROWSE2)->EnableWindow(true);
	}
	else
	{
		GetDlgItem(IDC_TIMELINE_PROJECT_FILE)->EnableWindow(false);
		GetDlgItem(IDC_BROWSE2)->EnableWindow(false);
	}
}


void CSettingsDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here

//	if (m_hMSMainUIWnd)
//		::SendMessage(m_hMSMainUIWnd, MSG_MAINUI, 0, 0);
	CDialogEx::OnCancel();
	::EnableWindow(m_hMSMainUIWnd, true);
}


void CSettingsDlg::OnBnClickedBrowse3()
{
	// TODO: Add your control notification handler code here
	TCHAR szFilters[] = _T("Picture Files (*.bmp)|*.bmp|(*.png)|*.png|All Files (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("bmp"), _T("*.bmp"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER, szFilters, this);

	if (fileDlg.DoModal() == IDOK)
	{
		CString filename = fileDlg.GetPathName();
		SetDlgItemText(IDC_PANEL1_MASK_FILE, filename.GetBuffer());
	}
}


void CSettingsDlg::OnBnClickedBrowse4()
{
	// TODO: Add your control notification handler code here
	TCHAR szFilters[] = _T("Picture Files (*.bmp)|*.bmp|(*.png)|*.png|All Files (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("bmp"), _T("*.bmp"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER, szFilters, this);

	if (fileDlg.DoModal() == IDOK)
	{
		CString filename = fileDlg.GetPathName();
		SetDlgItemText(IDC_PANEL2_MASK_FILE, filename.GetBuffer());
	}
}


void CSettingsDlg::OnBnClickedBrowse5()
{
	// TODO: Add your control notification handler code here
	TCHAR szFilters[] = _T("Picture Files (*.bmp)|*.bmp|(*.png)|*.png|All Files (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("bmp"), _T("*.bmp"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER, szFilters, this);

	if (fileDlg.DoModal() == IDOK)
	{
		CString filename = fileDlg.GetPathName();
		SetDlgItemText(IDC_PANEL3_MASK_FILE, filename.GetBuffer());
	}
}


void CSettingsDlg::OnBnClickedBrowse6()
{
	// TODO: Add your control notification handler code here
	TCHAR szFilters[] = _T("Picture Files (*.bmp)|*.bmp|(*.png)|*.png|All Files (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("bmp"), _T("*.bmp"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER, szFilters, this);

	if (fileDlg.DoModal() == IDOK)
	{
		CString filename = fileDlg.GetPathName();
		SetDlgItemText(IDC_PANEL4_MASK_FILE, filename.GetBuffer());
	}
}


void CSettingsDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	/*
	char szText[64];
	sprintf(szText,"%3.2f", theApp.m_fPresentFPS);
	SetDlgItemText(IDC_STATIC_DISPLAY_FPS, szText);
	*/

	CDialogEx::OnTimer(nIDEvent);
}
