// ModifyUI.cpp : 實作檔
//

#include "stdafx.h"
#include "MediaServer.h"
#include "ModifyUI.h"


// CModifyUI 對話方塊

IMPLEMENT_DYNAMIC(CModifyUI, CDialog)

CModifyUI::CModifyUI(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyUI::IDD, pParent)
{	
	m_bShowDlg = FALSE;
	m_iCurLang = ENGLISH_TYE;
}

CModifyUI::~CModifyUI()
{
}

void CModifyUI::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CModifyUI, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_PANNEL_LIST, &CModifyUI::OnCbnSelchangePannelList)
	ON_CBN_SELCHANGE(IDC_IMG_EFFECT_LIST, &CModifyUI::OnCbnSelchangeImgEffectList)
END_MESSAGE_MAP()


// CModifyUI 訊息處理常式

BOOL CModifyUI::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化
	#define WS_EX_LAYERED 0x00080000
    SetWindowLong(this->m_hWnd, GWL_EXSTYLE, GetWindowLong(this->m_hWnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
    SetLayeredWindowAttributes(0, 160, LWA_ALPHA);
	SetTimer(0, 100, NULL);

	((CComboBox*)GetDlgItem(IDC_PANNEL_LIST))->InsertString(0, "Pannel 4");
	((CComboBox*)GetDlgItem(IDC_PANNEL_LIST))->InsertString(0, "Pannel 3");
	((CComboBox*)GetDlgItem(IDC_PANNEL_LIST))->InsertString(0, "Pannel 2");
	((CComboBox*)GetDlgItem(IDC_PANNEL_LIST))->InsertString(0, "Pannel 1");
	((CComboBox*)GetDlgItem(IDC_PANNEL_LIST))->InsertString(0, "Pannel All");
	((CComboBox*)GetDlgItem(IDC_PANNEL_LIST))->SetCurSel(0);

	((CComboBox*)GetDlgItem(IDC_EFFECT_LIST))->InsertString(0, "All");
	((CComboBox*)GetDlgItem(IDC_EFFECT_LIST))->InsertString(0, "Fade In & Out");
	((CComboBox*)GetDlgItem(IDC_EFFECT_LIST))->InsertString(0, "Rotate By Y Axis");
	((CComboBox*)GetDlgItem(IDC_EFFECT_LIST))->InsertString(0, "Rotate By X Axis");
	((CComboBox*)GetDlgItem(IDC_EFFECT_LIST))->InsertString(0, "Rotate By Z Axis");
	((CComboBox*)GetDlgItem(IDC_EFFECT_LIST))->InsertString(0, "No Effect");
	((CComboBox*)GetDlgItem(IDC_PANNEL_LIST))->SetCurSel(0);

	if (g_PannelSetting.iTVWallMode)
	{
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "TV Wall");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->SetCurSel(0);
	}
	else
	{
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Mirage");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Blur");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Single Color");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Dot Mosaics");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Plane Rotate");

		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Cylinder Rotate");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Circle Center");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Kaleidoscope");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "WaterColor");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Wave");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Edge");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Wood Noise");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Halftone");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Fish Eye");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Ripple");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Glass Windows");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Color Modify");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Neon Light Texture");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Blind Texture");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Swirl Texture");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Cross Hatching");
		//((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "ColorConvert");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Rotate Scale");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Pixelate");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Multi Movie");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Freeze Movie");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Rectangle Bright Dark");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Sleazy Emboss");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Brightness Flicker");
		//((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Brightness");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "RGB Color");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Gray");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "Negative");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->InsertString(0, "No Effect");
		((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->SetCurSel(0);
	}

	if (g_PannelSetting.iTVWallMode)
	{
		m_ParmNameAry.Add({"Left","Top","Right","GapX", "GapY"});
	}
	else if (m_iCurLang == ENGLISH_TYE)
	{
		m_ParmNameAry.Add({ "", "","","","","" });
		m_ParmNameAry.Add({ "Level", "","","","","" });
		m_ParmNameAry.Add({ "Level", "Contrast","","","","" });
		m_ParmNameAry.Add({ "Level", "Red Shift","Green Shift","Blue Shift","","" });
		m_ParmNameAry.Add({ "Level", "Frequency","Amplitude","","","" });
		m_ParmNameAry.Add({ "Level", "Contast","","","","" });
		m_ParmNameAry.Add({ "Level", "Bright Shift","Left","Right","Top", "Down" });
		m_ParmNameAry.Add({ "Freeze", "","","","" });
		m_ParmNameAry.Add({ "Multi", "H Shift","V Shift","Mirror","" });
		m_ParmNameAry.Add({ "Pixel Size", "","","","" });
		m_ParmNameAry.Add({ "Angle", "Width Zoom","Heigth Zoom","H Shfit","V Shift" });
		m_ParmNameAry.Add({ "Level", "Density","","","" });
		m_ParmNameAry.Add({ "Amount", "Radius","","","" });
		m_ParmNameAry.Add({ "Level", "Density","Contrast","","" });
		m_ParmNameAry.Add({ "Level", "Light Size","H Move","V Move","" });
		m_ParmNameAry.Add({ "Level", "Brightness","Hue","Saturation","" });
		m_ParmNameAry.Add({ "Level", "Pattern","","","" });
		m_ParmNameAry.Add({ "Peak", "Speed","Wavelength","H Shift","V Shift" });
		m_ParmNameAry.Add({ "Curvature", "Layer","","","" });
		m_ParmNameAry.Add({ "Density", "Brightness","","","" });
		m_ParmNameAry.Add({ "R", "G","B","H Num","V Num" });
		m_ParmNameAry.Add({ "Edge Width", "","","","" });
		m_ParmNameAry.Add({ "Wave Number", "Frequency","Twist","", "" });
		m_ParmNameAry.Add({ "Level", "Saturation","","","" });
		m_ParmNameAry.Add({ "Level", "Speed","","","" });
		m_ParmNameAry.Add({ "Number", "Angle1","Rotate1","Angle2","Rotate2" });
		m_ParmNameAry.Add({ "Number", "Rotate","Shift","","" });
		m_ParmNameAry.Add({ "Plane Num", "Rotate","X Move","Z Move","Distance" });
		m_ParmNameAry.Add({ "Dot Size", "Brigtness","Soft","","" });
		m_ParmNameAry.Add({ "Color Range", "Red", "Green", "Blue","Saturation" });
		m_ParmNameAry.Add({ "Frame Num", "", "", "","" });
		m_ParmNameAry.Add({ "Frequency", "Angle", "Range", "Amplitude","Y Offset" });
	}
	
	for (int x = 0; x < 6; x++)
	{
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER1 + x))->SetRange(0, 100);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER1 + x))->SetPos(50);
	}

	if (g_PannelSetting.iTVWallMode)
	{
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER1 + 3))->SetPos(0);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER1 + 4))->SetPos(0);
	}


	CString strTemp;

	for (int x = 16; x > 0; x--)
	{
		strTemp.Format("%i", x);

		((CComboBox*)GetDlgItem(IDC_XNUM_COMBO))->InsertString(0, strTemp);

		if (x < 5)
			((CComboBox*)GetDlgItem(IDC_YNUM_COMBO))->InsertString(0, strTemp);
	}

	((CComboBox*)GetDlgItem(IDC_XNUM_COMBO))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_YNUM_COMBO))->SetCurSel(0);


	((CComboBox*)GetDlgItem(IDC_WHRATIO_COMBO))->InsertString(0, " 4 : 3");
	((CComboBox*)GetDlgItem(IDC_WHRATIO_COMBO))->InsertString(0, "16 : 9");
	((CComboBox*)GetDlgItem(IDC_WHRATIO_COMBO))->SetCurSel(0);

	/*m_FramePic   = new CMenu_Class(this, m_hWnd, 1, theApp.m_strCurPath + "UI Folder\\PVM_BK1.bmp");
	m_FramePic->Initialize();
	m_FramePic->SetDlgRegion(this);
	CRgn NullRgn;
	NullRgn.CreateRectRgn(0, 0, 0, 0);
	m_FramePic->SetWindowRgn(NullRgn, true);*/

	OnCbnSelchangeImgEffectList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CModifyUI::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此加入您的訊息處理常式程式碼
	// 不要呼叫圖片訊息的 CDialog::OnPaint()
}

void CModifyUI::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (nIDEvent == 0 && IsWindowVisible())
	{
		INT iSelPannel = ((CComboBox*)GetDlgItem(IDC_PANNEL_LIST))->GetCurSel();
		INT iSelEffect = ((CComboBox*)GetDlgItem(IDC_EFFECT_LIST))->GetCurSel();

		/*g_PannelSetting.bFullScreen = (((CButton*)GetDlgItem(IDC_FULLSCR_CHK))->GetCheck() == BST_CHECKED);
		g_PannelSetting.bShowGround = (((CButton*)GetDlgItem(IDC_SHOW_GROUND_CHK))->GetCheck() == BST_CHECKED);
		g_PannelSetting.bShowAxis = (((CButton*)GetDlgItem(IDC_SHOW_AXIS_CHK))->GetCheck() == BST_CHECKED);
		g_PannelSetting.bShowModelLine = (((CButton*)GetDlgItem(IDC_SHOW_Model_LINE_CHK))->GetCheck() == BST_CHECKED);
		g_PannelSetting.bRunModelLine = (((CButton*)GetDlgItem(IDC_RUN_Model_LINE_CHK))->GetCheck() == BST_CHECKED);
		g_PannelSetting.bMoveModel = (((CButton*)GetDlgItem(IDC_MOVE_MDL_CHK))->GetCheck() == BST_CHECKED);
		g_PannelSetting.bRotateModel = (((CButton*)GetDlgItem(IDC_ROTATE_MDL_CHK))->GetCheck() == BST_CHECKED);
		g_PannelSetting.bNotShowModal = (((CButton*)GetDlgItem(IDC_ONLY_VIDEO_CHK))->GetCheck() == BST_CHECKED);

		if (g_PannelSetting.bNotShowModal)
			((CButton*)GetDlgItem(IDC_HAVE_END_SUF_CHK))->SetCheck(TRUE);

		g_PannelSetting.bHaveEndSurface = (((CButton*)GetDlgItem(IDC_HAVE_END_SUF_CHK))->GetCheck() == BST_CHECKED);
		g_PannelSetting.iSelIdx = iSelPannel;*/


		g_EffectParam.iLevel = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER1))->GetPos();
		g_EffectParam.iParm1 = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER2))->GetPos();
		g_EffectParam.iParm2 = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER3))->GetPos();
		g_EffectParam.iParm3 = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER4))->GetPos();
		g_EffectParam.iParm4 = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER5))->GetPos();
		g_EffectParam.iParm5 = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER6))->GetPos();

		/*CString strTemp;
		strTemp.Format("%i", g_MediaStreamAry.at(0)->GetCurFrame());
		GetDlgItem(IDC_LEVEL_STC)->SetWindowTextA(strTemp);*/

		if (1)
		{
			iSelEffect = ((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->GetCurSel();
			//if (iSelEffect > 0)
			{
				g_MediaStreamAry.at(iSelPannel)->SetEffectIndex(iSelEffect);
				g_MediaStreamAry.at(iSelPannel)->SetEffectParam(g_EffectParam.iLevel, g_EffectParam.iParm1,
					g_EffectParam.iParm2, g_EffectParam.iParm3, g_EffectParam.iParm4, g_EffectParam.iParm5);
			}
		}
		else if (iSelPannel == 0)
		{
			for (int x = 0; x < 4; x++)
			{
				if (iSelEffect == 5)
					g_PannelSetting.iEffect[x] = x + 1;
				else
					g_PannelSetting.iEffect[x] = iSelEffect;

				g_PannelSetting.iImageEffect[x] = ((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->GetCurSel();
			}
		}
		else
		{
			g_PannelSetting.iEffect[iSelPannel - 1] = iSelEffect;			
			g_PannelSetting.iImageEffect[iSelPannel - 1] = ((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->GetCurSel();

		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CModifyUI::OnCbnSelchangePannelList()
{
	// TODO: 在此加入控制項告知處理常式程式碼
}


void CModifyUI::OnCbnSelchangeImgEffectList()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	INT iSelEffect = ((CComboBox*)GetDlgItem(IDC_IMG_EFFECT_LIST))->GetCurSel();

	if (iSelEffect >= 0 && iSelEffect < m_ParmNameAry.GetCount())
	{
		BOOL bEmptyParm;
		for (int x = 0; x < 6; x++)
		{
			bEmptyParm = m_ParmNameAry[iSelEffect].strParmName[x].IsEmpty();
			GetDlgItem(IDC_LEVEL_STC + x)->SetWindowTextA(m_ParmNameAry[iSelEffect].strParmName[x]);

			(CSliderCtrl*)GetDlgItem(IDC_SLIDER1 + x)->ShowWindow(!bEmptyParm);
			GetDlgItem(IDC_LEVEL_STC + x)->ShowWindow(!bEmptyParm);
		}
	}
}
