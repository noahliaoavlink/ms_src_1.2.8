#ifndef _OutVideoDll_H_
#define _OutVideoDll_H_

#define _MAX_VIDEO_OBJ 16 + 1

typedef void* (*_Out_OVD_Create_Proc)();
typedef void (*_Out_OVD_Destroy_Proc)(void* pOVDObj);
typedef void (*_Out_OVD_Init_Proc)(void* pOVDObj,HWND hWnd);
typedef int (*_Out_OVD_RenderFrame_Proc)(void* pOVDObj,unsigned char* pBuffer,int iWidth,int iHeight);
typedef int (*_Out_OVD_RenderFrame1_Proc)(void* pOVDObj,void* pSurface,int iWidth,int iHeight);
typedef int (*_Out_OVD_RenderFrame2_Proc)(void* pOVDObj,int iPixelFormat,unsigned char* pBuffer,int iWidth,int iHeight);
typedef int (*_Out_OVD_RenderFrame3_Proc)(void* pOVDObj,int iPixelFormat,unsigned char* pBuffer,int iWidth,int iHeight,RECT rSrc);
typedef void (*_Out_OVD_Flip_Proc)(void* pOVDObj);
typedef void (*_Out_OVD_UpdateOSD_Proc)(void* pOVDObj,int iType,wchar_t* wszData);
typedef void* (*_Out_OVD_GetD3DDevice_Proc)(void* pOVDObj);
typedef void (*_Out_OVD_Clean_Proc)(void* pOVDObj);
typedef void (*_Out_OVD_Target2RawData_Proc)(void* pOVDObj,int iWidth,int iHeight,unsigned char* pOutBuffer);
typedef int (*_Out_OVD_GetBackbufWidth_Proc)(void* pOVDObj);
typedef int (*_Out_OVD_GetBackbufHeight_Proc)(void* pOVDObj);
typedef void(*_Out_OVD_EnableOAR_Proc)(void* pOVDObj, bool bEnable);
typedef void(*_Out_OVD_SetOAROffset_Proc)(void* pOVDObj, float fValue);
typedef void(*_Out_OVD_SetOSDPos_Proc)(void* pOVDObj, int x, int y);
typedef void(*_Out_OVD_SetOSDColor_Proc)(void* pOVDObj, unsigned char r, unsigned char g, unsigned char b);

class OutVideoDll
{
		HINSTANCE m_hInst;
		//void* m_pOVDObj;
		void* m_pOVDObjs[_MAX_VIDEO_OBJ];
	public:
		OutVideoDll();
        ~OutVideoDll();

		bool LoadLib();
		void FreeLib();

		_Out_OVD_Create_Proc _Out_OVD_Create;
		_Out_OVD_Destroy_Proc _Out_OVD_Destroy;
		_Out_OVD_Init_Proc _Out_OVD_Init;
		_Out_OVD_RenderFrame_Proc _Out_OVD_RenderFrame;
		_Out_OVD_RenderFrame1_Proc _Out_OVD_RenderFrame1;
		_Out_OVD_RenderFrame2_Proc _Out_OVD_RenderFrame2;
		_Out_OVD_RenderFrame3_Proc _Out_OVD_RenderFrame3;
		_Out_OVD_Flip_Proc _Out_OVD_Flip;
		_Out_OVD_UpdateOSD_Proc _Out_OVD_UpdateOSD;
		_Out_OVD_GetD3DDevice_Proc _Out_OVD_GetD3DDevice;
		_Out_OVD_Clean_Proc _Out_OVD_Clean;
		_Out_OVD_Target2RawData_Proc _Out_OVD_Target2RawData;
		_Out_OVD_GetBackbufWidth_Proc _Out_OVD_GetBackbufWidth;
		_Out_OVD_GetBackbufHeight_Proc _Out_OVD_GetBackbufHeight;
		_Out_OVD_EnableOAR_Proc _Out_OVD_EnableOAR;
		_Out_OVD_SetOAROffset_Proc _Out_OVD_SetOAROffset;
		_Out_OVD_SetOSDPos_Proc _Out_OVD_SetOSDPos;
		_Out_OVD_SetOSDColor_Proc _Out_OVD_SetOSDColor;

		void Init(int iIndex,HWND hWnd);
		int RenderFrame(int iIndex,unsigned char* pBuffer,int iWidth,int iHeight);
		int RenderFrame1(int iIndex,void* pSurface,int iWidth,int iHeight);
		int RenderFrame2(int iIndex,int iPixelFormat,unsigned char* pBuffer,int iWidth,int iHeight);
		int RenderFrame3(int iIndex,int iPixelFormat,unsigned char* pBuffer,int iWidth,int iHeight,RECT rSrc);
		void Flip(int iIndex);
		void UpdateOSD(int iIndex,int iType,wchar_t* wszData);
		void* GetD3DDevice(int iIndex);
		void Clean(int iIndex);
		void Target2RawData(int iIndex,int iWidth,int iHeight,unsigned char* pOutBuffer);
		int GetBackbufWidth(int iIndex);
		int GetBackbufHeight(int iIndex);
		void EnableOAR(int iIndex,bool bEnable);
		void SetOAROffset(int iIndex, float fValue);
		void SetOSDPos(int iIndex, int x, int y);
		void SetOSDColor(int iIndex, unsigned char r, unsigned char g, unsigned char b);
};

#endif
