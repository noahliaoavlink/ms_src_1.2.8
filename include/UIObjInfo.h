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
   UI_OID_MENU_ID = UIID_BASE_ID + UIID_MAX_NUM * 11,    //選單流水號
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
	MUI_MSG_REDRAW,                   //重畫 
	MUI_MSG_MOUSE_MOVE,               //滑鼠移動
	MUI_MSG_LB_DWON,                  //滑鼠左鍵(按下)
	MUI_MSG_LB_UP,                    //滑鼠左鍵(放開)
	MUI_MSG_RB_DWON,                  //滑鼠右鍵(按下)
	MUI_MSG_RB_UP,                    //滑鼠右鍵(放開)
	MUI_MSG_LB_DBLCLK,                //Double Click
	MUI_MSG_REG_OBJ,
	MUI_MSG_UNREG_OBJ,
	MUI_MSG_UPDATE,
	MUI_MSG_UPDATE_EX,
	MUI_MSG_GET_UNKNOWN_ID,            //用來分配動態產生的控制元件的ID值
	MUI_MSG_GET_SKIN_DIR,              //取得面板目錄
	MUI_MSG_SET_SKIN_DIR,              //設定面板目錄 
	MUI_MSG_ENABLE_CTRL,               //Enable或Disable控制元件
	MUI_MSG_CTRL_EVENT,
	MUI_MSG_SET_POS,                   //設定位置(silder,scrollbar,progress,..)
	MUI_MSG_SYS_FUNCTION,              //內定功能(如scrollbar的上下鍵..)
	MUI_MSG_REG_BUTTON,                //註冊一個button元件
	MUI_MSG_IN_REGION,                 //判斷是否在區域內
	MUI_MSG_CHANGE_SKIN,               //變更面板功能
	MUI_MSG_SET_CHECKED,               //設為check的狀態
	MUI_MSG_IS_CHECKED,                //檢查是否為check的狀態
	MUI_MSG_RESET,                     //重設
	MUI_MSG_CHANGE_MODE,               //改變模式(相當是換一次面板，不同的是在同一目錄下變換)
	MUI_MSG_SET_RANGE,                 //設定範圍 (開始值及最大值)
	MUI_MSG_SET_TEXT,                  //設定控制元件的文字(或名稱) 
	MUI_MSG_GET_MAIN_WIN,
	MUI_MSG_IS_READY,
	MUI_MSG_GET_ID,
	MUI_MSG_GET_ID_NAME,
	MUI_MSG_SET_DISPLAY_WIN,           //for dvr
	MUI_MSG_SET_CTRL_POS,
	MUI_MSG_DRAW_RECT,                 //重繪指定的區域
	MUI_MSG_GET_REGION,                //取得控制元件的區域 
    MUI_MSG_TIME_EVENT,
    MUI_MSG_REG_EDITBOX,
    MUI_MSG_REG_MENU,
    MUI_MSG_REG_SCROLLBAR,
    MUI_MSG_SHOWWINDOW,
	MUI_MSG_CHANGE_IMAGE,              //變更影像資料
	MUI_MSG_GET_TOTAL_OF_STATUS,       //取得控制元件所有的狀態
	MUI_MSG_RESET_STATUS,              //重設狀態
	MUI_MSG_SET_ACTIVE,                //將指定的控制元件設定為ACTIVE
	MUI_MSG_TRIGGER,
    MUI_MSG_SET_FOCUS,                 //將指定的控制元件設定為FOCUS
    MUI_MSG_INSERT_STRING,             //新增一筆字串到控制元件
    MUI_MSG_ADD_STRING,                //插入一筆字串到控制元件
    MUI_MSG_DELETE_STRING,             //刪除一筆字串到控制元件
    MUI_MSG_RESET_CONTENT,             //清除控制元件中所有的資料
    MUI_MSG_PROCESS_SINGLE_SELECTION,  //處理單一選擇
    MUI_MSG_SIMULATE_KEYDOWN,          //模仿鍵盤訊息
    MUI_MSG_CANCLE,                    //取消
	MUI_MSG_SET_PARENT,
	MUI_MSG_SET_ITEM_ICON,             //設定item的icon
	MUI_MSG_REDRAW_CTRL,
	MUI_MSG_GET_CTRL_TEXT,             //取得元件的字串
	MUI_MSG_GET_ITEM_TEXT,             //取得指定item的字串
	MUI_MSG_IS_ACTIVE,                 //檢查是否為active的狀態
	MUI_MSG_GET_PARENT,                //取得父視窗
	MUI_MSG_CHECK_MENU_ID,             //檢查元件子選單ID(combobox)
	MUI_MSG_GET_BAR_POS,               //取得BAR的位置
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
	MUI_MSG_REG_TAB_ORDER,           //註冊Tab鍵功能
	MUI_MSG_REG_TAB_ORDER_EX,
	//for window
	MUI_MSG_SHOW_WINDOW,             //顯示/隱藏視窗
	MUI_MSG_SET_WINDOW_POS,          //設定視窗位置
	MUI_MSG_CREATE_CHILD_WINDOW,     //建立一個子視窗
    MUI_MSG_CAPTURE_IMAGE,           //補捉畫面
	MUI_MSG_SAVE_IMAGE,              //儲存畫面
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
	BNS_TEXT_BUTTON,           //文字型態(變換字型)
	BNS_TEXT_BUTTON1,          //文字型態(不變換字型)
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
	bool bDoMakeRGN;    //判斷是否要做產生rgn的功能
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
	int iTabOrder;                   //Tab的順序
	unsigned long ulGroupID;         //群組ID
	int iGroupIndex;                 //在群組中的index
	bool bSingleSelection;           //單一選擇
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
	bool bMakeRGN;    //判斷是否要做產生rgn的功能
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
	int iAlignText;                  //文字的排列方式
}TextInfo;

typedef struct
{
	CtrlInfo ctrlinfo;
	bool bEnableMask;
	unsigned char ucMask[3];
	unsigned long ulParentID;
	char szName[80];
    int iStyle;
	int iStartPos;                   //秀文字開始的位置
	int iAlignText;                  //文字的排列方式
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
    int iStartPos;                   //秀文字開始的位置
    unsigned char ucCursorColor[3];    
	int iStyle;
}EditBoxInfo;

typedef struct
{
    unsigned long ulID;
    char szName[80];
    unsigned long ulStyle;          //分隔線、字串、子選單..
    void* pParent;
    void* pChild;
	long lIconID;         //0 - 不秀Icon
}MenuItem;

typedef struct
{
    int iStyle;
    EditBoxInfo editbox;
    CtrlBaseInfo button;
    void* pMenuItems;
    ScrollBarInfo vscroll;
    int iMenuHeight;
	int iDataType;                         //資料的型態，一般或是範圍
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
	int iDataType;                         //資料的型態，一般或是範圍
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
	int iDataType;                         //資料的型態，一般或是範圍
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
	unsigned long ulGroupID;         //0 - 表示非群組
	int iGroupIndex;                 //群組中的序順
	void* pList;                     //存放群組中的資料
	int iCurSel;                     //目前群組中被選擇的項目
	bool bSingleSelection;           //單一選擇
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
