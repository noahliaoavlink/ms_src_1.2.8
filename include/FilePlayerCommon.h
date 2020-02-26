#ifndef _FilePlayerCommon_H_
#define _FilePlayerCommon_H_

enum MediaFileDialogMode
{
	MFDM_EDIT,
	MFDM_SELECT,
};

enum MediaFilePlaybackMode
{
	MFPM_STOP = 0,
	MFPM_FORWARD,
	MFPM_BACKWARD,
	MFPM_PAUSE
};

enum FilePlayerEvent
{
	FPE_DO_REPEAT = 1, //DoRepeat
};

enum DisplayIntervalMode
{
	DIM_NORMAL = 0,//normal
	DIM_LOCAL,
	DIM_GOBAL,
};

typedef struct
{
	void* pObj;
}CopyDataInfo;

typedef struct
{
	char szName[128];
	float fDuration;
	int iEffectID;
	int iLevel;
	int iParameter1;
	int iParameter2;
	int iParameter3;
	int iParameter4;
	int iParameter5;
}EffectInfo;

class FilePlayerCallback
{
   public:
		virtual void UpdatePosInfo(char* szCurPos,char* szTotalLen) = 0;
		virtual void SetSliderCtrlRange(double dPos) = 0;
		virtual void SetSliderCtrlPos(double dPos) = 0;
		virtual void DisplayVideo(unsigned char* pBuffer,int w,int h) = 0;
		virtual void PlayAudio(unsigned char* pBuffer,int iLen) = 0;
		virtual void Event(int iType) = 0;
		virtual void DisplayVideo_HW(void* pD3DDevice,unsigned char* pBuffer,int w,int h) = 0;
};

class MediaFileManagerCallback
{
	public:
		virtual void ReturnFileName(wchar_t* wszFileName) = 0;
	 
};

class MediaFileConverterCallback
{
	public:
		virtual void UpdateProgress(int iProgess) = 0;
		virtual void UpdateStatus(int iStatus,char* szMsg) = 0;
	 
};

class IOSourceCtrlCallback
{
	public:
		virtual void BeginOpenFile(wchar_t* wszFileName) = 0;
		virtual void EndOpenFile(wchar_t* wszFileName) = 0;
		virtual void Event(int iType,char* szMsg) = 0;
};

class VideoRawDataCallback
{
	public:
		virtual void DisplayVideoRawData(unsigned char* pBuffer,int w,int h) = 0;
		virtual void DisplayVideoSurfaceData(void* pSurface,int w,int h) = 0;
		virtual void DisplayNV12SurfaceData(void* pSurface,int w,int h) = 0;
	 
};

#endif
