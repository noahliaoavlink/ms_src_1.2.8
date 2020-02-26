// MixerCtrlDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MediaServer.h"
#include "MixerCtrlDlg.h"
#include "afxdialogex.h"
#include "Moveable_Item.h"
#include "MFilePlayerDll.h"
#include "ServiceProvider.h"


#define IDC_COMBO_ID 		 9000
#define IDC_XSPIN_ID 		 9001
#define IDC_YSPIN_ID 		 9002
#define IDC_XEDIT_ID 		 9003
#define IDC_YEDIT_ID 		 9004
#define IDC_STATIC_LEVEL_ID  9005
#define IDC_STATIC_PARAM1_ID 9006
#define IDC_STATIC_PARAM2_ID 9007
#define IDC_STATIC_PARAM3_ID 9008
#define IDC_STATIC_PARAM4_ID 9009
#define IDC_STATIC_PARAM5_ID 9010
#define IDC_PANEL_COMBO_ID   9011
#define IDC_STATIC_WIDTH_ID  9012
#define IDC_STATIC_HEIGHT_ID 9013
#define IDC_SLIDER			 9014
#define IDC_STATIC_SYSSND_ID 9015
#define	IDC_STATIC_IMG1TIME_ID  9016
#define	IDC_STATIC_IMG2TIME_ID  9017
#define	IDC_STATIC_MIXERTIME_ID 9018


//#define IDC_FILENAME	9011


#define IDC_STATIC_BMP1_ID	 9051
#define IDC_STATIC_BMP2_ID	 9052
#define IDC_STATIC_BMP3_ID	 9053

#define IWSCALE(NUM) (int(NUM*m_fWscaling))
#define IHSCALE(NUM) (int(NUM*m_fHscaling))
#define LWSCALE(NUM) (LONG(NUM*m_fWscaling))
#define LHSCALE(NUM) (LONG(NUM*m_fHscaling))

//reset for Image Maximum Display Time, it vary for future program set
#define FULL_IMAGE1_DISPLAY_TIME   255
#define FULL_IMAGE2_DISPLAY_TIME   255
#define FULL_MIXER_DISPLAY_TIME    255

//reset for RGB Maximum Setting Value(Capcity), it vary for future program set
#define MAX_BRIGHT_VAL     200
#define MAX_CONTRAST_VAL   200
#define MAX_HUE_VAL        200
#define MAX_SATURATION_VAL 200
#define MAX_R_VAL		   200
#define MAX_G_VAL		   200
#define MAX_B_VAL		   200

//reset for Level, Param Maximum Setting Value(Capcity), it vary for future program set
#define MAX_LEVEL_VAL      100
#define MAX_PARAM_VAL      100

//reset for Volume Maximum Setting Value(Capacity), it vary for future program set
#define MAX_VOLUME_VAL     100


//make system sound value 100 at top, value 0 at bottom
#define SYS_SOUND_VAL(VAL) (m_SysSndSldCtrl.GetRangeMax()-VAL)

//x-axis coodinate for image, color, effect slider buttons
#define IMAGE1_SLIDER_X_BEGIN IWSCALE(233)
#define IMAGE2_SLIDER_X_BEGIN IWSCALE(862)
#define MIXER_SLIDER_X_BEGIN  IWSCALE(1502)
#define IMAGE1_SLIDER_X_END IWSCALE(233+351)
#define IMAGE2_SLIDER_X_END IWSCALE(862+351)
#define MIXER_SLIDER_X_END  IWSCALE(1502+351)
#define IMAGE_SLIDER_Y		IHSCALE(450)
#define DISPLAY_PERCENT(ID, DISPLAY_X_COOD_BEGIN, DISPLAY_X_COOD_END) float(((CMoveable_Item*)(m_FramePic->GetItemByID(ID)))->GetRelativePos().x/*-DISPLAY_X_COOD_BEGIN*/)/(DISPLAY_X_COOD_END-DISPLAY_X_COOD_BEGIN)
#define	IMAGE_SLIDER_LEN	(IMAGE1_SLIDER_X_END-IMAGE1_SLIDER_X_BEGIN)

#define COLOR_SLIDER_X_BEGIN IWSCALE(43)
#define COLOR_SLIDER_X_END IWSCALE(43+477-30)
#define COLOR1_SLIDER_Y IHSCALE(611)
#define COLOR2_SLIDER_Y IHSCALE(715)
#define COLOR3_SLIDER_Y IHSCALE(815)
#define COLOR_SLIDER_LEN (COLOR_SLIDER_X_END-COLOR_SLIDER_X_BEGIN)
//#define COLOR_PERCENT(ID) float(((CMoveable_Item*)(m_FramePic->GetItemByID(ID)))->GetRelativePos().x-COLOR_SLIDER_X_BEGIN)/COLOR_SLIDER_LEN
#define COLOR_PERCENT(ID) float(((CMoveable_Item*)(m_FramePic->GetItemByID(ID)))->GetRelativePos().x)/COLOR_SLIDER_LEN                         //GetRelativePos() return pixel offset beginning from its start position


#define LEVEL_SLIDER_X_BEGIN IWSCALE(1070)
#define LEVEL_SLIDER_X_END IWSCALE(1354)
#define PARAM_SLIDER_X_BEGIN IWSCALE(1080)
#define PARAM_SLIDER_X_END IWSCALE(1349)
#define LEVEL_SLIDER_Y IHSCALE(590)
#define PARAM1_SLIDER_Y IHSCALE(645)
#define PARAM2_SLIDER_Y IHSCALE(702)
#define PARAM3_SLIDER_Y IHSCALE(762)
#define PARAM4_SLIDER_Y IHSCALE(822)
#define PARAM5_SLIDER_Y IHSCALE(881)
#define LEVEL_SLIDER_LEN (LEVEL_SLIDER_X_END-LEVEL_SLIDER_X_BEGIN)
#define PARAM_SLIDER_LEN (PARAM_SLIDER_X_END-PARAM_SLIDER_X_BEGIN)
//#define LEVEL_PERCENT float(((CMoveable_Item*)(m_FramePic->GetItemByID(LEVEL_SLIDER)))->GetRelativePos().x-LEVEL_SLIDER_X_BEGIN)/LEVEL_SLIDER_LEN
//#define EFFEC_PERCENT(ID) float(((CMoveable_Item*)(m_FramePic->GetItemByID(ID)))->GetRelativePos().x-PARAM_SLIDER_X_BEGIN)/PARAM_SLIDER_LEN
#define LEVEL_PERCENT float(((CMoveable_Item*)(m_FramePic->GetItemByID(LEVEL_SLIDER)))->GetRelativePos().x)/LEVEL_SLIDER_LEN
#define EFFEC_PERCENT(ID) float(((CMoveable_Item*)(m_FramePic->GetItemByID(ID)))->GetRelativePos().x)/PARAM_SLIDER_LEN


LANGUAGE_TYPE m_iCurLang;
CArray<PARM_NAME, PARM_NAME> m_ParmNameAry;

// CMixerCtrlDlg 對話方塊

IMPLEMENT_DYNAMIC(CMixerCtrlDlg, CDialog)

CImageArea::CImageArea(const CRect& area, CWnd* Parent/* = NULL*/, int IID) : m_bPlaying(FALSE), m_pParentWnd(Parent)
{
	m_RealityRect = area;

	m_ZoomedRect = { 0, 0, 0, 0 };
	m_iDecImgHeight = m_iDecImgWidth = 0;
	m_PicCtrl.Create(_T("A bitmap static control"), WS_CHILD | WS_BORDER | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE, area, m_pParentWnd, IID);
	m_pMediaStream = NULL;
	m_iAngle = 0;

	m_pDisplayManager = new CShapeDispManager;

	
	//init BRIGHT PAGE SETTING VALUES(100 mapping to underlying value 0)
	m_uBright=100;
	m_uContrast=100;
	//init HUE PAGE SETTING VALUES   (100 mapping to underlying value 0)
	m_uHue=100;
	m_uSaturation=100;
	//init RGB PAGE SETTING VALUES	 (100 mapping to underlying value 0)
	m_uR=100;
	m_uG=100;
	m_uB=100;



	m_pD3DRender = new D3DRender;
	m_pD3DRender->Init(m_PicCtrl.m_hWnd, VPF_RGB32, 1920, 1080);
	m_pD3DRender->Clear();
	m_pD3DRender->Flip();

	m_pDisplayManager->Init(m_PicCtrl.m_hWnd, m_pD3DRender->GetD3DDevice());
}

CImageArea::~CImageArea()
{	   
	if (m_pDisplayManager)
	{
		m_pDisplayManager->UsingD3D(0);
		delete m_pDisplayManager;
		m_pDisplayManager = NULL;
	}

	if (m_pD3DRender)
		delete m_pD3DRender;
}

void CImageArea::RenderSurface(IDirect3DSurface9* pBuffer, int iWidth, int iHeight)
{
	if (pBuffer)
	{
		m_pD3DRender->RenderFrame1(pBuffer, iWidth, iHeight);
		m_pD3DRender->Flip();
	}
}

void CImageArea::RenderFrame(unsigned char* pBuffer, int iWidth, int iHeight)
{	
	if (pBuffer)
	{
		m_pD3DRender->RenderFrame(pBuffer, iWidth, iHeight);
		m_pD3DRender->Flip();
	}
}

CMixerCtrlDlg::CMixerCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TEMP_DLG, pParent)
{
	m_bShow = FALSE;
	m_cdc = NULL;
	m_FramePic = NULL;
	m_bInitOK = FALSE;
	m_bTimerEnable = FALSE;
	m_bVolClicked = FALSE;
	m_uActivePage = BRIGHT;
	m_iCurLang = ENGLISH_TYE;
	ZeroMemory(m_cImage, sizeof(m_cImage));
	//m_sLevelVal = m_sParam1Val = m_sParam2Val = m_sParam3Val = m_sParam4Val = m_sParam5Val = _T("50");
	//g_EffectParam.iLevel = g_EffectParam.iParm1 = g_EffectParam.iParm2 = g_EffectParam.iParm3 = g_EffectParam.iParm4 = g_EffectParam.iParm5 = 50;

	for (int x = 0; x < 6; x++)
		m_sParmVal[x] = _T("50");
	//default layer1 to play	
	m_iImageID = LAYER1;

	//m_uEffItemBmp = 0;
	//init display duration
	//ZeroMemory(m_uDisplayDurationArray, sizeof(m_uDisplayDurationArray));
	//init image1, image2, Mixer enabled status
	//generate num of PLAY_THREAD_NUM class and init play list status
}

CMixerCtrlDlg::~CMixerCtrlDlg()
{
	//delete[] &m_cImage;
	

}

void CMixerCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (m_bInitOK)
	{
		DDX_Text(pDX, IDC_XEDIT_ID, m_cImage[m_iImageID]->m_ZoomLTpnt.x);
		DDV_MinMaxInt(pDX, m_cImage[m_iImageID]->m_ZoomLTpnt.x, m_ZoomOriLTpnt.x - 100, m_ZoomOriLTpnt.x + 100);
		DDX_Text(pDX, IDC_YEDIT_ID, m_cImage[m_iImageID]->m_ZoomLTpnt.y);
		DDV_MinMaxInt(pDX, m_cImage[m_iImageID]->m_ZoomLTpnt.y, m_ZoomOriLTpnt.y - 100, m_ZoomOriLTpnt.y + 100);
	   
		for (int x = 0; x < 6; x++)
		{
			DDX_Text(pDX, IDC_STATIC_LEVEL_ID + x, m_sParmVal[x]);
		}

		DDX_Text(pDX, IDC_STATIC_WIDTH_ID , m_sWidth);
		DDX_Text(pDX, IDC_STATIC_HEIGHT_ID, m_sHeight);

		DDX_Text(pDX, IDC_STATIC_SYSSND_ID , m_sSysSound);

		DDX_Text(pDX, IDC_STATIC_IMG1TIME_ID , m_sImg1Time);
		DDX_Text(pDX, IDC_STATIC_IMG2TIME_ID , m_sImg2Time);
		DDX_Text(pDX, IDC_STATIC_MIXERTIME_ID, m_sMixerTime);
	}
}


BEGIN_MESSAGE_MAP(CMixerCtrlDlg, CDialog)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO_ID, &CMixerCtrlDlg::OnSelchange)
	ON_CBN_SELCHANGE(IDC_PANEL_COMBO_ID, &CMixerCtrlDlg::OnSelPanelchange)
    ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CMixerCtrlDlg 訊息處理常式


BOOL CMixerCtrlDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別

	if (m_cImage[m_iImageID] && m_cImage[m_iImageID]->StreamExist())
	{
		switch (message)
		{
			case TIMER_ITEM:
				if (m_bTimerEnable)
				{
					m_cImage[m_iImageID]->MixerStream().SetEffectIndex(0);
					KillTimer(0);
				}
				else
				{
					SetTimer(0, 30, NULL);
				}
				m_bTimerEnable = !m_bTimerEnable;
				break;
	//trigger play list buttons function

			case REC_ITEM:
				if (m_cImage[m_iImageID] && m_cImage[m_iImageID]->StreamExist())
				{
					auto service = ServiceProvider::Instance()->GetTimelineService();
					if (service != nullptr)
					{
						auto engine = service->GetTimeCodeEngine();
						engine->SetRefTimeCode(10000);
					}
					for (int x = 0; x < g_MediaStreamAry.size(); x++)
					{
						if (g_MediaStreamAry.at(x)->IsOpened())
							g_MediaStreamAry.at(x)->SetReferenceTimeCode(10000.0f);
					}
				}
				break;
			case STOP_ITEM:
				{
					m_cImage[m_iImageID]->m_bPlaying = FALSE;

					//reset slider UI
			
					CPoint SliderPos;
			
					switch (m_iImageID)
					{
						case LAYER1:
							SliderPos={IMAGE1_SLIDER_X_BEGIN, IMAGE_SLIDER_Y};
							((CMoveable_Item*)m_FramePic->GetItemByID(IMAGE1_SLIDER))->SetToPos(SliderPos, TRUE); //SetToPos set absolute position, i.e. pixel coordination at screen
							((CMoveable_Item*)m_FramePic->GetItemByID(IMAGE1_SLIDER))->RedrawBGImage(true);
							break;
						case LAYER2:
							SliderPos={IMAGE2_SLIDER_X_BEGIN, IMAGE_SLIDER_Y};
							((CMoveable_Item*)m_FramePic->GetItemByID(IMAGE2_SLIDER))->SetToPos(SliderPos, TRUE);
							((CMoveable_Item*)m_FramePic->GetItemByID(IMAGE2_SLIDER))->RedrawBGImage(true);
							break;
						case MIXER:
							SliderPos={MIXER_SLIDER_X_BEGIN, IMAGE_SLIDER_Y};
							((CMoveable_Item*)m_FramePic->GetItemByID(MIXER_SLIDER))->SetToPos(SliderPos, TRUE);
							((CMoveable_Item*)m_FramePic->GetItemByID(MIXER_SLIDER))->RedrawBGImage(true);							
							break;
					}
					m_cImage[m_iImageID]->MixerStream().Stop();
					m_cImage[m_iImageID]->MixerStream().Close();
				}
				break;
			case PAUSE_ITEM:
				{
					m_cImage[m_iImageID]->m_bPlaying = FALSE;

					m_cImage[m_iImageID]->MixerStream().Pause();				
				}
				break;
			case PLAY_ITEM:
				{
					m_cImage[m_iImageID]->m_bPlaying = TRUE;

					//char szFileName[512];
					//GetDlgItemTextA(IDC_FILENAME, szFileName, 512);

					
					if (m_cImage[m_iImageID]->MixerStream().GetPlaybackMode() != MFPM_BACKWARD && m_cImage[m_iImageID]->MixerStream().GetPlaybackMode() != MFPM_PAUSE)
						m_cImage[m_iImageID]->MixerStream().Open(m_cImage[m_iImageID]->MixerStream().FilePath().GetBuffer());
					m_cImage[m_iImageID]->MixerStream().Play();
				}
				break;
			case TO_START_ITEM:
				m_cImage[m_iImageID]->MixerStream().Seek(0);
				break;
			case TO_END_ITEM:
				m_cImage[m_iImageID]->MixerStream().Seek(m_cImage[m_iImageID]->MixerStream().GetTotalPos());
				break;
			case FORWARD_ITEM:
				m_cImage[m_iImageID]->MixerStream().SpeedUp();
				break;
			case BACKWARD_ITEM:
				m_cImage[m_iImageID]->MixerStream().SpeedDown();
				break;
			case BACK_PLAY_ITEM:
				m_cImage[m_iImageID]->MixerStream().PlayBackward();
				break;

	//refresh zoom In-Out UI			

			case UP_BUTTON:
				//if (m_ZoomLTpnt.y>m_ZoomOriLTpnt.y)
				if (m_cImage[m_iImageID]->m_bZmPaintBit && m_cImage[m_iImageID]->m_ZoomRBpnt.y>(m_ZoomOriLTpnt.y+1)) //+1 for avoid crashing while touch critical edge
				{
					DrawZoomRect();
					m_cImage[m_iImageID]->m_ZoomLTpnt.y -= 1;
					m_cImage[m_iImageID]->m_ZoomRBpnt.y -= 1;
					DrawZoomRect();
					UpdateData(FALSE);
				}
				break;
			case DOWN_BUTTON:
				//if (m_ZoomRBpnt.y<m_ZoomOriRBpnt.y)
				if (m_cImage[m_iImageID]->m_bZmPaintBit && (m_cImage[m_iImageID]->m_ZoomLTpnt.y+1)<m_ZoomOriRBpnt.y) //+1 for avoid crashing while touch critical edge
				{
					DrawZoomRect();
					m_cImage[m_iImageID]->m_ZoomLTpnt.y += 1;
					m_cImage[m_iImageID]->m_ZoomRBpnt.y += 1;
					DrawZoomRect();
					UpdateData(FALSE);
				}
				break;
			case LEFT_BUTTON:
				//if (m_ZoomLTpnt.x>m_ZoomOriLTpnt.x)
				if (m_cImage[m_iImageID]->m_bZmPaintBit && m_cImage[m_iImageID]->m_ZoomRBpnt.x>(m_ZoomOriLTpnt.x+1)) //+1 for avoid crashing while touch critical edge
				{
					DrawZoomRect();
					m_cImage[m_iImageID]->m_ZoomLTpnt.x -= 1;
					m_cImage[m_iImageID]->m_ZoomRBpnt.x -= 1;
					DrawZoomRect();
					UpdateData(FALSE);
				}
				break;
			case RIGHT_BUTTON:
				//if (m_ZoomRBpnt.x<m_ZoomOriRBpnt.x)
				if (m_cImage[m_iImageID]->m_bZmPaintBit && (m_cImage[m_iImageID]->m_ZoomLTpnt.x+1)<m_ZoomOriRBpnt.x) //+1 for avoid crashing while touch critical edge
				{
					DrawZoomRect();
					m_cImage[m_iImageID]->m_ZoomLTpnt.x += 1;
					m_cImage[m_iImageID]->m_ZoomRBpnt.x += 1;
					DrawZoomRect();
					UpdateData(FALSE);
				}
				break;
			
	//save slider button value

			//adjust Mixer sliders position to seek the display point

			case IMAGE1_SLIDER:
			case IMAGE2_SLIDER:
			case MIXER_SLIDER:
				{
					if (::GetCursor() == AfxGetApp()->LoadCursor(IDC_HAND_CLOSE))
					{
						m_cImage[m_iImageID]->m_bPlaying = FALSE;
						//m_cImage[m_iImageID]->MixerStream().Pause();
						m_cImage[m_iImageID]->MixerStream().Seek(int(m_cImage[m_iImageID]->MixerStream().GetTotalPos()*DISPLAY_PERCENT(message, IMAGE1_SLIDER_X_BEGIN, IMAGE1_SLIDER_X_END)));
					}
					else
					{
						m_cImage[m_iImageID]->m_bPlaying = TRUE;
						m_cImage[m_iImageID]->MixerStream().Play();
					}
				}
				break;
				
			//RGB slider
			
			case COLOR1_SLIDER:
				{
					if (m_uActivePage == BRIGHT)
					{
						m_cImage[m_iImageID]->m_uBright = UINT(MAX_BRIGHT_VAL*COLOR_PERCENT(message));	
						m_cImage[m_iImageID]->MixerStream().SetBrightness(int(m_cImage[m_iImageID]->m_uBright)-100);
					}
					else if (m_uActivePage == HUE)
					{
						m_cImage[m_iImageID]->m_uHue = UINT(MAX_HUE_VAL*COLOR_PERCENT(message));
						m_cImage[m_iImageID]->MixerStream().SetHue(int(m_cImage[m_iImageID]->m_uHue)-100);
					}
					else if (m_uActivePage == RGB)
					{
						m_cImage[m_iImageID]->m_uR = UINT(MAX_R_VAL*COLOR_PERCENT(message));
						m_cImage[m_iImageID]->MixerStream().SetR(int(m_cImage[m_iImageID]->m_uR)-100);
					}
				}
				break;
			case COLOR2_SLIDER:
				{
					if (m_uActivePage == BRIGHT)
					{
						m_cImage[m_iImageID]->m_uContrast = UINT(MAX_CONTRAST_VAL*COLOR_PERCENT(message));	
						m_cImage[m_iImageID]->MixerStream().SetContrast(int(m_cImage[m_iImageID]->m_uContrast)-100);
					}
					else if (m_uActivePage == HUE)
					{
						m_cImage[m_iImageID]->m_uSaturation = UINT(MAX_SATURATION_VAL*COLOR_PERCENT(message));
						m_cImage[m_iImageID]->MixerStream().SetSaturation(int(m_cImage[m_iImageID]->m_uSaturation)-100);
					}
					else if (m_uActivePage == RGB)
					{
						m_cImage[m_iImageID]->m_uG = UINT(MAX_G_VAL*COLOR_PERCENT(message));
						m_cImage[m_iImageID]->MixerStream().SetG(int(m_cImage[m_iImageID]->m_uG)-100);
					}
				}
				break;
			case COLOR3_SLIDER:
				{
					m_cImage[m_iImageID]->m_uB = UINT(MAX_B_VAL*COLOR_PERCENT(message));
					m_cImage[m_iImageID]->MixerStream().SetB(int(m_cImage[m_iImageID]->m_uB)-100);
				}
				break;

				
			//Effec Slider
			
			case LEVEL_SLIDER:
			case PARAM1_SLIDER:
			case PARAM2_SLIDER:
			case PARAM3_SLIDER:
			case PARAM4_SLIDER:
			case PARAM5_SLIDER:
			{
				//m_cImage[m_iImageID]->m_cEffect.GetEffectParam().iLevel = g_EffectParam.iLevel = int(MAX_LEVEL_VAL*LEVEL_PERCENT);
				if (m_cImage[m_iImageID]->StreamExist())
				{
					INT32 iParmIdx = message - LEVEL_SLIDER;
					INT32 iParmVal;
					
					if (message == LEVEL_SLIDER)
					{
						iParmVal = int(MAX_LEVEL_VAL*LEVEL_PERCENT);
					}
					else
					{
						iParmVal = int(MAX_PARAM_VAL*EFFEC_PERCENT(message));
					}
					m_cImage[m_iImageID]->MixerStream().SetEffectParam(iParmIdx, iParmVal);
					m_sParmVal[iParmIdx].Format("%d", iParmVal);
					UpdateData(FALSE);
				}
			}
			break;
			default:
				break;
		}
	}

	//not relate to stream
	
	switch (message)
	{
	//refresh RGB block UI(text, slider button)

			case BRIGHT_PAGE:
				{
					m_uActivePage = BRIGHT;

					m_FramePic->DeleteItem(COLOR3_SLIDER);
					RefreshRgbCtrl();
					Invalidate();
					UpdateWindow();
					RefreshRgbText();
				}
				break;
			case HUE_PAGE:
				{
					m_uActivePage = HUE;				

					m_FramePic->DeleteItem(COLOR3_SLIDER);				
					RefreshRgbCtrl();
					Invalidate();
					UpdateWindow();
					RefreshRgbText();
				}
			break;
			case RGB_PAGE:
				{
					m_uActivePage = RGB;

					m_FramePic->DeleteItem(COLOR3_SLIDER);				
					AddItem(COLOR3_SLIDER);
					//?? lost to do m_FramePic->OnZoom() to adjust the true pos match to current screen size
					RefreshRgbCtrl();
					Invalidate();
					UpdateWindow();
					RefreshRgbText();
				}
			break;	
	}
	
	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}


void CMixerCtrlDlg::OnCancel()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	GetParent()->SendMessage(MSG_MAINUI);
	ShowWindow(SW_HIDE);
	return;
	CDialog::OnCancel();
}


void CMixerCtrlDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (!(nFlags & MK_LBUTTON)) return;

	if (m_bVolClicked && m_cImage[m_iImageID]->StreamExist())
	{
		if (point.y != m_OldMousePnt.y)	VolPntPos((point.y > m_OldMousePnt.y)? true: false);
		m_OldMousePnt.y = point.y;
	}

	//for move zoomed image arbitrarily by clicking mouse and moving at the same time in the boundary of zoomed rectangle
	if (m_cImage[m_iImageID]->StreamExist() && m_cImage[m_iImageID]->m_bZmPaintBit)
	{
		CRect zoomedRect(m_cImage[m_iImageID]->m_ZoomLTpnt, m_cImage[m_iImageID]->m_ZoomRBpnt);

		if (zoomedRect.PtInRect(point))
		{
			DrawZoomRect();

			if (point.x != m_OldMousePnt.x)
			{
				int diff = point.x - m_OldMousePnt.x;

				if ((m_cImage[m_iImageID]->m_ZoomLTpnt.x + diff)+1 < m_ZoomOriRBpnt.x &&
					(m_cImage[m_iImageID]->m_ZoomRBpnt.x + diff) > m_ZoomOriLTpnt.x+1)
				{
					m_cImage[m_iImageID]->m_ZoomLTpnt.x += diff;
					m_cImage[m_iImageID]->m_ZoomRBpnt.x += diff;
				}
			}
			if (point.y != m_OldMousePnt.y)
			{
				int diff = point.y - m_OldMousePnt.y;

				if ((m_cImage[m_iImageID]->m_ZoomLTpnt.y + diff)+1 < m_ZoomOriRBpnt.y &&
					(m_cImage[m_iImageID]->m_ZoomRBpnt.y + diff) > m_ZoomOriLTpnt.y+1)
				{
					m_cImage[m_iImageID]->m_ZoomLTpnt.y += diff;
					m_cImage[m_iImageID]->m_ZoomRBpnt.y += diff;
				}
			}
			
			DrawZoomRect();
			
			UpdateData(FALSE);
			
			m_OldMousePnt = point;	
		}
	}
	   
	CDialog::OnMouseMove(nFlags, point);
}

void CMixerCtrlDlg::OnLButtonDown(UINT nFlags, CPoint point) //event function only for icons directly painted on BG1.bmp such as zoom In-Out and vol icon
{																  //other click event on other items added into m_FramePic is triggered in function OnWndMsg
	//Mixer Layer Clicked detect to enable
	CRect Layer1Wnd(IWSCALE(8), IHSCALE(8), IWSCALE(637), IHSCALE(515)),
		  Layer2Wnd(IWSCALE(641), IHSCALE(8), IWSCALE(1270), IHSCALE(515)),
		  MixerWnd(IWSCALE(1275), IHSCALE(8), IWSCALE(1912), IHSCALE(515));
	
	if (Layer1Wnd.PtInRect(point))
	{
		DrawActivePnt(CPoint(Layer1Wnd.BottomRight().x-10, Layer1Wnd.BottomRight().y-10), LAYER1);
		m_EffectCombox.SetCurSel(m_cImage[m_iImageID]->m_cEffect.GetEffectID());
		RefreshEffectCtrl();
	}
	if (Layer2Wnd.PtInRect(point))
	{
		DrawActivePnt(CPoint(Layer2Wnd.BottomRight().x-10, Layer1Wnd.BottomRight().y-10), LAYER2);
		m_EffectCombox.SetCurSel(m_cImage[m_iImageID]->m_cEffect.GetEffectID());
		RefreshEffectCtrl();
	}
	if (MixerWnd.PtInRect(point))
	{
		DrawActivePnt(CPoint( MixerWnd.BottomRight().x-10,  MixerWnd.BottomRight().y-10), MIXER);
		m_EffectCombox.SetCurSel(m_cImage[m_iImageID]->m_cEffect.GetEffectID());
		RefreshEffectCtrl();
	}


	if (m_cImage[m_iImageID]->StreamExist())
	{
		//Volume Circle Button Region detect
		if ((pow(abs(point.x - m_VolCenter.x), 2.f) + pow(abs(point.y - m_VolCenter.y), 2.f)) < pow(m_fRadius, 2.f))
		{
			m_bVolClicked = TRUE;
			m_OldMousePnt = point;
		}
		
		//Volume Micro Adjustment Icon "+" "-" Region detect
		CRect volPlus(IWSCALE(1889), IHSCALE(1032), IWSCALE(1907), IHSCALE(1052)),
			  volMinus(IWSCALE(1742), IHSCALE(1032), IWSCALE(1762), IHSCALE(1052));

		if (volPlus.PtInRect(point))  VolPntPos(true);
		if (volMinus.PtInRect(point)) VolPntPos(false);

		//Zoom In-Out Client Region detect
		CRect zoomOriRect(m_ZoomOriLTpnt, m_ZoomOriRBpnt);
		
		if (zoomOriRect.PtInRect(point))
		{
			DrawZoomRect();
			m_OldMousePnt = point;
		}
	}
}

void CMixerCtrlDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_cImage[m_iImageID]->m_bZmPaintBit)
	{
		CRect zoomOriRect(m_ZoomOriLTpnt, m_ZoomOriRBpnt);

		if (zoomOriRect.PtInRect(point))
		{
			DrawZoomRect();
			m_cImage[m_iImageID]->m_fZoomRate = 1.f;
			m_cImage[m_iImageID]->m_ZoomLTpnt = m_ZoomOriLTpnt;
			m_cImage[m_iImageID]->m_ZoomRBpnt = m_ZoomOriRBpnt;
			m_cImage[m_iImageID]->ToNoZoomSym(CRect(m_ZoomOriLTpnt, m_ZoomOriRBpnt));
			DrawZoomRect();
			UpdateData(FALSE);
		}
	}

	#if 0
	//temporarily for open media file
	CRect volPlus(IWSCALE(1889), IHSCALE(1032), IWSCALE(1907), IHSCALE(1052));

	if (volPlus.PtInRect(point))
	{
		char szFile[256];
		CFileDialog fileDlg(TRUE, _T("txt"), "", OFN_EXPLORER | OFN_OVERWRITEPROMPT, _T("MP4 Files (*.mp4)|*.mp4|MKV Files (*.mkv)|*.mkv|MPEG Files (*.mpg)|*.mpg|MOV Files (*.mov)|*.mov|WMV Files (*.wmv)|*.wmv|VOB Files (*.vob)|*.vob|RMVB Files (*.rmvb)|*.rmvb|ALL Files (*.*)|*.*||"));
		

		if (fileDlg.DoModal() == IDOK)
		{
			strcpy(szFile, fileDlg.GetPathName());
			SetDlgItemText(IDC_FILENAME, szFile);
		}
	}
	#endif
}

void CMixerCtrlDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bVolClicked = FALSE;
}


void CMixerCtrlDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (bShow)
	{
		//CRect rect;

		//GetWindowRect(&rect);
		//m_fWscaling = rect.Width() / 1920.f;
		//m_fHscaling = rect.Height() / 1080.f;

		//init original zoom rect position
		m_ZoomOriLTpnt.x = LWSCALE(684);
		m_ZoomOriLTpnt.y = LHSCALE(707);
		m_ZoomOriRBpnt.x = LWSCALE(831);
		m_ZoomOriRBpnt.y = LHSCALE(802);

#if 0
		CRect zoomOriRect(m_ZoomOriLTpnt, m_ZoomOriRBpnt);

		LONG offset_H = LONG(zoomOriRect.Width()*(1.f - m_fZoomRate) / 2);
		LONG offset_V = LONG(zoomOriRect.Height()*(1.f - m_fZoomRate) / 2);
#endif

		//init zoomed rect position as original zoom rect position at start
		for (int i=0; i<PLAY_THREAD_NUM; i++)
		{
			CPoint zeroPnt(0, 0);

			if (m_cImage[i]->m_ZoomLTpnt == zeroPnt && m_cImage[i]->m_ZoomRBpnt == zeroPnt)
			{
				m_cImage[i]->m_ZoomLTpnt = m_ZoomOriLTpnt;
				m_cImage[i]->m_ZoomRBpnt = m_ZoomOriRBpnt;
				m_cImage[i]->ToNoZoomSym(CRect(m_ZoomOriLTpnt, m_ZoomOriRBpnt));
			}
		}

		//?? m_ZoomXspin need to move to cimageArea
		m_ZoomXspin.SetRange(short(m_ZoomOriLTpnt.x-100), short(m_ZoomOriLTpnt.x+100));
		m_ZoomXspin.SetPos(m_ZoomOriLTpnt.x);
		m_ZoomYspin.SetRange(short(m_ZoomOriLTpnt.y-100), short(m_ZoomOriLTpnt.y+100));
		m_ZoomYspin.SetPos(m_ZoomOriLTpnt.y);


		UpdateData(FALSE);
		
#if 0
		m_ZoomLTpnt.x +=/*= m_ZoomOriLTpnt.x +*/ offset_H;
		m_ZoomLTpnt.y +=/*m_ZoomOriLTpnt.y +*/ offset_V;
		m_ZoomRBpnt.x -=/*m_ZoomOriRBpnt.x -*/ offset_H;
		m_ZoomRBpnt.y -=/*m_ZoomOriRBpnt.y -*/ offset_V;
#endif

		//init vol white point position
		m_VolCenter.x = LWSCALE(1825);
		m_VolCenter.y = LHSCALE(998);

		for (int i=0; i<PLAY_THREAD_NUM; i++)
		{
			double angle = PI*m_cImage[i]->m_iAngle/180.0;

			m_cImage[i]->m_VolPoint.x = LONG(m_VolCenter.x + m_fRadius * cos(angle));
			m_cImage[i]->m_VolPoint.y = LONG(m_VolCenter.y + m_fRadius * sin(angle));
		}

		//init cdc mode		
		m_cdc->SetROP2(R2_XORPEN);

		//init Previous Activated Window
		m_PreActPnt.SetPoint(0, 0);
		m_bDrawActIcon = 0;

		//init Zoom Rect Paint bit
		for (int i=0; i<PLAY_THREAD_NUM; i++)
			m_cImage[i]->m_bZmPaintBit = 0;

		//init Vol Point Paint bit
		for (int i=0; i<PLAY_THREAD_NUM; i++)
			m_cImage[i]->m_bVolPnt = 0;

		//timer enable
		SetTimer(1, 100, NULL);
		
	}
	else
	{
		KillTimer(1);
	}

	m_bShow = bShow;

	CDialog::OnShowWindow(bShow, nStatus);
}

BOOL CMixerCtrlDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_cImage[m_iImageID]->m_bZmPaintBit)
	{
		LONG offset_H = 2;
		LONG offset_V = 2;

		CRect zoomOriRect(m_ZoomOriLTpnt, m_ZoomOriRBpnt), zoomRect(m_cImage[m_iImageID]->m_ZoomLTpnt, m_cImage[m_iImageID]->m_ZoomRBpnt);

		if ((zDelta > 0) && ((zoomOriRect.Width() - zoomRect.Width()) >= (zoomOriRect.Width()-offset_H*2) || (zoomOriRect.Height() - zoomRect.Height()) >= zoomOriRect.Height()-offset_V*2))
			return 0;
		if ((zDelta < 0) && ((zoomRect.Width() - zoomOriRect.Width()) >= 100 || (zoomRect.Height() - zoomOriRect.Height()) >= 100))
			return 0;

		DrawZoomRect();

		if (zDelta > 0)
		{
			m_cImage[m_iImageID]->m_ZoomLTpnt.x += offset_H;
			m_cImage[m_iImageID]->m_ZoomLTpnt.y += offset_V;
			m_cImage[m_iImageID]->m_ZoomRBpnt.x -= offset_H;
			m_cImage[m_iImageID]->m_ZoomRBpnt.y -= offset_V;
		}
		else
		{
			m_cImage[m_iImageID]->m_ZoomLTpnt.x -= offset_H;
			m_cImage[m_iImageID]->m_ZoomLTpnt.y -= offset_V;
			m_cImage[m_iImageID]->m_ZoomRBpnt.x += offset_H;
			m_cImage[m_iImageID]->m_ZoomRBpnt.y += offset_V;
		}

		m_cImage[m_iImageID]->m_fZoomRate = float(zoomRect.Width()) / zoomOriRect.Width();

		DrawZoomRect();

		UpdateData(FALSE);
	}
	return 0;
}

void CMixerCtrlDlg::RefreshEffectText()
{
	INT iSelEffect = m_cImage[m_iImageID]->m_cEffect.GetEffectID(); //((CComboBox*)GetDlgItem(IDC_COMBO_ID))->GetCurSel();
	
	if (iSelEffect >= 0 && iSelEffect < m_ParmNameAry.GetCount())
	{
		for (int x = 0; x < 6; x++)
		{
			m_cdc->SetBkMode(TRANSPARENT);
			m_cdc->SetTextColor(RGB(255, 255, 255));
	
			int y_axis_pos[]={IHSCALE(599), IHSCALE(645), IHSCALE(704), IHSCALE(763), IHSCALE(824), IHSCALE(883), 0};
				
			//if (m_ParmNameAry[iSelEffect].strParmName[0].CompareNoCase("Level")!=0)
			//	m_cdc->TextOut(1000, y_axis_pos[x+1], m_ParmNameAry[iSelEffect].strParmName[x]);
			//else
				m_cdc->TextOut(IWSCALE(980), y_axis_pos[x], m_ParmNameAry[iSelEffect].strParmName[x]);
		}
	}
}

void CMixerCtrlDlg::RefreshEffectCtrl()
{
	INT iSelEffect = m_cImage[m_iImageID]->m_cEffect.GetEffectID(); //((CComboBox*)GetDlgItem(IDC_COMBO_ID))->GetCurSel();
	
	if (iSelEffect >= 0 && iSelEffect < m_ParmNameAry.GetCount())
	{
		BOOL bEmptyParm;
			
		m_cImage[m_iImageID]->m_uEffItemBmp = 0;
			
		for (int x=0; x<6; x++)
		{
			bEmptyParm = m_ParmNameAry[iSelEffect].strParmName[x].IsEmpty();
				
			if (!bEmptyParm) 
			{
				//if (m_ParmNameAry[iSelEffect].strParmName[0].CompareNoCase("Level")==0)
					m_cImage[m_iImageID]->m_uEffItemBmp |= 0x01<<x;
				//else
					//m_uEffItemBmp |= 0x02<<x;
					
			}
		}
	
		BOOL bStreamExist = m_cImage[m_iImageID]->StreamExist();

		CPoint ptBtn;
		int Ptmp, PsliderY;
		for (int x=0; x<6; x++)
		{
			int slider_id[]={LEVEL_SLIDER, PARAM1_SLIDER, PARAM2_SLIDER, PARAM3_SLIDER, PARAM4_SLIDER, PARAM5_SLIDER};
	
			m_FramePic->DeleteItem(slider_id[x]);
				
			if (((m_cImage[m_iImageID]->m_uEffItemBmp>>x)&0x01)==1)
			{
				AddItem(slider_id[x]);
				//?? lost to do m_FramePic->OnZoom() to adjust the true pos match to current screen size

				if (slider_id[x]==LEVEL_SLIDER) //for Level
				{
					ptBtn = CPoint(LEVEL_SLIDER_X_BEGIN + m_cImage[m_iImageID]->m_cEffect.GetEffectParam().iLevel*LEVEL_SLIDER_LEN/MAX_LEVEL_VAL, LEVEL_SLIDER_Y);
				}
				else //for PARAMs
				{
					ptBtn = CPoint(PARAM_SLIDER_X_BEGIN + Ptmp*PARAM_SLIDER_LEN / MAX_PARAM_VAL, PsliderY);
				}

				if (m_cImage[m_iImageID]->StreamExist())
				{
					Ptmp = m_cImage[m_iImageID]->MixerStream().GetEffectParam(x);
					PsliderY = PARAM1_SLIDER_Y;

					m_sParmVal[x].Format("%d", Ptmp);
					UpdateData(FALSE);
				}
				((CMoveable_Item*)m_FramePic->GetItemByID(slider_id[x]))->SetToPos(ptBtn, TRUE);
				((CMoveable_Item*)m_FramePic->GetItemByID(slider_id[x]))->RedrawBGImage(TRUE);

				m_ParamStatic[x].ShowWindow(SW_SHOW);
			}
			else
				m_ParamStatic[x].ShowWindow(SW_HIDE);
		}
	}
}

void CMixerCtrlDlg::RefreshRgbText()
{
	switch(m_uActivePage)
	{
		case BRIGHT:
		{
			m_cdc->SetBkMode(TRANSPARENT);
			m_cdc->SetTextColor(RGB(255, 255, 255));
			m_cdc->TextOut(50, 580, "Brightness");
			m_cdc->TextOut(50, 690, "Contrast");
		}
		break;
		case HUE:
		{
			m_cdc->SetBkMode(TRANSPARENT);
			m_cdc->SetTextColor(RGB(255, 255, 255));
			m_cdc->TextOut(50, 580, "Hue");
			m_cdc->TextOut(50, 690, "Saturation");
		}
		break;
		case RGB:
		{
			m_cdc->SetBkMode(TRANSPARENT);
			m_cdc->SetTextColor(RGB(255, 255, 255));
			m_cdc->TextOut(50, 580, "R");
			m_cdc->TextOut(50, 690, "G");
			m_cdc->TextOut(50, 800, "B");
		}
		break;
	}
}

void CMixerCtrlDlg::RefreshRgbCtrl()
{
	switch(m_uActivePage)
	{
		case BRIGHT:
		{
			CPoint BriBtnPos(COLOR_SLIDER_X_BEGIN + m_cImage[m_iImageID]->m_uBright*COLOR_SLIDER_LEN/MAX_BRIGHT_VAL, COLOR1_SLIDER_Y);
			((CMoveable_Item*)m_FramePic->GetItemByID(COLOR1_SLIDER))->SetToPos(BriBtnPos, TRUE);
			
			CPoint CntrtBtnPos(COLOR_SLIDER_X_BEGIN+m_cImage[m_iImageID]->m_uContrast*COLOR_SLIDER_LEN/MAX_CONTRAST_VAL, COLOR2_SLIDER_Y);
			((CMoveable_Item*)m_FramePic->GetItemByID(COLOR2_SLIDER))->SetToPos(CntrtBtnPos, TRUE);
		}
		break;
		case HUE:
		{
			CPoint HueBtnPos(COLOR_SLIDER_X_BEGIN + m_cImage[m_iImageID]->m_uHue*COLOR_SLIDER_LEN/MAX_HUE_VAL, COLOR1_SLIDER_Y);
			((CMoveable_Item*)m_FramePic->GetItemByID(COLOR1_SLIDER))->SetToPos(HueBtnPos, TRUE);
			
			CPoint SatBtnPos(COLOR_SLIDER_X_BEGIN + m_cImage[m_iImageID]->m_uSaturation*COLOR_SLIDER_LEN/MAX_SATURATION_VAL, COLOR2_SLIDER_Y);
			((CMoveable_Item*)m_FramePic->GetItemByID(COLOR2_SLIDER))->SetToPos(SatBtnPos, TRUE);
		}
		break;
		case RGB:
		{
			CPoint R_BtnPos(COLOR_SLIDER_X_BEGIN + m_cImage[m_iImageID]->m_uR*COLOR_SLIDER_LEN/MAX_R_VAL, COLOR1_SLIDER_Y);
			((CMoveable_Item*)m_FramePic->GetItemByID(COLOR1_SLIDER))->SetToPos(R_BtnPos, TRUE);
			
			CPoint G_BtnPos(COLOR_SLIDER_X_BEGIN + m_cImage[m_iImageID]->m_uG*COLOR_SLIDER_LEN/MAX_G_VAL, COLOR2_SLIDER_Y);
			((CMoveable_Item*)m_FramePic->GetItemByID(COLOR2_SLIDER))->SetToPos(G_BtnPos, TRUE);
			
			CPoint B_BtnPos(COLOR_SLIDER_X_BEGIN + m_cImage[m_iImageID]->m_uB*COLOR_SLIDER_LEN/MAX_B_VAL, COLOR3_SLIDER_Y);
			((CMoveable_Item*)m_FramePic->GetItemByID(COLOR3_SLIDER))->SetToPos(B_BtnPos, TRUE);
			
			((CMoveable_Item*)m_FramePic->GetItemByID(COLOR3_SLIDER))->RedrawBGImage(true);
		}
		break;
	}
	((CMoveable_Item*)m_FramePic->GetItemByID(COLOR1_SLIDER))->RedrawBGImage(true);
	((CMoveable_Item*)m_FramePic->GetItemByID(COLOR2_SLIDER))->RedrawBGImage(true);
}

void CMixerCtrlDlg::PrintZoomXYText()
{
	m_cdc->SetBkMode(TRANSPARENT);
	m_cdc->SetTextColor(RGB(255, 255, 255));
	m_cdc->TextOut(IWSCALE(680), IHSCALE(846), "X");
	m_cdc->TextOut(IWSCALE(776), IHSCALE(846), "Y");
	m_cdc->TextOut(IWSCALE(680), IHSCALE(900), "Width:");
	m_cdc->TextOut(IWSCALE(776), IHSCALE(900), "Height:");
	m_cdc->TextOut(IWSCALE(1667), IHSCALE(875), "Panel ID");
	//m_cdc->TextOut(IWSCALE(1680), IHSCALE(575), "Effect");  original position
	m_cdc->TextOut(IWSCALE(1663), IHSCALE(575), "Effect");
	m_cdc->TextOut(IWSCALE(1864), IHSCALE(769), "System");	
	m_cdc->TextOut(IWSCALE(1868), IHSCALE(783), "Sound");	
}

void CMixerCtrlDlg::AddItem(int iIDNum)
{
	CMoveable_Item *pMoveItem;

	switch (iIDNum)
	{
		case COLOR1_SLIDER:
			pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Color-slider__01.bmp",
																 theApp.m_strCurPath + "\\UI Folder\\Mixer\\Color-slider.bmp",
																 CPoint(COLOR_SLIDER_X_BEGIN, COLOR1_SLIDER_Y), CPoint(COLOR_SLIDER_X_END, COLOR1_SLIDER_Y));
			break;
		case COLOR2_SLIDER:
			pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Color-slider__01.bmp",
																 theApp.m_strCurPath + "\\UI Folder\\Mixer\\Color-slider.bmp",
																 CPoint(COLOR_SLIDER_X_BEGIN, COLOR2_SLIDER_Y), CPoint(COLOR_SLIDER_X_END, COLOR2_SLIDER_Y));
			break;
		case COLOR3_SLIDER:
			//pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Color-slider.bmp", true, true, true);
			pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Color-slider__01.bmp",
																 theApp.m_strCurPath + "\\UI Folder\\Mixer\\Color-slider.bmp",
																 CPoint(COLOR_SLIDER_X_BEGIN, COLOR3_SLIDER_Y), CPoint(COLOR_SLIDER_X_END, COLOR3_SLIDER_Y));
			break;
		case LEVEL_SLIDER:
			pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Level_Silder.bmp",
																 theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parameter-L-b.bmp",
																 CPoint(LEVEL_SLIDER_X_BEGIN, LEVEL_SLIDER_Y), CPoint(LEVEL_SLIDER_X_END, LEVEL_SLIDER_Y));
			break;
		case PARAM1_SLIDER:
			pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parm_Silder.bmp",
																 theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parameter-s01-b.bmp",
																 CPoint(PARAM_SLIDER_X_BEGIN, PARAM1_SLIDER_Y), CPoint(PARAM_SLIDER_X_END, PARAM1_SLIDER_Y));
			break;
		case PARAM2_SLIDER:
			pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parm_Silder.bmp",
																 theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parameter-s01-b.bmp",
																 CPoint(PARAM_SLIDER_X_BEGIN, PARAM2_SLIDER_Y), CPoint(PARAM_SLIDER_X_END, PARAM2_SLIDER_Y));
			break;
		case PARAM3_SLIDER:
			pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parm_Silder.bmp",
																 theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parameter-s01-b.bmp",
																 CPoint(PARAM_SLIDER_X_BEGIN, PARAM3_SLIDER_Y), CPoint(PARAM_SLIDER_X_END, PARAM3_SLIDER_Y));
			break;
		case PARAM4_SLIDER:
			pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parm_Silder.bmp",
																 theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parameter-s01-b.bmp",
																 CPoint(PARAM_SLIDER_X_BEGIN, PARAM4_SLIDER_Y), CPoint(PARAM_SLIDER_X_END, PARAM4_SLIDER_Y));
			break;
		case PARAM5_SLIDER:
			pMoveItem = new CMoveable_Item(this, m_hWnd, iIDNum, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parm_Silder.bmp",
																 theApp.m_strCurPath + "\\UI Folder\\Mixer\\Parameter-s01-b.bmp",
																 CPoint(PARAM_SLIDER_X_BEGIN, PARAM5_SLIDER_Y), CPoint(PARAM_SLIDER_X_END, PARAM5_SLIDER_Y));
			break;
	}

	switch (iIDNum)
	{
		case COLOR1_SLIDER:
			//pMoveItem->SetItemMoveLimit(CPoint(COLOR_SLIDER_X_BEGIN, COLOR1_SLIDER_Y), CPoint(COLOR_SLIDER_X_END, COLOR1_SLIDER_Y));
			pMoveItem->OffsetObject(CPoint(COLOR_SLIDER_X_BEGIN, COLOR1_SLIDER_Y));
			break;			
		case COLOR2_SLIDER:
			//pMoveItem->SetItemMoveLimit(CPoint(COLOR_SLIDER_X_BEGIN, COLOR2_SLIDER_Y), CPoint(COLOR_SLIDER_X_END, COLOR2_SLIDER_Y));
			pMoveItem->OffsetObject(CPoint(COLOR_SLIDER_X_BEGIN, COLOR2_SLIDER_Y));
			break;
		case COLOR3_SLIDER:
			//pMoveItem->SetItemMoveLimit(CPoint(COLOR_SLIDER_X_BEGIN, COLOR3_SLIDER_Y), CPoint(COLOR_SLIDER_X_END, COLOR3_SLIDER_Y));
			pMoveItem->OffsetObject(CPoint(COLOR_SLIDER_X_BEGIN, COLOR3_SLIDER_Y));
			break;
		case LEVEL_SLIDER:
			//pMoveItem->SetItemMoveLimit(CPoint(LEVEL_SLIDER_X_BEGIN, LEVEL_SLIDER_Y), CPoint(LEVEL_SLIDER_X_END, LEVEL_SLIDER_Y));
			pMoveItem->OffsetObject(CPoint(LEVEL_SLIDER_X_BEGIN, LEVEL_SLIDER_Y));
			break;
		case PARAM1_SLIDER:
			//pMoveItem->SetItemMoveLimit(CPoint(PARAM_SLIDER_X_BEGIN, PARAM1_SLIDER_Y), CPoint(PARAM_SLIDER_X_END, PARAM1_SLIDER_Y));
			pMoveItem->OffsetObject(CPoint(PARAM_SLIDER_X_BEGIN, PARAM1_SLIDER_Y));
			break;
		case PARAM2_SLIDER:
			//pMoveItem->SetItemMoveLimit(CPoint(PARAM_SLIDER_X_BEGIN, PARAM2_SLIDER_Y), CPoint(PARAM_SLIDER_X_END, PARAM2_SLIDER_Y));
			pMoveItem->OffsetObject(CPoint(PARAM_SLIDER_X_BEGIN, PARAM2_SLIDER_Y));
			break;
		case PARAM3_SLIDER:
			//pMoveItem->SetItemMoveLimit(CPoint(PARAM_SLIDER_X_BEGIN, PARAM3_SLIDER_Y), CPoint(PARAM_SLIDER_X_END, PARAM3_SLIDER_Y));
			pMoveItem->OffsetObject(CPoint(PARAM_SLIDER_X_BEGIN, PARAM3_SLIDER_Y));
			break;
		case PARAM4_SLIDER:
			//pMoveItem->SetItemMoveLimit(CPoint(PARAM_SLIDER_X_BEGIN, PARAM4_SLIDER_Y), CPoint(PARAM_SLIDER_X_END, PARAM4_SLIDER_Y));
			pMoveItem->OffsetObject(CPoint(PARAM_SLIDER_X_BEGIN, PARAM4_SLIDER_Y));
			break;
		case PARAM5_SLIDER:
			//pMoveItem->SetItemMoveLimit(CPoint(PARAM_SLIDER_X_BEGIN, PARAM5_SLIDER_Y), CPoint(PARAM_SLIDER_X_END, PARAM5_SLIDER_Y));
			pMoveItem->OffsetObject(CPoint(PARAM_SLIDER_X_BEGIN, PARAM5_SLIDER_Y));
			break;
	}
	
	pMoveItem->SetParentMenu(m_FramePic);
	m_FramePic->AddItem(pMoveItem);
	pMoveItem->Initialize();
	//?? lost to do m_FramePic->OnZoom() to adjust the true pos match to current screen size
}

void CMixerCtrlDlg::OnPaint()
{
	Invalidate();
	UpdateWindow();

	RefreshRgbText();
	RefreshEffectText();
	PrintZoomXYText();

	m_PreActPnt.SetPoint(0, 0);
	m_bDrawActIcon = 0;
	
	m_cImage[m_iImageID]->m_bZmPaintBit = 0;
	m_cImage[m_iImageID]->m_bVolPnt = 0;
	
	CDialog::OnPaint();
}

void CMixerCtrlDlg::OnSelchange()
{
	//Invalidate();
	//UpdateWindow();

	m_cImage[m_iImageID]->m_cEffect.SetEffectID(((CComboBox*)GetDlgItem(IDC_COMBO_ID))->GetCurSel());
	RefreshEffectCtrl();

	m_PreActPnt.SetPoint(0, 0);
	m_bDrawActIcon = 0;
}

void CMixerCtrlDlg::OnSelPanelchange()
{
	CString val;

	((CComboBox*)GetDlgItem(IDC_PANEL_COMBO_ID))->GetWindowTextA(val);
	g_PannelSetting.iSelIdx = atoi((const char *) val)-1;
}

void CMixerCtrlDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	if (nSBCode == SB_ENDSCROLL)
		return;

	if (m_cImage[m_iImageID]->m_bZmPaintBit)
	{
		switch (pScrollBar->GetDlgCtrlID())
		{
			case IDC_XSPIN_ID:
				{
					DrawZoomRect();
					
					int ori_Xval = m_cImage[m_iImageID]->m_ZoomLTpnt.x;
					UpdateData();																			//update  m_zoomLTpnt.x
					m_cImage[m_iImageID]->m_ZoomRBpnt.x += (m_cImage[m_iImageID]->m_ZoomLTpnt.x-ori_Xval);	//update  m_zoomRBpnt.x with the same direction offset of m_zoomLTpnt.x
				
					DrawZoomRect();
				}
				break;
			case IDC_YSPIN_ID:
				{
					DrawZoomRect();
					
					int ori_Yval = m_cImage[m_iImageID]->m_ZoomLTpnt.y;
					UpdateData();																			//update  m_zoomLTpnt.x
					m_cImage[m_iImageID]->m_ZoomRBpnt.y += (m_cImage[m_iImageID]->m_ZoomLTpnt.y-ori_Yval);	//update  m_zoomRBpnt.x with the same direction offset of m_zoomLTpnt.x
				
					DrawZoomRect();
				}			
				break;
		}
	}
	else
		UpdateData(FALSE);


	if (pScrollBar->GetDlgCtrlID() == IDC_SLIDER)
	{
		m_cImage[m_iImageID]->MixerStream().SetSystemVolume(SYS_SOUND_VAL(m_SysSndSldCtrl.GetPos()));
		m_sSysSound.Format("%d", SYS_SOUND_VAL(m_SysSndSldCtrl.GetPos()));
		UpdateData(FALSE);
	}
	
	//CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)pScrollBar;
	//CString strVal;
	//strVal.Format("%3.1f", (double)pSpin->GetPos()/10);
	//pSpin->GetBuddy()->SetWindowText(strVal);

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CMixerCtrlDlg::VolPntPos(bool greater)
{
	DrawVolPnt();

	if (greater) m_cImage[m_iImageID]->m_iAngle++;
	else 		 m_cImage[m_iImageID]->m_iAngle--;
	
	if (m_cImage[m_iImageID]->m_iAngle > 220) m_cImage[m_iImageID]->m_iAngle=220;
	if (m_cImage[m_iImageID]->m_iAngle < 0)   m_cImage[m_iImageID]->m_iAngle=0;

	m_cImage[m_iImageID]->MixerStream().SetVolume(int(float(m_cImage[m_iImageID]->m_iAngle*MAX_VOLUME_VAL)/220));
	
	double angle = PI*m_cImage[m_iImageID]->m_iAngle/180.0;

	m_cImage[m_iImageID]->m_VolPoint.x = LONG(m_VolCenter.x + m_fRadius * cos(angle));
	m_cImage[m_iImageID]->m_VolPoint.y = LONG(m_VolCenter.y + m_fRadius * sin(angle));

	DrawVolPnt();
}

void CMixerCtrlDlg::InitVolPntPos()
{
	int vol = m_cImage[m_iImageID]->MixerStream().GetVolume();

	m_cImage[m_iImageID]->m_iAngle = vol*220/MAX_VOLUME_VAL;

	double angle = PI*m_cImage[m_iImageID]->m_iAngle/180.0;
			
	m_cImage[m_iImageID]->m_VolPoint.SetPoint(LONG(m_VolCenter.x + m_fRadius * cos(angle)), LONG(m_VolCenter.y + m_fRadius * sin(angle)));
}


void CMixerCtrlDlg::DrawZoomRect()
{
	COLORREF penColor;

	if      (m_cImage[m_iImageID]->m_fZoomRate < 1.f)		 penColor = RGB(255, 0, 0);
	else if (m_cImage[m_iImageID]->m_fZoomRate ==1.f)        penColor = RGB(0, 255, 0);
	else                                                     penColor = RGB(0, 0, 255);

	CPen myPen(PS_DASH | PS_ENDCAP_SQUARE, 2, penColor), *oldPen;

	oldPen = m_cdc->SelectObject(&myPen);

	m_cdc->MoveTo(m_cImage[m_iImageID]->m_ZoomLTpnt);
	m_cdc->LineTo(m_cImage[m_iImageID]->m_ZoomRBpnt.x, m_cImage[m_iImageID]->m_ZoomLTpnt.y);
	m_cdc->LineTo(m_cImage[m_iImageID]->m_ZoomRBpnt.x, m_cImage[m_iImageID]->m_ZoomRBpnt.y);
	m_cdc->LineTo(m_cImage[m_iImageID]->m_ZoomLTpnt.x, m_cImage[m_iImageID]->m_ZoomRBpnt.y);
	m_cdc->LineTo(m_cImage[m_iImageID]->m_ZoomLTpnt);

	m_cdc->SelectObject(oldPen);

	m_cImage[m_iImageID]->m_PreZmRectPos.SetRect(m_cImage[m_iImageID]->m_ZoomLTpnt, m_cImage[m_iImageID]->m_ZoomRBpnt);
	m_cImage[m_iImageID]->m_bZmPaintBit ^= 1;
}

void CMixerCtrlDlg::DrawVolPnt()
{
	//CClientDC cdc(this);
	CPen *myPen, *oldPen;
	
	myPen = new CPen(PS_SOLID|PS_ENDCAP_ROUND, 10, RGB(255, 255, 255));
	oldPen = m_cdc->SelectObject(myPen);
	
	m_cdc->MoveTo(m_cImage[m_iImageID]->m_VolPoint.x, m_cImage[m_iImageID]->m_VolPoint.y);
	m_cdc->LineTo(m_cImage[m_iImageID]->m_VolPoint.x, m_cImage[m_iImageID]->m_VolPoint.y);
	
	m_cdc->SelectObject(oldPen);
	delete myPen;

	m_cImage[m_iImageID]->m_PreVolPnt.SetPoint(m_cImage[m_iImageID]->m_VolPoint.x, m_cImage[m_iImageID]->m_VolPoint.y);
	m_cImage[m_iImageID]->m_bVolPnt ^= 1;
}
void CMixerCtrlDlg::DrawActivePnt(CPoint point, int player_id)
{
	CPen *myPen, *oldPen;
		
	myPen = new CPen(PS_SOLID|PS_ENDCAP_ROUND, 20, RGB(255, 0, 255));
	oldPen = m_cdc->SelectObject(myPen);

	// for remove previous pnt {
	if (m_PreActPnt != CPoint(0, 0))
	{
		m_cdc->MoveTo(m_PreActPnt.x, m_PreActPnt.y);
		m_cdc->LineTo(m_PreActPnt.x, m_PreActPnt.y);

		// for remove previous zoom icon if painted while change active window in mixers
		if (m_cImage[m_iImageID]->m_bZmPaintBit) //m_iImageID now still remain previous active window (old) index
		{
		    m_cImage[m_iImageID]->m_ZoomLTpnt = m_cImage[m_iImageID]->m_PreZmRectPos.TopLeft();
			m_cImage[m_iImageID]->m_ZoomRBpnt = m_cImage[m_iImageID]->m_PreZmRectPos.BottomRight();
			DrawZoomRect();
		}

		// for remove previous vol pnt icon if painted while change active window in mixers
		if (m_cImage[m_iImageID]->m_bVolPnt) //m_iImageID now still remain previous active window (old) index
		{
			m_cImage[m_iImageID]->m_VolPoint.SetPoint(m_cImage[m_iImageID]->m_PreVolPnt.x, m_cImage[m_iImageID]->m_PreVolPnt.y);
			DrawVolPnt();
		}
	}
	m_PreActPnt = point;
	// for remove previous pnt }
		
	m_cdc->MoveTo(point.x, point.y);
	m_cdc->LineTo(point.x, point.y);
		
	m_cdc->SelectObject(oldPen);
	delete myPen;

	HDC dc = ::GetDC(NULL);
	COLORREF color = GetPixel(dc, point.x, point.y);
	BYTE R = GetRValue(color);
	BYTE G = GetGValue(color);
	BYTE B = GetBValue(color);
	::ReleaseDC(NULL, dc);

	if (R==B && R!=G)
		m_iImageID = player_id;
	else
		m_iImageID = LAYER1;

	m_bDrawActIcon = 1;

	if (m_cImage[m_iImageID]->StreamExist())
	{
		m_sWidth.Format("%d", m_cImage[m_iImageID]->MixerStream().GetVideoWidth());
		m_sHeight.Format("%d", m_cImage[m_iImageID]->MixerStream().GetVideoHeight());

		m_SysSndSldCtrl.EnableWindow(TRUE);
		m_sSysSound.Format("%d", m_cImage[m_iImageID]->MixerStream().GetSystemVolume());
		m_SysSndSldCtrl.SetPos(SYS_SOUND_VAL(m_cImage[m_iImageID]->MixerStream().GetSystemVolume()));

		InitVolPntPos();
		DrawVolPnt();

		//set RGB .etc pages button value and position
		((CMoveable_Item*)m_FramePic->GetItemByID(COLOR1_SLIDER))->EnableWindow(TRUE);
		((CMoveable_Item*)m_FramePic->GetItemByID(COLOR2_SLIDER))->EnableWindow(TRUE);
		 if (m_uActivePage == RGB) 
		 {
		 	((CMoveable_Item*)m_FramePic->GetItemByID(COLOR3_SLIDER))->EnableWindow(TRUE);
		 }

		 m_EffectCombox.EnableWindow(TRUE);
		 m_PanelCombox.EnableWindow(TRUE);
		 //SetFocus();
		 
		 for (int id=LEVEL_SLIDER; id<=PARAM5_SLIDER; id++)
		 	if ((CMoveable_Item*)m_FramePic->GetItemByID(id))
				((CMoveable_Item*)m_FramePic->GetItemByID(id))->EnableWindow(TRUE);
	}
	else
	{
		m_sWidth.Format("%s", "");
		m_sHeight.Format("%s", "");

		m_SysSndSldCtrl.SetPos(SYS_SOUND_VAL(0));
		m_SysSndSldCtrl.EnableWindow(FALSE);
		m_sSysSound.Format("%s", "");
		
		((CMoveable_Item*)m_FramePic->GetItemByID(COLOR1_SLIDER))->EnableWindow(FALSE);
		((CMoveable_Item*)m_FramePic->GetItemByID(COLOR2_SLIDER))->EnableWindow(FALSE);
		if (m_uActivePage == RGB) ((CMoveable_Item*)m_FramePic->GetItemByID(COLOR3_SLIDER))->EnableWindow(FALSE);

		m_EffectCombox.EnableWindow(FALSE);
		m_PanelCombox.EnableWindow(FALSE);
		SetFocus();

		for (int id=LEVEL_SLIDER; id<=PARAM5_SLIDER; id++)
		   if ((CMoveable_Item*)m_FramePic->GetItemByID(id))
			   ((CMoveable_Item*)m_FramePic->GetItemByID(id))->EnableWindow(FALSE);
	}
	RefreshRgbCtrl();
	
	UpdateData(FALSE);
	
}

void CMixerCtrlDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (nIDEvent == 0 && g_MixCtrlAry.size() > 0)
	{
		INT32 iIndex = INT(GetTickCount() / 100) % 30;
		VMR9NormalizedRect rTemp;
		rTemp.top = rTemp.left = 0;
		rTemp.bottom = rTemp.right = float(iIndex) / 30;
		g_MixCtrlAry.at(0)->SetOutputRect(0, &rTemp);
		g_MixCtrlAry.at(0)->SetAlpha(0, float(iIndex) / 30);
	}
	else if (nIDEvent == 1 && IsWindowVisible())
	{
		if (m_cImage[m_iImageID] && m_cImage[m_iImageID]->StreamExist())
			m_cImage[m_iImageID]->MixerStream().SetEffectIndex(m_EffectCombox.GetCurSel());

		if (PLAY_THREAD_NUM > g_MediaStreamAry.size())
		{
			for (int i=g_MediaStreamAry.size(); i<PLAY_THREAD_NUM; i++)
			{
				if (((CMoveable_Item*)m_FramePic->GetItemByID(IMAGE1_SLIDER+i))->GetRelativePos().x != 0)
				{
					CPoint tmpPnt[PLAY_THREAD_NUM] = {{IMAGE1_SLIDER_X_BEGIN, IMAGE_SLIDER_Y},
													  {IMAGE2_SLIDER_X_BEGIN, IMAGE_SLIDER_Y},
													  {MIXER_SLIDER_X_BEGIN,  IMAGE_SLIDER_Y}};
				
					((CMoveable_Item*)m_FramePic->GetItemByID(IMAGE1_SLIDER+i))->SetToPos(tmpPnt[i], TRUE);
					((CMoveable_Item*)m_FramePic->GetItemByID(IMAGE1_SLIDER+i))->RedrawBGImage(true);

					switch (i)
					{
						case LAYER1:
							m_sImg1Time.Format("%s", "");
							break;
						case LAYER2:						
							m_sImg2Time.Format("%s", "");
							break;		
						case MIXER:						
							m_sMixerTime.Format("%s", "");
							break;						
					}
					UpdateData(FALSE);
				}
			}
		}
	
		for (int x = 0; x < PLAY_THREAD_NUM && x < g_MediaStreamAry.size(); x++)
		{
			if (g_MediaStreamAry.at(x)->IsOpened() && !g_MediaStreamAry.at(x)->SetDisplayManager(m_cImage[x]->GetDispManager(), 100 + x)) //Mixer Using 100 Index
			{
				g_MediaStreamAry.at(x)->CreateDispMixer(m_cImage[x]->GetD3DDevice(), m_cImage[x]->GetDesktopD3DFormat(), 1920, 1080);
				m_cImage[x]->InitMixerStream(g_MediaStreamAry.at(x));
				m_cImage[x]->m_bPlaying = TRUE;
			}

			if (1)
			{
				IDirect3DSurface9* pCurSurface = g_MediaStreamAry.at(x)->GetSurface(100 + x);
				INT32 iWidth = g_MediaStreamAry.at(x)->GetVideoWidth();
				INT32 iHeight = g_MediaStreamAry.at(x)->GetVideoHeight();
				m_cImage[x]->RenderSurface(pCurSurface, iWidth, iHeight);
			}
			else
			{
				unsigned char *pSrc = g_MediaStreamAry.at(x)->GetVideoBuffer();
				INT32 iWidth;
				INT32 iHeight;
				if (pSrc)
				{
					iWidth = g_MediaStreamAry.at(x)->GetVideoWidth();
					iHeight = g_MediaStreamAry.at(x)->GetVideoHeight();

					m_cImage[x]->InitMixerStream(g_MediaStreamAry.at(x));
					m_cImage[x]->RenderFrame(pSrc, iWidth, iHeight);
				}
			}
			/*
			if (!g_MediaStreamAry.at(x)->HasRender())
			{
				//m_cImage[x]->InitMixerStream(NULL);

				//initial zoom position info
				CRect NoZoomedSym(m_ZoomOriLTpnt, m_ZoomOriRBpnt), ZoomedSym(m_cImage[x]->m_ZoomLTpnt, m_cImage[x]->m_ZoomRBpnt);

				m_cImage[x]->m_iDecImgWidth = g_MediaStreamAry.at(x)->GetVideoWidth();
				m_cImage[x]->m_iDecImgHeight= g_MediaStreamAry.at(x)->GetVideoHeight();

				//initial D3D render
				g_MediaStreamAry.at(x)->InitRender(GetDlgItem(IDC_STATIC_BMP1_ID + x)->GetSafeHwnd(), m_cImage[x]);

				//get image render site at Mixer Ctrl Window
				m_cImage[x]->CalImageSiteLT(NoZoomedSym, ZoomedSym); //must put after InitRender, only at that time to get m_icur_backbuf_width, m_icur_backbuf_height

				//get stream info
				m_cImage[x]->InitMixerStream(g_MediaStreamAry.at(x));
				m_cImage[x]->MixerStream().EnableAudio(true);

				m_cImage[x]->m_bPlaying = TRUE;
			}*/
		}
	}


	
	//resfresh display duration time while playing if necessary
	if (m_bShow && m_cImage[m_iImageID]->m_bPlaying)
	{
		static unsigned char cnt = 0;
		
		if ((++cnt)%10 == 0) //up to 1 sec interval
		{
			if (m_cImage[m_iImageID]->m_bZmPaintBit)
			{
				CRect NoZoomedSym(m_ZoomOriLTpnt, m_ZoomOriRBpnt), ZoomedSym(m_cImage[m_iImageID]->m_ZoomLTpnt, m_cImage[m_iImageID]->m_ZoomRBpnt);
			
				m_cImage[m_iImageID]->CalImageSiteLT(NoZoomedSym, ZoomedSym);
				/*
				CPoint imgdst_LT, imgsrc_LT;
				int src_w, src_h, dst_w, dst_h;
			
				imgdst_LT = m_cImage[m_iImageID]->ImgDst();
				imgsrc_LT = m_cImage[m_iImageID]->ImgSrc();
				src_w = m_cImage[m_iImageID]->SrcImgWidth();
				src_h = m_cImage[m_iImageID]->SrcImgHeight();
				dst_w = m_cImage[m_iImageID]->DstImgWidth();
				dst_h = m_cImage[m_iImageID]->DstImgHeight();
				*/
			}
			//StretchBlt(::GetDC(NULL), m_cImage[m_iImageID]->ImgDst().x, m_cImage[m_iImageID]->ImgDst().y, m_cImage[m_iImageID]->DstImgWidth(), m_cImage[m_iImageID]->DstImgHeight(),
			//	       ::GetDC(NULL), m_cImage[m_iImageID]->ImgSrc().x, m_cImage[m_iImageID]->ImgSrc().y, m_cImage[m_iImageID]->SrcImgWidth(), m_cImage[m_iImageID]->SrcImgHeight(), SRCCOPY);


			//refresh system sound slider pos
			if (SYS_SOUND_VAL(m_SysSndSldCtrl.GetPos()) != m_cImage[m_iImageID]->MixerStream().GetSystemVolume())
			{
				m_SysSndSldCtrl.SetPos(SYS_SOUND_VAL(m_cImage[m_iImageID]->MixerStream().GetSystemVolume()));
				m_sSysSound.Format("%d", m_cImage[m_iImageID]->MixerStream().GetSystemVolume());
				UpdateData(FALSE);
			}
			

			//reset count
			cnt = 0;
		}
	}



	if (!m_bDrawActIcon)
	{
		CRect Layer1Wnd(IWSCALE(8), IHSCALE(8), IWSCALE(637), IHSCALE(515)),
			  Layer2Wnd(IWSCALE(641), IHSCALE(8), IWSCALE(1270), IHSCALE(515)),
			  MixerWnd(IWSCALE(1275), IHSCALE(8), IWSCALE(1912), IHSCALE(515));

		switch (m_iImageID)
		{
			case LAYER1:
				DrawActivePnt(CPoint(Layer1Wnd.BottomRight().x-10, Layer1Wnd.BottomRight().y-10), LAYER1);
				break;
			case LAYER2:
				DrawActivePnt(CPoint(Layer2Wnd.BottomRight().x-10, Layer1Wnd.BottomRight().y-10), LAYER2);
				break;
			case MIXER:
				DrawActivePnt(CPoint(MixerWnd.BottomRight().x-10,	MixerWnd.BottomRight().y-10), MIXER);
				break;
		}
	}

	//update Image1, Image2, Mixer slider position if necessary
	if (m_bShow)
	{
		static unsigned char cnt2 = 0;
		
		if ((++cnt2)%2 == 0) //up to 1/4 sec interval
		{
			int startPos[PLAY_THREAD_NUM] = {IMAGE1_SLIDER_X_BEGIN, IMAGE2_SLIDER_X_BEGIN, MIXER_SLIDER_X_BEGIN};
			
			for (int i=0; i<PLAY_THREAD_NUM; i++)
				if (m_cImage[i]->m_bPlaying)
				{
					 CPoint tmpPnt = {startPos[i] + int(float(IMAGE_SLIDER_LEN*m_cImage[i]->MixerStream().GetCurPos())/m_cImage[i]->MixerStream().GetTotalPos()), IMAGE_SLIDER_Y};
					 
					 ((CMoveable_Item*)m_FramePic->GetItemByID(IMAGE1_SLIDER+i))->SetToPos(tmpPnt, TRUE);
					 ((CMoveable_Item*)m_FramePic->GetItemByID(IMAGE1_SLIDER+i))->RedrawBGImage(true);

					 switch (i)
					 {
					 	case LAYER1:
							 m_sImg1Time.Format("%s/%s", m_cImage[LAYER1]->MixerStream().CurPos().GetBuffer(), m_cImage[LAYER1]->MixerStream().TotalLen().GetBuffer());
							 break;
					 	case LAYER2:
							 m_sImg2Time.Format("%s/%s", m_cImage[LAYER2]->MixerStream().CurPos().GetBuffer(), m_cImage[LAYER2]->MixerStream().TotalLen().GetBuffer());
							 break;
						case MIXER:
							 m_sMixerTime.Format("%s/%s", m_cImage[MIXER]->MixerStream().CurPos().GetBuffer(), m_cImage[MIXER]->MixerStream().TotalLen().GetBuffer());
							 break;
					 }
					 
					 UpdateData(FALSE);
				}
			//cnt2 = 0;
		}
	}

	CDialog::OnTimer(nIDEvent);
}

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

BOOL CMixerCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//get wide and high zoom rate into m_fWscaling and m_fHscaling
	CRect rectTemp;
	rectTemp = g_DispArys.GetAt(0);
	INT32 iWidth = rectTemp.Width();
	INT32 iHeight = rectTemp.Height();

	m_fWscaling = float(iWidth) / 1920;
	m_fHscaling = float(iHeight) / 1080;

	//start add icons on MixerCtrlDlg
	CRgn NullRgn;
	NullRgn.CreateRectRgn(0, 0, 0, 0);

	{

		m_FramePic = new CMenu_Class(this, m_hWnd, 1, theApp.m_strCurPath + "\\UI Folder\\Mixer\\BG1.bmp");
		
		CItem_Class *pItem;

		pItem = new CItem_Class(this, m_hWnd, UP_BUTTON, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Physics-up.bmp", true, true, true);
		pItem->OffsetObject(CPoint(735, 564));
		m_FramePic->AddItem(pItem);

		pItem = new CItem_Class(this, m_hWnd, DOWN_BUTTON, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Physics-down.bmp", true, true, true);
		pItem->OffsetObject(CPoint(735, 650));
		m_FramePic->AddItem(pItem);

		pItem = new CItem_Class(this, m_hWnd, LEFT_BUTTON, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Physics-left.bmp", true, true, true);
		pItem->OffsetObject(CPoint(697, 606));
		m_FramePic->AddItem(pItem);

		pItem = new CItem_Class(this, m_hWnd, RIGHT_BUTTON, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Physics-right.bmp", true, true, true);
		pItem->OffsetObject(CPoint(775, 606));
		m_FramePic->AddItem(pItem);

		CMoveable_Item *pMoveItem;

		//pMoveItem = new CMoveable_Item(this, m_hWnd, IMAGE1_SLIDER, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Layer.bmp", true, true, true);
		//pMoveItem->SetItemMoveLimit(CPoint(IMAGE1_SLIDER_X_BEGIN, IMAGE_SLIDER_Y), CPoint(IMAGE1_SLIDER_X_END, IMAGE_SLIDER_Y));
		pMoveItem = new CMoveable_Item(this, m_hWnd, IMAGE1_SLIDER, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Layer slider.bmp",
															 		theApp.m_strCurPath + "\\UI Folder\\Mixer\\Layer.bmp",
															 		CPoint(IMAGE1_SLIDER_X_BEGIN, IMAGE_SLIDER_Y), CPoint(IMAGE1_SLIDER_X_END, IMAGE_SLIDER_Y));
		pMoveItem->SetParentMenu(m_FramePic);
		pMoveItem->OffsetObject(CPoint(IMAGE1_SLIDER_X_BEGIN, IMAGE_SLIDER_Y));
		m_FramePic->AddItem(pMoveItem);

		//pMoveItem = new CMoveable_Item(this, m_hWnd, IMAGE2_SLIDER, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Layer.bmp", true, true, true);
		//pMoveItem->SetItemMoveLimit(CPoint(IMAGE2_SLIDER_X_BEGIN, IMAGE_SLIDER_Y), CPoint(IMAGE2_SLIDER_X_END, IMAGE_SLIDER_Y));
		pMoveItem = new CMoveable_Item(this, m_hWnd, IMAGE2_SLIDER, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Layer slider.bmp",
															 		theApp.m_strCurPath + "\\UI Folder\\Mixer\\Layer.bmp",
															 		CPoint(IMAGE2_SLIDER_X_BEGIN, IMAGE_SLIDER_Y), CPoint(IMAGE2_SLIDER_X_END, IMAGE_SLIDER_Y));
		pMoveItem->SetParentMenu(m_FramePic);
		pMoveItem->OffsetObject(CPoint(IMAGE2_SLIDER_X_BEGIN, IMAGE_SLIDER_Y));
		m_FramePic->AddItem(pMoveItem);

		//pMoveItem = new CMoveable_Item(this, m_hWnd, MIXER_SLIDER, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Layer.bmp", true, true, true);
		//pMoveItem->SetItemMoveLimit(CPoint(MIXER_SLIDER_X_BEGIN, IMAGE_SLIDER_Y), CPoint(MIXER_SLIDER_X_END, IMAGE_SLIDER_Y));
		pMoveItem = new CMoveable_Item(this, m_hWnd, MIXER_SLIDER, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Layer slider.bmp",
															 		theApp.m_strCurPath + "\\UI Folder\\Mixer\\Layer.bmp",
															 		CPoint(MIXER_SLIDER_X_BEGIN, IMAGE_SLIDER_Y), CPoint(MIXER_SLIDER_X_END, IMAGE_SLIDER_Y));
		pMoveItem->SetParentMenu(m_FramePic);
		pMoveItem->OffsetObject(CPoint(MIXER_SLIDER_X_BEGIN, IMAGE_SLIDER_Y));
		m_FramePic->AddItem(pMoveItem);

		AddItem(COLOR1_SLIDER);
		AddItem(COLOR2_SLIDER);

		pItem = new CItem_Class(this, m_hWnd, BRIGHT_PAGE, theApp.m_strCurPath + "UI Folder\\Mixer\\Color Space Btn1.bmp", true, true, true, 1);
		pItem->OffsetObject(CPoint(4, 898));
		m_FramePic->AddItem(pItem);

		pItem = new CItem_Class(this, m_hWnd, HUE_PAGE, theApp.m_strCurPath + "UI Folder\\Mixer\\Color Space Btn1.bmp", true, true, true, 1);
		pItem->OffsetObject(CPoint(216, 898));
		m_FramePic->AddItem(pItem);

		pItem = new CItem_Class(this, m_hWnd, RGB_PAGE, theApp.m_strCurPath + "UI Folder\\Mixer\\Color Space Btn1.bmp", true, true, true, 1);
		pItem->OffsetObject(CPoint(428, 898));
		m_FramePic->AddItem(pItem);

#define GAP 35

		pItem = new CItem_Class(this, m_hWnd, REC_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Media-01.bmp", true, true, true);
		pItem->OffsetObject(CPoint(368, 993));
		m_FramePic->AddItem(pItem);

		pItem = new CItem_Class(this, m_hWnd, STOP_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Media-02.bmp", true, true, true);
		pItem->OffsetObject(CPoint(475 + GAP, 993));
		m_FramePic->AddItem(pItem);

		pItem = new CItem_Class(this, m_hWnd, PAUSE_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Media-03.bmp", true, true, true);
		pItem->OffsetObject(CPoint(582 + 2*GAP, 993));
		m_FramePic->AddItem(pItem);

		pItem = new CItem_Class(this, m_hWnd, PLAY_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Media-04.bmp", true, true, true);
		pItem->OffsetObject(CPoint(689 + 3*GAP, 993));
		m_FramePic->AddItem(pItem);

		pItem = new CItem_Class(this, m_hWnd, TO_START_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Media-05.bmp", true, true, true);
		pItem->OffsetObject(CPoint(796 + 4*GAP, 993));
		m_FramePic->AddItem(pItem);

		pItem = new CItem_Class(this, m_hWnd, TO_END_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Media-06.bmp", true, true, true);
		pItem->OffsetObject(CPoint(903 + 5*GAP, 993));
		m_FramePic->AddItem(pItem);

		pItem = new CItem_Class(this, m_hWnd, BACKWARD_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Media-07.bmp", true, true, true);
		pItem->OffsetObject(CPoint(1010 + 6*GAP, 993));
		m_FramePic->AddItem(pItem);

		pItem = new CItem_Class(this, m_hWnd, FORWARD_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Media-08.bmp", true, true, true);
		pItem->OffsetObject(CPoint(1117 + 7*GAP, 993));
		m_FramePic->AddItem(pItem);

		pItem = new CItem_Class(this, m_hWnd, BACK_PLAY_ITEM, theApp.m_strCurPath + "\\UI Folder\\Mixer\\Media-09.bmp", true, true, true);
		pItem->OffsetObject(CPoint(1224 + 8*GAP, 993));
		m_FramePic->AddItem(pItem);
	}

	m_FramePic->Initialize();

	if (iWidth != 1920 || iHeight != 1080)
		m_FramePic->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);

	m_FramePic->SetWindowRgn(NullRgn, true);

	m_bTimerEnable = FALSE;

	//m_iAngle = 0;
	m_fRadius = 40.f;

	m_cdc = new CClientDC(this);
#if 0
	int xpos = 1653;            // Horizontal position of the window.
	int ypos = 594;            // Vertical position of the window.
	int nwidth = 100;          // Width of the window
	int nheight = 200;         // Height of the window
	HWND hwndParent = m_hWnd; // Handle to the parent window


	HWND hWndComboBox = CreateWindow(WC_COMBOBOX, TEXT(""),
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		xpos, ypos, nwidth, nheight, hwndParent, NULL, HINST_THISCOMPONENT,
		NULL);

	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Wood Noise");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Halftone");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Fish Eye");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Ripple");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Glass Windows");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Color Modify");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Neon Light Texture");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Blind Texture");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Swirl Texture");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Cross Hatching");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Rotate Scale");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Pixelate");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Multi Movie");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Freeze Movie");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Rectangle Bright Dark");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Sleazy Emboss");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Brightness Flicker");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"RGB Color");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Gray");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"Negative");
	::SendMessage(hWndComboBox, CB_INSERTSTRING, 0, (LPARAM)"No Effect");
	::SendMessage(hWndComboBox, CB_SETCURSEL, 0, 0);
#endif

	//Add system sound slider control
	m_SysSndSldCtrl.Create(TBS_VERT | WS_CHILD | WS_VISIBLE | TBS_BOTTOM, CRect(IWSCALE(1875), IHSCALE(800), IWSCALE(1900), IHSCALE(900)), this, IDC_SLIDER);
	m_SysSndSldCtrl.SetRange(0, 100);
    m_SysSndSldCtrl.SetPos(SYS_SOUND_VAL(0));
	m_SysSoundStatic.Create("", WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(IWSCALE(1875), IHSCALE(905), IWSCALE(1900), IHSCALE(921)), this, IDC_STATIC_SYSSND_ID);

	//Add display time for Image1, Image2, Mixer respectively
	m_Img1TimeStatic.Create("", WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(IMAGE1_SLIDER_X_BEGIN-IWSCALE(90), IMAGE_SLIDER_Y+IHSCALE(7), IMAGE1_SLIDER_X_BEGIN-IWSCALE(10), IMAGE_SLIDER_Y+IHSCALE(23)), this, IDC_STATIC_IMG1TIME_ID);
	m_Img2TimeStatic.Create("", WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(IMAGE2_SLIDER_X_BEGIN-IWSCALE(90), IMAGE_SLIDER_Y+IHSCALE(7), IMAGE2_SLIDER_X_BEGIN-IWSCALE(10), IMAGE_SLIDER_Y+IHSCALE(23)), this, IDC_STATIC_IMG2TIME_ID);
	m_MixerTimeStatic.Create("", WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(MIXER_SLIDER_X_BEGIN-IWSCALE(90), IMAGE_SLIDER_Y+IHSCALE(7), MIXER_SLIDER_X_BEGIN-IWSCALE(10), IMAGE_SLIDER_Y+IHSCALE(23)), this, IDC_STATIC_MIXERTIME_ID);

	//Add panel selection
	m_PanelCombox.Create(WS_CHILD | CBS_DROPDOWNLIST | CBS_SORT | CBS_HASSTRINGS | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,
						  CRect(IWSCALE(1653), IHSCALE(894), IWSCALE(1753), IHSCALE(1094)), this, IDC_PANEL_COMBO_ID);

	char cbuf[5];

	for (int i=0; i<MAX_PANNEL; i++)
	{
		m_PanelCombox.InsertString(0, itoa(i+1, cbuf, 10));
	}

	m_PanelCombox.SetCurSel(0);

	//Add effect selection

	//original position
	//m_EffectCombox.Create(WS_CHILD | CBS_DROPDOWNLIST | CBS_SORT | CBS_HASSTRINGS | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,
						  //CRect(IWSCALE(1653), IHSCALE(594), IWSCALE(1753), IHSCALE(794)), this, IDC_COMBO_ID);


	m_EffectCombox.Create(WS_CHILD | CBS_DROPDOWNLIST | CBS_SORT | CBS_HASSTRINGS | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE,
						  CRect(IWSCALE(1598), IHSCALE(594), IWSCALE(1765), IHSCALE(794)), this, IDC_COMBO_ID);

	

	m_EffectCombox.InsertString(0, "Mirage");
	m_EffectCombox.InsertString(0, "Blur");
	m_EffectCombox.InsertString(0, "Single Color");
	m_EffectCombox.InsertString(0, "Dot Mosaics");
	m_EffectCombox.InsertString(0, "Plane Rotate");
	m_EffectCombox.InsertString(0, "Cylinder Rotate");
	m_EffectCombox.InsertString(0, "Circle Center");
	m_EffectCombox.InsertString(0, "Kaleidoscope");
	m_EffectCombox.InsertString(0, "WaterColor");
	m_EffectCombox.InsertString(0, "Wave");
	m_EffectCombox.InsertString(0, "Edge");
	m_EffectCombox.InsertString(0, "Wood Noise");
	m_EffectCombox.InsertString(0, "Halftone");
	m_EffectCombox.InsertString(0, "Fish Eye");
	m_EffectCombox.InsertString(0, "Ripple");
	m_EffectCombox.InsertString(0, "Glass Windows");
	m_EffectCombox.InsertString(0, "Color Modify");
	m_EffectCombox.InsertString(0, "Neon Light Texture");
	m_EffectCombox.InsertString(0, "Blind Texture");
	m_EffectCombox.InsertString(0, "Swirl Texture");
	m_EffectCombox.InsertString(0, "Cross Hatching");
	m_EffectCombox.InsertString(0, "Rotate Scale");
	m_EffectCombox.InsertString(0, "Pixelate");
	m_EffectCombox.InsertString(0, "Multi Movie");
	m_EffectCombox.InsertString(0, "Freeze Movie");
	m_EffectCombox.InsertString(0, "Rectangle Bright Dark");
	m_EffectCombox.InsertString(0, "Sleazy Emboss");
	m_EffectCombox.InsertString(0, "Brightness Flicker");
	m_EffectCombox.InsertString(0, "RGB Color");
	m_EffectCombox.InsertString(0, "Gray");
	m_EffectCombox.InsertString(0, "Negative");
	m_EffectCombox.InsertString(0, "No Effect");
	m_EffectCombox.SetCurSel(0);

	if (g_PannelSetting.iTVWallMode)
	{
		m_ParmNameAry.Add({"Left","Top","Right","GapX", "GapY"});
	}
	else if (m_iCurLang == ENGLISH_TYE)
	{
		m_ParmNameAry.Add({ "Mirage", "", "","","","","" });
		m_ParmNameAry.Add({ "Blur", "Level", "","","","","" });
		m_ParmNameAry.Add({ "Single Color", "Level", "Contrast","","","","" });
		m_ParmNameAry.Add({ "Dot Mosaics", "Level", "Red Shift","Green Shift","Blue Shift","","" });
		m_ParmNameAry.Add({ "Plane Rotate", "Level", "Frequency","Amplitude","","","" });
		m_ParmNameAry.Add({ "Cylinder Rotate", "Level", "Contast","","","","" });
		m_ParmNameAry.Add({ "Circle Center", "Level", "Bright Shift","Left","Right","Top", "Down" });
		m_ParmNameAry.Add({ "Kaleidoscope", "Freeze", "","","","" });
		m_ParmNameAry.Add({ "WaterColor", "Multi", "H Shift","V Shift","Mirror","" });
		m_ParmNameAry.Add({ "Wave", "Pixel Size", "","","","" });
		m_ParmNameAry.Add({ "Edge", "Angle", "Width Zoom","Heigth Zoom","H Shfit","V Shift" });
		m_ParmNameAry.Add({ "Wood Noise", "Level", "Density","","","" });
		m_ParmNameAry.Add({ "Halftone", "Amount", "Radius","","","" });
		m_ParmNameAry.Add({ "Fish Eye", "Level", "Density","Contrast","","" });
		m_ParmNameAry.Add({ "Ripple", "Level", "Light Size","H Move","V Move","" });
		m_ParmNameAry.Add({ "Glass Windows", "Level", "Brightness","Hue","Saturation","" });
		m_ParmNameAry.Add({ "Color Modify", "Level", "Pattern","","","" });
		m_ParmNameAry.Add({ "Neon Light Texture", "Peak", "Speed","Wavelength","H Shift","V Shift" });
		m_ParmNameAry.Add({ "Blind Texture", "Curvature", "Layer","","","" });
		m_ParmNameAry.Add({ "Swirl Texture", "Density", "Brightness","","","" });
		m_ParmNameAry.Add({ "Cross Hatching", "R", "G","B","H Num","V Num" });
		m_ParmNameAry.Add({ "Rotate Scale", "Edge Width", "","","","" });
		m_ParmNameAry.Add({ "Pixelate", "Wave Number", "Frequency","Twist","", "" });
		m_ParmNameAry.Add({ "Multi Movie", "Level", "Saturation","","","" });
		m_ParmNameAry.Add({ "Freeze Movie", "Level", "Speed","","","" });
		m_ParmNameAry.Add({ "Rectangle Bright Dark", "Number", "Angle1","Rotate1","Angle2","Rotate2" });
		m_ParmNameAry.Add({ "Sleazy Emboss", "Number", "Rotate","Shift","","" });
		m_ParmNameAry.Add({ "Brightness Flicker", "Plane Num", "Rotate","X Move","Z Move","Distance" });
		m_ParmNameAry.Add({ "RGB Color", "Dot Size", "Brigtness","Soft","","" });
		m_ParmNameAry.Add({ "Gray", "Color Range", "Red", "Green", "Blue","Saturation" });
		m_ParmNameAry.Add({ "Negative", "Frame Num", "", "", "","" });
		m_ParmNameAry.Add({ "No Effect", "Frequency", "Angle", "Range", "Amplitude","Y Offset" });
	}

	m_ZoomXspin.Create(UDS_AUTOBUDDY | UDS_SETBUDDYINT | UDS_ARROWKEYS | WS_CHILD | WS_VISIBLE, CRect(IWSCALE(721), IHSCALE(846), IWSCALE(741), IHSCALE(866)), this, IDC_XSPIN_ID);
	m_ZoomXedit.Create(WS_CHILD | WS_VISIBLE, CRect(IWSCALE(695), IHSCALE(846), IWSCALE(721), IHSCALE(866)), this, IDC_XEDIT_ID);
	m_ZoomXspin.SetBuddy(&m_ZoomXedit);
	m_ZoomYspin.Create(UDS_AUTOBUDDY | UDS_SETBUDDYINT | UDS_ARROWKEYS | WS_CHILD | WS_VISIBLE, CRect(IWSCALE(817), IHSCALE(846), IWSCALE(837), IHSCALE(866)), this, IDC_YSPIN_ID);
	m_ZoomYedit.Create(WS_CHILD | WS_VISIBLE, CRect(IWSCALE(791), IHSCALE(846), IWSCALE(817), IHSCALE(866)), this, IDC_YEDIT_ID);
	m_ZoomYspin.SetBuddy(&m_ZoomYedit);

	//m_FileNameEdit.Create(WS_CHILD, CRect(0,0,100,100), this, IDC_FILENAME);

	
	m_ParamStatic[0].Create ("50", WS_CHILD | SS_CENTER, CRect(IWSCALE(1450), IHSCALE(599), IWSCALE(1475), IHSCALE(615)), this, IDC_STATIC_LEVEL_ID);
	m_ParamStatic[1].Create("50", WS_CHILD | SS_CENTER, CRect(IWSCALE(1450), IHSCALE(645), IWSCALE(1475), IHSCALE(661)), this, IDC_STATIC_PARAM1_ID);
	m_ParamStatic[2].Create("50", WS_CHILD | SS_CENTER, CRect(IWSCALE(1450), IHSCALE(704), IWSCALE(1475), IHSCALE(720)), this, IDC_STATIC_PARAM2_ID);
	m_ParamStatic[3].Create("50", WS_CHILD | SS_CENTER, CRect(IWSCALE(1450), IHSCALE(763), IWSCALE(1475), IHSCALE(779)), this, IDC_STATIC_PARAM3_ID);
	m_ParamStatic[4].Create("50", WS_CHILD | SS_CENTER, CRect(IWSCALE(1450), IHSCALE(824), IWSCALE(1475), IHSCALE(840)), this, IDC_STATIC_PARAM4_ID);
	m_ParamStatic[5].Create("50", WS_CHILD | SS_CENTER, CRect(IWSCALE(1450), IHSCALE(883), IWSCALE(1475), IHSCALE(899)), this, IDC_STATIC_PARAM5_ID);
	

	m_WidthStatic.Create("", WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(IWSCALE(725), IHSCALE(900), IWSCALE(760), IHSCALE(916)), this, IDC_STATIC_WIDTH_ID);
	m_HeightStatic.Create("", WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(IWSCALE(825), IHSCALE(900), IWSCALE(860), IHSCALE(916)), this, IDC_STATIC_HEIGHT_ID);

	//CImageArea creation
	for (int id=0; id<PLAY_THREAD_NUM; id++)
	{
		CRect area;

		switch (id)
		{
			case LAYER1:
				area = {IWSCALE(8), IHSCALE(71), IWSCALE(637), IHSCALE(421)};
				break;
			case LAYER2:
				area = {IWSCALE(641), IHSCALE(71), IWSCALE(1270), IHSCALE(421)};
				break;
			case MIXER:
				area = {IWSCALE(1275), IHSCALE(71), IWSCALE(1912), IHSCALE(421)};
				break;
		}
		
		if (!m_cImage[id])
			m_cImage[id] = new CImageArea(area, this, IDC_STATIC_BMP1_ID + id);

		m_cImage[id]->m_fZoomRate = 1.0;
	}
	
	//after init ok, DDX data change activate correctly
	m_bInitOK = TRUE;
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}


void CMixerCtrlDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此加入您的訊息處理常式程式碼
	if (m_FramePic)
		delete m_FramePic;

	if (m_bTimerEnable)
		KillTimer(0);

	if (m_cdc)
		delete m_cdc;

	for (int i = 0; i<PLAY_THREAD_NUM; i++)
		if (m_cImage[i])
		{
			delete m_cImage[i];
			m_cImage[i] = NULL;
		}
}

void CMixerCtrlDlg::OnOK()
{
	if (m_cImage[m_iImageID]->m_bZmPaintBit)
	{
		UINT uCtrlid = GetFocus()->GetDlgCtrlID();

		switch (uCtrlid)
		{
			case IDC_XEDIT_ID:
				{
					DrawZoomRect();
					
					int ori_Xval = m_cImage[m_iImageID]->m_ZoomLTpnt.x;

					if (UpdateData())//update  m_zoomLTpnt.x
						m_cImage[m_iImageID]->m_ZoomRBpnt.x += (m_cImage[m_iImageID]->m_ZoomLTpnt.x - ori_Xval);	//update m_zoomRBpnt.x with the same direction offset of m_zoomLTpnt.x
					else
					{
						m_cImage[m_iImageID]->m_ZoomLTpnt.x = ori_Xval;
						UpdateData(FALSE);
					}

					DrawZoomRect();
				}
				break;
			case IDC_YEDIT_ID:
				{
					DrawZoomRect();
					
					int ori_Yval = m_cImage[m_iImageID]->m_ZoomLTpnt.y;

					if (UpdateData())//update  m_zoomLTpnt.y
						m_cImage[m_iImageID]->m_ZoomRBpnt.y += (m_cImage[m_iImageID]->m_ZoomLTpnt.y - ori_Yval);  //update m_zoomRBpnt.y with the same direction offset of m_zoomLTpnt.y
					else
					{
						m_cImage[m_iImageID]->m_ZoomLTpnt.y = ori_Yval;
						UpdateData(FALSE);
					}

					DrawZoomRect();
				}
				break;
		}
	}
	else
		UpdateData(FALSE);
	
	//CDialog::OnOK();
}

/*ref code
{
HDC hDC = ::GetDC(m_hWnd);
BitBlt(hDC,0,0,800,800,hDC,1000,0,SRCCOPY);
Ellipse(hDC, 1000 - 50, 500 - 50, 1000 + 50, 500 + 50);
}

{
CClientDC cdc(this);
CBrush brshRed(RGB(255, 0, 0));
CRect rectTemp = CRect(m_VolCenter.x, m_VolCenter.y, m_VolCenter.x + 10, m_VolCenter.y + 10);
cdc.FillRect(&rectTemp, &brshRed);
}

{
CDC::FromHandle(::GetDC(m_hWnd))->SetROP2(R2_XORPEN);

//CClientDC cdc(this);
CDC* pDC = CDC::FromHandle(::GetDC(m_hWnd));
CPen *myPen, *oldPen;

myPen = new CPen(PS_SOLID|PS_ENDCAP_ROUND, 10, RGB(255, 255, 255));
oldPen = pDC->SelectObject(myPen);

pDC->MoveTo(m_VolPoint.x, m_VolPoint.y);
pDC->LineTo(m_VolPoint.x, m_VolPoint.y);

pDC->SelectObject(oldPen);
delete myPen;
}

{
m_cdc->SetBkMode(TRANSPARENT);
m_cdc->Rectangle(150, 150, 300, 300);

CRect a(150, 150, 300, 300);
CBrush brshRed(HS_DIAGCROSS | PS_DASH, RGB(0, 255, 0));

m_cdc->Draw3dRect(5, 30, 80, 70, RGB(0, 255, 0), RGB(0, 255, 0));
m_cdc->FrameRect(&a, &brshRed);

CRect a(150, 150, 300, 300);
m_cdc->DrawEdge(&a, EDGE_ETCHED, BF_RECT);
}

//threads for layer1, layer2, mixer to play
HANDLE	m_hThreadArray[PLAY_THREAD_NUM];
//init threads Handle
ZeroMemory(m_hThreadArray, sizeof(m_hThreadArray));
{
for (int i = 0; i < PLAY_THREAD_NUM; i++)
	if (m_hThreadArray[i])
	{
		SuspendThread(m_hThreadArray[i]);
		m_uDisplayDurationArray[i] = 0;

		//reset slider UI
		
		CPoint SliderPos;
		
		switch (i)
		{
			case LAYER1:
				SliderPos={IMAGE1_SLIDER_X_BEGIN, IMAGE_SLIDER_Y};
				((CMoveable_Item*)m_FramePic->GetItemByID(IMAGE1_SLIDER))->SetToPos(SliderPos, TRUE);
				break;
			case LAYER2:
				SliderPos={IMAGE2_SLIDER_X_BEGIN, IMAGE_SLIDER_Y};
				((CMoveable_Item*)m_FramePic->GetItemByID(IMAGE2_SLIDER))->SetToPos(SliderPos, TRUE);
				break;
			case MIXER:
				SliderPos={MIXER_SLIDER_X_BEGIN, IMAGE_SLIDER_Y};
				((CMoveable_Item*)m_FramePic->GetItemByID(MIXER_SLIDER))->SetToPos(SliderPos, TRUE);
				break;
		}
	}
}
{
	for (int i = 0; i < PLAY_THREAD_NUM; i++)
		if (m_hThreadArray[i]) SuspendThread(m_hThreadArray[i]);
}
for (int i = 0; i < PLAY_THREAD_NUM; i++)
	if (m_hThreadArray[i]) ResumeThread(m_hThreadArray[i]);
{
if (R==B && R!=G)
{
	unsigned threadID;
	
	m_hThreadArray[player_id] = (HANDLE)_beginthreadex(
					  NULL, 			// default security attributes
					  0,				// use default stack size  
					  &ThreadFunc,		// thread function name
					  NULL, 			// argument to thread function 
					  CREATE_SUSPENDED, // creation flags 
					  &threadID);		// returns the thread identifier 

	if (m_hThreadArray[player_id] == 0)
	{
		DWORD	dwExitCode;
		GetExitCodeThread(m_hThreadArray[player_id], &dwExitCode);
		MessageBox("Fail to create Play threads", "Warning", MB_OK);
	}
}
else
{
	if (!m_bPlaying)
	{
		DWORD	dwExitCode;

		GetExitCodeThread(m_hThreadArray[player_id], &dwExitCode);

		if (TerminateThread(m_hThreadArray[player_id], dwExitCode) == 0)
			MessageBox("Fail to terminate Play threads", "Warning", MB_OK);

		CloseHandle(m_hThreadArray[player_id]);
		m_hThreadArray[player_id] = 0;
	}
}
}
{
if (m_bPlaying)
{
	for (int i=0; i<PLAY_THREAD_NUM;i++)
		if (m_hThreadArray[i]) m_uDisplayDurationArray[i]++;
}
}
static unsigned _stdcall ThreadFunc(void* pArguments);

unsigned CMixerCtrlDlg::ThreadFunc(void * pArguments)
{
	return 0;
}
	
*/
