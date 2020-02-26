#ifndef _MediaFileCommon_H_
#define _MediaFileCommon_H_

#define _MAX_SCREENS 16
#define _MAX_TAG_LEN 256

#define PLAY_3D_MODE
#define _THUMBNAIL_FRAME_W 80
#define _THUMBNAIL_FRAME_H 60

enum MediaFileFormat
{
	MFF_ALL = -1,
	MFF_UNKNOW = 0,
	MFF_OTHER,
	MFF_MP4,
	MFF_AVI,
	MFF_MKV,
	MFF_RMVB,
	MFF_WMV,
	MFF_RTSP,
	MFF_SEARCH_RESULTS,
};

enum FileInfoDisplayMode
{
	FIDM_LIST = 0,
    FIDM_ICON,
};

enum FilterType
{
	FT_EQ = 0,
	FT_SCALE,
	FT_CROP,
	FT_DRAW_TEXT,
	FT_FADE,
	FT_HUE, //hue
	FT_LUTYUV,//lutyuv
	FT_LUTRGB,//lutrgb
	FT_FREI0R,//frei0r
	FT_SMARTBLUR,//smartblur
	FT_AUDIO_FILTER_BEGIN,
	FT_AFORMAT = 101,//audio , aformat
	FT_ATEMPO, //atempo
};

typedef struct
{
	wchar_t wszFileName[512];
	wchar_t wszTag[_MAX_TAG_LEN];
	//wchar_t wszTags[_MAX_TAGS][64];
	int iFileFormat;
	int iVideoCodecID;
	int iAudioCodecID;
	double dLen;
	wchar_t wszFileFormat[10];
	wchar_t wszVideoCodecName[20];
	wchar_t wszAudioCodecName[20];
	wchar_t wszLen[20];
	int iW;
	int iH;
	long lCoverIndex;//Thumbnail
	unsigned long ulID;
	bool bIsEncrypted;
}MediaFileInfo;

typedef struct
{
	unsigned char* pBuffer;
	int iLen;
}ThumbnailPicture;

typedef struct
{
	int iDeviceIndex;
	int iSrcX;
	int iSrcY;
	int iSrcW;
	int iSrcH;
}DeviceMapping;

typedef struct
{
	wchar_t wszName[64];
	wchar_t wszFileName[512];
	int iLayoutIndex;
	int iTotalOfDevices;
	DeviceMapping Devices[_MAX_SCREENS];
	//int iDstX;
	//int iDstY;
	//int iDstW;
	//int iDstH;
}DisplayTask;

typedef struct 
{
	wchar_t wszDeviceName[128];
	wchar_t wszDeviceString[128];
	wchar_t wszMonitorName[128];
	int x;
	int y;
	int h;
	int w;
}MonitorDeviceInfo;

#define OBJ_MSG_BASE_ID   3001
#define PLUGIN_MSG_BASE_ID   10001

enum ObjMsg
{
   OBJ_MSG_INIT = OBJ_MSG_BASE_ID,
   OBJ_MSG_DESTROY,
   OBJ_MSG_RBUTTON_DOWN,
   OBJ_MSG_SEL_CHANGED,
   OBJ_MSG_UPDATE_LIST,
   OBJ_MSG_UPDATE_POS_INFO,
   OBJ_MSG_SET_SLIDER_POS,
   OBJ_MSG_SET_SLIDER_RANGE,
};

class WinMsgCallback
{
   public:
    virtual void* WinMsg(int iMsg,void* pParameter1,void* pParameter2) = 0;
};

class EventCallback
{
   public:
    virtual void* Event(int iMsg,void* pParameter1,void* pParameter2) = 0;
};

typedef struct
{
	char* szVersion;
	unsigned long ulID;        //保護ID，當ID值符合時才能使用plugin
	int iType;
	void* (*SendMsg)(int iMsg,void* pParameter1,void* pParameter2);
}MsgInfo;
/*
enum WindowType
{
	WT_NORMAL = 1,
    WT_EXT,
};
*/
enum UIMsg
{
    UI_MSG_INIT = PLUGIN_MSG_BASE_ID + 12000,
    UI_MSG_DESTROY,
    UI_MSG_SET_POS,                      //設定slider,scrollbar,..的位置
    UI_MSG_ENABLE_CTRL,                  //enable,disable指定的控制元件
    UI_MSG_EXIT,                         //離開
    UI_MSG_CHANGE_SKIN,                  //切換面板
    UI_MSG_CLICKED,                      //button被clicked時的訊息 
    UI_MSG_DBCLICK,                      //double click
    UI_MSG_SET_CHECK,                    //設定或解除check的狀態
    UI_MSG_CTRL_UPDATE,                  //範圍型控制元件，位置更新的訊息
    UI_MSG_SET_DISPLAY_WIN,              //設定顯示視窗 (for DVR)
    UI_MSG_IS_CHECKED,                   //是否為check的狀態
    UI_MSG_CHANGE_MODE,                  //切換模式 
    UI_MSG_SET_RANGE,                    //設定slider,scrollbar,..的範圍
    UI_MSG_SET_TEXT,                     //設定字串 
    UI_MSG_PRESS_BAR,                    //bar剛被點選
    UI_MSG_RELEASE_BAR,                  //bar被釋放
    UI_MSG_GET_MAIN_WIN,
    UI_MSG_IS_READY,
    UI_MSG_SET_CTRL_POS,                 //設定控制元件的位置
    UI_MSG_GET_REGION,                   //取得控制元件的範圍資訊
    UI_MSG_REDRAW,                       //更新UI所有控制元件
    UI_MSG_REDRAW_CTRL,                  //更新指定的UI控制元件
    UI_MSG_CHANGE_IMAGE,                 //變更Icon的資料
    UI_MSG_GET_TOTAL_OF_STATUS,          //取得控制元件所有的狀態
    UI_MSG_CTRL_ACTIVE_EVENT,
    UI_MSG_CTRL_SELECTED,
    UI_MSG_INSERT_STRING,                //新增一筆字串到控制元件
    UI_MSG_ADD_STRING,                   //插入一筆字串到控制元件
    UI_MSG_DELETE_STRING,                //刪除一筆字串到控制元件
    UI_MSG_RESET_CONTENT,                //清除控制元件中所有的資料
    UI_MSG_SET_CUR_SEL,
    UI_MSG_HOTKEY_EVENT,
    UI_MSG_SIMULATE_KEYDOWN,             //模仿鍵盤訊息
    UI_MSG_GET_ID,                       //由名稱取得ID值
    UI_MSG_SET_FOCUS,
    UI_MSG_GET_CTRL_TEXT,                //取得元件的字串 
    UI_MSG_GET_CUR_SEL,                  //取得目前選擇的index
    UI_MSG_GET_ITEM_TEXT,                //取得指定item的文字
    UI_MSG_IS_ACTIVE,                    //檢查是否為active
    UI_MSG_ENABLE_RC_MODE,               //開啟/關閉搖控器模式
    UI_MSG_GET_TOTAL,                    //取得控制元件選項的數量
    UI_MSG_GET_BAR_POS,
	UI_MSG_GET_COUNT,
	UI_MSG_SELCHANGE,
    //for hotkey
    UI_MSG_GET_HOTKEY_LIST,
    UI_MSG_ASSIGN_HOTKEY,
    UI_MSG_REMOVE_HOTKEY,
    UI_MSG_ENABLE_HOTKEY,              //開啟或是關閉hotkey功能 0-disable,1-enable  
    //for MutiLanguage
    UI_MSG_GET_TIP_LANGUAGE,  
    //for window
    UI_MSG_SHOW_WINDOW,
    UI_MSG_SET_WINDOW_POS,
    UI_MSG_CREATE_CHILD_WINDOW,
    UI_MSG_SET_ICON,                    //設定軟體圖示
    UI_MSG_SET_AP_NAME,                 //設定軟體名稱
    UI_MSG_GET_WIN,
    //for DVR
    UI_MSG_GET_DISPLAY_WIN_INFO,
    UI_MSG_ENABLE_QUARTER_MODE,         //開啟/關閉15分鐘模式
    UI_MSG_GET_ITEM_STATUS,             //取得排程元件項目的狀態
    UI_MSG_SET_CYCLE_TYPE,              //設定周期種類
    UI_MSG_RESET,
    UI_MSG_SET_ITEM_STATUS,
    UI_MSG_SET_DAILY,
    UI_MSG_WINDOWPOSCHANGED,
    UI_MSG_CTRL_EVENT,
    UI_MSG_SET_VOLUME,
    UI_MSG_RECORD,
    UI_MSG_NETWORK,
    UI_MSG_GET_RECORD_STATUS,
    UI_MSG_GET_NETWORK_STATUS,
    UI_MSG_MINIMIZE,
    UI_MSG_KEY_DOWN,
    UI_MSG_KEY_UP,

	UI_MSG_GET_GRIDCTRL_INFO,
	UI_MSG_GET_CAMERATREE_INFO,
	UI_MSG_DROPFILES,
	UI_MSG_ENABLE_DROPFILES,
	UI_MSG_UPDATE_SLIDER_RANGE,
	UI_MSG_ENABLE_FULL_SCREEN_MODE,
	UI_MSG_SHOW_CTRL,
	UI_MSG_ESCAPE,
	UI_MSG_GET_POS,
	UI_MSG_ENABLE_MAIN_WIN_MOVABLE,
	UI_MSG_DO_SCALE,
	UI_MSG_CANCLE,
};

#endif
