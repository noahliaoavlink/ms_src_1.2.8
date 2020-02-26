//------------------------------------------------------------------------------
// File: Allocator.cpp
//
// Desc: DirectShow sample code - implementation of the CAllocator class
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "Allocator.h"
#include "MediaServer.h"
#include "AreaOptionDlg.h"
#include "ServiceProvider.h"

#pragma comment(lib, "rpcrt4.lib")

vector<CComPtr<IBaseFilter>>   g_BaseFilterAry;
vector<CComPtr<IGraphBuilder>> g_GraphBuildAry;
vector<CComPtr<IMediaControl>> g_MediaControlAry;
vector<CComPtr<IMediaSeeking>> g_MediaSeekingAry;
vector<CComPtr<IVMRMixerControl9>> g_MixCtrlAry;

#ifdef _ENABLE_MEDIA_STREAM_EX
vector<CMediaStreamEx*> g_MediaStreamAry;
#else
vector<CMediaStream*> g_MediaStreamAry;
#endif

//static LONGLONG g_lSyncTick = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

unsigned int __stdcall UpdateVideoThread(void* lpvThreadParm);

//#define MEDIA_STREAM_TEST
//CRITICAL_SECTION m_csEffectInUse;
//CStdioFile sfTemp;
CAllocator::CAllocator(HRESULT& hr, HWND wnd, int iIndex, CRect rectClient, IMediaSeeking *pMS,
	                   IDirect3D9* d3d, IDirect3DDevice9* d3dd, CWnd* pParentWnd)
: m_refCount(1)
#ifdef _ENABLE_D3D9EX
, m_pD3D(d3d)
, m_pD3DDevice(d3dd)
#else
, m_D3D(d3d)
, m_D3DDev(d3dd)
#endif
, m_window( wnd )
, m_iIndex(iIndex)
, m_pParentWnd(pParentWnd)
{
	m_rectClient = rectClient;
    //CAutoLock Lock(&m_ObjectLock);
    hr = E_FAIL;
	g_pMediaSeeking = pMS;
	m_pPreviewThread = NULL;
	m_iCurStatus = STATUS_NO_INITIAL;
	m_iCurThdIdx = 0;
	m_vertexBuffer = NULL;
	m_EndTexture = NULL;
	m_EndSurface = NULL;
	m_bPreviewFlag = FALSE;
	m_vertices = NULL;
	m_iBufferAction = 0;
	m_iTextureIdx = 0;
	m_i2DVerNum = 0;
	m_uCurPtArea = 0;
	m_uPanelGain = 0;

	m_lSyncTick = 0;

	m_pSyncPBCtrl = 0;
	m_bDoPBSync = false;

	m_iPresentInterval = 40;

/*
	m_dwLastPresentTime = 0;
	m_dwPresentFPSLastTime = 0;
	m_iPresentCount = 0;
	m_AvgPresentFPS.SetMax(30);
	*/
#ifdef _ENABLE_DISPLAY_MANAGER
	m_pDisplayManager = new CShapeDispManager;
#endif

	if (IsWindow(wnd) == FALSE)
	{
		hr = E_INVALIDARG;
		return;
	}

#ifdef _ENABLE_D3D9EX
	m_pD3DPen = 0;
	m_bIsD3D9Ex = true;

	if (m_bIsD3D9Ex)
		Direct3DCreate9Ex(D3D_SDK_VERSION, &m_pD3DEx);
	else
		m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();

	//	if (m_iIndex == 0)
	{
		ClipCursor(m_rectClient);
		InitializeCriticalSection(&m_csEffectInUse);
	}

	if (!pD3D9Ptr)
	{
		hr = E_FAIL;
		return;
	}

	if (m_pD3DDevice == NULL)
	{
		hr = CreateDevice();
	}
#else
//	m_bIsD3D9Ex = false;

    if( m_D3D == NULL )
    {
        ASSERT( d3dd ==  NULL ); 

        m_D3D = Direct3DCreate9(D3D_SDK_VERSION);
        if (m_D3D == NULL) {
            hr = E_FAIL;
            return;
        }
    }

    if( m_D3DDev == NULL )
    {
        hr = CreateDevice();
    }
#endif

	CClientDC cdc(m_pParentWnd);
	m_MemoryDC = new CDC();
	m_MemoryDC->CreateCompatibleDC(&cdc);
	m_Bitmap.CreateCompatibleBitmap(&cdc, m_rectClient.Width(), m_rectClient.Height());
	m_MemoryDC->SelectObject(&m_Bitmap);

	m_BKDC = new CDC();
	m_BKDC->CreateCompatibleDC(&cdc);
	m_BKBitmap.CreateCompatibleBitmap(&cdc, m_rectClient.Width(), m_rectClient.Height());
	m_BKDC->SelectObject(&m_BKBitmap);
	
	for (int x = 0; x < MAX_FILM; x++)
	{
		m_pImgEffectThd[x] = NULL;
		m_lFrameCount[x] = 0;
	}

	m_ptAreaSel = CRect(0, 0, 0, 0);
	m_i2DMapAction = MAP2D_NOACT;

	for (int x = 0; x < MAX_PTAREA; x++)
		m_pRgnOverLap[x] = NULL;

	for (int x = 0; x < MAX_PTAREA; x++)
	for (int y = 0; y < 500 * 500 * 6; y++)
	{
		m_fVectColorAry[y][x] = 1.0f;

		if (y == 0)
			m_bSelAry[y] = 0;
	}

	if (m_iIndex == 0)
	{
		//SetBrightness(32);
		

		//SetGamma(1.0f);
		//SetGamma(2.2f);
		//SetGamma(1.0f/1.2);


		//SetBrightness(128);
	}

	m_dwLastDisplayTime = 0;
	m_dwLastTime2 = 0;
	m_iLastFrameCount = 0;
#ifdef _ENABLE_FLIP_THREAD
	InitGDI();

	m_bIsAlive = false;
	m_hShowVideoThread = 0;
	InitializeCriticalSection(&m_SVCriticalSection);
#endif

#ifdef _ENABLE_MASK
	m_MaskTexture = 0;
	m_bDoMask = false;
	char szKeyName[128];
	char szData[128];

	m_iMaskX = 0;
	m_iMaskY = 0;

	m_pSprite = 0;

	if (m_iIndex >= 1)
	{
		bool bEnableMask = GetPrivateProfileInt("Mask", "Enable", 0, theApp.m_strCurPath + "Setting.ini");

		sprintf(szKeyName, "Panel%d_File", m_iIndex);
		GetPrivateProfileString("Mask", szKeyName, "", m_szMaskFileName, 256, theApp.m_strCurPath + "Setting.ini");
		sprintf(szKeyName, "Panel%d_X", m_iIndex);
		m_iMaskX = GetPrivateProfileInt("Mask", szKeyName, 0, theApp.m_strCurPath + "Setting.ini");
		sprintf(szKeyName, "Panel%d_Y", m_iIndex);
		m_iMaskY = GetPrivateProfileInt("Mask", szKeyName, 0, theApp.m_strCurPath + "Setting.ini");

		GetPrivateProfileString("Mask", szKeyName, "1.0", szData, 256, theApp.m_strCurPath + "Setting.ini");
		m_fMaskScale = atof(szData);


		IDirect3DDevice9* pD3DDevice = GetD3DDevice();

		if (PathFileExists(m_szMaskFileName) && bEnableMask)
		{
			hr = D3DXCreateTextureFromFileEx(pD3DDevice, m_szMaskFileName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DCOLOR_ARGB(255, 255, 255, 255), NULL, NULL, &m_MaskTexture);
			if (D3D_OK == hr)
				m_bDoMask = true;
		}

		D3DXCreateSprite(pD3DDevice, &m_pSprite);
	}
#endif

	m_AvgRenderSpendTime.SetMax(100);

	for (int j = 0; j < MAX_FILM; j++)
	{
		m_dCurPos[j] = 0.0;
	}

#ifdef _ENABLE_EVENT_SYN
	char szEventName[256];
	UUID uuid;
/*
	UuidCreate(&uuid);
	wsprintf(szEventName, "effect_thread_event_1_%d", uuid.Data1);
	m_hEffect1 = ::CreateEvent(NULL, TRUE, TRUE, szEventName);
	*/

	for (int j = 0; j < MAX_FILM; j++)
	{
		UuidCreate(&uuid);
		wsprintf(szEventName, "effect_thread_events_%d_%d",j+1, uuid.Data1);
		m_hEffects[j] = ::CreateEvent(NULL, TRUE, TRUE, szEventName);

		::SetEvent(m_hEffects[j]);

		m_pTmpSurfaces[j] = 0;
		m_fEffectTime[j] = 0.0;
	}

	
	UuidCreate(&uuid);
	wsprintf(szEventName, "effect_thread_event_2_%d", uuid.Data1);
	m_hEffect2 = ::CreateEvent(NULL, TRUE, TRUE, szEventName);
	
//	::SetEvent(m_hEffect1);
	::SetEvent(m_hEffect2);
#endif

#ifdef _VF_SINGLE_THREAD
	m_bIsAlive2 = false;
	for (int j = 0; j < MAX_FILM; j++)
		m_iThdIdx[j] = -1;
	m_hEffectThd = 0;
#endif

#ifdef _TRAIL_VERSION
	m_iWatermarkFrameCount = 0;
#endif

	m_bDoClear = false;
//	m_iWaitCount = 0;

	InitializeCriticalSection(&m_csFunBsy);
	InitializeCriticalSection(&m_CriticalSection);
}

CAllocator::~CAllocator()
{
	//sfTemp.Close();

#ifdef _TRAIL_VERSION
	if (m_pD3DSprite)
		delete m_pD3DSprite;

	if (m_pWatermarkTexture)
		m_pWatermarkTexture->Release();
#endif

#ifdef _ENABLE_FLIP_THREAD
	if (m_hShowVideoThread)
	{
		m_bIsAlive = false;
		::WaitForSingleObject(m_hShowVideoThread, 3000);
		m_hShowVideoThread = 0;
	}

	DeleteGDI();
#else
	m_pPreviewThread = NULL;

	while (m_bPreviewFlag)
		Sleep(1);
#endif


#ifdef _VF_SINGLE_THREAD
	if (m_hEffectThd)
	{
		m_bIsAlive2 = false;
		::WaitForSingleObject(m_hEffectThd, 3000);
		m_hEffectThd = 0;
	}

	for (int j = 0; j < MAX_FILM; j++)
		m_iEffectStatus[j] = -1;
#endif

#ifdef _ENABLE_MASK
	if (m_MaskTexture)
		m_MaskTexture->Release();
	m_MaskTexture = 0;

	if (m_pSprite != NULL)
	{
		m_pSprite->Release();
		m_pSprite = 0;
	}
#endif

	CString strFileName;
	strFileName.Format("2DMapping_%i.map", m_iIndex);
	Save2DMapping(theApp.m_strCurPath + "\\TempShape\\" + strFileName);

#ifdef _VF_SINGLE_THREAD
#else
	for (int x = 0; x < min(m_surfaces.size(), MAX_FILM); x++)
	{
		m_pImgEffectThd[x] = NULL;

		while (m_iEffectStatus[x] != -1)
			Sleep(1);
	}
#endif

    DeleteSurfaces();

#ifdef _ENABLE_D3D9EX

	if (m_pD3DPen)
		delete m_pD3DPen;

	Destroy();
#else
	if (m_D3DDev)
		m_D3DDev->Release();

	if (m_D3D)
		m_D3D->Release();
#endif

	if (m_MemoryDC)  delete m_MemoryDC;
	m_Bitmap.DeleteObject();

	if (m_BKDC)  delete m_BKDC;
	m_BKBitmap.DeleteObject();

#ifdef _ENABLE_DISPLAY_MANAGER
	STREM_MAPPING smTemp = theApp.GetStreamMapByIdx(m_iIndex, m_iCurThdIdx);
	g_MediaStreamAry.at(smTemp.iMediaIdx)->SetDisplayManager(0, m_iIndex);

	if (m_pDisplayManager)
	{
		m_pDisplayManager->UsingD3D(0);
		delete m_pDisplayManager;
		m_pDisplayManager = NULL;
	}

	if (m_vertices)
		delete m_vertices;

	if (m_iIndex == 0)
		SetBrightness(128);
	
#endif

#ifdef _ENABLE_EVENT_SYN

	for (int j = 0; j < 10; j++)
	{
		::CloseHandle(m_hEffects[j]);
	}

	//::CloseHandle(m_hEffect1);
	::CloseHandle(m_hEffect2);
#endif

	DeleteCriticalSection(&m_csEffectInUse);

	DeleteCriticalSection(&m_csFunBsy);
	DeleteCriticalSection(&m_CriticalSection);
		   
#ifdef _ENABLE_FLIP_THREAD
	DeleteCriticalSection(&m_SVCriticalSection);
#endif
}

HRESULT CAllocator::CreateDevice()
{
	HRESULT hr;
#ifdef _ENABLE_D3D9EX
	
	//m_D3DDev = NULL;
	D3DDISPLAYMODE dm;

	IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();

	hr = pD3D9Ptr->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);
	D3DPRESENT_PARAMETERS pp;
	ZeroMemory(&pp, sizeof(pp));
	pp.BackBufferWidth = m_rectClient.Width();
	pp.BackBufferHeight = m_rectClient.Height();
	pp.Windowed = TRUE;
	pp.hDeviceWindow = m_window;
	//	pp.SwapEffect = D3DSWAPEFFECT_COPY;
	pp.BackBufferFormat = dm.Format;
#ifdef PLAY_3D_MODE
	pp.EnableAutoDepthStencil = TRUE;//--For3D
	pp.AutoDepthStencilFormat = D3DFMT_D16;//--For3D

	pp.MultiSampleQuality = 0;
	pp.Flags = D3DPRESENTFLAG_VIDEO;// 0;
	pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	//	pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	//For Seperate
	pp.MultiSampleType = D3DMULTISAMPLE_NONE;
	pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // D3DPRESENT_INTERVAL_DEFAULT;//D3DPRESENT_INTERVAL_ONE;
	
	pp.BackBufferFormat = D3DFMT_UNKNOWN;

	if (m_bIsD3D9Ex)
	{
		int iSwapEffect = GetPrivateProfileInt("Media", "SwapEffect", 1, theApp.m_strCurPath + "Setting.ini");
		
		pp.SwapEffect = (D3DSWAPEFFECT)iSwapEffect;// D3DSWAPEFFECT_FLIPEX;// D3DSWAPEFFECT_FLIPEX;
		pp.BackBufferCount = 2;
	}
	else
	{
		pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		pp.BackBufferCount = 1;
	}

#endif

	if (m_bIsD3D9Ex)
	{
		hr = ((IDirect3D9Ex*)pD3D9Ptr)->CreateDeviceEx(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL, m_window,
			D3DCREATE_HARDWARE_VERTEXPROCESSING |
			D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,//D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&pp, NULL, &m_pD3DDeviceEx);
	}
	else
	{
		V(pD3D9Ptr->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			m_window,
			D3DCREATE_HARDWARE_VERTEXPROCESSING |
			D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
			&pp,
			&m_pD3DDevice));
	}

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	m_renderTarget = NULL;
	hr = pD3DDevice->GetRenderTarget(0, &m_renderTarget);
	m_iCurStatus = STATUS_NO_PREVIEW;

	m_scene.SetIndex(m_iIndex);


#ifdef PLAY_3D_MODE
	hr = m_scene.Init(pD3DDevice, m_pParentWnd, theApp.m_strCurPath, m_rectClient.Width(),
		m_rectClient.Height(), "", 1.0f);
#else
	hr = m_scene.Init(m_D3DDev, NULL, m_rectClient.Width(), m_rectClient.Height());
#endif

	D3DCAPS9 d3dcaps;
	pD3DDevice->GetDeviceCaps(&d3dcaps);


	if (d3dcaps.Caps2 & DDCAPS2_PRIMARYGAMMA)
	{
		//m_D3DDev->QueryInterface(IID_IDirectDrawGammaControl, (void**)&m_D3DGamma);
		d3dcaps.Caps2 = d3dcaps.Caps2;
	}

#ifndef NOT_PROC_EFFECT
	m_ImgEffect3D.Create(m_D3DDev);
#endif

#ifdef _ENABLE_DISPLAY_MANAGER
	m_pDisplayManager->Init(m_window, pD3DDevice, m_bIsD3D9Ex);
#endif

	m_pD3DPen = new D3DPen;
	m_pD3DPen->Create(pD3DDevice, 2, D3DCOLOR_ARGB(255, 255, 0, 0));

#else  //_ENABLE_D3D9EX
   
    m_D3DDev = NULL;
    D3DDISPLAYMODE dm;

    hr = m_D3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &dm);
    D3DPRESENT_PARAMETERS pp;
    ZeroMemory(&pp, sizeof(pp));
	pp.BackBufferWidth = m_rectClient.Width();
	pp.BackBufferHeight = m_rectClient.Height();
	pp.Windowed = TRUE;
    pp.hDeviceWindow = m_window;
//	pp.SwapEffect = D3DSWAPEFFECT_COPY;
    pp.BackBufferFormat = dm.Format;
#ifdef PLAY_3D_MODE
	pp.EnableAutoDepthStencil = TRUE;//--For3D
	pp.AutoDepthStencilFormat = D3DFMT_D16;//--For3D

	pp.MultiSampleQuality = 0;
	pp.Flags = D3DPRESENTFLAG_VIDEO;// 0;
	pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
//	pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	//For Seperate
	pp.MultiSampleType = D3DMULTISAMPLE_NONE;
	pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // D3DPRESENT_INTERVAL_DEFAULT;//D3DPRESENT_INTERVAL_ONE;
    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp.BackBufferFormat = D3DFMT_UNKNOWN;
	pp.BackBufferCount = 1;
#endif

    V( m_D3D->CreateDevice(  D3DADAPTER_DEFAULT,
                                    D3DDEVTYPE_HAL,
                                    m_window,
                                    D3DCREATE_HARDWARE_VERTEXPROCESSING | 
                                    D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
                                    &pp,
                                    &m_D3DDev));

    m_renderTarget = NULL;
	hr = m_D3DDev->GetRenderTarget( 0, & m_renderTarget );
	m_iCurStatus = STATUS_NO_PREVIEW;

	m_scene.SetIndex(m_iIndex);


#ifdef PLAY_3D_MODE
	hr = m_scene.Init(m_D3DDev, m_pParentWnd, theApp.m_strCurPath, m_rectClient.Width(), 
					  m_rectClient.Height(), "", 1.0f);
#else
	hr = m_scene.Init(m_D3DDev, NULL, m_rectClient.Width(),m_rectClient.Height());
#endif

	D3DCAPS9 d3dcaps;
	m_D3DDev->GetDeviceCaps(&d3dcaps);


	if (d3dcaps.Caps2 & DDCAPS2_PRIMARYGAMMA)
	{
		//m_D3DDev->QueryInterface(IID_IDirectDrawGammaControl, (void**)&m_D3DGamma);
		d3dcaps.Caps2 = d3dcaps.Caps2;
	}

#ifndef NOT_PROC_EFFECT
	m_ImgEffect3D.Create(m_D3DDev);
#endif

#ifdef _ENABLE_DISPLAY_MANAGER
	m_pDisplayManager->Init(m_window, m_D3DDev);
#endif


#endif    //_ENABLE_D3D9EX

	/*
#ifdef _VF_SINGLE_THREAD
	if (!m_hEffectThd)
	{
		m_bIsAlive2 = true;
		unsigned threadID1;
		m_hEffectThd = (HANDLE)_beginthreadex(NULL, 0, UpdateVideoThread, this, 0, &threadID1);
	}
#endif
*/

#ifdef _TRAIL_VERSION

	D3DXCreateTextureFromFile(pD3DDevice, theApp.m_strCurPath + "UI Folder\\SimpleMixer\\bear_head2.jpg", &m_pWatermarkTexture);
	m_pD3DSprite = new D3DSprite;
	m_pD3DSprite->Create(pD3DDevice);

#endif
	return hr;
}


void CAllocator::DeleteSurfaces()
{
    //CAutoLock Lock(&m_ObjectLock);
    for(UINT i = 0; i < m_surfaces.size(); ++i ) 
    {
        m_surfaces[i] = NULL;

#ifdef _VF_SINGLE_THREAD
		m_iThdIdx[i] = -1;
#endif
    }

	m_surfaces.resize(0);

	m_ImgEffect3D.Destroy();

	if (m_vertexBuffer)
		m_vertexBuffer->Release();
	m_vertexBuffer = NULL;

	if (m_EndTexture)
		m_EndTexture->Release();
	m_EndTexture = NULL;

	if (m_EndSurface)
		m_EndSurface->Release();
	m_EndSurface = NULL;
/*
	for (int x = 0; x < m_surBuffaces.size(); x++)
	{
		m_surBuffaces.at(x)->Release();
		m_surBuffaces.at(x) = NULL;
	}
	m_surBuffaces.resize(0);
	
	for (int x = 0; x < m_sur3DBuf.size(); x++)
	{
		m_sur3DBuf.at(x)->Release();
		m_sur3DBuf.at(x) = NULL;
	}
	m_sur3DBuf.resize(0);
	*/
	for (int x = 0; x < m_BufTexture.size(); x++)
	{
		m_BufTexture.at(x)->Release();
		m_BufTexture.at(x) = NULL;
	}
	m_BufTexture.resize(0);
	m_bMediaStatusAry.RemoveAll();
	/*
	for (int x = 0; x < m_Buf3DTexture.size(); x++)
	{
		m_Buf3DTexture.at(x)->Release();
		m_Buf3DTexture.at(x) = NULL;
	}
	m_Buf3DTexture.resize(0);
	*/
	for (int x = 0; x < m_pBufSurface.size(); x++)
	{
		m_pBufSurface.at(x)->Release();
		m_pBufSurface.at(x) = NULL;
	}
	m_pBufSurface.resize(0);

#ifdef _ENABLE_SYNC_PB_CTRL
	if (m_pSyncPBCtrl)
		m_pSyncPBCtrl->SetSourceCount(m_iIndex, m_surfaces.size());
#endif //_ENABLE_SYNC_PB_CTRL

}

void CAllocator::ReleaseSurface(int iIndex)
{
	if (iIndex == -1)
	{
		int i;
		for (i = 0; i < m_BufTexture.size(); i++)
		{
			if (m_BufTexture.at(i))
			{
				m_BufTexture.at(i)->Release();
				m_BufTexture.at(i) = NULL;
			}
		}
		/*
		for (i = 0; i < m_Buf3DTexture.size(); i++)
		{
			if (m_Buf3DTexture.at(i))
			{
				m_Buf3DTexture.at(i)->Release();
				m_Buf3DTexture.at(i) = NULL;
			}
		}
		
		for (i = 0; i < m_surBuffaces.size(); i++)
		{
			if (m_surBuffaces.at(i))
			{
				m_surBuffaces.at(i)->Release();
				m_surBuffaces.at(i) = NULL;
			}
		}
		
		for (i = 0; i < m_sur3DBuf.size(); i++)
		{
			if (m_sur3DBuf.at(i))
			{
				m_sur3DBuf.at(i)->Release();
				m_sur3DBuf.at(i) = NULL;
			}
		}
		*/
	}
	else
	{
		if (m_BufTexture.size() > 0 && m_BufTexture.at(iIndex))
		{
			m_BufTexture.at(iIndex)->Release();
			m_BufTexture.at(iIndex) = NULL;
		}
		/*
		if (m_Buf3DTexture.size() > 0 && m_Buf3DTexture.at(iIndex))
		{
			m_Buf3DTexture.at(iIndex)->Release();
			m_Buf3DTexture.at(iIndex) = NULL;
		}
		
		if (m_surBuffaces.size() > 0 && m_surBuffaces.at(iIndex))
		{
			m_surBuffaces.at(iIndex)->Release();
			m_surBuffaces.at(iIndex) = NULL;
		}
		
		if (m_sur3DBuf.size() > 0 && m_sur3DBuf.at(iIndex))
		{
			m_sur3DBuf.at(iIndex)->Release();
			m_sur3DBuf.at(iIndex) = NULL;
		}
		*/
	}
}

HRESULT CAllocator::InitShareSurface(CString strObjPath, DOUBLE dZoomRatio)
{
	HRESULT hr = S_OK;

	INT32 iWidth = 0;
	INT32 iHeight = 0;

	STREM_MAPPING smTemp = theApp.GetStreamMapByIdx(m_iIndex, 0);
	iWidth = smTemp.iWidth;
	iHeight = smTemp.iHeight;

	if (iWidth == 0)
	{
		iWidth = m_rectClient.Width();
		iHeight = m_rectClient.Height();
	}


	/*m_scene.SetIndex(m_iIndex);

	#ifdef PLAY_3D_MODE
		if (m_iIndex == 0)
		{
			STREM_MAPPING smTemp = g_StreamMappingAry.GetAt(g_StreamMappingAry.GetCount() - 1);
			hr = m_scene.Init(m_D3DDev, m_pParentWnd, theApp.m_strCurPath, iWidth, iHeight, 
				              strObjPath, dZoomRatio, smTemp.iPanelIdx);
		}
		else
			hr = m_scene.Init(m_D3DDev, m_pParentWnd, theApp.m_strCurPath, iWidth, iHeight, 
				              strObjPath, dZoomRatio);
	#else
		hr = m_scene.Init(m_D3DDev, NULL, iWidth, iHeight);
	#endif*/

	m_bPreviewFlag = TRUE;
	LoadObjFile(strObjPath, dZoomRatio);

	if (m_iIndex != 0 || g_PannelAry.GetSize() == 1)
	{
		m_surfaces.resize(1);
		CreateBufferTexture(iWidth, iHeight);
		CreateTVWallTexture(iWidth, iHeight);

#ifdef _ENABLE_FLIP_THREAD
		if (!m_hShowVideoThread)
		{
#ifdef _ENABLE_VIDEO_WALL
			//for debug
			if (m_iIndex == 0)
			{
				m_bIsAlive = true;
				m_hShowVideoThread = AfxBeginThread(PreviewThread, this);
			}
#else
			m_bIsAlive = true;
			m_hShowVideoThread = AfxBeginThread(PreviewThread, this);
#endif
		}
#else
		if (m_pPreviewThread == NULL)
		{
			/*for (UINT x = 0; x < min(m_surfaces.size(), MAX_FILM); x++)
			{
				m_iCurThdIdx = x;
				m_iEffectStatus[x] = -1;
				m_pImgEffectThd[x] = AfxBeginThread(VidoeEffectThread, this, THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED);
				SetProcessAffinityMask(m_pImgEffectThd[x], x);

				m_pImgEffectThd[x]->ResumeThread();

				while (m_iEffectStatus[x] != 0)
					Sleep(1);
			} */
			m_pPreviewThread = AfxBeginThread(PreviewThread, this);
		}
#endif
		m_iCurStatus = STATUS_PREVIEWING;
	}
	else
	{
#ifdef _ENABLE_FLIP_THREAD
		if (!m_hShowVideoThread)
		{
#ifdef _ENABLE_VIDEO_WALL
			//for debug
			if (m_iIndex == 0)
			{
				m_bIsAlive = true;
				m_iCurStatus = STATUS_PREVIEWING;
				m_hShowVideoThread = AfxBeginThread(PreviewThread, this);
			}
			else
			{
				//m_bIsAlive = true;
				m_iCurStatus = STATUS_PREVIEWING;
				//m_hShowVideoThread = AfxBeginThread(PreviewThread, this);
			}
#else
			m_bIsAlive = true;
			m_iCurStatus = STATUS_PREVIEWING;
			m_hShowVideoThread = AfxBeginThread(PreviewThread, this);
#endif
		}
#else
		if (m_pPreviewThread == NULL)
		{
			m_iCurStatus = STATUS_PREVIEWING;
			m_pPreviewThread = AfxBeginThread(PreviewThread, this);
		}
#endif
	}

	return hr;
}

void CAllocator::LoadObjFile(CString strObjPath, DOUBLE dZoomRatio,bool bCheckStatus)
{
	if (m_iIndex != 0 || g_PannelAry.GetSize() == 1)
	{
		m_iCurStatus = STATUS_LOADING;
		UINT iWidth, iHeight;
		INT32 iCurIdx = m_surfaces.size();

		STREM_MAPPING smTemp = theApp.GetStreamMapByIdx(m_iIndex, iCurIdx);
		iWidth = smTemp.iWidth;
		iHeight = smTemp.iHeight;

		D3DDISPLAYMODE dm;
		D3DSURFACE_DESC rectSurface;

#ifdef _ENABLE_D3D9EX
		IDirect3DDevice9* pD3DDevice = GetD3DDevice();
		pD3DDevice->GetDisplayMode(NULL, &dm);
#else
		m_D3DDev->GetDisplayMode(NULL, &dm);
#endif

		//#ifdef MULTI_STREAM
		#if 0
		g_MediaStreamAry[smTemp.iMediaIdx]->SetVideoRawDataCallback(this);
		g_MediaStreamAry[smTemp.iMediaIdx]->CreateMixEffect(m_D3DDev, dm.Format, 1920, 1080);
		g_MediaStreamAry[smTemp.iMediaIdx]->EnableGPU(true, m_D3DDev);
		#endif

		m_surfaces.resize(iCurIdx + 1);
		CreateBufferTexture(iWidth, iHeight, iCurIdx);
		CreateTVWallTexture(iWidth, iHeight, iCurIdx);

		m_iCurThdIdx = iCurIdx;

		m_lFrameCount[m_iCurThdIdx] = 0;

		m_iEffectStatus[iCurIdx] = -1;

#ifdef _VF_SINGLE_THREAD
		m_iThdIdx[iCurIdx] = iCurIdx;
		//m_iEffectStatus[iCurIdx] = 0;

		if (!m_hEffectThd)
		{
			m_bIsAlive2 = true;
			unsigned threadID1;
			m_hEffectThd = (HANDLE)_beginthreadex(NULL, 0, UpdateVideoThread, this, 0, &threadID1);
	}


#else
		m_pImgEffectThd[iCurIdx] = AfxBeginThread(VidoeEffectThread, this, THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED);
		SetProcessAffinityMask(m_pImgEffectThd[iCurIdx], iCurIdx);

		m_pImgEffectThd[iCurIdx]->ResumeThread();
#endif
		


#ifdef _ENABLE_FLIP_THREAD

#ifdef _ENABLE_VIDEO_WALL
			m_iCurStatus = STATUS_PREVIEWING;
#else
		if (m_hShowVideoThread)
			m_iCurStatus = STATUS_PREVIEWING;
		else
			m_iCurStatus = STATUS_NO_PREVIEW;
#endif


#else
		if (m_pPreviewThread)
			m_iCurStatus = STATUS_PREVIEWING;
		else
			m_iCurStatus = STATUS_NO_PREVIEW;
#endif


#ifdef _VF_SINGLE_THREAD
#else
		while (m_iEffectStatus[iCurIdx] != 0)
			Sleep(1);
#endif

#ifdef _ENABLE_DISPLAY_MANAGER
		INT32 iMediaIdx = -1;
		smTemp = theApp.GetStreamMapByIdx(m_iIndex, m_iCurThdIdx);
		while (m_pDisplayManager->IsD3DUsing() != 0)
			Sleep(1);
		m_pDisplayManager->UsingD3D(2);
		bool bRet = g_MediaStreamAry.at(smTemp.iMediaIdx)->SetDisplayManager(m_pDisplayManager, m_iIndex);
		if(!bRet)
			g_MediaStreamAry.at(smTemp.iMediaIdx)->CreateDispMixer(m_pDisplayManager->GetD3DDevice(), m_pDisplayManager->GetDesktopD3DFormat(), 1920, 1080);
		//g_MediaStreamAry.at(smTemp.iMediaIdx)->CreateDispMixer(m_pDisplayManager->GetD3DDevice(), m_pDisplayManager->GetDesktopD3DFormat(), 1920, 1080, m_iIndex);
		m_pDisplayManager->UsingD3D(0);
#endif

#ifdef _ENABLE_SYNC_PB_CTRL
		if (m_pSyncPBCtrl)
			m_pSyncPBCtrl->SetSourceCount(m_iIndex,m_surfaces.size());
#endif //_ENABLE_SYNC_PB_CTRL

	}
	#ifdef PLAY_3D_MODE

	if (bCheckStatus)
	{
		WaitForFinished();
		m_pDisplayManager->SetRenderStatus(RS_WAIT);
	}

//		EnterCriticalSection(&m_csEffectInUse);
		if (m_iCurStatus != STATUS_NO_INITIAL)
		{
			if (m_iIndex == 0)
			{
				//STREM_MAPPING smTemp = g_StreamMappingAry.GetAt(g_StreamMappingAry.GetCount() - 1);
				//m_scene.LoadObj(strObjPath, dZoomRatio, smTemp.iPanelIdx);
				m_scene.LoadObj(strObjPath, dZoomRatio, g_PannelSetting.iSelIdx);
			}
			else
				m_scene.LoadObj(strObjPath, dZoomRatio);
		}

	if (bCheckStatus)		
		m_pDisplayManager->SetRenderStatus(RS_NORMAL);

//		LeaveCriticalSection(&m_csEffectInUse);
	#endif
}

HRESULT CAllocator::MultiInitializeDevice( 
            /* [in] */ DWORD_PTR dwUserID,
            /* [in] */ VMR9AllocationInfo *lpAllocInfo,
            /* [out][in] */ DWORD *lpNumBuffers)
{
	HRESULT hr = S_OK;
	D3DCAPS9 d3dcaps;
    DWORD dwWidth = 1;
    DWORD dwHeight = 1;
    float fTU = 1.f;
    float fTV = 1.f;

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	pD3DDevice->GetDeviceCaps(&d3dcaps);

    if( d3dcaps.TextureCaps & D3DPTEXTURECAPS_POW2 )
    {
        while( dwWidth < lpAllocInfo->dwWidth )
            dwWidth = dwWidth << 1;
        while( dwHeight < lpAllocInfo->dwHeight )
            dwHeight = dwHeight << 1;

        fTU = (float)(lpAllocInfo->dwWidth) / (float)(dwWidth);
        fTV = (float)(lpAllocInfo->dwHeight) / (float)(dwHeight);
        m_scene.SetSrcRect( fTU, fTV );
        lpAllocInfo->dwWidth = dwWidth;
        lpAllocInfo->dwHeight = dwHeight;
    }

    // NOTE:
    // we need to make sure that we create textures because
    // surfaces can not be textured onto a primitive.
    lpAllocInfo->dwFlags |= VMR9AllocFlag_TextureSurface;
	
	INT32 iCount = INT32(m_surfaces.size()) + 1;

	
	if (m_surfaces.size() == m_IDAry.GetCount())
		return S_OK;

	//*lpNumBuffers = iCount;
	m_surfaces.resize(iCount);

	IVMRSurfaceAllocatorNotify9 *lpIVMRSurfAllocNotify = NULL;
	V( g_BaseFilterAry.at(iCount - 1)->QueryInterface(IID_IVMRSurfaceAllocatorNotify9, reinterpret_cast<void**>(&lpIVMRSurfAllocNotify)));

	hr = lpIVMRSurfAllocNotify->AllocateSurfaceHelper(lpAllocInfo, lpNumBuffers, & m_surfaces.at(iCount - 1) );
    

	if (iCount == 1)
	{
#ifdef PLAY_3D_MODE
		hr = m_scene.Init(pD3DDevice, m_pParentWnd, theApp.m_strCurPath, m_rectClient.Width(), m_rectClient.Height());
#else
			hr = m_scene.Init(m_D3DDev, g_pMediaSeeking, m_rectClient.Width(), m_rectClient.Height());
#endif
		m_scene.SetIndex(m_iIndex);	
	}

	if (m_surfaces.size() == m_IDAry.GetCount())
	{
		g_MediaControlAry.resize(m_surfaces.size());
		g_MediaSeekingAry.resize(m_surfaces.size());

		IMediaControl *pMediaControl = NULL;
		IMediaSeeking *pMediaSeeking = NULL;
		DWORD dCanSeek=0;
		CString strTemp;

		for (UINT x = 0; x < m_surfaces.size(); x++)
		{
			V(g_GraphBuildAry.at(x)->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&pMediaControl)));
			g_MediaControlAry.at(x) = pMediaControl;
			pMediaControl = NULL;

			V(g_GraphBuildAry.at(x)->QueryInterface(IID_IMediaSeeking, reinterpret_cast<void**>(&pMediaSeeking)));

			pMediaSeeking->GetCapabilities(&dCanSeek);
			
			if (1)//dCanSeek & 0x1)
			{
				g_MediaSeekingAry.at(x) = pMediaSeeking;
			}
			else
				g_MediaSeekingAry.at(x) = NULL;
			
			pMediaSeeking = NULL;
		}

		CreateBufferTexture();
		   
#ifdef _ENABLE_FLIP_THREAD
		if (!m_hShowVideoThread)
		{
			for (UINT x = 0; x < min(m_surfaces.size(), MAX_FILM); x++)
			{
				m_iCurThdIdx = x;
				m_iEffectStatus[x] = -1;

#ifdef _VF_SINGLE_THREAD
				m_iThdIdx[x] = x;
#else
				m_pImgEffectThd[x] = AfxBeginThread(VidoeEffectThread, this, THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED);
				SetProcessAffinityMask(m_pImgEffectThd[x], x);

				m_pImgEffectThd[x]->ResumeThread();
#endif

				while (m_iEffectStatus[x] != 0)
					Sleep(1);
			}

			m_iCurStatus = STATUS_PREVIEWING;
#ifdef _ENABLE_VIDEO_WALL
#else
			m_bIsAlive = true;
			m_hShowVideoThread = AfxBeginThread(PreviewThread, this);
#endif
		}
#else
		if (m_pPreviewThread == NULL)
		{
			for (UINT x = 0; x < min(m_surfaces.size(), MAX_FILM); x++)
			{
				m_iCurThdIdx = x;
				m_iEffectStatus[x] = -1;
				m_pImgEffectThd[x] = AfxBeginThread(VidoeEffectThread, this, THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED);
				SetProcessAffinityMask(m_pImgEffectThd[x], x);

				m_pImgEffectThd[x]->ResumeThread();

				while (m_iEffectStatus[x] != 0)
					Sleep(1);
			}

			m_iCurStatus = STATUS_PREVIEWING;
			m_pPreviewThread = AfxBeginThread(PreviewThread, this);
		}
#endif
	}

	return hr;
}


//IVMRSurfaceAllocator9
HRESULT CAllocator::InitializeDevice( 
            /* [in] */ DWORD_PTR dwUserID,
            /* [in] */ VMR9AllocationInfo *lpAllocInfo,
            /* [out][in] */ DWORD *lpNumBuffers)
{
    D3DCAPS9 d3dcaps;
    DWORD dwWidth = 1;
    DWORD dwHeight = 1;
    float fTU = 1.f;
    float fTV = 1.f;

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	m_ImgEffect3D.Create(pD3DDevice);

    if( lpNumBuffers == NULL )
    {
        return E_POINTER;
    }

	if( g_BaseFilterAry.size() > 0)
	{
		return MultiInitializeDevice(dwUserID, lpAllocInfo, lpNumBuffers);
	}

	//sfTemp.Open("D:\\Allocator Log.csv", CFile::modeCreate | CFile::modeReadWrite);

    if( m_lpIVMRSurfAllocNotify[0] == NULL )
    {
        return E_FAIL;
    }

    HRESULT hr = S_OK;

	pD3DDevice->GetDeviceCaps( &d3dcaps );
    if( d3dcaps.TextureCaps & D3DPTEXTURECAPS_POW2 )
    {
        while( dwWidth < lpAllocInfo->dwWidth )
            dwWidth = dwWidth << 1;
        while( dwHeight < lpAllocInfo->dwHeight )
            dwHeight = dwHeight << 1;

        fTU = (float)(lpAllocInfo->dwWidth) / (float)(dwWidth);
        fTV = (float)(lpAllocInfo->dwHeight) / (float)(dwHeight);
        m_scene.SetSrcRect( fTU, fTV );
        lpAllocInfo->dwWidth = dwWidth;
        lpAllocInfo->dwHeight = dwHeight;
    }

    // NOTE:
    // we need to make sure that we create textures because
    // surfaces can not be textured onto a primitive.
    lpAllocInfo->dwFlags |= VMR9AllocFlag_TextureSurface;

	
	if (FILTER_NUM == 1 || m_surfaces.size() == 0 || !m_lpIVMRSurfAllocNotify[m_surfaces.size()])
	{
		DeleteSurfaces();
		m_surfaces.resize(*lpNumBuffers);
		//V(m_D3DDev->CreateOffscreenPlainSurface(688, 384, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_surfaces.at(0), NULL));
		hr = m_lpIVMRSurfAllocNotify[0]->AllocateSurfaceHelper(lpAllocInfo, lpNumBuffers, & m_surfaces.at(0) );
	}
	else if (m_surfaces.size() < FILTER_NUM)
	{
		*lpNumBuffers = 2;
		m_surfaces.resize(2);
		hr = m_lpIVMRSurfAllocNotify[1]->AllocateSurfaceHelper(lpAllocInfo, lpNumBuffers, & m_surfaces.at(1) );
	}
    
	CRect rect;
	GetClientRect(m_window, &rect);

	#ifdef PLAY_3D_MODE
		hr = m_scene.Init(pD3DDevice, m_pParentWnd, theApp.m_strCurPath, abs(rect.right - rect.left), abs(rect.top - rect.bottom));
	#else
		hr = m_scene.Init(m_D3DDev, g_pMediaSeeking, abs(rect.right - rect.left), abs(rect.top - rect.bottom));
	#endif

	m_scene.SetIndex(m_iIndex);
	
	if (m_BufTexture.size() == 0)
	{
		CreateBufferTexture();
	}

    return hr;
}
            
HRESULT CAllocator::TerminateDevice( 
        /* [in] */ DWORD_PTR dwID)
{
	if (m_IDAry.GetCount() >0)
	{

	}
	else
		DeleteSurfaces();

    return S_OK;
}
    
HRESULT CAllocator::GetSurface( 
        /* [in] */ DWORD_PTR dwUserID,
        /* [in] */ DWORD SurfaceIndex,
        /* [in] */ DWORD SurfaceFlags,
        /* [out] */ IDirect3DSurface9 **lplpSurface)
{
    if( lplpSurface == NULL )
    {
        return E_POINTER;
    }

	if (m_IDAry.GetCount() > 0)
		SurfaceIndex = dwUserID - 0xff00;

    if (SurfaceIndex >= m_surfaces.size() ) 
    {
        return E_FAIL;
    }

    //CAutoLock Lock(&m_ObjectLock);
#ifndef PLAY_3D_MODE
	m_surfaces[SurfaceIndex].CopyTo(lplpSurface);
#endif
	(*lplpSurface)->AddRef();

	return S_OK;
}

HRESULT CAllocator::BindGraphic()
{
	//CAutoLock Lock(&m_ObjectLock);

    HRESULT hr;

	IVMRSurfaceAllocatorNotify9 *lpIVMRSurfAllocNotify;
	DWORD dwRegister;

	IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	for (UINT x = 0; x < g_BaseFilterAry.size(); x++)
	{
		lpIVMRSurfAllocNotify = NULL;
		V( g_BaseFilterAry.at(x)->QueryInterface(IID_IVMRSurfaceAllocatorNotify9, reinterpret_cast<void**>(&lpIVMRSurfAllocNotify)));
		V( lpIVMRSurfAllocNotify->AdviseSurfaceAllocator( 0xff00 + m_iIndex, this));

		HMONITOR hMonitor = pD3D9Ptr->GetAdapterMonitor( D3DADAPTER_DEFAULT );
		V( lpIVMRSurfAllocNotify->SetD3DDevice(pD3DDevice, hMonitor ) );

		m_IDAry.Add(0xff00 + x);

		AddToRot(g_GraphBuildAry.at(x), &dwRegister);
	}

	return hr;

}
    
HRESULT CAllocator::AdviseNotify( 
        /* [in] */ IVMRSurfaceAllocatorNotify9 *lpIVMRSurfAllocNotify)
{
    //CAutoLock Lock(&m_ObjectLock);

    HRESULT hr = S_OK;

	if (g_BaseFilterAry.size() > 0)
		return hr;

	IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

    m_lpIVMRSurfAllocNotify[m_surfaces.size()] = lpIVMRSurfAllocNotify;

    HMONITOR hMonitor = pD3D9Ptr->GetAdapterMonitor( D3DADAPTER_DEFAULT );
    V( m_lpIVMRSurfAllocNotify[m_surfaces.size()]->SetD3DDevice(pD3DDevice, hMonitor ) );

    return hr;
}

HRESULT CAllocator::StartPresenting( 
    /* [in] */ DWORD_PTR dwUserID)
{
    //CAutoLock Lock(&m_ObjectLock);

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

    ASSERT(pD3DDevice);
    if(pD3DDevice == NULL )
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CAllocator::StopPresenting( 
    /* [in] */ DWORD_PTR dwUserID)
{
    return S_OK;
}

HRESULT CAllocator::PresentImage( 
    /* [in] */ DWORD_PTR dwUserID,
    /* [in] */ VMR9PresentationInfo *lpPresInfo)
{
    HRESULT hr;
    //CAutoLock Lock(&m_ObjectLock);

	IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

    // if we are in the middle of the display change
    if( NeedToHandleDisplayChange() )
    {
        // NOTE: this piece of code is left as a user exercise.  
        // The D3DDevice here needs to be switched
        // to the device that is using another adapter
    }

	if (m_IDAry.GetCount() > 0)
		hr = PresentByID(dwUserID, lpPresInfo);
	else
		hr = PresentHelper( lpPresInfo );

    // IMPORTANT: device can be lost when user changes the resolution
    // or when (s)he presses Ctrl + Alt + Delete.
    // We need to restore our video memory after that
    if( hr == D3DERR_DEVICELOST)
    {
        if (pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        {
            DeleteSurfaces();
            V( CreateDevice() );

            HMONITOR hMonitor = pD3D9Ptr->GetAdapterMonitor( D3DADAPTER_DEFAULT );

			for (int x = 0; x < FILTER_NUM; x++)
				V( m_lpIVMRSurfAllocNotify[x]->ChangeD3DDevice(pD3DDevice, hMonitor ) );
        }

        hr = S_OK;
    }

    return hr;
}

UINT CAllocator::VidoeEffectThread(LPVOID pParam)
{
	INT32* iThdNum = reinterpret_cast<INT32*>(pParam);


	CAllocator *pThis = reinterpret_cast<CAllocator *>(pParam);

	pThis->VidoeEffect();
	//pThis->VidoeEffect2();
	return 1;
}

UINT CAllocator::PreviewThread(LPVOID pParam)
{
	CAllocator *pThis = reinterpret_cast<CAllocator *>(pParam);

#ifdef _ENABLE_FLIP_THREAD
	while (pThis->IsAlive())
	{
		pThis->ShowVideoLoop();
		Sleep(1);
	}
#else
	pThis->ShowImageLoop();
#endif
	return 1;
}

UINT CAllocator::TVWallThread(LPVOID pParam)
{
	CAllocator *pThis = reinterpret_cast<CAllocator *>(pParam);
	pThis->TVWallLoop();
	return 1;
}

HRESULT CAllocator::TVWallLoop()
{
	HRESULT hr = S_OK;
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	INT32 iVideoIdx = m_iCurThdIdx;
	CRect rectSrc, rectDest;
	rectSrc = CRect(0, 0, g_PannelSetting.rectSurface.Width / g_PannelSetting.iTVNumH,
		g_PannelSetting.rectSurface.Height / g_PannelSetting.iTVNumV);

	rectSrc.OffsetRect(rectSrc.Width()*(iVideoIdx%g_PannelSetting.iTVNumH), 
		               rectSrc.Height()*INT32(iVideoIdx/g_PannelSetting.iTVNumH));

	rectDest = CRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

	rectDest.OffsetRect(rectDest.Width()*(iVideoIdx%g_PannelSetting.iTVNumH),
		                rectDest.Height()*INT32(iVideoIdx / g_PannelSetting.iTVNumH));

	do
	{
		m_iTVWallStatus[iVideoIdx] = 0;
		while (m_pTVWallThd[iVideoIdx] && m_iTVWallStatus[iVideoIdx] == 0)
		{
			Sleep(1);
		}
		//V(m_D3DDev->Present(rectSrc, rectDest, NULL, NULL));
		V(pD3DDevice->Present(NULL, rectDest, NULL, NULL));

	} while (m_pTVWallThd[iVideoIdx]);

	return hr;
}

HRESULT CAllocator::CreateTVWallTexture(INT32 iWidth, INT32 iHeight, UINT iSelIdx)
{
	HRESULT hr;
	D3DDISPLAYMODE dm;
	CRect rectArea;

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	V(pD3DDevice->GetDisplayMode(NULL, &dm));
	
	//rectArea = CRect(0, 0, g_PannelSetting.rectSurface.Width  / g_PannelSetting.iTVNumH,
	//	                   g_PannelSetting.rectSurface.Height / g_PannelSetting.iTVNumV);

	
	rectArea = CRect(0, 0, iWidth, iHeight);
	
	m_pBufSurface.resize(iSelIdx + 1);
	V(pD3DDevice->CreateOffscreenPlainSurface(rectArea.Width(), rectArea.Height(), dm.Format, D3DPOOL_DEFAULT, &m_pBufSurface.at(iSelIdx), NULL));
	//Need to Debug

	if (g_PannelSetting.iTVWallMode != TVWALL_DISABLE)
	{
		rectArea.OffsetRect((m_iIndex % g_PannelSetting.iTVNumH) * g_PannelSetting.rectSurface.Width / g_PannelSetting.iTVNumH,
			           INT32(m_iIndex / g_PannelSetting.iTVNumH) * g_PannelSetting.rectSurface.Height / g_PannelSetting.iTVNumV);
	}

	if (iSelIdx > m_rectDispAry.GetCount() - 1 || iSelIdx == 0)
		m_rectDispAry.Add(rectArea);
	else
		m_rectDispAry.SetAt(iSelIdx, rectArea);
		
	return hr;
}

HRESULT CAllocator::CreateBufferTexture(INT32 iWidth, INT32 iHeight, UINT iSelIdx)
{
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	ASSERT(pD3DDevice);
	HRESULT hr;
	D3DDISPLAYMODE dm;
	D3DSURFACE_DESC rectSurface;

	V(pD3DDevice->GetDisplayMode(NULL,  &dm));
	m_BufTexture.resize(m_surfaces.size());
//	m_Buf3DTexture.resize(m_surfaces.size());

	m_bMediaStatusAry.Add(FALSE);

	//m_lFrameCount[iSelIdx] = 0;

//	m_surBuffaces.resize(m_surfaces.size());
//	m_sur3DBuf.resize(m_surfaces.size());

	void* pData=NULL;
		
	if (iSelIdx > 0)
	{
		if (iWidth == 0)
			m_surfaces.at(iSelIdx)->GetDesc(&rectSurface);
		else
		{
			rectSurface.Width = iWidth;
			rectSurface.Height = iHeight;
		}

		//V(pD3DDevice->CreateTexture(rectSurface.Width, rectSurface.Height, 1, D3DUSAGE_RENDERTARGET, dm.Format, D3DPOOL_DEFAULT, &m_BufTexture.at(iSelIdx), NULL));
		V(pD3DDevice->CreateTexture(rectSurface.Width, rectSurface.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &m_BufTexture.at(iSelIdx), NULL));

		//V(m_D3DDev->CreateTexture(rectSurface.Width, rectSurface.Height, 1, D3DUSAGE_RENDERTARGET, dm.Format, D3DPOOL_DEFAULT, &m_Buf3DTexture.at(iSelIdx), NULL));
		/*
		hr = pD3DDevice->CreateTexture(rectSurface.Width, rectSurface.Height, 1, D3DUSAGE_RENDERTARGET, dm.Format, D3DPOOL_DEFAULT, &m_Buf3DTexture.at(iSelIdx), NULL);
		if (FAILED(hr))
		{
			char szMsg[512];
			sprintf_s(szMsg, "#Allocator# CreateTexture failed!! \n");
			OutputDebugStringA(szMsg);
		}
		*/
//		V(pD3DDevice->CreateOffscreenPlainSurface(rectSurface.Width, rectSurface.Height, dm.Format, D3DPOOL_SYSTEMMEM, &m_surBuffaces.at(iSelIdx), NULL));
//		V(pD3DDevice->CreateDepthStencilSurface(rectSurface.Width, rectSurface.Height, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_sur3DBuf.at(iSelIdx), NULL));
	}
	else
	{
		for (UINT x = 0; x < m_surfaces.size(); x++)
		{
			if (iWidth == 0)
				m_surfaces.at(x)->GetDesc(&rectSurface);
			else
			{
				rectSurface.Width = iWidth;
				rectSurface.Height = iHeight;
			}

			//V(pD3DDevice->CreateTexture(rectSurface.Width, rectSurface.Height, 1, D3DUSAGE_RENDERTARGET, dm.Format, D3DPOOL_DEFAULT, &m_BufTexture.at(x), NULL));
			V(pD3DDevice->CreateTexture(rectSurface.Width, rectSurface.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &m_BufTexture.at(x), NULL));


//			V(pD3DDevice->CreateTexture(rectSurface.Width, rectSurface.Height, 1, D3DUSAGE_RENDERTARGET, dm.Format, D3DPOOL_DEFAULT, &m_Buf3DTexture.at(x), NULL));
//			V(pD3DDevice->CreateOffscreenPlainSurface(rectSurface.Width, rectSurface.Height, dm.Format, D3DPOOL_SYSTEMMEM, &m_surBuffaces.at(x), NULL));
//			V(pD3DDevice->CreateDepthStencilSurface(rectSurface.Width, rectSurface.Height, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_sur3DBuf.at(x), NULL));
		}
	}
	
	if (m_EndTexture == NULL)
	{
		RECT rect;
		GetClientRect(m_window, &rect);
		//rect = CRect(0, 0, rectSurface.Width, rectSurface.Height);
		double fRatio = fabs(rect.right - rect.left) / fabs(rect.bottom - rect.top);

		//Create2DMapping(150, 50);
		
		V(pD3DDevice->CreateTexture(abs(rect.right - rect.left), abs(rect.bottom - rect.top), 1,
		  D3DUSAGE_RENDERTARGET, dm.Format, D3DPOOL_DEFAULT, &m_EndTexture, NULL));

		V(pD3DDevice->CreateDepthStencilSurface(abs(rect.right - rect.left), abs(rect.bottom - rect.top),
			D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_EndSurface, NULL));
		
		D3DXMatrixPerspectiveFovLH(&m_matEndProj, D3DX_PI / 4.89f, fRatio, 1.0f, 5.0f);
		//V(m_D3DDev->SetTransform(D3DTS_PROJECTION, &matProj));
		
		D3DXVECTOR3 from(0.0f, 0.0f, -3.0f);
		D3DXVECTOR3 at(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

		D3DXMatrixLookAtLH(&m_matEndView, &from, &at, &up);
		//V(m_D3DDev->SetTransform(D3DTS_VIEW, &matView));

		hr = D3DXCreateTextureFromFile(pD3DDevice, theApp.m_strCurPath + "Texture\\White.bmp", &m_TextureAry[0]);
		hr = D3DXCreateTextureFromFile(pD3DDevice, theApp.m_strCurPath + "Texture\\Black.bmp", &m_TextureAry[1]);
		hr = D3DXCreateTextureFromFile(pD3DDevice, theApp.m_strCurPath + "Texture\\Gray.bmp", &m_TextureAry[2]);
		hr = D3DXCreateTextureFromFile(pD3DDevice, theApp.m_strCurPath + "Texture\\Green.bmp", &m_TextureAry[3]);
		hr = D3DXCreateTextureFromFile(pD3DDevice, theApp.m_strCurPath + "Texture\\Red.bmp", &m_TextureAry[4]);
		hr = D3DXCreateTextureFromFile(pD3DDevice, theApp.m_strCurPath + "Texture\\Blue.bmp", &m_TextureAry[5]);
	}
	return hr;
}

void CAllocator::Save2DMapping(CString strFilePath)
{
	if (m_i2DVerNum > 0)
	{
		CStdioFile sfFile;
		sfFile.Open(strFilePath, CFile::modeCreate | CFile::modeReadWrite);
		CString strData;

		for (int x = 0; x < m_i2DVerNum; x++)
		{
			strData.Format("%i,%f,%f,%f,%i,%f,%f,%i\n", m_vertices[x].color, m_vertices[x].position.x,
				m_vertices[x].position.y, m_vertices[x].position.z, m_vertices[x].spcolor,
				m_vertices[x].tu, m_vertices[x].tv, m_bSelAry[x]);

			sfFile.WriteString(strData);
		}

		sfFile.Close();
	}
}

void CAllocator::Init2DMapping(INT32 iStaus)
{
	if (iStaus == 0)
	{
		CString strFileName;
		HRESULT hr;
		CFileFind ffFinder;
		strFileName.Format("2DMapping_%i.map", m_iIndex);

		EnterCriticalSection(&m_csFunBsy);

		if (ffFinder.FindFile(theApp.m_strCurPath + "\\TempShape\\" + strFileName))
			Create2DMapping(theApp.m_strCurPath + "\\TempShape\\" + strFileName);
	}
	else if (iStaus == 1)
	{
		void* pData;
		HRESULT hr;

		if (m_i2DVerNum > 0)
		{
			V(m_vertexBuffer->Lock(0, 0, &pData, 0));
			memcpy(pData, m_vertices, sizeof(CAllocator::MAP2DVERTEX) * m_i2DVerNum);
			V(m_vertexBuffer->Unlock());
		}

		m_iBufferAction = 0;
		m_iCurStatus = STATUS_PREVIEWING;
		LeaveCriticalSection(&m_csFunBsy);
	}
}

HRESULT CAllocator::Create2DMapping(CString strFilePath)
{
	HRESULT hr;

	CFileFind fFind;
	CFile sfFile;
	CString strData, strTemp;
	INT32 iStrPos = 0;
	INT32 iVerIndx = 0;

	if (!fFind.FindFile(strFilePath))
		return S_FALSE;

	if (m_vertexBuffer)
		m_vertexBuffer->Release();
	m_vertexBuffer = NULL;

	if (m_vertices)
		delete m_vertices;

	INT32 iGridX = 200;
	INT32 iGridY = 100;
	INT32 iReadByte = 0;
	m_i2DVerNum = (8 + iGridX *iGridY) * 6;

	m_vertices = new MAP2DVERTEX[m_i2DVerNum];
	char cData[10000] = "";

	sfFile.Open(strFilePath, CFile::modeRead);
	while (sfFile.GetPosition() < sfFile.GetLength())
	{
		ZeroMemory(cData, 10000);

		if (sfFile.GetLength() > sfFile.GetPosition() + 10000)
			iReadByte = 10000;
		else
			iReadByte = sfFile.GetLength() - sfFile.GetPosition();

		sfFile.Read(cData, iReadByte);
		//sfFile.ReadString(strData);

		for (int m = 0; m < iReadByte; m++)
		{
			if (cData[m] != '\n')
				strData += cData[m];
			else
			{
				strData.Trim();
				iStrPos = 0;

				strTemp = strData.Tokenize(",", iStrPos);
				m_vertices[iVerIndx].color = 0xffffffff; //atoi(strTemp);

				strTemp = strData.Tokenize(",", iStrPos);
				m_vertices[iVerIndx].position.x = atof(strTemp);

				strTemp = strData.Tokenize(",", iStrPos);
				m_vertices[iVerIndx].position.y = atof(strTemp);

				strTemp = strData.Tokenize(",", iStrPos);
				m_vertices[iVerIndx].position.z = atof(strTemp);

				strTemp = strData.Tokenize(",", iStrPos);
				m_vertices[iVerIndx].spcolor = atoi(strTemp);

				strTemp = strData.Tokenize(",", iStrPos);
				m_vertices[iVerIndx].tu = atof(strTemp);

				strTemp = strData.Tokenize(",", iStrPos);
				m_vertices[iVerIndx].tv = atof(strTemp);

				strTemp = strData.Tokenize(",", iStrPos);
				m_bSelAry[iVerIndx] = atoi(strTemp);

				strData = "";
				iVerIndx++;
			}
		}
	}

	m_i2DVerNum = iVerIndx;

	sfFile.Close();

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	pD3DDevice->CreateVertexBuffer(sizeof(CAllocator::MAP2DVERTEX) * m_i2DVerNum, 0,
		(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1), D3DPOOL_MANAGED, &m_vertexBuffer, NULL);
	return hr;
}

HRESULT CAllocator::Create2DMapping()
{
	HRESULT hr;
	CRect rectRgn;

	INT32 iPreStatus = m_iCurStatus;
	m_iCurStatus = STATUS_LOADING;
	Sleep(100);

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	EnterCriticalSection(&m_csFunBsy);

	if (m_vertexBuffer)
		m_vertexBuffer->Release();
	m_vertexBuffer = NULL;

	for (int x = 0; x < MAX_PTAREA; x++)
	{
		if (m_pRgnOverLap[x])
		{
			::DeleteObject(m_pRgnOverLap[x]);
			m_pRgnOverLap[x] = NULL;
		}
	}

	CRect rectWindow, rectTemp;
	GetClientRect(m_window, &rectWindow);

	
	CArray<CPoint, CPoint> ptAry;
	BOOL bDarkAreaAry[10] = { FALSE };
	m_ptAry.RemoveAt(0);
	INT32 iCurAreaIdx = -1;
	BlendingPt bptTemp;
	INT32 iMaxIdx = -1;
	for (int x = 0; x < m_ptAry.GetCount(); x++)
	{
		bptTemp = m_ptAry.GetAt(x);

		if (iCurAreaIdx != bptTemp.iArea && x != 0)
		{
			m_pRgnOverLap[iCurAreaIdx] = new CRgn();
			m_pRgnOverLap[iCurAreaIdx]->CreatePolygonRgn(ptAry.GetData(), ptAry.GetCount(), ALTERNATE);
			ptAry.RemoveAll();
		}
		ptAry.Add(bptTemp.pt);
		iCurAreaIdx = bptTemp.iArea;
		bDarkAreaAry[iCurAreaIdx] = bptTemp.bDarkMask;

		if (iMaxIdx < iCurAreaIdx)
			iMaxIdx = iCurAreaIdx;
	}

	if (ptAry.GetCount() > 0)
	{
		m_pRgnOverLap[iMaxIdx] = new CRgn();
		m_pRgnOverLap[iMaxIdx]->CreatePolygonRgn(ptAry.GetData(), ptAry.GetCount(), ALTERNATE);
		ptAry.RemoveAll();
	}

	CRgn rgnTemp;
	rgnTemp.CreateRectRgn(0, 0, 0, 0);
	CString strTemp;
	for (int x = 0; x <= iMaxIdx; x++)
	{
		if (x == 0)
			CopyRgn(rgnTemp, *m_pRgnOverLap[0]);
		else
			CombineRgn(rgnTemp, rgnTemp, *m_pRgnOverLap[x], RGN_OR);			
	}
	rgnTemp.GetRgnBox(&rectRgn);

	if (rectRgn.left < 10)
		rectRgn.left = 0;
	if (rectRgn.top < 10)
		rectRgn.top = 0;
	if (abs(rectRgn.right - rectWindow.right) < 10)
		rectRgn.right = rectWindow.right;
	if (abs(rectRgn.bottom - rectWindow.bottom) < 10)
		rectRgn.bottom = rectWindow.bottom;

	INT32 iGridX = 200;
	INT32 iGridY = 100;
	m_i2DVerNum = (8 + iGridX *iGridY)*6;

	if (m_vertices)
		delete m_vertices;
	m_vertices = new MAP2DVERTEX[m_i2DVerNum];
	INT32 iVerIndx = 0;

	for (int x = 0; x < 9; x++)
	{
		if (x == 0)
			rectTemp = CRect(0, 0, rectRgn.left, rectRgn.top);
		else if (x == 1)
			rectTemp = CRect(rectRgn.left, 0, rectRgn.right, rectRgn.top);
		else if (x == 2)
			rectTemp = CRect(rectRgn.right, 0, rectWindow.right, rectRgn.top);
		else if (x == 3)
			rectTemp = CRect(0, rectRgn.top, rectRgn.left, rectRgn.bottom);
		else if (x == 4)
			rectTemp = rectRgn;
		else if (x == 5)
			rectTemp = CRect(rectRgn.right, rectRgn.top, rectWindow.right, rectRgn.bottom);
		else if (x == 6)
			rectTemp = CRect(0, rectRgn.bottom, rectRgn.left, rectWindow.bottom);
		else if (x == 7)
			rectTemp = CRect(rectRgn.left, rectRgn.bottom, rectRgn.right, rectWindow.bottom);
		else if (x == 8)
			rectTemp = CRect(rectRgn.right, rectRgn.bottom, rectWindow.right, rectWindow.bottom);

		if (rectTemp.Width() == 0 || rectTemp.Height() == 0)
		{
			m_i2DVerNum -= 6;
			continue;
		}

		INT32 iX, iY;
		double fRatio = double(m_rectClient.Width()) / m_rectClient.Height();

		if (x != 4)
		{
			for (int y = 0; y < 6; y++)
			{
				m_bSelAry[iVerIndx] = 0;
				iX = rectTemp.left; 
				iY = rectTemp.top;

				if (y == 2 || y == 3 || y == 5)
					iX = rectTemp.right;

				if (y == 1 || y == 4 || y == 5)
					iY = rectTemp.bottom;

				m_vertices[iVerIndx].tu = double(iX) / m_rectClient.Width();
				m_vertices[iVerIndx].tv = double(iY) / m_rectClient.Height();

				m_vertices[iVerIndx].color = 0xffffffff;
				m_vertices[iVerIndx].spcolor = 0;

				m_vertices[iVerIndx].position = CAllocator::MAP2DVERTEX::Position(-fRatio + fRatio * 2.0f * double(iX) / m_rectClient.Width(),
					1.0f - 2.0f*double(iY) / m_rectClient.Height(), 0.0f);

				iVerIndx++;
			}
		}
		else
		{
			INT32 iRectInRgn;
			BOOL bDarkMark = FALSE;
			for (int m = 0; m < iGridX; m++)
			for (int n = 0; n < iGridY; n++)
			{
				rectTemp = CRect(rectRgn.left + m*rectRgn.Width() / iGridX, 
					             rectRgn.top + n*rectRgn.Height() / iGridY,
								 rectRgn.left + (m + 1)*rectRgn.Width() / iGridX, 
					             rectRgn.top + (n + 1)*rectRgn.Height() / iGridY);

				iRectInRgn = 0;
				bDarkMark = FALSE;
				for (int p = 0; p <= iMaxIdx; p++)
				{
					if (m_pRgnOverLap[p] && m_pRgnOverLap[p]->RectInRegion(rectTemp))
					{
						iRectInRgn |= 1 << p;
						bDarkMark = bDarkAreaAry[p];
					}
				}

				for (int y = 0; y < 6; y++)
				{
					m_bSelAry[iVerIndx] = 0;
					iX = rectTemp.left;
					iY = rectTemp.top;

					if (y == 2 || y == 3 || y == 5)
						iX = rectTemp.right;

					if (y == 1 || y == 4 || y == 5)
						iY = rectTemp.bottom;

					m_vertices[iVerIndx].tu = double(iX) / m_rectClient.Width();
					m_vertices[iVerIndx].tv = double(iY) / m_rectClient.Height();


					m_vertices[iVerIndx].color = 0xffffffff;
					m_vertices[iVerIndx].spcolor = 0;

					if (iRectInRgn > 0)
					{
						if (!bDarkMark)
						{
							m_vertices[iVerIndx].color = 0xffff0000;
							m_bSelAry[iVerIndx] = iRectInRgn;
						}
						else
						{
							m_vertices[iVerIndx].color = 0;
							m_bSelAry[iVerIndx] = 0x40;
						}
					}

					m_vertices[iVerIndx].position = CAllocator::MAP2DVERTEX::Position(-fRatio + fRatio * 2.0f * double(iX) / m_rectClient.Width(),
						1.0f - 2.0f*double(iY) / m_rectClient.Height(), 0.0f);

					iVerIndx++;
				}
			}
		}
	}

	pD3DDevice->CreateVertexBuffer(sizeof(CAllocator::MAP2DVERTEX) * m_i2DVerNum, 0,
		(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1), D3DPOOL_MANAGED, &m_vertexBuffer, NULL);

	void* pData;
	V(m_vertexBuffer->Lock(0, 0, &pData, 0));
	memcpy(pData, m_vertices, sizeof(CAllocator::MAP2DVERTEX) * m_i2DVerNum);
	V(m_vertexBuffer->Unlock());

	m_iCurStatus = iPreStatus;
	LeaveCriticalSection(&m_csFunBsy);

	m_iBufferAction = 2;
	return hr;
}

HRESULT CAllocator::Create2DMapping(INT32 iGridX, INT32 iGridY)
{
	HRESULT hr;
	INT32 iX, iY;

	m_iGridX = iGridX;
	m_iGridY = iGridY;

	m_iCurStatus = STATUS_LOADING;
	Sleep(100);

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	if (m_vertices)
		delete m_vertices;

	m_i2DVerNum = m_iGridX *m_iGridY * 2 * 3;
	m_vertices = new MAP2DVERTEX[m_i2DVerNum];

	if (m_vertexBuffer)
		m_vertexBuffer->Release();
	m_vertexBuffer = NULL;
	
	double fRatio = double(m_rectClient.Width()) / m_rectClient.Height();

	for (int x = 0; x < m_i2DVerNum; x++)
	{
		m_bSelAry[x] = 0;
		iX = INT(x / 6) % m_iGridX;
		if (x % 6 == 2 || x % 6 == 3 || x % 6 == 5)
			iX += 1;

		iY = INT(INT(x / 6) / m_iGridX);

		if (x % 6 == 1 || x % 6 == 4 || x % 6 == 5)
			iY += 1;

		m_vertices[x].tu = double(iX) / m_iGridX;
		m_vertices[x].tv = double(iY) / m_iGridY;

		m_vertices[x].color = 0xffffffff;
		m_vertices[x].spcolor = 0;
		
		m_vertices[x].position = CAllocator::MAP2DVERTEX::Position(-fRatio + fRatio * 2.0f * double(iX) / m_iGridX,
			1.0f - 2.0f*double(iY) / m_iGridY, 0.0f);
	}

	pD3DDevice->CreateVertexBuffer(sizeof(CAllocator::MAP2DVERTEX) * m_i2DVerNum, 0,
		(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1), D3DPOOL_MANAGED, &m_vertexBuffer, NULL);

	void* pData;
	V(m_vertexBuffer->Lock(0, 0, &pData, 0));
	memcpy(pData, m_vertices, sizeof(CAllocator::MAP2DVERTEX) * m_i2DVerNum);
	V(m_vertexBuffer->Unlock());

	m_iCurStatus = STATUS_PREVIEWING;

	return hr;
}

void CAllocator::OnMouseMove(INT32 iPannel, UINT nFlags, CPoint point, MOUSE_EVT nEventIdx)
{
	if ((g_PannelSetting.iSelIdx == 0 && g_PannelAry.GetSize() > 1) || m_iSelObjIdx == -1)
		return;

	EnterCriticalSection(&m_csFunBsy);
	if (point.x >= m_rectClient.Width())
		point.x = m_rectClient.Width();
	if (point.y >= m_rectClient.Height())
		point.y = m_rectClient.Height();
	/*if (point.x < 0)
		point.x = 0;
	if (point.y < 0)
		point.y = 0;*/

	

	if (m_ptSel.x != -1 && (m_i2DMapAction & 0xf) != MAP2D_MODIFY_PT && (g_PannelSetting.iModelAction & 0xf) != MODEL_MAP)
	{
		m_ptSel = CPoint(-1, -1);
	}

	#ifdef PLAY_3D_MODE
		if ((m_i2DMapAction & 0xf) == MAP2D_SELECT_PT)
		{
			if (nEventIdx == MOUSE_LBUT_DOWN)
			{
				if (point.x < 2)
					point.x -= 2;
				if (point.y < 2)
					point.y -= 2;

				m_ptAreaSel.left = point.x;
				m_ptAreaSel.top = point.y;
			}
			else if (nEventIdx == MOUSE_MOVE)
			{
				if (nFlags & MK_LBUTTON)
				{
					if (point.x > m_rectClient.Width() - 2)
						point.x += 2;
					if (point.y > m_rectClient.Height() - 2)
						point.y += 2;

					m_ptAreaSel.right = point.x;
					m_ptAreaSel.bottom = point.y;
				}
				else
				{
					m_ptAreaSel = CRect(point.x - 2, point.y - 2, point.x + 2, point.y + 2);
				}
			}
			else if (nEventIdx == MOUSE_LBTN_UP && m_ptAreaSel.Width() > 0)
			{
				CRect rectTemp = m_ptAreaSel;
				m_ptAreaSel = CRect(0, 0, 0, 0);

				CPoint ptTemp;
				INT32 iBaseH = m_rectClient.Height() / 2;
				INT32 iBaseW = m_rectClient.Width() / 2;

				for (int x = 0; x < m_i2DVerNum; x++)
				{
					ptTemp.x = m_vertices[x].position.x * iBaseH + iBaseW;
					ptTemp.y = -m_vertices[x].position.y * iBaseH + iBaseH;

					if (rectTemp.PtInRect(ptTemp))
					{
						m_bSelAry[x] |= 0x80;
					}
				}

				if (MessageBox(NULL, "This PT Correct?", "Select", MB_OKCANCEL | MB_TASKMODAL| MB_TOPMOST) != IDOK)
				{
					m_iBufferAction = 1;
				}
			}
		}
		else if ((m_i2DMapAction & 0xf) == MAP2D_MODIFY_PT)
		{
			if (nEventIdx == MOUSE_LBUT_DOWN)
			{
				m_ptSel = point;
			}
			else if (nEventIdx == MOUSE_MOVE)
			{
				if (m_ptSel.x > 0 && m_ptSel.y > 0)
					m_ptOffset = point - m_ptSel;
			}
			else if (nEventIdx == MOUSE_LBTN_UP)
			{
				m_ptSel = CPoint(-1, -1);
				if (MessageBox(NULL, "This Move Correct?", "Select", MB_OKCANCEL | MB_TASKMODAL| MB_TOPMOST) == IDOK)
				{
					m_iBufferAction = 2;
				}
				else
				{
					m_ptOffset = CPoint(0, 0);
				}
			}
		}
		else if (m_ptAry.GetCount() > 0)
		{
			if (m_iCurStatus == STATUS_PREVIEWING)
			{
				if (nEventIdx == MOUSE_MOVE)
					m_ptSel = point;
				else if (nEventIdx == MOUSE_LBTN_UP)
				{
					if (MessageBox(NULL, "Point Correct?", "Select", MB_OKCANCEL | MB_TASKMODAL| MB_TOPMOST) == IDOK)
					{
						m_ptAry.Add({ point, m_uCurPtArea, FALSE });
						m_ptSel = CPoint(-1, -1);
					}
					else
					{
						m_ptSel = CPoint(-1, -1);
					}
				}
				else if (nEventIdx == MOUSE_RBTN_UP)
				{
					/*/if (MessageBox(NULL, "Finished or Cancel?", "Select", MB_OKCANCEL | MB_TASKMODAL) == IDOK)
					{
						if (MessageBox(NULL, "Connect to First Point?", "Select", MB_OKCANCEL | MB_TASKMODAL) == IDOK)
							m_ptAry.Add(m_ptAry.GetAt(1));

						Create2DMapping();
					}*/

					CAreaOptionDlg dlgAranOption;

					if (dlgAranOption.DoModal() == IDOK)
					{
						BlendingPt bptTemp;

						if (dlgAranOption.m_uGroupEnd == 1)
						{
							for (int x = 1; x < m_ptAry.GetCount(); x++)
							{
								if (m_ptAry.GetAt(x).iArea == m_uCurPtArea)
								{
									bptTemp = m_ptAry.GetAt(x);
									break;
								}
							}
							m_ptAry.Add(bptTemp);
						}

						if (dlgAranOption.m_uDarkMask == 1)
						{
							for (int x = 1; x < m_ptAry.GetCount(); x++)
							{
								if (m_ptAry.GetAt(x).iArea == m_uCurPtArea)
								{
									m_ptAry.GetAt(x).bDarkMask = TRUE;
								}
							}
						}

						if (dlgAranOption.m_uNextAction == 0)
						{
							Create2DMapping();
							m_ptAry.RemoveAll();
							m_uCurPtArea = 0;
						}
						else if (dlgAranOption.m_uNextAction == 1)
						{
							m_uCurPtArea++;
						}
						m_ptSel = CPoint(-1, -1);
					}
					else
					{
						m_ptAry.RemoveAll();
						m_ptSel = CPoint(-1, -1);
					}
				}

				if (m_iIndex == 0 && g_PannelSetting.iSelIdx != 0)
					g_PannelAry[g_PannelSetting.iSelIdx]->SetSelectPt(point, FALSE);
			}
		}
		else if ((g_PannelSetting.iModelAction & 0xf) == MODEL_MAP)
		{
			if (m_ptSel.x == -1)
			{
				if (m_scene.OnMouseMove(nFlags, point, nEventIdx, g_PannelSetting.iModelAction) == 1)
				{
					if (MessageBox(NULL, "This PT Correct?", "Select", MB_OKCANCEL | MB_TASKMODAL| MB_TOPMOST) == IDOK)
					{
						m_ptSel = point;

						if (m_iIndex == 0 && g_PannelSetting.iSelIdx != 0)
							g_PannelAry[g_PannelSetting.iSelIdx]->SetSelectPt(point, FALSE);
					}
					else
						m_scene.CleanSelectPt();
				}
			}
			else if (m_iCurStatus == STATUS_PREVIEWING)
			{
				if (nEventIdx == MOUSE_MOVE)
					m_ptSel = point;
				else 
				{
					m_iCurStatus = STATUS_BUSYING;
					if (m_iIndex == 0 && g_PannelSetting.iSelIdx != 0)
						g_PannelAry[g_PannelSetting.iSelIdx]->SetSelectPt(point, TRUE);

					if (nEventIdx == MOUSE_LBTN_UP)
					{
						if (MessageBox(NULL, "Mapping Correct?", "Select", MB_OKCANCEL | MB_TASKMODAL| MB_TOPMOST) == IDOK)
						{
							BOOL bNotify = m_scene.AddSelectPt(m_ptSel);
							m_ptSel = CPoint(-1, -1);

							if (m_iIndex == 0 && g_PannelSetting.iSelIdx != 0)
							{
								g_PannelAry[g_PannelSetting.iSelIdx]->SetSelectPt(CPoint(-1, -1), FALSE);
								if (bNotify)
									g_PannelAry[g_PannelSetting.iSelIdx]->ResetFromSetting();
								else
									g_PannelAry[g_PannelSetting.iSelIdx]->CleanModifyPt();
							}
						}
						else
						{
							m_ptSel = CPoint(-1, -1);
							m_scene.CleanSelectPt();

							if (m_iIndex == 0 && g_PannelSetting.iSelIdx != 0)
							{
								g_PannelAry[g_PannelSetting.iSelIdx]->SetSelectPt(CPoint(-1, -1), FALSE);
								g_PannelAry[g_PannelSetting.iSelIdx]->CleanModifyPt();
							}
						}
					}
					else if (nEventIdx == MOUSE_RBTN_UP)
					{
						if (MessageBox(NULL, "Leavl Mapping?", "Select", MB_OKCANCEL | MB_TASKMODAL| MB_TOPMOST) == IDOK)
						{
							m_ptSel = CPoint(-1, -1);
							m_scene.CleanSelectPt();

							if (m_iIndex == 0 && g_PannelSetting.iSelIdx != 0)
							{
								g_PannelAry[g_PannelSetting.iSelIdx]->SetSelectPt(CPoint(-1, -1), FALSE);
								g_PannelAry[g_PannelSetting.iSelIdx]->CleanModifyPt();
							}
						}
					}
					m_iCurStatus = STATUS_PREVIEWING;
				}
			}
		}
		else if (g_PannelSetting.iModelAction & MODEL_MODIFY)
		{
			if ((g_PannelSetting.iModelAction & 0xf) == MODEL_MODIFY_SELECT)
			{
				if (nEventIdx == MOUSE_LBUT_DOWN)
				{
					m_ptFirstSel = point;
					m_ptAreaSel.left = point.x;
					m_ptAreaSel.top = point.y;

					if (m_iIndex == 0)
					SetCapture(m_pParentWnd->m_hWnd);
				}
				else if (nEventIdx == MOUSE_MOVE && (nFlags & MK_LBUTTON))
				{
					m_ptAreaSel = CRect(m_ptFirstSel.x, m_ptFirstSel.y, point.x, point.y);
					//m_ptAreaSel.right = point.x;
					//m_ptAreaSel.bottom = point.y;

					m_ptAreaSel.NormalizeRect();
				}
				else if (nEventIdx == MOUSE_LBTN_UP)
				{
					ReleaseCapture();

					if (m_ptAreaSel.Width() > 0)
					{
						if (m_scene.SelectAreaPt(m_ptAreaSel))
						{
							m_ptAreaSel = CRect(0, 0, 0, 0);
							if (MessageBox(NULL, "This PT Correct?", "Select", MB_OKCANCEL | MB_TASKMODAL| MB_TOPMOST) == IDOK)
							{
								g_PannelSetting.iNotifyAction = MODEL_MODIFY_MOVE;
								g_PannelSetting.iModelAction = MODEL_MODIFY_MOVE;
								m_ptSel = point;

								if (m_iIndex == 0 && g_PannelSetting.iSelIdx != 0)
									g_PannelAry[g_PannelSetting.iSelIdx]->SetSelectPt(point, FALSE);
							}
							else
								m_scene.CleanSelectPt();
						}
						m_ptAreaSel = CRect(0, 0, 0, 0);
					}

				}
			}
			else
				m_ptAreaSel = CRect(0, 0, 0, 0);

			INT32 iResult = m_scene.OnMouseMove(nFlags, point, nEventIdx, g_PannelSetting.iModelAction);

			if ( iResult == 1)
			{
				if (MessageBox(NULL, "This PT Correct?", "Select", MB_OKCANCEL | MB_TASKMODAL| MB_TOPMOST) == IDOK)
				{
					m_ptSel = point;

					if (m_iIndex == 0 && g_PannelSetting.iSelIdx != 0)
						g_PannelAry[g_PannelSetting.iSelIdx]->SetSelectPt(point, FALSE);

					g_PannelSetting.iNotifyAction = MODEL_MODIFY_MOVE;
					g_PannelSetting.iModelAction = MODEL_MODIFY_MOVE;
				}
				else
					m_scene.CleanSelectPt();
			}
			else if (iResult == 2)
			{
				if (m_iIndex == 0 && g_PannelSetting.iSelIdx != 0)
					g_PannelAry[g_PannelSetting.iSelIdx]->LoadPtOffset();
			}

		}
		else
			m_scene.OnMouseMove(nFlags, point, nEventIdx, g_PannelSetting.iModelAction);

		{
			if (m_iIndex == 0 && g_PannelSetting.iSelIdx != 0)
			{
				g_PannelAry[g_PannelSetting.iSelIdx]->SlaveMsg(nFlags, point, nEventIdx);
			}
		}
	#endif

	LeaveCriticalSection(&m_csFunBsy);
}

int CAllocator::UpdateSurfaceBySample(IDirect3DTexture9 *pTexture, INT32 iObjIdx)
{
	IMediaSample *pMediaSample = NULL;
	int iMediaIdx = -1;
	HRESULT hr = S_OK;

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	STREM_MAPPING smTemp = theApp.GetStreamMapByIdx(m_iIndex, iObjIdx);

	if (smTemp.iWidth > 0)
		iMediaIdx = smTemp.iMediaIdx;

	if (iMediaIdx == -1)
		return E_FAIL;

#ifdef _ENABLE_DISPLAY_MANAGER
	IDirect3DSurface9 *pVideoSurface = NULL;

	if (!g_MediaStreamAry.at(iMediaIdx)->IsOpened() || !pTexture)
		return E_FAIL;

	pTexture->GetSurfaceLevel(0, &pVideoSurface);
	IDirect3DSurface9* pCurSurface = g_MediaStreamAry.at(iMediaIdx)->GetSurface(m_iIndex);
	if (pCurSurface)
	{
		m_pTmpSurfaces[iObjIdx] = pCurSurface;

#ifdef _ENABLE_VIDEO_WALL
		
		hr = pD3DDevice->StretchRect(pCurSurface, NULL, pVideoSurface, NULL, D3DTEXF_NONE);
		if (FAILED(hr))
		{
			char szMsg[512];
			sprintf_s(szMsg, "#Allocator# StretchRect failed!! \n");
			OutputDebugStringA(szMsg);
		}
		
#else
#endif
		pVideoSurface->Release();
	}
	else
	{
		pVideoSurface->Release();
		return E_FAIL;
	}
#else
	//EnterCriticalSection(&m_csEffectInUse);
	unsigned char *pSrc = g_MediaStreamAry.at(iMediaIdx)->GetVideoBuffer();
	if (pSrc == NULL)
		return E_FAIL;

	INT32 iWidth = g_MediaStreamAry.at(iMediaIdx)->GetVideoWidth();
	INT32 iHeight = g_MediaStreamAry.at(iMediaIdx)->GetVideoHeight();

	D3DLOCKED_RECT d3drect;
	IDirect3DSurface9 *pVideoSurface = NULL;
	pTexture->GetSurfaceLevel(0, &pVideoSurface);
	//m_D3DDev->GetRenderTargetData(pVideoSurface, m_surBuffaces.at(0));

	V(m_pBufSurface.at(iObjIdx)->LockRect(&d3drect, NULL, 0));
	BYTE *pDest = (BYTE*)d3drect.pBits;
	if (pDest == NULL)
		return E_FAIL;

	memcpy(pDest, pSrc, iWidth * iHeight * 4);
	V(m_pBufSurface.at(iObjIdx)->UnlockRect());
	V(m_D3DDev->StretchRect(m_pBufSurface.at(iObjIdx), NULL, pVideoSurface, NULL, D3DTEXF_NONE));
	//LeaveCriticalSection(&m_csEffectInUse);
#endif

#ifdef MEDIA_STREAM_TEST
	
	//============= Video Effect Loop==============//
	INT32 iEffectIdx = g_MediaStreamAry.at(iMediaIdx)->GetEffectIndex();

	if (iEffectIdx == 0)
		g_MediaStreamAry.at(iMediaIdx)->SetEffectIndex(iMediaIdx + 1);

	INT32 iLevel = g_MediaStreamAry.at(iMediaIdx)->GetEffectParam(0);
	if (iLevel < 99)
		iLevel++;
	else
	{
		if (iEffectIdx >= 31)
			iEffectIdx = 0;
		g_MediaStreamAry.at(iMediaIdx)->SetEffectIndex(iEffectIdx + 1);
		iLevel = 0;
	}

	g_MediaStreamAry.at(iMediaIdx)->SetEffectParam(0, iLevel);

	//============= Zoom Loop==============//
	float fFactor = float(GetTickCount()%1000)/1000;

	if (fFactor < 0.01)
		fFactor = 0.01;
	g_MediaStreamAry.at(iMediaIdx)->SetSrcArea(CRect(0, 0, iWidth*fFactor, iHeight));
	g_MediaStreamAry.at(iMediaIdx)->SetDestArea(CRect(0, 0, iWidth, iHeight*fFactor));
#endif

	return hr;
}

int CAllocator::UpdateSurfaceBySample(IDirect3DSurface9 *pVideoSurface, INT32 iObjIdx)
{
	IMediaSample *pMediaSample = NULL;
	INT32 iMediaIdx = -1;
	HRESULT hr = S_OK;
	D3DSURFACE_DESC d3d_surface_desc;
	pVideoSurface->GetDesc(&d3d_surface_desc);
	
	STREM_MAPPING smTemp = theApp.GetStreamMapByIdx(m_iIndex, iObjIdx);

	if (smTemp.iWidth > 0)
		iMediaIdx = smTemp.iMediaIdx;

	if (iMediaIdx == -1)
		return E_FAIL;

	unsigned char *src = g_MediaStreamAry.at(iMediaIdx)->GetVideoBuffer();
	if (src == NULL)
		return E_FAIL;

	INT32 iWidth = g_MediaStreamAry.at(iMediaIdx)->GetVideoWidth();
	INT32 iHeight = g_MediaStreamAry.at(iMediaIdx)->GetVideoHeight();

	D3DLOCKED_RECT d3drect;
	V(pVideoSurface->LockRect(&d3drect, NULL, 0));

	unsigned char *dst = (unsigned char*)d3drect.pBits;

	if (d3d_surface_desc.Format == D3DFMT_A8R8G8B8 || d3d_surface_desc.Format == D3DFMT_X8R8G8B8)  //RGBA 32
	{
		if (d3drect.Pitch == iWidth*4)
			memcpy(dst, src, iWidth * iHeight * 4);
		else
		{
			int iBytesbyPerLine = iWidth * 4;
			for (int i = 0; i < iHeight; i++)
			{
				memcpy(dst, src, iBytesbyPerLine);
				src += iBytesbyPerLine;
				dst += d3drect.Pitch;
			}
		}
	}
	

	V(pVideoSurface->UnlockRect());
	return hr;
}

HRESULT CAllocator::VidoeEffect()
{
	HRESULT hr = S_OK;

//	struct sEffectD3DParam D3DParam;
	INT32 iVideoIdx = m_iCurThdIdx;

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

//	D3DParam.d3ddev = pD3DDevice;

	do
	{
		m_iEffectStatus[iVideoIdx] = 0;
		while (m_pImgEffectThd[iVideoIdx] && m_iEffectStatus[iVideoIdx] == 0 && m_bPreviewFlag)
		{
			Sleep(1);
		}

		if (!m_bPreviewFlag || !m_pImgEffectThd[iVideoIdx])
			break;

#ifdef NOT_PROC_EFFECT

#ifdef _ENABLE_VIDEO_WALL
		//for debug 
		//Sleep(1);
		if (m_iIndex == 0)
		{
			EnterCriticalSection(&m_csEffectInUse);
			if (m_pDisplayManager->GetRenderStatus() == RS_DRAW || m_pDisplayManager->GetRenderStatus() == RS_WAIT || m_iCurStatus == STATUS_LOADING)
				LeaveCriticalSection(&m_csEffectInUse);
			else
			{
				if (UpdateSurfaceBySample(m_BufTexture[iVideoIdx], iVideoIdx) == E_FAIL)
					m_bMediaStatusAry[iVideoIdx] = FALSE;
				else
				{
					m_bMediaStatusAry[iVideoIdx] = TRUE;
					m_lFrameCount[iVideoIdx]++;
				}
				LeaveCriticalSection(&m_csEffectInUse);
			}
		}
#else
		//EnterCriticalSection(&m_csEffectInUse);

		STREM_MAPPING smTemp = theApp.GetStreamMapByIdx(m_iIndex, iVideoIdx);

		int iSurfaceIndex = g_MediaStreamAry.at(smTemp.iMediaIdx)->GetSurfaceIndex(m_iIndex);

		if (iSurfaceIndex == -1 || m_pDisplayManager->GetRenderStatus() == RS_DRAW || m_pDisplayManager->GetRenderStatus() == RS_WAIT || m_iCurStatus == STATUS_LOADING || m_pDisplayManager->GetCurPos(iSurfaceIndex) == m_dCurPos[iVideoIdx])
		//if (m_pDisplayManager->GetRenderStatus() == RS_DRAW || m_pDisplayManager->GetRenderStatus() == RS_WAIT || m_iCurStatus == STATUS_LOADING || m_pDisplayManager->GetFrameCount() == m_iLastFrameCount)
		{
			//LeaveCriticalSection(&m_csEffectInUse);
				continue;
		}
		else
		{
			/*
			if (m_iIndex >= 1)
			{
				DWORD dwRet = ::WaitForSingleObject(m_hEffect2, 1000);

				if (WAIT_TIMEOUT == dwRet)
					continue;
			}
			*/
			if (m_pDisplayManager->GetRenderStatus() != RS_NORMAL)
			{
				//LeaveCriticalSection(&m_csEffectInUse);				
				continue;
			}

			EnterCriticalSection(&m_csEffectInUse);
			//::ResetEvent(m_hEffect1);
			::ResetEvent(m_hEffects[iVideoIdx]);

			if (UpdateSurfaceBySample(m_BufTexture[iVideoIdx], iVideoIdx) == E_FAIL)
				m_bMediaStatusAry[iVideoIdx] = FALSE;
			else
			{
				m_bMediaStatusAry[iVideoIdx] = TRUE;
				m_lFrameCount[iVideoIdx]++;
			}

			m_dCurPos[iVideoIdx] = m_pDisplayManager->GetCurPos(iSurfaceIndex);
			LeaveCriticalSection(&m_csEffectInUse);
			//::SetEvent(m_hEffect1);
			::SetEvent(m_hEffects[iVideoIdx]);
		}
#endif
//		Sleep(1);
		continue;
#endif
		/*
#ifdef PLAY_3D_MODE
		if (m_iEffectStatus[iVideoIdx] == 1)
		{
			if (0)
			{
				IDirect3DSurface9 *backBuffer;
				hr = pD3DDevice->GetRenderTarget(0, &backBuffer);

				//IDirect3DSurface9 *pBufSurface;
				//m_Buf3DTexture[iVideoIdx]->GetSurfaceLevel(0, &pBufSurface);

				UpdateSurfaceBySample(m_pBufSurface.at(iVideoIdx), iVideoIdx);
				pD3DDevice->StretchRect(m_pBufSurface.at(iVideoIdx), NULL, backBuffer, NULL, D3DTEXF_NONE);
				D3DParam.pVideoStream = backBuffer;
			}
			else if (1)
			{
				UpdateSurfaceBySample(m_pBufSurface.at(iVideoIdx), iVideoIdx);
				D3DParam.pVideoStream = m_pBufSurface.at(iVideoIdx);
			}
			else
				D3DParam.pVideoStream = m_surfaces.at(iVideoIdx);  //Source
		}
		else // Mutli Effect
		{
			IDirect3DSurface9 *pTempSurface = NULL;
			m_BufTexture[iVideoIdx]->GetSurfaceLevel(0, &pTempSurface);  //Source
			D3DParam.pVideoStream = pTempSurface;
		}
#else  
		D3DParam.pVideoStream = m_surfaces.at(iVideoIdx);  //Source

#endif // DEBUG


		if (m_iEffectIdx[iVideoIdx] == 8)
			m_iEffectIdx[iVideoIdx] = m_iEffectIdx[iVideoIdx];

		D3DParam.pOffscreenSurface2D = m_surBuffaces.at(iVideoIdx);
		D3DParam.pOutFrameTexture    = m_BufTexture[iVideoIdx];  //Output

		D3DParam.pOffscreenTexture3D = m_Buf3DTexture[iVideoIdx];
		D3DParam.pzSurface = m_sur3DBuf[iVideoIdx];

		//D3DParam.pOffscreenTexture3D=m_pOffsereenTexture3D;
		//D3DParam.pzSurface            = m_zSurface;
		//g_EffectParam.iLevel = 40;

#ifdef PLAY_3D_MODE
		
		D3DXMATRIX matProj, matView, matWorld;

		if (Is3DEffect(m_iEffectIdx[iVideoIdx]))
		{
			EnterCriticalSection(&m_csEffectInUse);
			V(pD3DDevice->GetTransform(D3DTS_WORLD, &matWorld));
			V(pD3DDevice->GetTransform(D3DTS_VIEW, &matView));
			V(pD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj));
		}

		VideoEffect( m_iEffectIdx[iVideoIdx], D3DParam, g_EffectParam );

		if (Is3DEffect(m_iEffectIdx[iVideoIdx]))
		{
			V(pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld));
			V(pD3DDevice->SetTransform(D3DTS_VIEW, &matView));
			V(pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj));
			LeaveCriticalSection(&m_csEffectInUse);
		}
#else
		EnterCriticalSection(&m_csEffectInUse);
		VideoEffect( m_iEffectIdx[m_iIndex], D3DParam, g_EffectParam );
		LeaveCriticalSection(&m_csEffectInUse);
#endif
*/

	}while (m_pImgEffectThd[iVideoIdx]);

	m_iEffectStatus[iVideoIdx] = -1;
	
	return hr;
}


#ifdef _VF_SINGLE_THREAD
HRESULT CAllocator::UpdateVideo()
{
	HRESULT hr = S_OK;

	char szMsg[512];

	EnterCriticalSection(&m_csEffectInUse);

	struct sEffectD3DParam D3DParam;
	//INT32 iVideoIdx = 0;// = m_iCurThdIdx;
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	D3DParam.d3ddev = pD3DDevice;

	//m_iThdIdx[MAX_FILM];
	for (int i = 0; i < m_surfaces.size(); i++)
	{
		m_iVideoIdx = m_iThdIdx[i];
		if (m_iVideoIdx != -1)
		{
			m_iEffectStatus[m_iVideoIdx] = 0;

			STREM_MAPPING smTemp = theApp.GetStreamMapByIdx(m_iIndex, m_iVideoIdx);
			int iSurfaceIndex = g_MediaStreamAry.at(smTemp.iMediaIdx)->GetSurfaceIndex(m_iIndex);

			if (iSurfaceIndex == -1)
			{
				sprintf_s(szMsg, "#Allocator# UpdateVideo [i = %d , %d , %d , %d , m_iVideoIdx = %d] \n", i, m_iIndex, smTemp.iMediaIdx, iSurfaceIndex, m_iVideoIdx);
				OutputDebugStringA(szMsg);

				continue;
			}

			if (m_pDisplayManager->GetRenderStatus() == RS_DRAW || m_pDisplayManager->GetRenderStatus() == RS_WAIT || m_iCurStatus == STATUS_LOADING || (m_pDisplayManager->GetCurPos(iSurfaceIndex) == m_dCurPos[m_iVideoIdx] && !m_bDoClear))
			//if (m_pDisplayManager->GetRenderStatus() == RS_DRAW || m_pDisplayManager->GetRenderStatus() == RS_WAIT || m_iCurStatus == STATUS_LOADING)
				//if (m_pDisplayManager->GetRenderStatus() == RS_DRAW || m_pDisplayManager->GetRenderStatus() == RS_WAIT || m_iCurStatus == STATUS_LOADING || m_pDisplayManager->GetFrameCount() == m_iLastFrameCount)
			{
				break;// LeaveCriticalSection(&m_csEffectInUse);
			}
			else
			{
				if (m_pDisplayManager->GetRenderStatus() != RS_NORMAL)
					break;

	//			if (m_iIndex >= 1)
				{
					DWORD dwRet = ::WaitForSingleObject(m_hEffect2, 10);

					if (WAIT_TIMEOUT == dwRet)
						break;
				}

				if (!m_BufTexture[m_iVideoIdx])
					break;

				sprintf_s(szMsg, "#Allocator# UpdateVideo [i = %d , %d , %d , %d , m_iVideoIdx = %d] \n", i, m_iIndex, smTemp.iMediaIdx, iSurfaceIndex, m_iVideoIdx);
				OutputDebugStringA(szMsg);

#ifdef _ENABLE_SYNC_PB_CTRL
				if (m_pSyncPBCtrl)
				{
					if (m_iIndex == 0 || !m_bDoPBSync)
					{
						::ResetEvent(m_hEffects[m_iVideoIdx]);

						if (UpdateSurfaceBySample(m_BufTexture[m_iVideoIdx], m_iVideoIdx) == E_FAIL)
							m_bMediaStatusAry[m_iVideoIdx] = FALSE;
						else
						{
							m_bMediaStatusAry[m_iVideoIdx] = TRUE;
							m_lFrameCount[m_iVideoIdx]++;
						}

						m_dCurPos[m_iVideoIdx] = m_pDisplayManager->GetCurPos(iSurfaceIndex);

						//::SetEvent(m_hEffect1);
						::SetEvent(m_hEffects[m_iVideoIdx]);
					}
					else
					{
						if (m_pSyncPBCtrl->GetStatus() == PBCS_UPDATING && m_pSyncPBCtrl->GetPanelStatus(m_iIndex) == PSS_UPDATING)
						{
							::ResetEvent(m_hEffects[m_iVideoIdx]);

							if (UpdateSurfaceBySample(m_BufTexture[m_iVideoIdx], m_iVideoIdx) == E_FAIL)
								m_bMediaStatusAry[m_iVideoIdx] = FALSE;
							else
							{
								m_bMediaStatusAry[m_iVideoIdx] = TRUE;
								m_lFrameCount[m_iVideoIdx]++;
							}

							m_dCurPos[m_iVideoIdx] = m_pDisplayManager->GetCurPos(iSurfaceIndex);

							m_pSyncPBCtrl->SetPanelSourcePos(m_iIndex, i, m_dCurPos[m_iVideoIdx]);
							m_pSyncPBCtrl->SetPanelStatus(m_iIndex, PSS_UPDATED);

							//::SetEvent(m_hEffect1);
							::SetEvent(m_hEffects[m_iVideoIdx]);
						}
						/*
						else
						{
							sprintf(szMsg, "#SyncPBCtrl# [i = %d , m_iIndex = %d , GetStatus() = %d , GetPanelStatus(m_iIndex) = %d ] \n", i, m_iIndex, m_pSyncPBCtrl->GetStatus(), m_pSyncPBCtrl->GetPanelStatus(m_iIndex));
							OutputDebugStringA(szMsg);
						}
						*/
					}
				}
#else
				//::ResetEvent(m_hEffect1);
				::ResetEvent(m_hEffects[m_iVideoIdx]);

				if (UpdateSurfaceBySample(m_BufTexture[m_iVideoIdx], m_iVideoIdx) == E_FAIL)
					m_bMediaStatusAry[m_iVideoIdx] = FALSE;
				else
				{
					m_bMediaStatusAry[m_iVideoIdx] = TRUE;
					m_lFrameCount[m_iVideoIdx]++;
				}

				m_dCurPos[m_iVideoIdx] = m_pDisplayManager->GetCurPos(iSurfaceIndex);

				//::SetEvent(m_hEffect1);
				::SetEvent(m_hEffects[m_iVideoIdx]);
#endif //_ENABLE_SYNC_PB_CTRL

			}
		}
//		Sleep(1);
	}
	LeaveCriticalSection(&m_csEffectInUse);
	Sleep(1);
	return hr;
}
#endif

BOOL CAllocator::Is3DEffect(INT32 iEffectIdx)
{
	BOOL bResult = TRUE;
	SHORT sNot3DEffectAry[200] = {0, 1, 2, 3, 4, 5, 6, 7, 9};

	if (iEffectIdx == 0)
		return FALSE;

	for (int x = 0; x < 200; x++)
	{
		if (!sNot3DEffectAry[x] == iEffectIdx)
		{
			bResult = FALSE;
			break;
		}
	}

	return bResult;
}

HRESULT CAllocator::ShowImageLoop()
{
    HRESULT hr;
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();

	pD3DDevice->SetRenderTarget( 0, m_renderTarget);
	//V( m_scene.DrawScene( m_D3DDev, NULL, -100));
	m_iFixTimes = 0;
	V(pD3DDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L ));
	
	LONGLONG lCurPos, lLength, lAverage = 0;
	  
	CClientDC cdc(m_pParentWnd);
	CBrush brhBlack(RGB(255,0,0));

	INT32 iMediaIdx = -1;
	STREM_MAPPING smTemp = theApp.GetStreamMapByIdx(m_iIndex, m_iCurThdIdx);
	float fFPS = g_MediaStreamAry.at(smTemp.iMediaIdx)->GetFPS();
		
	if (g_PannelSetting.dStartShowTick == 0)
		g_PannelSetting.dStartShowTick = GetTickCount();

	DWORD  timeDiff;
	INT32 iEffectStatus = 0;

	IDirect3DSurface9 *pOutFrameSurface = NULL;
	BOOL bDoEffectAgain = FALSE;
	IDirect3DSurface9 *pOldZBuffer = NULL;
	BOOL bHaveEndSurface;
	BOOL bShowModel;
	BOOL b2DMapStatus; 
	//0: Not 2D Mapping Mode
	//1: 2D Mapping Mode (But not Init)
	//2: 2D Mapping Mode (Had Init)

	m_ptSel = CPoint(-1, -1);
	CPen penDiff(PS_SOLID, 1, RGB(255 * (m_iIndex % 3 == 1), 255 * (m_iIndex % 3 == 2), 255 * (m_iIndex % 3 == 0)));
	CPen penRed(PS_SOLID, 1, RGB(255, 0, 0));
	CPen penBlack(PS_SOLID, 1, RGB(0, 0, 0));
	CPen penArea[6];

	penArea[0].CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	penArea[1].CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	penArea[2].CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	penArea[3].CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
	penArea[4].CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
	penArea[5].CreatePen(PS_SOLID, 1, RGB(255, 0, 255));

	while(m_pPreviewThread)
	{	
		while (m_iCurStatus == STATUS_LOADING)
			Sleep(1);
		DWORD dwCurTime = timeGetTime();
		bHaveEndSurface = g_PannelSetting.bHaveEndSurface;
		bShowModel = !g_PannelSetting.bNotShowModal;

		if (bHaveEndSurface && bShowModel)
		{
			if (m_i2DVerNum != 0)
				b2DMapStatus = 2;
			else
				b2DMapStatus = 1;
		}
		else
			b2DMapStatus = 0;

		if (((g_PannelSetting.iModelAction &0xf) == MODEL_MAP || m_ptAry.GetCount() > 0) && m_ptSel.x != -1)
		{
			DrawAimingPoint();
			continue;
		}
		else if (m_i2DMapAction & MAP2D_NEED_UPDATE_BK)
		{
			m_BKDC->BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(), &cdc, 0, 0, SRCCOPY);
			m_i2DMapAction = m_i2DMapAction &(~MAP2D_NEED_UPDATE_BK);
		}
		else if (m_iBufferAction == 2)
		{
			m_iCurStatus = STATUS_LOADING;
			EnterCriticalSection(&m_csFunBsy);

			INT32 iBaseH = m_rectClient.Height() / 2;
			for (int x = 0; x < m_i2DVerNum; x++)
			{
				if ((m_bSelAry[x] & 0x80) > 0)
				{
					m_vertices[x].position.x += float(m_ptOffset.x) / iBaseH;
					m_vertices[x].position.y -= float(m_ptOffset.y) / iBaseH;
				}
			}
			m_iBufferAction = 0;
			m_ptOffset = CPoint(0, 0);

			void* pData;
			V(m_vertexBuffer->Lock(0, 0, &pData, 0));
			memcpy(pData, m_vertices, sizeof(CAllocator::MAP2DVERTEX) * m_i2DVerNum);
			V(m_vertexBuffer->Unlock());

			m_iCurStatus = STATUS_PREVIEWING;
			LeaveCriticalSection(&m_csFunBsy);
		}
		else if (m_iBufferAction == 3)
		{			
			void* pData;
			V(m_vertexBuffer->Lock(0, 0, &pData, 0));
			memcpy(pData, m_vertices, sizeof(CAllocator::MAP2DVERTEX) * m_i2DVerNum);
			V(m_vertexBuffer->Unlock());
		}
		else if (m_iBufferAction == 1)
		{
			m_iBufferAction = 0;
			m_uCurPtArea = 0;

			for (int x = 0; x < m_i2DVerNum; x++)
			{
				m_bSelAry[x] &= 0x7f;
			}
		}
		else if (m_i2DMapAction & MAP2D_SHOW_BK_PT)
		{
			CPen *pedNew = NULL;
			HGDIOBJ pOldPen;
			INT32 iBaseH = m_rectClient.Height() / 2;
			INT32 iBaseW = m_rectClient.Width() / 2;
			D3DCOLOR    color;
			CPen penWhite(PS_SOLID, 1, RGB(255, 255, 255));
			BOOL bSelFlag1 = FALSE;
			BOOL bSelFlag2 = FALSE;
			CPoint ptTemp1, ptTemp2;

			m_MemoryDC->BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(), m_BKDC, 0, 0, SRCCOPY);
			pOldPen = m_MemoryDC->SelectObject(penDiff);

			for (int x = 0; x < m_i2DVerNum; x += 6)
			{
				/*color = m_vertices[x].color;
				pedNew = new CPen(PS_SOLID, 1, RGB(0xff &(color >>16), 0xff & (color >> 8), 0xff & color));

				if (x == 0)
				pOldPen = m_MemoryDC->SelectObject(pedNew);
				else
				m_MemoryDC->SelectObject(pedNew);*/
				bSelFlag1 = bSelFlag2 = FALSE;				

				if ((m_bSelAry[x] & 0x80) > 0)
				{
					bSelFlag1 = TRUE;
				}

				if ((m_bSelAry[x + 2] & 0x80) > 0)
				{
					bSelFlag2 = TRUE;
				}

				if (bSelFlag1 && bSelFlag2)
					m_MemoryDC->SelectObject(penWhite);
				else
					m_MemoryDC->SelectObject(penDiff);

				ptTemp1 = CPoint(m_vertices[x].position.x * iBaseH + iBaseW, -m_vertices[x].position.y * iBaseH + iBaseH);
				ptTemp2 = CPoint(m_vertices[x + 2].position.x * iBaseH + iBaseW, -m_vertices[x + 2].position.y * iBaseH + iBaseH);

				if (bSelFlag1)
					ptTemp1 += m_ptOffset;

				if (bSelFlag2)
					ptTemp2 += m_ptOffset;

				m_MemoryDC->MoveTo(ptTemp1);
				m_MemoryDC->LineTo(ptTemp2);

				//delete pedNew;
			}

			for (int x = 0; x < m_i2DVerNum; x += 6)
			{
				/*color = m_vertices[x].color;
				pedNew = new CPen(PS_SOLID, 1, RGB(0xff & (color >> 16), 0xff & (color >> 8), 0xff & color));
				m_MemoryDC->SelectObject(pedNew);*/

				bSelFlag1 = bSelFlag2 = FALSE;

				if ((m_bSelAry[x] & 0x80) > 0)
				{
					bSelFlag1 = TRUE;
				}

				if ((m_bSelAry[x + 1] & 0x80) > 0)
				{
					bSelFlag2 = TRUE;
				}

				if (bSelFlag1 && bSelFlag2)
					m_MemoryDC->SelectObject(penWhite);
				else
					m_MemoryDC->SelectObject(penDiff);

				ptTemp1 = CPoint(m_vertices[x].position.x * iBaseH + iBaseW, -m_vertices[x].position.y * iBaseH + iBaseH);
				ptTemp2 = CPoint(m_vertices[x + 1].position.x * iBaseH + iBaseW, -m_vertices[x + 1].position.y * iBaseH + iBaseH);

				if (bSelFlag1)
					ptTemp1 += m_ptOffset;

				if (bSelFlag2)
					ptTemp2 += m_ptOffset;

				m_MemoryDC->MoveTo(ptTemp1);
				m_MemoryDC->LineTo(ptTemp2);

				//delete pedNew;
			}

			m_MemoryDC->SelectObject(pOldPen);
			if (m_ptAreaSel.bottom > 0)
			{
				CClientDC cdc(m_pParentWnd);
				CBrush brhWhite(RGB(255, 255, 255));
				m_MemoryDC->FrameRect(m_ptAreaSel, &brhWhite);
			}

			cdc.BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(), m_MemoryDC, 0, 0, SRCCOPY);
			continue;
		}

		if (m_pDisplayManager->IsSuspend())
			continue;

		if (bHaveEndSurface && m_surfaces.size() > 0 && b2DMapStatus != 1)
		{
			V(m_EndTexture->GetSurfaceLevel(0, &pOutFrameSurface));
			V(pD3DDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0L));
			V(pD3DDevice->SetRenderTarget(0, pOutFrameSurface));
			pD3DDevice->GetDepthStencilSurface(&pOldZBuffer);
			V(pD3DDevice->SetDepthStencilSurface(m_EndSurface));
			V(pD3DDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0L));

			D3DSURFACE_DESC rc;
			pOutFrameSurface->GetDesc(&rc);
		}
		else
			pD3DDevice->SetRenderTarget( 0, m_renderTarget);

		m_iDeviceStatus = pD3DDevice->ValidateDevice(&m_iNumPasses);

		if (m_iIndex == 0)
			m_lSyncTick = GetTickCount();

		for (UINT y = 0; y < m_surfaces.size(); y++)
		{
			m_iEffectIdx[y] = g_PannelSetting.iImageEffect[y];
		}

#if 1

		while (m_pPreviewThread && (m_pDisplayManager->IsD3DUsing() == 2 || !IsWindowVisible(m_pParentWnd->m_hWnd)))
		//while (m_pPreviewThread && m_pDisplayManager->IsD3DUsing() == 2)
		{
			Sleep(1);
		}

		if (!m_pPreviewThread)
			break;

		m_pDisplayManager->UsingD3D(1);


		//while (m_iCurStatus == STATUS_LOADING)
		//	Sleep(1);

		EnterCriticalSection(&m_csFunBsy);

		//#ifndef MULTI_STREAM
		for (UINT y = 0; y < m_surfaces.size(); y++)
		{
			/*#ifdef NOT_PROC_EFFECT
				UpdateSurfaceBySample(m_BufTexture[y], y);
			#else*/
			m_iEffectStatus[y] = 1;
			iEffectStatus = 1;
			//#endif
		}

		while (m_pPreviewThread && iEffectStatus > 0)
		{
			iEffectStatus = 0;

			for (UINT y = 0; y < m_surfaces.size(); y++)
			{
				iEffectStatus += m_iEffectStatus[y];
			}
		}

		if (!m_pPreviewThread)
		{
			m_pDisplayManager->UsingD3D(0);
			LeaveCriticalSection(&m_csFunBsy);
			break;
		}
#endif

		if (bDoEffectAgain)
		{
			for (UINT y = 0; y < m_surfaces.size(); y++)
			{
				m_iEffectStatus[y] = 11;
			}

			while (m_pPreviewThread && iEffectStatus > 0)
			{
				iEffectStatus = 0;

				for (UINT y = 0; y < m_surfaces.size(); y++)
				{
					iEffectStatus += m_iEffectStatus[y];
				}
			}
			
			if (!m_pPreviewThread)
			{
				m_pDisplayManager->UsingD3D(0);
				LeaveCriticalSection(&m_csFunBsy);
				break;
			}
		}

		if (bShowModel || m_surfaces.size() == 0)
		{
#ifdef PLAY_3D_MODE
			//V(m_scene.DrawScene(m_D3DDev, NULL, g_PannelSetting.iSelIdx, g_PannelSetting.iModelAction, -100));
			hr = m_scene.DrawScene(pD3DDevice, NULL, g_PannelSetting.iSelIdx, g_PannelSetting.iModelAction, -100);
			if (FAILED(hr))
			{
				char szMsg[512];
				sprintf_s(szMsg, "#Allocator# DrawScene failed!! \n");
				OutputDebugStringA(szMsg);
			}
#else
			V(m_scene.DrawScene(m_D3DDev, NULL));
#endif
		}

		if (m_surfaces.size() == 0)
		{
			#ifdef PLAY_3D_MODE
				V(m_scene.DrawScene(pD3DDevice, NULL, g_PannelSetting.iSelIdx, g_PannelSetting.iModelAction, 0));
			#else
				V(m_scene.DrawScene(m_D3DDev, NULL));
			#endif
		}

		
		for (UINT x = 0; x < m_BufTexture.size() && bShowModel; x++)
		{
			//while (m_iCurStatus == STATUS_LOADING)
			//	Sleep(1);
			#ifdef PLAY_3D_MODE
			if (m_bMediaStatusAry[x])
			{
				hr = m_scene.DrawScene(pD3DDevice, m_BufTexture[x], g_PannelSetting.iSelIdx,
					g_PannelSetting.iModelAction, x);
				if (FAILED(hr))
				{
					char szMsg[512];
					sprintf_s(szMsg, "#Allocator# DrawScene failed!! \n");
					OutputDebugStringA(szMsg);
				}
				//V(m_scene.DrawScene(m_D3DDev, m_BufTexture[x], g_PannelSetting.iSelIdx,
				//	g_PannelSetting.iModelAction, x));
			}
			#else
				V( m_scene.DrawScene(m_D3DDev, m_BufTexture[x]));
			#endif
		}

		if (!bHaveEndSurface || m_surfaces.size() == 0 || b2DMapStatus == 1)
		{
			//while (m_iCurStatus == STATUS_LOADING)
			//	Sleep(1);

			//V(m_D3DDev->EndScene());//Sometimes will crashed at here
			hr = pD3DDevice->EndScene();//Sometimes will crashed at here
			if (FAILED(hr))
			{
				char szMsg[512];
				sprintf_s(szMsg, "#Allocator# EndScene failed!! \n");
				OutputDebugStringA(szMsg);
			}

#ifdef _ENABLE_DISPLAY_MANAGER
			float fInterval = 1000.0 / fFPS;
			if (dwCurTime - m_dwLastTime >= fInterval * 2.0 && m_pDisplayManager->IsRecordAVI())
				m_pDisplayManager->CaptureRenderTarget(m_renderTarget);
#endif
				if (IsWindowVisible(m_pParentWnd->m_hWnd))
				{
					if (0)//m_iIndex == 0 && g_DispArys.GetCount() != 1)
					{
						CRect rectDisp = g_DispArys.GetAt(m_iIndex);

						CRect rectDest;
						rectDest.left = 165 * rectDisp.Width() / 1920;
						rectDest.top = 160 * rectDisp.Height() / 1080;
						rectDest.right = 1720 * rectDisp.Width() / 1920;
						rectDest.bottom = 1034 * rectDisp.Height() / 1080;//867

						V(pD3DDevice->Present(NULL, &rectDest, NULL, NULL));
					}
					else
					{
#ifdef _ENABLE_D3D9EX
						if (m_bIsD3D9Ex)
						{
							hr = ((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(0, 0, 0, 0, 0);
						}

#else
						hr = pD3DDevice->Present(NULL, NULL, NULL, NULL);
#endif
					}

					if (hr == D3DERR_DEVICELOST)
					{
						if (pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
						{
							DeleteSurfaces();
							if (pD3DDevice)
								pD3DDevice->Release();

							if (pD3D9Ptr)
								pD3D9Ptr->Release();

#ifdef _ENABLE_D3D9EX
							if (m_bIsD3D9Ex)
								Direct3DCreate9Ex(D3D_SDK_VERSION, &m_pD3DEx);
							else
								m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
#else
							m_D3D = Direct3DCreate9(D3D_SDK_VERSION);
#endif
							V(CreateDevice());

							V(InitShareSurface());
						}
					}
				}
			}
		else
		{
			if (bShowModel)
				V(pD3DDevice->EndScene());

			D3DXMATRIX matView, matProj;
			V(pD3DDevice->GetTransform(D3DTS_VIEW, &matView));
			V(pD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj));
			V(pD3DDevice->SetTransform(D3DTS_VIEW, &m_matEndView));
			V(pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_matEndProj));
			
			pD3DDevice->SetRenderTarget(0, m_renderTarget);
			V(pD3DDevice->SetDepthStencilSurface(pOldZBuffer));
			V(pD3DDevice->BeginScene());

			if (bShowModel)
			{
				//V(m_D3DDev->SetRenderState(D3DRS_LIGHTING, TRUE));
				V(pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE));

				D3DLIGHT9 light;
				ZeroMemory(&light, sizeof(light));
				light.Type = D3DLIGHT_DIRECTIONAL;
				light.Ambient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
				light.Diffuse = D3DXCOLOR(0.03f, 0.03f, 0.03f, 1.0f);
				light.Specular = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
				light.Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
				//V(m_D3DDev->SetLight(0, &light));
				//V(m_D3DDev->LightEnable(0, TRUE));

				if (m_iTextureIdx != 0)
				{
					V(pD3DDevice->SetTexture(0, m_TextureAry[m_iTextureIdx - 1]));
				}
				else
				{
					if (0)
					{
						D3DGAMMARAMP gdata;

						pD3DDevice->GetGammaRamp(false, &gdata);
						unsigned short rp; 
						for (rp = 0; rp < 256; rp++)
						{
							gdata.red [rp] = 0; gdata.green [rp] = 0; gdata.blue [rp] = 0; 
						}; 
						pD3DDevice->SetGammaRamp(false, D3DSGR_NO_CALIBRATION, &gdata);
					}

					V(pD3DDevice->StretchRect(pOutFrameSurface, NULL, m_renderTarget, NULL, D3DTEXF_NONE));
					V(pD3DDevice->SetTexture(0, m_EndTexture));
				}
				V(pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1));
				V(pD3DDevice->SetStreamSource(0, m_vertexBuffer, 0, sizeof(CAllocator::MAP2DVERTEX)));
				V(pD3DDevice->SetVertexShader(NULL));
				V(pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_i2DVerNum/3));
				V(pD3DDevice->SetTexture(0, NULL));

				V(pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE));
				//V(m_D3DDev->LightEnable(0, FALSE));
				V(pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE));

			}
			else if (m_iSelObjIdx >= 0 && m_iSelObjIdx < m_BufTexture.size())
			{
				IDirect3DSurface9 *pVideoSurface = NULL;
				m_BufTexture[m_iSelObjIdx]->GetSurfaceLevel(0, &pVideoSurface);
				V(pD3DDevice->StretchRect(pVideoSurface, NULL, m_renderTarget, NULL, D3DTEXF_NONE));
				pVideoSurface->Release();
			}

			V(pD3DDevice->EndScene());
			V(pD3DDevice->SetTexture(0, NULL));
#ifdef _ENABLE_D3D9EX
			if (m_bIsD3D9Ex)
			{
				hr = ((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(0, 0, 0, 0, 0);
			}

#else
			V(pD3DDevice->Present(NULL, NULL, NULL, NULL));
#endif
			V(pD3DDevice->SetTransform(D3DTS_VIEW, &matView));
			V(pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj));

		}
		
		m_pDisplayManager->UsingD3D(0);
		LeaveCriticalSection(&m_csFunBsy);
	 
		LONGLONG lpRocessTime = GetTickCount() - m_lSyncTick;

		if (m_ptAreaSel.bottom > 0)
		{
			CClientDC cdc(m_pParentWnd);
			CBrush brhRed(RGB(255, 0, 0));
			cdc.FrameRect(m_ptAreaSel, &brhRed);
		}


#ifdef _ENABLE_DISPLAY_MANAGER
		float fInterval = 1000.0 / fFPS;
		
		if (m_pDisplayManager->IsRecordAVI())
		{
			fInterval *= 4;
			Sleep(DWORD(max(0, fInterval - lpRocessTime)));
		}
		else
#endif
		if (!(g_PannelSetting.iModelAction & (MODEL_MODIFY | MODEL_SHOW_MODEL_LINE | MODEL_RUN_MODEL_LINE))
			&& g_PannelSetting.iModelAction != MODEL_MAP)
		{
			Sleep(DWORD(max(0, fInterval - lpRocessTime)));
		}

		CalPresentFPS();

		m_dwLastTime = dwCurTime;
	}

	m_bPreviewFlag = FALSE;

	if (pOutFrameSurface)
		pOutFrameSurface->Release();

	return hr;
}

#ifdef _ENABLE_FLIP_THREAD

void CAllocator::InitGDI()
{
	//CPen penDiff(PS_SOLID, 1, RGB(255 * (m_iIndex % 3 == 1), 255 * (m_iIndex % 3 == 2), 255 * (m_iIndex % 3 == 0)));
	//CPen penRed(PS_SOLID, 1, RGB(255, 0, 0));
	//CPen penBlack(PS_SOLID, 1, RGB(0, 0, 0));

	m_penDiff.CreatePen(PS_SOLID, 1, RGB(255 * (m_iIndex % 3 == 1), 255 * (m_iIndex % 3 == 2), 255 * (m_iIndex % 3 == 0)));
	m_penRed.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	m_penBlack.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	m_penWhite.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));

	//	CPen penArea[6];

	m_penArea[0].CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	m_penArea[1].CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	m_penArea[2].CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	m_penArea[3].CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
	m_penArea[4].CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
	m_penArea[5].CreatePen(PS_SOLID, 1, RGB(255, 0, 255));

	m_pCDC = new CClientDC(m_pParentWnd);

	m_brhBlack.CreateSolidBrush(RGB(0, 0, 0));
	m_brhWhite.CreateSolidBrush(RGB(255, 255, 255));
	m_brhRed.CreateSolidBrush(RGB(255, 0, 0));
}

void CAllocator::DeleteGDI()
{
	m_penDiff.DeleteObject();
	m_penRed.DeleteObject();
	m_penBlack.DeleteObject();
	m_penWhite.DeleteObject();

	for (int i = 0; i < 6; i++)
		m_penArea[i].DeleteObject();

	m_brhBlack.DeleteObject();
	m_brhWhite.DeleteObject();
	m_brhRed.DeleteObject();

	if (m_pCDC)
		delete m_pCDC;
}

HRESULT CAllocator::ShowVideoLoop()
{
	HRESULT hr = S_OK;
	char szMsg[512];

	INT32 iEffectStatus = 0;

	INT32 iMediaIdx = -1;
	STREM_MAPPING smTemp = theApp.GetStreamMapByIdx(m_iIndex, m_iCurThdIdx);
	float fFPS = g_MediaStreamAry.at(smTemp.iMediaIdx)->GetFPS();

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	IDirect3DSurface9 *pOutFrameSurface = NULL;
	BOOL bDoEffectAgain = FALSE;
	IDirect3DSurface9 *pOldZBuffer = NULL;
	BOOL bHaveEndSurface;
	BOOL bShowModel;
	BOOL b2DMapStatus;

	DWORD dwCurTime = timeGetTime();
	bHaveEndSurface = g_PannelSetting.bHaveEndSurface;
	bShowModel = !g_PannelSetting.bNotShowModal;

//	::SetEvent(m_hEffect2);

	if (bHaveEndSurface && bShowModel)
	{
		if (m_i2DVerNum != 0)
			b2DMapStatus = 2;
		else
			b2DMapStatus = 1;
	}
	else
		b2DMapStatus = 0;

	if (((g_PannelSetting.iModelAction & 0xf) == MODEL_MAP || m_ptAry.GetCount() > 0) && m_ptSel.x != -1)
	{
		DrawAimingPoint();
		return hr;// continue;
	}
	else if (m_i2DMapAction & MAP2D_NEED_UPDATE_BK)
	{
		m_BKDC->BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(), m_pCDC, 0, 0, SRCCOPY);
		m_i2DMapAction = m_i2DMapAction &(~MAP2D_NEED_UPDATE_BK);
	}
	else if (m_iBufferAction == 2)
	{
		m_iCurStatus = STATUS_LOADING;
		EnterCriticalSection(&m_csFunBsy);

		INT32 iBaseH = m_rectClient.Height() / 2;
		for (int x = 0; x < m_i2DVerNum; x++)
		{
			if ((m_bSelAry[x] & 0x80) > 0)
			{
				m_vertices[x].position.x += float(m_ptOffset.x) / iBaseH;
				m_vertices[x].position.y -= float(m_ptOffset.y) / iBaseH;
			}
		}
		m_iBufferAction = 0;
		m_ptOffset = CPoint(0, 0);

		void* pData;
		V(m_vertexBuffer->Lock(0, 0, &pData, 0));
		memcpy(pData, m_vertices, sizeof(CAllocator::MAP2DVERTEX) * m_i2DVerNum);
		V(m_vertexBuffer->Unlock());

		m_iCurStatus = STATUS_PREVIEWING;
		LeaveCriticalSection(&m_csFunBsy);
	}
	else if (m_iBufferAction == 3)
	{
		void* pData;
		V(m_vertexBuffer->Lock(0, 0, &pData, 0));
		memcpy(pData, m_vertices, sizeof(CAllocator::MAP2DVERTEX) * m_i2DVerNum);
		V(m_vertexBuffer->Unlock());
	}
	else if (m_iBufferAction == 1)
	{
		m_iBufferAction = 0;
		m_uCurPtArea = 0;

		for (int x = 0; x < m_i2DVerNum; x++)
		{
			m_bSelAry[x] &= 0x7f;
		}
	}
	else if (m_i2DMapAction & MAP2D_SHOW_BK_PT)
	{
		CPen *pedNew = NULL;
		HGDIOBJ pOldPen;
		INT32 iBaseH = m_rectClient.Height() / 2;
		INT32 iBaseW = m_rectClient.Width() / 2;
		D3DCOLOR    color;
		//		CPen penWhite(PS_SOLID, 1, RGB(255, 255, 255));
		BOOL bSelFlag1 = FALSE;
		BOOL bSelFlag2 = FALSE;
		CPoint ptTemp1, ptTemp2;

		m_MemoryDC->BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(), m_BKDC, 0, 0, SRCCOPY);
		pOldPen = m_MemoryDC->SelectObject(m_penDiff);

		for (int x = 0; x < m_i2DVerNum; x += 6)
		{
			/*color = m_vertices[x].color;
			pedNew = new CPen(PS_SOLID, 1, RGB(0xff &(color >>16), 0xff & (color >> 8), 0xff & color));

			if (x == 0)
			pOldPen = m_MemoryDC->SelectObject(pedNew);
			else
			m_MemoryDC->SelectObject(pedNew);*/
			bSelFlag1 = bSelFlag2 = FALSE;

			if ((m_bSelAry[x] & 0x80) > 0)
			{
				bSelFlag1 = TRUE;
			}

			if ((m_bSelAry[x + 2] & 0x80) > 0)
			{
				bSelFlag2 = TRUE;
			}

			if (bSelFlag1 && bSelFlag2)
				m_MemoryDC->SelectObject(m_penWhite);
			else
				m_MemoryDC->SelectObject(m_penDiff);

			ptTemp1 = CPoint(m_vertices[x].position.x * iBaseH + iBaseW, -m_vertices[x].position.y * iBaseH + iBaseH);
			ptTemp2 = CPoint(m_vertices[x + 2].position.x * iBaseH + iBaseW, -m_vertices[x + 2].position.y * iBaseH + iBaseH);

			if (bSelFlag1)
				ptTemp1 += m_ptOffset;

			if (bSelFlag2)
				ptTemp2 += m_ptOffset;

			m_MemoryDC->MoveTo(ptTemp1);
			m_MemoryDC->LineTo(ptTemp2);

			//delete pedNew;
		}

		for (int x = 0; x < m_i2DVerNum; x += 6)
		{
			/*color = m_vertices[x].color;
			pedNew = new CPen(PS_SOLID, 1, RGB(0xff & (color >> 16), 0xff & (color >> 8), 0xff & color));
			m_MemoryDC->SelectObject(pedNew);*/

			bSelFlag1 = bSelFlag2 = FALSE;

			if ((m_bSelAry[x] & 0x80) > 0)
			{
				bSelFlag1 = TRUE;
			}

			if ((m_bSelAry[x + 1] & 0x80) > 0)
			{
				bSelFlag2 = TRUE;
			}

			if (bSelFlag1 && bSelFlag2)
				m_MemoryDC->SelectObject(m_penWhite);
			else
				m_MemoryDC->SelectObject(m_penDiff);

			ptTemp1 = CPoint(m_vertices[x].position.x * iBaseH + iBaseW, -m_vertices[x].position.y * iBaseH + iBaseH);
			ptTemp2 = CPoint(m_vertices[x + 1].position.x * iBaseH + iBaseW, -m_vertices[x + 1].position.y * iBaseH + iBaseH);

			if (bSelFlag1)
				ptTemp1 += m_ptOffset;

			if (bSelFlag2)
				ptTemp2 += m_ptOffset;

			m_MemoryDC->MoveTo(ptTemp1);
			m_MemoryDC->LineTo(ptTemp2);

			//delete pedNew;
		}

		m_MemoryDC->SelectObject(pOldPen);
		if (m_ptAreaSel.bottom > 0)
		{
			CClientDC cdc(m_pParentWnd);
			CBrush brhWhite(RGB(255, 255, 255));
			m_MemoryDC->FrameRect(m_ptAreaSel, &brhWhite);
		}

		m_pCDC->BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(), m_MemoryDC, 0, 0, SRCCOPY);
		return hr;// continue;
	}

	if (m_pDisplayManager->IsSuspend())
	{
		return -1;
	}

	if (bHaveEndSurface && m_surfaces.size() > 0 && b2DMapStatus != 1)
	{
		V(m_EndTexture->GetSurfaceLevel(0, &pOutFrameSurface));
		V(pD3DDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0L));
		V(pD3DDevice->SetRenderTarget(0, pOutFrameSurface));
		pD3DDevice->GetDepthStencilSurface(&pOldZBuffer);
		V(pD3DDevice->SetDepthStencilSurface(m_EndSurface));
		V(pD3DDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0L));

		D3DSURFACE_DESC rc;
		pOutFrameSurface->GetDesc(&rc);
	}
	else
		pD3DDevice->SetRenderTarget(0, m_renderTarget);

	m_iDeviceStatus = pD3DDevice->ValidateDevice(&m_iNumPasses);

//	if (m_iIndex == 0)
		m_lSyncTick = GetTickCount();

	for (UINT y = 0; y < m_surfaces.size(); y++)
	{
		m_iEffectIdx[y] = g_PannelSetting.iImageEffect[y];
	}

#if 1

	//while (((m_pDisplayManager && m_pDisplayManager->IsD3DUsing() == 2) || !IsWindowVisible(m_pParentWnd->m_hWnd)))
	while (m_pDisplayManager && m_pDisplayManager->IsD3DUsing() == 2 || m_pDisplayManager->GetRenderStatus() == RS_WAIT)
	{
		if (!m_pDisplayManager)
			break;
		
		Sleep(1);
	}

	//	if (!m_pPreviewThread)
	//		return hr;// break;

	if (m_pDisplayManager)
		m_pDisplayManager->UsingD3D(1);

	//m_hEffects
	HANDLE handles[10];
	for(int j = 0;j<10;j++)
		handles[j] = m_hEffects[j];
	
	DWORD dwRet = WaitForMultipleObjects(10, handles, true, INFINITE);  //1000
	//::WaitForSingleObject(m_hEffect1, INFINITE);

	//if (dwCurTime < m_dwLastTime + 40)//40,33
	if (dwCurTime < m_dwLastTime + m_iPresentInterval)//40,33
	{
		Sleep(1);

		if (m_pDisplayManager)
		{
			m_pDisplayManager->UsingD3D(0);
			m_pDisplayManager->SetRenderStatus(RS_NORMAL);
		}

//		::SetEvent(m_hEffect2);
		return hr;
	}

#ifdef _ENABLE_SYNC_PB_CTRL
	if (m_pSyncPBCtrl)
	{
		if (m_pSyncPBCtrl->GetStatus() == PBCS_PRESENTING && m_pSyncPBCtrl->GetPanelStatus(m_iIndex) == PSS_PRESENTING || m_iIndex == 0 || !m_bDoPBSync)
			;
		else
		{
			if (m_pDisplayManager)
			{
				m_pDisplayManager->UsingD3D(0);
				m_pDisplayManager->SetRenderStatus(RS_NORMAL);
			}

			//		::SetEvent(m_hEffect2);
			return hr;
		}
	}
#endif

	::ResetEvent(m_hEffect2);

	//while (m_iCurStatus == STATUS_LOADING)
	//	Sleep(1);

	if (m_bDoClear)
	{
	//	if(m_iWaitCount == 0)
			m_bDoClear = false;
	/*
		m_iWaitCount--;
		if (m_iWaitCount < 0)
			m_iWaitCount = 0;
	*/
		if (m_pDisplayManager)
			m_pDisplayManager->UsingD3D(0);

		::SetEvent(m_hEffect2);
	}
	else
		DoEffect();

//	EnterCriticalSection(&m_csFunBsy);

	//#ifndef MULTI_STREAM
	for (UINT y = 0; y < m_surfaces.size(); y++)
	{
		/*#ifdef NOT_PROC_EFFECT
		UpdateSurfaceBySample(m_BufTexture[y], y);
		#else*/
		m_iEffectStatus[y] = 1;
		iEffectStatus = 1;
		//#endif
	}


#ifdef _VF_SINGLE_THREAD
#else
	//while (m_pPreviewThread && iEffectStatus > 0)
	while (iEffectStatus > 0)
	{
		iEffectStatus = 0;

		for (UINT y = 0; y < m_surfaces.size(); y++)
		{
			iEffectStatus += m_iEffectStatus[y];
		}
	}

	if (bDoEffectAgain)
	{
		for (UINT y = 0; y < m_surfaces.size(); y++)
		{
			m_iEffectStatus[y] = 11;
		}

		//while (m_pPreviewThread && iEffectStatus > 0)
		while (iEffectStatus > 0)
		{
			iEffectStatus = 0;

			for (UINT y = 0; y < m_surfaces.size(); y++)
			{
				iEffectStatus += m_iEffectStatus[y];
			}
		}
		/*
		if (!m_pPreviewThread)
		{
		m_pDisplayManager->UsingD3D(0);
		LeaveCriticalSection(&m_csFunBsy);
		return hr;// break;
		}
		*/
	}
#endif
	/*
	if (!m_pPreviewThread)
	{
	m_pDisplayManager->UsingD3D(0);
	LeaveCriticalSection(&m_csFunBsy);
	return hr;// break;
	}
	*/
#endif

	if (!m_bIsAlive)
	{
		if (m_pDisplayManager)
			m_pDisplayManager->UsingD3D(0);
//		LeaveCriticalSection(&m_csFunBsy);

		::SetEvent(m_hEffect2);
		return hr;
	}

	m_pDisplayManager->SetRenderStatus(RS_DRAW);

	if (bShowModel || m_surfaces.size() == 0)
	{
#ifdef PLAY_3D_MODE

		//V(m_scene.DrawScene(m_D3DDev, NULL, g_PannelSetting.iSelIdx, g_PannelSetting.iModelAction, -100));
		hr = m_scene.DrawScene(pD3DDevice, NULL, g_PannelSetting.iSelIdx, g_PannelSetting.iModelAction, -100);
		if (FAILED(hr))
		{
			sprintf_s(szMsg, "#Allocator# DrawScene failed!! hr = %d\n", hr);
			OutputDebugStringA(szMsg);

			if (hr == -2)
			{
				m_pDisplayManager->SetRenderStatus(RS_NORMAL);
				m_pDisplayManager->UsingD3D(0);
//				LeaveCriticalSection(&m_csFunBsy);
				::SetEvent(m_hEffect2);

#ifdef _ENABLE_SYNC_PB_CTRL
				if (m_pSyncPBCtrl)
				{
					m_pSyncPBCtrl->SetPanelStatus(m_iIndex, PSS_PRESENTED);
				}
#endif  //_ENABLE_SYNC_PB_CTRL

				return hr;
			}
		}
#else
		V(m_scene.DrawScene(m_D3DDev, NULL));
#endif
	}

	if (m_surfaces.size() == 0)
	{
#ifdef PLAY_3D_MODE

		//V(m_scene.DrawScene(m_D3DDev, NULL, g_PannelSetting.iSelIdx, g_PannelSetting.iModelAction, 0));
		hr = m_scene.DrawScene(pD3DDevice, NULL, g_PannelSetting.iSelIdx, g_PannelSetting.iModelAction, 0);
		if (FAILED(hr))
		{
			sprintf_s(szMsg, "#Allocator# DrawScene failed!!  hr = %d\n", hr);
			OutputDebugStringA(szMsg);

			if (hr == -2)
			{
				m_pDisplayManager->SetRenderStatus(RS_NORMAL);
				m_pDisplayManager->UsingD3D(0);
//				LeaveCriticalSection(&m_csFunBsy);
				::SetEvent(m_hEffect2);

#ifdef _ENABLE_SYNC_PB_CTRL
				if (m_pSyncPBCtrl)
				{
					m_pSyncPBCtrl->SetPanelStatus(m_iIndex, PSS_PRESENTED);
			}
#endif  //_ENABLE_SYNC_PB_CTRL

				return hr;
			}
		}
#else
		V(m_scene.DrawScene(m_D3DDev, NULL));
#endif
	}


	for (UINT x = 0; x < m_BufTexture.size() && bShowModel; x++)
	{
		//while (m_iCurStatus == STATUS_LOADING)
		//	Sleep(1);
#ifdef PLAY_3D_MODE
		if (m_bMediaStatusAry[x] && m_lFrameCount[x] > 2)
		{
			try
			{
				hr = m_scene.DrawScene(pD3DDevice, m_BufTexture[x], g_PannelSetting.iSelIdx,
					g_PannelSetting.iModelAction, x);
			}
			catch (...)
			{
				OutputDebugString("HEError - Catching an exception in ShowVideoLoop - m_scene.DrawScene !! \n");
//				::MessageBoxA(NULL, "LoadObj() Failed!!", "", MB_OK);
			}

			if (FAILED(hr))
			{
				sprintf_s(szMsg, "#Allocator# DrawScene failed!! [%d]\n",x);
				OutputDebugStringA(szMsg);

				if (hr == -2)
				{
					m_pDisplayManager->SetRenderStatus(RS_NORMAL);
					m_pDisplayManager->UsingD3D(0);
//					LeaveCriticalSection(&m_csFunBsy);
					::SetEvent(m_hEffect2);

#ifdef _ENABLE_SYNC_PB_CTRL
					if (m_pSyncPBCtrl)
					{
						m_pSyncPBCtrl->SetPanelStatus(m_iIndex, PSS_PRESENTED);
					}
#endif  //_ENABLE_SYNC_PB_CTRL

					return hr;
				}
			}
			//V(m_scene.DrawScene(m_D3DDev, m_BufTexture[x], g_PannelSetting.iSelIdx,
			//	g_PannelSetting.iModelAction, x));
		}
#else
		V(m_scene.DrawScene(m_D3DDev, m_BufTexture[x]));
#endif
	}

	if (!bHaveEndSurface || m_surfaces.size() == 0 || b2DMapStatus == 1)
	{
		//while (m_iCurStatus == STATUS_LOADING)
		//	Sleep(1);

		//V(m_D3DDev->EndScene());//Sometimes will crashed at here

		try
		{
			hr = pD3DDevice->EndScene();//Sometimes will crashed at here
//			m_scene.DecreaseDrawSceneCount();
		}
		catch (...)
		{
			OutputDebugString("HEError - Catching an exception in ShowVideoLoop - pD3DDevice->EndScene() !! \n");
			//				::MessageBoxA(NULL, "LoadObj() Failed!!", "", MB_OK);
		}

		if (FAILED(hr))
		{
			sprintf_s(szMsg, "#Allocator# EndScene failed!! \n");
			OutputDebugStringA(szMsg);
		}

#ifdef _ENABLE_DISPLAY_MANAGER
		float fInterval = 1000.0 / fFPS;
		if (dwCurTime - m_dwLastTime >= fInterval * 2.0 && m_pDisplayManager->IsRecordAVI())
			m_pDisplayManager->CaptureRenderTarget(m_renderTarget);
#endif
		m_pDisplayManager->SetRenderStatus(RS_PRESENT);

#ifdef _ENABLE_D3D9EX
		//if(m_iIndex == 0)
			DrawSelArea();
#endif

#ifdef _TRAIL_VERSION
		DrawWatermark();
#endif

		DWORD dwTime4 = timeGetTime();
		Flip();
		DWORD dwTime5 = timeGetTime();

		if (m_iIndex != 0)
		{
			int iVideoIdx = m_iThdIdx[0];
			STREM_MAPPING smTemp = theApp.GetStreamMapByIdx(m_iIndex, m_iVideoIdx);
			int iSurfaceIndex = g_MediaStreamAry.at(smTemp.iMediaIdx)->GetSurfaceIndex(m_iIndex);

			sprintf(szMsg, "#MSE# [Panel: %d] Allocator::ShowVideoLoop() - Flip : %d (%d) %5.2f\n", m_iIndex, dwTime5 - dwTime4, dwTime5, m_dCurPos[iVideoIdx]);
			OutputDebugStringA(szMsg);
		}
	}
	else
	{
		if (bShowModel)
		{
			V(pD3DDevice->EndScene());
//			m_scene.DecreaseDrawSceneCount();
		}

		D3DXMATRIX matView, matProj;
		V(pD3DDevice->GetTransform(D3DTS_VIEW, &matView));
		V(pD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj));
		V(pD3DDevice->SetTransform(D3DTS_VIEW, &m_matEndView));
		V(pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_matEndProj));

		pD3DDevice->SetRenderTarget(0, m_renderTarget);
		V(pD3DDevice->SetDepthStencilSurface(pOldZBuffer));
		V(pD3DDevice->BeginScene());

		if (bShowModel)
		{
			//V(m_D3DDev->SetRenderState(D3DRS_LIGHTING, TRUE));
			V(pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE));

			D3DLIGHT9 light;
			ZeroMemory(&light, sizeof(light));
			light.Type = D3DLIGHT_DIRECTIONAL;
			light.Ambient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
			light.Diffuse = D3DXCOLOR(0.03f, 0.03f, 0.03f, 1.0f);
			light.Specular = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
			light.Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			//V(m_D3DDev->SetLight(0, &light));
			//V(m_D3DDev->LightEnable(0, TRUE));

			if (m_iTextureIdx != 0)
			{
				V(pD3DDevice->SetTexture(0, m_TextureAry[m_iTextureIdx - 1]));
			}
			else
			{
				if (0)
				{
					D3DGAMMARAMP gdata;

					pD3DDevice->GetGammaRamp(false, &gdata);
					unsigned short rp;
					for (rp = 0; rp < 256; rp++)
					{
						gdata.red[rp] = 0; gdata.green[rp] = 0; gdata.blue[rp] = 0;
					};
					pD3DDevice->SetGammaRamp(false, D3DSGR_NO_CALIBRATION, &gdata);
				}

				V(pD3DDevice->StretchRect(pOutFrameSurface, NULL, m_renderTarget, NULL, D3DTEXF_NONE));
				V(pD3DDevice->SetTexture(0, m_EndTexture));
			}
			V(pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1));
			V(pD3DDevice->SetStreamSource(0, m_vertexBuffer, 0, sizeof(CAllocator::MAP2DVERTEX)));
			V(pD3DDevice->SetVertexShader(NULL));
			V(pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_i2DVerNum / 3));
			V(pD3DDevice->SetTexture(0, NULL));

			V(pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE));
			//V(m_D3DDev->LightEnable(0, FALSE));
			V(pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE));

		}
		else if (m_iSelObjIdx >= 0 && m_iSelObjIdx < m_BufTexture.size())
		{
			IDirect3DSurface9 *pVideoSurface = NULL;
			m_BufTexture[m_iSelObjIdx]->GetSurfaceLevel(0, &pVideoSurface);
			V(pD3DDevice->StretchRect(pVideoSurface, NULL, m_renderTarget, NULL, D3DTEXF_NONE));
			pVideoSurface->Release();
		}

		V(pD3DDevice->EndScene());
//		m_scene.DecreaseDrawSceneCount();
		V(pD3DDevice->SetTexture(0, NULL));

#ifdef _TRAIL_VERSION
		DrawWatermark();
#endif

		m_pDisplayManager->SetRenderStatus(RS_PRESENT);

#ifdef _ENABLE_D3D9EX

		if (m_bIsD3D9Ex)
		{
			hr = ((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(0, 0, 0, 0, 0);
		}

#else
		V(pD3DDevice->Present(NULL, NULL, NULL, NULL));
#endif
		V(pD3DDevice->SetTransform(D3DTS_VIEW, &matView));
		V(pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj));

	}

	m_pDisplayManager->SetRenderStatus(RS_NORMAL);
	m_pDisplayManager->UsingD3D(0);
//	LeaveCriticalSection(&m_csFunBsy);
	::SetEvent(m_hEffect2);

	LONGLONG lpRocessTime = GetTickCount() - m_lSyncTick;

#ifdef _ENABLE_D3D9EX
#else
	DrawSelArea();
#endif
	/*
	if (m_ptAreaSel.bottom > 0)
	{

		//		CClientDC cdc(m_pParentWnd);
		//		CBrush brhRed(RGB(255, 0, 0));
		m_pCDC->FrameRect(m_ptAreaSel, &m_brhRed);
	}
	*/

	if (pOutFrameSurface)
		pOutFrameSurface->Release();

#ifdef _ENABLE_DISPLAY_MANAGER
	float fInterval = 1000.0 / fFPS;

	if (m_pDisplayManager->IsRecordAVI())
	{
		fInterval *= 4;
		Sleep(DWORD(max(0, fInterval - lpRocessTime)));
	}
	else
#endif
		if (!(g_PannelSetting.iModelAction & (MODEL_MODIFY | MODEL_SHOW_MODEL_LINE | MODEL_RUN_MODEL_LINE))
			&& g_PannelSetting.iModelAction != MODEL_MAP)
		{
//			Sleep(DWORD(max(0, fInterval - lpRocessTime)));
			Sleep(1);
		}
	
	CalPresentFPS();

	m_dwLastTime = dwCurTime;

#ifdef _ENABLE_SYNC_PB_CTRL
	if (m_pSyncPBCtrl)
	{
		m_pSyncPBCtrl->SetPanelStatus(m_iIndex, PSS_PRESENTED);
	}
#endif  //_ENABLE_SYNC_PB_CTRL

	return hr;
}

HRESULT CAllocator::ShowVideo_VW(bool bDoPresent)
{
	HRESULT hr = S_OK;

	INT32 iEffectStatus = 0;

	INT32 iMediaIdx = -1;
	STREM_MAPPING smTemp = theApp.GetStreamMapByIdx(m_iIndex, m_iCurThdIdx);
	float fFPS = g_MediaStreamAry.at(smTemp.iMediaIdx)->GetFPS();

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();

	IDirect3DSurface9 *pOutFrameSurface = NULL;
	BOOL bDoEffectAgain = FALSE;
	IDirect3DSurface9 *pOldZBuffer = NULL;
	BOOL bHaveEndSurface;
	BOOL bShowModel;
	BOOL b2DMapStatus;

	DWORD dwCurTime = timeGetTime();
	bHaveEndSurface = g_PannelSetting.bHaveEndSurface;
	bShowModel = !g_PannelSetting.bNotShowModal;

	if (bHaveEndSurface && bShowModel)
	{
		if (m_i2DVerNum != 0)
			b2DMapStatus = 2;
		else
			b2DMapStatus = 1;
	}
	else
		b2DMapStatus = 0;

	if (((g_PannelSetting.iModelAction & 0xf) == MODEL_MAP || m_ptAry.GetCount() > 0) && m_ptSel.x != -1)
	{
		DrawAimingPoint();
		return hr;// continue;
	}
	else if (m_i2DMapAction & MAP2D_NEED_UPDATE_BK)
	{
		m_BKDC->BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(), m_pCDC, 0, 0, SRCCOPY);
		m_i2DMapAction = m_i2DMapAction &(~MAP2D_NEED_UPDATE_BK);
	}
	else if (m_iBufferAction == 2)
	{
		m_iCurStatus = STATUS_LOADING;
		EnterCriticalSection(&m_csFunBsy);

		INT32 iBaseH = m_rectClient.Height() / 2;
		for (int x = 0; x < m_i2DVerNum; x++)
		{
			if ((m_bSelAry[x] & 0x80) > 0)
			{
				m_vertices[x].position.x += float(m_ptOffset.x) / iBaseH;
				m_vertices[x].position.y -= float(m_ptOffset.y) / iBaseH;
			}
		}
		m_iBufferAction = 0;
		m_ptOffset = CPoint(0, 0);

		void* pData;
		V(m_vertexBuffer->Lock(0, 0, &pData, 0));
		memcpy(pData, m_vertices, sizeof(CAllocator::MAP2DVERTEX) * m_i2DVerNum);
		V(m_vertexBuffer->Unlock());

		m_iCurStatus = STATUS_PREVIEWING;
		LeaveCriticalSection(&m_csFunBsy);
	}
	else if (m_iBufferAction == 3)
	{
		void* pData;
		V(m_vertexBuffer->Lock(0, 0, &pData, 0));
		memcpy(pData, m_vertices, sizeof(CAllocator::MAP2DVERTEX) * m_i2DVerNum);
		V(m_vertexBuffer->Unlock());
	}
	else if (m_iBufferAction == 1)
	{
		m_iBufferAction = 0;
		m_uCurPtArea = 0;

		for (int x = 0; x < m_i2DVerNum; x++)
		{
			m_bSelAry[x] &= 0x7f;
		}
	}
	else if (m_i2DMapAction & MAP2D_SHOW_BK_PT)
	{
		CPen *pedNew = NULL;
		HGDIOBJ pOldPen;
		INT32 iBaseH = m_rectClient.Height() / 2;
		INT32 iBaseW = m_rectClient.Width() / 2;
		D3DCOLOR    color;
		//		CPen penWhite(PS_SOLID, 1, RGB(255, 255, 255));
		BOOL bSelFlag1 = FALSE;
		BOOL bSelFlag2 = FALSE;
		CPoint ptTemp1, ptTemp2;

		m_MemoryDC->BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(), m_BKDC, 0, 0, SRCCOPY);
		pOldPen = m_MemoryDC->SelectObject(m_penDiff);

		for (int x = 0; x < m_i2DVerNum; x += 6)
		{
			/*color = m_vertices[x].color;
			pedNew = new CPen(PS_SOLID, 1, RGB(0xff &(color >>16), 0xff & (color >> 8), 0xff & color));

			if (x == 0)
			pOldPen = m_MemoryDC->SelectObject(pedNew);
			else
			m_MemoryDC->SelectObject(pedNew);*/
			bSelFlag1 = bSelFlag2 = FALSE;

			if ((m_bSelAry[x] & 0x80) > 0)
			{
				bSelFlag1 = TRUE;
			}

			if ((m_bSelAry[x + 2] & 0x80) > 0)
			{
				bSelFlag2 = TRUE;
			}

			if (bSelFlag1 && bSelFlag2)
				m_MemoryDC->SelectObject(m_penWhite);
			else
				m_MemoryDC->SelectObject(m_penDiff);

			ptTemp1 = CPoint(m_vertices[x].position.x * iBaseH + iBaseW, -m_vertices[x].position.y * iBaseH + iBaseH);
			ptTemp2 = CPoint(m_vertices[x + 2].position.x * iBaseH + iBaseW, -m_vertices[x + 2].position.y * iBaseH + iBaseH);

			if (bSelFlag1)
				ptTemp1 += m_ptOffset;

			if (bSelFlag2)
				ptTemp2 += m_ptOffset;

			m_MemoryDC->MoveTo(ptTemp1);
			m_MemoryDC->LineTo(ptTemp2);

			//delete pedNew;
		}

		for (int x = 0; x < m_i2DVerNum; x += 6)
		{
			/*color = m_vertices[x].color;
			pedNew = new CPen(PS_SOLID, 1, RGB(0xff & (color >> 16), 0xff & (color >> 8), 0xff & color));
			m_MemoryDC->SelectObject(pedNew);*/

			bSelFlag1 = bSelFlag2 = FALSE;

			if ((m_bSelAry[x] & 0x80) > 0)
			{
				bSelFlag1 = TRUE;
			}

			if ((m_bSelAry[x + 1] & 0x80) > 0)
			{
				bSelFlag2 = TRUE;
			}

			if (bSelFlag1 && bSelFlag2)
				m_MemoryDC->SelectObject(m_penWhite);
			else
				m_MemoryDC->SelectObject(m_penDiff);

			ptTemp1 = CPoint(m_vertices[x].position.x * iBaseH + iBaseW, -m_vertices[x].position.y * iBaseH + iBaseH);
			ptTemp2 = CPoint(m_vertices[x + 1].position.x * iBaseH + iBaseW, -m_vertices[x + 1].position.y * iBaseH + iBaseH);

			if (bSelFlag1)
				ptTemp1 += m_ptOffset;

			if (bSelFlag2)
				ptTemp2 += m_ptOffset;

			m_MemoryDC->MoveTo(ptTemp1);
			m_MemoryDC->LineTo(ptTemp2);

			//delete pedNew;
		}

		m_MemoryDC->SelectObject(pOldPen);
		if (m_ptAreaSel.bottom > 0)
		{
			CClientDC cdc(m_pParentWnd);
			CBrush brhWhite(RGB(255, 255, 255));
			m_MemoryDC->FrameRect(m_ptAreaSel, &brhWhite);
		}

		m_pCDC->BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(), m_MemoryDC, 0, 0, SRCCOPY);
		return hr;// continue;
	}

//	if (m_pDisplayManager->IsSuspend())
//		return -1;

	if (bHaveEndSurface && m_surfaces.size() > 0 && b2DMapStatus != 1)
	{
		V(m_EndTexture->GetSurfaceLevel(0, &pOutFrameSurface));
		V(pD3DDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0L));
		V(pD3DDevice->SetRenderTarget(0, pOutFrameSurface));
		pD3DDevice->GetDepthStencilSurface(&pOldZBuffer);
		V(pD3DDevice->SetDepthStencilSurface(m_EndSurface));
		V(pD3DDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0L));

		D3DSURFACE_DESC rc;
		pOutFrameSurface->GetDesc(&rc);
	}
	else
		pD3DDevice->SetRenderTarget(0, m_renderTarget);

//	m_iDeviceStatus = m_D3DDev->ValidateDevice(&m_iNumPasses);

//	if (m_iIndex == 0)
	//	g_lSyncTick = GetTickCount();

	for (UINT y = 0; y < m_surfaces.size(); y++)
	{
		m_iEffectIdx[y] = g_PannelSetting.iImageEffect[y];
	}

#if 1

	/*
	//while (((m_pDisplayManager && m_pDisplayManager->IsD3DUsing() == 2) || !IsWindowVisible(m_pParentWnd->m_hWnd)))
	while (m_pDisplayManager && m_pDisplayManager->IsD3DUsing() == 2 || m_pDisplayManager->GetRenderStatus() == RS_WAIT)
	{
		if (!m_pDisplayManager)
			break;

		Sleep(1);
	}
	*/

	if (m_pDisplayManager->GetRenderStatus() == RS_WAIT)
		return hr;

	//	if (!m_pPreviewThread)
	//		return hr;// break;

	if (m_pDisplayManager)
		m_pDisplayManager->UsingD3D(1);


	//while (m_iCurStatus == STATUS_LOADING)
	//	Sleep(1);

//	EnterCriticalSection(&m_csFunBsy);

	//#ifndef MULTI_STREAM
	for (UINT y = 0; y < m_surfaces.size(); y++)
	{
		/*#ifdef NOT_PROC_EFFECT
		UpdateSurfaceBySample(m_BufTexture[y], y);
		#else*/
		m_iEffectStatus[y] = 1;
		iEffectStatus = 1;
		//#endif
	}

	//while (m_pPreviewThread && iEffectStatus > 0)
	while (iEffectStatus > 0)
	{
		iEffectStatus = 0;

		for (UINT y = 0; y < m_surfaces.size(); y++)
		{
			iEffectStatus += m_iEffectStatus[y];
		}
	}
	/*
	if (!m_pPreviewThread)
	{
	m_pDisplayManager->UsingD3D(0);
	LeaveCriticalSection(&m_csFunBsy);
	return hr;// break;
	}
	*/
#endif

	if (bDoEffectAgain)
	{
		for (UINT y = 0; y < m_surfaces.size(); y++)
		{
			m_iEffectStatus[y] = 11;
		}

		//while (m_pPreviewThread && iEffectStatus > 0)
		while (iEffectStatus > 0)
		{
			iEffectStatus = 0;

			for (UINT y = 0; y < m_surfaces.size(); y++)
			{
				iEffectStatus += m_iEffectStatus[y];
			}
		}
		/*
		if (!m_pPreviewThread)
		{
		m_pDisplayManager->UsingD3D(0);
		LeaveCriticalSection(&m_csFunBsy);
		return hr;// break;
		}
		*/
	}
	/*
	if (!m_bIsAlive)
	{
		if (m_pDisplayManager)
			m_pDisplayManager->UsingD3D(0);
		LeaveCriticalSection(&m_csFunBsy);
		return hr;
	}
	*/
	m_pDisplayManager->SetRenderStatus(RS_DRAW);

	if (bShowModel || m_surfaces.size() == 0)
	{
#ifdef PLAY_3D_MODE
		//V(m_scene.DrawScene(m_D3DDev, NULL, g_PannelSetting.iSelIdx, g_PannelSetting.iModelAction, -100));
		hr = m_scene.DrawScene(pD3DDevice, NULL, g_PannelSetting.iSelIdx, g_PannelSetting.iModelAction, -100);
		if (FAILED(hr))
		{
			char szMsg[512];
			sprintf_s(szMsg, "#Allocator# ShowVideo_VW DrawScene failed!! \n");
			OutputDebugStringA(szMsg);


		}
#else
		V(m_scene.DrawScene(m_D3DDev, NULL));
#endif
	}

	if (m_surfaces.size() == 0)
	{
#ifdef PLAY_3D_MODE
		//V(m_scene.DrawScene(m_D3DDev, NULL, g_PannelSetting.iSelIdx, g_PannelSetting.iModelAction, 0));
		hr = m_scene.DrawScene(pD3DDevice, NULL, g_PannelSetting.iSelIdx, g_PannelSetting.iModelAction, 0);
		if (FAILED(hr))
		{
			char szMsg[512];
			sprintf_s(szMsg, "#Allocator# ShowVideo_VW DrawScene failed!! \n");
			OutputDebugStringA(szMsg);
		}
#else
		V(m_scene.DrawScene(m_D3DDev, NULL));
#endif
	}


	for (UINT x = 0; x < m_BufTexture.size() && bShowModel; x++)
	{
		//while (m_iCurStatus == STATUS_LOADING)
		//	Sleep(1);
#ifdef PLAY_3D_MODE
		if (m_bMediaStatusAry[x])
		{
			try
			{
				hr = m_scene.DrawScene(pD3DDevice, m_BufTexture[x], g_PannelSetting.iSelIdx,
					g_PannelSetting.iModelAction, x);
			}
			catch (...)
			{
				OutputDebugString("HEError - Catching an exception in ShowVideoLoop - m_scene.DrawScene !! \n");
				//				::MessageBoxA(NULL, "LoadObj() Failed!!", "", MB_OK);
			}

			if (FAILED(hr))
			{
				char szMsg[512];
				sprintf_s(szMsg, "#Allocator# ShowVideo_VW DrawScene failed!! [%d]\n", x);
				OutputDebugStringA(szMsg);
			}
			//V(m_scene.DrawScene(m_D3DDev, m_BufTexture[x], g_PannelSetting.iSelIdx,
			//	g_PannelSetting.iModelAction, x));
		}
#else
		V(m_scene.DrawScene(m_D3DDev, m_BufTexture[x]));
#endif
	}

	if (!bHaveEndSurface || m_surfaces.size() == 0 || b2DMapStatus == 1)
	{
		//while (m_iCurStatus == STATUS_LOADING)
		//	Sleep(1);

		//V(m_D3DDev->EndScene());//Sometimes will crashed at here
		hr = pD3DDevice->EndScene();//Sometimes will crashed at here
		if (FAILED(hr))
		{
			char szMsg[512];
			sprintf_s(szMsg, "#Allocator# ShowVideo_VW EndScene failed!! \n");
			OutputDebugStringA(szMsg);
		}
		/*
#ifdef _ENABLE_DISPLAY_MANAGER
		float fInterval = 1000.0 / fFPS;
		if (dwCurTime - m_dwLastTime >= fInterval * 2.0 && m_pDisplayManager->IsRecordAVI())
			m_pDisplayManager->CaptureRenderTarget(m_renderTarget);
#endif */
		m_pDisplayManager->SetRenderStatus(RS_PRESENT);

#ifdef _ENABLE_MASK
		if (m_bDoMask)
		{
			//IDirect3DSurface9 *pBackBuffer;
			//hr = m_D3DDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
			//hr = m_D3DDev->SetRenderTarget(0, pBackBuffer);

			//float fH = 1.0; //(float)1920 / (float)m_pOverlayObj[i].iWidth;
			//float fV = 1.0; //(float)1080 / (float)m_pOverlayObj[i].iHeight;

			D3DXVECTOR3 vPosition((float)m_iMaskX, (float)m_iMaskY, 0.0f);
			m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

			//D3DXMatrixScaling(&m_ScaleMatrix, m_fMaskScale, m_fMaskScale, 1.f);
			//m_pSprite->SetTransform(&m_ScaleMatrix);
			m_pSprite->Draw(m_MaskTexture, NULL, NULL, &vPosition, D3DCOLOR_ARGB(255, 255, 255, 255));
			m_pSprite->End();

			//pBackBuffer->Release();
		}
#endif

		if (IsWindowVisible(m_pParentWnd->m_hWnd))
		{
			if (0)//m_iIndex == 0 && g_DispArys.GetCount() != 1)
			{
				CRect rectDisp = g_DispArys.GetAt(m_iIndex);

				CRect rectDest;
				rectDest.left = 165 * rectDisp.Width() / 1920;
				rectDest.top = 160 * rectDisp.Height() / 1080;
				rectDest.right = 1720 * rectDisp.Width() / 1920;
				rectDest.bottom = 1034 * rectDisp.Height() / 1080;//867

				V(pD3DDevice->Present(NULL, &rectDest, NULL, NULL));
			}
			else
			{
#ifdef _ENABLE_D3D9EX
				DrawSelArea();

				if (m_bIsD3D9Ex)
				{
					hr = ((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(0, 0, 0, 0, 0);
				}

#else
				hr = pD3DDevice->Present(NULL, NULL, NULL, NULL);
#endif
			}

			if (hr == D3DERR_DEVICELOST)
			{
				if (pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
				{
					DeleteSurfaces();
					if (pD3DDevice)
						pD3DDevice->Release();

					if (pD3D9Ptr)
						pD3D9Ptr->Release();

#ifdef _ENABLE_D3D9EX
					if (m_bIsD3D9Ex)
						Direct3DCreate9Ex(D3D_SDK_VERSION, &m_pD3DEx);
					else
						m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
#else
					m_D3D = Direct3DCreate9(D3D_SDK_VERSION);
#endif
					V(CreateDevice());

					V(InitShareSurface());
				}
			}
		}

	}
	else
	{
		if (bShowModel)
			V(pD3DDevice->EndScene());

		D3DXMATRIX matView, matProj;
		V(pD3DDevice->GetTransform(D3DTS_VIEW, &matView));
		V(pD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj));
		V(pD3DDevice->SetTransform(D3DTS_VIEW, &m_matEndView));
		V(pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_matEndProj));

		pD3DDevice->SetRenderTarget(0, m_renderTarget);
		V(pD3DDevice->SetDepthStencilSurface(pOldZBuffer));
		V(pD3DDevice->BeginScene());

		if (bShowModel)
		{
			//V(m_D3DDev->SetRenderState(D3DRS_LIGHTING, TRUE));
			V(pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE));

			D3DLIGHT9 light;
			ZeroMemory(&light, sizeof(light));
			light.Type = D3DLIGHT_DIRECTIONAL;
			light.Ambient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
			light.Diffuse = D3DXCOLOR(0.03f, 0.03f, 0.03f, 1.0f);
			light.Specular = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
			light.Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			//V(m_D3DDev->SetLight(0, &light));
			//V(m_D3DDev->LightEnable(0, TRUE));

			if (m_iTextureIdx != 0)
			{
				V(pD3DDevice->SetTexture(0, m_TextureAry[m_iTextureIdx - 1]));
			}
			else
			{
				if (0)
				{
					D3DGAMMARAMP gdata;

					pD3DDevice->GetGammaRamp(false, &gdata);
					unsigned short rp;
					for (rp = 0; rp < 256; rp++)
					{
						gdata.red[rp] = 0; gdata.green[rp] = 0; gdata.blue[rp] = 0;
					};
					pD3DDevice->SetGammaRamp(false, D3DSGR_NO_CALIBRATION, &gdata);
				}

				V(pD3DDevice->StretchRect(pOutFrameSurface, NULL, m_renderTarget, NULL, D3DTEXF_NONE));
				V(pD3DDevice->SetTexture(0, m_EndTexture));
			}
			V(pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1));
			V(pD3DDevice->SetStreamSource(0, m_vertexBuffer, 0, sizeof(CAllocator::MAP2DVERTEX)));
			V(pD3DDevice->SetVertexShader(NULL));
			V(pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_i2DVerNum / 3));
			V(pD3DDevice->SetTexture(0, NULL));

			V(pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE));
			//V(m_D3DDev->LightEnable(0, FALSE));
			V(pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE));

		}
		else if (m_iSelObjIdx >= 0 && m_iSelObjIdx < m_BufTexture.size())
		{
			IDirect3DSurface9 *pVideoSurface = NULL;
			m_BufTexture[m_iSelObjIdx]->GetSurfaceLevel(0, &pVideoSurface);
			V(pD3DDevice->StretchRect(pVideoSurface, NULL, m_renderTarget, NULL, D3DTEXF_NONE));
			pVideoSurface->Release();
		}

		V(pD3DDevice->EndScene());
		V(pD3DDevice->SetTexture(0, NULL));

		m_pDisplayManager->SetRenderStatus(RS_PRESENT);

#ifdef _ENABLE_MASK
		if (m_bDoMask)
		{
			//IDirect3DSurface9 *pBackBuffer;
			//hr = m_D3DDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
			//hr = m_D3DDev->SetRenderTarget(0, pBackBuffer);

			//float fH = 1.0; //(float)1920 / (float)m_pOverlayObj[i].iWidth;
			//float fV = 1.0; //(float)1080 / (float)m_pOverlayObj[i].iHeight;

			D3DXVECTOR3 vPosition((float)m_iMaskX, (float)m_iMaskY, 0.0f);
			m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

			//D3DXMatrixScaling(&m_ScaleMatrix, m_fMaskScale, m_fMaskScale, 1.f);
			//m_pSprite->SetTransform(&m_ScaleMatrix);
			m_pSprite->Draw(m_MaskTexture, NULL, NULL, &vPosition, D3DCOLOR_ARGB(255, 255, 255, 255));
			m_pSprite->End();

			//pBackBuffer->Release();
		}
#endif

#ifdef _ENABLE_D3D9EX
		if (m_bIsD3D9Ex)
		{
			hr = ((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(0, 0, 0, 0, 0);
		}

#else
		V(pD3DDevice->Present(NULL, NULL, NULL, NULL));
#endif
		V(pD3DDevice->SetTransform(D3DTS_VIEW, &matView));
		V(pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj));

	}

	m_pDisplayManager->SetRenderStatus(RS_NORMAL);
	m_pDisplayManager->UsingD3D(0);
//	LeaveCriticalSection(&m_csFunBsy);

//	LONGLONG lpRocessTime = GetTickCount() - g_lSyncTick;

#ifdef _ENABLE_D3D9EX
	//WaitForVBlank
	//GetPresentStatistics
	//IDirect3DSwapChain9Ex* pD3DSwapChain;
	//UINT iLastPresentCount;
	//hr = ((IDirect3DDevice9Ex*)pD3DDevice)->GetSwapChain(0, &pD3DSwapChain);
	//hr = pD3DSwapChain->GetLastPresentCount(&iLastPresentCount);
#else
	DrawSelArea();
#endif
	/*
	if (m_ptAreaSel.bottom > 0)
	{
		//		CClientDC cdc(m_pParentWnd);
		//		CBrush brhRed(RGB(255, 0, 0));
		m_pCDC->FrameRect(m_ptAreaSel, &m_brhRed);
	}
	*/

	/*
#ifdef _ENABLE_DISPLAY_MANAGER
	float fInterval = 1000.0 / fFPS;

	if (m_pDisplayManager->IsRecordAVI())
	{
		fInterval *= 4;
		Sleep(DWORD(max(0, fInterval - lpRocessTime)));
	}
	else
#endif
		if (!(g_PannelSetting.iModelAction & (MODEL_MODIFY | MODEL_SHOW_MODEL_LINE | MODEL_RUN_MODEL_LINE))
			&& g_PannelSetting.iModelAction != MODEL_MAP)
		{
			Sleep(DWORD(max(0, fInterval - lpRocessTime)));
		}
		*/

//	CalPresentFPS();

	m_dwLastTime = dwCurTime;

	if (pOutFrameSurface)
		pOutFrameSurface->Release();

	return hr;
}

bool CAllocator::IsAlive()
{
	return m_bIsAlive;
}
#endif

#ifdef _VF_SINGLE_THREAD
bool CAllocator::IsAlive2()
{
	return m_bIsAlive2;
}
#endif   //_VF_SINGLE_THREAD

HRESULT CAllocator::PresentByID(DWORD_PTR dwUserID, VMR9PresentationInfo *lpPresInfo)
{
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	if (m_pPreviewThread)
	{
		return S_OK;
	}

	if( lpPresInfo == NULL )
    {
        return E_POINTER;
    }
    else if( lpPresInfo->lpSurf == NULL )
    {
        return E_POINTER;
    }

    //CAutoLock Lock(&m_ObjectLock);
    HRESULT hr;

	//V( m_D3DDev->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L ));

    IDirect3DTexture9* texture;
    V( lpPresInfo->lpSurf->GetContainer(__uuidof(IDirect3DTexture9), (LPVOID*) & texture ) );
	//V( m_surfaces.at(dwUserID - 0xff00)->GetContainer( __uuidof(IDirect3DTexture9), (LPVOID*) & texture.p ) );

	pD3DDevice->SetRenderTarget( 0, m_renderTarget );
	V(pD3DDevice->BeginScene() );
	
#ifdef PLAY_3D_MODE
	V( m_scene.DrawScene(pD3DDevice, texture, g_PannelSetting.iSelIdx,
		                 g_PannelSetting.iModelAction, dwUserID - 0xff00));
#else
	V( m_scene.DrawScene(m_D3DDev, texture) );
#endif

	V(pD3DDevice->EndScene());

#ifdef _ENABLE_D3D9EX
	if (m_bIsD3D9Ex)
	{
		hr = ((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(0, 0, 0, 0, 0);
	}

#else
	V(pD3DDevice->Present( NULL, NULL, NULL, NULL ) );
#endif
	texture->Release();

    return hr;
}

BOOL SaveToBmp24File(IN const LPVOID pImageBuf, IN const int nPixelX, IN const int nPixelY, IN LPCTSTR lpszFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BITMAPINFOHEADER bmih;
	ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biBitCount = 24;
	bmih.biPlanes = 1;
	bmih.biCompression = BI_RGB;
	bmih.biWidth = nPixelX;
	bmih.biHeight = nPixelY;
	bmih.biSizeImage = WIDTHBYTES(nPixelX * bmih.biBitCount) * nPixelY;

	INT nBitsOffset = sizeof(BITMAPFILEHEADER) + bmih.biSize;
	LONG lImageSize = bmih.biSizeImage;
	LONG lFileSize = nBitsOffset + lImageSize;

	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 'MB';	//'B' + ('M' << 8);
	bmfh.bfOffBits = nBitsOffset;
	bmfh.bfSize = lFileSize;
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;

	HANDLE hFile;
	BOOL bRet = FALSE;
	do
	{
		hFile = CreateFile(lpszFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile == INVALID_HANDLE_VALUE)
			break;

		DWORD dwNumberOfBytesWritten;

		if (!WriteFile(hFile,			// handle to file to write to
			&bmfh,						// pointer to data to write to file
			sizeof(BITMAPFILEHEADER),	// number of bytes to write
			&dwNumberOfBytesWritten,	// pointer to number of bytes written
			NULL))
			break;

		if (!WriteFile(hFile,			// handle to file to write to
			&bmih,						// pointer to data to write to file
			sizeof(BITMAPINFOHEADER),	// number of bytes to write
			&dwNumberOfBytesWritten,	// pointer to number of bytes written
			NULL))
			break;

		if (!WriteFile(hFile,			// handle to file to write to
			pImageBuf,					// pointer to data to write to file
			lImageSize,					// number of bytes to write
			&dwNumberOfBytesWritten,	// pointer to number of bytes written
			NULL))
			break;

		bRet = TRUE;

	} while (false);

	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	return bRet;
}

HRESULT CAllocator::PresentHelper(VMR9PresentationInfo *lpPresInfo)
{	
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
    // parameter validation
    if( lpPresInfo == NULL )
    {
        return E_POINTER;
    }
    else if( lpPresInfo->lpSurf == NULL )
    {
        return E_POINTER;
    }

    //CAutoLock Lock(&m_ObjectLock);
    HRESULT hr;

	/*LONGLONG lCurPos, lLength;
	V(g_MediaSeekingAry.at(0)->GetCurrentPosition(&lCurPos));
	V(g_MediaSeekingAry.at(0)->GetStopPosition(&lLength));

	if (abs(lLength - lCurPos) < 10000)
	{
		lCurPos = 0;
		V(g_MediaSeekingAry.at(0)->SetPositions(&lCurPos, AM_SEEKING_AbsolutePositioning, NULL, 0));
	} */

	pD3DDevice->SetRenderTarget( 0, m_renderTarget );
	
	// this is the case where we have got the textures allocated by VMR
	// all we need to do is to get them from the surface
    {
		/*RECT rectSrc;
		RECT rectDest;

		rectSrc.top = 0;
		rectSrc.left = 0;
		rectSrc.right = 100;
		rectSrc.bottom = 100;

		rectDest.top = 320;
		rectDest.left = 240;
		rectDest.right = 640;
		rectDest.bottom = 480;


        // copy the full surface onto the texture's surface
        V( m_D3DDev->StretchRect( lpPresInfo->lpSurf, &rectSrc,
                             m_renderTarget, &rectDest,
                             D3DTEXF_NONE ) );*/

        LONGLONG lpRocessTime = 0;

		m_iEffectIdx[m_iIndex] = g_PannelSetting.iImageEffect[m_iIndex];
		V(VidoeEffect());		
#ifdef PLAY_3D_MODE
		V( m_scene.DrawScene(pD3DDevice, m_BufTexture[0], g_PannelSetting.iSelIdx, g_PannelSetting.iModelAction));
#else
		V(m_scene.DrawScene(m_D3DDev, m_BufTexture[0]));
#endif

		CString strTemp;
		strTemp.Format("%i, %i\n", lpPresInfo->szAspectRatio.cx, lpPresInfo->szAspectRatio.cy);
		//sfTemp.WriteString(strTemp);
    }

#ifdef _ENABLE_D3D9EX
	if (m_bIsD3D9Ex)
	{
		hr = ((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(0, 0, 0, 0, 0);
	}

#else
    V(pD3DDevice->Present( NULL, NULL, NULL, NULL ) );
#endif
    return hr;
}

bool CAllocator::NeedToHandleDisplayChange()
{
	IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	if( ! m_lpIVMRSurfAllocNotify[0]  && g_BaseFilterAry.size() == 0)
    {
        return false;
    }

    D3DDEVICE_CREATION_PARAMETERS Parameters;
    if( FAILED(pD3DDevice->GetCreationParameters(&Parameters) ) )
    {
        ASSERT( false );
        return false;
    }

    HMONITOR currentMonitor = pD3D9Ptr->GetAdapterMonitor( Parameters.AdapterOrdinal );

    HMONITOR hMonitor = pD3D9Ptr->GetAdapterMonitor( D3DADAPTER_DEFAULT );

    return hMonitor != currentMonitor;
}


// IUnknown
HRESULT CAllocator::QueryInterface( 
        REFIID riid,
        void** ppvObject)
{
    HRESULT hr = E_NOINTERFACE;

    if( ppvObject == NULL ) {
        hr = E_POINTER;
    } 
    else if( riid == IID_IVMRSurfaceAllocator9 ) {
        *ppvObject = static_cast<IVMRSurfaceAllocator9*>( this );
        AddRef();
        hr = S_OK;
    } 
    else if( riid == IID_IVMRImagePresenter9 ) {
        *ppvObject = static_cast<IVMRImagePresenter9*>( this );
        AddRef();
        hr = S_OK;
    } 
    else if( riid == IID_IUnknown ) {
        *ppvObject = 
            static_cast<IUnknown*>( 
            static_cast<IVMRSurfaceAllocator9*>( this ) );
        AddRef();
        hr = S_OK;    
    }

    return hr;
}

ULONG CAllocator::AddRef()
{
    return InterlockedIncrement(& m_refCount);
}

ULONG CAllocator::Release()
{
    ULONG ret = InterlockedDecrement(& m_refCount);
    if( ret == 0 )
    {
        delete this;
    }

    return ret;
}

HRESULT CAllocator::AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister)
{
	IMoniker * pMoniker = NULL;
	IRunningObjectTable *pROT = NULL;

	if (FAILED(GetRunningObjectTable(0, &pROT)))
	{
		return E_FAIL;
	}

	const size_t STRING_LENGTH = 256;

	WCHAR wsz[STRING_LENGTH];

	StringCchPrintfW(
		wsz, STRING_LENGTH,
		L"FilterGraph %08x pid %08x",
		(DWORD_PTR)pUnkGraph,
		GetCurrentProcessId()
	);

	HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if (SUCCEEDED(hr))
	{
		hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,
			pMoniker, pdwRegister);
		pMoniker->Release();
	}
	pROT->Release();

	return hr;
}

void CAllocator::RemoveFromRot(DWORD pdwRegister)
{
	IRunningObjectTable *pROT;
	if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
		pROT->Revoke(pdwRegister);
		pROT->Release();
	}
}

void CAllocator::SetObjIdx(INT32 iSelObjIdx)
{
#ifdef PLAY_3D_MODE
	if (m_scene.GetCurStatus() != STATUS_NO_INITIAL)
	{
		if (m_iIndex == 0)
		{
			INT32 iIndex = 0;
			for (int x = 0; x < g_StreamMappingAry.GetCount(); x++)
			{
				if (g_StreamMappingAry.GetAt(x).iPanelIdx == g_PannelSetting.iSelIdx)
				{
					if (iSelObjIdx == iIndex)
					{
						iSelObjIdx = x;
						break;
					}
					iIndex++;
				}
			}
		}

		m_iSelObjIdx = iSelObjIdx;
		return m_scene.SetObjIdx(iSelObjIdx);
	}
#endif
};

void CAllocator::DeleteObject(INT32 iDelObjIdx)
{
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

#ifdef PLAY_3D_MODE
	if (m_scene.GetCurStatus() != STATUS_NO_INITIAL)
	{
		if (iDelObjIdx == -1)
		{
			WaitForFinished();
			m_pDisplayManager->SetRenderStatus(RS_WAIT);

#ifdef _ENABLE_FLIP_THREAD
			m_bIsAlive = false;
			::WaitForSingleObject(m_hShowVideoThread, 3000);
			m_hShowVideoThread = 0;
#else
			m_pPreviewThread = NULL;

			while (m_bPreviewFlag)
				Sleep(1);
#endif

			EnterCriticalSection(&m_csFunBsy);
			m_iCurStatus = STATUS_LOADING;

			if (m_iIndex != 0 || g_PannelAry.GetSize() == 1)
			{
				for (int x = 0; x < min(m_surfaces.size(), MAX_FILM); x++)
				{
					m_pImgEffectThd[x] = NULL;

#ifdef _VF_SINGLE_THREAD
#else
					while (m_iEffectStatus[x] != -1)
						Sleep(1);
#endif
				}
			}

			ReleaseSurface(iDelObjIdx);
			m_scene.DeleteObject(iDelObjIdx);

			m_surfaces.resize(0);
//			m_surBuffaces.resize(0);
//			m_sur3DBuf.resize(0);
			m_BufTexture.resize(0);
//			m_Buf3DTexture.resize(0);
			m_bMediaStatusAry.RemoveAll();

			if (m_iIndex == 0)
				theApp.DeleteObject(-1, 0);

			m_pDisplayManager->SetRenderStatus(RS_NORMAL);
			LeaveCriticalSection(&m_csFunBsy);
		}
		else
		{
			WaitForFinished();

			m_pDisplayManager->SetRenderStatus(RS_WAIT);

			EnterCriticalSection(&m_csFunBsy);
			m_iCurStatus = STATUS_LOADING;

			if (m_iIndex == 0)
			{
				INT32 iIndex = 0;
				for (int x = 0; x < g_StreamMappingAry.GetCount(); x++)
				{
					if (g_StreamMappingAry.GetAt(x).iPanelIdx == g_PannelSetting.iSelIdx)
					{
						if (iDelObjIdx == iIndex)
						{
							theApp.DeleteObject(g_PannelSetting.iSelIdx, iDelObjIdx);
							iDelObjIdx = x;
							break;
						}
						iIndex++;
					}
				}
			}


			if (m_iIndex != 0 || g_PannelAry.GetSize() == 1)
			{
				INT32 iLastVideoIdx = min(m_surfaces.size(), MAX_FILM) - 1;

				m_pImgEffectThd[iLastVideoIdx] = NULL;

#ifdef _VF_SINGLE_THREAD
#else
				while (m_iEffectStatus[iLastVideoIdx] != -1)
					Sleep(1);
#endif

				ReleaseSurface(iDelObjIdx);

				m_surfaces.erase(m_surfaces.begin() + iDelObjIdx);
//				m_surBuffaces.erase(m_surBuffaces.begin() + iDelObjIdx);
//				m_sur3DBuf.erase(m_sur3DBuf.begin() + iDelObjIdx);
				m_BufTexture.erase(m_BufTexture.begin() + iDelObjIdx);
//				m_Buf3DTexture.erase(m_Buf3DTexture.begin() + iDelObjIdx);

				m_bMediaStatusAry.RemoveAt(0);
			}

			m_scene.DeleteObject(iDelObjIdx);
		}

		pD3DDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0L);
		pD3DDevice->BeginScene();
		pD3DDevice->EndScene();
		pD3DDevice->SetTexture(0, NULL);
#ifdef _ENABLE_D3D9EX
		if (m_bIsD3D9Ex)
		{
			((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(0, 0, 0, 0, 0);
		}

#else
		pD3DDevice->Present(NULL, NULL, NULL, NULL);
#endif

		if (iDelObjIdx == -1)
			m_iCurStatus = STATUS_NO_PREVIEW;
		else
			m_iCurStatus = STATUS_PREVIEWING;

		m_pDisplayManager->SetRenderStatus(RS_NORMAL);
		LeaveCriticalSection(&m_csFunBsy);
	}
#endif


#ifdef _ENABLE_SYNC_PB_CTRL
	if (m_pSyncPBCtrl)
		m_pSyncPBCtrl->SetSourceCount(m_iIndex, m_surfaces.size());
#endif //_ENABLE_SYNC_PB_CTRL
}

void CAllocator::DisplayVideoRawData(unsigned char* pBuffer, int w, int h)
{
	/*if (m_pD3DRender && m_pD3DRender->HadInited())
	{
		if (m_pD3DRender->GetWidth() != w || m_pD3DRender->GetHeight() != h)
		{
			int iPixelFormat = VPF_RGB32;
			int iRet = m_pD3DRender->RestSurfaceFormat(iPixelFormat, w, h);
		}
		m_pD3DRender->RenderFrame(pBuffer, w, h);
		m_pD3DRender->Flip();
	}*/
}

void CAllocator::DisplayVideoSurfaceData(void* pSurface, int w, int h)
{
	/*if (m_pD3DRender && m_pD3DRender->HadInited())
	{
		if (m_pD3DRender->GetWidth() != w || m_pD3DRender->GetHeight() != h)
		{
			int iPixelFormat = VPF_RGB32;
			int iRet = m_pD3DRender->RestSurfaceFormat(iPixelFormat, w, h);
		}

		m_pD3DRender->RenderFrame1((IDirect3DSurface9 *)pSurface, w, h);
		m_pD3DRender->Flip();
	}*/
}

void CAllocator::DisplayNV12SurfaceData(void* pSurface, int w, int h)
{
	HRESULT hr;
	IDirect3DSurface9 *pBackBuffer;
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	hr = pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

	//hr = m_D3DDev->SetRenderTarget(0, pBackBuffer);
	
	IDirect3DSurface9 *pVideoSurface = NULL;
	m_BufTexture[0]->GetSurfaceLevel(0, &pVideoSurface);

	hr = pD3DDevice->StretchRect((IDirect3DSurface9 *)pSurface, NULL, pVideoSurface, NULL, D3DTEXF_NONE);
	pVideoSurface->Release();
}

void CAllocator::DisplayVideo_VW(void* pSurface, int w, int h,bool bDoPresent)
{	
	HRESULT hr;
	char szMsg[512];
	//EnterCriticalSection(&m_csEffectInUse);
	IDirect3DSurface9 *pVideoSurface = NULL;
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

#if 0
	if (m_BufTexture.size() > 0 && m_BufTexture[0])
	{
		m_BufTexture[0]->GetSurfaceLevel(0, &pVideoSurface);

		hr = m_D3DDev->StretchRect((IDirect3DSurface9 *)pSurface, NULL, pVideoSurface, NULL, D3DTEXF_NONE);

		m_bMediaStatusAry[0] = TRUE;
		m_lFrameCount[0]++;
	}
	else
	{
		if(m_bMediaStatusAry.GetCount() > 0)
			m_bMediaStatusAry[0] = FALSE;
	}
#else

	DWORD dwTime1 = timeGetTime();

	for (UINT x = 0; x < m_BufTexture.size(); x++)
	{
		if (m_BufTexture[x])
		{
			m_BufTexture[x]->GetSurfaceLevel(0, &pVideoSurface);

			hr = pD3DDevice->StretchRect((IDirect3DSurface9 *)pSurface, NULL, pVideoSurface, NULL, D3DTEXF_NONE);

			m_bMediaStatusAry[x] = TRUE;
			m_lFrameCount[x]++;
			pVideoSurface->Release();
		}
		else
		{
			if (m_bMediaStatusAry.GetCount() > 0)
				m_bMediaStatusAry[x] = FALSE;
		}
	}

	DWORD dwTime2 = timeGetTime();
#endif
	//LeaveCriticalSection(&m_csFunBsy);
	ShowVideo_VW(bDoPresent);

	if (bDoPresent)
	{
		DWORD dwTime3 = timeGetTime();

		m_dwLastPresentTime = dwTime3;

		int iCurFrameCount = m_pDisplayManager->GetFrameCount();

		if (iCurFrameCount > m_iLastFrameCount + 1)
		{
			sprintf(szMsg, "#MS# Panel:%d lose frame [%d - %d] \n", m_iIndex, m_iLastFrameCount, iCurFrameCount);
			OutputDebugStringA(szMsg);
		}
	
		m_iLastFrameCount = iCurFrameCount;

		//m_iLastFrameCount = m_pDisplayManager->GetFrameCount();

//		sprintf(szMsg, "#MS# Panel:%d %d (%d)\n", m_iIndex, dwTime3, m_pDisplayManager->GetFrameCount());
//		OutputDebugStringA(szMsg);

		m_AvgRenderSpendTime.Calculate(dwTime3 - dwTime2);

		if (m_AvgRenderSpendTime.GetCount() % 30 == 29)
		{
			float fSpendTime = m_AvgRenderSpendTime.GetAverage();

			sprintf(szMsg, "#MS# Panel:%d %2.3f \n", m_iIndex, fSpendTime);
			OutputDebugStringA(szMsg);
		}
	}
}

void CAllocator::DisplayVideo_VW(int iIndex,void* pSurface, int w, int h)
{
	HRESULT hr;
	//EnterCriticalSection(&m_csEffectInUse);
	IDirect3DSurface9 *pVideoSurface = NULL;
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	if (m_BufTexture.size() > iIndex && m_BufTexture[iIndex])
	{
		m_BufTexture[iIndex]->GetSurfaceLevel(0, &pVideoSurface);

		hr = pD3DDevice->StretchRect((IDirect3DSurface9 *)pSurface, NULL, pVideoSurface, NULL, D3DTEXF_NONE);

		m_bMediaStatusAry[iIndex] = TRUE;
		m_lFrameCount[iIndex]++;
		pVideoSurface->Release();
	}
	else
	{
		if (m_bMediaStatusAry.GetCount() > iIndex)
			m_bMediaStatusAry[iIndex] = FALSE;
	}
	//LeaveCriticalSection(&m_csFunBsy);
	ShowVideo_VW();
}

void CAllocator::DisplayVideo_HDEffect(int iIndex, void* pSurface, int w, int h)
{
	HRESULT hr;
	//EnterCriticalSection(&m_csEffectInUse);
	IDirect3DSurface9 *pVideoSurface = NULL;
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	if (m_BufTexture.size() > iIndex && m_BufTexture[iIndex])
	{
		m_BufTexture[iIndex]->GetSurfaceLevel(0, &pVideoSurface);

		hr = pD3DDevice->StretchRect((IDirect3DSurface9 *)pSurface, NULL, pVideoSurface, NULL, D3DTEXF_NONE);

		m_bMediaStatusAry[iIndex] = TRUE;
		m_lFrameCount[iIndex]++;
		pVideoSurface->Release();
	}
	else
	{
		if (m_bMediaStatusAry.GetCount() > iIndex)
			m_bMediaStatusAry[iIndex] = FALSE;
	}
}

void CAllocator::Set2DFunction(INT32 iFunIdx, UINT uData)
{
	EnterCriticalSection(&m_csFunBsy);

	if (iFunIdx == 0) //單一顏色材質投影
	{
		m_iTextureIdx = uData;
	}
	else if (iFunIdx == 1) //開啟手動設定疊影範圍模式
	{
		m_uCurPtArea = 0;
		m_ptAry.RemoveAll();
		m_ptAry.Add({ CPoint(0, 0), m_uCurPtArea, FALSE });
	}
	else if (iFunIdx == 2) //設定營幕Gamma值
	{
		if (SetGamma(double(uData) / 100))
			OutputDebugStringA("Set Gamma OK!~");
		else
			OutputDebugStringA("Set Gamma FAIL!~");
	}
	else if (iFunIdx == 3 && m_i2DVerNum > 0) //調整投影機的Gain值
	{
		/*for (int x = 0; x < m_i2DVerNum; x++)
		{
			//if (m_bSelAry[x] == 0 || (m_bSelAry[x] & 0x40) > 0)
				m_vertices[x].spcolor = uData;
			//else
				//m_vertices[x].spcolor = 0;
		}*/
		DOUBLE dTempColor;
		UINT iCurveVal;

		m_uPanelGain = uData;

		for (int x = 0; x < m_i2DVerNum; x++)
		{
			m_vertices[x].spcolor = m_uPanelGain;
			if (m_bSelAry[x] > 0 && (m_bSelAry[x] & 0x40) == 0)
			{
				dTempColor = 1.0f;

				for (int y = 0; y < MAX_PTAREA; y++)
				{
					dTempColor *= m_fVectColorAry[x][y];
				}
				
				for (int y = 0; y < 4; y++)
				{
					iCurveVal = ((m_uPanelGain >> (8 * y)) & 0xff) * dTempColor;
					iCurveVal = iCurveVal << (8 * y);
					m_vertices[x].spcolor |= iCurveVal;
				}
			}
		}

		m_iBufferAction = 2;
	}

	LeaveCriticalSection(&m_csFunBsy);
}

void CAllocator::Set2DColor(DOUBLE *pColAry, UINT iAreaIdx, CPoint ptDark)
{
	BOOL bNeedLock = TRUE;

	if (m_i2DVerNum == 0)
		return;

	if (iAreaIdx >= 100)
	{
		iAreaIdx -= 100;
		bNeedLock = FALSE;
	}

	if (bNeedLock)
		EnterCriticalSection(&m_csFunBsy);

	if (pColAry) // 調整所選區塊每點的亮度值
	{
		CPoint ptVector = CPoint(50, 50) - ptDark;
		DOUBLE dCurDist;
		m_uCurPtArea = iAreaIdx;
		float iMaxX = -1000000.f, iMinX = 1000000.f;
		for (int x = 0; x < m_i2DVerNum; x++)
		{
			if (m_bSelAry[x] & (1<< m_uCurPtArea))
			{
				dCurDist = m_vertices[x].position.x * ptVector.x / 44  - 
					       m_vertices[x].position.y * ptVector.y / 44;

				if (dCurDist > iMaxX)
					iMaxX = dCurDist;

				if (dCurDist < iMinX)
					iMinX = dCurDist;

				/*if (m_vertices[x].position.x > iMaxX)
					iMaxX = m_vertices[x].position.x;

				if (m_vertices[x].position.x < iMinX)
					iMinX = m_vertices[x].position.x;*/
			}
		}

		for (int x = 0; x < m_i2DVerNum; x++)
		{
			if (m_bSelAry[x] & (1 << m_uCurPtArea))
			{
				dCurDist = m_vertices[x].position.x * ptVector.x / 44-
						   m_vertices[x].position.y * ptVector.y / 44;

				m_fVectColorAry[x][m_uCurPtArea] = pColAry[INT32(DOUBLE(100 - 1) *(dCurDist - iMinX) / (iMaxX - iMinX))];
				//m_fVectColorAry[x][m_uCurPtArea] = pColAry[INT32(DOUBLE(100 - 1) *(m_vertices[x].position.x - iMinX) / (iMaxX - iMinX))];
				//m_vertices[x].color = pColAry[INT32(DOUBLE(100 - 1) *(m_vertices[x].position.x - iMinX) / (iMaxX - iMinX))];
				//m_vertices[x].spcolor = 0;
			}
		}

		DOUBLE dTempColor;
		UINT iCurveVal;
		for (int x = 0; x < m_i2DVerNum; x++)
		{
			m_vertices[x].spcolor = m_uPanelGain;
			if (m_bSelAry[x] > 0 && (m_bSelAry[x] & 0x40) == 0)
			{
				dTempColor = 1.0f;

				for (int y = 0; y < MAX_PTAREA; y++)
				{
					dTempColor *= m_fVectColorAry[x][y];
				}

				iCurveVal = 255 * dTempColor;
				m_vertices[x].color = (iCurveVal << 24) | (iCurveVal << 16) | (iCurveVal << 8) | iCurveVal;

				for (int y = 0; y < 4; y++)
				{
					iCurveVal = ((m_uPanelGain >> (8 * y)) & 0xff) * dTempColor;
					iCurveVal = iCurveVal << (8 * y);
					m_vertices[x].spcolor |= iCurveVal;
				}
			}
		}

		if (bNeedLock)
			m_iBufferAction = 2;		
	}

	if (bNeedLock)
		LeaveCriticalSection(&m_csFunBsy);
}


void CAllocator::Set2DMapAction(INT32 i2DMapAction, UINT32 iVale)
{	
		m_i2DMapAction = i2DMapAction;
}

void CAllocator::SetBlendSetting(BLEND_SETTING bsBlendSetting)
{
	m_BlendSetting = bsBlendSetting;
}

BOOL CAllocator::SetGamma(DOUBLE dGammaVal)
{
	BOOL bResult = FALSE;
	HDC hGammaDC = GetDC(m_window);
	DOUBLE dCurVal;

	//Load the display device context of the entire screen if hDC is NULL.
	if (hGammaDC == NULL)
		hGammaDC = GetDC(NULL);

	if (hGammaDC != NULL)
	{
		//Generate the 256-colors array for the specified wBrightness value.
		WORD GammaArray[3][256];

		for (int iIndex = 0; iIndex < 256; iIndex++)
		{
			dCurVal = 256.f*(256.f * pow(float(iIndex + 0.5) / 256, dGammaVal) - 0.5);

			if (dCurVal > 65535)
				dCurVal = 65535;

			GammaArray[0][iIndex] =
			GammaArray[1][iIndex] =
			GammaArray[2][iIndex] = (WORD)dCurVal;

		}

		//Set the GammaArray values into the display device context.
		bResult = SetDeviceGammaRamp(hGammaDC, GammaArray);
	}

	ReleaseDC(NULL, hGammaDC);

	return bResult;
}

BOOL CAllocator::SetBrightness(WORD wBrightness)
{
	BOOL bResult = FALSE;

	HDC hGammaDC = GetDC(m_window);

	//EnumDisplayMonitors(hGammaDC,);


	//Load the display device context of the entire screen if hDC is NULL.
	if (hGammaDC == NULL)
		hGammaDC = GetDC(NULL);

	if (hGammaDC != NULL)
	{
		//Generate the 256-colors array for the specified wBrightness value.
		WORD GammaArray[3][256];

		for (int iIndex = 0; iIndex < 256; iIndex++)
		{
			int iArrayValue = iIndex * (wBrightness + 128);

			if (iArrayValue > 65535)
				iArrayValue = 65535;

			GammaArray[0][iIndex] =
				GammaArray[1][iIndex] =
				GammaArray[2][iIndex] = (WORD)iArrayValue;

		}

		//Set the GammaArray values into the display device context.
		bResult = SetDeviceGammaRamp(hGammaDC, GammaArray);
	}

	ReleaseDC(NULL, hGammaDC);

	return bResult;
}

int CAllocator::CheckDeviceState()
{
	IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	EnterCriticalSection(&m_CriticalSection);
	if (pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		INT32 iWidth = 0;
		INT32 iHeight = 0;
		int x;
		m_bIsDeivceLost = true;

#ifdef _ENABLE_FLIP_THREAD
		m_bIsAlive = false;
		::WaitForSingleObject(m_hShowVideoThread, 3000);
		m_hShowVideoThread = 0;
#else
		m_pPreviewThread = NULL;

		while (m_bPreviewFlag)
			Sleep(1);
#endif

		//	DeleteSurfaces();


		for (x = 0; x < m_BufTexture.size(); x++)
		{
			ReleaseSurface(x);
		}


		if (pD3DDevice)
			pD3DDevice->Release();

		if (pD3D9Ptr)
			pD3D9Ptr->Release();

#ifdef _ENABLE_D3D9EX
		if (m_bIsD3D9Ex)
			Direct3DCreate9Ex(D3D_SDK_VERSION, &m_pD3DEx);
		else
			m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
#else
		m_D3D = Direct3DCreate9(D3D_SDK_VERSION);
#endif

#ifdef PLAY_3D_MODE
//		m_scene.ResetD3DDevice(m_D3DDev);
#endif
//		CreateDevice2();
		//		m_iCurThdIdx;


		STREM_MAPPING smTemp = theApp.GetStreamMapByIdx(m_iIndex, 0);
		iWidth = smTemp.iWidth;
		iHeight = smTemp.iHeight;

		for (x = 0; x < m_BufTexture.size(); x++)
		{
			CreateBufferTexture(iWidth, iHeight, x);
			CreateTVWallTexture(iWidth, iHeight, x);
		}

		m_pDisplayManager->ResetD3DDevice(pD3DDevice);


		//		g_PannelAry.GetAt(0)->LoadPorejct2();


		/*
		if (m_D3D)
		m_D3D->Release();

		m_D3D = Direct3DCreate9(D3D_SDK_VERSION);
		V(CreateDevice());

		V(InitShareSurface());
		*/
		LeaveCriticalSection(&m_CriticalSection);
		return 1;
	}
	LeaveCriticalSection(&m_CriticalSection);
	return -1;
}

void CAllocator::Flip()
{
	HRESULT hr;
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
#if 0
	long lOffset = 0;
	DWORD dwCurTime = timeGetTime();

	if (m_dwLastDisplayTime == 0)
		m_dwLastDisplayTime = dwCurTime;

	//if (m_bIsDeivceLost)
	//lOffset = 200;

	if (dwCurTime >= m_dwLastDisplayTime + 40 + lOffset)//40,33
	{
		m_dwLastDisplayTime = dwCurTime;

		if (IsWindowVisible(m_pParentWnd->m_hWnd))
		{
			if (0)//m_iIndex == 0 && g_DispArys.GetCount() != 1)
			{
				CRect rectDisp = g_DispArys.GetAt(m_iIndex);

				CRect rectDest;
				rectDest.left = 165 * rectDisp.Width() / 1920;
				rectDest.top = 160 * rectDisp.Height() / 1080;
				rectDest.right = 1720 * rectDisp.Width() / 1920;
				rectDest.bottom = 1034 * rectDisp.Height() / 1080;//867

				V(m_D3DDev->Present(NULL, &rectDest, NULL, NULL));
			}
			else
				hr = m_D3DDev->Present(NULL, NULL, NULL, NULL);

			if (FAILED(hr))
			{
//				CheckDeviceState();
				/*
				if (m_D3DDev->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
				{
				DeleteSurfaces();
				if (m_D3DDev)
				m_D3DDev->Release();

				if (m_D3D)
				m_D3D->Release();

				m_D3D = Direct3DCreate9(D3D_SDK_VERSION);
				V(CreateDevice());

				V(InitShareSurface());
				}
				*/
			}
			else
				m_bIsDeivceLost = false;

			m_dwLastDisplayTime = timeGetTime();
		}
	}
#else
	char szMsg[512];

	if (m_iIndex == 0)
	{
		DWORD dwCurTime = timeGetTime();

		if (m_dwLastDisplayTime == 0)
			m_dwLastDisplayTime = dwCurTime;

		if (dwCurTime >= m_dwLastDisplayTime + 40)//40,33
		{
		}
		else
			return;
	}

	DWORD dwTime2 = timeGetTime();
	if (IsWindowVisible(m_pParentWnd->m_hWnd))
	{
		bool bShowModel = !g_PannelSetting.bNotShowModal;

		if (bShowModel)
		{
			D3DXMATRIX matView, matProj;
			V(pD3DDevice->GetTransform(D3DTS_VIEW, &matView));
			V(pD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj));
			//V(m_D3DDev->SetTransform(D3DTS_VIEW, &m_matEndView));
			//V(m_D3DDev->SetTransform(D3DTS_PROJECTION, &m_matEndProj));

#ifdef _ENABLE_D3D9EX
			if (m_bIsD3D9Ex)
			{
				hr = ((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(0, 0, 0, 0, 0);
			}
#else
			hr = pD3DDevice->Present(NULL, NULL, NULL, NULL);
#endif
			hr = pD3DDevice->SetTransform(D3DTS_VIEW, &matView);
			hr = pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
		}
		else
		{
#ifdef _ENABLE_D3D9EX
			if (m_bIsD3D9Ex)
			{
				hr = ((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(0, 0, 0, 0, 0);
			}
#else
			hr = pD3DDevice->Present(NULL, NULL, NULL, NULL);
#endif
		}
	}

	DWORD dwTime3 = timeGetTime();

	int iCurFrameCount = m_pDisplayManager->GetFrameCount();

	if (iCurFrameCount > m_iLastFrameCount + 1)
	{
		sprintf(szMsg, "#MS# Panel:%d lose frame [%d - %d] \n", m_iIndex, m_iLastFrameCount, iCurFrameCount);
		OutputDebugStringA(szMsg);
	}
	/*
	if (m_iIndex > 0)
	{
		sprintf(szMsg, "#MS# Panel:%d %d \n", m_iIndex, dwTime3);
		OutputDebugStringA(szMsg);
	}
	*/
	m_iLastFrameCount = iCurFrameCount;

	m_AvgRenderSpendTime.Calculate(dwTime3 - dwTime2);

	if (m_AvgRenderSpendTime.GetCount() % 30 == 29)
	{
		float fSpendTime = m_AvgRenderSpendTime.GetAverage();

		if (m_iIndex > 0)
		{
			sprintf(szMsg, "#MS# Panel:%d %2.3f\n", m_iIndex, fSpendTime);
			OutputDebugStringA(szMsg);
		}
	}

#endif

//	CalPresentFPS();
}

void CAllocator::WaitForFinished()
{
	bool bDo = true;

	HANDLE handles[10];
	for (int j = 0; j<10; j++)
		handles[j] = m_hEffects[j];

	DWORD dwRet = WaitForMultipleObjects(10, handles, true, INFINITE);

	while (bDo)
	{
		if (m_pDisplayManager->GetRenderStatus() == RS_NORMAL)
		//if (m_pDisplayManager->IsD3DUsing() == 0)
		//if (m_bIsFinished)
			bDo = false;
		MSG Msg;
		if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		Sleep(1);
	}
}

void CAllocator::ApplyMask()
{
	HRESULT hr;
#ifdef _ENABLE_MASK

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	if (m_iIndex >= 1)
	{
		char szKeyName[512];
		char szData[128];

		m_bDoMask = false;

		if (m_MaskTexture)
			m_MaskTexture->Release();

		bool bEnableMask = GetPrivateProfileInt("Mask", "Enable", 0, theApp.m_strCurPath + "Setting.ini");

		sprintf(szKeyName, "Panel%d_File", m_iIndex);
		GetPrivateProfileString("Mask", szKeyName, "", m_szMaskFileName, 256, theApp.m_strCurPath + "Setting.ini");
		sprintf(szKeyName, "Panel%d_X", m_iIndex);
		m_iMaskX = GetPrivateProfileInt("Mask", szKeyName, 0, theApp.m_strCurPath + "Setting.ini");
		sprintf(szKeyName, "Panel%d_Y", m_iIndex);
		m_iMaskY = GetPrivateProfileInt("Mask", szKeyName, 0, theApp.m_strCurPath + "Setting.ini");

//		GetPrivateProfileString("Mask", szKeyName, "1.0", szData, 256, theApp.m_strCurPath + "Setting.ini");
//		m_fMaskScale = atof(szData);
		
		if (PathFileExists(m_szMaskFileName) && bEnableMask)
		{
			hr = D3DXCreateTextureFromFileEx(pD3DDevice, m_szMaskFileName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DCOLOR_ARGB(255, 255, 255, 255), NULL, NULL, &m_MaskTexture);
			if (D3D_OK == hr)
				m_bDoMask = true;
		}
		

		if (bEnableMask)
			m_bDoMask = true;

		//D3DXCreateSprite(m_D3DDev, &m_pSprite);
	}
#endif
}

int CAllocator::GetLastFrameCount()
{
	return m_iLastFrameCount;
}

DWORD CAllocator::GetLastPresentTime()
{
	return m_dwLastPresentTime;
}

IDirect3DDevice9* CAllocator::GetD3DDevice()
{
#ifdef _ENABLE_D3D9EX
	if (m_bIsD3D9Ex)
		return m_pD3DDeviceEx;
	return m_pD3DDevice;
#else
	return m_D3DDev;
#endif
}

IDirect3D9* CAllocator::GetDirect3D9Ptr()
{
#ifdef _ENABLE_D3D9EX
	if (m_bIsD3D9Ex)
		return m_pD3DEx;
	return m_pD3D;
#else
	return m_D3D;
#endif
}


#ifdef _ENABLE_D3D9EX

void CAllocator::Destroy()
{
	//DestroySurfaces();

	/* Destroy the D3D Device */
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	if (pD3DDevice)
		pD3DDevice->Release();
	pD3DDevice = NULL;

	/* Stop the whole Direct3D */
	IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();
	if (pD3D9Ptr)
		pD3D9Ptr->Release();

	pD3D9Ptr = NULL;
}
#endif

void CAllocator::DrawSelArea()
{
	if (m_ptAreaSel.bottom > 0)
	{
#ifdef _ENABLE_D3D9EX
		POINT p1, p2;

		IDirect3DDevice9* pD3DDevice = GetD3DDevice();

		IDirect3DSurface9* pOldRenderTargetSurface;
		pD3DDevice->GetRenderTarget(0, &pOldRenderTargetSurface);
		pD3DDevice->SetRenderTarget(0, m_renderTarget);

		p1.x = m_ptAreaSel.left;
		p1.y = m_ptAreaSel.top;
		p2.x = m_ptAreaSel.right;
		p2.y = m_ptAreaSel.top;
		m_pD3DPen->DrawLine(p1, p2);

		p1.x = m_ptAreaSel.right;
		p1.y = m_ptAreaSel.top;
		p2.x = m_ptAreaSel.right;
		p2.y = m_ptAreaSel.bottom;
		m_pD3DPen->DrawLine(p1, p2);

		p1.x = m_ptAreaSel.right;
		p1.y = m_ptAreaSel.bottom;
		p2.x = m_ptAreaSel.left;
		p2.y = m_ptAreaSel.bottom;
		m_pD3DPen->DrawLine(p1, p2);

		p1.x = m_ptAreaSel.left;
		p1.y = m_ptAreaSel.bottom;
		p2.x = m_ptAreaSel.left;
		p2.y = m_ptAreaSel.top;
		m_pD3DPen->DrawLine(p1, p2);

		pD3DDevice->SetRenderTarget(0, pOldRenderTargetSurface);
#else
		//		CClientDC cdc(m_pParentWnd);
		//		CBrush brhRed(RGB(255, 0, 0));
		m_pCDC->FrameRect(m_ptAreaSel, &m_brhRed);
#endif
	}
}

void CAllocator::CalPresentFPS()
{
	char szMsg[256];
	DWORD dwCurTime = timeGetTime();

	if (m_dwPresentFPSLastTime == 0)
		m_dwPresentFPSLastTime = dwCurTime;

	if (m_iPresentCount >= 30)
	{
		DWORD dwSpendTime = dwCurTime - m_dwPresentFPSLastTime;

		m_fPresentFPS = 30.0 / ((float)dwSpendTime / 1000.0);

		m_AvgPresentFPS.Calculate(m_fPresentFPS);

//		theApp.m_fPresentFPS = m_AvgPresentFPS.GetAverage();

		sprintf(szMsg, "#MS# CAllocator::CalPresentFPS(): [%d] %3.2F\n", m_iIndex, m_AvgPresentFPS.GetAverage());
		OutputDebugStringA(szMsg);

		m_iPresentCount = 0;
		m_dwPresentFPSLastTime = dwCurTime;
	}
	else
		m_iPresentCount++;
}

float CAllocator::GetAvgPresentFPS()
{
	if(m_AvgPresentFPS.GetCount() > 15)
		return m_AvgPresentFPS.GetAverage();
	return 25.0;
}

void CAllocator::DrawAimingPoint()
{
	//		CClientDC cdc(m_pParentWnd);
	HGDIOBJ pOldPen;

	pOldPen = m_MemoryDC->SelectObject(m_penRed);
	const UINT iRadius = 2000;
	const UINT iLinNum = 8;

	//		CBrush brhWhite(RGB(255, 255, 255));
	//		CBrush brhBlack(RGB(0, 0, 0));

	if (m_ptAry.GetCount() == 0)
		m_MemoryDC->FillRect(&m_rectClient, &m_brhWhite);
	else
		m_MemoryDC->FillRect(&m_rectClient, &m_brhBlack);

	BOOL bAreaFirstPt = TRUE;

	for (int x = 1; x < m_ptAry.GetCount(); x++)
	{
		if (m_ptAry.GetAt(x).iArea == m_uCurPtArea)
		{
			bAreaFirstPt = FALSE;
			break;
		}
	}

	if (bAreaFirstPt)
	{
#if 0
		for (int x = 0; x <= 8; x++)
		{
			m_MemoryDC->MoveTo(m_ptSel.x + (x - 4)*iRadius, m_ptSel.y - iRadius*iLinNum / 2);
			m_MemoryDC->LineTo(m_ptSel.x + (4 - x) * iRadius, m_ptSel.y + iRadius*iLinNum / 2);

			m_MemoryDC->MoveTo(m_ptSel.x - iRadius*iLinNum / 2, m_ptSel.y + (x - 4) * iRadius);
			m_MemoryDC->LineTo(m_ptSel.x + iRadius*iLinNum / 2, m_ptSel.y + (4 - x) * iRadius);
		}
#else
		m_MemoryDC->MoveTo(m_ptSel.x, m_rectClient.top);
		m_MemoryDC->LineTo(m_ptSel.x , m_rectClient.bottom);

		m_MemoryDC->MoveTo(m_rectClient.left, m_ptSel.y);
		m_MemoryDC->LineTo(m_rectClient.right, m_ptSel.y);

		m_MemoryDC->MoveTo(m_ptSel.x - 10, m_ptSel.y - 10);
		m_MemoryDC->LineTo(m_ptSel.x + 10, m_ptSel.y - 10);
		m_MemoryDC->LineTo(m_ptSel.x + 10, m_ptSel.y + 10);
		m_MemoryDC->LineTo(m_ptSel.x - 10, m_ptSel.y + 10);
		m_MemoryDC->LineTo(m_ptSel.x - 10, m_ptSel.y - 10);
#endif
	}
	else
	{
		CPoint ptTemp;
		INT32 iCurAreaIdx = -1;
		BlendingPt bptTemp;

		for (int x = 1; x < m_ptAry.GetCount(); x++)
		{
			bptTemp = m_ptAry.GetAt(x);

			if (!bptTemp.bDarkMask)
				m_MemoryDC->SelectObject(m_penArea[bptTemp.iArea % 6]);
			else
				m_MemoryDC->SelectObject(m_penBlack);

			if (bptTemp.iArea != iCurAreaIdx)
			{
				iCurAreaIdx = bptTemp.iArea;
				m_MemoryDC->MoveTo(bptTemp.pt);
			}
			else
			{
				m_MemoryDC->LineTo(bptTemp.pt);
			}
		}
	}

	m_MemoryDC->SelectObject(m_penArea[m_uCurPtArea % 6]);
//	m_MemoryDC->LineTo(m_ptSel);
	m_MemoryDC->SelectObject(pOldPen);

	m_pCDC->BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(), m_MemoryDC, 0, 0, SRCCOPY);
}

void CAllocator::SetDisplayScale(float fX, float fY)
{
//	m_scene.SetDisplayScale(fX,fY);
}


void CAllocator::DoEffect()
{
	HRESULT hr;

	char szMsg[512];

#ifdef _ENABLE_SINGLE_EFFECT
	IDirect3DSurface9 *pVideoSurface = NULL;
	CMixerEffect* pMixerEffect = m_pDisplayManager->GetMixerEffect();
	sEffectParam effect_param = pMixerEffect->GetEffectParam();
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	TimelineManager* m_manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();

#ifdef _ENABLE_GPU
	m_pDisplayManager->Suspend();
#else //_ENABLE_GPU
#endif //_ENABLE_GPU

	int iEffectSpendTime = 0;
	int iDrawSpendTime = 0;

//	if (m_manager->GetPBStatus() == PBC_PLAY)
//		m_pDisplayManager->DoAllHDEffects();
	
	IDirect3DSurface9 *pBackBuffer;
	hr = pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	
	m_pD3DDeviceEx->SetRenderTarget(0, pBackBuffer);

	D3DSURFACE_DESC d3d_surface_desc;
	for (UINT x = 0; x < m_BufTexture.size(); x++)
	{
		if (!m_pTmpSurfaces[x])
			continue;

		D3DXMATRIX matProj, matView, matWorld;
		pD3DDevice->GetTransform(D3DTS_WORLD, &matWorld);
		pD3DDevice->GetTransform(D3DTS_VIEW, &matView);
		pD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj);

		m_BufTexture[x]->GetSurfaceLevel(0, &pVideoSurface);

		
		if (pMixerEffect->GetEffectID() != 0 && effect_param.iLevel != 0 && m_pDisplayManager->EffectIsEnabled() && m_manager->GetPBStatus() == PBC_PLAY)
		//if (pMixerEffect->GetEffectID() != 0 && effect_param.iLevel != 0 && m_manager->GetPBStatus() == PBC_PLAY)
		{
			pVideoSurface->GetDesc(&d3d_surface_desc);
			
#ifdef _ENABLE_HD_EFFECT
			
			hr = pD3DDevice->StretchRect((IDirect3DSurface9 *)m_pTmpSurfaces[x], NULL, pVideoSurface, NULL, D3DTEXF_NONE);

			STREM_MAPPING smTemp = theApp.GetStreamMapByIdx(m_iIndex, x);
			int iSurfaceIndex = g_MediaStreamAry.at(smTemp.iMediaIdx)->GetSurfaceIndex(m_iIndex);
			D3DSurfaceItem* pD3DSurfaceItem = m_pDisplayManager->GetSurface(iSurfaceIndex);
			
		//	IDirect3DSurface9* pHDSurface = m_pDisplayManager->GetHDSurface(iSurfaceIndex);
		//	if(pHDSurface)
			//	hr = pD3DDevice->StretchRect((IDirect3DSurface9 *)pHDSurface, NULL, pVideoSurface, NULL, D3DTEXF_NONE);
				
			
//			hr = pD3DDevice->StretchRect((IDirect3DSurface9 *)pD3DSurfaceItem->pOverlaySurface, NULL, pVideoSurface, NULL, D3DTEXF_NONE);
			hr = pD3DDevice->StretchRect((IDirect3DSurface9 *)m_pTmpSurfaces[x], NULL, pVideoSurface, NULL, D3DTEXF_NONE);

#else   
			if (pMixerEffect->GetWidth() != d3d_surface_desc.Width || pMixerEffect->GetHeight() != d3d_surface_desc.Height)
				pMixerEffect->ResetSurfaces(d3d_surface_desc.Width, d3d_surface_desc.Height);

			if (pMixerEffect->GetEffectID() >= HD_EID_BRIGHTNESS)
				hr = pD3DDevice->StretchRect((IDirect3DSurface9 *)m_pTmpSurfaces[x], NULL, pVideoSurface, NULL, D3DTEXF_NONE);
			else
			{
				pMixerEffect->SetTime(m_fEffectTime[x]);

				IDirect3DSurface9* pOutSurface = pMixerEffect->DoVideoEffect_Surface(m_pTmpSurfaces[x], d3d_surface_desc.Width, d3d_surface_desc.Height);
				if (pOutSurface)
					hr = pD3DDevice->StretchRect((IDirect3DSurface9 *)pOutSurface, NULL, pVideoSurface, NULL, D3DTEXF_NONE);
				m_fEffectTime[x] = pMixerEffect->GetTime();
			}
#endif
			
		}
		else if (pMixerEffect->GetEffectID() != 0 && effect_param.iLevel != 0 && m_pDisplayManager->EffectIsEnabled() && m_manager->GetPBStatus() == PBC_PAUSE)
			;//hr = pD3DDevice->StretchRect((IDirect3DSurface9 *)m_pTmpSurfaces[x], NULL, pVideoSurface, NULL, D3DTEXF_NONE);
		else 
			hr = pD3DDevice->StretchRect((IDirect3DSurface9 *)m_pTmpSurfaces[x], NULL, pVideoSurface, NULL, D3DTEXF_NONE);

	//	hr = pD3DDevice->StretchRect((IDirect3DSurface9 *)m_pTmpSurfaces[x], NULL, pVideoSurface, NULL, D3DTEXF_NONE);

		pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
		pD3DDevice->SetTransform(D3DTS_VIEW, &matView);
		pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
	}
	

	if(pVideoSurface)
		pVideoSurface->Release();
	pBackBuffer->Release();
#ifdef _ENABLE_GPU
	m_pDisplayManager->Resume();
#else  //_ENABLE_GPU
#endif  //_ENABLE_GPU
		
#endif
}

void CAllocator::EnableDoClear(bool bEnable)
{
	m_bDoClear = bEnable;
//	m_iWaitCount = 2;
}

void CAllocator::CheckEventStatus()
{
	HANDLE handles[10];
	for (int j = 0; j<10; j++)
		handles[j] = m_hEffects[j];

	DWORD dwRet = WaitForMultipleObjects(10, handles, true, INFINITE);  //1000
}

void CAllocator::SetSyncPBCtrl(SyncPBCtrl* pObj)
{
	m_pSyncPBCtrl = pObj;
}

void CAllocator::ApplySetting()
{
	bool bSync = GetPrivateProfileInt("Mapping_Setting", "Sync", 0, theApp.m_strCurPath + "Setting.ini");
	int iMPFPS = GetPrivateProfileInt("Mapping_Setting", "FPS", 25, theApp.m_strCurPath + "Setting.ini");

	m_bDoPBSync = bSync;

	if (m_pSyncPBCtrl)
	{
		if(m_iIndex == 0)
			m_pSyncPBCtrl->Reset();

		if (m_pSyncPBCtrl && m_bDoPBSync)
			m_pSyncPBCtrl->SetSourceCount(m_iIndex, m_surfaces.size());
	}

	m_iPresentInterval = (float)1000.0 / (float)iMPFPS;
}

#ifdef _TRAIL_VERSION
void CAllocator::DrawWatermark()
{
	if ((g_PannelAry.GetCount() > 1 && m_iIndex != 0) || g_PannelAry.GetCount() == 1)
	{
		INT x, y, wx, wy;

		x = m_iWatermarkFrameCount * 10;
		wx = 1920 - (352 / 2);
		x = (x / wx) & 0x1 ? wx - (x % wx) : x % wx;

		y = m_iWatermarkFrameCount * 10;
		wy = 1080 - (292 / 2);
		y = (y / wy) & 0x1 ? wy - (y % wy) : y % wy;

		m_pD3DSprite->Scale(0.5, 0.5);
		m_pD3DSprite->Draw(m_pWatermarkTexture, x * 2, y * 2,128);

		m_iWatermarkFrameCount++;
	}
}
#endif

#ifdef _VF_SINGLE_THREAD
unsigned int __stdcall UpdateVideoThread(void* lpvThreadParm)
{
	CAllocator* pObj = (CAllocator*)lpvThreadParm;

	while (pObj->IsAlive2())
	{
		pObj->UpdateVideo();
//		Sleep(1);
	}
	return 0;
}
#endif