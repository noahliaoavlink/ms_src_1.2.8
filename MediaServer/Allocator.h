//------------------------------------------------------------------------------
// File: Allocator.h
//
// Desc: DirectShow sample code - interface for the CAllocator class
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#if !defined(AFX_ALLOCATOR_H__F675D766_1E57_4269_A4B9_C33FB672B856__INCLUDED_)
#define AFX_ALLOCATOR_H__F675D766_1E57_4269_A4B9_C33FB672B856__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <dshow.h>

#pragma warning(push, 2)
#include <vector>
#pragma warning(pop)
using namespace std;

const int FILTER_NUM = 1;
const UINT MAX_FILM = 10;
const UINT MAX_PTAREA = 6;

#define _ENABLE_FLIP_THREAD 1
#define _ENABLE_EVENT_SYN 1
#define _VF_SINGLE_THREAD 1
#define _ENABLE_SYNC_PB_CTRL 1

#ifdef _DISABLE_PLANE_SCENE_3D
#else
#define PLAY_3D_MODE
#endif

#define NOT_PROC_EFFECT

#ifdef PLAY_3D_MODE
	#include "PlaneScene3D.h"
#else
#include <vmr9.h>
	#include "PlaneScene2D.h"
#endif	   
const UINT DEF_NOTIFY_MSG = 0x1000;

#include "MyEffect3D.h"
#include "EdgeBlendDlg.h"
#include "SyncPBCtrl.h"

#ifdef _TRAIL_VERSION
#include "D3DSprite.h"
#endif

#include "..\\..\\..\\Include\\MathCommon.h"

#define _ENABLE_MASK 1

typedef enum
{
	MOUSE_MOVE = 0,  
	MOUSE_LBUT_DOWN,
	MOUSE_LBTN_UP,
	MOUSE_RBTN_DOWN,
	MOUSE_RBTN_UP,
	MOUSE_KEY_MOVE
} MOUSE_EVT;

typedef enum
{
	MAP2D_NOACT = 0,
	MAP2D_SET_GRID = 1,
	MAP2D_SELECT_PT = 2,
	MAP2D_MODIFY_PT = 3,
	MAP2D_SETPT_COLR = 4,
	MAP2D_NEED_UPDATE_BK = 1 << 4,
	MAP2D_SHOW_BK_PT = 1 << 5,
} MAP2D_ACT;

class CAllocator  : public	VideoRawDataCallback, public IVMRSurfaceAllocator9, public IVMRImagePresenter9
{
public:
    CAllocator(HRESULT& hr, HWND wnd, int iIndex, CRect rectClient, IMediaSeeking *pMS,
		       IDirect3D9* d3d = NULL, IDirect3DDevice9* d3dd = NULL, CWnd* pParentWnd = NULL);
    virtual ~CAllocator();

    // IVMRSurfaceAllocator9
    virtual HRESULT STDMETHODCALLTYPE InitializeDevice( 
            /* [in] */ DWORD_PTR dwUserID,
            /* [in] */ VMR9AllocationInfo *lpAllocInfo,
            /* [out][in] */ DWORD *lpNumBuffers);
            
    virtual HRESULT STDMETHODCALLTYPE TerminateDevice( 
        /* [in] */ DWORD_PTR dwID);
    
    virtual HRESULT STDMETHODCALLTYPE GetSurface( 
        /* [in] */ DWORD_PTR dwUserID,
        /* [in] */ DWORD SurfaceIndex,
        /* [in] */ DWORD SurfaceFlags,
        /* [out] */ IDirect3DSurface9 **lplpSurface);
    
    virtual HRESULT STDMETHODCALLTYPE AdviseNotify( 
        /* [in] */ IVMRSurfaceAllocatorNotify9 *lpIVMRSurfAllocNotify);

    // IVMRImagePresenter9
    virtual HRESULT STDMETHODCALLTYPE StartPresenting( 
        /* [in] */ DWORD_PTR dwUserID);
    
    virtual HRESULT STDMETHODCALLTYPE StopPresenting( 
        /* [in] */ DWORD_PTR dwUserID);
    
    virtual HRESULT STDMETHODCALLTYPE PresentImage( 
        /* [in] */ DWORD_PTR dwUserID,
        /* [in] */ VMR9PresentationInfo *lpPresInfo);
    
    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        REFIID riid,
        void** ppvObject);

    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

	HRESULT BindGraphic();
	INT32 m_iFixTimes;
	INT32 m_iDeviceStatus;
	DWORD m_iNumPasses;
	void OnMouseMove(INT32 iPannel, UINT nFlags, CPoint point, MOUSE_EVT nEventIdx = MOUSE_MOVE);
	BOOL HadInited(){return m_iCurStatus != STATUS_NO_INITIAL;};
	BOOL HadSurface() {return (m_iCurStatus != STATUS_NO_PREVIEW) && (m_iCurStatus != STATUS_NO_INITIAL); };
	HRESULT InitShareSurface(CString strObjPath = "", DOUBLE dZoomRatio = 1.0f);
	void LoadObjFile(CString strObjPath, DOUBLE dZoomRatio = 1.0f, bool bCheckStatus = false);
	void FreezeDisplay(BOOL bFreeze) 
	{
		if (bFreeze && m_iCurStatus == STATUS_PREVIEWING)
		{
			m_iCurStatus = STATUS_LOADING;
		}
		else if (!bFreeze && m_iCurStatus == STATUS_LOADING)
		{
			m_iCurStatus = STATUS_PREVIEWING;
		}
		m_dwLastTime = 0;

		for (int j = 0; j < MAX_FILM; j++)
		{
			m_pTmpSurfaces[j] = 0;
		//	m_fEffectTime[j] = 0.0;
		}
	};

	INT32 GetObjCount() 
	{
#ifdef PLAY_3D_MODE
		if (m_scene.GetCurStatus() != STATUS_NO_INITIAL)
			return m_scene.GetObjCount();
		else
#endif
			return 0;
	};
	void SetSelectPt(CPoint ptSel, BOOL bNeedLock)
	{
		if (bNeedLock)
			m_iCurStatus = STATUS_BUSYING;
		else
			m_iCurStatus = STATUS_PREVIEWING;
			m_ptSel = ptSel;
	};

	void LoadPtOffset(CString strProjectPath)
	{
#ifdef PLAY_3D_MODE
		if (m_scene.GetCurStatus() != STATUS_NO_INITIAL)
			m_scene.LoadPtOffset(strProjectPath);
#endif
	}

	void SetObjIdx(INT32 iSelObjIdx);
	void DeleteObject(INT32 iDelObjIdx);
	INT32 UndoAction(INT32 iSetIdx = -1)
	{
		#ifdef PLAY_3D_MODE
				if (m_scene.GetCurStatus() != STATUS_NO_INITIAL)
					return m_scene.UndoAction(iSetIdx);
		#endif
	}

	INT32 RedoAction(INT32 iSetIdx = -1)
	{
		#ifdef PLAY_3D_MODE
				if (m_scene.GetCurStatus() != STATUS_NO_INITIAL)
					return m_scene.RedoAction(iSetIdx);
		#endif

	}
	void ResetFromSetting() 
	{ 
		#ifdef PLAY_3D_MODE
			if (m_scene.GetCurStatus() != STATUS_NO_INITIAL)
				m_scene.ResetFromSetting();
		#endif
	};

	void CleanModifyPt()
	{
#ifdef PLAY_3D_MODE
		if (m_scene.GetCurStatus() != STATUS_NO_INITIAL)
			m_scene.CleanSelectPt();
#endif
	}

	void SaveModifyPt(CString strSrcPath, CString strDestPath)
	{
#ifdef PLAY_3D_MODE
		if (m_scene.GetCurStatus() != STATUS_NO_INITIAL)
			m_scene.SaveModifyPt(strSrcPath, strDestPath);
#endif
	}

//	IDirect3DDevice9* GetD3DDevice() { return m_D3DDev;};

	//callback functions
	void DisplayVideoRawData(unsigned char* pBuffer, int w, int h);
	void DisplayVideoSurfaceData(void* pSurface, int w, int h);
	void DisplayNV12SurfaceData(void* pSurface, int w, int h);

	void DisplayVideo_VW(void* pSurface, int w, int h, bool bDoPresent = true);
	void DisplayVideo_VW(int iIndex, void* pSurface, int w, int h);
	HRESULT ShowVideo_VW(bool bDoPresent = true);
	void DisplayVideo_HDEffect(int iIndex, void* pSurface, int w, int h);

	void ApplyMask();

	DisplayManager* GetDisplayManager() { return m_pDisplayManager; };
	void Save2DMapping(CString strFilePath);
	HRESULT Create2DMapping(CString strFilePath);
	HRESULT Create2DMapping(INT32 iGridX, INT32 iGridY);
	HRESULT Create2DMapping();
	void Create2DObject();
	void AddBlendPt(UINT32 iStatus, CPoint pt, UINT32 iArea, BOOL bDarkMask) 
		//iStatus: 1:Start, 2:Finish, 0:None
	{
		BlendingPt bptTemp;
		if (iStatus == 1)
		{
			m_ptAry.RemoveAll();
			m_ptAry.Add(bptTemp);
		}
		else if (iStatus == 0)
		{
			bptTemp.pt = pt;
			bptTemp.iArea = iArea;
			bptTemp.bDarkMask = bDarkMask;
			m_ptAry.Add(bptTemp);
		}		
		else if (iStatus == 2)
		{
			Create2DMapping();
			m_ptAry.RemoveAll();
			m_uCurPtArea = 0;
		}
	}

	void Set2DMapAction(INT32 i2DMapAction, UINT32 iVale = 0);
	void SetBlendSetting(BLEND_SETTING bsBlendSetting);
	void Init2DMapping(INT32 iStaus);
	void Set2DColor(DOUBLE *pColAry, UINT iAreaIdx, CPoint ptDark);
	void Set2DFunction(INT32 iFunIdx, UINT uData);
	void PresetPanelGain(UINT uData) { m_uPanelGain = uData;};
	void SaveObjectSetting(CString strProjectPath = "Shape.xml") 
	{
		if (m_scene.GetCurStatus() != STATUS_NO_INITIAL && m_iIndex == 0)
			m_scene.SaveObjectSetting(strProjectPath);
	};

	void SelectPannel(BOOL bSelected)
	{ 
		if (m_scene.GetCurStatus() != STATUS_NO_INITIAL)
			m_scene.SelectPannel(bSelected);
	};

	void ReleaseSurface(int iIndex);

	int CheckDeviceState();
	void Flip();
	//HRESULT VidoeEffect2();
	void WaitForFinished();
	int GetLastFrameCount();
	void DrawSelArea();
	DWORD GetLastPresentTime();
	void DrawAimingPoint();
	void SetDisplayScale(float fX, float fY);
	void DoEffect();

	void EnableDoClear(bool bEnable);
	void CheckEventStatus();
	void SetSyncPBCtrl(SyncPBCtrl* pObj);
	void ApplySetting();

#ifdef _ENABLE_FLIP_THREAD
	HRESULT ShowVideoLoop();
	bool IsAlive();
	void InitGDI();
	void DeleteGDI();
#endif

	IDirect3D9* GetDirect3D9Ptr();
	IDirect3DDevice9* GetD3DDevice();

#ifdef _ENABLE_D3D9EX
	void Destroy();
#endif

	void CalPresentFPS();
	float GetAvgPresentFPS();

#ifdef _VF_SINGLE_THREAD
	bool IsAlive2();
	HRESULT UpdateVideo();
#endif

#ifdef _TRAIL_VERSION
	void DrawWatermark();
#endif

protected:
    HRESULT CreateDevice();

    // a helper function to erase every surface in the vector
    void DeleteSurfaces();

    bool NeedToHandleDisplayChange();
	static UINT PreviewThread(LPVOID pParam);
	static UINT VidoeEffectThread(LPVOID pParam);
	static UINT TVWallThread(LPVOID pParam);
	HRESULT ShowImageLoop();
	HRESULT TVWallLoop();
	BOOL Is3DEffect(INT32 iEffectIdx);


    // This function is here so we can catch the loss of surfaces.
    // All the functions are using the FAIL_RET macro s;o that they exit
    // with the last error code.  When this returns with the surface lost
    // error code we can restore the surfaces.
    HRESULT PresentHelper(VMR9PresentationInfo *lpPresInfo);
	HRESULT PresentByID(DWORD_PTR dwUserID, VMR9PresentationInfo *lpPresInfo);
	HRESULT CreateBufferTexture(INT32 iWidth = 0, INT32 iHeight = 0, UINT iSelIdx = 0);
	HRESULT CreateTVWallTexture(INT32 iWidth = 0, INT32 iHeight = 0, UINT iSelIdx = 0);

	HRESULT VidoeEffect();
	//void VideoEffect(INT32 iEffectNum, DWORD * pImageData, INT32 iWidth, INT32 iHeight, INT32 iLevel, INT32 iParm1, INT32 iParm2, INT32 iParm3, INT32 iParm4, INT32 iParm5);
	void VideoEffect( int effectID, struct sEffectD3DParam D3DParam,  struct sEffectParam EffectParam );
	HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
	void RemoveFromRot(DWORD pdwRegister);
	int UpdateSurfaceBySample(IDirect3DSurface9 *pVideoSurface, INT32 iObjIdx = 0);
	int UpdateSurfaceBySample(IDirect3DTexture9 *pTexture, INT32 iObjIdx = 0);

private:
    // needed to make this a thread safe object
    //CCritSec    m_ObjectLock;
    HWND        m_window;
    long        m_refCount;
	CMyEffect3D m_ImgEffect3D;
	CMyEffect	m_ImgEffect;
	CRect m_rectClient;
	CPoint m_ptSel;
	CPoint m_ptOffset;
	CRect m_ptAreaSel;
	CPoint m_ptFirstSel;
	INT32 m_iSelObjIdx;
	INT32 m_iGridX;
	INT32 m_iGridY;
	INT32 m_i2DVerNum;
	INT32 m_iBufferAction;//1: Clear Select Array, 2: Modify Grid PT Offset, 3: Modify Color

#ifdef _ENABLE_EVENT_SYN
//	HANDLE m_hEffect1;
	HANDLE m_hEffect2;
	HANDLE m_hEffects[MAX_FILM];
#endif

#ifdef _VF_SINGLE_THREAD
	INT32 m_iThdIdx[MAX_FILM];
	HANDLE m_hEffectThd;
	bool m_bIsAlive2;

	INT32 m_iVideoIdx;
#endif

	//INT32 m_iMediaIdx;

	bool m_bDoClear;
//	int m_iWaitCount;

	double m_dCurPos[MAX_FILM];
	CRITICAL_SECTION m_csEffectInUse;

	CRITICAL_SECTION m_csFunBsy;


	IDirect3DTexture9* m_TextureAry[6];

	INT32 m_iTextureIdx;

	struct CUSTOMVERTEX
	{
		struct Position {
			Position() :
				x(0.0f), y(0.0f), z(0.0f) {
			};
			Position(float x_, float y_, float z_) :
				x(x_), y(y_), z(z_) {
			};
			float x, y, z;
		};

		Position    position; // The position
		D3DXVECTOR3 n;        // Referenced as v3 in the vertex shader
		D3DCOLOR    color;    // The color
		FLOAT       tu, tv;   // The texture coordinates
	};

	struct MAP2DVERTEX
	{
		struct Position {
			Position() :
				x(0.0f), y(0.0f), z(0.0f) {
			};
			Position(float x_, float y_, float z_) :
				x(x_), y(y_), z(z_) {
			};
			float x, y, z;
		};

		Position    position; // The position
		D3DXVECTOR3 n;        // Referenced as v3 in the vertex shader
		D3DCOLOR    color;    // The color
		D3DCOLOR    spcolor;    // The specular color
		FLOAT       tu, tv;   // The texture coordinates
	};

	typedef struct typBlendingPt
	{
		//BlendingPt(CPoint pt_, UINT32 iArea_, BOOL bDarkMask_) :
			//pt(pt_), iArea(iArea_), bDarkMask(bDarkMask_){};

		CPoint pt;
		UINT32 iArea;
		BOOL bDarkMask;
		//INT32 iNearByArea[5];
	}BlendingPt;

	UINT32 m_uCurPtArea;
    

#ifdef _ENABLE_D3D9EX
	IDirect3D9*                     m_pD3D;
	IDirect3DDevice9*               m_pD3DDevice;
	IDirect3D9Ex* m_pD3DEx;
	IDirect3DDevice9Ex* m_pD3DDeviceEx;
	bool m_bIsD3D9Ex;
	D3DPen* m_pD3DPen;
#else
	IDirect3D9*                     m_D3D;
	IDirect3DDevice9*               m_D3DDev;
#endif

    IVMRSurfaceAllocatorNotify9*    m_lpIVMRSurfAllocNotify[FILTER_NUM];
	#ifdef PLAY_3D_MODE	
		vector<IDirect3DSurface9*>     m_surfaces;
	#else
		vector<CComPtr<IDirect3DSurface9> >     m_surfaces;
	#endif
//	vector<IDirect3DSurface9*>     m_surBuffaces;     //2D\AFS\AE\C4 \A5\B2\B6\B7\A6b D3DPOOL_SYSTEMMEM
//	vector<IDirect3DSurface9*>     m_sur3DBuf;        //3D\AFS\AE\C4 \A5\B2\B6\B7\A6b D3DPOOL_MANAGED
	vector<IDirect3DTexture9*>     m_BufTexture;
//	vector<IDirect3DTexture9*>     m_Buf3DTexture;
	CArray<BOOL, BOOL> m_bMediaStatusAry;
	IDirect3DSurface9 *m_renderTarget;
	struct MAP2DVERTEX						*m_vertices;// [DEF_PLANE_W *DEF_PLANE_H * 2 * 3];
	IDirect3DVertexBuffer9                  *m_vertexBuffer;
	BLEND_SETTING m_BlendSetting;

	IDirect3DSurface9* m_pTmpSurfaces[MAX_FILM];

	float m_fEffectTime[MAX_FILM];
	LONGLONG m_lSyncTick;

	D3DXMATRIX m_matEndProj;
	D3DXMATRIX m_matEndView;
	IDirect3DTexture9 *m_EndTexture;
	IDirect3DSurface9 *m_EndSurface;
	vector<IDirect3DSurface9*>  m_pBufSurface;
	CArray<CRect, CRect> m_rectDispAry;
	CArray<INT32, INT32> m_ptSelAry;
	INT32 m_bSelAry[500 * 500*6];
	DOUBLE m_fVectColorAry[500 * 500 * 6][MAX_PTAREA];
	UINT  m_uPanelGain;
	CDC*     m_MemoryDC;
	CRgn*    m_pRgnOverLap[MAX_PTAREA];
	CDC*     m_BKDC;
	CBitmap  m_Bitmap;
	CBitmap  m_BKBitmap;
	INT32 m_i2DMapAction;

	CArray<BlendingPt, BlendingPt> m_ptAry;
	CArray<INT32, INT32> m_IDAry;
	CWinThread* m_pPreviewThread;
	CWinThread* m_pImgEffectThd[MAX_FILM];
	CWinThread* m_pTVWallThd[MAX_PANNEL];
	INT32 m_iCurStatus;
	INT32 m_iCurThdIdx;
	BOOL m_bPreviewFlag;
	CWnd        *m_pParentWnd;
	INT32		m_iEffectIdx[MAX_FILM];
	INT32		m_iEffectStatus[MAX_FILM];
	INT32		m_iTVWallStatus[MAX_PANNEL];

#ifdef PLAY_3D_MODE
    CPlaneScene3D                             m_scene;
#else
	CPlaneScene2D                             m_scene;
#endif


#ifdef _ENABLE_DISPLAY_MANAGER
	CShapeDispManager* m_pDisplayManager;
#endif

	DWORD m_dwLastTime;
	DWORD m_dwLastTime2;
	DWORD m_dwLastDisplayTime;
	DWORD m_dwLastPresentTime;
	//unsigned long m_ulDisplayCount;
	CRITICAL_SECTION m_CriticalSection;
	bool m_bIsDeivceLost;

	long m_lFrameCount[MAX_FILM];

	int m_iPresentInterval;

#ifdef _ENABLE_FLIP_THREAD
	bool m_bIsAlive;
	HANDLE m_hShowVideoThread;
	CRITICAL_SECTION m_SVCriticalSection;

	CPen m_penArea[6];
	CPen m_penDiff;
	CPen m_penRed;
	CPen m_penBlack;
	CPen m_penWhite;
	CClientDC* m_pCDC;
	CBrush m_brhBlack;
	CBrush m_brhWhite;
	CBrush m_brhRed;
#endif

#ifdef _ENABLE_MASK
	IDirect3DTexture9* m_MaskTexture;
	char m_szMaskFileName[512];
	int m_iMaskX;
	int m_iMaskY;
	bool m_bDoMask;

	float m_fMaskScale;

	LPD3DXSPRITE m_pSprite;
	D3DXMATRIX m_ScaleMatrix;
#endif

#ifdef _TRAIL_VERSION
	D3DSprite* m_pD3DSprite;
	IDirect3DTexture9* m_pWatermarkTexture;
	int m_iWatermarkFrameCount;
#endif

	Average m_AvgRenderSpendTime;
	int m_iLastFrameCount;

	DWORD m_dwPresentFPSLastTime;
	int m_iPresentCount;
	float m_fPresentFPS;
	Average m_AvgPresentFPS;

	SyncPBCtrl* m_pSyncPBCtrl;
	bool m_bDoPBSync;

	INT32 m_iIndex;
	IMediaSeeking *g_pMediaSeeking;
	HRESULT MultiInitializeDevice( DWORD_PTR dwUserID, VMR9AllocationInfo *lpAllocInfo, DWORD *lpNumBuffers);
	BOOL SetBrightness(WORD wBrightness);
	//BOOL SetGamma(UINT32 *uGammaTable);
	BOOL SetGamma(DOUBLE dGammaVal);
	
};

#endif // !defined(AFX_ALLOCATOR_H__F675D766_1E57_4269_A4B9_C33FB672B856__INCLUDED_)
