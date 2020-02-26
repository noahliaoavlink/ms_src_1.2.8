#ifndef _RenderCommon_H_
#define _RenderCommon_H_

#define _MAX_OSD_LEN 256

#define _BUF_WIDTH 1920
#define _BUF_HEIGHT 1080

enum VideoPixelFormat
{
	VPF_I420 = 1,
	VPF_YUY2,
	VPF_RGB32,
	VPF_RGB24,
	VPF_NV12,
	VPF_YV12,
};

enum OSDType
{
	OSD_CAMERA_NAME = 1,
	OSD_TIMESTAMP,
	OSD_PLAYBACK_STATUS,
	OSD_DEBUG_INFO,
	OSD_DEBUG_INFO2,
	OSD_SIGNAL_TYPE,
};

enum VideoRenderMode
{
	VRM_DIB = 1,
	VRM_D3D_MULTI_VIEW,
	VRM_D3D_SINGLE_VIEW,
};

enum ScrollingTextType
{
	SST_NONE,
	SST_H, //horizontal
	SST_V, //vertical
};

typedef struct
{
	int iObjIndex;
	unsigned long id;
	bool bEnable;
	int x;
	int y;
	unsigned char ucColor[4];
	unsigned long ulFormat;
	RECT rect;
	bool bOutlineText;
	bool bUpdate;
	int iScrollingTextType;
	int iOffset;
	//char szData[_MAX_OSD_LEN];
	wchar_t wszData[_MAX_OSD_LEN];
}OSDInfo;

typedef struct
{
	int id;
	int x;
	int y;
	char szFileName[256];
}IconInfo;

typedef struct
{
	int id;
	int iIconID;
	bool bEnable;
	int x;
	int y;
}IconCopyInfo;

typedef struct
{
	char szName[256];
	int iSize;
}FontInfo;

typedef struct
{
	int iIndex;
	void* pBuffer;
	int iWidth;
	int iHeight;
	int iPixelFormat;
}VideoFrameParam;

typedef struct
{
	int iIndex;
	char szFacename[256];
	int iWidth;
	int iHeight;
	void* pD3DOSDObj;
}D3DOSDObjInfo;

typedef struct
{
	int iType;
	char szData[256];
	wchar_t wszData[256];
}OSDString;

#endif