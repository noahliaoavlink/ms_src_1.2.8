#include "stdafx.h"
#include "D3DRender.h"

#include "VersionHelpers.h"

#include "MixerCtrlDlg.h"

#include "Mmsystem.h"

#pragma comment(lib, "winmm.lib")

//#pragma comment(lib, "D3D9.lib")

#define _WIN32_WINNT_WIN7                   0x0601
/*
VERSIONHELPERAPI IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0,{ 0 }, 0, 0 };
	DWORDLONG        const dwlConditionMask = VerSetConditionMask(
		VerSetConditionMask(
			VerSetConditionMask(
				0, VER_MAJORVERSION, VER_GREATER_EQUAL),
			VER_MINORVERSION, VER_GREATER_EQUAL),
		VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

	osvi.dwMajorVersion = wMajorVersion;
	osvi.dwMinorVersion = wMinorVersion;
	osvi.wServicePackMajor = wServicePackMajor;

	return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}


bool IsWindows7OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 0);
}
*/

D3DPen::D3DPen()
{
	D3DCOLOR m_D3DColor;
	m_iWidth = 1;
	m_pLine = 0;
	m_D3DColor = D3DCOLOR_ARGB(0xFF, 0x80, 0x80, 0x00);
}

D3DPen::~D3DPen()
{
	if (m_pLine)
	{
		m_pLine->Release();
		m_pLine = 0;
	}
}

//D3DCOLOR_ARGB(0xFF, 0x00, 0x00, 0x00)  //80
void D3DPen::Create(IDirect3DDevice9* pD3DDevice, int iWidth, D3DCOLOR color)
{
	HRESULT hr = S_OK;
	m_pD3DDevice = pD3DDevice;
	m_iWidth = iWidth;
	m_D3DColor = color;

	hr = D3DXCreateLine(pD3DDevice, &m_pLine);
	if (SUCCEEDED(hr))
	{
		m_pLine->SetWidth(m_iWidth);
		m_pLine->SetAntialias(true);
		m_pLine->SetGLLines(false);
	}
}

void D3DPen::DrawLine(POINT p1, POINT p2)
{
	D3DXVECTOR2 vLine0[2];
	vLine0[0].x = (float)p1.x;
	vLine0[0].y = (float)p1.y;
	vLine0[1].x = (float)p2.x;
	vLine0[1].y = (float)p2.y;

	m_pD3DDevice->BeginScene();
	m_pLine->Begin();
	m_pLine->Draw(vLine0, 2, m_D3DColor);
	m_pLine->End();
	m_pD3DDevice->EndScene();
}

void D3DPen::DrawRect(RECT rect)
{
	D3DXVECTOR2 vLine0[2];
	vLine0[0].x = (float)rect.left;
	vLine0[0].y = (float)rect.top;
	vLine0[1].x = (float)rect.right;
	vLine0[1].y = (float)rect.top;

	D3DXVECTOR2 vLine1[2];
	vLine1[0].x = (float)rect.right;
	vLine1[0].y = (float)rect.top;
	vLine1[1].x = (float)rect.right;
	vLine1[1].y = (float)rect.bottom;

	D3DXVECTOR2 vLine2[2];
	vLine2[0].x = (float)rect.left;
	vLine2[0].y = (float)rect.bottom;
	vLine2[1].x = (float)rect.right;
	vLine2[1].y = (float)rect.bottom;

	D3DXVECTOR2 vLine3[2];
	vLine3[0].x = (float)rect.left;
	vLine3[0].y = (float)rect.bottom;
	vLine3[1].x = (float)rect.left;
	vLine3[1].y = (float)rect.top;

	m_pD3DDevice->BeginScene();
	m_pLine->Begin();
	m_pLine->Draw(vLine0, 2, m_D3DColor);
	m_pLine->Draw(vLine1, 2, m_D3DColor);
	m_pLine->Draw(vLine2, 2, m_D3DColor);
	m_pLine->Draw(vLine3, 2, m_D3DColor);
	m_pLine->End();
	m_pD3DDevice->EndScene();
}

D3DRender::D3DRender()
{
	hD3DLib = 0;
	OurDirect3DCreate9 = 0;
	d3d_surface = NULL;
	d3d_backbuf = NULL;
	m_bHadInited = FALSE;

	m_bIsD3D9Ex = false;

	//if (0)//CheckD3D9Ex() == S_OK)
#ifdef _ENABLE_D3D9EX
	if (CheckD3D9Ex() == S_OK)
	{
		char szMsg[512];
		sprintf_s(szMsg, "#nCCTV_HD_D3D# CD3D9Helper::D3DRender() m_bIsD3D9Ex = true \n");
		OutputDebugStringA(szMsg);
		m_bIsD3D9Ex = true;
	}
#endif
}

D3DRender::~D3DRender()
{
}

int D3DRender::Init(HWND hWnd, int iPixelFormat, int iWidth, int iHeight)
{
	m_hWnd = hWnd;
	m_iWidth = iWidth;
	m_iHeight = iHeight;

	m_VideoSurfaceFmt = (D3DFORMAT)GetD3DFormat(iPixelFormat);

	if (m_bIsD3D9Ex)
		Direct3DCreate9Ex(D3D_SDK_VERSION, &m_pD3DEx);
	else
		m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);  //OurDirect3DCreate9

	IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();

	if (!pD3D9Ptr)
	{
		MessageBoxA(NULL, "Could not create Direct3D9 instance.", "", MB_OK|MB_TOPMOST);
		return 0;
	}

	int iRet = GetDeviceInfo();

	//vo_w32_init();

	CreateDevice(hWnd);

	CreateSurfaces(iWidth, iHeight, m_VideoSurfaceFmt);

	SetViewport(iWidth, iHeight);

	m_bHadInited = TRUE;
	return 1;
}

void D3DRender::Destroy()
{
	DestroySurfaces();

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

void D3DRender::DestroySurfaces()
{
	if (d3d_surface)
		d3d_surface->Release();
	d3d_surface = NULL;

	/* kill the OSD texture and its shadow copy */
	/*
	if (d3d_texture_osd)
	d3d_texture_osd->Release();
	d3d_texture_osd = NULL;
	*/

	/*
	if (priv->d3d_texture_system)
	IDirect3DTexture9_Release(priv->d3d_texture_system);
	priv->d3d_texture_system = NULL;
	*/
	if (d3d_backbuf)
		d3d_backbuf->Release();
	d3d_backbuf = NULL;
}

D3DFORMAT D3DRender::GetSurfaceFormat()
{
	return m_VideoSurfaceFmt;
}

int D3DRender::GetDeviceInfo()
{
	char szMsg[512];

	//D3DCAPS9 d3dCaps;
	DWORD texture_caps;
	DWORD dev_caps;
	D3DADAPTER_IDENTIFIER9 di;

	D3DDISPLAYMODE d3ddm;
	IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();

	HMONITOR hMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	UINT uAdapterCount = pD3D9Ptr->GetAdapterCount();
	for (UINT i = 0; i < uAdapterCount; i++)
	{
		HMONITOR hmAdapter = pD3D9Ptr->GetAdapterMonitor(i);

		sprintf_s(szMsg, "#nCCTV_HD_D3D# D3DRender::GetDeviceInfo() 0 [%d , %d]\n", i, hmAdapter);
		OutputDebugStringA(szMsg);

		if (hmAdapter == hMonitor)
			m_iAdapter = i;
	}

	if (D3D_OK != pD3D9Ptr->GetAdapterDisplayMode(m_iAdapter, &d3ddm))
	{
		MessageBoxA(NULL, "Could not read adapter display mode.", "", MB_OK|MB_TOPMOST);
		return 0;
	}

	sprintf_s(szMsg, "#nCCTV_HD_D3D# D3DRender::GetDeviceInfo() 1 [%d , %d , %d]\n", m_iAdapter, d3ddm.Format, d3ddm.Width, d3ddm.Height);
	OutputDebugStringA(szMsg);

	pD3D9Ptr->GetAdapterIdentifier(m_iAdapter, 0, &di); //D3DADAPTER_DEFAULT
														//di.Description;

	sprintf_s(szMsg, "#nCCTV_HD_D3D# D3DRender::GetDeviceInfo() adapter [%s]\n", di.Description);
	OutputDebugStringA(szMsg);

	/* Store in priv->desktop_fmt the user desktop's colorspace. Usually XRGB. */
	desktop_fmt = d3ddm.Format;
	cur_backbuf_width = d3ddm.Width;
	cur_backbuf_height = d3ddm.Height;

	ZeroMemory(&d3dCaps, sizeof(d3dCaps));
	if (D3D_OK != pD3D9Ptr->GetDeviceCaps(m_iAdapter, D3DDEVTYPE_HAL, &d3dCaps))
	{
		MessageBoxA(NULL, "Could not read adapter capabilities.", "", MB_OK|MB_TOPMOST);
		return 0;
	}
	
	/* Store relevant information reguarding caps of device */
	/*
	texture_caps = d3dCaps.TextureCaps;
	dev_caps = d3dCaps.DevCaps;
	device_caps_power2_only = (texture_caps & D3DPTEXTURECAPS_POW2) &&
		!(texture_caps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL);
	device_caps_square_only = texture_caps & D3DPTEXTURECAPS_SQUAREONLY;
	device_texture_sys = dev_caps & D3DDEVCAPS_TEXTURESYSTEMMEMORY;
	max_texture_width = d3dCaps.MaxTextureWidth;
	max_texture_height = d3dCaps.MaxTextureHeight;
	*/
	return 1;
}

int D3DRender::CreateDevice(HWND hWnd, bool bReset)
{
	m_bUseFlipEx = false;

#if 1
	bool bIsWin7 = IsWindows7OrGreater();

#else
	OSVERSIONINFOW version;
	ZeroMemory(&version, sizeof(version));
	version.dwOSVersionInfoSize = sizeof(version);
	GetVersionEx(&version);

	bool bIsWin7 = (version.dwMajorVersion > 6) ||
		((version.dwMajorVersion == 6) && (version.dwMinorVersion >= 1));
#endif

	if (bIsWin7 && m_bIsD3D9Ex)
		m_bUseFlipEx = true;

	DWORD dwBehaviorFlags = 0;
	// TODO - try alternative adaptor formats if necessary
	//D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&m_D3Dpp, sizeof(D3DPRESENT_PARAMETERS));
	m_D3Dpp.Windowed = TRUE;
	//d3dpp.Flags                  = D3DPRESENTFLAG_VIDEO;
	m_D3Dpp.hDeviceWindow = hWnd;//vo_w32_window; /* w32_common var */
	m_D3Dpp.BackBufferWidth = cur_backbuf_width;
	m_D3Dpp.BackBufferHeight = cur_backbuf_height;
	//d3dpp.MultiSampleType        = D3DMULTISAMPLE_NONE;
	m_D3Dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;//D3DPRESENT_INTERVAL_DEFAULT;//D3DPRESENT_INTERVAL_IMMEDIATE;//D3DPRESENT_INTERVAL_ONE;
	m_D3Dpp.BackBufferFormat = desktop_fmt;

	m_D3Dpp.EnableAutoDepthStencil = FALSE;
	m_D3Dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_D3Dpp.MultiSampleQuality = 0;                  // FIXME: Unimplemented
	m_D3Dpp.MultiSampleType = D3DMULTISAMPLE_NONE;   // FIXME: Unimplemented

	dwBehaviorFlags = D3DCREATE_MULTITHREADED;
	dwBehaviorFlags |= D3DCREATE_FPU_PRESERVE;
	
	if (d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		dwBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		dwBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	if (m_bIsD3D9Ex)
		dwBehaviorFlags |= D3DCREATE_DISABLE_DRIVER_MANAGEMENT_EX;

	if (m_bUseFlipEx)
	{
		m_D3Dpp.SwapEffect = D3DSWAPEFFECT_FLIPEX;
		m_D3Dpp.BackBufferCount = 2;

		m_D3Dpp.Flags = D3DPRESENTFLAG_VIDEO;

		//d3dpp.Flags &= ~D3DPRESENTFLAG_DEVICECLIP; // FlipEx fails if D3DPRESENTFLAG_DEVICECLIP is set.
		//d3dpp.Flags &= ~D3DPRESENTFLAG_RESTRICT_SHARED_RESOURCE_DRIVER; //AP will hang on calling Present if adding this flag on AMD case (PCOM+FlipEx). Don't know root cause yet.

		//dwBehaviorFlags = D3DCREATE_FPU_PRESERVE | D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_ENABLE_PRESENTSTATS;
		//dwBehaviorFlags != D3DCREATE_ENABLE_PRESENTSTATS;
	}
	else
	{
		m_D3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_COPY;//D3DSWAPEFFECT_OVERLAY;//D3DSWAPEFFECT_COPY;
		m_D3Dpp.BackBufferCount = 1;

		m_D3Dpp.Flags = D3DPRESENTFLAG_VIDEO;
	}

	if (bReset)
	{
		IDirect3DDevice9* pD3DDevice = GetD3DDevice();
		if (D3D_OK != pD3DDevice->Reset(&m_D3Dpp))
		{
			MessageBoxA(NULL, "Reseting Direct3D Device failed.", "", MB_OK|MB_TOPMOST);
			return 0;
		}
	}
	else
	{
		IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();
		if (!pD3D9Ptr)
			return 0;

		if (m_bIsD3D9Ex)
		{
			if (D3D_OK != ((IDirect3D9Ex*)pD3D9Ptr)->CreateDeviceEx(m_iAdapter,
				D3DDEVTYPE_HAL, m_D3Dpp.hDeviceWindow,
				dwBehaviorFlags,//D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&m_D3Dpp, NULL, &m_pD3DDeviceEx))
			{
				MessageBoxA(NULL, "Creating Direct3D DeviceEx failed.", "", MB_OK|MB_TOPMOST);
				return 0;
			}
		}
		else
		{
			if (D3D_OK != pD3D9Ptr->CreateDevice(m_iAdapter,
				D3DDEVTYPE_HAL, m_D3Dpp.hDeviceWindow,
				dwBehaviorFlags,//D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&m_D3Dpp, &m_pD3DDevice))
			{
				MessageBoxA(NULL, "Creating Direct3D Device failed.", "", MB_OK|MB_TOPMOST);
				return 0;
			}
		}
	}
	return 1;
}

int D3DRender::CreateSurfaces(int iWidth, int iHeight, D3DFORMAT format)
{
	d3d_surface = NULL;
	d3d_backbuf = NULL;

	bool bHasHW = FormatSupported(m_VideoSurfaceFmt, desktop_fmt);
	if (!bHasHW)
	{
		if (strcmp("I420", GetD3DFormatName(format)) == 0)
		{
			//Change color space to YV12. 
			m_VideoSurfaceFmt = (D3DFORMAT)GetD3DFormat(VPF_YV12);
			bHasHW = FormatSupported(m_VideoSurfaceFmt, desktop_fmt);
		}

		if (!bHasHW)
		{
			char szMsg[256];
			sprintf_s(szMsg, "Can`nt support %s format!!", GetD3DFormatName(format));
			MessageBoxA(NULL, szMsg, "", MB_OK|MB_TOPMOST);
			return 0;
		}
		format = m_VideoSurfaceFmt;
	}

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	HRESULT hr = pD3DDevice->CreateOffscreenPlainSurface(iWidth, iHeight,
		format, D3DPOOL_DEFAULT, &d3d_surface, NULL);
	if (FAILED(hr))
	{
		char szMsg[512];
		sprintf_s(szMsg, "Failed to create surface!! ErrorID:%d w:%d h:%d format:%d", hr, iWidth, iHeight, format);
		MessageBoxA(NULL, szMsg, "", MB_OK|MB_TOPMOST);
		return 0;
	}

	//hr = pD3DDevice->CreateOffscreenPlainSurface(iWidth, iHeight,D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &m_pD3DSurface2, NULL);
	//hr = pD3DDevice->CreateOffscreenPlainSurface(cur_backbuf_width, cur_backbuf_height, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &m_pD3DSurface2, NULL);
	/*
	hr = pD3DDevice->GetBackBuffer(0, 0,D3DBACKBUFFER_TYPE_MONO,&d3d_backbuf);
	if (FAILED(hr))
	{
	MessageBoxA(NULL,"Failed to GetBackBuffer.","",MB_OK);
	return 0;
	}
	*/
	SetupDefaultRenderState();

	return 1;
}

int D3DRender::RestSurfaceFormat(int iPixelFormat, int iWidth, int iHeight)
{
	m_iPixelFormat = iPixelFormat;
	m_VideoSurfaceFmt = (D3DFORMAT)GetD3DFormat(iPixelFormat);
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	return RestSurfaces(iWidth, iHeight, m_VideoSurfaceFmt);
}

IDirect3DSurface9* D3DRender::GetSurface()
{
	return d3d_surface;
}

D3DFORMAT D3DRender::GetDesktopD3DFormat()
{
	return desktop_fmt;
}

int D3DRender::RestSurfaces(int iWidth, int iHeight, D3DFORMAT format)
{
	DestroySurfaces();
	return CreateSurfaces(iWidth, iHeight, format);
}

int D3DRender::GetWidth()
{
	return m_iWidth;
}

int D3DRender::GetHeight()
{
	return m_iHeight;
}

void D3DRender::SetViewport(int iWidth, int iHeight)
{
	D3DVIEWPORT9 vp = { 0, 0, iWidth, iHeight, 0, 1 };
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	if (pD3DDevice)
		pD3DDevice->SetViewport(&vp);
}

bool D3DRender::FormatSupported(D3DFORMAT surface, D3DFORMAT adaptor)
{
	IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();
	if (!pD3D9Ptr)
		return false;

	HRESULT hr = pD3D9Ptr->CheckDeviceFormat(m_iAdapter, D3DDEVTYPE_HAL,
		adaptor, 0, D3DRTYPE_SURFACE, surface);
	if (SUCCEEDED(hr))
	{
		hr = pD3D9Ptr->CheckDeviceFormatConversion(m_iAdapter, D3DDEVTYPE_HAL, surface, adaptor);
		if (SUCCEEDED(hr))
			return true;
	}
	return false;
}

void D3DRender::SetupDefaultRenderState()
{
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	/* setup default renderstate */
	pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pD3DDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x0);
	pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
}

int D3DRender::Flip()
{
	RECT rect = { 0, 0, cur_backbuf_width, cur_backbuf_height };

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	if (!pD3DDevice)
	{
		int iRet = CheckDeviceState();
		if (iRet == 1)
			return 2;
		return -2;
	}

	if (!pD3DDevice)
		return -1;

	HRESULT hr;
	//HRESULT hr = pD3DDevice->Present(&rect, 0, 0, 0);
	if (m_bIsD3D9Ex)
	{
	/*
		if(m_bUseFlipEx)
			hr = ((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(&rect, 0, 0, 0,D3DPRESENT_FORCEIMMEDIATE);
		else
			hr = ((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(&rect, 0, 0, 0,0);
			*/
		hr = ((IDirect3DDevice9Ex*)pD3DDevice)->PresentEx(0, 0, 0, 0, 0);
	}
	else
		hr = pD3DDevice->Present(&rect, 0, 0, 0);

	if (FAILED(hr))
	{
		int iRet = CheckDeviceState();
		if (iRet == 1)
			return 2;
		return -2;
		//MessageBox(NULL,"Reset","",MB_OK);
	}

	return 1;

//	else
	//	m_iFrameCount++;
}

int D3DRender::Reset()
{
#if 0
	DestroySurfaces();

	/* Destroy the D3D Device */
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	if (pD3DDevice)
		pD3DDevice->Release();
	pD3DDevice = NULL;

	if (m_bIsD3D9Ex)
	{
		if (m_pD3DEx)
			m_pD3DEx->Release();
	}
	else
	{
		if (m_pD3D)
			m_pD3D->Release();
	}
#else
	Destroy();
#endif

	/* Initialize Direct3D from the beginning */
	if (m_bIsD3D9Ex)
		Direct3DCreate9Ex(D3D_SDK_VERSION, &m_pD3DEx);
	else
		m_pD3D = Direct3DCreate9(D3D_SDK_VERSION); //OurDirect3DCreate9

	IDirect3D9* pD3D9Ptr = GetDirect3D9Ptr();

	if (!pD3D9Ptr)
	{
		MessageBoxA(NULL, "Could not create Direct3D9 instance.", "", MB_OK|MB_TOPMOST);
		return 0;
	}

	GetDeviceInfo();
	CreateDevice(m_hWnd);
	CreateSurfaces(m_iWidth, m_iHeight, m_VideoSurfaceFmt);
	SetViewport(m_iWidth, m_iHeight);
	return 1;
}

int D3DRender::CheckDeviceState()
{
	HRESULT hr;

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	// Check device state
	hr = pD3DDevice->TestCooperativeLevel();

	// Device can be reset now
	//if (SUCCEEDED(hr) || hr == D3DERR_DEVICENOTRESET)
	if (hr == D3DERR_DEVICENOTRESET)
	{
		Reset();
		return 1;
	}
	return -1;
}

int D3DRender::RenderFrame(unsigned char* pBuffer, int iWidth, int iHeight)
{
	int iRet;
	iRet = UpdateSurfaceData(pBuffer, iWidth, iHeight);

	if (iRet == 0)
		return 0;

	return RenderFrame1(d3d_surface, iWidth, iHeight);
}

int D3DRender::RenderFrame1(IDirect3DSurface9* pSurface, int iWidth, int iHeight)
{
	int iRet;
	iRet = BeginDrawSurface();

	if (iRet == 0)
		return 0;

	iRet = DrawSurface(pSurface, iWidth, iHeight);

	if (iRet == 0)
		return 0;
	

	return EndDrawSurface();
}

int D3DRender::BeginDrawSurface()
{
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	if (!pD3DDevice)
		return 0;

	HRESULT hr = pD3DDevice->BeginScene();

	if (hr != D3D_OK)
	{
		MessageBoxA(NULL, "BeginScene() failed.", "", MB_OK|MB_TOPMOST);
		return 0;
	}

	hr = pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET,
		D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	if (hr != D3D_OK)
	{
		MessageBoxA(NULL, "Clear() failed.", "", MB_OK| MB_TOPMOST);
		return 0;
	}

	return 1;
}

int D3DRender::EndDrawSurface()
{
	IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	if (!pD3DDevice)
		return 0;

	HRESULT hr = pD3DDevice->EndScene();
	if (hr != D3D_OK)
	{
		MessageBoxA(NULL, "EndScene() failed.", "", MB_OK| MB_TOPMOST);
		return 0;
	}

	return 1;
}

int D3DRender::DrawSurface(IDirect3DSurface9* pSurface, int iWidth, int iHeight)
{
	RECT rDest, rSrc;

	rSrc.left = 0;
	rSrc.top = 0;
	rSrc.right = iWidth;
	rSrc.bottom = iHeight;

	rDest.left = 0;
	rDest.top = 0;
	rDest.right = cur_backbuf_width;// iWidth;
	rDest.bottom = cur_backbuf_height;// iHeight;

	IDirect3DDevice9* pD3DDevice = GetD3DDevice();

	IDirect3DSurface9 *pBackBuffer;
	if (pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer) != D3D_OK)
	{
		//error
		return 0;
	}

	pD3DDevice->SetRenderTarget(0, pBackBuffer);

	HRESULT hr = pD3DDevice->StretchRect(pSurface, &rSrc, pBackBuffer,
		&rDest, D3DTEXF_LINEAR); //
	if (FAILED(hr))
	{
		//MessageBoxA(NULL,"StretchRect() failed.","",MB_OK);
		char szMsg[512];
		sprintf_s(szMsg, "D3DError - StretchRect() failed!!\n");
		OutputDebugStringA(szMsg);
		return 0;
	}


	pBackBuffer->Release();

	return 1;
}

int D3DRender::UpdateSurfaceData(unsigned char* pBuffer, int iWidth, int iHeight)
{
	return UpdateSurfaceData(d3d_surface, pBuffer, iWidth, iHeight);
}

int D3DRender::UpdateSurfaceData(IDirect3DSurface9* pSurface, unsigned char* pBuffer, int iWidth, int iHeight)
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
		sprintf_s(szMsg, "D3DError - Failed to lock picture surface!! %s \n","");
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

int D3DRender::GetD3DFormat(int iPixelFormat)
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
	default:
		break;		
	}

	return 0;
}

HRESULT D3DRender::CheckD3D9Ex(void)
{
	HRESULT hr = E_FAIL;
	HMODULE libHandle = NULL;

	// Manually load the d3d9.dll library.
	libHandle = LoadLibraryA("d3d9.dll");

	if (libHandle != NULL)
	{
		// Define a function pointer to the Direct3DCreate9Ex function.
		typedef HRESULT(WINAPI *LPDIRECT3DCREATE9EX)(UINT, void **);

		// Obtain the address of the Direct3DCreate9Ex function. 
		LPDIRECT3DCREATE9EX Direct3DCreate9ExPtr = NULL;

		Direct3DCreate9ExPtr = (LPDIRECT3DCREATE9EX)GetProcAddress(libHandle, "Direct3DCreate9Ex");

		if (Direct3DCreate9ExPtr != NULL)
		{
			// Direct3DCreate9Ex is supported.
			hr = S_OK;
		}
		else
		{
			// Direct3DCreate9Ex is not supported on this
			// operating system.
			hr = ERROR_NOT_SUPPORTED;
		}

		// Free the library.
		FreeLibrary(libHandle);

	}

	return hr;
}

IDirect3D9* D3DRender::GetDirect3D9Ptr()
{
	if (m_bIsD3D9Ex)
		return m_pD3DEx;
	return m_pD3D;
}

IDirect3DDevice9* D3DRender::GetD3DDevice()
{
	if (m_bIsD3D9Ex)
		return m_pD3DDeviceEx;
	return m_pD3DDevice;
}

char* D3DRender::GetD3DFormatName(D3DFORMAT iFormat)
{
	if (iFormat == (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'))
		return "YV12";
	else if (iFormat == (D3DFORMAT)MAKEFOURCC('I', '4', '2', '0'))
		return "I420";
	else if (iFormat == (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2'))
		return "NV12";
	else if (iFormat == (D3DFORMAT)MAKEFOURCC('Y', 'U', 'Y', '2'))
		return "YUY2";
	else if (iFormat == D3DFMT_A8R8G8B8)
		return "RGB32";
	else if (iFormat == D3DFMT_R8G8B8)
		return "RGB24";
	else
		return "Unknown";
}

void D3DRender::Clear()
{
	int iRet;
	iRet = BeginDrawSurface();
	iRet = EndDrawSurface();
}

D3DPRESENT_PARAMETERS* D3DRender::GetD3DPresentParameters()
{
	return &m_D3Dpp;
}

bool D3DRender::IsD3D9Ex()
{
	return m_bIsD3D9Ex;
}