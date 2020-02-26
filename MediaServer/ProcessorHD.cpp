#include "stdafx.h"
#include "ProcessorHD.h"

#include "dxvahd_utils.h"
#include "Mmsystem.h"

//Dxva2.lib
#pragma comment(lib, "Dxva2.lib")

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

RECT ScaleRectangle(const RECT& input, const RECT& src, const RECT& dst)
{
	RECT rect;

	UINT src_dx = src.right - src.left;
	UINT src_dy = src.bottom - src.top;

	UINT dst_dx = dst.right - dst.left;
	UINT dst_dy = dst.bottom - dst.top;

	// Scale the input rectangle by dst / src.

	rect.left = input.left   * dst_dx / src_dx;
	rect.right = input.right  * dst_dx / src_dx;
	rect.top = input.top    * dst_dy / src_dy;
	rect.bottom = input.bottom * dst_dy / src_dy;

	return rect;
}

VOID WINAPI ColorFill(D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord,const D3DXVECTOR2* pTexelSize, LPVOID pData)
{
	D3DXVECTOR4* pColor = (D3DXVECTOR4*)pData;
	*pOut = D3DXVECTOR4(pColor->x, pColor->y, pColor->z, pColor->w);
}

ProcessorHD::ProcessorHD()
{
	m_bDXVA_SW = false;
	m_pD3DDeviceEx = 0;
	m_pMainStream = 0;

	m_pDXVAVP = 0;
	m_pDXVAHD = 0;

	m_iSurfaceW = 0;
	m_iSurfaceH = 0;

	m_usage = DXVAHD_DEVICE_USAGE_PLAYBACK_NORMAL;

	m_ulFrameCount = 0;
	m_iGPUSurfaceCount = 0;
	//m_iLastGPUSurfaceCount = -1;
	m_bFinished = true;

	m_fAlpha = 0.0;

	//m_PixelAlphaValue = DEFAULT_PIXEL_ALPHA_VALUE;
	//m_PlanarAlphaValue = DEFAULT_PLANAR_ALPHA_VALUE;

	//m_rcMainVideoSourceRect = VIDEO_MAIN_RECT;
	m_rcMainVideoDestRect = VIDEO_MAIN_DEST_RECT;
	//m_rcVideoSubRect = VIDEO_SUB_RECT;

	//m_iExtendedColor = 0;
	//m_iBackgroundColor = 0;

	m_TargetWidthPercent = 100;
	m_TargetHeightPercent = 100;

#if _BG_MODE
	m_pBGTexture = 0;
	m_pBGTextureSurface = 0;
/*
	m_pD3DSprite = new D3DSprite;

	m_BlackOverlay.pTexture = 0;

	m_RedOverlay.pTexture = 0;
	m_GreenOverlay.pTexture = 0;
	m_BlueOverlay.pTexture = 0;
	m_BabyBlueOverlay.pTexture = 0;
	m_YellowOverlay.pTexture = 0;
	m_PurpleOverlay.pTexture = 0;
	m_WhiteOverlay.pTexture = 0;
	*/
#endif

	m_CurBGColor.RGB.R = 128 / 255.0;
	m_CurBGColor.RGB.G = 128 / 255.0;
	m_CurBGColor.RGB.B = 128 / 255.0;
	m_CurBGColor.RGB.A = 1.0;
}

ProcessorHD::~ProcessorHD()
{
	DestroySurface();

#if _BG_MODE
	DestroyOverlayResource(m_pBGTextureSurface, m_pBGTexture);
/*
	if (m_pD3DSprite)
		delete m_pD3DSprite;

	if (m_BlackOverlay.pTexture)
		m_BlackOverlay.pTexture->Release();

	if(m_RedOverlay.pTexture)
		m_RedOverlay.pTexture->Release();

	if (m_GreenOverlay.pTexture)
		m_GreenOverlay.pTexture->Release();

	if (m_BlueOverlay.pTexture)
		m_BlueOverlay.pTexture->Release();

	if (m_BabyBlueOverlay.pTexture)
		m_BabyBlueOverlay.pTexture->Release();

	if (m_YellowOverlay.pTexture)
		m_YellowOverlay.pTexture->Release();

	if (m_PurpleOverlay.pTexture)
		m_PurpleOverlay.pTexture->Release();

	if(m_WhiteOverlay.pTexture)
		m_WhiteOverlay.pTexture->Release();
		*/
#endif
}

bool ProcessorHD::Init(HWND hWnd,IDirect3DDevice9Ex* pD3DDeviceEx,int w,int h)
{
	HRESULT hr = S_OK;
	DWORD index = 0;

	m_hWnd = hWnd;
	m_iWidth = w;
	m_iHeight = h;
	m_pD3DDeviceEx = pD3DDeviceEx;

	D3DFORMAT *pFormats = NULL;
	DXVAHD_VPCAPS *pVPCaps = NULL;

	DXVAHD_CONTENT_DESC desc;

	desc.InputFrameFormat = DXVAHD_FRAME_FORMAT_PROGRESSIVE;
	desc.InputFrameRate.Numerator = VIDEO_FPS;
	desc.InputFrameRate.Denominator = 1;
	desc.InputWidth = w;// VIDEO_MAIN_WIDTH;
	desc.InputHeight = h;// VIDEO_MAIN_HEIGHT;
	desc.OutputFrameRate.Numerator = VIDEO_FPS;
	desc.OutputFrameRate.Denominator = 1;
	desc.OutputWidth = w;//VIDEO_MAIN_WIDTH;
	desc.OutputHeight = h;//VIDEO_MAIN_HEIGHT;
	//desc.Usage = D3D11_VIDEO_USAGE_PLAYBACK_NORMAL;

	PDXVAHDSW_Plugin pSWPlugin = NULL;

	if (m_bDXVA_SW)
	{
		// Load the software DXVA-HD device.

		HMODULE hSWPlugin = LoadLibraryA("dxvahdsw.dll");

		if (hSWPlugin == NULL)
		{
			OutputDebugStringA("Could not load dxvahdsw.dll\n");
			return FALSE;
		}

		pSWPlugin = (PDXVAHDSW_Plugin)GetProcAddress(hSWPlugin, "DXVAHDSW_Plugin");

		if (pSWPlugin == NULL)
		{
			OutputDebugStringA("Could not get DXVAHDSW_Plugin proc address.\n");
			return FALSE;
		}
	}

	BOOL bRet = IsBadReadPtr(m_pD3DDeviceEx, sizeof(m_pD3DDeviceEx));

	if (bRet)
	{
		char szMsg[512];
		sprintf(szMsg, "ProcessorHD - DXVAHD_CreateDevice failed !!\n");
		OutputDebugStringA(szMsg);

//		MessageBoxA(NULL, szMsg, "", MB_OK | MB_TOPMOST);
		return -1;
	}

	// Create the DXVA-HD device.
	hr = DXVAHD_CreateDevice(
		m_pD3DDeviceEx,
		&desc,
		m_usage,
		pSWPlugin,
		&m_pDXVAHD
	);

	if (FAILED(hr)) 
		goto done;

	// Get the DXVA-HD device caps.
	ZeroMemory(&caps, sizeof(caps));

	hr = m_pDXVAHD->GetVideoProcessorDeviceCaps(&caps);

	if (FAILED(hr))
		goto done;

	if (caps.MaxInputStreams < 1 + SUB_STREAM_COUNT)
	{
		//OutputDebugString(L"Device only supports %d input streams.\n", caps.MaxInputStreams);
		hr = E_FAIL;
		goto done;
	}

	// Check the output format.

	pFormats = new (std::nothrow) D3DFORMAT[caps.OutputFormatCount];
	if (pFormats == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto done;
	}

	//D3DFORMAT i420format = (D3DFORMAT)MAKEFOURCC('I', '4', '2', '0');
	D3DFORMAT nv12format = (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2');
	D3DFORMAT yv12format = (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2');

	int iFormat = yv12format;

	hr = m_pDXVAHD->GetVideoProcessorOutputFormats(caps.OutputFormatCount, pFormats);

	if (FAILED(hr))
		goto done;

	for (index = 0; index < caps.OutputFormatCount; index++)
	{
		if (pFormats[index] == VIDEO_RENDER_TARGET_FORMAT)
		{
			break;
		}
	}

	if (index == caps.OutputFormatCount)
	{
		hr = E_FAIL;
		goto done;
	}

	delete[] pFormats;
	pFormats = NULL;

	// Check the input formats.

	pFormats = new (std::nothrow) D3DFORMAT[caps.InputFormatCount];
	if (pFormats == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto done;
	}

	hr = m_pDXVAHD->GetVideoProcessorInputFormats(caps.InputFormatCount, pFormats);

	if (FAILED(hr)) { goto done; }
	
	D3DFORMAT inputFormats[] = { VIDEO_MAIN_FORMAT, VIDEO_SUB_FORMAT };

	for (DWORD j = 0; j < 2; j++)
	{
		for (index = 0; index < caps.InputFormatCount; index++)
		{
			if (pFormats[index] == inputFormats[j])
			{
				break;
			}
		}
		if (index == caps.InputFormatCount)
		{
			hr = E_FAIL;
			goto done;
		}
	}

	delete[] pFormats;
	pFormats = NULL;
	
	// Create the VP device.

	pVPCaps = new (std::nothrow) DXVAHD_VPCAPS[caps.VideoProcessorCount];
	if (pVPCaps == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto done;
	}

	hr = m_pDXVAHD->GetVideoProcessorCaps(caps.VideoProcessorCount, pVPCaps);

	if (FAILED(hr))
		goto done;

	hr = m_pDXVAHD->CreateVideoProcessor(&pVPCaps[0].VPGuid, &m_pDXVAVP);

	if (FAILED(hr))
		goto done;

	m_iSurfaceW = VIDEO_MAIN_WIDTH;
	m_iSurfaceH = VIDEO_MAIN_HEIGHT;
	/*
	//DXVAHD_SetInputColorSpace(m_pDXVAVP,0,TRUE,0,0,0 );
	DXVAHD_COLOR clr;
	clr.RGB = { 0,0,0,0 };
	//hr = DXVAHD_SetBackgroundColor(m_pDXVAVP,FALSE,clr);
	*/

	/*
	// Create the video surface for the primary video stream.
	hr = m_pDXVAHD->CreateVideoSurface(
		VIDEO_MAIN_WIDTH,
		VIDEO_MAIN_HEIGHT,
		VIDEO_MAIN_FORMAT,
		caps.InputPool,
		0,
		DXVAHD_SURFACE_TYPE_VIDEO_INPUT,
		1,
		&m_pMainStream,
		NULL
	);
	
	if (FAILED(hr)) { goto done; }
	*/

	// Set the initial stream states for the primary stream.
	hr = DXVAHD_SetStreamFormat(m_pDXVAVP, 0, VIDEO_MAIN_FORMAT);

	if (FAILED(hr)) { goto done; }

	hr = DXVAHD_SetFrameFormat(m_pDXVAVP, 0, DXVAHD_FRAME_FORMAT_PROGRESSIVE);

	if (FAILED(hr)) { goto done; }

	/*
	// Create substream surfaces.
	hr = m_pDXVAHD->CreateVideoSurface(
		VIDEO_SUB_SURF_WIDTH,
		VIDEO_SUB_SURF_HEIGHT,
		VIDEO_SUB_FORMAT,
		caps.InputPool,
		0,
		DXVAHD_SURFACE_TYPE_VIDEO_INPUT,
		SUB_STREAM_COUNT,
		m_ppSubStream,
		NULL
	);

	if (FAILED(hr)) { goto done; }


	// Set the initial stream states for the substream.

	// Video format
	hr = DXVAHD_SetStreamFormat(m_pDXVAVP, 1, VIDEO_SUB_FORMAT);

	if (FAILED(hr)) { goto done; }
	*/
	/*
	// Frame format (progressive)
	hr = DXVAHD_SetFrameFormat(m_pDXVAVP, 1, DXVAHD_FRAME_FORMAT_PROGRESSIVE);

	if (FAILED(hr)) { goto done; }
	
	// Luma key
	hr = DXVAHD_SetLumaKey(m_pDXVAVP, 1, TRUE, 0.9f, 1.0f);

	if (FAILED(hr)) { goto done; }
	*/
	// Draw the video frame for the primary video stream. 
	// This frame does not change.

	//hr = DrawColorBars(m_pMainStream, VIDEO_MAIN_WIDTH, VIDEO_MAIN_HEIGHT);

	//if (FAILED(hr)) { goto done; }

	// Get the image filtering capabilities.

	for (DWORD i = 0; i < NUM_FILTERS; i++)
	{
		if (caps.FilterCaps & (1 << i))
		{
			m_Filters[i].bSupported = TRUE;

			m_pDXVAHD->GetVideoProcessorFilterRange(PROCAMP_FILTERS[i], &m_Filters[i].Range);

			m_Filters[i].CurrentValue = m_Filters[i].Range.Default;

			INT range = m_Filters[i].Range.Maximum - m_Filters[i].Range.Minimum;

			//m_Filters[i].Step = range > 32 ? range / 32 : 1;
			m_Filters[i].Step = range > 200 ? range / 200 : 1;

		}
		else
			m_Filters[i].bSupported = FALSE;
	}

	ResetFliter();
	/*
	for (DWORD i = 0; i < NUM_FILTERS; i++)
	{
		hr = AdjustFilter((DXVAHD_FILTER)i, -10000);

		if (FAILED(hr)) { goto done; }
	}
	*/
#if _BG_MODE
	m_pBGTexture = CreateOverlayTexture(w, h);
	m_pBGTexture->GetSurfaceLevel(0, &m_pBGTextureSurface);
/*
	m_pD3DSprite->Create(pD3DDeviceEx);
//	CreateFadeOverlay(w, h);
	CreateRGBColorOverlay();
	*/
#endif

done:
	delete[] pFormats;
	delete[] pVPCaps;
	return (SUCCEEDED(hr));
}

IDirect3DSurface9* ProcessorHD::GetMainStream()
{
	return m_pMainStream;
}

HRESULT ProcessorHD::AdjustFilter(DXVAHD_FILTER filter, int dy)
{
	HRESULT hr = S_OK;

	if (!m_pDXVAVP)
		return -1;

	if (!m_bFinished)
		return -2;

	if (filter > NUM_FILTERS)
		return E_UNEXPECTED;

	if (!m_Filters[filter].bSupported)
		return S_OK; // Unsupported filter. Ignore.

	const INT step = m_Filters[filter].Step;
	const INT minimum = m_Filters[filter].Range.Minimum;
	const INT maximum = m_Filters[filter].Range.Maximum;

	//INT val = m_Filters[filter].CurrentValue + dy * step;
	//INT val = minimum + dy * step;

	INT val = dy * step;

	if (val >= minimum && val <= maximum)
	{
		hr = DXVAHD_SetFilterValue(
			m_pDXVAVP,
			0,
			filter,
			TRUE,
			val
		);

		if (FAILED(hr))
		{
			// Try the default.

			val = m_Filters[filter].Range.Default;

			hr = DXVAHD_SetFilterValue(
				m_pDXVAVP,
				0,
				filter,
				TRUE,
				val
			);

		}

		if (SUCCEEDED(hr))
			m_Filters[filter].CurrentValue = val;

	}
	
	return hr;
}

HRESULT ProcessorHD::AdjustTargetRect(int dx, int dy)
{
	HRESULT hr = S_OK;
#if 0
	if (dy > 0)
		m_TargetHeightPercent = min(100, m_TargetHeightPercent + 4);
	else if (dy < 0)
		m_TargetHeightPercent = max(0, m_TargetHeightPercent - 4);

	if (dx > 0)
		m_TargetWidthPercent = min(100, m_TargetWidthPercent + 4);
	else if (dx < 0)
		m_TargetWidthPercent = max(0, m_TargetWidthPercent - 4);

	RECT client;
	RECT target;

	GetClientRect(m_hWnd, &client);

	target.left = client.left + (client.right - client.left) / 2 * (100 - m_TargetWidthPercent) / 100;
	target.right = client.right - (client.right - client.left) / 2 * (100 - m_TargetWidthPercent) / 100;
	target.top = client.top + (client.bottom - client.top) / 2 * (100 - m_TargetHeightPercent) / 100;
	target.bottom = client.bottom - (client.bottom - client.top) / 2 * (100 - m_TargetHeightPercent) / 100;
#else
	RECT target = {0,0,1920,1080};
#endif
	hr = DXVAHD_SetTargetRect(m_pDXVAVP, TRUE, target);

	return hr;
}

int ProcessorHD::ProcessVideoFrame()
{
	HRESULT hr = S_OK;
	IDirect3DSurface9 *pRT = NULL;  // Render target
	RECT dest = { 0 };
	RECT ssdest = { 0 };

	if (m_iGPUSurfaceCount < 1)
		return -1;

	//if(m_iLastGPUSurfaceCount == m_iGPUSurfaceCount)
	//	return -1;

	if (!m_pD3DDeviceEx)
		return -2;
	if (!m_pDXVAVP)
		return -3;
	if(!m_pDXVAHD)
		return -4;

	m_bFinished = false;

	DWORD dwTime1 = timeGetTime();

	DXVAHD_STREAM_DATA stream_data[2];
	ZeroMemory(&stream_data, sizeof(stream_data));

	RECT client;
	GetClientRect(m_hWnd, &client);

	client.right = 1920;
	client.bottom = 1080;

#if _BG_MODE
	pRT = m_pBGTextureSurface;
	m_pD3DDeviceEx->SetRenderTarget(0, m_pBGTextureSurface);
#else
	hr = m_pD3DDeviceEx->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pRT);
#endif
//	m_pD3DDeviceEx->SetRenderTarget(0, pRT);

	if (FAILED(hr)) { 
		goto done; 
	}
	/*
	if(m_iGPUSurfaceCount < 1 || m_ulFrameCount < 1)
		stream_data[0].Enable = FALSE;
	else  */
		stream_data[0].Enable = TRUE;
	stream_data[0].OutputIndex = 0;
	stream_data[0].InputFrameOrField = m_ulFrameCount;// m_iGPUSurfaceCount;
	stream_data[0].pInputSurface = m_pGPUSurface;// m_pMainStream;
/*
	stream_data[1].Enable = TRUE;
	stream_data[1].OutputIndex = 0;
	stream_data[1].InputFrameOrField = m_ulFrameCount;
	stream_data[1].pInputSurface = m_ppSubStream[0];
	*/

	AdjustTargetRect(0, 0);

	// Apply the destination rectangle for the main video stream.
	// Scale the destination rectangle to the window client area.
	dest = ScaleRectangle(m_rcMainVideoDestRect, VIDEO_MAIN_RECT, client);

	hr = DXVAHD_SetDestinationRect(
		m_pDXVAVP,
		0,
		TRUE,
		dest
	);

	if (FAILED(hr)) { 
		goto done; 
	}
	
	// Calculate the substream destination rectangle from the frame number.
	CalculateSubstreamRect(m_ulFrameCount, &ssdest);

	// Scale to the window client area.
	ssdest = ScaleRectangle(ssdest, VIDEO_MAIN_RECT, client);

	hr = DXVAHD_SetDestinationRect(
		m_pDXVAVP,
		1,
		TRUE,
		ssdest
	);

	if (FAILED(hr)) 
	{ 
		goto done; 
	}

	if (m_TargetWidthPercent < 100 || m_TargetHeightPercent < 100)
	{
		hr = m_pD3DDeviceEx->ColorFill(pRT, NULL, D3DCOLOR_XRGB(255, 255, 255));

		if (FAILED(hr)) { 
			goto done; 
		}
	}

	DWORD dwTime2 = timeGetTime();
	/*
	int i;
	D3DSURFACE_DESC surfaceDesc;
	pRT->GetDesc(&surfaceDesc);
	D3DLOCKED_RECT LockedRect;
	hr = pRT->LockRect(&LockedRect, nullptr, D3DLOCK_READONLY);
	if (FAILED(hr)) 
		goto done;

	unsigned char *src = (unsigned char*)LockedRect.pBits;

	for (i = 0; i < surfaceDesc.Height; i++) //surfaceDesc.Height //pInFrame->height
	{
	}

	pRT->UnlockRect();
	*/

	//hr = DXVAHD_SetStreamFormat(m_pDXVAVP, 0, (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2'));

	BOOL bRet = IsBadReadPtr(stream_data[0].pInputSurface, sizeof(stream_data[0].pInputSurface));
	if (bRet)
	{
		m_bFinished = true;
		return false;
	}

	bRet = IsBadReadPtr(m_pDXVAHD,sizeof(m_pDXVAHD));
	if (bRet)
	{
		m_bFinished = true;
		return false;
	}

	try
	{
//		m_iLastGPUSurfaceCount = m_iGPUSurfaceCount;
		// Perform the blit.
		hr = m_pDXVAVP->VideoProcessBltHD(
			pRT,
			m_ulFrameCount,
			1,//2,
			stream_data
		);
	}
	catch (...)
	{
//		OutputDebugString("HEError - Catching an exception in VideoProcessBltHD !! \n");
//		::MessageBoxA(NULL, "VideoProcessBltHD() Failed!!", "", MB_OK | MB_TOPMOST);
	}

	DWORD dwTime3 = timeGetTime();

	if (FAILED(hr)) {
		goto done;
	}

#if _BG_MODE
	/*
	hr = m_pD3DDeviceEx->BeginScene();

	ComputeOverlayAlpha();

	DrawOverlay(&m_RedOverlay);
	DrawOverlay(&m_GreenOverlay);
	DrawOverlay(&m_BlueOverlay);
	DrawOverlay(&m_BabyBlueOverlay);
	DrawOverlay(&m_YellowOverlay);
	DrawOverlay(&m_PurpleOverlay);
	DrawOverlay(&m_WhiteOverlay);
	DrawOverlay(&m_BlackOverlay);

	hr = m_pD3DDeviceEx->EndScene();
	*/
#else
#endif

	DWORD dwTime4 = timeGetTime();

	if (dwTime2 - dwTime1 > 3 || dwTime3 - dwTime2 > 3 || dwTime4 - dwTime3 > 3)
	{
		char szMsg[512];
		sprintf(szMsg, "#HD_effect# ProcessorHD::ProcessVideoFrame(): [%d , %d , %d]\n", dwTime2 - dwTime1, dwTime3 - dwTime2, dwTime4 - dwTime3);
		OutputDebugStringA(szMsg);
	}

	if (FAILED(hr)) { 
		goto done; 
	}

	m_ulFrameCount++;
done:

#if _BG_MODE
#else
	SafeRelease(&pRT);
#endif
	m_bFinished = true;
	return SUCCEEDED(hr);
}

void ProcessorHD::Flip()
{
	HRESULT hr;

#if _BG_MODE
	IDirect3DSurface9 *pRT = NULL;  // Render target
	hr = m_pD3DDeviceEx->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pRT);
	m_pD3DDeviceEx->SetRenderTarget(0, pRT);

	hr = m_pD3DDeviceEx->StretchRect(m_pBGTextureSurface, NULL, pRT,NULL, D3DTEXF_NONE); //D3DTEXF_LINEAR

	pRT->Release();
#else
#endif
	hr = m_pD3DDeviceEx->PresentEx(0, 0, 0, 0, 0);
}

void ProcessorHD::CalculateSubstreamRect(INT frame, RECT *prect)
{
	INT x, y, wx, wy;

	x = frame * VIDEO_SUB_VX;
	wx = VIDEO_MAIN_WIDTH - VIDEO_SUB_WIDTH;
	x = (x / wx) & 0x1 ? wx - (x % wx) : x % wx;

	y = frame * VIDEO_SUB_VY;
	wy = VIDEO_MAIN_HEIGHT - VIDEO_SUB_HEIGHT;
	y = (y / wy) & 0x1 ? wy - (y % wy) : y % wy;

	SetRect(prect, x, y, x + VIDEO_SUB_WIDTH, y + VIDEO_SUB_HEIGHT);
}

void ProcessorHD::SetBrightness(int iValue)
{
	AdjustFilter(DXVAHD_FILTER_BRIGHTNESS, iValue);
}

void ProcessorHD::SetContrast(int iValue)
{
	AdjustFilter(DXVAHD_FILTER_CONTRAST, iValue);
}

void ProcessorHD::SetHue(int iValue)
{
	AdjustFilter(DXVAHD_FILTER_HUE, iValue);
}

void ProcessorHD::SetSaturation(int iValue)
{
	AdjustFilter(DXVAHD_FILTER_SATURATION, iValue);
}

void ProcessorHD::SetBGColor(int r,int g ,int b)
{
	HRESULT hr = S_OK;
	DXVAHD_COLOR clr;

	if (!m_pDXVAVP)
		return;

	if (!m_bFinished)
		return;

	clr.RGB.R = (float)r / 255.0;
	clr.RGB.G = (float)g / 255.0;
	clr.RGB.B = (float)b / 255.0;
	clr.RGB.A = 1.0;

	if (clr.RGB.R != m_CurBGColor.RGB.R || clr.RGB.G != m_CurBGColor.RGB.G || clr.RGB.G != m_CurBGColor.RGB.B)
	{
		m_CurBGColor = clr;

		hr = DXVAHD_SetBackgroundColor(
			m_pDXVAVP,
			FALSE,       // YCbCr?
			clr
		);
	}
}

void ProcessorHD::SetBGColor_R(int r)
{
	HRESULT hr = S_OK;

	if (!m_pDXVAVP)
		return;

	if (!m_bFinished)
		return ;

	float fRColor = (float)r / 255.0;

	if (fRColor != m_CurBGColor.RGB.R)
	{
		m_CurBGColor.RGB.R = fRColor;

		hr = DXVAHD_SetBackgroundColor(
			m_pDXVAVP,
			FALSE,       // YCbCr?
			m_CurBGColor
		);
	}
}

void ProcessorHD::SetBGColor_G(int g)
{
	HRESULT hr = S_OK;

	if (!m_pDXVAVP)
		return;

	if (!m_bFinished)
		return;

	float fGColor = (float)g / 255.0;

	if (fGColor != m_CurBGColor.RGB.G)
	{
		m_CurBGColor.RGB.G = fGColor;

		hr = DXVAHD_SetBackgroundColor(
			m_pDXVAVP,
			FALSE,       // YCbCr?
			m_CurBGColor
		);
	}
}

void ProcessorHD::SetBGColor_B(int b)
{
	HRESULT hr = S_OK;

	if (!m_pDXVAVP)
		return;

	if (!m_bFinished)
		return;

	float fBColor = (float)b / 255.0;

	if (fBColor != m_CurBGColor.RGB.B)
	{
		m_CurBGColor.RGB.B = fBColor;

		hr = DXVAHD_SetBackgroundColor(
			m_pDXVAVP,
			FALSE,       // YCbCr?
			m_CurBGColor
		);
	}
}
void ProcessorHD::SetPlanarAlpha(int iValue)
{
	HRESULT hr = S_OK;

	if (!m_pDXVAVP)
		return;

	if (!m_bFinished)
		return;

	float fAlpha = (float)iValue / 0xFF;
	//hr = DXVAHD_SetPlanarAlpha(m_pDXVAVP, 0, TRUE, float(m_PlanarAlphaValue) / 0xFF);

	if (m_fAlpha != fAlpha)
	{
		m_fAlpha = fAlpha;
		hr = DXVAHD_SetPlanarAlpha(m_pDXVAVP, 0, TRUE, fAlpha);
	}
}

int ProcessorHD::CreateSurface(D3DFORMAT format,int iWidth, int iHeight)
{
	HRESULT hr = S_OK;

	if (!m_pDXVAHD)
		return -1;

	// Create the video surface for the primary video stream.
	hr = m_pDXVAHD->CreateVideoSurface(
		iWidth,
		iHeight,
		format,
		caps.InputPool,
		0,
		DXVAHD_SURFACE_TYPE_VIDEO_INPUT,
		1,
		&m_pMainStream,
		NULL
	);

//	hr = DXVAHD_SetStreamFormat(m_pDXVAVP, 0, format);

	m_iSurfaceW = iWidth;
	m_iSurfaceH = iHeight;

	return hr;
}

void ProcessorHD::DestroySurface()
{
	if (m_pMainStream)
		m_pMainStream->Release();
	m_pMainStream = NULL;

	m_ulFrameCount = 0;
}

int ProcessorHD::RestSurface(D3DFORMAT format, int iWidth, int iHeight)
{
	DestroySurface();
	int iRet = CreateSurface(format, iWidth,iHeight);
	//int iRet = CreateSurface(VIDEO_MAIN_FORMAT, iWidth, iHeight);
	return iRet;
}

int ProcessorHD::GetSurfaceWidth()
{
	return m_iSurfaceW;
}

int ProcessorHD::GetSurfaceHeight()
{
	return m_iSurfaceH;
}

void ProcessorHD::UpdateSurface(IDirect3DSurface9* pSurface)
{
	HRESULT hr = S_OK;
	//hr = DXVAHD_SetStreamFormat(m_pDXVAVP, 0, VIDEO_MAIN_FORMAT);

	m_pGPUSurface = pSurface;
	m_iGPUSurfaceCount++;

//	hr = m_pD3DDeviceEx->StretchRect(pSurface, NULL, m_pMainStream, NULL, D3DTEXF_NONE);
//	int kk = 0;
}

IDirect3DTexture9* ProcessorHD::CreateOverlayTexture(int iWidth, int iHeight)
{
	IDirect3DTexture9* pTexture = NULL;
	//IDirect3DDevice9* pD3DDevice = GetD3DDevice();
	HRESULT hr = m_pD3DDeviceEx->CreateTexture(iWidth, iHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &pTexture, NULL);
	//HRESULT hr = m_pD3DDeviceEx->CreateTexture(iWidth, iHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &pTexture, NULL);
	return pTexture;
}

void ProcessorHD::DestroyOverlayResource(IDirect3DSurface9* pSurface, IDirect3DTexture9* pTexture)
{
	if (pSurface)
		pSurface->Release();

	if (pTexture)
		pTexture->Release();
}
/*
int ProcessorHD::CreateFadeOverlay(int iWidth, int iHeight)
{
	HRESULT hr;
	m_pFade.iWidth = 800;
	m_pFade.iHeight = 600;

	m_pFade.iAlpha = 0;

	D3DXCreateTextureFromFile(m_pD3DDeviceEx,L"D:\\noah_src\\TVWall\\VWall\\bin\\Win32\\Debug\\black.png", &m_pFade.pTexture);

	return 1;
}
*/


int ProcessorHD::CreateRGBColorOverlay()
{
	HRESULT hr;
/*
	D3DXVECTOR4 red_color = { 255,0,0,255 };
	D3DXVECTOR4 green_color = { 0,255,0,255 };
	D3DXVECTOR4 blue_color = { 0,0,255,255 };
	D3DXVECTOR4 babyblue_color = { 0,255,255,255 };
	D3DXVECTOR4 yellow_color = { 255,255,0,255 };
	D3DXVECTOR4 purle_color = { 255,0,255,255 };
	D3DXVECTOR4 white_color = { 255,255,255,255 };
	D3DXVECTOR4 black_color = { 0,0,0,255 };

	m_RedOverlay.iAlpha = 0;
	m_RedOverlay.iDrawAlpha = 0;
	m_RedOverlay.iWidth = 800;
	m_RedOverlay.iHeight = 600;
	hr = m_pD3DDeviceEx->CreateTexture(m_RedOverlay.iWidth, m_RedOverlay.iHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_RedOverlay.pTexture, NULL);
	hr = D3DXFillTexture(m_RedOverlay.pTexture, ColorFill, &red_color);

	m_GreenOverlay.iAlpha = 0;
	m_GreenOverlay.iDrawAlpha = 0;
	m_GreenOverlay.iWidth = 800;
	m_GreenOverlay.iHeight = 600;
	hr = m_pD3DDeviceEx->CreateTexture(m_GreenOverlay.iWidth, m_GreenOverlay.iHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_GreenOverlay.pTexture, NULL);
	hr = D3DXFillTexture(m_GreenOverlay.pTexture, ColorFill, &green_color);

	m_BlueOverlay.iAlpha = 0;
	m_BlueOverlay.iDrawAlpha = 0;
	m_BlueOverlay.iWidth = 800;
	m_BlueOverlay.iHeight = 600;
	hr = m_pD3DDeviceEx->CreateTexture(m_BlueOverlay.iWidth, m_BlueOverlay.iHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_BlueOverlay.pTexture, NULL);
	hr = D3DXFillTexture(m_BlueOverlay.pTexture, ColorFill, &blue_color);

	m_BabyBlueOverlay.iAlpha = 0;
	m_BabyBlueOverlay.iDrawAlpha = 0;
	m_BabyBlueOverlay.iWidth = 800;
	m_BabyBlueOverlay.iHeight = 600;
	hr = m_pD3DDeviceEx->CreateTexture(m_BabyBlueOverlay.iWidth, m_BabyBlueOverlay.iHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_BabyBlueOverlay.pTexture, NULL);
	hr = D3DXFillTexture(m_BabyBlueOverlay.pTexture, ColorFill, &babyblue_color);

	m_YellowOverlay.iAlpha = 0;
	m_YellowOverlay.iDrawAlpha = 0;
	m_YellowOverlay.iWidth = 800;
	m_YellowOverlay.iHeight = 600;
	hr = m_pD3DDeviceEx->CreateTexture(m_YellowOverlay.iWidth, m_YellowOverlay.iHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_YellowOverlay.pTexture, NULL);
	hr = D3DXFillTexture(m_YellowOverlay.pTexture, ColorFill, &yellow_color);

	m_PurpleOverlay.iAlpha = 0;
	m_PurpleOverlay.iDrawAlpha = 0;
	m_PurpleOverlay.iWidth = 800;
	m_PurpleOverlay.iHeight = 600;
	hr = m_pD3DDeviceEx->CreateTexture(m_PurpleOverlay.iWidth, m_PurpleOverlay.iHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_PurpleOverlay.pTexture, NULL);
	hr = D3DXFillTexture(m_PurpleOverlay.pTexture, ColorFill, &purle_color);

	m_WhiteOverlay.iAlpha = 0;
	m_WhiteOverlay.iDrawAlpha = 0;
	m_WhiteOverlay.iWidth = 800;
	m_WhiteOverlay.iHeight = 600;
	hr = m_pD3DDeviceEx->CreateTexture(m_WhiteOverlay.iWidth, m_WhiteOverlay.iHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_WhiteOverlay.pTexture, NULL);
	hr = D3DXFillTexture(m_WhiteOverlay.pTexture, ColorFill, &white_color);

	m_BlackOverlay.iAlpha = 0;
	m_BlackOverlay.iDrawAlpha = 0;
	m_BlackOverlay.iWidth = 800;
	m_BlackOverlay.iHeight = 600;
	hr = m_pD3DDeviceEx->CreateTexture(m_BlackOverlay.iWidth, m_BlackOverlay.iHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_BlackOverlay.pTexture, NULL);
	hr = D3DXFillTexture(m_BlackOverlay.pTexture, ColorFill, &black_color);
	*/
	return 1;
}

void ProcessorHD::SetRedAlpha(int iValue)
{
	//m_RedOverlay.iAlpha = iValue;
}

void ProcessorHD::SetBabyBlueAlpha(int iValue)
{
	//m_BabyBlueOverlay.iAlpha = iValue;
}

void ProcessorHD::SetGreenAlpha(int iValue)
{
	//m_GreenOverlay.iAlpha = iValue;
}

void ProcessorHD::SetPurpleAlpha(int iValue)
{
	//m_PurpleOverlay.iAlpha = iValue;
}

void ProcessorHD::SetBlueAlpha(int iValue)
{
	//m_BlueOverlay.iAlpha = iValue;
}

void ProcessorHD::SetYellowAlpha(int iValue)
{
	//m_YellowOverlay.iAlpha = iValue;
}

void ProcessorHD::DrawOverlay(OverlayObj* pObj)
{
	/*
	float fH = 1.0, fV = 1.0;
	if (pObj->iDrawAlpha > 0)
	{
		fH = (float)m_iWidth / (float)pObj->iWidth;
		fV = (float)m_iHeight / (float)pObj->iHeight;

		m_pD3DSprite->Scale(fH, fV);
		m_pD3DSprite->Draw(pObj->pTexture, 0, 0, pObj->iDrawAlpha);
	}
	*/
}

void ProcessorHD::ComputeOverlayAlpha()
{
	/*
	m_WhiteOverlay.iDrawAlpha = 0;
	m_BlackOverlay.iDrawAlpha = 0;
	m_RedOverlay.iDrawAlpha = 0;
	m_GreenOverlay.iDrawAlpha = 0;
	m_BlueOverlay.iDrawAlpha = 0;
	m_BabyBlueOverlay.iDrawAlpha = 0;
	m_PurpleOverlay.iDrawAlpha = 0;
	m_YellowOverlay.iDrawAlpha = 0;

	if (m_RedOverlay.iAlpha > 0 && m_GreenOverlay.iAlpha > 0 && m_BlueOverlay.iAlpha > 0)  //white
	{
		int iMin = m_RedOverlay.iAlpha;
		if (iMin > m_GreenOverlay.iAlpha)
			iMin = m_GreenOverlay.iAlpha;
		if (iMin > m_BlueOverlay.iAlpha)
			iMin = m_BlueOverlay.iAlpha;

		m_WhiteOverlay.iDrawAlpha = iMin;
		m_BlueOverlay.iDrawAlpha = m_BlueOverlay.iAlpha - iMin;
		m_GreenOverlay.iDrawAlpha = m_GreenOverlay.iAlpha - iMin;
		m_RedOverlay.iDrawAlpha = m_RedOverlay.iAlpha - iMin;
	}
	else if (m_RedOverlay.iAlpha > 0 && m_GreenOverlay.iAlpha > 0)  //yellow
	{
		int iMin = m_RedOverlay.iAlpha;
		if (iMin > m_GreenOverlay.iAlpha)
			iMin = m_GreenOverlay.iAlpha;

		if (m_YellowOverlay.iAlpha > 0)
			m_YellowOverlay.iDrawAlpha = iMin + m_YellowOverlay.iAlpha;
		else
			m_YellowOverlay.iDrawAlpha = iMin;
		m_RedOverlay.iDrawAlpha = m_RedOverlay.iAlpha - iMin;
		m_GreenOverlay.iDrawAlpha = m_GreenOverlay.iAlpha - iMin;
	}
	else if (m_RedOverlay.iAlpha > 0 && m_BlueOverlay.iAlpha > 0)  //Purple
	{
		int iMin = m_RedOverlay.iAlpha;
		if (iMin > m_BlueOverlay.iAlpha)
			iMin = m_BlueOverlay.iAlpha;

		if (m_PurpleOverlay.iAlpha > 0)
			m_PurpleOverlay.iDrawAlpha = iMin + m_PurpleOverlay.iAlpha;
		else
			m_PurpleOverlay.iDrawAlpha = iMin;
		m_RedOverlay.iDrawAlpha = m_RedOverlay.iAlpha - iMin;
		m_BlueOverlay.iDrawAlpha = m_BlueOverlay.iAlpha - iMin;
	}
	else if (m_GreenOverlay.iAlpha > 0 && m_BlueOverlay.iAlpha > 0)  //BabyBlue
	{
		int iMin = m_GreenOverlay.iAlpha;
		if (iMin > m_BlueOverlay.iAlpha)
			iMin = m_BlueOverlay.iAlpha;

		if (m_BabyBlueOverlay.iAlpha > 0)
			m_BabyBlueOverlay.iDrawAlpha = iMin + m_BabyBlueOverlay.iAlpha;
		else
			m_BabyBlueOverlay.iDrawAlpha = iMin;
		m_BlueOverlay.iDrawAlpha = m_BlueOverlay.iAlpha - iMin;
		m_GreenOverlay.iDrawAlpha = m_GreenOverlay.iAlpha - iMin;
	}
	else if (m_BabyBlueOverlay.iAlpha > 0 && m_PurpleOverlay.iAlpha > 0 && m_YellowOverlay.iAlpha > 0)  //black
	{
		int iMin = m_BabyBlueOverlay.iAlpha;
		if (iMin > m_PurpleOverlay.iAlpha)
			iMin = m_PurpleOverlay.iAlpha;
		if (iMin > m_YellowOverlay.iAlpha)
			iMin = m_YellowOverlay.iAlpha;

		m_BlackOverlay.iDrawAlpha = iMin;
		m_BabyBlueOverlay.iDrawAlpha = m_BabyBlueOverlay.iAlpha - iMin;
		m_PurpleOverlay.iDrawAlpha = m_PurpleOverlay.iAlpha - iMin;
		m_YellowOverlay.iDrawAlpha = m_YellowOverlay.iAlpha - iMin;
	}
	else
	{
		if (m_RedOverlay.iAlpha > 0)
			m_RedOverlay.iDrawAlpha = m_RedOverlay.iAlpha;
		if (m_GreenOverlay.iAlpha > 0)
			m_GreenOverlay.iDrawAlpha = m_GreenOverlay.iAlpha;
		if (m_BlueOverlay.iAlpha > 0)
			m_BlueOverlay.iDrawAlpha = m_BlueOverlay.iAlpha;

		if (m_BabyBlueOverlay.iAlpha > 0)
			m_BabyBlueOverlay.iDrawAlpha = m_BabyBlueOverlay.iAlpha;

		if (m_PurpleOverlay.iAlpha > 0)
			m_PurpleOverlay.iDrawAlpha = m_PurpleOverlay.iAlpha;

		if (m_YellowOverlay.iAlpha > 0)
			m_YellowOverlay.iDrawAlpha = m_YellowOverlay.iAlpha;
	}
	*/
}

IDirect3DSurface9* ProcessorHD::GetBGSurface()
{
	 return m_pBGTextureSurface;
}

void ProcessorHD::ResetFliter()
{
	HRESULT hr = S_OK;
	INT val;

	if (!m_pDXVAVP || !m_bFinished)
		return;
	
	for (DWORD i = 0; i < NUM_FILTERS; i++)
	{
		//hr = AdjustFilter((DXVAHD_FILTER)i, -10000);

		val = m_Filters[(DXVAHD_FILTER)i].Range.Default;

		hr = DXVAHD_SetFilterValue(
			m_pDXVAVP,
			0,
			(DXVAHD_FILTER)i,
			TRUE,
			val
		);
	}

	SetPlanarAlpha(128);
/*
	// Main video source rectangle.

	hr = DXVAHD_SetSourceRect(
		m_pDXVAVP,
		0,
		TRUE,
		VIDEO_MAIN_RECT
	);

	//if (FAILED(hr)) { goto done; }

	// Main video destination rectangle.

	hr = DXVAHD_SetDestinationRect(
		m_pDXVAVP,
		0,
		TRUE,
		VIDEO_MAIN_RECT
	);

	// Target rectangle.
	hr = AdjustTargetRect(0, 0);
	*/
	//VIDEO_MAIN_RECT

	m_CurBGColor.RGB.R = 0.0;
	m_CurBGColor.RGB.G = 0.0;
	m_CurBGColor.RGB.B = 0.0;
	m_CurBGColor.RGB.A = 0.0;

	hr = DXVAHD_SetBackgroundColor(
		m_pDXVAVP,
		FALSE,       // YCbCr?
		m_CurBGColor
	);

	
}

void ProcessorHD::Reset()
{
//	m_iSurfaceW = 0;
//	m_iSurfaceH = 0;
	m_ulFrameCount = 0;
	m_iGPUSurfaceCount = 0;
}

void ProcessorHD::WaitForFinished()
{
	DWORD dwStart = 0;
	DWORD dwEnd = 0;
	bool bDo = true;

	while (bDo)
	{
		if (m_bFinished)
			bDo = false;

		MSG Msg;
		if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE | PM_NOYIELD))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		Sleep(1);
	}
}