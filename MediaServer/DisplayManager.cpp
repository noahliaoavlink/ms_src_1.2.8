#include "stdafx.h"
#include "DisplayManager.h"
#include "MediaServer.h"
#include "ServiceProvider.h"

#include "../../../Include/ShellCommon.h"
#include "../../../Include/EffectParam.h"

#include "Mmsystem.h"
#pragma comment(lib, "Winmm.lib")

void GetGridRect(int iIndex, RECT* pRect, int iCellW, int iCellH,int w,int h)
{
	int iCount = 0;
	int iHFactor = h / iCellH;
	int iWFactor = w / iCellW;
	for (int i = 0; i < iCellH; i++)
	{
		for (int j = 0; j < iCellW; j++)
		{
			if (iIndex == iCount)
			{
				pRect->left = iWFactor * j;
				pRect->top = iHFactor * i;
				pRect->right = pRect->left + iWFactor;
				pRect->bottom = pRect->top + iHFactor;
				return;
			}
			iCount++;
		}
	}
}

unsigned int __stdcall _PlayerThreadProc(void* lpvThreadParm);

FPSCounter::FPSCounter()
{
	m_dwLastTime = 0;
	//m_dwLastTime = 0;
	m_iCount = 0;
	m_AvgFPS.SetMax(30);
}

FPSCounter::~FPSCounter()
{
}

void FPSCounter::CalFPS() 
{
	char szMsg[256];
	DWORD dwCurTime = timeGetTime();

	if (m_dwLastTime == 0)
		m_dwLastTime = dwCurTime;

	if (m_iCount >= 30)
	{
		DWORD dwSpendTime = dwCurTime - m_dwLastTime;

		m_fFPS = 30.0 / ((float)dwSpendTime / 1000.0);

		m_AvgFPS.Calculate(m_fFPS);

		sprintf(szMsg, "#LoadingCtrl# FPSCounter::CalFPS(): %3.2F\n", m_AvgFPS.GetAverage());
		OutputDebugStringA(szMsg);

		m_iCount = 0;
		m_dwLastTime = dwCurTime;
	}
	else
		m_iCount++;
}

float FPSCounter::GetAvgFPS() 
{
	return m_AvgFPS.GetAverage();
}

DisplayManager::DisplayManager()
{
	m_pD3DDevice = 0;
	m_dwLastTime = 0;
	m_bDoI420ToYV12 = false;
	m_hThread = 0;

	m_iOffset = 0;

	m_hWnd = NULL;
	m_hPanelWnd = NULL;

	m_pD3DSurfaceList = new SQList;
	m_pD3DSurfaceList->EnableRemoveData(false);

	m_pRawDataBuffer = 0;
	m_pRawDataSurface = 0;
	m_bEnableRecordAVI = false;
	m_bIsSuspend = false;

	m_pD3Dpp = 0;
	m_hTimeLineHwnd = 0;
	m_iPanelIndex = -1;
	m_bIsDeivceLost = false;

	m_pMFRecorderDll = new MFRecorderDll;

	int iBufLen = 4096 * 4096 * 3 / 2;

	m_pBlackBuffer = new unsigned char[iBufLen];

	memset(m_pBlackBuffer, 16, 640 * 240);
	memset(m_pBlackBuffer + 640 * 240, 128, iBufLen - 640 * 240);

	m_bGPUIsEnabled = false;

	m_bIsD3D9Ex = false;

	for (int j = 0; j < 16; j++)
	{
		m_dCurPos[j] = 0.0;
		m_bUpdate[j] = false;
	}

#if (_ENABLE_LINE_MARK && _ENABLE_VIDEO_WALL)
	for (int i = 0; i < 6; i++)
	{
		m_pD3DPen[i] = 0;
		m_pD3DColorBarPen[i];

//		m_hPen[i];
	}

	m_iMarkMode = MM_NONE;
	m_iFrameCount = 0;
#endif
	//m_iSuspendCount = 0;

#ifdef _ENABLE_SINGLE_EFFECT
	m_pMixerEffect = 0;

	m_pOffscreenTexture = 0;
	m_pOffscreenSurface = 0;
#endif

#ifdef _ENABLE_GPU
	for (int k = 0; k < 10; k++)
		m_pProcessorHDs[k] = 0;
#endif

	m_iPresentInterval = 100;

	InitializeCriticalSection(&m_CriticalSection);
}

DisplayManager::~DisplayManager()
{
#ifdef _ENABLE_GPU
	for (int k = 0; k < 10; k++)
	{
		if (m_pProcessorHDs[k])
			delete m_pProcessorHDs[k];
	}
#endif

	DestroyAllD3DSurfaces();

	if (m_pRawDataBuffer)
		delete m_pRawDataBuffer;

	if (m_pRawDataSurface)
		m_pRawDataSurface->Release();

	if (m_pMFRecorderDll)
		delete m_pMFRecorderDll;

	if (m_pBlackBuffer)
		delete m_pBlackBuffer;

#if (_ENABLE_LINE_MARK && _ENABLE_VIDEO_WALL)
	for (int i = 0; i < 6; i++)
	{
		if (m_pD3DPen[i])
			delete m_pD3DPen[i];

		if (m_pD3DColorBarPen[i])
			delete m_pD3DColorBarPen[i];

//		if (m_hPen[i])
//			DeleteObject(m_hPen[i]);
	}
#endif


#ifdef _ENABLE_SINGLE_EFFECT
	if (m_pMixerEffect)
		m_pMixerEffect->DestroySurfaces();

	if (m_pOffscreenSurface)
		m_pOffscreenSurface->Release();

	if (m_pOffscreenTexture)
		m_pOffscreenTexture->Release();
#endif

	DeleteCriticalSection(&m_CriticalSection);
}

void DisplayManager::Init(HWND hWnd , IDirect3DDevice9* pD3DDevice, bool bIsD3D9Ex)
{
	HRESULT hr;
	m_hWnd = hWnd;
//	DestroyAllD3DSurfaces();

	m_bIsD3D9Ex = bIsD3D9Ex;

	if (m_bIsD3D9Ex)
		m_pD3DDeviceEx = (IDirect3DDevice9Ex*)pD3DDevice;
	else
		m_pD3DDevice = pD3DDevice;

	IDirect3DDevice9* pCurD3DDevice = GetD3DDevice();

	CheckAdapterInfo();

#if (_ENABLE_LINE_MARK && _ENABLE_VIDEO_WALL)
	m_pD3DPen[0] = new D3DPen;
	m_pD3DPen[0]->Create(pCurD3DDevice,2, D3DCOLOR_ARGB(255,255,255,255));

	m_pD3DPen[1] = new D3DPen;
	m_pD3DPen[1]->Create(pCurD3DDevice, 2, D3DCOLOR_ARGB(255, 255, 0, 0));

	m_pD3DPen[2] = new D3DPen;
	m_pD3DPen[2]->Create(pCurD3DDevice, 2, D3DCOLOR_ARGB(255, 255, 255, 0));

	m_pD3DPen[3] = new D3DPen;
	m_pD3DPen[3]->Create(pCurD3DDevice, 2, D3DCOLOR_ARGB(255, 255, 128, 0));

	m_pD3DPen[4] = new D3DPen;
	m_pD3DPen[4]->Create(pCurD3DDevice, 2, D3DCOLOR_ARGB(255, 0, 0, 255));

	m_pD3DPen[5] = new D3DPen;
	m_pD3DPen[5]->Create(pCurD3DDevice, 2, D3DCOLOR_ARGB(255, 0, 255, 0));

	m_pD3DColorBarPen[0] = new D3DPen;
	m_pD3DColorBarPen[0]->Create(pCurD3DDevice, 10, D3DCOLOR_ARGB(255, 255, 255, 255));

	m_pD3DColorBarPen[1] = new D3DPen;
	m_pD3DColorBarPen[1]->Create(pCurD3DDevice, 10, D3DCOLOR_ARGB(255, 255, 0, 0));

	m_pD3DColorBarPen[2] = new D3DPen;
	m_pD3DColorBarPen[2]->Create(pCurD3DDevice, 10, D3DCOLOR_ARGB(255, 255, 255, 0));

	m_pD3DColorBarPen[3] = new D3DPen;
	m_pD3DColorBarPen[3]->Create(pCurD3DDevice, 10, D3DCOLOR_ARGB(255, 255, 128, 0));

	m_pD3DColorBarPen[4] = new D3DPen;
	m_pD3DColorBarPen[4]->Create(pCurD3DDevice, 10, D3DCOLOR_ARGB(255, 0, 0, 255));

	m_pD3DColorBarPen[5] = new D3DPen;
	m_pD3DColorBarPen[5]->Create(pCurD3DDevice, 10, D3DCOLOR_ARGB(255, 0, 255, 0));
/*
	m_hPen[0] = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
	m_hPen[1] = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	m_hPen[2] = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
	m_hPen[3] = CreatePen(PS_SOLID, 2, RGB(255, 128, 0));
	m_hPen[4] = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	m_hPen[5] = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	*/
#endif

	m_pRawDataBuffer = new unsigned char[m_iWidth * m_iHeight * 4];
	hr = pCurD3DDevice->CreateOffscreenPlainSurface(m_iWidth, m_iHeight, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &m_pRawDataSurface, NULL);

	m_pMFRecorderDll->LoadLib();
	m_pMFRecorderDll->Init();

#ifdef _ENABLE_SINGLE_EFFECT
	CreateMixEffect(pCurD3DDevice, GetDesktopD3DFormat(), 1920, 1080);

	m_pOffscreenTexture = CreateOverlayTexture(m_iWidth, m_iHeight);
//	m_pOffscreenTexture->GetSurfaceLevel(0, &m_pOffscreenSurface);

	m_bDoEffect = true; 
#endif

#ifdef _ENABLE_GPU
	for (int k = 0; k < 10; k++)
	{
		m_pProcessorHDs[k] = new ProcessorHD;
		m_pProcessorHDs[k]->Init(hWnd, (IDirect3DDevice9Ex*)m_pD3DDeviceEx, 1920, 1080);

		//m_pProcessorHD->SetBrightness(10);
		//m_pProcessorHD->SetRedAlpha(10);
		/*
		m_pProcessorHD->SetContrast(50.0);
		m_pProcessorHD->SetHue(50.0);
		m_pProcessorHD->SetSaturation(50.0);
		*/
	}
#endif

	ApplySetting();
}

void DisplayManager::SetTimeLineHwnd(HWND hWnd)
{
	m_hTimeLineHwnd = hWnd;
}

HWND DisplayManager::GetTimeLineHwnd()
{
	return m_hTimeLineHwnd;
}

void DisplayManager::SetPanelIndex(int iIndex)
{
	m_iPanelIndex = iIndex;
}

void DisplayManager::ResetD3DDevice(IDirect3DDevice9* pD3DDevice)
{
	HRESULT hr;

	ResetAllD3DSurfaces();

	if (m_bIsD3D9Ex)
		m_pD3DDeviceEx = (IDirect3DDevice9Ex*)pD3DDevice;
	else
		m_pD3DDevice = pD3DDevice;

	//m_pD3DDevice = pD3DDevice;

	IDirect3DDevice9* pCurD3DDevice = GetD3DDevice();

	if (m_pRawDataSurface)
		m_pRawDataSurface->Release();

	hr = pCurD3DDevice->CreateOffscreenPlainSurface(m_iWidth, m_iHeight, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &m_pRawDataSurface, NULL);
}

void DisplayManager::Start()
{
	unsigned threadID1;
	m_bIsAlive = true;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, _PlayerThreadProc, this, 0, &threadID1);
}

void DisplayManager::Stop()
{
	m_bIsAlive = false;
	::WaitForSingleObject(m_hThread, 1000);
}

int DisplayManager::CreateSurfaceItem()
{
	D3DSurfaceItem* pNewSurfaceItem = new D3DSurfaceItem;

	m_iItemIndex = m_pD3DSurfaceList->GetTotal();

	pNewSurfaceItem->pD3DSurface = 0;
	pNewSurfaceItem->iPixelFormat = 0;
	pNewSurfaceItem->iWidth = 0;
	pNewSurfaceItem->iHeight = 0;
	pNewSurfaceItem->pOverlayTexture = 0;
	pNewSurfaceItem->pOverlaySurface = 0;
	m_pD3DSurfaceList->Add(pNewSurfaceItem);

	return m_iItemIndex;
}

void DisplayManager::DestroyAllD3DSurfaces()
{
	int iTotal = m_pD3DSurfaceList->GetTotal();
	for (int i = 0; i < iTotal; i++)
	{
		D3DSurfaceItem* pCurD3DSurfaceItem = (D3DSurfaceItem*)m_pD3DSurfaceList->Get(i);
		if (pCurD3DSurfaceItem)
		{
			if (pCurD3DSurfaceItem->pD3DSurface)
			{
				DestroyOverlayResource(pCurD3DSurfaceItem->pOverlaySurface, pCurD3DSurfaceItem->pOverlayTexture);
				pCurD3DSurfaceItem->pOverlaySurface = 0;
				pCurD3DSurfaceItem->pOverlayTexture = 0;
				DeleteSurface(pCurD3DSurfaceItem->pD3DSurface);
				pCurD3DSurfaceItem->pD3DSurface = 0;

				pCurD3DSurfaceItem->iHeight = 0;
				pCurD3DSurfaceItem->iWidth = 0;
			}
			delete pCurD3DSurfaceItem;
		}
	}

	m_pD3DSurfaceList->Reset();
}

void DisplayManager::ResetAllD3DSurfaces()
{
	EnterCriticalSection(&m_CriticalSection);
	int iTotal = m_pD3DSurfaceList->GetTotal();
	for (int i = 0; i < iTotal; i++)
	{
		D3DSurfaceItem* pCurD3DSurfaceItem = (D3DSurfaceItem*)m_pD3DSurfaceList->Get(i);
		if (pCurD3DSurfaceItem)
		{
			if (pCurD3DSurfaceItem->pD3DSurface)
			{
				DestroyOverlayResource(pCurD3DSurfaceItem->pOverlaySurface, pCurD3DSurfaceItem->pOverlayTexture);
				pCurD3DSurfaceItem->pOverlaySurface = 0;
				pCurD3DSurfaceItem->pOverlayTexture = 0;
				DeleteSurface(pCurD3DSurfaceItem->pD3DSurface);
				pCurD3DSurfaceItem->pD3DSurface = 0;

				pCurD3DSurfaceItem->iHeight = 0;
				pCurD3DSurfaceItem->iWidth = 0;
			}
//			delete pCurD3DSurfaceItem;
		}
	}
	LeaveCriticalSection(&m_CriticalSection);
}

void DisplayManager::DestroyOverlayResource(IDirect3DSurface9* pSurface, IDirect3DTexture9* pTexture)
{
	if (pSurface)
		pSurface->Release();

	pSurface = NULL;

	if (pTexture)
		pTexture->Release();

	pTexture = NULL;
}

void DisplayManager::DeleteSurface(IDirect3DSurface9* pSurface)
{
	if (pSurface)
		pSurface->Release();
	pSurface = NULL;
}

D3DSurfaceItem* DisplayManager::GetSurface(int iIndex)
{
	EnterCriticalSection(&m_CriticalSection);
	if (iIndex < m_pD3DSurfaceList->GetTotal())
	{
		m_pTmpD3DSurfaceItem = (D3DSurfaceItem*)m_pD3DSurfaceList->Get(iIndex);
		LeaveCriticalSection(&m_CriticalSection);
		return m_pTmpD3DSurfaceItem;
	}
	LeaveCriticalSection(&m_CriticalSection);
	return 0;
}

int DisplayManager::GetTotalOfSurfaceItems()
{
	return m_pD3DSurfaceList->GetTotal();
}

IDirect3DDevice9* DisplayManager::GetD3DDevice()
{
	if (m_bIsD3D9Ex)
		return m_pD3DDeviceEx;
	return m_pD3DDevice;
}

void DisplayManager::CheckAdapterInfo()
{
	char szMsg[512];

	D3DDISPLAYMODE d3ddm;
	IDirect3D9* pD3D9Ptr;

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	pD3DDevice->GetDirect3D(&pD3D9Ptr);

	HMONITOR hMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	UINT uAdapterCount = pD3D9Ptr->GetAdapterCount();
	for (UINT i = 0; i < uAdapterCount; i++)
	{
		HMONITOR hmAdapter = pD3D9Ptr->GetAdapterMonitor(i);

		//sprintf(szMsg, "#nCCTV_HD_D3D# D3DRender::GetDeviceInfo() 0 [%d , %d]\n", i, hmAdapter);
		//OutputDebugStringA(szMsg);

		if (hmAdapter == hMonitor)
			m_iAdapter = i;
	}

	RECT rect;
	GetWindowRect(m_hWnd, &rect);
	//sprintf(szMsg, "#nCCTV_HD_D3D# DisplayManager::GetDeviceInfo()  [%d , %d] rect = [%d,%d,%d,%d]\n", m_hWnd, m_iAdapter, rect.left, rect.top,rect.right,rect.bottom);
	//OutputDebugStringA(szMsg);

	pD3D9Ptr->GetAdapterDisplayMode(m_iAdapter, &d3ddm);

	m_iWidth = d3ddm.Width;
	m_iHeight = d3ddm.Height;
	m_DesktopD3DFormat = d3ddm.Format;
}

IDirect3DTexture9* DisplayManager::CreateOverlayTexture(int iWidth, int iHeight)
{
	IDirect3DTexture9* pTexture = NULL;
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	HRESULT hr = pD3DDevice->CreateTexture(iWidth, iHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &pTexture, NULL);

//	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
//	HRESULT hr = pD3DDevice->CreateTexture(iWidth, iHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &pTexture, NULL);
	return pTexture;
}

bool DisplayManager::FormatSupported(D3DFORMAT surface/*, D3DFORMAT adaptor*/)
{
	D3DDISPLAYMODE d3ddm;

	IDirect3D9* pD3D9Ptr;
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	pD3DDevice->GetDirect3D(&pD3D9Ptr);
	//IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();
	if (!pD3D9Ptr)
		return false;

	pD3D9Ptr->GetAdapterDisplayMode(m_iAdapter, &d3ddm);


	HRESULT hr = pD3D9Ptr->CheckDeviceFormat(m_iAdapter, D3DDEVTYPE_HAL,
		d3ddm.Format /*adaptor*/, 0, D3DRTYPE_SURFACE, surface);
	if (SUCCEEDED(hr))
	{
		hr = pD3D9Ptr->CheckDeviceFormatConversion(m_iAdapter, D3DDEVTYPE_HAL, surface, d3ddm.Format/*adaptor*/);
		if (SUCCEEDED(hr))
			return true;
	}
	return false;
}

D3DFORMAT DisplayManager::GetDesktopD3DFormat()
{
	return m_DesktopD3DFormat;
}

int DisplayManager::GetD3DFormat(int iPixelFormat)
{
	switch (iPixelFormat)
	{
	case VPF_I420:
		return MAKEFOURCC('I', '4', '2', '0');	//MAKEFOURCC('Y','V','1','2');	
		break;
	case VPF_YUY2:
		return  MAKEFOURCC('Y', 'U', 'Y', '2');
		break;
	case VPF_RGB32:
		return D3DFMT_A8R8G8B8;
		break;
	case VPF_RGB24:
		return D3DFMT_R8G8B8;
		break;
	case VPF_NV12:
		return MAKEFOURCC('N', 'V', '1', '2');
		break;
	case VPF_YV12:
		return MAKEFOURCC('Y', 'V', '1', '2');
		break;
	}
}

IDirect3DSurface9* DisplayManager::CreateSurfaces(int iWidth, int iHeight, D3DFORMAT format)
{
	//format = (D3DFORMAT)GetD3DFormat(VPF_YV12);
	IDirect3DSurface9* temp_surface = NULL;
	bool bHasHW = FormatSupported(format);
	if (!bHasHW)
	{
		/*
		char szMsg[256];
		sprintf(szMsg, "Can`nt support %s format!!", GetD3DFormatName(format));
		MessageBoxA(NULL, szMsg, "", MB_OK);
		*/
		return 0;
	}

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	HRESULT hr = pD3DDevice->CreateOffscreenPlainSurface(iWidth, iHeight,
		format, D3DPOOL_DEFAULT, &temp_surface, NULL);
	if (FAILED(hr))
	{
		/*
		CString strTemp;
		strTemp.Format("%i, %i, %d, %d", iWidth, iHeight, format, (temp_surface == NULL));
		AfxMessageBox(strTemp);
		MessageBoxA(NULL, "Failed to create surface.", "", MB_OK);
		*/
		return 0;
	}

//	SetupDefaultRenderState();

	return temp_surface;
}

void DisplayManager::EnableI420ToYV12(bool bEnable)
{
	m_bDoI420ToYV12 = bEnable;
}

void DisplayManager::SetRect(int iIndex, RECT rect)
{
	m_RectList[iIndex].left = rect.left;
	m_RectList[iIndex].top = rect.top;
	m_RectList[iIndex].right = rect.right;
	m_RectList[iIndex].bottom = rect.bottom;
}

int DisplayManager::UpdateSurfaceData(IDirect3DSurface9* pSurface, unsigned char* pBuffer, int iWidth, int iHeight)
{
	char szMsg[512];
	D3DLOCKED_RECT d3drect;
	D3DSURFACE_DESC d3d_surface_desc;

	pSurface->GetDesc(&d3d_surface_desc);

	if (d3d_surface_desc.Format == (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2'))
		return 1;

	HRESULT hr = pSurface->LockRect(&d3drect, NULL, 0);

	if (FAILED(hr))
	{
		//MessageBoxA(NULL,"Failed to lock picture surface.","",MB_OK);
		sprintf_s(szMsg, "D3DError - Failed to lock picture surface!! %s \n", "");
		OutputDebugStringA(szMsg);
		return 0;
	}

	int i;
	unsigned char *dst = (unsigned char*)d3drect.pBits;
	unsigned char *src = pBuffer;

	if (d3d_surface_desc.Format == (D3DFORMAT)MAKEFOURCC('I', '4', '2', '0'))
	{
		if (d3drect.Pitch == iWidth)
		{
			//m_iOffset = 0;
			BOOL bRet = IsBadReadPtr(src, iWidth * iHeight * 3 / 2);
			if (bRet)
			{
				//char szMsg[512];
				sprintf_s(szMsg, "D3DError - UpdateSurfaceData IsBadReadPtr !!\n");
				OutputDebugStringA(szMsg);
				return 0;
			}
			memcpy(dst, src, iWidth * iHeight * 3 / 2);
		}
		else
		{
			if(iWidth > m_iWidth)
				m_iOffset = d3drect.Pitch - iWidth;
			/*
			for (i = 0 ; i < iHeight ; i++)
			{
			memcpy(dst, src, iWidth);
			dst += d3drect.Pitch;
			src += iWidth;
			}
			for (i = 0 ; i < iHeight / 2 ; i++)
			{
			memcpy(dst, src, iWidth);
			dst += d3drect.Pitch;
			src += iWidth;
			}
			*/
			int chroma_width = iWidth >> 1;
			int chroma_height = iHeight >> 1;
			int chroma_pitch = d3drect.Pitch >> 1;

			for (i = 0; i < iHeight; i++)
			{
				memcpy(dst, src, iWidth);
				dst += d3drect.Pitch;
				src += iWidth;
			}


			dst = (unsigned char*)d3drect.pBits + (iHeight * d3drect.Pitch * 5 / 4);
			src = pBuffer + (iHeight * iWidth * 5 / 4);

			for (i = 0; i < chroma_height; i++)
			{
				memcpy(dst, src, chroma_width);
				dst += chroma_pitch;
				src += chroma_width;
			}

			dst = (unsigned char*)d3drect.pBits + (iHeight * d3drect.Pitch);
			src = pBuffer + (iHeight * iWidth);

			for (i = 0; i < chroma_height; i++)
			{
				memcpy(dst, src, chroma_width);
				dst += chroma_pitch;
				src += chroma_width;
			}
		}
	}
	else if (d3d_surface_desc.Format == (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'))
	{
		//I420 -> YV12
		{
			
			int chroma_width = iWidth >> 1;
			int chroma_height = iHeight >> 1;
			int chroma_pitch = d3drect.Pitch >> 1;

			for (i = 0; i < iHeight; i++)
			{
				memcpy(dst, src, iWidth);
				dst += d3drect.Pitch;
				src += iWidth;
			}

			dst = (unsigned char*)d3drect.pBits + (iHeight * d3drect.Pitch);
			src = pBuffer + (iHeight * iWidth * 5 / 4);

			for (i = 0; i < chroma_height; i++)
			{
				memcpy(dst, src, chroma_width);
				dst += chroma_pitch;
				src += chroma_width;
			}

			dst = (unsigned char*)d3drect.pBits + (iHeight * d3drect.Pitch * 5 / 4);
			src = pBuffer + (iHeight * iWidth);

			for (i = 0; i < chroma_height; i++)
			{
				memcpy(dst, src, chroma_width);
				dst += chroma_pitch;
				src += chroma_width;
			}
		}
	}
	else if (d3d_surface_desc.Format == (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2'))
	{
		if (d3drect.Pitch == iWidth)
			memcpy(dst, src, iWidth * iHeight * 3 / 2);
		else
		{
			for (i = 0; i < iHeight; i++)
			{
				memcpy(dst, src, iWidth);
				dst += d3drect.Pitch;
				src += iWidth;
			}
			for (i = 0; i < iHeight / 2; i++)
			{
				memcpy(dst, src, iWidth);
				dst += d3drect.Pitch;
				src += iWidth;
			}
		}
	}
	else if (d3d_surface_desc.Format == (D3DFORMAT)MAKEFOURCC('Y', 'U', 'Y', '2'))
	{
		if (d3drect.Pitch == iWidth)
			memcpy(dst, src, iWidth * iHeight * 2);
		else
		{
			int iBytesbyPerLine = iWidth * 2;
			for (i = 0; i < iHeight; i++)
			{
				memcpy(dst, src, iBytesbyPerLine);
				src += iBytesbyPerLine;
				dst += d3drect.Pitch;
			}
		}
	}
	else if (d3d_surface_desc.Format == D3DFMT_A8R8G8B8)  //RGBA 32
	{
		if (d3drect.Pitch == iWidth)
			memcpy(dst, src, iWidth * iHeight * 4);
		else
		{
			int iBytesbyPerLine = iWidth * 4;
			for (i = 0; i < iHeight; i++)
			{
				memcpy(dst, src, iBytesbyPerLine);
				src += iBytesbyPerLine;
				dst += d3drect.Pitch;
			}
		}
	}
	else if (d3d_surface_desc.Format == D3DFMT_R8G8B8)
	{
		if (d3drect.Pitch == iWidth)
			memcpy(dst, src, iWidth * iHeight * 3);
		else
		{
			int iBytesbyPerLine = iWidth * 3;
			for (i = 0; i < iHeight; i++)
			{
				memcpy(dst, src, iBytesbyPerLine);
				src += iBytesbyPerLine;
				dst += d3drect.Pitch;
			}
		}
	}

	hr = pSurface->UnlockRect();
	if (FAILED(hr))
	{
		//MessageBoxA(NULL,"Failed to unlock picture surface.","",MB_OK);
		sprintf_s(szMsg, "D3DError - Failed to unlock picture surface!!\n");
		OutputDebugStringA(szMsg);
		return 0;
	}

	return 1;
}

int DisplayManager::RenderFrame_OS(int iIndex, IDirect3DSurface9* pOffscreenSurface, IDirect3DSurface9* pSurface, unsigned char* pBuffer, int iWidth, int iHeight, RECT rDest)
{
	int iRet;

	if (!pSurface)
		return 0;

	iRet = UpdateSurfaceData(pSurface, pBuffer, iWidth, iHeight);

	if (iRet == 0)
		return 0;

	return RenderFrame1_OS(iIndex, pOffscreenSurface, pSurface, iWidth, iHeight, rDest);
}

int DisplayManager::RenderFrame_OS(int iIndex, IDirect3DSurface9* pOffscreenSurface, IDirect3DSurface9* pSurface, unsigned char* pBuffer, int iWidth, int iHeight, RECT rSrc, RECT rDest)
{
	int iRet;

	if (!pSurface)
		return 0;

	iRet = UpdateSurfaceData(pSurface, pBuffer, iWidth, iHeight);

	if (iRet == 0)
		return 0;

	return RenderFrame1_OS(iIndex, pOffscreenSurface, pSurface, rSrc, rDest);
}

int DisplayManager::RenderFrame1_OS(int iIndex, IDirect3DSurface9* pOffscreenSurface, IDirect3DSurface9* pSurface, int iWidth, int iHeight, RECT rDest)
{
	int iRet;

	//if (!m_bRenderFrame1Finished)
		//return 0;

	//m_bRenderFrame1Finished = false;

	//	if(iRet == 0)
	//	return 0;

	iRet = DrawSurface_OS(pOffscreenSurface, pSurface, iWidth, iHeight, rDest);

	//if (iRet == 0)
	//{
		//m_bRenderFrame1Finished = true;
		//return 0;
//	}

	//m_bRenderFrame1Finished = true;
	return iRet;
}

int DisplayManager::RenderFrame1_OS(int iIndex, IDirect3DSurface9* pOffscreenSurface, IDirect3DSurface9* pSurface, RECT rSrc, RECT rDest)
{
	int iRet;
	iRet = DrawSurface_OS(pOffscreenSurface, pSurface, rSrc, rDest);
	return iRet;
}

int DisplayManager::DrawSurface_OS(IDirect3DSurface9* pOffscreenSurface, IDirect3DSurface9* pSurface, int iWidth, int iHeight, RECT rDest)
{
	RECT rSrc, rDest1;
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	D3DSURFACE_DESC d3d_surface_desc;

	if (!pSurface)
		return 0;

	pSurface->GetDesc(&d3d_surface_desc);

	D3DSURFACE_DESC d3d_offscreen_surface_desc;
	pOffscreenSurface->GetDesc(&d3d_offscreen_surface_desc);

	rSrc.left = 0;
	rSrc.top = 0;
	
	if (d3d_surface_desc.Width < iWidth - m_iOffset)
		rSrc.right = d3d_surface_desc.Width;
	else
		rSrc.right = iWidth - m_iOffset;

	if(d3d_surface_desc.Height < iHeight)
		rSrc.bottom = d3d_surface_desc.Height;
	else
		rSrc.bottom = iHeight;

	d3d_surface_desc.Width;
	d3d_surface_desc.Height;

	rDest1.left = 0;
	rDest1.top = 0;
	
	if (d3d_offscreen_surface_desc.Width < rDest.right - rDest.left)
		rDest1.right = d3d_offscreen_surface_desc.Width;
	else
		rDest1.right = rDest.right - rDest.left;

	if (d3d_offscreen_surface_desc.Height < rDest.bottom - rDest.top)
		rDest1.bottom = d3d_offscreen_surface_desc.Height;
	else
		rDest1.bottom = rDest.bottom - rDest.top;

	//	pD3DDevice->SetRenderTarget(0, pOffscreenSurface);
	HRESULT hr = pD3DDevice->StretchRect(pSurface, &rSrc, pOffscreenSurface,
		&rDest1, D3DTEXF_LINEAR);//D3DTEXF_LINEAR , D3DTEXF_NONE
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "StretchRect() failed.", "", MB_OK| MB_TOPMOST);
		return 0;
	}

#if (_ENABLE_LINE_MARK && _ENABLE_VIDEO_WALL)
	if (m_iMarkMode == MM_LINE)
	{
		IDirect3DSurface9* pOldRenderTargetSurface;
		pD3DDevice->GetRenderTarget(0, &pOldRenderTargetSurface);
		pD3DDevice->SetRenderTarget(0, pOffscreenSurface);
		DrawLineMark(d3d_offscreen_surface_desc.Width, d3d_offscreen_surface_desc.Height);
		pD3DDevice->SetRenderTarget(0, pOldRenderTargetSurface);
	}
	else if (m_iMarkMode == MM_COLOR_BAR)
	{
		IDirect3DSurface9* pOldRenderTargetSurface;
		pD3DDevice->GetRenderTarget(0, &pOldRenderTargetSurface);
		pD3DDevice->SetRenderTarget(0, pOffscreenSurface);
		DrawColorBarMark(d3d_offscreen_surface_desc.Width, d3d_offscreen_surface_desc.Height);
		pD3DDevice->SetRenderTarget(0, pOldRenderTargetSurface);
	}
#endif

	//pBackBuffer->Release();
//	m_iFrameCount++;

	return 1;
}

int DisplayManager::DrawSurface_OS(IDirect3DSurface9* pOffscreenSurface, IDirect3DSurface9* pSurface, RECT rSrc, RECT rDest)
{
	RECT rSrc1, rDest1;
	//RECT rDest1;
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	D3DSURFACE_DESC d3d_surface_desc;

	if (!pSurface)
		return 0;

	pSurface->GetDesc(&d3d_surface_desc);

	D3DSURFACE_DESC d3d_offscreen_surface_desc;
	pOffscreenSurface->GetDesc(&d3d_offscreen_surface_desc);

	//rSrc1.left = rSrc.left;
	//rSrc1.top = rSrc.top;
	/*
	if (d3d_surface_desc.Width < iWidth - m_iOffset)
		rSrc.right = d3d_surface_desc.Width;
	else
		rSrc.right = iWidth - m_iOffset;

	if (d3d_surface_desc.Height < iHeight)
		rSrc.bottom = d3d_surface_desc.Height;
	else
		rSrc.bottom = iHeight;
		*/
	/*
	int iTmpNum = rSrc.right % 16;

	rSrc1.left = rSrc.left;
	rSrc1.top = rSrc.top;
	rSrc1.right = rSrc.right - iTmpNum;//iWidth - m_iOffset;
	rSrc1.bottom = rSrc.bottom;//iHeight;
	*/
	d3d_surface_desc.Width;
	d3d_surface_desc.Height;

	rDest1.left = 0;
	rDest1.top = 0;

	if (d3d_offscreen_surface_desc.Width < rDest.right - rDest.left)
		rDest1.right = d3d_offscreen_surface_desc.Width;
	else
		rDest1.right = rDest.right - rDest.left;

	if (d3d_offscreen_surface_desc.Height < rDest.bottom - rDest.top)
		rDest1.bottom = d3d_offscreen_surface_desc.Height;
	else
		rDest1.bottom = rDest.bottom - rDest.top;

	//	pD3DDevice->SetRenderTarget(0, pOffscreenSurface);
	HRESULT hr = pD3DDevice->StretchRect(pSurface, &rSrc, pOffscreenSurface,
		&rDest1, D3DTEXF_NONE);//D3DTEXF_LINEAR , D3DTEXF_NONE

	if (FAILED(hr))
	{
		MessageBoxA(NULL, "StretchRect() failed.", "", MB_OK| MB_TOPMOST);
		return 0;
	}
//	m_iFrameCount++;
	//pBackBuffer->Release();

#if (_ENABLE_LINE_MARK && _ENABLE_VIDEO_WALL)
	if (m_iMarkMode == MM_LINE)
	{
		IDirect3DSurface9* pOldRenderTargetSurface;
		pD3DDevice->GetRenderTarget(0, &pOldRenderTargetSurface);
		pD3DDevice->SetRenderTarget(0, pOffscreenSurface);
		DrawLineMark(d3d_offscreen_surface_desc.Width, d3d_offscreen_surface_desc.Height);
		pD3DDevice->SetRenderTarget(0, pOldRenderTargetSurface);
	}
	else if (m_iMarkMode == MM_COLOR_BAR)
	{
		IDirect3DSurface9* pOldRenderTargetSurface;
		pD3DDevice->GetRenderTarget(0, &pOldRenderTargetSurface);
		pD3DDevice->SetRenderTarget(0, pOffscreenSurface);
		DrawColorBarMark(d3d_offscreen_surface_desc.Width, d3d_offscreen_surface_desc.Height);
		pD3DDevice->SetRenderTarget(0, pOldRenderTargetSurface);
	}
#endif
	return 1;
}


int DisplayManager::RenderFrame(int iIndex, int iPixelFormat, unsigned char* pBuffer, int iWidth, int iHeight)
{
	char szMsg[512];
	if (m_hWnd && IsWindowVisible(m_hWnd))
	{
	DWORD dwTime1 = timeGetTime();

	EnterCriticalSection(&m_CriticalSection);

	if (iIndex < m_pD3DSurfaceList->GetTotal())
	{
		D3DSurfaceItem* pCurD3DSurfaceItem = GetSurface(iIndex);

		if (!pCurD3DSurfaceItem)
		{
			LeaveCriticalSection(&m_CriticalSection);
			return -2;
		}

		if (pCurD3DSurfaceItem->iWidth != iWidth || pCurD3DSurfaceItem->iHeight != iHeight || pCurD3DSurfaceItem->iPixelFormat != iPixelFormat)
		{
			if (pCurD3DSurfaceItem->pD3DSurface)
			{
				DestroyOverlayResource(pCurD3DSurfaceItem->pOverlaySurface, pCurD3DSurfaceItem->pOverlayTexture);
				pCurD3DSurfaceItem->pOverlaySurface = 0;
				pCurD3DSurfaceItem->pOverlayTexture = 0;
				DeleteSurface(pCurD3DSurfaceItem->pD3DSurface);
				pCurD3DSurfaceItem->pD3DSurface = 0;

				pCurD3DSurfaceItem->iHeight = 0;
				pCurD3DSurfaceItem->iWidth = 0;
			}
		}

		if (pCurD3DSurfaceItem->pOverlaySurface == 0)
		{
			//RECT* pRect = m_pLayoutCtrl->GetGridRect(iIndex);

#if 0
			int h = pRect->bottom - pRect->top;
			int w = pRect->right - pRect->left;
#else
			int h = iHeight;
			int w = iWidth;
#endif
			pCurD3DSurfaceItem->pOverlayTexture = CreateOverlayTexture(w, h);
			pCurD3DSurfaceItem->pOverlayTexture->GetSurfaceLevel(0, &pCurD3DSurfaceItem->pOverlaySurface);
		}

		if (pCurD3DSurfaceItem->iWidth != iWidth || pCurD3DSurfaceItem->iHeight != iHeight || pCurD3DSurfaceItem->iPixelFormat != iPixelFormat)
		{
			pCurD3DSurfaceItem->iWidth = iWidth;
			pCurD3DSurfaceItem->iHeight = iHeight;
			pCurD3DSurfaceItem->iPixelFormat = iPixelFormat;
			pCurD3DSurfaceItem->bI420ToYV12 = false;
			if (pCurD3DSurfaceItem->pD3DSurface)
			{
				DeleteSurface(pCurD3DSurfaceItem->pD3DSurface);
				pCurD3DSurfaceItem->pD3DSurface = 0;
			}

			bool bSupported = FormatSupported((D3DFORMAT)GetD3DFormat(iPixelFormat));
			if (bSupported)
			{
				pCurD3DSurfaceItem->pD3DSurface = CreateSurfaces(iWidth, iHeight, (D3DFORMAT)GetD3DFormat(iPixelFormat));
				if (pCurD3DSurfaceItem->pD3DSurface == 0)
				{
					//change to YV12
					bSupported = FormatSupported((D3DFORMAT)GetD3DFormat(VPF_YV12));
					if (bSupported)
					{
						pCurD3DSurfaceItem->pD3DSurface = CreateSurfaces(iWidth, iHeight, (D3DFORMAT)GetD3DFormat(VPF_YV12));
						pCurD3DSurfaceItem->bI420ToYV12 = true;
					}
				}
			}
			else
			{
				//change to YV12
				bSupported = FormatSupported((D3DFORMAT)GetD3DFormat(VPF_YV12));
				if (bSupported)
				{
					pCurD3DSurfaceItem->pD3DSurface = CreateSurfaces(iWidth, iHeight, (D3DFORMAT)GetD3DFormat(VPF_YV12));
					pCurD3DSurfaceItem->bI420ToYV12 = true;
				}
			}
		}

		//RECT* pRect = m_pLayoutCtrl->GetGridRect(iIndex);
		RECT rect = { 0,0,iWidth, iHeight };

		if (iPixelFormat == VPF_NV12)
		{
			int iRet = RenderFrame1_OS(iIndex, pCurD3DSurfaceItem->pOverlaySurface, (IDirect3DSurface9 *)pBuffer, iWidth, iHeight, rect);
			m_FPSCounter[iIndex].CalFPS();
			LeaveCriticalSection(&m_CriticalSection);
			m_bUpdate[iIndex] = true;
			return iRet;
		}
		else
		{
			if (pCurD3DSurfaceItem->bI420ToYV12 && pCurD3DSurfaceItem->iPixelFormat == VPF_I420)
				EnableI420ToYV12(pCurD3DSurfaceItem->bI420ToYV12);
			else
				EnableI420ToYV12(false);

			int iRet = RenderFrame_OS(iIndex, pCurD3DSurfaceItem->pOverlaySurface, pCurD3DSurfaceItem->pD3DSurface, pBuffer, iWidth, iHeight, rect);
			m_FPSCounter[iIndex].CalFPS();
			LeaveCriticalSection(&m_CriticalSection);
			DWORD dwTime2 = timeGetTime();

			sprintf(szMsg, "DisplayManager::RenderFrame() [%d]\n", dwTime2 - dwTime1);
			//OutputDebugStringA(szMsg);
			m_bUpdate[iIndex] = true;
			return iRet;
		}

	}
	LeaveCriticalSection(&m_CriticalSection);
	}
	return -1;
}



int DisplayManager::RenderSurface(int iIndex, int iPixelFormat, unsigned char* pBuffer, int iWidth, int iHeight)
{
	char szMsg[512];

	if (m_hWnd && IsWindowVisible(m_hWnd))
	{
	DWORD dwTime1 = timeGetTime();

	EnterCriticalSection(&m_CriticalSection);

	if (iIndex < m_pD3DSurfaceList->GetTotal())
	{
		D3DSurfaceItem* pCurD3DSurfaceItem = GetSurface(iIndex);

		if (!pCurD3DSurfaceItem)
		{
			LeaveCriticalSection(&m_CriticalSection);
			return -2;
		}

		if (pCurD3DSurfaceItem->pOverlaySurface == 0)
		{
			//RECT* pRect = m_pLayoutCtrl->GetGridRect(iIndex);

#if 0
			int h = pRect->bottom - pRect->top;
			int w = pRect->right - pRect->left;
#else
			int h = iHeight;
			int w = iWidth;
#endif
			pCurD3DSurfaceItem->pOverlayTexture = CreateOverlayTexture(w, h);
			pCurD3DSurfaceItem->pOverlayTexture->GetSurfaceLevel(0, &pCurD3DSurfaceItem->pOverlaySurface);
		}

		if (pCurD3DSurfaceItem->iWidth != iWidth || pCurD3DSurfaceItem->iHeight != iHeight || pCurD3DSurfaceItem->iPixelFormat != iPixelFormat)
		{
			pCurD3DSurfaceItem->iWidth = iWidth;
			pCurD3DSurfaceItem->iHeight = iHeight;
			pCurD3DSurfaceItem->iPixelFormat = iPixelFormat;
			pCurD3DSurfaceItem->bI420ToYV12 = false;
			if (pCurD3DSurfaceItem->pD3DSurface)
			{
				DeleteSurface(pCurD3DSurfaceItem->pD3DSurface);
				pCurD3DSurfaceItem->pD3DSurface = 0;
			}

			bool bSupported = FormatSupported((D3DFORMAT)GetD3DFormat(iPixelFormat));
			if (bSupported)
				pCurD3DSurfaceItem->pD3DSurface = CreateSurfaces(iWidth, iHeight, (D3DFORMAT)GetD3DFormat(iPixelFormat));
			else
			{
				//change to YV12
				bSupported = FormatSupported((D3DFORMAT)GetD3DFormat(VPF_YV12));
				if (bSupported)
				{
					pCurD3DSurfaceItem->pD3DSurface = CreateSurfaces(iWidth, iHeight, (D3DFORMAT)GetD3DFormat(VPF_YV12));
					pCurD3DSurfaceItem->bI420ToYV12 = true;
				}
			}
		}

		//RECT* pRect = m_pLayoutCtrl->GetGridRect(iIndex);
		RECT rect = { 0,0,iWidth, iHeight };
		pCurD3DSurfaceItem->pD3DSurface = (IDirect3DSurface9 *)pBuffer;
		int iRet = RenderFrame1_OS(iIndex, pCurD3DSurfaceItem->pOverlaySurface, (IDirect3DSurface9 *)pBuffer, iWidth, iHeight, rect);
		m_FPSCounter[iIndex].CalFPS();
		LeaveCriticalSection(&m_CriticalSection);

		DWORD dwTime2 = timeGetTime();

		sprintf(szMsg, "DisplayManager::RenderSurface() [%d]\n", dwTime2 - dwTime1);
		//OutputDebugStringA(szMsg);
		m_bUpdate[iIndex] = true;
		return iRet;
	}
	LeaveCriticalSection(&m_CriticalSection);
	}
	return -1;
}

int DisplayManager::RenderSurface(int iIndex, int iPixelFormat, unsigned char* pBuffer, int iWidth, int iHeight,int iPanelIndex)
{
	char szMsg[512];

	if (m_hWnd && IsWindowVisible(m_hWnd))
	{
		DWORD dwTime1 = timeGetTime();

		EnterCriticalSection(&m_CriticalSection);

		if (iIndex < m_pD3DSurfaceList->GetTotal())
		{
			D3DSurfaceItem* pCurD3DSurfaceItem = GetSurface(iIndex);

			if (!pCurD3DSurfaceItem)
			{
				LeaveCriticalSection(&m_CriticalSection);
				return -2;
			}

			if (pCurD3DSurfaceItem->pOverlaySurface == 0)
			{
				//RECT* pRect = m_pLayoutCtrl->GetGridRect(iIndex);

#if 0
				int h = pRect->bottom - pRect->top;
				int w = pRect->right - pRect->left;
#else
				//int h = iHeight/2;
				//int w = iWidth/2;
				int h = iHeight;
				int w = iWidth;
#endif
				pCurD3DSurfaceItem->pOverlayTexture = CreateOverlayTexture(w, h);
				pCurD3DSurfaceItem->pOverlayTexture->GetSurfaceLevel(0, &pCurD3DSurfaceItem->pOverlaySurface);
			}

			if (pCurD3DSurfaceItem->iWidth != iWidth || pCurD3DSurfaceItem->iHeight != iHeight || pCurD3DSurfaceItem->iPixelFormat != iPixelFormat)
			{
				pCurD3DSurfaceItem->iWidth = iWidth;
				pCurD3DSurfaceItem->iHeight = iHeight;
				pCurD3DSurfaceItem->iPixelFormat = iPixelFormat;
				pCurD3DSurfaceItem->bI420ToYV12 = false;
				if (pCurD3DSurfaceItem->pD3DSurface)
				{
					DeleteSurface(pCurD3DSurfaceItem->pD3DSurface);
					pCurD3DSurfaceItem->pD3DSurface = 0;
				}

				bool bSupported = FormatSupported((D3DFORMAT)GetD3DFormat(iPixelFormat));
				if (bSupported)
					pCurD3DSurfaceItem->pD3DSurface = CreateSurfaces(iWidth, iHeight, (D3DFORMAT)GetD3DFormat(iPixelFormat));
				else
				{
					//change to YV12
					bSupported = FormatSupported((D3DFORMAT)GetD3DFormat(VPF_YV12));
					if (bSupported)
					{
						pCurD3DSurfaceItem->pD3DSurface = CreateSurfaces(iWidth, iHeight, (D3DFORMAT)GetD3DFormat(VPF_YV12));
						pCurD3DSurfaceItem->bI420ToYV12 = true;
					}
				}
			}

			//RECT* pRect = m_pLayoutCtrl->GetGridRect(iIndex);
			//RECT rect = { 0,0,iWidth/2, iHeight/2};
			RECT rect = { 0,0,iWidth, iHeight};


			RECT rSrc;
			GetGridRect(iPanelIndex, &rSrc, 2, 2, iWidth, iHeight);
//			int iRet = RenderFrame1_OS(iIndex, pCurD3DSurfaceItem->pOverlaySurface, (IDirect3DSurface9 *)pBuffer, rSrc, rect);

			int iRet = RenderFrame1_OS(iIndex, pCurD3DSurfaceItem->pOverlaySurface, (IDirect3DSurface9 *)pBuffer, iWidth, iHeight, rect);

			m_FPSCounter[iIndex].CalFPS();

			LeaveCriticalSection(&m_CriticalSection);

			DWORD dwTime2 = timeGetTime();

			sprintf(szMsg, "DisplayManager::RenderSurface() [%d]\n", dwTime2 - dwTime1);
			//OutputDebugStringA(szMsg);
			m_bUpdate[iIndex] = true;
			return iRet;
		}
		LeaveCriticalSection(&m_CriticalSection);
	}
	return -1;
}

void DoMultiple(int& Value, int iMultiple)
{
	if (Value % iMultiple != 0)
	{
		int iRemainder = Value % iMultiple;
		Value += iMultiple - iRemainder;
	}
}

int DisplayManager::RenderFrame(int iIndex, int iPixelFormat, unsigned char* pBuffer, int iWidth, int iHeight, int iPanelIndex)
{
	char szMsg[512];
	if (m_hWnd && IsWindowVisible(m_hWnd))
	{
		DWORD dwTime1 = timeGetTime();

		EnterCriticalSection(&m_CriticalSection);

		if (iIndex < m_pD3DSurfaceList->GetTotal())
		{
			D3DSurfaceItem* pCurD3DSurfaceItem = GetSurface(iIndex);

			if (!pCurD3DSurfaceItem)
			{
				LeaveCriticalSection(&m_CriticalSection);
				return -2;
			}

			if (pCurD3DSurfaceItem->iWidth != iWidth || pCurD3DSurfaceItem->iHeight != iHeight || pCurD3DSurfaceItem->iPixelFormat != iPixelFormat)
			{
				if (pCurD3DSurfaceItem->pD3DSurface)
				{
					DestroyOverlayResource(pCurD3DSurfaceItem->pOverlaySurface, pCurD3DSurfaceItem->pOverlayTexture);
					pCurD3DSurfaceItem->pOverlaySurface = 0;
					pCurD3DSurfaceItem->pOverlayTexture = 0;
					DeleteSurface(pCurD3DSurfaceItem->pD3DSurface);
					pCurD3DSurfaceItem->pD3DSurface = 0;

					pCurD3DSurfaceItem->iHeight = 0;
					pCurD3DSurfaceItem->iWidth = 0;
				}
			}

			float fHFactor = (float)1920.0 / (float)iWidth;
			float fVFactor = (float)1080.0 / (float)iHeight;

			if (pCurD3DSurfaceItem->pOverlaySurface == 0)
			{
				//RECT* pRect = m_pLayoutCtrl->GetGridRect(iIndex);

#if 0
				int h = pRect->bottom - pRect->top;
				int w = pRect->right - pRect->left;
#else
				int h = iHeight;
				int w = iWidth;
#endif
				pCurD3DSurfaceItem->pOverlayTexture = CreateOverlayTexture(w, h);
				pCurD3DSurfaceItem->pOverlayTexture->GetSurfaceLevel(0, &pCurD3DSurfaceItem->pOverlaySurface);
			}

			if (pCurD3DSurfaceItem->iWidth != iWidth || pCurD3DSurfaceItem->iHeight != iHeight || pCurD3DSurfaceItem->iPixelFormat != iPixelFormat)
			{
				pCurD3DSurfaceItem->iWidth = iWidth;
				pCurD3DSurfaceItem->iHeight = iHeight;
				pCurD3DSurfaceItem->iPixelFormat = iPixelFormat;
				pCurD3DSurfaceItem->bI420ToYV12 = false;
				if (pCurD3DSurfaceItem->pD3DSurface)
				{
					DeleteSurface(pCurD3DSurfaceItem->pD3DSurface);
					pCurD3DSurfaceItem->pD3DSurface = 0;
				}

				bool bSupported = FormatSupported((D3DFORMAT)GetD3DFormat(iPixelFormat));
				if (bSupported)
				{
					pCurD3DSurfaceItem->pD3DSurface = CreateSurfaces(iWidth, iHeight, (D3DFORMAT)GetD3DFormat(iPixelFormat));
					if (pCurD3DSurfaceItem->pD3DSurface == 0)
					{
						//change to YV12
						bSupported = FormatSupported((D3DFORMAT)GetD3DFormat(VPF_YV12));
						if (bSupported)
						{
							pCurD3DSurfaceItem->pD3DSurface = CreateSurfaces(iWidth, iHeight, (D3DFORMAT)GetD3DFormat(VPF_YV12));
							pCurD3DSurfaceItem->bI420ToYV12 = true;
						}
					}
				}
				else
				{
					//change to YV12
					bSupported = FormatSupported((D3DFORMAT)GetD3DFormat(VPF_YV12));
					if (bSupported)
					{
						pCurD3DSurfaceItem->pD3DSurface = CreateSurfaces(iWidth, iHeight, (D3DFORMAT)GetD3DFormat(VPF_YV12));
						pCurD3DSurfaceItem->bI420ToYV12 = true;
					}
				}
			}

			//RECT* pRect = m_pLayoutCtrl->GetGridRect(iIndex);
			RECT rect = { 0,0,iWidth, iHeight};
			//RECT rect = { 0,0,1920, 1080 };

			if (iPixelFormat == VPF_NV12)
			{
				int iRet = RenderFrame1_OS(iIndex, pCurD3DSurfaceItem->pOverlaySurface, (IDirect3DSurface9 *)pBuffer, iWidth, iHeight, rect);
				m_FPSCounter[iIndex].CalFPS();
				LeaveCriticalSection(&m_CriticalSection);
				return iRet;
			}
			else
			{
				if (pCurD3DSurfaceItem->bI420ToYV12 && pCurD3DSurfaceItem->iPixelFormat == VPF_I420)
					EnableI420ToYV12(pCurD3DSurfaceItem->bI420ToYV12);
				else
					EnableI420ToYV12(false);

				RECT rSrc, rSrc1;
				GetGridRect(iPanelIndex, &rSrc, 2, 2, iWidth, iHeight);

				if (iWidth > 1920 || iHeight > 1080)
				{
					rSrc1.left = rSrc.left * fHFactor;
					rSrc1.top = rSrc.top * fVFactor + 0.1;
					rSrc1.right = rSrc.right * fHFactor;
					rSrc1.bottom = rSrc.bottom * fVFactor + 0.1;
				}
				else
				{
					rSrc1.left = rSrc.left;
					rSrc1.top = rSrc.top;
					rSrc1.right = rSrc.right;
					rSrc1.bottom = rSrc.bottom;
				}

				DoMultiple((int&)rSrc1.left, 4);
				DoMultiple((int&)rSrc1.top, 4);
				DoMultiple((int&)rSrc1.right, 4);
				DoMultiple((int&)rSrc1.bottom, 4);
				
				int iRet = RenderFrame_OS(iIndex, pCurD3DSurfaceItem->pOverlaySurface, pCurD3DSurfaceItem->pD3DSurface, pBuffer, iWidth, iHeight, rSrc1, rect);
				m_FPSCounter[iIndex].CalFPS();
				LeaveCriticalSection(&m_CriticalSection);
				DWORD dwTime2 = timeGetTime();

				sprintf(szMsg, "DisplayManager::RenderFrame() [%d]\n", dwTime2 - dwTime1);
				//OutputDebugStringA(szMsg);
				m_bUpdate[iIndex] = true;
				return iRet;
			}

		}
		LeaveCriticalSection(&m_CriticalSection);
	}
	return -1;
}

int DisplayManager::Merge()
{
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	RECT rDest, rSrc;

	IDirect3DSurface9 *pBackBuffer;
	if (pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer) != D3D_OK)
	{
		//error
		return 0;
	}

/*
	if (m_iClearCount > 0)
	{
		m_iClearCount--;
		m_pD3DRender->Clear();
	}
	*/

#ifdef _ENABLE_SINGLE_EFFECT

#ifdef _ENABLE_GPU
#else   //_ENABLE_GPU
	m_pOffscreenTexture->GetSurfaceLevel(0, &m_pOffscreenSurface);
#endif  //_ENABLE_GPU

#endif

	//	DrawLayout();
	D3DSURFACE_DESC d3d_surface_desc;
	for (int i = 0; i < m_pD3DSurfaceList->GetTotal(); i++)
	{
		D3DSurfaceItem* pCurD3DSurfaceItem = GetSurface(i);
		if (pCurD3DSurfaceItem && pCurD3DSurfaceItem->pD3DSurface && pCurD3DSurfaceItem->iWidth > 0 && pCurD3DSurfaceItem->iHeight && pCurD3DSurfaceItem->pOverlaySurface)
		{
			RECT* pRect = &m_RectList[i];// m_pLayoutCtrl->GetGridRect(i);

			rSrc.left = 0;
			rSrc.top = 0;

			pCurD3DSurfaceItem->pOverlaySurface->GetDesc(&d3d_surface_desc);

#if 0
			rSrc.right = pRect->right - pRect->left;//pCurD3DSurfaceItem->iWidth;
			rSrc.bottom = pRect->bottom - pRect->top;//pCurD3DSurfaceItem->iHeight;
#else
			if(d3d_surface_desc.Width < pCurD3DSurfaceItem->iWidth)
				rSrc.right = d3d_surface_desc.Width;
			else
				rSrc.right = pCurD3DSurfaceItem->iWidth;//pCurD3DSurfaceItem->iWidth;
			if (d3d_surface_desc.Height < pCurD3DSurfaceItem->iHeight)
				rSrc.bottom = d3d_surface_desc.Height;
			else
				rSrc.bottom = pCurD3DSurfaceItem->iHeight;//pCurD3DSurfaceItem->iHeight;
#endif

			rDest.left = pRect->left;
			rDest.top = pRect->top;
			rDest.right = pRect->right;
			rDest.bottom = pRect->bottom;

			//m_pD3DRender->DrawAllOSDs_OS(pCurD3DSurfaceItem->pOverlaySurface);

			if (pCurD3DSurfaceItem->iWidth > 0 && pCurD3DSurfaceItem->iHeight && pCurD3DSurfaceItem->pOverlaySurface)
			{
				//HRESULT hr;
				

#ifdef _ENABLE_SINGLE_EFFECT

#ifdef _ENABLE_GPU
				HRESULT hr;
				sEffectParam effect_param = m_pMixerEffect->GetEffectParam();
				if (m_pMixerEffect->GetEffectID() != 0 && effect_param.iLevel != 0 && m_bDoEffect && m_dCurPos[i] > 1.3 && m_bUpdate[i])
				{
					
#ifdef _ENABLE_HD_EFFECT

					hr = pD3DDevice->StretchRect(pCurD3DSurfaceItem->pOverlaySurface, &rSrc, pBackBuffer, &rDest, D3DTEXF_NONE); //D3DTEXF_LINEAR
					/*
					IDirect3DSurface9* pHDOutSurface = DoHDEffect(i,pCurD3DSurfaceItem->pD3DSurface, pCurD3DSurfaceItem->iWidth, pCurD3DSurfaceItem->iHeight);
					if (pHDOutSurface)
					{
						m_pD3DDeviceEx->SetRenderTarget(0, pBackBuffer);

						hr = m_pD3DDeviceEx->StretchRect(pHDOutSurface, &rSrc, pBackBuffer,
							&rDest, D3DTEXF_NONE); //D3DTEXF_LINEAR
					}
					else
					{
						hr = pD3DDevice->StretchRect(pCurD3DSurfaceItem->pOverlaySurface, &rSrc, pBackBuffer,
							&rDest, D3DTEXF_NONE); //D3DTEXF_LINEAR
					}
					*/
#else
					if (m_pMixerEffect->GetWidth() != m_iWidth || m_pMixerEffect->GetHeight() != m_iHeight)
						m_pMixerEffect->ResetSurfaces(m_iWidth, m_iHeight);
					
					IDirect3DSurface9* pOutSurface = m_pMixerEffect->DoVideoEffect_Surface(pHDOutSurface, pCurD3DSurfaceItem->iWidth, pCurD3DSurfaceItem->iHeight);

					m_pD3DDeviceEx->SetRenderTarget(0, pBackBuffer);

					hr = m_pD3DDeviceEx->StretchRect(pOutSurface, &rSrc, pBackBuffer,&rDest, D3DTEXF_NONE); //D3DTEXF_LINEAR
#endif
					
				}
				else
				{
					hr = pD3DDevice->StretchRect(pCurD3DSurfaceItem->pOverlaySurface, &rSrc, pBackBuffer,&rDest, D3DTEXF_NONE); //D3DTEXF_LINEAR
				}

#else   //_ENABLE_GPU   
				HRESULT hr = pD3DDevice->StretchRect(pCurD3DSurfaceItem->pOverlaySurface, &rSrc, m_pOffscreenSurface,
					&rDest, D3DTEXF_NONE); //D3DTEXF_LINEAR
#endif  //_ENABLE_GPU

#else
				HRESULT hr = pD3DDevice->StretchRect(pCurD3DSurfaceItem->pOverlaySurface, &rSrc, pBackBuffer,
					&rDest, D3DTEXF_NONE); //D3DTEXF_LINEAR
#endif
				if (FAILED(hr))
				{
					//	MessageBoxA(NULL,"StretchRect() failed.","",MB_OK);
					return 0;
				}
			}
			m_bUpdate[i] = false;
		}
	}

#ifdef _ENABLE_SINGLE_EFFECT
	
	sEffectParam effect_param = m_pMixerEffect->GetEffectParam();
	if (m_pMixerEffect->GetEffectID() != 0 && effect_param.iLevel != 0 && m_bDoEffect)
	{

#ifdef _ENABLE_GPU

#else
		//HRESULT hr = pD3DDevice->GetRenderTargetData(m_pOffscreenSurface, m_pRawDataSurface);
		//D3DSurface2RawData(m_pRawDataSurface, m_iWidth, m_iHeight, m_pRawDataBuffer);

		if (m_pMixerEffect->GetWidth() != m_iWidth || m_pMixerEffect->GetHeight() != m_iHeight)
			m_pMixerEffect->ResetSurfaces(m_iWidth, m_iHeight);

		if (m_pMixerEffect->GetEffectID() >= HD_EID_BRIGHTNESS)
			HRESULT hr = pD3DDevice->StretchRect(m_pOffscreenSurface, NULL, pBackBuffer, NULL, D3DTEXF_NONE);
		else
		{
			IDirect3DSurface9* pOutSurface = m_pMixerEffect->DoVideoEffect_Surface(m_pOffscreenSurface, m_iWidth, m_iHeight);
			//IDirect3DSurface9* pOutSurface = m_pMixerEffect->DoVideoEffect2(m_pRawDataBuffer, m_iWidth, m_iHeight, PF_RGB32);
			if (pOutSurface)
				HRESULT hr = pD3DDevice->StretchRect(pOutSurface, NULL, pBackBuffer, NULL, D3DTEXF_NONE); //D3DTEXF_LINEAR
		}
#endif
	}
	else
	{
		
#ifdef _ENABLE_GPU

#else  
		HRESULT hr = pD3DDevice->StretchRect(m_pOffscreenSurface, NULL, pBackBuffer, NULL, D3DTEXF_NONE); //D3DTEXF_LINEAR
#endif

	}
#else
#endif

	pBackBuffer->Release();

#ifdef _ENABLE_SINGLE_EFFECT
	if (m_pOffscreenSurface)
	{
		m_pOffscreenSurface->Release();
		m_pOffscreenSurface = 0;
	}
#endif

	return 1;
}

void DisplayManager::Flip()
{

	if (m_bIsSuspend)
		return;

	RECT rect = { 0, 0, m_iWidth, m_iHeight };

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	Merge();

	if (!pD3DDevice)
		return;

	HRESULT hr;

	if (m_bIsD3D9Ex)
	{
		/*
#ifdef _ENABLE_GPU
		sEffectParam effect_param = m_pMixerEffect->GetEffectParam();
		if (m_pMixerEffect->GetEffectID() != 0 && effect_param.iLevel != 0 && m_bDoEffect)
		{
			m_pProcessorHD->Flip();
		}
		else
		{
			hr = ((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(0, 0, 0, 0, 0);
			if (FAILED(hr))
			{
				CheckDeviceState();
				return;
	}
			else
				m_bIsDeivceLost = false;
		}
#else   */
		DWORD dwTime1 = timeGetTime();
		hr = ((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(0, 0, 0, 0, 0);
		DWORD dwTime2 = timeGetTime();

//		char szMsg[512];
//		sprintf(szMsg, "#FPO# DisplayManager::Flip: [%d] \n", dwTime2 - dwTime1);
//		OutputDebugStringA(szMsg);

		if (FAILED(hr))
		{
			CheckDeviceState();
			return;
		}
		else
			m_bIsDeivceLost = false;
//#endif
	}
	else
	{
		hr = pD3DDevice->Present(&rect, 0, 0, 0);
		if (FAILED(hr))
		{
			CheckDeviceState();
			return;
		}
		else
			m_bIsDeivceLost = false;
	}
	//hr = ((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(0, 0, 0, 0, 0);
}

bool DisplayManager::IsAlive()
{
	return m_bIsAlive;
}

int DisplayManager::D3DSurface2RawData(IDirect3DSurface9* pInSurface, int iWidth, int iHeight, unsigned char* pOutBuffer)
{
	char szMsg[512];
	D3DLOCKED_RECT d3drect;
	HRESULT hr = pInSurface->LockRect(&d3drect, NULL, 0);

	if (FAILED(hr))
	{
		//MessageBoxA(NULL,"Failed to lock picture surface.","",MB_OK);
		sprintf(szMsg, "D3DError - Failed to lock picture surface!!\n");
		OutputDebugStringA(szMsg);
		return 0;
	}

	int i;
	unsigned char *dst = pOutBuffer;
	unsigned char *src = (unsigned char*)d3drect.pBits;

	memcpy(dst, src, iWidth * iHeight * 4);

	hr = pInSurface->UnlockRect();
	if (FAILED(hr))
	{
		//MessageBoxA(NULL,"Failed to unlock picture surface.","",MB_OK);
		sprintf(szMsg, "D3DError - Failed to unlock picture surface!!\n");
		OutputDebugStringA(szMsg);
		return 0;
	}

	return 1;
}

void DisplayManager::CaptureRenderTarget(IDirect3DSurface9 *pRenderTarget)
{
	EnterCriticalSection(&m_CriticalSection);
	if (m_bEnableRecordAVI)
	{
		DWORD dwTime1 = timeGetTime();

		IDirect3DDevice9* pD3DDevice = GetD3DDevice();

		HRESULT hr = pD3DDevice->GetRenderTargetData(pRenderTarget, m_pRawDataSurface);
		D3DSurface2RawData(m_pRawDataSurface, m_iWidth, m_iHeight, m_pRawDataBuffer);

		m_pMFRecorderDll->WriteVideoFrame(m_pRawDataBuffer, m_iWidth * m_iHeight * 4);

		DWORD dwTime2 = timeGetTime();

		char szMsg[512];
		sprintf(szMsg, "DisplayManager::CaptureRenderTarget() - [%d]\n", dwTime2 - dwTime1);
		OutputDebugStringA(szMsg);

		/*
		char szFileName[512];
		sprintf(szFileName,"C:\\Users\\noah\\Pictures\\surface_to_bmp_%d.bmp", timeGetTime());
		SaveBmpFile(szFileName, m_pRawDataBuffer, m_iWidth, m_iHeight);
		*/
	}
	LeaveCriticalSection(&m_CriticalSection);
}

void DisplayManager::RecordAudio(unsigned char* pBuffer,int iLen)
{
	EnterCriticalSection(&m_CriticalSection);
	if (m_bEnableRecordAVI)
	{
		m_pMFRecorderDll->WriteAudioFrame(pBuffer, iLen);
	}
	LeaveCriticalSection(&m_CriticalSection);
}

void DisplayManager::EnableRecordAVI(bool bEnable,float fFPS,int iChannelLayout, int iSampleRate, int iNBSamples)
{
	EnterCriticalSection(&m_CriticalSection);
	bool bLastStatus = m_bEnableRecordAVI;
	m_bEnableRecordAVI = bEnable;

	if (bLastStatus)
		m_pMFRecorderDll->Close();

	if (m_bEnableRecordAVI)
	{
		char szFileName[512];
		char szProgramDataPath[128];
		int iRet = GetFolderPath(CSIDL_COMMON_APPDATA, szProgramDataPath);

		sprintf(szFileName, "%s\\shape_%u.avi", szProgramDataPath, timeGetTime());
		m_pMFRecorderDll->Open(szFileName);
		m_pMFRecorderDll->SetVideoInfo(m_iWidth, m_iHeight, 1024 * 1024 * 10, fFPS);
		if (iChannelLayout != 0)
		{
			m_pMFRecorderDll->SetAudioInfo(2, 48000, 192000);
			//m_pMFRecorderDll->SetAudioInfo(2, 44100, 192000);
			m_pMFRecorderDll->SetOriginalAudioInfo(iChannelLayout, iSampleRate, iNBSamples);
		}
		m_pMFRecorderDll->WriteHeader();
	}
	LeaveCriticalSection(&m_CriticalSection);
}

bool DisplayManager::IsRecordAVI()
{
	return m_bEnableRecordAVI;
}

void DisplayManager::SetPanelHWND(HWND hWnd)
{
	m_hPanelWnd = hWnd;
}

HWND DisplayManager::GetPanelHWND()
{
	return m_hPanelWnd;
}

void DisplayManager::Clear(int iIndex,int w,int h)
{
	int iBufLen = w * h * 3 / 2;
	memset(m_pBlackBuffer, 16, w * h);
	memset(m_pBlackBuffer + w * h, 128, iBufLen - w * h);

	RenderFrame(iIndex, VPF_I420, m_pBlackBuffer, w , h);
}

void DisplayManager::ClearAll()
{
	int iBufLen;
	for (int i = 0; i < 10; i++)
	{
		m_bUpdate[i] = true;
		D3DSurfaceItem* pCurD3DSurfaceItem = GetSurface(i);
		if (pCurD3DSurfaceItem)
		{
			if (pCurD3DSurfaceItem->pD3DSurface)
			{
				if (pCurD3DSurfaceItem->iHeight > 0 && pCurD3DSurfaceItem->iWidth > 0)
				{
					/*
					iBufLen = pCurD3DSurfaceItem->iWidth * pCurD3DSurfaceItem->iHeight * 3 / 2;
					memset(m_pBlackBuffer, 16, pCurD3DSurfaceItem->iWidth * 240);
					memset(m_pBlackBuffer + pCurD3DSurfaceItem->iWidth * pCurD3DSurfaceItem->iHeight, 128, iBufLen - pCurD3DSurfaceItem->iWidth * pCurD3DSurfaceItem->iHeight);

					RenderFrame(i, VPF_I420, m_pBlackBuffer, pCurD3DSurfaceItem->iWidth, pCurD3DSurfaceItem->iHeight);
					*/
					Clear(i, pCurD3DSurfaceItem->iWidth , pCurD3DSurfaceItem->iHeight);
				}
				else
					Clear(i, 1920, 1080);
			}
			else
			{
				Clear(i, 1920, 1080);
			}
		}
		else
		{
			Clear(i, 1920, 1080);
		}
	}
}

void DisplayManager::EnableGPU(bool bEnable)
{
	m_bGPUIsEnabled = bEnable;
}

void DisplayManager::Suspend()
{
	EnterCriticalSection(&m_CriticalSection);
	m_bIsSuspend = true;
//	m_iSuspendCount++;
}

void DisplayManager::Resume()
{
//	m_iSuspendCount--;
//	if(m_iSuspendCount <= 0)
		m_bIsSuspend = false;
	LeaveCriticalSection(&m_CriticalSection);
}

bool DisplayManager::IsSuspend()
{
	return m_bIsSuspend;
}

void DisplayManager::SetD3dPresentParamters(D3DPRESENT_PARAMETERS* pD3Dpp)
{
	m_pD3Dpp = pD3Dpp;
}

void DisplayManager::CheckDeviceState()
{
	HRESULT hr;

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	// Check device state
	hr = pD3DDevice->TestCooperativeLevel();

	// Device can be reset now
	//if (SUCCEEDED(hr) || hr == D3DERR_DEVICENOTRESET)
	if (hr == D3DERR_DEVICENOTRESET)
	{
		if (m_hTimeLineHwnd)
			::PostMessage(m_hTimeLineHwnd, WM_DEVICENOTRESET, m_iPanelIndex, 0);
//		m_bIsDeivceLost = false;
	}
	else if (hr == D3DERR_DEVICELOST)
		m_bIsDeivceLost = true;
}

bool DisplayManager::IsDeivceLost()
{
	return m_bIsDeivceLost;
}

void DisplayManager::TimeEvent()
{
	EnterCriticalSection(&m_CriticalSection);
	//tScopedLock MySpinLock(m_LockObj);
	DWORD dwCurTime = timeGetTime();

	if (m_dwLastTime == 0)
		m_dwLastTime = dwCurTime;

	if (dwCurTime >= m_dwLastTime + m_iPresentInterval)//40,33
	{
		m_dwLastTime = dwCurTime;

//		DWORD dwTime1 = timeGetTime();

		if(m_hWnd && IsWindowVisible(m_hWnd))
			Flip();

		m_dwLastTime = timeGetTime();
	}
	LeaveCriticalSection(&m_CriticalSection);
}

#if (_ENABLE_LINE_MARK && _ENABLE_VIDEO_WALL)
void DisplayManager::DrawLineMark(int iWidth, int iHigh)
{
	//int iWidth = 1920 - 2;
	//int iHigh = 1080 - 2;
	int i;
	POINT p1,p2;

	float fPortionW = (float)iWidth / 10.0;
	float fPortionH = (float)iHigh / 10.0;
	
	for (int j = 1; j < 10; j++)
	{
		int iXPos = j * fPortionW;
		int iYPos = j * fPortionH;		

		p1.x = iXPos;
		p1.y = 0;
		p2.x = iXPos;
		p2.y = iHigh;
		m_pD3DPen[0]->DrawLine(p1, p2);


		p1.x = 0;
		p1.y = iYPos;
		p2.x = iWidth;
		p2.y = iYPos;
		m_pD3DPen[0]->DrawLine(p1, p2);
	}

	p1.x = 1;
	p1.y = 1;
	p2.x = iWidth - 1;
	p2.y = 1;
	m_pD3DPen[0]->DrawLine(p1, p2);

	p1.x = iWidth - 1;
	p1.y = iHigh - 1;
	p2.x = iWidth - 1;
	p2.y = 1;
	m_pD3DPen[0]->DrawLine(p1, p2);

	p1.x = iWidth - 1;
	p1.y = iHigh - 1;
	p2.x = 1;
	p2.y = iHigh - 1;
	m_pD3DPen[0]->DrawLine(p1, p2);

	p1.x = 1;
	p1.y = 1;
	p2.x = 1;
	p2.y = iHigh - 1;
	m_pD3DPen[0]->DrawLine(p1, p2);

	//MoveToEx(hDC, 0, 0, NULL);
	//LineTo(hDC, iWidth, iHigh);

	p1.x = 0;
	p1.y = 0;
	p2.x = iWidth;
	p2.y = iHigh;
	m_pD3DPen[2]->DrawLine(p1, p2);

	//MoveToEx(hDC, iWidth, 0, NULL);
	//LineTo(hDC, 0, iHigh);

	p1.x = iWidth;
	p1.y = 0;
	p2.x = 0;
	p2.y = iHigh;
	m_pD3DPen[2]->DrawLine(p1, p2);

	for (i = 1; i < 10; i++)
	{
		int iXPos = i * fPortionW;
		int iYPos = i * fPortionH;

		if (i == 2)
		{
			//MoveToEx(hDC, iXPos, 0, NULL);
			//LineTo(hDC, iWidth, iHigh - iYPos);

			p1.x = iXPos;
			p1.y = 0;
			p2.x = iWidth;
			p2.y = iHigh - iYPos;
			m_pD3DPen[1]->DrawLine(p1, p2);


			//MoveToEx(hDC, 0, iYPos, NULL);
			//LineTo(hDC, iWidth - iXPos, iHigh);

			p1.x = 0;
			p1.y = iYPos;
			p2.x = iWidth - iXPos;
			p2.y = iHigh;
			m_pD3DPen[1]->DrawLine(p1, p2);

			//MoveToEx(hDC, iWidth, iYPos, NULL);
			//LineTo(hDC, iXPos, iHigh);

			p1.x = iWidth;
			p1.y = iYPos;
			p2.x = iXPos;
			p2.y = iHigh;
			m_pD3DPen[1]->DrawLine(p1, p2);
		}

		if (i == 8)
		{
			//MoveToEx(hDC, iXPos, 0, NULL);
			//LineTo(hDC, 0, iYPos);
			p1.x = iXPos;
			p1.y = 0;
			p2.x = 0;
			p2.y = iYPos;
			m_pD3DPen[1]->DrawLine(p1, p2);
		}
	}

	for (i = 1; i < 10; i++)
	{
		int iXPos = i * fPortionW;
		int iYPos = i * fPortionH;
		if (i == 2)
		{
			//MoveToEx(hDC, iXPos, 0, NULL);
			//LineTo(hDC, 0, iYPos);

			p1.x = iXPos;
			p1.y = 0;
			p2.x = 0;
			p2.y = iYPos;
			m_pD3DPen[3]->DrawLine(p1, p2);

			//MoveToEx(hDC, iXPos, iHigh, NULL);
			//LineTo(hDC, 0, iHigh - iYPos);

			p1.x = iXPos;
			p1.y = iHigh;
			p2.x = 0;
			p2.y = iHigh - iYPos;
			m_pD3DPen[3]->DrawLine(p1, p2);
		}

		if (i == 8)
		{
			//MoveToEx(hDC, iXPos, 0, NULL);
			//LineTo(hDC, iWidth, iHigh - iYPos);

			p1.x = iXPos;
			p1.y = 0;
			p2.x = iWidth;
			p2.y = iHigh - iYPos;
			m_pD3DPen[3]->DrawLine(p1, p2);

			//MoveToEx(hDC, iXPos, iHigh, NULL);
			//LineTo(hDC, iWidth, iYPos);

			p1.x = iXPos;
			p1.y = iHigh;
			p2.x = iWidth;
			p2.y = iYPos;
			m_pD3DPen[3]->DrawLine(p1, p2);
		}
	}

	for (int i = 1; i < 10; i++)
	{
		int iXPos = i * fPortionW;
		int iYPos = i * fPortionH;
		if (i == 4)
		{
			//MoveToEx(hDC, iXPos, 0, NULL);
			///LineTo(hDC, iWidth, iHigh - iYPos);

			p1.x = iXPos;
			p1.y = 0;
			p2.x = iWidth;
			p2.y = iHigh - iYPos;
			m_pD3DPen[4]->DrawLine(p1, p2);

			//MoveToEx(hDC, 0, iYPos, NULL);
			//LineTo(hDC, iWidth - iXPos, iHigh);

			p1.x = 0;
			p1.y = iYPos;
			p2.x = iWidth - iXPos;
			p2.y = iHigh;
			m_pD3DPen[4]->DrawLine(p1, p2);

			//MoveToEx(hDC, iWidth, iYPos, NULL);
			//LineTo(hDC, iXPos, iHigh);

			p1.x = iWidth;
			p1.y = iYPos;
			p2.x = iXPos;
			p2.y = iHigh;
			m_pD3DPen[4]->DrawLine(p1, p2);
		}

		if (i == 6)
		{
			//MoveToEx(hDC, iXPos, 0, NULL);
			//LineTo(hDC, 0, iYPos);

			p1.x = iXPos;
			p1.y = 0;
			p2.x = 0;
			p2.y = iYPos;
			m_pD3DPen[4]->DrawLine(p1, p2);
		}
	}

	for (int i = 1; i < 10; i++)
	{
		int iXPos = i * fPortionW;
		int iYPos = i * fPortionH;
		if (i == 4)
		{
			//MoveToEx(hDC, iXPos, 0, NULL);
			//LineTo(hDC, 0, iYPos);

			p1.x = iXPos;
			p1.y = 0;
			p2.x = 0;
			p2.y = iYPos;
			m_pD3DPen[5]->DrawLine(p1, p2);
		}

		if (i == 6)
		{
			//MoveToEx(hDC, iXPos, 0, NULL);
			//LineTo(hDC, iWidth, iHigh - iYPos);

			p1.x = iXPos;
			p1.y = 0;
			p2.x = iWidth;
			p2.y = iHigh - iYPos;
			m_pD3DPen[5]->DrawLine(p1, p2);

			//MoveToEx(hDC, 0, iYPos, NULL);
			//LineTo(hDC, iWidth - iXPos, iHigh);

			p1.x = 0;
			p1.y = iYPos;
			p2.x = iWidth - iXPos;
			p2.y = iHigh;
			m_pD3DPen[5]->DrawLine(p1, p2);

			//MoveToEx(hDC, iWidth, iYPos, NULL);
			//LineTo(hDC, iXPos, iHigh);

			p1.x = iWidth;
			p1.y = iYPos;
			p2.x = iXPos;
			p2.y = iHigh;
			m_pD3DPen[5]->DrawLine(p1, p2);
		}
	}
}
/*
void DisplayManager::DrawLineMark(IDirect3DSurface9* pD3DSurface,int iWidth, int iHigh)
{
	HDC hDC;
	float fPortionW = (float)iWidth / 10.0;
	float fPortionH = (float)iHigh / 10.0;
	HRESULT hr = pD3DSurface->GetDC(&hDC);

//	float fPortionW = iWidth / 10.0;
//	float fPortionH = iHigh / 10.0;
//	HDC hDC = GetDC(m_pWins[i]->GetSafeHwnd());
	HPEN hOldPen = (HPEN)SelectObject(hDC, m_hPen[0]);
	for (int j = 1; j < 10; j++)
	{
		int iXPos = j * fPortionW;
		int iYPos = j * fPortionH;

		MoveToEx(hDC, iXPos, 0, NULL);
		LineTo(hDC, iXPos, iHigh);

		MoveToEx(hDC, 0, iYPos, NULL);
		LineTo(hDC, iWidth, iYPos);
	}

	MoveToEx(hDC, 1, 1, NULL);
	LineTo(hDC, iWidth - 1, 1);
	LineTo(hDC, iWidth - 1, iHigh - 1);
	LineTo(hDC, 1, iHigh - 1);
	LineTo(hDC, 1, 1);

	SelectObject(hDC, m_hPen[2]);

	MoveToEx(hDC, 0, 0, NULL);
	LineTo(hDC, iWidth, iHigh);

	MoveToEx(hDC, iWidth, 0, NULL);
	LineTo(hDC, 0, iHigh);

	SelectObject(hDC, m_hPen[1]);
	for (int i = 1; i < 10; i++)
	{
		int iXPos = i * fPortionW;
		int iYPos = i * fPortionH;

		if (i == 2)
		{
			MoveToEx(hDC, iXPos, 0, NULL);
			LineTo(hDC, iWidth, iHigh - iYPos);

			MoveToEx(hDC, 0, iYPos, NULL);
			LineTo(hDC, iWidth - iXPos, iHigh);

			MoveToEx(hDC, iWidth, iYPos, NULL);
			LineTo(hDC, iXPos, iHigh);
		}

		if (i == 8)
		{
			MoveToEx(hDC, iXPos, 0, NULL);
			LineTo(hDC, 0, iYPos);
		}
	}

	SelectObject(hDC, m_hPen[3]);
	for (int i = 1; i < 10; i++)
	{
		int iXPos = i * fPortionW;
		int iYPos = i * fPortionH;
		if (i == 2)
		{
			MoveToEx(hDC, iXPos, 0, NULL);
			LineTo(hDC, 0, iYPos);

			MoveToEx(hDC, iXPos, iHigh, NULL);
			LineTo(hDC, 0, iHigh - iYPos);
		}

		if (i == 8)
		{
			MoveToEx(hDC, iXPos, 0, NULL);
			LineTo(hDC, iWidth, iHigh - iYPos);

			MoveToEx(hDC, iXPos, iHigh, NULL);
			LineTo(hDC, iWidth, iYPos);
		}
	}

	SelectObject(hDC, m_hPen[4]);
	for (int i = 1; i < 10; i++)
	{
		int iXPos = i * fPortionW;
		int iYPos = i * fPortionH;
		if (i == 4)
		{
			MoveToEx(hDC, iXPos, 0, NULL);
			LineTo(hDC, iWidth, iHigh - iYPos);

			MoveToEx(hDC, 0, iYPos, NULL);
			LineTo(hDC, iWidth - iXPos, iHigh);

			MoveToEx(hDC, iWidth, iYPos, NULL);
			LineTo(hDC, iXPos, iHigh);
		}

		if (i == 6)
		{
			MoveToEx(hDC, iXPos, 0, NULL);
			LineTo(hDC, 0, iYPos);
		}
	}

	SelectObject(hDC, m_hPen[5]);
	for (int i = 1; i < 10; i++)
	{
		int iXPos = i * fPortionW;
		int iYPos = i * fPortionH;
		if (i == 4)
		{
			MoveToEx(hDC, iXPos, 0, NULL);
			LineTo(hDC, 0, iYPos);
		}

		if (i == 6)
		{
			MoveToEx(hDC, iXPos, 0, NULL);
			LineTo(hDC, iWidth, iHigh - iYPos);

			MoveToEx(hDC, 0, iYPos, NULL);
			LineTo(hDC, iWidth - iXPos, iHigh);

			MoveToEx(hDC, iWidth, iYPos, NULL);
			LineTo(hDC, iXPos, iHigh);
		}
	}

	SelectObject(hDC, hOldPen);
//	ReleaseDC(m_pWins[i]->GetSafeHwnd(), hDC);
//}

	pD3DSurface->ReleaseDC(hDC);
}
*/
void DisplayManager::DrawColorBarMark(int iWidth, int iHigh)
{
	POINT p1, p2;
	int iColorIndex = m_iFrameCount % 6;

	p1.x = 0;
	p1.y = iHigh / 4;
	p2.x = iWidth;
	p2.y = iHigh / 4;
	m_pD3DColorBarPen[iColorIndex]->DrawLine(p1, p2);

	p1.x = 0;
	p1.y = iHigh / 2;
	p2.x = iWidth;
	p2.y = iHigh / 2;
	m_pD3DColorBarPen[iColorIndex]->DrawLine(p1, p2);

	p1.x = 0;
	p1.y = iHigh * 3 / 4;
	p2.x = iWidth;
	p2.y = iHigh * 3 / 4;
	m_pD3DColorBarPen[iColorIndex]->DrawLine(p1, p2);

//	m_iFrameCount++;
}

void DisplayManager::SetMarkMode(int iMode)
{
	m_iMarkMode = iMode;
}
#endif

int DisplayManager::GetFrameCount()
{
	return m_iFrameCount;
}

void DisplayManager::UpdateFrameCount(int iValue)
{
	m_iFrameCount = iValue;
}

float DisplayManager::GetAvgFPS(int iIndex)
{
	return m_FPSCounter[iIndex].GetAvgFPS();
}

void DisplayManager::SetCurPos(int iIndex,double dPos)
{
	m_dCurPos[iIndex] = dPos;
}

double DisplayManager::GetCurPos(int iIndex)
{
	return m_dCurPos[iIndex];
}

#ifdef _ENABLE_SINGLE_EFFECT

int DisplayManager::CreateMixEffect(IDirect3DDevice9* pD3DDev, D3DFORMAT foramt, int w, int h)
{
	if (m_pMixerEffect)
		m_pMixerEffect->DestroySurfaces();
	else
		m_pMixerEffect = new CMixerEffect();

	m_pMixerEffect->Create(pD3DDev, foramt, w, h);
	return 0;
}

CMixerEffect* DisplayManager::GetMixerEffect()
{
	return m_pMixerEffect;
}

void DisplayManager::SetEffectIndex(int iID)
{
//	m_iEffectIdx = iID;
	if (m_pMixerEffect)
		m_pMixerEffect->SetEffectID(iID);

#ifdef _ENABLE_HD_EFFECT
	for (int k = 0; k < 10; k++)
		m_pProcessorHDs[k]->ResetFliter();
#endif
}

int DisplayManager::GetEffectIndex()
{
	if (m_pMixerEffect)
		return m_pMixerEffect->GetEffectID();
	return 0;
}

void DisplayManager::SetEffectParam(int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5)
{
	sEffectParam effect_param;
	effect_param.iLevel = iLevel;
	effect_param.iParm1 = iParm1;
	effect_param.iParm2 = iParm2;
	effect_param.iParm3 = iParm3;
	effect_param.iParm4 = iParm4;
	effect_param.iParm5 = iParm5;

	if (m_pMixerEffect)
		m_pMixerEffect->SetEffectParam(effect_param);

#ifdef _ENABLE_HD_EFFECT
	bool bIsHDEffect = false;

	int iTotalOfEffectItems = sizeof(g_hd_effect_param_items) / sizeof(EffectParam);
	for (int i = 0; i < iTotalOfEffectItems; i++)
	{
		if (g_hd_effect_param_items[i].iID == m_pMixerEffect->GetEffectID())
		{
			bIsHDEffect = true;
			break;
		}
	}

	if (bIsHDEffect)
	{
		switch (m_pMixerEffect->GetEffectID())
		{
			case HD_EID_BRIGHTNESS:
				//SetBrightness(iLevel);
				{
					//-16 ~ 16
					int iCurValue = (iLevel - 50) * 2;
					float fValue = iCurValue / 6.25;
					for (int k = 0; k < 10; k++)
						m_pProcessorHDs[k]->SetBrightness(fValue);
				}
				break;
			case HD_EID_CONTRAST:
				//SetContrast(iLevel);
				{
					//0~16
					int iCurValue = (iLevel - 50) * 2;
					float fValue = (iCurValue + 100.0) / (6.25 * 2);
					for (int k = 0; k < 10; k++)
						m_pProcessorHDs[k]->SetContrast(fValue);
				}
				break;
			case HD_EID_HUE:
				//SetHue(iLevel);
				{
					//-16 ~ 16
					int iCurValue = (iLevel - 50) * 2;
					float fValue = iCurValue / 6.25;
					for (int k = 0; k < 10; k++)
						m_pProcessorHDs[k]->SetHue(fValue);
				}
				break;
			case HD_EID_SATURATION:
				//SetSaturation(iLevel);
				{
					//0~16
					int iCurValue = (iLevel - 50) * 2;
					float fValue = (iCurValue + 100.0) / (6.25 * 2);
					for (int k = 0; k < 10; k++)
						m_pProcessorHDs[k]->SetSaturation(fValue);
				}
				break;
			case HD_EID_R:
				//SetR(iLevel);
				{
					int iCurValue = (iLevel - 50) * 2;

					if (iCurValue == 0)
					{
						for (int k = 0; k < 10; k++)
						{
							m_pProcessorHDs[k]->SetRedAlpha(0);
							m_pProcessorHDs[k]->SetBabyBlueAlpha(0);
						}
					}
					else if (iCurValue > 0)
					{
						float fValue = iCurValue * 1.5;
						for (int k = 0; k < 10; k++)
						{
							m_pProcessorHDs[k]->SetRedAlpha(fValue);
							m_pProcessorHDs[k]->SetBabyBlueAlpha(0);
						}
					}
					else
					{
						float fValue = abs(iCurValue) * 1.5;
						for (int k = 0; k < 10; k++)
						{
							m_pProcessorHDs[k]->SetBabyBlueAlpha(fValue);
							m_pProcessorHDs[k]->SetRedAlpha(0);
						}
					}
				}
				break;
			case HD_EID_G:
				//SetG(iLevel);
				{
					int iCurValue = (iLevel - 50) * 2;

					if (iCurValue == 0)
					{
						for (int k = 0; k < 10; k++)
						{
							m_pProcessorHDs[k]->SetGreenAlpha(0);
							m_pProcessorHDs[k]->SetPurpleAlpha(0);
						}
					}
					else if (iCurValue > 0)
					{
						float fValue = iCurValue * 1.5;
						for (int k = 0; k < 10; k++)
						{
							m_pProcessorHDs[k]->SetGreenAlpha(fValue);
							m_pProcessorHDs[k]->SetPurpleAlpha(0);
						}
					}
					else
					{
						float fValue = abs(iCurValue) * 1.5;
						for (int k = 0; k < 10; k++)
						{
							m_pProcessorHDs[k]->SetPurpleAlpha(fValue);
							m_pProcessorHDs[k]->SetGreenAlpha(0);
						}
					}
				}
				break;
			case HD_EID_B:
				//SetB(iLevel);
				{
					int iCurValue = (iLevel - 50) * 2;

					if (iCurValue == 0)
					{
						for (int k = 0; k < 10; k++)
						{
							m_pProcessorHDs[k]->SetBlueAlpha(0);
							m_pProcessorHDs[k]->SetYellowAlpha(0);
						}
					}
					else if (iCurValue > 0)
					{
						float fValue = abs(iCurValue) * 1.5;
						for (int k = 0; k < 10; k++)
						{
							m_pProcessorHDs[k]->SetBlueAlpha(fValue);
							m_pProcessorHDs[k]->SetYellowAlpha(0);
						}
					}
					else
					{
						float fValue = abs(iCurValue) * 1.5;
						for (int k = 0; k < 10; k++)
						{
							m_pProcessorHDs[k]->SetYellowAlpha(fValue);
							m_pProcessorHDs[k]->SetBlueAlpha(0);
						}
					}
				}
				break;
			case HD_EID_RGBA:
				{
					for (int k = 0; k < 10; k++)
					{
						int iCurApha = (float)iLevel * 2.55;
						int iR = (float)iParm1 * 2.55;
						int iG = (float)iParm2 * 2.55;
						int iB = (float)iParm3 * 2.55;
						m_pProcessorHDs[k]->SetBGColor(iR, iG, iB);
						m_pProcessorHDs[k]->SetPlanarAlpha(iCurApha);
					}
				}
				break;
		}
	}
#endif
}

void DisplayManager::SetEffectParam(int iIndex, int iValue)
{
	if (m_pMixerEffect)
		m_pMixerEffect->SetEffectParam(iIndex, iValue);
}

INT32 DisplayManager::GetEffectParam(int iIndex)
{
	INT32 iResult = 0;

	if (m_pMixerEffect)
		iResult = m_pMixerEffect->GetEffectParam(iIndex);

	return iResult;
}

void DisplayManager::EnableToDoEffect(bool bEnable)
{
	m_bDoEffect = bEnable;
}

bool DisplayManager::EffectIsEnabled()
{
	return m_bDoEffect;
}

#endif

IDirect3DSurface9* DisplayManager::DoHDEffect(int iIndex,IDirect3DSurface9* pIn,int w,int h)
{
#ifdef _ENABLE_GPU
	if (m_pProcessorHDs[iIndex]->GetSurfaceWidth() != w || m_pProcessorHDs[iIndex]->GetSurfaceHeight() != h)
	{
		int iFormat = (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2');
		m_pProcessorHDs[iIndex]->RestSurface((D3DFORMAT)iFormat, w, h);
	}

	m_pProcessorHDs[iIndex]->UpdateSurface((IDirect3DSurface9*)pIn);
	int iRet = m_pProcessorHDs[iIndex]->ProcessVideoFrame();

	if (iRet < 0)
		return 0;

	return m_pProcessorHDs[iIndex]->GetBGSurface();
#else
	return 0;
#endif
	//hr = m_pD3DDeviceEx->StretchRect(m_pProcessorHD->GetBGSurface(), &rSrc, pBackBuffer,
		//&rDest, D3DTEXF_NONE); //D3DTEXF_LINEAR
}

void DisplayManager::DoHDEffect2(int iIndex, IDirect3DSurface9* pIn, int w, int h)
{
	HRESULT hr;
	D3DSurfaceItem* pCurD3DSurfaceItem = GetSurface(iIndex);
	if (pCurD3DSurfaceItem)
	{
#ifdef _ENABLE_GPU
		if (m_pProcessorHDs[iIndex]->GetSurfaceWidth() != w || m_pProcessorHDs[iIndex]->GetSurfaceHeight() != h)
		{
			int iFormat = (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2');
			m_pProcessorHDs[iIndex]->RestSurface((D3DFORMAT)iFormat, w, h);
		}

		m_pProcessorHDs[iIndex]->UpdateSurface((IDirect3DSurface9*)pIn);
		int iRet = m_pProcessorHDs[iIndex]->ProcessVideoFrame();

		if (iRet >= 0)
		{
			IDirect3DSurface9* pHDOutSurface = m_pProcessorHDs[iIndex]->GetBGSurface();
			if(pHDOutSurface)
				hr = m_pD3DDeviceEx->StretchRect(pHDOutSurface, NULL, pCurD3DSurfaceItem->pOverlaySurface, NULL, D3DTEXF_NONE); //D3DTEXF_LINEAR
		}
#endif
	}
}

void DisplayManager::DoHDEffect3(int iIndex, IDirect3DSurface9* pIn, int w, int h)
{
	HRESULT hr;
	D3DSurfaceItem* pCurD3DSurfaceItem = GetSurface(iIndex);
	if (pCurD3DSurfaceItem)
	{
#ifdef _ENABLE_GPU
		if (m_pProcessorHDs[iIndex]->GetSurfaceWidth() != w || m_pProcessorHDs[iIndex]->GetSurfaceHeight() != h)
		{
			int iFormat = (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2');
			m_pProcessorHDs[iIndex]->RestSurface((D3DFORMAT)iFormat, w, h);
		}

		m_pProcessorHDs[iIndex]->UpdateSurface((IDirect3DSurface9*)pIn);
		int iRet = m_pProcessorHDs[iIndex]->ProcessVideoFrame();

		/*
		if (iRet >= 0)
		{
			IDirect3DSurface9* pHDOutSurface = m_pProcessorHDs[iIndex]->GetBGSurface();
			if (pHDOutSurface)
				hr = m_pD3DDeviceEx->StretchRect(pHDOutSurface, NULL, pCurD3DSurfaceItem->pOverlaySurface, NULL, D3DTEXF_NONE); //D3DTEXF_LINEAR
		}
		*/
#endif
	}
}

IDirect3DSurface9* DisplayManager::GetHDSurface(int iIndex)
{
#ifdef _ENABLE_GPU
	return m_pProcessorHDs[iIndex]->GetBGSurface();
#else
	return 0;
#endif
}

void DisplayManager::DoAllHDEffects()
{
	HRESULT hr;
	D3DSURFACE_DESC d3d_surface_desc;
	sEffectParam effect_param = m_pMixerEffect->GetEffectParam();
	for (int i = 0; i < m_pD3DSurfaceList->GetTotal(); i++)
	{
		D3DSurfaceItem* pCurD3DSurfaceItem = GetSurface(i);
		if (pCurD3DSurfaceItem && pCurD3DSurfaceItem->pD3DSurface && pCurD3DSurfaceItem->iWidth > 0 && pCurD3DSurfaceItem->iHeight && pCurD3DSurfaceItem->pOverlaySurface)
		{
			if (m_pMixerEffect->GetEffectID() != 0 && effect_param.iLevel != 0 && m_bDoEffect && m_dCurPos[i] > 1.3 && m_bUpdate[i])
			{
				IDirect3DSurface9* pHDOutSurface = DoHDEffect(i, pCurD3DSurfaceItem->pD3DSurface, pCurD3DSurfaceItem->iWidth, pCurD3DSurfaceItem->iHeight);
				if(pHDOutSurface)
					hr = m_pD3DDeviceEx->StretchRect(pHDOutSurface, NULL, pCurD3DSurfaceItem->pOverlaySurface, NULL, D3DTEXF_NONE); //D3DTEXF_LINEAR
			}

			m_bUpdate[i] = false;
		}
	}
}

void DisplayManager::ResetHDEffect()
{
#ifdef _ENABLE_GPU
	for (int k = 0; k < 10; k++)
	{
		m_pProcessorHDs[k]->Reset();
		m_pProcessorHDs[k]->ResetFliter();
	}
#endif
}

void DisplayManager::WaitForHDEffectFinished()
{
#ifdef _ENABLE_GPU
	sEffectParam effect_param = m_pMixerEffect->GetEffectParam();
	if (m_pMixerEffect->GetEffectID() != 0 && effect_param.iLevel != 0 && m_bDoEffect)
	{
		for (int k = 0; k < 10; k++)
		{
			m_pProcessorHDs[k]->WaitForFinished();
		}
	}
#endif
}

void DisplayManager::ApplySetting()
{
	int iPresentFPS = GetPrivateProfileInt("Timeline_Setting", "FPS", 10, theApp.m_strCurPath + "Setting.ini");
	m_iPresentInterval = (float)1000.0 / (float)iPresentFPS;
}

unsigned int __stdcall _PlayerThreadProc(void* lpvThreadParm)
{
	DisplayManager* pObj = (DisplayManager *)lpvThreadParm;

	while (pObj->IsAlive())
	{
		pObj->TimeEvent();
		Sleep(1);
	}

	return 0;
}
