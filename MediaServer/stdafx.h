// stdafx.h : �i�b�����Y�ɤ��]�t�зǪ��t�� Include �ɡA
// �άO�g�`�ϥΫo�ܤ��ܧ�
// �M�ױM�� Include �ɮ�

#ifndef STAFX_H_INCLUDED
#define STAFX_H_INCLUDED

#ifndef POINTER_64
#define POINTER_64 __ptr64
#endif // !POINTER_64

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// �q Windows ���Y�ư����`�ϥΪ�����
#endif

// �p�G�z�������u����������x�A�Эק�U�C�w�q�C
// �Ѧ� MSDN ���o���P���x�����Ȫ��̷s��T�C
#ifndef WINVER				// ���\�ϥ� Windows XP (�t) �H�᪩�����S�w�\��C
#define WINVER 0x0501		// �N���ܧ󬰰w�� Windows ��L�������A��ȡC
#endif

#ifndef _WIN32_WINNT		// ���\�ϥ� Windows XP (�t) �H�᪩�����S�w�\��C
#define _WIN32_WINNT 0x0501	// �N���ܧ󬰰w�� Windows ��L�������A��ȡC
#endif						

#ifndef _WIN32_WINDOWS		// ���\�ϥ� Windows 98 (�t) �H�᪩�����S�w�\��C
#define _WIN32_WINDOWS 0x0410 // �N���ܧ󬰰w�� Windows Me (�t) �H�᪩�����A��ȡC
#endif

#ifndef _WIN32_IE			// ���\�ϥ� IE 6.0 (�t) �H�᪩�����S�w�\��C
#define _WIN32_IE 0x0600	// �N���ܧ󬰰w�� IE ��L�������A��ȡC
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ���T�w�q������ CString �غc�禡

// ���� MFC ���ä@�Ǳ`���Υi����ĵ�i�T�����\��
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �֤߻P�зǤ���
#include <afxext.h>         // MFC �X�R�\��


#include <afxdisp.h>        // MFC Automation ���O



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC �䴩�� Internet Explorer 4 �q�α��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC �䴩�� Windows �q�α��
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <d3d9.h>
#include <afxcontrolbars.h>
#include <vector>

//#define _ENABLE_MEDIA_STREAM_EX 1
//#define _ENABLE_GPU 1
//#define _ENABLE_VIDEO_WALL 1
#define _ENABLE_SPF 1
#define _ENABLE_D3D9EX 1
#define _ENABLE_SINGLE_EFFECT 1
#define _ENABLE_CHECK_KEYPRO 1
//#define _TRAIL_VERSION 1

#ifdef _ENABLE_GPU
#define _ENABLE_HD_EFFECT 1
#endif

#ifdef _ENABLE_MEDIA_STREAM_EX
#include "MediaStreamEx.h"
extern std::vector<CMediaStreamEx*> g_MediaStreamAry;
#else
#include "MediaStream.h"
extern std::vector<CMediaStream*> g_MediaStreamAry;
#endif

#define _ENABLE_DISPLAY_MANAGER 1
#define _ENABLE_TC_NET 1

const INT32 MAX_PANNEL = 8;
typedef struct strPANNEL_SETTING
{
	float fRatio[MAX_PANNEL];
	float fOffsetX[MAX_PANNEL];
	float fOffsetY[MAX_PANNEL];
	INT32 iEffect[MAX_PANNEL];
	INT32 iSelIdx;
	INT32 iPannelNum;
	INT32 iImageEffect[MAX_PANNEL];
	INT32 iTestPattern;
	INT32 iTVNumH;
	INT32 iTVNumV;
	DWORD dStartShowTick;
	//BOOL bFullScreen;
	//BOOL bShowGround;
	//BOOL bShowAxis;
	//BOOL bShowModelLine;
	//BOOL bRunModelLine;
	UINT iModelAction;
	UINT iNotifyAction;
	BOOL bNotShowModal;
	BOOL bHaveEndSurface;
	BOOL iTVWallMode;
	BOOL bDisableUI;
	INT32 iEncodeMedia;
	CString strVideoPath;
	CString strModelPath;
	D3DSURFACE_DESC rectSurface;
} PANNEL_SETTING;

enum
{
	TVWALL_DISABLE = 0,
	TVWALL_SHARE_SURFACE,
	TVWALL_SHAPE_MODE,
	TVWALL_MULTI_THREAD,
	TVWALL_PARTITION
};

typedef enum
{
	MODEL_NOTHING = 0,
	MODEL_MOVE = 1,
	MODEL_ROTATE = 2,
	MODEL_MAP = 3,
	MODEL_ZOOM = 4,
	MODEL_ZOOM_ALL = 5,
	MODEL_ZOOM_ONE = 6,
	MODEL_MODIFY = 8,
	MODEL_MODIFY_SELECT = 9,
	MODEL_MODIFY_MOVE = 10,
	MODEL_FULL_SCREEN = 1 << 4,
	MODEL_SHOW_GROUND = 1 << 5,
	MODEL_SHOW_AXIS = 1 << 6,
	MODEL_SHOW_MODEL_LINE = 1 << 7,
	MODEL_SHOW_DISABLE = 1 << 8,
	MODEL_SHOW_END_SURFACE = 1 << 9,
	MODEL_RUN_MODEL_LINE = 1 << 10,
	MODEL_SUBITEM_1 = 1 << 11,
	MODEL_SUBITEM_2 = 1 << 12,
	MODEL_SUBITEM_3 = 1 << 13,
	MODEL_SUBITEM_4 = 1 << 14,
	MODEL_2DMAPPING = 1 << 15,
	MODEL_ROTATE_OFFSET = 1 << 16
}MODEL_ACTION;

typedef struct typParmName
{
	CString strEffectName;
	CString strParmName[6];

}PARM_NAME;

typedef enum
{
	ENGLISH_TYE,
	CHINESS_TYE
}LANGUAGE_TYPE;

void SetModelAction(UINT &uSetting, MODEL_ACTION uAction, BOOL bEnable)
{
	if (bEnable)
		uSetting |= uAction;
	else
		uSetting &= (~uAction);
}

#define ModelReverseAction(uSetting, uAction) { if (uSetting & uAction) uSetting &= (~uAction); else uSetting |= uAction;}
/*void ModelReverseAction(UINT &uSetting, MODEL_ACTION uAction)
{
	if (uSetting & uAction)
		uSetting &= (~uAction);
	else
		uSetting |= uAction;
}*/
  
enum
{
	MSG_MAINUI = WM_USER + 0x50,
	MSG_SELECT_MEDIA,
	//MSG_INSERT_EFFECT,
};

extern sEffectParam g_EffectParam;
extern PANNEL_SETTING g_PannelSetting;

extern LANGUAGE_TYPE m_iCurLang;
extern CArray<PARM_NAME, PARM_NAME> m_ParmNameAry;



#define _NEW_TIME_LINE 1
#define _MIXER2 1

#define WM_TM_PLAY  WM_USER + 101
#define WM_TM_PAUSE  WM_USER + 102
#define WM_TM_BEGIN  WM_USER + 103
#define WM_TM_RETURN_FILENAME WM_USER + 104
#define WM_TM_UPDATE_HAND WM_USER + 105 
#define WM_TM_SET_TIMEBAR_OFFSET WM_USER + 106 
#define WM_TM_SET_VERTICAL_SCROLL_SHIFT WM_USER + 107
#define WM_TM_REDRAW WM_USER + 108 
#define WM_TM_UPDATE_TIME_CODE  WM_USER + 109
#define WM_TM_OPEN_PROJECT_FILE WM_USER + 110
#define WM_TREE_UPDATE_LIST WM_USER + 111
#define WM_STOP_AVI WM_USER + 112
#define WM_UPDATE_NETWORK_SETTING WM_USER + 113
#define WM_BUTTON_CLICK WM_USER + 114
#define WM_UPDATE_UI_STATE WM_USER + 115
#define WM_TM_RESET_TIME_CODE WM_USER + 116
#define WM_TM_BACK_TL_TIEM WM_USER + 117
#define WM_TM_NEXT_TL_TIEM WM_USER + 118
#define WM_MIXER_SEEK WM_USER + 119
#define WM_MIXER_VOL_NOTIFY WM_USER + 120
#define WM_MIXER_CLR_EXIST_ZOOM WM_USER + 121
#define WM_SHAP_SET_MODAL_NUM WM_USER + 122
#define WM_DEVICENOTRESET WM_USER + 123
#define WM_TM_PLAY2  WM_USER + 124
#define WM_SET_NETWORK_INFO WM_USER + 125

#define IDC_SM_LIST_CTRL  1001
#define IDC_SM_EFFECT_LIST_CTRL  1002

//#define ENABLE_INTERSECTION_POINT 1
#define MULTI_STREAM 1

#ifdef _WIN64 
#define GCL_HBRBACKGROUND GCLP_HBRBACKGROUND 
#define GCL_HICON GCLP_HICON
#endif

#ifdef _WIN64

#ifdef _DEBUG
//#define _DISABLE_PLANE_SCENE_3D 1
#else
//#define _DISABLE_PLANE_SCENE_3D 1
#endif

//#define _DISABLE_HASP 1
#endif

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win64' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win64' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#endif


#include "gdiplus.h"
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")
