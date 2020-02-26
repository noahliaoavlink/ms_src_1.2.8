#ifndef UIObjInfo_H
#define UIObjInfo_H

//#include "..\\Include\\SCreateWin.h"

#define UIID_BASE_ID  101
#define UIID_MAX_NUM  1024
#define BMP_BASE_ID 1001
#define MAX_STATUS 20

#define UNKNOWN_BASE_ID 16691200

enum UIObjID
{//IDTableManager
   UI_OID_KEVENT_MANAGER = UIID_BASE_ID - 8,
   UI_OID_IDTABLE_MANAGER = UIID_BASE_ID - 7,
   UI_OID_HOTKEY_MANAGER = UIID_BASE_ID - 6,
   UI_OID_CURSOR = UIID_BASE_ID - 5,
   //UI_OID_TOOLTIP = UIID_BASE_ID - 4,
   UI_OID_PROCESSER = UIID_BASE_ID - 3,
   UI_OID_CTRL_MANAGER = UIID_BASE_ID - 2,
   UI_OID_RENDER = UIID_BASE_ID - 1,
   UI_OID_BUTTON = UIID_BASE_ID,
   UI_OID_SCROLLBAR = UIID_BASE_ID + UIID_MAX_NUM,
   UI_OID_WINDOW = UIID_BASE_ID + UIID_MAX_NUM * 2,
   UI_OID_SLIDER = UIID_BASE_ID + UIID_MAX_NUM * 3,
   UI_OID_PROGRESS = UIID_BASE_ID + UIID_MAX_NUM * 4,
   UI_OID_TEXT = UIID_BASE_ID + UIID_MAX_NUM * 5,
   UI_OID_SYS = UIID_BASE_ID + UIID_MAX_NUM * 6,
   UI_OID_CHECK_BOX = UIID_BASE_ID + UIID_MAX_NUM * 7,
   UI_OID_PICTURE = UIID_BASE_ID + UIID_MAX_NUM * 8,
   UI_OID_EDITBOX = UIID_BASE_ID + UIID_MAX_NUM * 9,
   UI_OID_MENU = UIID_BASE_ID + UIID_MAX_NUM * 10,
   UI_OID_MENU_ID = UIID_BASE_ID + UIID_MAX_NUM * 11,    //���y����
   UI_OID_COMBOBOX = UIID_BASE_ID + UIID_MAX_NUM * 12,
   UI_OID_LISTBOX = UIID_BASE_ID + UIID_MAX_NUM * 13,
   UI_OID_BUTTON_MENU = UIID_BASE_ID + UIID_MAX_NUM * 22,
   UI_OID_CUSTOM_UI = UIID_BASE_ID + UIID_MAX_NUM * 23,
   UI_OID_TOOLTIP = UIID_BASE_ID + UIID_MAX_NUM * 24,
   
   /*
   //for OSD
   UI_OSD_TEXT_BUTTON = UIID_BASE_ID + UIID_MAX_NUM * 14,
   UI_OSD_BUTTON = UIID_BASE_ID + UIID_MAX_NUM * 15,
   UI_OSD_TEXT = UIID_BASE_ID + UIID_MAX_NUM * 16,
   UI_OSD_CHECK_BOX = UIID_BASE_ID + UIID_MAX_NUM * 17,
   UI_OSD_EDITBOX = UIID_BASE_ID + UIID_MAX_NUM * 18,
   UI_OSD_COMBOBOX = UIID_BASE_ID + UIID_MAX_NUM * 19,
   UI_OSD_ANIMATION_CTRL = UIID_BASE_ID + UIID_MAX_NUM * 20,
   UI_OSD_TV_REMOTE_CONTROLLER = UIID_BASE_ID + UIID_MAX_NUM * 21,     //TV remote controller
   */
};

enum MiracleUIMsgID
{
	MUI_MSG_SHOW_EVT = 99,      //for test
	MUI_MSG_INIT = 100,
	MUI_MSG_DESTROY,
	MUI_MSG_REDRAW,                   //���e 
	MUI_MSG_MOUSE_MOVE,               //�ƹ�����
	MUI_MSG_LB_DWON,                  //�ƹ�����(���U)
	MUI_MSG_LB_UP,                    //�ƹ�����(��})
	MUI_MSG_RB_DWON,                  //�ƹ��k��(���U)
	MUI_MSG_RB_UP,                    //�ƹ��k��(��})
	MUI_MSG_LB_DBLCLK,                //Double Click
	MUI_MSG_REG_OBJ,
	MUI_MSG_UNREG_OBJ,
	MUI_MSG_UPDATE,
	MUI_MSG_UPDATE_EX,
	MUI_MSG_GET_UNKNOWN_ID,            //�ΨӤ��t�ʺA���ͪ������ID��
	MUI_MSG_GET_SKIN_DIR,              //���o���O�ؿ�
	MUI_MSG_SET_SKIN_DIR,              //�]�w���O�ؿ� 
	MUI_MSG_ENABLE_CTRL,               //Enable��Disable�����
	MUI_MSG_CTRL_EVENT,
	MUI_MSG_SET_POS,                   //�]�w��m(silder,scrollbar,progress,..)
	MUI_MSG_SYS_FUNCTION,              //���w�\��(�pscrollbar���W�U��..)
	MUI_MSG_REG_BUTTON,                //���U�@��button����
	MUI_MSG_IN_REGION,                 //�P�_�O�_�b�ϰ줺
	MUI_MSG_CHANGE_SKIN,               //�ܧ󭱪O�\��
	MUI_MSG_SET_CHECKED,               //�]��check�����A
	MUI_MSG_IS_CHECKED,                //�ˬd�O�_��check�����A
	MUI_MSG_RESET,                     //���]
	MUI_MSG_CHANGE_MODE,               //���ܼҦ�(�۷�O���@�����O�A���P���O�b�P�@�ؿ��U�ܴ�)
	MUI_MSG_SET_RANGE,                 //�]�w�d�� (�}�l�Ȥγ̤j��)
	MUI_MSG_SET_TEXT,                  //�]�w����󪺤�r(�ΦW��) 
	MUI_MSG_GET_MAIN_WIN,
	MUI_MSG_IS_READY,
	MUI_MSG_GET_ID,
	MUI_MSG_GET_ID_NAME,
	MUI_MSG_SET_DISPLAY_WIN,           //for dvr
	MUI_MSG_SET_CTRL_POS,
	MUI_MSG_DRAW_RECT,                 //��ø���w���ϰ�
	MUI_MSG_GET_REGION,                //���o����󪺰ϰ� 
    MUI_MSG_TIME_EVENT,
    MUI_MSG_REG_EDITBOX,
    MUI_MSG_REG_MENU,
    MUI_MSG_REG_SCROLLBAR,
    MUI_MSG_SHOWWINDOW,
	MUI_MSG_CHANGE_IMAGE,              //�ܧ�v�����
	MUI_MSG_GET_TOTAL_OF_STATUS,       //���o�����Ҧ������A
	MUI_MSG_RESET_STATUS,              //���]���A
	MUI_MSG_SET_ACTIVE,                //�N���w�������]�w��ACTIVE
	MUI_MSG_TRIGGER,
    MUI_MSG_SET_FOCUS,                 //�N���w�������]�w��FOCUS
    MUI_MSG_INSERT_STRING,             //�s�W�@���r��챱���
    MUI_MSG_ADD_STRING,                //���J�@���r��챱���
    MUI_MSG_DELETE_STRING,             //�R���@���r��챱���
    MUI_MSG_RESET_CONTENT,             //�M������󤤩Ҧ������
    MUI_MSG_PROCESS_SINGLE_SELECTION,  //�B�z��@���
    MUI_MSG_SIMULATE_KEYDOWN,          //�ҥ���L�T��
    MUI_MSG_CANCLE,                    //����
	MUI_MSG_SET_PARENT,
	MUI_MSG_SET_ITEM_ICON,             //�]�witem��icon
	MUI_MSG_REDRAW_CTRL,
	MUI_MSG_GET_CTRL_TEXT,             //���o���󪺦r��
	MUI_MSG_GET_ITEM_TEXT,             //���o���witem���r��
	MUI_MSG_IS_ACTIVE,                 //�ˬd�O�_��active�����A
	MUI_MSG_GET_PARENT,                //���o������
	MUI_MSG_CHECK_MENU_ID,             //�ˬd����l���ID(combobox)
	MUI_MSG_GET_BAR_POS,               //���oBAR����m
	MUI_MSG_GET_COUNT,
	MUI_MSG_SHOW_CTRL,
	//for tip
	MUI_MSG_SHOW_TIP,
	MUI_MSG_SHOW_TIP_DATA,
	MUI_MSG_ADD_TIP,
	MUI_MSG_TIME_EVT,
	MUI_MSG_SET_BK_COLOR,
	MUI_MSG_SET_TEXT_COLOR,
	//for cursor
	MUI_MSG_CHANGE_CURSOR,
	//for hotkey  
	MUI_MSG_PROCESS_HOTKEY_EVENT,
	MUI_MSG_HOTKEY_EVENT,
	MUI_MSG_GET_HOTKEY_LIST,
	MUI_MSG_ASSIGN_HOTKEY,
	MUI_MSG_REMOVE_HOTKEY,
	MUI_MSG_ENABLE_HOTKEY,
	//for keyboard
	MUI_MSG_KEYDOWN,
	MUI_MSG_KEYUP,
	MUI_MSG_CHAR,
    //for menu
    MUI_MSG_GET_MENU_ID,
    MUI_MSG_RELEASE_MENU_ID,
    MUI_MSG_SEL_ITEM,
    MUI_MSG_GET_ITEM_NAME,
    MUI_MSG_SET_CUR_SEL,
    MUI_MSG_GET_CUR_SEL,
    MUI_MSG_IS_VISIBLE,
	MUI_MSG_GET_INVISIBLE_ITEMS,
	//for KEvent
	MUI_MSG_REG_TAB_ORDER,           //���UTab��\��
	MUI_MSG_REG_TAB_ORDER_EX,
	//for window
	MUI_MSG_SHOW_WINDOW,             //���/���õ���
	MUI_MSG_SET_WINDOW_POS,          //�]�w������m
	MUI_MSG_CREATE_CHILD_WINDOW,     //�إߤ@�Ӥl����
    MUI_MSG_CAPTURE_IMAGE,           //�ɮ��e��
	MUI_MSG_SAVE_IMAGE,              //�x�s�e��
	MUI_MSG_GET_WIN,
	MUI_MSG_SET_ICON,
	MUI_MSG_SET_AP_NAME,
	MUI_MSG_ENABLE_FULL_SCREEN_MODE,
	//for custom ui
	MUI_MSG_ADD_CUSTOM_UI_HWND,

	MUI_MSG_GET_GRIDCTRL_INFO,
	MUI_MSG_GET_CAMERATREE_INFO,
	MUI_MSG_MOUSE_WHEEL,
	MUI_MSG_ENABLE_DROPFILES,
	MUI_MSG_GET_POS,
	MUI_MSG_ENABLE_MAIN_WIN_MOVABLE,
	MUI_MSG_DO_SCALE,
};

enum CtrlEvent
{
	CEVT_CLICKED = 1,
    CEVT_UPDATE,
    CEVT_PRESS_BAR,
    CEVT_RELEASE_BAR,
	CEVT_ACTIVE,
	CEVT_SELECTED,
	CEVT_DBCLICK,
	CEVT_FOCUS,
	CEVT_WINDOWPOSCHANGED,
	CEVT_LOG,
	CEVT_SELCHANGE,
	CEVT_DROPFILES,
	CEVT_ESCAPE,
	CEVT_USER_DEFINE_MSG,
};

enum ButtonStyle
{
	BNS_NORMAL = 1,
    BNS_BITMAP,
	BNS_TEXT_BUTTON,           //��r���A(�ܴ��r��)
	BNS_TEXT_BUTTON1,          //��r���A(���ܴ��r��)
	BNS_CHECK_BOX,
	BNS_BMP_CHECK_BOX,
};
/*
enum WindowStyle
{
	WS_NORMAL = 1,
};
*/
enum ProgressStyle
{
	PRS_NORMAL = 0,
	PRS_ENABLE_EVENT = 1,
};

enum MenuStyle
{
	MUI_MS_NORMAL = 0,
    MUI_MS_CTRL = 1,
    MUI_MS_VSCROLL = (1 << 1),
	MUI_MS_ICON = (1 << 2),
};

enum ComboBoxStyle
{
    MUI_CBS_NORMAL = 0,
    MUI_CBS_VSCROLL = 1,
};

enum CtrlStatus
{
	CTRLS_NORMAL = 0,
    CTRLS_ACTIVE,
	CTRLS_DOWN,
	CTRLS_DISABLE,
};

enum CheckBoxCtrlStatus
{
	CBCS_NORMAL = 0,
	CBCS_ACTIVE,
	CBCS_DOWN,
	CBCS_CHECKED_NORMAL,
	CBCS_CHECKED_ACTIVE,
	CBCS_CHECKED_DOWN,
	CBCS_DISABLE,
};

enum SilderDirection
{
	SD_HORIZONTAL,
	SD_VERTICAL,
};

enum CtrlFunctions
{
	CF_CUSTOM = 0,
	CF_SYS_INC,
	CF_SYS_DEC,
    CF_SYS_PULL_DOWN,
};

enum CursorStatus
{
	MUI_CS_NORMAL = 0,
    MUI_CS_ACTIVE,
	MUI_CS_EDIT,
	MUI_CS_RESIZE,
};

enum AlignTextType
{
	ATT_LEFT = 0,
    ATT_CENTER,
	ATT_RIGHT,
};

enum ComboBoxDataType
{
	CBDT_NORMAL = 0,
	CBDT_RANGE,
};

enum EditBoxStyle
{
	UI_EBS_NORMAL = 0,
	UI_EBS_ALWAY_FOCUS = 1,
	UI_EBS_MULTI_LINES = 2,
	UI_EBS_PASSWORD, 
};

enum WindowStyle
{
	WS_NORMAL = 1,
	WS_STRETCH_LEFT = 2,
	WS_STRETCH_BOTH = 3,
};

enum WindowType
{
	WT_NORMAL = 1,
    WT_EXT,
};

typedef struct
{
    int left;
	int top;
	int right;
	int bottom;
	int GetWidth()
	{
		return right - left;
	}
	int GetHeight()
	{
		return bottom - top;
	}
}_Region;

typedef struct
{
	int x;
	int y;
}PositionInfo;

typedef struct
{
    char szFontName[128];
	int iFontSize;
}UIFontInfo;

typedef struct
{
	char szFileName[256];
	int x;
	int y;
	_Region region;
	unsigned long ulID;
	void* pBmpObj;
	unsigned char ucColorKey[3];
	bool bEnableColorKey;
	HRGN hRGN;
	bool bDoMakeRGN;    //�P�_�O�_�n������rgn���\��
}BmpItem;

typedef struct
{
	unsigned long ulID;
	int iStatus;
	BmpItem* pItem;
}RegObjInfo;

typedef struct
{
    HDC hDC;
    unsigned long ulID;
    int iStatus;
	int x;
	int y;
	int w;
	int h;
}UpdateInfo;

typedef struct
{
	int iX;
	int iY;
	int iWidth;
	int iHeight;
	int iStatus;
	PositionInfo SrcPos[MAX_STATUS];
}CtrlBaseInfo;

typedef struct
{
    int iX;
	int iY;
	int iWidth;
	int iHeight;
	int iStatus;
	PositionInfo SrcPos[MAX_STATUS];
	unsigned long ulID;
    unsigned long ulTipID;
	char szFileName[256];
	char szDescription[256];
	int iTabOrder;                   //Tab������
	unsigned long ulGroupID;         //�s��ID
	int iGroupIndex;                 //�b�s�դ���index
	bool bSingleSelection;           //��@���
}CtrlInfo;

typedef struct
{
	CtrlInfo ctrlinfo;
	bool bEnableMask;
	unsigned char ucMask[3];
	unsigned long ulParentID;
    char szName[80];
    int iStyle;
}ButtonInfo;

typedef struct
{
	CtrlInfo ctrlinfo;
	bool bEnableMask;
	bool bMakeRGN;    //�P�_�O�_�n������rgn���\��
	unsigned char ucMask[3];
	char szMaskFileName[256];
	
	int iStyle;
	_Region rStretch[2];
}WindowInfo;

typedef struct
{
	CtrlInfo ctrlinfo;
	int iDirection;
	CtrlBaseInfo bar;
	unsigned long ulParentID;
	int iMin;
	int iMax;
	CtrlBaseInfo spin[2];
	bool bEnableMask;
	unsigned char ucMask[3];
}ScrollBarInfo;

typedef struct
{
	CtrlInfo ctrlinfo;
	unsigned long ulStyle;
	PositionInfo border;
	unsigned long ulParentID;
	int iMin;
	int iMax;
}ProgressInfo;

typedef struct
{
	void* pWin;
	unsigned long ulCtrlID;
	unsigned long ulMsg;
	void* pParm1;
	void* pParm2;
}CtrlEvtInfo;

typedef struct
{
	int x;
	int y;
	char szMsg[256];
	_Region rect;
	HWND hWnd;
	unsigned long ulTipID;
}TipInfo;

typedef struct
{
	int x;
	int y;
	wchar_t wszMsg[256];
	_Region rect;
	HWND hWnd;
	unsigned long ulTipID;
}TipInfoW;

typedef struct
{
	unsigned char ucBK[3];
	unsigned char ucText[3];
	//char szFontName[40];
	//int iFontSize;
    UIFontInfo font;
}TipUIInfo;

typedef struct
{
	CtrlInfo ctrlinfo;
	unsigned char ucText[3];
	//char szFontName[40];
	//int iFontSize;
    UIFontInfo font;
	unsigned long ulParentID;
	int iAlignText;                  //��r���ƦC�覡
}TextInfo;

typedef struct
{
	CtrlInfo ctrlinfo;
	bool bEnableMask;
	unsigned char ucMask[3];
	unsigned long ulParentID;
	char szName[80];
    int iStyle;
	int iStartPos;                   //�q��r�}�l����m
	int iAlignText;                  //��r���ƦC�覡
}CheckBoxInfo;

typedef struct
{
	CtrlInfo ctrlinfo;
	bool bEnableMask;
	unsigned char ucMask[3];
	unsigned long ulParentID;
}PictureInfo,AnimationCtrlInfo;

typedef struct
{
	int iMin;
	int iMax;
}RangeInfo;

typedef struct
{
	char szFolder[256];
	char szSettingFile[80];
	bool bHide;
	HWND hParent;
}SkinInfo;

typedef struct
{
    unsigned char ucNormal[3];
    unsigned char ucActive[3];
    unsigned char ucDown[3];
    unsigned char ucDisable[3];
	unsigned char ucOutline[3];
    //char szFontName[40];
	//int iFontSize;
    UIFontInfo font[3];
}CtrlTextColorInfo;

typedef struct  
{
    HWND hWnd;
	unsigned short usKeyCode;
	unsigned short usKeyData;
}KeyboardEventInfo;

typedef struct
{
	CtrlInfo ctrlinfo;
	char szText[256];
	unsigned long ulParentID;
    int iStartPos;                   //�q��r�}�l����m
    unsigned char ucCursorColor[3];    
	int iStyle;
}EditBoxInfo;

typedef struct
{
    unsigned long ulID;
    char szName[80];
    unsigned long ulStyle;          //���j�u�B�r��B�l���..
    void* pParent;
    void* pChild;
	long lIconID;         //0 - ���qIcon
}MenuItem;

typedef struct
{
    int iStyle;
    EditBoxInfo editbox;
    CtrlBaseInfo button;
    void* pMenuItems;
    ScrollBarInfo vscroll;
    int iMenuHeight;
	int iDataType;                         //��ƪ����A�A�@��άO�d��
}ComboBoxInfo;
/*
typedef struct
{
    int iStyle;
	CtrlInfo ctrlinfo;
    //EditBoxInfo editbox;
    ButtonInfo button;
    void* pMenuItems;
    ScrollBarInfo vscroll;
    int iMenuHeight;
	int iDataType;                         //��ƪ����A�A�@��άO�d��
	unsigned long ulParentID;
}MenuItemInfo;
*/
typedef struct
{
	CtrlInfo ctrlinfo;
	bool bEnableMask;
	unsigned char ucMask[3];
	unsigned long ulParentID;
    char szName[80];
    int iStyle;
	void* pMenuItems;
    ScrollBarInfo vscroll;
    int iMenuHeight;
	int iDataType;                         //��ƪ����A�A�@��άO�d��
}ButtonMenuInfo;

typedef struct
{
    unsigned char ucWinBK[3];
	unsigned char ucText[3];
	unsigned char ucHLText[3];
	unsigned char ucHLColor[3];
	UIFontInfo font;
}MenuGDIInfo;

typedef struct
{
	unsigned long ulCtrlID;
	int iTabOrder;
	unsigned long ulGroupID;         //0 - ��ܫD�s��
	int iGroupIndex;                 //�s�դ����Ƕ�
	void* pList;                     //�s��s�դ������
	int iCurSel;                     //�ثe�s�դ��Q��ܪ�����
	bool bSingleSelection;           //��@���
	void* pWin;
}TabOrderInfo;

typedef struct
{
	int iIndex;
	char szText[256];
	unsigned long ulIconID;
}CtrlItemInfo;

typedef struct
{
	int iStyle;
	CtrlInfo ctrlinfo;
	unsigned long ulParentID;
    void* pMenuItems;
    ScrollBarInfo vscroll;
    int iMenuHeight;
}ListBoxInfo;

//for DVR
typedef struct
{
	int x;
	int y;
	int w;
	int h;
}DisplayWinInfo;

typedef struct
{
	unsigned long ulID;
	int x;
	int y;
	int w;
	int h;
}CustomUIInfo;

typedef struct
{
	unsigned long ulID;
	HWND hWnd;
}CustomUIHandleInfo;

typedef struct
{
	int x;
	int y;
	int w;
	int h;
} GridCtrlInfo;

typedef struct
{
	int x;
	int y;
	int w;
	int h;
} CameraTreeInfo;

typedef struct
{
	HWND hWnd;
	short iKeys;
	short iDelta;
	//int iX;
	//int iY;
}MouseWheelInfo;

#endif
