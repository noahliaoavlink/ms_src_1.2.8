// Pannel.cpp : 實作檔
//

#include "stdafx.h"
#include "MediaServer.h"
#include "Panel.h"

#include "ServiceProvider.h"
#include "PanelBlendDlg.h"
#include "../../../Include/StrConv.h"
#include "../../../Include/SCreateWin.h"

// CPanel
IMPLEMENT_DYNAMIC(CPanel, CDialog)

CPanel::CPanel(CWnd* pParent /*=NULL*/)
#ifdef _ENABLE_CWND
#else
	: CDialog(IDD_TEMP_DLG, pParent)
#endif
{
	m_iIndex = 0;

	m_pParent = pParent;

	for (int x = 0; x < FILTER_NUM; x++)
	{
		m_pBasicAudio = NULL;
		m_pMediaSeeking = NULL;
		m_pMixCtrl = NULL;
	}

	m_iBlendPanelIdx = -1;

	m_pMultiAllocator = NULL;
	m_pShapeDlg = NULL;
	m_bShapeUI = FALSE;
	m_BlendSetting.iStatus = 0;
	m_ptClickPos = CPoint(-1, -1);

	m_bDoRecordAVI = false;
//#ifdef _ENABLE_GPU
	m_bShowUI = FALSE;
//#else
//	m_bShowUI = TRUE;
//#endif
	m_bReleaseFocus = FALSE;
}

CPanel::~CPanel()
{ 	
	//CloseGraph(m_hWnd);
//	DestroyD3D();
}

#ifdef _ENABLE_CWND
BEGIN_MESSAGE_MAP(CPanel, CWnd)
#else
BEGIN_MESSAGE_MAP(CPanel, CDialog)
#endif
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONUP()
	ON_WM_SHOWWINDOW()
	ON_WM_RBUTTONUP()
	ON_WM_CLOSE()
	ON_WM_KEYDOWN()
	ON_MESSAGE(WM_STOP_AVI, OnStopAVI)
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// CPanel 訊息處理常式

WCHAR* CPanel::AsWideString(const char* str)
{
	if( str == NULL )
	return NULL;

	CString strTemp = str;
	 
	int nBufSize = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, strTemp, -1, NULL, 0 );
	WCHAR* wideString = new WCHAR[nBufSize+1];
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, strTemp, -1, wideString, nBufSize );
	 
	return wideString;
}

HRESULT CPanel::RunVideo(BOOL bRun)
{
	HRESULT     hr = S_OK;

	/*if (0)
	{
		FAIL_RET( g_mediaControl->Run());
		LONGLONG lCurPos, lLength;
		g_pMediaSeeking->GetCurrentPosition(&lCurPos);
		g_pMediaSeeking->GetStopPosition(&lLength);
		lCurPos = lLength/11;
		hr = g_pMediaSeeking->SetPositions(&lCurPos, AM_SEEKING_AbsolutePositioning, &lLength, AM_SEEKING_AbsolutePositioning);
		FAIL_RET( g_mediaControl->Pause());
		return hr;
	}*/

	if (m_MediaControl)
	{
		if (bRun)
			{
				FAIL_RET( m_MediaControl->Run());
			}
			else
				FAIL_RET( m_MediaControl->Stop());
	}
	return hr;
}

HRESULT CPanel::ShowTempVideo(CString strObjPath, CString strVideoPath, DOUBLE dZoomRatio, BOOL bExtraVideo, bool bCheckStatus)
{
	CStringArray strVideoAry;
	if (!strVideoPath.IsEmpty())
		strVideoAry.Add(strVideoPath);

	return ShowTempVideo(strVideoAry, strObjPath, dZoomRatio, bExtraVideo, bCheckStatus);
}

HRESULT CPanel::ShowTempVideo(CStringArray &strVideoAry, CString strObjPath, DOUBLE dZoomRatio, BOOL bExtraVideo,bool bCheckStatus)
{
	HRESULT     hr = S_OK;
   	//CloseGraph(m_hWnd);

	CString strFilePath;
	CString strTemp;
	INT32 iIndex = 0;
	for (int x = 0; x < strVideoAry.GetCount(); x++)
	{
#ifdef _ENABLE_MEDIA_STREAM_EX
		CMediaStreamEx *pMedisStream = new CMediaStreamEx(m_hWnd);
#else
		CMediaStream *pMedisStream = NULL;

		for (int y = 0; y < g_MediaStreamAry.size(); y++)
		{
			if (g_MediaStreamAry.at(y)->FilePath().IsEmpty())
			{
				pMedisStream = g_MediaStreamAry.at(y);
				iIndex = y;
				break;
			}
		}

		if (pMedisStream == NULL)
			continue;
#endif
		//pMedisStream->GrabSamples(strVideoAry.GetAt(x).AllocSysString());
		strFilePath = strVideoAry.GetAt(x);

		if (g_PannelSetting.iEncodeMedia == 0 || bExtraVideo)
		{
			pMedisStream->Open(strFilePath.GetBuffer(), TRUE);
			pMedisStream->Play();
			pMedisStream->ExtraVideo(bExtraVideo);
		}
		else
		{
			//CPlaneScene3D::FileEncoder(strFilePath.GetBuffer(), FALSE);
			if (g_PannelSetting.iEncodeMedia == 1)
			{
				CFileFind fFind;
				if (!fFind.FindFile(theApp.m_strCurPath + "\\UI Folder\\Temp"))
				{
					CreateDirectory(theApp.m_strCurPath + "\\UI Folder\\Temp", NULL);
				}
				strTemp.Format("%s\\UI Folder\\Temp\\MS%i.mov", theApp.m_strCurPath, iIndex);
#ifdef PLAY_3D_MODE
				CPlaneScene3D::FileDecoder(strFilePath.GetBuffer(), strTemp, FALSE);
#endif
				pMedisStream->Open(strTemp.GetBuffer(), TRUE);
				pMedisStream->Play();
			}
			else if (g_PannelSetting.iEncodeMedia == 2)
			{
#ifdef PLAY_3D_MODE

				ContentProcessor cp;
				wchar_t wszFileName[512];
				wcscpy(wszFileName, ANSIToUnicode(strFilePath.GetBuffer()));

				if (cp.CheckFileEncryptStatus(wszFileName))
				{
					strTemp = CPlaneScene3D::FileDecoder(strFilePath.GetBuffer(), "", FALSE, TRUE);
				}
#endif
				if (cp.CheckFileEncryptStatus(wszFileName))
					pMedisStream->Open(strTemp.GetBuffer(), TRUE);
				else
					pMedisStream->Open(strFilePath.GetBuffer(), TRUE);

				pMedisStream->Play();
			}
		}

		//INT32 iIndex = g_MediaStreamAry.size();
		//g_MediaStreamAry.resize(iIndex + 1);
		//g_MediaStreamAry.at(iIndex) = pMedisStream;
		//GrabSamples(strVideoAry.GetAt(0).AllocSysString());
	}
		
	/*if (!m_pMultiAllocator)
	{
		CRect rectCur;
		GetClientRect(&rectCur);
		m_pMultiAllocator = new CAllocator(hr, m_hWnd, m_iIndex, rectCur, NULL, NULL, NULL, this);
		FAIL_RET(m_pMultiAllocator->InitShareSurface(strObjPath, dZoomRatio));
	}
	else
	{
		m_pMultiAllocator->LoadObjFile(strObjPath, dZoomRatio);

	}*/

	if (m_pMultiAllocator->HadSurface())
		m_pMultiAllocator->LoadObjFile(strObjPath, dZoomRatio, bCheckStatus);
	else
		FAIL_RET(m_pMultiAllocator->InitShareSurface(strObjPath, dZoomRatio));

	return hr;
}

HRESULT CPanel::ShowVideoAry(CStringArray &strVideoAry)
{
	HRESULT     hr = S_OK;

#ifndef PLAY_3D_MODE
	return ShowVideo(strVideoAry);
#endif
	CloseGraph(m_hWnd);

    IVMRFilterConfig9 *filterConfig;
	IGraphBuilder     *pGraphBuild;
	IBaseFilter       *pBaseFilter;
	IMediaControl     *pMediaControl;
	IBasicAudio	      *pBasicAudio;
	IMediaSeeking	  *pMediaSeeking;
	//IVMRMixerControl9 *pMixCtrl;
	//IMediaFilter *pMediaFilter = 0;
	IReferenceClock *pClock = NULL;

	for (int x = 0; x < strVideoAry.GetCount() && m_iIndex == 0; x++)
	{
		filterConfig = NULL;
		pGraphBuild = NULL;
		pBaseFilter = NULL;
		pMediaControl = NULL;
		pBasicAudio = NULL;
		pMediaSeeking = NULL;

		FAIL_RET(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraphBuild));
		FAIL_RET(pGraphBuild->QueryInterface(IID_IBasicAudio, (void **)&pBasicAudio));

		FAIL_RET(CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pBaseFilter));
		FAIL_RET( pBaseFilter->QueryInterface(IID_IVMRFilterConfig9, reinterpret_cast<void**>(&filterConfig)));
		FAIL_RET( filterConfig->SetNumberOfStreams(strVideoAry.GetCount()));
		
		FAIL_RET( pGraphBuild->QueryInterface(IID_IMediaSeeking,  (void **)&pMediaSeeking));
		
		FAIL_RET( filterConfig->SetRenderingMode( VMR9Mode_Renderless));

		FAIL_RET( pGraphBuild->AddFilter(pBaseFilter, L"Video Mixing Renderer 9"));
		//FAIL_RET( SetAllocatorPresenter( pBaseFilter, m_hWnd, pMediaSeeking, m_iIndex));

		if (0)
		{
			WCHAR* pWCharPath = AsWideString(strVideoAry.GetAt(x));
			FAIL_RET( pGraphBuild->RenderFile(pWCharPath, NULL ));
			delete pWCharPath;
			FAIL_RET( pGraphBuild->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&pMediaControl)));
		}
		else
			AddBaseFilter(pGraphBuild, pBaseFilter, m_hWnd, m_iIndex);
     
		//pGraphBuild->QueryInterface(IID_IMediaFilter, (void**)&pMediaFilter);
		//pMediaFilter->GetSyncSource(&pClock);
	}

	if (m_iIndex != 0)
	{
		CRect rectCur;
		GetClientRect(&rectCur);
		m_pMultiAllocator = new CAllocator(hr, m_hWnd, m_iIndex, rectCur, NULL);
	}

	if (1)
	{
		if (m_pMultiAllocator)
			m_pMultiAllocator->BindGraphic();

		WCHAR* pWidCharPath;
		for (int x = 0; x < strVideoAry.GetCount() && m_iIndex == 0; x++)
		{
			pMediaControl = NULL;
			//RenderFileToVMR9(g_GraphBuildAry.at(x), AsWideString(strVideoAry.GetAt(x)), NULL);
			pWidCharPath = AsWideString(strVideoAry.GetAt(x));
			FAIL_RET(g_GraphBuildAry.at(x)->RenderFile(pWidCharPath, NULL));
			FAIL_RET(g_GraphBuildAry.at(x)->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&pMediaControl)));
			//pMediaControl->Run();
			delete pWidCharPath;
		}

		//if (m_iIndex > 0)
		//		FAIL_RET(pBasicAudio->put_Volume(-10000));
	}

	filterConfig = NULL;
	pGraphBuild = NULL;
	pBaseFilter = NULL;
	pMediaControl = NULL;
	pBasicAudio = NULL;
	pMediaSeeking = NULL;

    return hr;
}

HRESULT CPanel::ShowVideo(CStringArray &strVideoAry)
{
	HRESULT     hr = S_OK;
	CloseGraph(m_hWnd);

    IVMRFilterConfig9* filterConfig;

	if (0)
	{
		ICreateDevEnum * pDevEnum = NULL;
		IEnumMoniker * pEnum = NULL;
		IMoniker *pMoniker = NULL;
		FAIL_RET(CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pDevEnum));
		FAIL_RET(pDevEnum->CreateClassEnumerator(AM_KSCATEGORY_RENDER, &pEnum, 0));

		IPropertyBag * pPropBag;
		VARIANT varName;
		while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
		{
			FAIL_RET(pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag));
			hr = (pPropBag->Read(L"Description", &varName, 0));
			VariantClear(&varName);
			hr = (pPropBag->Read(L"FriendlyName", &varName, 0));
			VariantClear(&varName);	
		}
	}
	else if (0)
	{
		//GrabSamples(strVideoAry.GetAt(0).AllocSysString());
	} 

	FAIL_RET(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_GraphBuild));
	//FAIL_RET( m_GraphBuild.CoCreateInstance(CLSID_FilterGraph));
	FAIL_RET( m_GraphBuild->QueryInterface(IID_IBasicAudio, (void **)&m_pBasicAudio));

	//FAIL_RET( m_BaseFilter.CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC_SERVER));
	FAIL_RET(CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&m_BaseFilter));
	FAIL_RET( m_BaseFilter->QueryInterface(IID_IVMRFilterConfig9, reinterpret_cast<void**>(&filterConfig)));
	FAIL_RET( filterConfig->SetNumberOfStreams(strVideoAry.GetCount()));
	
	FAIL_RET( m_GraphBuild->QueryInterface(IID_IMediaSeeking,  (void **)&m_pMediaSeeking));
	FAIL_RET(m_BaseFilter->QueryInterface(IID_IVMRMixerControl9, (void**)&m_pMixCtrl));

	FAIL_RET(filterConfig->SetRenderingMode(VMR9Mode_Renderless));
	FAIL_RET(SetAllocatorPresenter(m_BaseFilter, m_hWnd, m_pMediaSeeking, m_iIndex));

	FAIL_RET( m_GraphBuild->AddFilter(m_BaseFilter, L"Video Mixing Renderer 9"));

	if (m_pMixCtrl)
	{
		VMR9NormalizedRect rTemp;
		INT iCount = strVideoAry.GetCount();

		for (int x = 0; x < iCount; x++)
		{			
			rTemp.top = 0;

			if (x == 0)
			{
				rTemp.left = 0;
				rTemp.right = 1;
				rTemp.bottom = 1.f;
				FAIL_RET( m_pMixCtrl->SetAlpha(x, 1.0f));
			}
			else
			{
				rTemp.left = 1.f/iCount*x;
				rTemp.right = 1.f/iCount*(x + 1);
				rTemp.bottom = 1.f/iCount*(x + 1);
				FAIL_RET( m_pMixCtrl->SetAlpha(x, 1.0f*x/iCount));
			}
			m_pMixCtrl->SetOutputRect(x, &rTemp);
		}
		//m_pMixCtrl->SetZOrder(x, 1);
	}

	WCHAR *pWidCharPath;
	for (int x = 0; x < strVideoAry.GetCount(); x++)
	{
		pWidCharPath = AsWideString(strVideoAry.GetAt(x));
		FAIL_RET( m_GraphBuild->RenderFile(pWidCharPath, NULL ));
		//RenderFileToVMR9(m_GraphBuild, AsWideString(strVideoAry.GetAt(x)), m_BaseFilter);
		delete pWidCharPath;
	}

	FAIL_RET( m_GraphBuild->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&m_MediaControl)));

	g_MediaControlAry.resize(1);
	g_MediaSeekingAry.resize(1);
	g_MediaControlAry.at(0) = m_MediaControl;
	g_MediaSeekingAry.at(0) = m_pMediaSeeking;
	g_MixCtrlAry.resize(1);
	g_MixCtrlAry.at(0) = m_pMixCtrl;
	g_BaseFilterAry.resize(1);
	g_BaseFilterAry.at(0) = m_BaseFilter;
	g_GraphBuildAry.resize(1);
	g_GraphBuildAry.at(0) = m_GraphBuild;


	

	//m_GraphBuild->SetDefaultSyncSource();
	//long lWidth, lHeight;
	//m_WindowCtrl->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL); 

	//FAIL_RET( g_mediaControl->Run());

	/*if (FILTER_NUM == 1)
		FAIL_RET( SetAllocatorPresenter( m_BaseFilter, m_hWnd, m_pMediaSeeking, m_iIndex));
	else
		FAIL_RET( SetAllocatorPresenter( m_BaseFilter, m_hWnd, m_pMediaSeeking, x));*/

	if (m_iIndex > 0)
		FAIL_RET(m_pBasicAudio->put_Volume(-10000));

	filterConfig->Release();

	if (0)
	{		
		LONGLONG lCurPos, lLength;
		m_pMediaSeeking->GetCurrentPosition(&lCurPos);
		m_pMediaSeeking->GetStopPosition(&lLength);
		lCurPos = lLength/4;
		hr = m_pMediaSeeking->SetPositions(&lCurPos, AM_SEEKING_AbsolutePositioning, &lLength, AM_SEEKING_AbsolutePositioning);
	}
	


    return hr;
}

HRESULT CPanel::GetVideoSize(CString strAVPath, LONG &iWidth, LONG &iHeight)
{
	HRESULT     hr = S_OK;
	CloseGraph(m_hWnd);
    IVMRFilterConfig9* filterConfig;
	IBasicVideo		*pBV;

   FAIL_RET( m_GraphBuild.CoCreateInstance(CLSID_FilterGraph) );
	FAIL_RET( m_BaseFilter.CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC_SERVER) );
	FAIL_RET( m_BaseFilter->QueryInterface(IID_IVMRFilterConfig9, reinterpret_cast<void**>(&filterConfig)));
	FAIL_RET( filterConfig->SetRenderingMode( VMR9Mode_Windowless ) );
	FAIL_RET( filterConfig->SetNumberOfStreams(1) );
	FAIL_RET( m_GraphBuild->AddFilter(m_BaseFilter, L"Video Mixing Renderer 9") );    
	FAIL_RET( m_GraphBuild->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&m_MediaControl)) );
	WCHAR* pWCharPath = AsWideString(strAVPath);
	FAIL_RET( m_GraphBuild->RenderFile(pWCharPath, NULL ) );
	delete pWCharPath;

	FAIL_RET( m_GraphBuild->QueryInterface(IID_IBasicVideo, (void **)&pBV));

	pBV->GetVideoSize(&iWidth, &iHeight);
	pBV->Release();
	filterConfig->Release();

	CloseGraph(m_hWnd);
	return hr;
}

void CPanel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此加入您的訊息處理常式程式碼
	// 不要呼叫圖片訊息的 CDialog::OnPaint()

	CClientDC cdc(this);
	CBrush brhBlack(RGB(0, 0, 0));
	CBrush brhGray(RGB(200, 200, 200));
	RECT rectCur;
	GetClientRect(&rectCur);

	if (m_BlendSetting.iStatus == 1)
	{
		if (m_iBlendPanelIdx != -1)
		{
			CBrush brshWhite(RGB(255, 255, 255));
			CBrush brshBlack(RGB(0, 0, 0));
			CBrush brshRed(RGB(255, 0, 0));
			CBrush brshGreen(RGB(0, 255, 0));
			CPen penWhite(PS_SOLID, 1, RGB(255, 255, 255));
			CRect rectTemp;
			INT32 iTempVal;

			m_MemoryDC->FillRect(&m_rectClient, &brshBlack);

			for (int z = 0; z < 4; z++)
			{
				rectTemp = m_rectClient;

				if (z == 0) rectTemp.right = rectTemp.left + GetRefBlendLength(TRUE);
				else if (z == 1) rectTemp.bottom = rectTemp.top + GetRefBlendLength(FALSE);
				else if (z == 2) rectTemp.left = rectTemp.right - GetRefBlendLength(TRUE);
				else if (z == 3) rectTemp.top = rectTemp.bottom - GetRefBlendLength(FALSE);

				if (m_BlendSetting.bBlendArea[m_iBlendPanelIdx][z])
				{
					m_MemoryDC->FrameRect(rectTemp, &brshRed);
				}
			}

			HGDIOBJ pOldPen;
			pOldPen = m_MemoryDC->SelectObject(penWhite);
			for (int z = 1; z < m_BlendSetting.uGridH; z++)
			{
				iTempVal = m_rectClient.left + m_rectClient.Width() * z / m_BlendSetting.uGridH;
				m_MemoryDC->MoveTo(iTempVal, m_rectClient.top);
				m_MemoryDC->LineTo(iTempVal, m_rectClient.bottom);
			}
			for (int z = 1; z < m_BlendSetting.uGridV; z++)
			{
				iTempVal = m_rectClient.top + m_rectClient.Height()	* z / m_BlendSetting.uGridV;
				m_MemoryDC->MoveTo(m_rectClient.left, iTempVal);
				m_MemoryDC->LineTo(m_rectClient.right, iTempVal);
			}

			m_MemoryDC->SelectObject(pOldPen);
			m_MemoryDC->FrameRect(&m_rectClient, &brshGreen);

			cdc.BitBlt(0, 0, m_rectClient.Width() + 1, m_rectClient.Height() + 1, m_MemoryDC, 0, 0, SRCCOPY);
		}
	}
	else if (m_BlendSetting.iStatus == 2)
	{
		CPen penWhite(PS_SOLID, 2, RGB(255, 255, 255));
		CPen penRed(PS_SOLID, 2, RGB(255, 0, 0));
		CPen penGreen(PS_SOLID, 2, RGB(0, 255, 0));
		CBrush brshBlack(RGB(0, 0, 0));
		HGDIOBJ pOldPen;

		m_MemoryDC->FillRect(&m_rectClient, &brshBlack);
		pOldPen = m_MemoryDC->SelectObject(penWhite);
		for (int x = 0; x <= m_BlendSetting.uGridV; x++)
		{
			if (x == 0 || x == m_BlendSetting.uGridV)//m_pt2DGrid[x + 1].IsEmpty())
				m_MemoryDC->SelectObject(penGreen);
			else
				m_MemoryDC->SelectObject(penWhite);

			for (int y = 1; y <= m_BlendSetting.uGridH; y++) //m_pt2DGrid[x].GetCount()
			{
				m_MemoryDC->MoveTo(GetBlendPt(FALSE, x, y - 1)); //m_pt2DGrid[x].GetAt(y - 1)
				m_MemoryDC->LineTo(GetBlendPt(FALSE, x, y)); //m_pt2DGrid[x].GetAt(y)
			}

			if (x > 0)
			{
				for (int y = 0; y <= m_BlendSetting.uGridH; y++) //m_pt2DGrid[x].GetCount()
				{
					if (y == 0 || y == m_BlendSetting.uGridH) //m_pt2DGrid[x].GetCount()
						m_MemoryDC->SelectObject(penGreen);
					else
						m_MemoryDC->SelectObject(penWhite);

					m_MemoryDC->MoveTo(GetBlendPt(FALSE, x - 1, y)); //m_pt2DGrid[x - 1].GetAt(y)
					m_MemoryDC->LineTo(GetBlendPt(FALSE, x, y)); //m_pt2DGrid[x].GetAt(y)
				}
			}
		}

		m_MemoryDC->SelectObject(penRed);
		INT32 iBlendPtCount;
		for (int x = 0; x < 4; x++)
		{
			iBlendPtCount = GetBlendPtCount(TRUE, x); //m_ptBlendArea[x].GetCount()
			if (IsBlendArea(x))
			{
				for (int y = 1; y < iBlendPtCount; y++)
				{
					m_MemoryDC->MoveTo(GetBlendPt(TRUE, x, y - 1));// m_ptBlendArea[x].GetAt(y - 1));
					m_MemoryDC->LineTo(GetBlendPt(TRUE, x, y));// m_ptBlendArea[x].GetAt(y));
				}

			}
		}
		m_MemoryDC->SelectObject(pOldPen);

		cdc.BitBlt(0, 0, m_rectClient.Width() + 1, m_rectClient.Height() + 1, m_MemoryDC, 0, 0, SRCCOPY);
	}
	else if (m_iIndex == g_PannelSetting.iSelIdx && m_iIndex > 0)
		cdc.FillRect(&rectCur, &brhGray);
	else
		cdc.FillRect(&rectCur, &brhBlack);
}

void CPanel::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if (m_iIndex > 0)
	{
		if (!(nFlags & DEF_NOTIFY_MSG))
			return;
		else
			nFlags &= ~DEF_NOTIFY_MSG;
	}

#ifdef _ENABLE_CWND
	CWnd::OnMouseMove(nFlags, point);
#else
	CDialog::OnMouseMove(nFlags, point);
#endif
	CClientDC cdc(this);
	CString strTemp;

	if (m_iIndex <= 0)
	{
#ifdef _ENABLE_CWND
		m_pParent->SendMessage(WM_MOUSEMOVE, 0, MAKELPARAM(point.x, point.y));
#else
		GetParent()->SendMessage(WM_MOUSEMOVE, 0, MAKELPARAM(point.x, point.y));
#endif
	}

	if (m_BlendSetting.iStatus == 2)
	{
		CPoint ptTemp;
		if (m_ptClickPos.x < 0)
		{
			CString strTemp;
			HCURSOR hCursor;

			for (int y = 0; y < 100; y++)
			for (int x = 0; x <= m_BlendSetting.uGridH; x++)
			{
				ptTemp = GetBlendPt(FALSE, y, x);//m_pt2DGrid[x].GetAt(y);
				if (abs(point.x - ptTemp.x) < 5 && abs(point.y - ptTemp.y) < 5)
				{
					if (nFlags != MK_LBUTTON)
					{
						hCursor = AfxGetApp()->LoadCursor(IDC_HAND_OPEN);

						if (GetCursor() != hCursor)
							SetCursor(hCursor);
					}

					break;
				}
			}
		}
		else
		{
			ModifyPtByMouse(point);			
		}
		g_PannelAry[m_iBlendPanelIdx + 1]->OnPaint();
		OnPaint();
	}
	else if (m_pMultiAllocator && m_pMultiAllocator->HadInited() && !m_bReleaseFocus)
		m_pMultiAllocator->OnMouseMove(m_iIndex, nFlags, point);
	return;

	LONGLONG lCurPos;
	for (UINT x = 0; x < g_MediaSeekingAry.size(); x++)
	{
		g_MediaSeekingAry.at(x)->GetCurrentPosition(&lCurPos);
		strTemp.Format("Moive %i, Pos: %i", x, lCurPos);
		cdc.TextOutA(50, 150 + 50*x, strTemp);
	}

	strTemp.Format("Fixed Times: %i", m_pMultiAllocator->m_iFixTimes);
	cdc.TextOutA(250, 50, strTemp);

	strTemp.Format("Status: %x, NumPass: %i", m_pMultiAllocator->m_iDeviceStatus, m_pMultiAllocator->m_iNumPasses);
	cdc.TextOutA(250, 100, strTemp);
}

void CPanel::ModifyPtByMouse(CPoint point)
{
	CPoint ptSelOffset = point - m_ptClickPos;
	CPoint ptTemp;

	if (m_ptSelBlend == CPoint(0, 0) || m_ptSelBlend == CPoint(0, m_BlendSetting.uGridV) ||
		m_ptSelBlend == CPoint(m_BlendSetting.uGridH, 0) ||
		m_ptSelBlend == CPoint(m_BlendSetting.uGridH, m_BlendSetting.uGridV)) //Corner
	{
		for (int x = 0; x <= m_BlendSetting.uGridH; x++)
		{
			ptTemp = GetBlendPt(FALSE, m_ptSelBlend.y, x, FALSE);
			ptTemp.x += double(ptSelOffset.x) * (m_BlendSetting.uGridH - abs(x - m_ptSelBlend.x)) / m_BlendSetting.uGridH;
			ptTemp.y += double(ptSelOffset.y) * (m_BlendSetting.uGridH - abs(x - m_ptSelBlend.x)) / m_BlendSetting.uGridH;
			SetBlendPt(FALSE, m_ptSelBlend.y, x, TRUE, ptTemp);
		}

		for (int y = 0; y <= m_BlendSetting.uGridV; y++)
		{
			ptTemp = GetBlendPt(FALSE, y, m_ptSelBlend.x, FALSE);
			ptTemp.x += double(ptSelOffset.x) * (m_BlendSetting.uGridV - abs(y - m_ptSelBlend.y)) / m_BlendSetting.uGridV;
			ptTemp.y += double(ptSelOffset.y) * (m_BlendSetting.uGridV - abs(y - m_ptSelBlend.y)) / m_BlendSetting.uGridV;
			SetBlendPt(FALSE, y, m_ptSelBlend.x, TRUE, ptTemp);
		}

		for (int y = 1; y < m_BlendSetting.uGridV; y++)
		{
			ptSelOffset = GetBlendPt(FALSE, y, m_BlendSetting.uGridH) -
				GetBlendPt(FALSE, y, m_BlendSetting.uGridH, FALSE) +
				GetBlendPt(FALSE, y, 0) - GetBlendPt(FALSE, y, 0, FALSE);

			for (int x = 1; x < m_BlendSetting.uGridH; x++)
			{
				ptTemp = GetBlendPt(FALSE, y, x, FALSE);
				ptTemp.x += double(ptSelOffset.x) * (m_BlendSetting.uGridH - abs(x - m_ptSelBlend.x)) / m_BlendSetting.uGridH;
				ptTemp.y += double(ptSelOffset.y) * (m_BlendSetting.uGridH - abs(x - m_ptSelBlend.x)) / m_BlendSetting.uGridH;
				SetBlendPt(FALSE, y, x, TRUE, ptTemp);
			}
		}

		for (int x = 0; x < 4; x++)
		{
			INT32 iBlendPtCount = GetBlendPtCount(TRUE, x); //m_ptBlendArea[x].GetCount()
			if (IsBlendArea(x))
			{
				if (x == 0 || x == 2)
				{
					for (int y = 0; y < iBlendPtCount; y++)
					{
						ptSelOffset = GetBlendPt(FALSE, y, m_BlendSetting.uGridH) - GetBlendPt(FALSE, y, 0);

						ptTemp = GetBlendPt(TRUE, x, y, FALSE);
						ptTemp.y = double(ptSelOffset.y) * ptTemp.x / 1920 + GetBlendPt(FALSE, y, 0).y;

						if (m_bModifyBlendArea)
							ptTemp.x = double(ptSelOffset.x) * ptTemp.x / 1920 + GetBlendPt(FALSE, y, 0).x;

						SetBlendPt(TRUE, x, y, TRUE, ptTemp);
					}
				}
				else
				{
					for (int y = 0; y < iBlendPtCount; y++)
					{
						ptSelOffset = GetBlendPt(FALSE, m_BlendSetting.uGridV, y) - GetBlendPt(FALSE, 0, y);

						ptTemp = GetBlendPt(TRUE, x, y, FALSE);
						ptTemp.x = double(ptSelOffset.x) * ptTemp.y / 1080 + GetBlendPt(FALSE, 0, y).x;

						if (m_bModifyBlendArea)
							ptTemp.y = double(ptSelOffset.y) * ptTemp.y / 1080 + GetBlendPt(FALSE, 0, y).y;

						SetBlendPt(TRUE, x, y, TRUE, ptTemp);
					}
				}
			}
		}
	}
	else
	{
	
		ptTemp = GetBlendPt(FALSE, m_ptSelBlend.y, m_ptSelBlend.x, FALSE);
		ptTemp.x += double(ptSelOffset.x) * (m_BlendSetting.uGridH - abs(0 - m_ptSelBlend.x)) / m_BlendSetting.uGridH;
		ptTemp.y += double(ptSelOffset.y) * (m_BlendSetting.uGridH - abs(0 - m_ptSelBlend.x)) / m_BlendSetting.uGridH;
		SetBlendPt(FALSE, m_ptSelBlend.y, m_ptSelBlend.x, TRUE, ptTemp);
		/*
		for (int y = 0; y <= m_BlendSetting.uGridV; y++)
		for (int x = 0; x <= m_BlendSetting.uGridH; x++)
		{
			ptTemp = GetBlendPt(FALSE, y, x, TRUE);
			//SetBlendPt(FALSE, m_ptSelBlend.y, m_ptSelBlend.x, TRUE, ptTemp);
			SetBlendPt(FALSE, y, x, TRUE, ptTemp);
		}
	*/
	}
	
}

HRESULT CPanel::AddBaseFilter(IGraphBuilder *pGraphBuild, IBaseFilter* pBaseFilter, HWND window, int iIndex)
{
	if(!pGraphBuild || !pBaseFilter)
    {
        return E_FAIL;
    }

    HRESULT hr;

	// create our surface allocator

	if (m_pMultiAllocator == NULL)
	{
		CRect rectCur;
		GetClientRect(&rectCur);
		m_pMultiAllocator = new CAllocator(hr, window, iIndex, rectCur, NULL);
	}

	INT iCurSize = INT(g_BaseFilterAry.size());
	g_BaseFilterAry.resize(iCurSize + 1);
	g_BaseFilterAry.at(iCurSize) = pBaseFilter;

	g_GraphBuildAry.resize(iCurSize + 1);
	g_GraphBuildAry.at(iCurSize) = pGraphBuild;
    
    return hr;
}


HRESULT CPanel::SetAllocatorPresenter( CComPtr<IBaseFilter> filter, HWND window, IMediaSeeking *pMS, int iIndex)
{
    if( filter == NULL )
    {
        return E_FAIL;
    }

    HRESULT hr;

    IVMRSurfaceAllocatorNotify9* lpIVMRSurfAllocNotify;
    FAIL_RET( filter->QueryInterface(IID_IVMRSurfaceAllocatorNotify9, reinterpret_cast<void**>(&lpIVMRSurfAllocNotify)) );

	// create our surface allocator

	if (m_allocator == NULL)
	{
		CRect rectCur;
		GetClientRect(&rectCur);
		m_allocator.Attach(new CAllocator( hr, window, iIndex, rectCur, pMS));
		if( FAILED( hr ) )
		{
			m_allocator = NULL;
			return hr;
		}
		// let the allocator and the notify know about each other
		FAIL_RET( lpIVMRSurfAllocNotify->AdviseSurfaceAllocator( 0xACDCACDC + iIndex, m_allocator));

	}

	FAIL_RET( m_allocator->AdviseNotify(lpIVMRSurfAllocNotify) );
	lpIVMRSurfAllocNotify->Release();
    
    return hr;
}

HRESULT CPanel::CloseGraph(HWND window)
{
	if (m_pShapeDlg)
	{
		KillTimer(0);
		m_pShapeDlg->SendMessage(WM_CLOSE);
	}

	if( m_MediaControl != NULL ) 
	{
		OAFilterState state;
		do {
			m_MediaControl->Stop();
			m_MediaControl->GetState(0, & state );
		} while( state != State_Stopped );
		m_MediaControl = NULL;
	}

	if (m_pBasicAudio)
	{
		m_pBasicAudio->Release();
		m_pBasicAudio = NULL;

	}

	if (m_pMediaSeeking)
	{
		m_pMediaSeeking->Release();
		m_pMediaSeeking = NULL;
	}

	if (m_pMixCtrl)
	{
		m_pMixCtrl->Release();
		m_pMixCtrl = NULL;
	}
	
	FreezeDisplay(FALSE);

	if (m_pMultiAllocator)
	{
		m_pMultiAllocator->Release();
		m_pMultiAllocator = NULL;
	}

	m_allocator    = NULL;        
	m_MediaControl = NULL;
	m_BaseFilter   = NULL;
	m_GraphBuild   = NULL;

	::InvalidateRect( window, NULL, true );
    return S_OK;
}


HRESULT CPanel::RenderFileToVMR9(IGraphBuilder *pGB, WCHAR *wFileName, 
                         IBaseFilter *pRenderer, BOOL bRenderAudio)
{
    HRESULT hr=S_OK;
    CComPtr <IPin> pOutputPin;
	CComPtr <IPin> pInputPin;
    CComPtr <IBaseFilter> pSource;
    CComPtr <IBaseFilter> pAudioRenderer;
    CComPtr <IFilterGraph2> pFG;

    // Add a file source filter for this media file
    // Add the source filter to the graph
    if (FAILED(hr = pGB->AddSourceFilter(wFileName, L"SOURCE", &pSource)))
    {
        return hr;
    }

    // Get the interface for the first unconnected output pin
    FAIL_RET(GetUnconnectedPin(pSource, PINDIR_OUTPUT, &pOutputPin));

    // Render audio if requested (defaults to TRUE)
    if (bRenderAudio)
    {
        // Because we will be rendering with the RENDERTOEXISTINGRENDERERS flag,
        // we need to create an audio renderer and add it to the graph.  
        // Create an instance of the DirectSound renderer (for each media file).
        //
        // Note that if the system has no sound card (or if the card is disabled),
        // then creating the DirectShow renderer will fail.  In that case,
        // handle the failure quietly.
        if (SUCCEEDED(CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER, 
                                       IID_IBaseFilter, (void **)&pAudioRenderer)))
        {
            // The audio renderer was successfully created, so add it to the graph
            FAIL_RET(pGB->AddFilter(pAudioRenderer, L"Audio Renderer"));
        }
    }

    // Get an IFilterGraph2 interface to assist in building the
    // multifile graph with the non-default VMR9 renderer
    FAIL_RET(pGB->QueryInterface(IID_IFilterGraph2, (void **)&pFG));

    // Render the output pin, using the VMR9 as the specified renderer.  This is 
    // necessary in case the GraphBuilder needs to insert a Color Space convertor,
    // or if multiple filters insist on using multiple allocators.
    // The audio renderer will also be used, if the media file contains audio.

    FAIL_RET(pFG->RenderEx(pOutputPin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, NULL));

	if (pRenderer)
	{
		AM_MEDIA_TYPE pmt;
		FindPinOnFilter(pRenderer, PINDIR_INPUT, 0, TRUE, &pInputPin);
		pOutputPin->ConnectionMediaType(&pmt);
		FAIL_RET(pFG->ConnectDirect(pOutputPin, pInputPin, &pmt));
	}

    // If this media file does not contain an audio stream, then the 
    // audio renderer that we created will be unconnected.  If left in the 
    // graph, it could interfere with rate changes and timing.
    // Therefore, if the audio renderer is unconnected, remove it from the graph.
    if (pAudioRenderer != NULL)
    {
        IPin *pUnconnectedPin=0;

        // Is the audio renderer's input pin connected?
        HRESULT hrPin = GetUnconnectedPin(pAudioRenderer, PINDIR_INPUT, &pUnconnectedPin);

        // If there is an unconnected pin, then remove the unused filter
        if (SUCCEEDED(hrPin) && (pUnconnectedPin != NULL))
        {
            // Release the returned IPin interface
            pUnconnectedPin->Release();

            // Remove the audio renderer from the graph
            hrPin = pGB->RemoveFilter(pAudioRenderer);
        }
    }
    return hr;
}

HRESULT CPanel::GetUnconnectedPin(
    IBaseFilter *pFilter,   // Pointer to the filter.
    PIN_DIRECTION PinDir,   // Direction of the pin to find.
    IPin **ppPin)           // Receives a pointer to the pin.
{
    IEnumPins *pEnum = 0;
    IPin *pPin = 0;

    if (!ppPin)
        return E_POINTER;
    *ppPin = 0;

    // Get a pin enumerator
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr))
        return hr;

    // Look for the first unconnected pin
    while (pEnum->Next(1, &pPin, NULL) == S_OK)
    {
        PIN_DIRECTION ThisPinDir;

        pPin->QueryDirection(&ThisPinDir);
        if (ThisPinDir == PinDir)
        {
            IPin *pTmp = 0;

            hr = pPin->ConnectedTo(&pTmp);
            if (SUCCEEDED(hr))  // Already connected, not the pin we want.
            {
                pTmp->Release();
            }
            else  // Unconnected, this is the pin we want.
            {
                pEnum->Release();
                *ppPin = pPin;
                return S_OK;
            }
        }
        pPin->Release();
    }

    // Release the enumerator
    pEnum->Release();

    // Did not find a matching pin
    return E_FAIL;
}

HRESULT CPanel::FindPinOnFilter( IBaseFilter * pFilter, PIN_DIRECTION PinDir,
                        DWORD dwPin, BOOL fConnected, IPin ** ppPin )
{
    HRESULT         hr = S_OK;
    IEnumPins *     pEnumPin = NULL;
    IPin *          pConnectedPin = NULL;
    PIN_DIRECTION   PinDirection;
    ULONG           ulFetched;
    DWORD           nFound = 0;

    ASSERT( pFilter != NULL );
    if (!pFilter || !ppPin)
        return E_POINTER;
        
    *ppPin = NULL;

    // Get a pin enumerator for the filter's pins
    hr = pFilter->EnumPins( &pEnumPin );
    if(SUCCEEDED(hr))
    {
        while ( S_OK == ( hr = pEnumPin->Next( 1L, ppPin, &ulFetched ) ) )
        {
            hr = (*ppPin)->ConnectedTo( &pConnectedPin );
            if (pConnectedPin)
            {
                pConnectedPin->Release();
                pConnectedPin = NULL;
            }

            if ( ( ( VFW_E_NOT_CONNECTED == hr ) && !fConnected ) ||
                 ( ( S_OK                == hr ) &&  fConnected ) )
            {
                hr = (*ppPin)->QueryDirection( &PinDirection );
                if ( ( S_OK == hr ) && ( PinDirection == PinDir ) )
                {
                    if ( nFound == dwPin ) 
                        break;
                    nFound++;
                }
            }
            (*ppPin)->Release();
        }

        // Release enumerator
        pEnumPin->Release();
    }

    return hr;   
}


HRESULT CPanel::GetNumberConnectedPins( DWORD& number )
{
    if( m_BaseFilter == NULL )
    {
        return E_FAIL;
    }
    number = 0;

    CComPtr<IEnumPins> pPE;
    HRESULT hr = m_BaseFilter->EnumPins( & pPE );
    if( SUCCEEDED( hr ) )
    {
        hr = S_OK;
        CComPtr<IPin> pPin;
        while( hr == S_OK )
        {
            pPin = NULL;

            hr = pPE->Next( 1, &pPin, 0 );
            if( hr == S_OK )
            {
                CComPtr<IPin> pPinConnected;
                hr = pPin->ConnectedTo(& pPinConnected );
                if( SUCCEEDED( hr ) )
                {
                    if( pPinConnected != NULL )
                    {
                        number++;
                    }
                }
                else if( hr == VFW_E_NOT_CONNECTED ) // reset hr if the pPin is just telling us that
                {   //it's not connected.
                    hr = S_OK;
                }
            }
        }
    }
    return hr;
}
BOOL CPanel::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別

	CString strTemp;
	strTemp.Format("Media Server Dlg, %x, %x, %x", message, wParam, lParam);
	theApp.SaveLog(strTemp);

	/*if (message == WM_MOUSEMOVE || message == WM_LBUTTONDOWN || message == WM_LBUTTONUP)
	{
		if (m_iIndex == 0 && m_BlendSetting.iStatus == 2)
		{
			g_PannelAry[g_PannelSetting.iSelIdx]->SendMessage(message | DEF_NOTIFY_MSG, wParam, lParam);
		}
	}
	else*/ 

#if 0
	if (message == WM_CLOSE)
		CloseGraph(m_hWnd);
#else
	switch (message)
	{
		case WM_CLOSE:
			CloseGraph(m_hWnd);
			break;
		case WM_SYSCOMMAND:
		{
			if (SC_CLOSE == wParam)
				return true;
			else if (SC_KEYMENU == wParam)
				return true;
		}
		break;
	}
#endif

#ifdef _ENABLE_CWND
	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
#else
	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
#endif
}

void CPanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if (m_iIndex > 0)
	{
		if (!(nFlags & DEF_NOTIFY_MSG))
			return;
		else
			nFlags &= ~DEF_NOTIFY_MSG;
	}

	if (m_BlendSetting.iStatus == 2)
	{
		CPoint ptTemp;
		CString strTemp;
		for (int x = 0; x < 100; x++)
		for (int y = 0; y <= m_BlendSetting.uGridH; y++) //m_pt2DGrid[x].GetCount()
		{
			ptTemp = GetBlendPt(FALSE, x, y);//m_pt2DGrid[x].GetAt(y);
			if (abs(point.x - ptTemp.x) < 5 && abs(point.y - ptTemp.y) < 5)
			{
				m_ptSelBlend = CPoint(y, x);
				m_ptClickPos = point;
				//strTemp.Format("Panel: %i, (%i, %i)", m_iBlendPanelIdx, y, x);
				//AfxMessageBox(strTemp);

				HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_HAND_CLOSE);

				if (GetCursor() != hCursor)
					SetCursor(hCursor);

				SetCapture();
				x = 100;
				break;
			}
		}		
	}
	else if (m_pMultiAllocator && m_pMultiAllocator->HadInited() && !m_bReleaseFocus)
		m_pMultiAllocator->OnMouseMove(m_iIndex, nFlags, point, MOUSE_LBUT_DOWN);
	return;

#ifdef _ENABLE_CWND
	CWnd::OnLButtonDown(nFlags, point);
#else
	CDialog::OnLButtonDown(nFlags, point);
#endif
}

void CPanel::SlaveMsg(UINT nFlags, CPoint point, MOUSE_EVT nEventIdx)
{ 
	if (m_pMultiAllocator && m_pMultiAllocator->HadInited())
		m_pMultiAllocator->OnMouseMove(m_iIndex, nFlags, point, nEventIdx);
}

void CPanel::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if (m_iIndex > 0)
	{
		if (!(nFlags & DEF_NOTIFY_MSG))
			return;
		else
			nFlags &= ~DEF_NOTIFY_MSG;
	}

	if (m_ptClickPos.x >= 0)
	{
		m_ptSelBlend = CPoint(-1, -1);
		m_ptClickPos = CPoint(-1, -1);
		ReleaseCapture();
		SetBlendPtFromTemp();
	}

	if (m_pMultiAllocator && m_pMultiAllocator->HadInited() && !m_bReleaseFocus)
		m_pMultiAllocator->OnMouseMove(m_iIndex, nFlags, point, MOUSE_LBTN_UP);

#ifdef _ENABLE_CWND
	CWnd::OnLButtonUp(nFlags, point);
#else
	CDialog::OnLButtonUp(nFlags, point);
#endif
}

void CPanel::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (nChar == 0x52)  //R
		RecordAVI();
	else if (nChar == 0x53)  //S
		StopAVI();

#ifdef _ENABLE_CWND
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
#else
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
#endif
}


void CPanel::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	CClientDC cdc(this);
	CString strTemp;
	do
	{
		if (m_iIndex > 0)
			break;
		else
		{
				//strTemp.Format("%c", nChar);
				//cdc.TextOutA(50, 200, strTemp);
				CPoint ptMove;

				if (nChar == 37)
					ptMove = CPoint(-nRepCnt, 0);
				else if (nChar == 38)
					ptMove = CPoint(0, -nRepCnt);
				else if (nChar == 39)
					ptMove = CPoint(nRepCnt, 0);
				else if (nChar == 40)
					ptMove = CPoint(0, nRepCnt);
				else if (nChar == 13)
					ptMove = CPoint(0, 0);
				else if (nChar == 17)
				{
					if (m_bShapeUI && m_pShapeDlg)
					{
						if (m_pShapeDlg->IsWindowVisible())
						{
							m_bShowUI = FALSE;
							m_pShapeDlg->ShowWindow(SW_HIDE);
						}
						else
						{
							m_bShowUI = TRUE;
							m_pShapeDlg->ShowWindow(SW_SHOW);
						}

						SetFocus();
					}

					break;
				}
				else if (nChar == 27)  //Esc
				{
#ifdef _ENABLE_CWND
					if (m_iIndex == 0)
					{
						char szMsg[256];
						sprintf(szMsg, "#MS_KeyUp# CPanel::OnKeyUp [%d]\n", m_iIndex);
						OutputDebugStringA(szMsg);

						for (int x = 1; x < g_PannelAry.GetCount(); x++)
						{
							//g_PannelAry[x]->ShowWindow(SW_SHOW);
							
							SetTopWin(g_PannelAry[x]->GetSafeHwnd(), true);
						}

						//m_pShapeDlg->ShowWindow(SW_HIDE);
						m_pParent->SendMessage(MSG_MAINUI);
						ShowWindow(SW_HIDE);
						//		SetFocus();
					}
//					CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
					return;
#endif
				}
				else
					break;

				if (m_pMultiAllocator && m_pMultiAllocator->HadInited() && !m_bReleaseFocus)
					m_pMultiAllocator->OnMouseMove(m_iIndex, 0, ptMove, MOUSE_KEY_MOVE);
		}

	} while (false);

#ifdef _ENABLE_CWND
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
#else
	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
#endif
}

void CPanel::OnCancel()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別

	if (m_iIndex == 0)
	{
		//m_pShapeDlg->ShowWindow(SW_HIDE);
		GetParent()->SendMessage(MSG_MAINUI);
		ShowWindow(SW_HIDE);
//		SetFocus();
	}
	return;

#ifdef _ENABLE_CWND
	//CWnd::OnCancel();
#else
	CDialog::OnCancel();
#endif
}

void CPanel::DestroyD3D(INT32 iIndex)
{
	if (iIndex == -1)
	{
		for (int x = 0; x < MAX_D3DNUM; x++)
		{
			m_D3DDev[x]->Release();
			m_D3DDev[x] = NULL;

			m_D3D[x]->Release();
			m_D3D[x] = NULL;

			m_bD3DPoolFlag[x] = FALSE;
		}
	}
	else
	{
		m_D3DDev[iIndex]->Release();
		m_D3DDev[iIndex] = NULL;

		m_D3D[iIndex]->Release();
		m_D3D[iIndex] = NULL;

		m_bD3DPoolFlag[iIndex] = FALSE;
	}
}

BOOL CPanel::CreateD3DDevice()
{
	for (int x = 0; x < MAX_D3DNUM; x++)
	{
		HRESULT hr;
		m_D3DDev[x] = NULL;
		D3DDISPLAYMODE dm;

		m_D3D[x] = Direct3DCreate9(D3D_SDK_VERSION);
		if (m_D3D[x] == NULL)
			return FALSE;

		hr = m_D3D[x]->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);
		D3DPRESENT_PARAMETERS pp;
		ZeroMemory(&pp, sizeof(pp));
		pp.BackBufferWidth = 1920;
		pp.BackBufferHeight = 1080;
		pp.Windowed = TRUE;
		pp.hDeviceWindow = m_hWnd;
//		pp.SwapEffect = D3DSWAPEFFECT_COPY;
		pp.BackBufferFormat = dm.Format;
		pp.EnableAutoDepthStencil = TRUE;//--For3D
		pp.AutoDepthStencilFormat = D3DFMT_D16;//--For3D

		pp.MultiSampleQuality = 0;
		pp.Flags = D3DPRESENTFLAG_VIDEO;// 0;
		pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
//		pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		//For Seperate
		pp.MultiSampleType = D3DMULTISAMPLE_NONE;
		pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;// D3DPRESENT_INTERVAL_DEFAULT;//D3DPRESENT_INTERVAL_ONE;
		pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		pp.BackBufferFormat = D3DFMT_UNKNOWN;
		pp.BackBufferCount = 1;

		V(m_D3D[x]->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			m_hWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING |
			D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
			&pp,
			&m_D3DDev[x]));

		V(m_D3DDev[x]->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
		V(m_D3DDev[x]->SetRenderState(D3DRS_LIGHTING, FALSE));
		V(m_D3DDev[x]->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));
		V(m_D3DDev[x]->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
		V(m_D3DDev[x]->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));
		V(m_D3DDev[x]->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE));
		V(m_D3DDev[x]->SetRenderState(D3DRS_ALPHAREF, 0x10));
		V(m_D3DDev[x]->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER));
		//= m_D3DDev->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD));

		V(m_D3DDev[x]->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
		V(m_D3DDev[x]->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));
		V(m_D3DDev[x]->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
		V(m_D3DDev[x]->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
		V(m_D3DDev[x]->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR));

		m_bD3DPoolFlag[x] = FALSE;
	}

	return TRUE;
}

void CPanel::RecordAVI()
{
	float fFPS = 30.0;
	int iChannelLayout = 0,iSampleRate = 0,iNBSamples = 0;
	int iCurChannelLayout, iCurSampleRate, iCurNBSamples;

	m_bDoRecordAVI = true;
	
	for (int x = 0; x < g_MediaStreamAry.size(); x++)
	{
		if (g_MediaStreamAry.at(x)->IsOpened())
		{
			g_MediaStreamAry.at(x)->SpeedDown();
			g_MediaStreamAry.at(x)->SpeedDown();
			g_MediaStreamAry.at(x)->Seek(0);
			g_MediaStreamAry.at(x)->EnableReSampleRate(false);

			if (x == 0)
				fFPS = g_MediaStreamAry.at(x)->GetFPS();

			int iRet = g_MediaStreamAry.at(x)->GetAudioInfo(iCurChannelLayout, iCurSampleRate, iCurNBSamples);
			if (iRet && iCurChannelLayout != 0)
			{
				iChannelLayout = iCurChannelLayout;
				iSampleRate = iCurSampleRate;
				iNBSamples = iCurNBSamples;
			}
		}
	}

	auto service = ServiceProvider::Instance()->GetTimelineService();
	if (service != nullptr)
	{
		auto engine = service->GetTimeCodeEngine();
		engine->SetInterval(80);
		engine->ResetTimeCode();
	}
	
	DisplayManager* pDisplayManager = m_pMultiAllocator->GetDisplayManager();
	pDisplayManager->SetPanelHWND(m_hWnd);
	pDisplayManager->EnableRecordAVI(m_bDoRecordAVI, fFPS, iChannelLayout, iSampleRate, iNBSamples);
}

void CPanel::StopAVI()
{
	if (m_bDoRecordAVI)
	{
		float fFPS = 30.0;
		m_bDoRecordAVI = false;

		for (int x = 0; x < g_MediaStreamAry.size(); x++)
		{
			g_MediaStreamAry.at(x)->SpeedUp();
			g_MediaStreamAry.at(x)->SpeedUp();
			g_MediaStreamAry.at(x)->EnableReSampleRate(true);

			if (x == 0)
				fFPS = g_MediaStreamAry.at(x)->GetFPS();
		}

		auto service = ServiceProvider::Instance()->GetTimelineService();
		if (service != nullptr)
		{
			auto engine = service->GetTimeCodeEngine();
			engine->SetInterval(20);
		}

		DisplayManager* pDisplayManager = m_pMultiAllocator->GetDisplayManager();
		pDisplayManager->EnableRecordAVI(m_bDoRecordAVI, fFPS, 0, 0, 0);
	}
}

LRESULT CPanel::OnStopAVI(WPARAM wParam, LPARAM lParam)
{
	StopAVI();
	return true;
}

//#ifdef _ENABLE_GPU
DisplayManager* CPanel::GetDisplayManager()
{
	return m_pMultiAllocator->GetDisplayManager();
}
//#endif

#ifdef _ENABLE_CWND

BOOL CPanel::RegisterWindowClass(char* szClassName)
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();
	wchar_t wszClassName[256];
	wcscpy(wszClassName, ANSIToUnicode(szClassName));

	//#if (_USE_UNICODE == 1)
#ifdef _UNICODE
	if (!(::GetClassInfo(hInst, wszClassName, &wndcls)))
#else
	if (!(::GetClassInfo(hInst, szClassName, &wndcls)))
#endif
	{
		// otherwise we need to register a new class
		wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc = ::DefWindowProc;
		wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
		wndcls.hInstance = hInst;
		wndcls.hIcon = NULL;
		wndcls.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
		wndcls.lpszMenuName = NULL;

		//#if (_USE_UNICODE == 1)
#ifdef _UNICODE
		wndcls.lpszClassName = wszClassName;
#else
		wndcls.lpszClassName = szClassName;
#endif

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

void CPanel::Create()
{
	char szPanelWinName[512];
	char szPanelClassName[512];

	RECT rect = { 0,0,10,10 };
	sprintf(szPanelWinName, "");
	sprintf(szPanelClassName, "PanelClass_%d", GetTickCount());

	RegisterWindowClass(szPanelClassName);

	BOOL bRet = CWnd::CreateEx(WS_EX_TOOLWINDOW,szPanelClassName, szPanelWinName, WS_POPUP | WS_VISIBLE, rect, NULL, 0,0);
}

void CPanel::Init()
{
	if (m_bShapeUI)
	{
		m_pShapeDlg = new CShapeUIDlg();
		m_pShapeDlg->Create(IDD_TEMP_DLG, this);
#ifdef _ENABLE_GPU
		//m_pShapeDlg->ShowWindow(SW_SHOW);
		m_pShapeDlg->ShowWindow(SW_HIDE);
		//		m_pShapeDlg->SetForegroundWindow();
		//m_pShapeDlg->SetWindowPos(this, 0, 0, 1920, 1080, SWP_SHOWWINDOW);
		m_pShapeDlg->SetWindowPos(this, 0, 0, 1920, 1080, SWP_HIDEWINDOW);
		//		m_pShapeDlg->BringWindowToTop();
#else
		//m_pShapeDlg->ShowWindow(SW_SHOW);
		m_pShapeDlg->ShowWindow(SW_HIDE);
		m_pShapeDlg->SetForegroundWindow();
		//m_pShapeDlg->SetWindowPos(this, 0, 0, 1920, 1080, SWP_SHOWWINDOW);
		m_pShapeDlg->SetWindowPos(this, 0, 0, 1920, 1080, SWP_HIDEWINDOW);
		m_pShapeDlg->BringWindowToTop();
#endif
		SetTimer(0, 500, NULL);
	}
}

#else
BOOL CPanel::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化
	if (m_bShapeUI)
	{
		m_pShapeDlg = new CShapeUIDlg();
		m_pShapeDlg->Create(IDD_TEMP_DLG, this);
#ifdef _ENABLE_GPU
		//m_pShapeDlg->ShowWindow(SW_SHOW);
		m_pShapeDlg->ShowWindow(SW_HIDE);
//		m_pShapeDlg->SetForegroundWindow();
		//m_pShapeDlg->SetWindowPos(this, 0, 0, 1920, 1080, SWP_SHOWWINDOW);
		m_pShapeDlg->SetWindowPos(this, 0, 0, 1920, 1080, SWP_HIDEWINDOW);
//		m_pShapeDlg->BringWindowToTop();
#else
		//m_pShapeDlg->ShowWindow(SW_SHOW);
		m_pShapeDlg->ShowWindow(SW_HIDE);
		m_pShapeDlg->SetForegroundWindow();
		//m_pShapeDlg->SetWindowPos(this, 0, 0, 1920, 1080, SWP_SHOWWINDOW);
		m_pShapeDlg->SetWindowPos(this, 0, 0, 1920, 1080, SWP_HIDEWINDOW);
		m_pShapeDlg->BringWindowToTop();
#endif
		SetTimer(0, 500, NULL);
	}

//	CreateD3DDevice();	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}
#endif

void CPanel::OnShowWindow(BOOL bShow, UINT nStatus)
{
#ifdef _ENABLE_CWND
	CWnd::OnShowWindow(bShow, nStatus);
#else
	CDialog::OnShowWindow(bShow, nStatus);
#endif
																	 
	// TODO: 在此加入您的訊息處理常式程式碼
	if (m_pShapeDlg)
	{
		m_bShowUI = bShow;
		m_pShapeDlg->ShowWindow(bShow);
	}
}

void CPanel::CreateBlendObj(BOOL bCreateObject)
{
	STREM_MAPPING StreamMap;
	CString strObjPath, strVideoPath;

	for (UINT y = 0; y < g_MediaStreamAry.size(); y++)
	{
		strVideoPath = g_MediaStreamAry.at(y)->FilePath();
		if (!strVideoPath.IsEmpty() && g_MediaStreamAry[y]->IsOpened())
		{
			StreamMap.iMediaIdx = y;
			//StreamMap.iMediaIdx += DEF_GLOBAL_MEDIA;
			strVideoPath = "";
			StreamMap.strVideoPath = strVideoPath;
			StreamMap.iWidth = g_MediaStreamAry.at(StreamMap.iMediaIdx)->GetVideoWidth();
			StreamMap.iHeight = g_MediaStreamAry.at(StreamMap.iMediaIdx)->GetVideoHeight();
		}
	}

	if (bCreateObject)
	{
		CString strFilePath, strData;
		CStdioFile sfFile;
		CPoint ptTemp;
		CRect rectUV, rectTemp;
		INT32 iIndex;
		double fRatio = double(m_rectClient.Width()) / m_rectClient.Height();
		for (int x = 1; x < g_PannelAry.GetCount(); x++)
		{
			m_iBlendPanelIdx = x - 1;
			strFilePath.Format("%s\\Obj\\2DBlend%i.obj", theApp.m_strCurPath, x);
			sfFile.Open(strFilePath, CFile::modeCreate | CFile::modeReadWrite);

			for (int y = 0; y <= m_BlendSetting.uGridV; y++)
			{
				for (int z = 0; z <= m_BlendSetting.uGridH; z++)
				{
					ptTemp = GetBlendPt(FALSE, y, z);
					strData.Format("v %.4f %.4f %.4f\n", 20.9*(-1.0f + 2.0f * double(ptTemp.x) / m_rectClient.Width()),
						20.9 / fRatio*(1.6f - 2.0f*double(ptTemp.y) / m_rectClient.Height()), 0.0f);

					sfFile.WriteString(strData);
				}
			}

			rectUV = m_BlendSetting.rectDispAry[x - 1];

			for (int y = 0; y <= m_BlendSetting.uGridV; y++)
			for (int z = 0; z <= m_BlendSetting.uGridH; z++)
			{
				ptTemp.x = rectUV.left + rectUV.Width() * z / m_BlendSetting.uGridH;
				ptTemp.y = rectUV.top + rectUV.Height()	* y / m_BlendSetting.uGridV;

				strData.Format("vt %.4f %.4f 0.0000\n", double(ptTemp.x) / StreamMap.iWidth,
					1.0 - double(ptTemp.y) / StreamMap.iHeight, 0.0f);

				sfFile.WriteString(strData);
			}

			sfFile.WriteString("vn 0.0000 0.0000 1.0000\n");

			iIndex = 0;
			for (int y = 0; y < m_BlendSetting.uGridV; y++)
			for (int z = 0; z < m_BlendSetting.uGridH; z++)
			{
				iIndex = y * (m_BlendSetting.uGridV + 1) + z + 1;
				strData.Format("f %i/%i/1 %i/%i/1 %i/%i/1 %i/%i/1\n",
					iIndex, iIndex, iIndex + 1, iIndex + 1,
					iIndex + m_BlendSetting.uGridV + 2,
					iIndex + m_BlendSetting.uGridV + 2,
					iIndex + m_BlendSetting.uGridV + 1,
					iIndex + m_BlendSetting.uGridV + 1);

				sfFile.WriteString(strData);
			}
			sfFile.Close();
		}
	}

	UINT iCount = 0;
	for (int x = 1; x < g_PannelAry.GetCount(); x++)
	{
		m_iBlendPanelIdx = x - 1;

		g_PannelAry[x]->AddBlendPt(1, CPoint(0, 0), 0, FALSE);

		for (int y = 0; y < 4; y++)
		{
			iCount = GetBlendPtCount(TRUE, y);
			if (IsBlendArea(y))
			{
				g_PannelAry[x]->AddBlendPt(0, GetBlendPt(TRUE, y, iCount - 1), y, FALSE);

				if (y == 0)
				{
					for (int z = 0; z <= m_BlendSetting.uGridV; z++)
					{
						g_PannelAry[x]->AddBlendPt(0, GetBlendPt(FALSE, 
							m_BlendSetting.uGridV - z, 0, FALSE), y, FALSE);
					}
				}
				else if (y == 1)
				{
					for (int z = 0; z <= m_BlendSetting.uGridH; z++)
					{
						g_PannelAry[x]->AddBlendPt(0, GetBlendPt(FALSE, 0,
							m_BlendSetting.uGridH - z, FALSE), y, FALSE);
					}
				}
				else if (y == 2)
				{
					for (int z = 0; z <= m_BlendSetting.uGridV; z++)
					{
						g_PannelAry[x]->AddBlendPt(0, GetBlendPt(FALSE, m_BlendSetting.uGridV - z,
							m_BlendSetting.uGridH, FALSE), y, FALSE);
					}
				}
				else if (y == 3)
				{
					for (int z = 0; z <= m_BlendSetting.uGridH; z++)
					{
						g_PannelAry[x]->AddBlendPt(0, GetBlendPt(FALSE, m_BlendSetting.uGridV,
							m_BlendSetting.uGridH - z, FALSE), y, FALSE);
					}
				}

				for (int z = 0; z < iCount; z++)
				{
					g_PannelAry[x]->AddBlendPt(0, GetBlendPt(TRUE, y, z), y, FALSE);
				}
			}
		}

		g_PannelAry[x]->AddBlendPt(2, CPoint(0, 0), 0, FALSE);
	}

	for (int x = 1; x < g_PannelAry.GetCount(); x++)
	{
		g_PannelSetting.iSelIdx = x;
		strObjPath.Format("%s\\Obj\\2DBlend%i.obj", theApp.m_strCurPath, x);
		StreamMap.iPanelIdx = x;
		StreamMap.iObjIdx = g_PannelAry[x]->GetObjCount();
		StreamMap.strObjPath = strObjPath;

		g_StreamMappingAry.Add(StreamMap);
		if (g_PannelAry.GetSize() > 1)
			g_PannelAry[0]->ShowTempVideo(strObjPath, "", 100.0f);

		g_PannelAry[x]->ShowTempVideo(strObjPath, strVideoPath, 100.0f, FALSE);


		INT32 iObjNum = g_PannelAry[x]->GetObjCount();
		m_pShapeDlg->SetModalNum(iObjNum, TRUE);

		theApp.SaveShapeProject(theApp.m_strCurPath + "Mapping.xml");

		g_PannelAry[0]->SaveObjectSetting();
	}

	m_pShapeDlg->CreateDefultGama();
	m_BlendSetting.iStatus = 0;

}

void CPanel::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if (m_BlendSetting.iStatus == 2)
	{
		CPanelBlendDlg dlgPanelBlend;

		if (dlgPanelBlend.DoModal() == IDOK)
		{
			INT32 iStatus = dlgPanelBlend.GetFinishStatus();

			if (iStatus == 0)
			{
				m_bModifyBlendArea = !dlgPanelBlend.IsLockBlendArea();

				g_PannelSetting.iSelIdx = dlgPanelBlend.GetPanelIdx();
				if (m_iBlendPanelIdx != g_PannelSetting.iSelIdx - 1)
				{
					m_iBlendPanelIdx = g_PannelSetting.iSelIdx - 1;
					OnPaint();
				}
			}
			else
			{
				CreateBlendObj(iStatus == 2);				
			}
		}
	}
	if (m_pMultiAllocator && m_pMultiAllocator->HadInited() && !m_bReleaseFocus)
		m_pMultiAllocator->OnMouseMove(m_iIndex, nFlags, point, MOUSE_RBTN_UP);

#ifdef _ENABLE_CWND
	CWnd::OnRButtonUp(nFlags, point);
#else
	CDialog::OnRButtonUp(nFlags, point);
#endif
}

void CPanel::OnOK()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別

	return;

#ifdef _ENABLE_CWND
	//
#else
	CDialog::OnOK();
#endif
}

void CPanel::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (nIDEvent == 0)
	{
		if (m_BlendSetting.iStatus == 2)
		{
			if (m_pShapeDlg->IsWindowVisible())
			{
				m_pShapeDlg->ShowWindow(SW_HIDE);

				if (m_iBlendPanelIdx != g_PannelSetting.iSelIdx - 1)
				{
					m_iBlendPanelIdx = g_PannelSetting.iSelIdx - 1;
					OnPaint();
				}
			}
		}
		else if (!IsWindowVisible() && m_pShapeDlg->IsWindowVisible() && !m_bShowUI)
		{
			m_pShapeDlg->ShowWindow(SW_HIDE);
		}
		else if (IsWindowVisible() && !m_pShapeDlg->IsWindowVisible() && m_bShowUI)
		{
			m_pShapeDlg->ShowWindow(SW_SHOW);
		}


		if (m_pShapeDlg->IsWindowVisible() && !m_bReleaseFocus)
		{
			SetFocus();
		}

		if (m_iIndex == 0)
		{
			CRect rect;
			GetClientRect(&rect);
			ClipCursor(rect);
		}
	}

#ifdef _ENABLE_CWND
	CWnd::OnTimer(nIDEvent);
#else
	CDialog::OnTimer(nIDEvent);
#endif
}

void CPanel::SetBlendSetting(BLEND_SETTING bsBlendSetting, BOOL bFinalSetting)
{
	m_BlendSetting = bsBlendSetting;


	//if (m_pMultiAllocator && !bFinalSetting)
	//	m_pMultiAllocator->SetBlendSetting(bsBlendSetting);

	if (m_iIndex == 0)
	{
		m_bModifyBlendArea = TRUE;
		m_iBlendPanelIdx = g_PannelSetting.iSelIdx - 1;
	}
	else
	{
		for (int x = 0; x < 4; x++)
		{
			if (m_BlendSetting.iPanelIdxAry[x] == m_iIndex)
			{
				m_iBlendPanelIdx = x;
				break;
			}
		}
	}


	if (bFinalSetting && m_iIndex != 0)
	{
		CPoint ptTemp;

		for (int x = 0; x < 100; x++)
		{
			m_pt2DGrid[x].RemoveAll();
			m_pt2DGridTemp[x].RemoveAll();
		}

		for (int y = 0; y <= m_BlendSetting.uGridV; y++)
		for (int x = 0; x <= m_BlendSetting.uGridH; x++)
		{
			ptTemp.x = m_rectClient.left + m_rectClient.Width() * x / m_BlendSetting.uGridH;
			ptTemp.y = m_rectClient.top + m_rectClient.Height()	* y / m_BlendSetting.uGridV;

			if (y == m_BlendSetting.uGridV)
				ptTemp.y -= 1;

			if (x == m_BlendSetting.uGridH)
				ptTemp.x -= 1;

			m_pt2DGrid[y].Add(ptTemp);
			m_pt2DGridTemp[y].Add(ptTemp);
		}

		CRect rectTemp = m_rectClient;

		for (int x = 0; x < 4; x++)
		{
			m_ptBlendArea[x].RemoveAll();
			m_ptBlendAreaTemp[x].RemoveAll();

			if (IsBlendArea(x))
			{
				if (x == 0) ptTemp.x = GetRefBlendLength(TRUE);
				else if (x == 1) ptTemp.y = GetRefBlendLength(FALSE);
				else if (x == 2) ptTemp.x = m_rectClient.Width() - GetRefBlendLength(TRUE);
				else if (x == 3) ptTemp.y = m_rectClient.Height() - GetRefBlendLength(FALSE);

				if (x == 0 || x == 2)
				{
					for (int y = 0; y <= m_BlendSetting.uGridV; y++)
					{
						ptTemp.y = m_rectClient.top + m_rectClient.Height()	* y / m_BlendSetting.uGridV;
						m_ptBlendArea[x].Add(ptTemp);
						m_ptBlendAreaTemp[x].Add(ptTemp);
					}
				}
				else
				{
					for (int y = 0; y <= m_BlendSetting.uGridH; y++)
					{
						ptTemp.x = m_rectClient.left + m_rectClient.Width() * y / m_BlendSetting.uGridH;
						m_ptBlendArea[x].Add(ptTemp);
						m_ptBlendAreaTemp[x].Add(ptTemp);
					}
				}
			}
		}
	}

	OnPaint();
}

BOOL CPanel::DestroyWindow()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
#ifdef _ENABLE_CWND
	return CWnd::DestroyWindow();
#else
	return CDialog::DestroyWindow();
#endif
}

void CPanel::SetBlendPtFromTemp()
{
	if (m_iIndex == 0)
		g_PannelAry[m_iBlendPanelIdx + 1]->SetBlendPtFromTemp();
	else
	{
		for (int y = 0; y <= m_BlendSetting.uGridV; y++)
		for (int x = 0; x <= m_BlendSetting.uGridH; x++)
		{
			m_pt2DGrid[y].GetAt(x) = m_pt2DGridTemp[y].GetAt(x);
		}
	}
}

void CPanel::SetBlendPt(BOOL bBlendArea, UINT iArea, UINT iIndex, BOOL bTemp, CPoint ptNew)
{
	if (m_iIndex == 0)
		g_PannelAry[m_iBlendPanelIdx + 1]->SetBlendPt(bBlendArea, iArea, iIndex, bTemp, ptNew);
	else if (!bBlendArea)
	{
		if (iArea < 100 && iIndex < m_pt2DGrid[iArea].GetCount())
		{
			if (bTemp)
				m_pt2DGridTemp[iArea].GetAt(iIndex) = ptNew;
			else
				m_pt2DGrid[iArea].GetAt(iIndex) = ptNew;
		}
	}
	else
	{
		if (iArea < 4 && iIndex < m_ptBlendArea[iArea].GetCount())
		{
			if (bTemp)
				m_ptBlendAreaTemp[iArea].GetAt(iIndex) = ptNew;
			else
				m_ptBlendArea[iArea].GetAt(iIndex) = ptNew;
		}
	}

	//OnPaint();
}

CPoint CPanel::GetBlendPt(BOOL bBlendArea, UINT iArea, UINT iIndex, BOOL bTemp)
{
	CPoint ptRet(0, 0);

	if (m_iIndex == 0)
		ptRet = g_PannelAry[m_iBlendPanelIdx + 1]->GetBlendPt(bBlendArea, iArea, iIndex, bTemp);
	else if (!bBlendArea)
	{
		if (iArea < 100 && iIndex < m_pt2DGrid[iArea].GetCount())
		{
			if (bTemp)
				ptRet = m_pt2DGridTemp[iArea].GetAt(iIndex);
			else
				ptRet = m_pt2DGrid[iArea].GetAt(iIndex);

		}
	}
	else
	{
		if (iArea < 4 && iIndex < m_ptBlendArea[iArea].GetCount())
		{
			if (bTemp)
				ptRet = m_ptBlendAreaTemp[iArea].GetAt(iIndex);
			else
				ptRet = m_ptBlendArea[iArea].GetAt(iIndex);
		}
	}

	return ptRet;
}

UINT CPanel::GetBlendLineNum()
{
	UINT iCount;

	if (m_iIndex == 0)
		iCount = g_PannelAry[m_iBlendPanelIdx + 1]->GetBlendLineNum();
	else
	{
		for (int x = 0; x < 100; x++)
		{
			if (m_pt2DGrid[x].GetCount() == 0)
			{
				iCount = x;
				break;
			}
		}		
	}

	return iCount;

}

UINT CPanel::GetBlendPtCount(BOOL bBlendArea, UINT iArea)
{
	UINT iCount;

	if (m_iIndex == 0)
		iCount = g_PannelAry[m_iBlendPanelIdx + 1]->GetBlendPtCount(bBlendArea, iArea);
	else if (!bBlendArea)
	{
		if (iArea < 100)
			iCount = m_pt2DGrid[iArea].GetCount();
	}
	else
	{
		if (iArea < 4)
			iCount = m_ptBlendArea[iArea].GetCount();
	}

	return iCount;
}


BOOL CPanel::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
#ifdef _ENABLE_CWND
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
#else
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
#endif
}

void CPanel::DisplayVideo_VW(void* pSurface, int w, int h, bool bDoPresent)
{
	if(m_pMultiAllocator)
		m_pMultiAllocator->DisplayVideo_VW(pSurface,w,h, bDoPresent);
}

void CPanel::DisplayVideo_VW(int iIndex,void* pSurface, int w, int h)
{
	if (m_pMultiAllocator)
		m_pMultiAllocator->DisplayVideo_VW(iIndex,pSurface, w, h);
}

void CPanel::DisplayVideo_HDEffect(int iIndex, void* pSurface, int w, int h)
{
	if (m_pMultiAllocator)
		m_pMultiAllocator->DisplayVideo_HDEffect(iIndex, pSurface, w, h);
}

void CPanel::ApplyMask()
{
	if (m_pMultiAllocator)
		m_pMultiAllocator->ApplyMask();
}

void CPanel::Flip()
{
	if (m_pMultiAllocator)
		m_pMultiAllocator->Flip();
}

int CPanel::GetLastFrameCount()
{
	if (m_pMultiAllocator)
		return m_pMultiAllocator->GetLastFrameCount();
	return 0;
}

DWORD CPanel::GetLastPresentTime()
{
	if (m_pMultiAllocator)
		return m_pMultiAllocator->GetLastPresentTime();
	return 0;
}

float CPanel::GetAvgPresentFPS()
{
	if (m_pMultiAllocator)
		return m_pMultiAllocator->GetAvgPresentFPS();
	return 0;
}

void CPanel::EnableDoClear(bool bEnable)
{
	if (m_pMultiAllocator)
		m_pMultiAllocator->EnableDoClear(bEnable);
}

void CPanel::CheckEventStatus()
{
	if (m_pMultiAllocator)
		m_pMultiAllocator->CheckEventStatus();
}

void CPanel::SetSyncPBCtrl(SyncPBCtrl* pObj)
{
	if (m_pMultiAllocator)
		m_pMultiAllocator->SetSyncPBCtrl(pObj);
}

void CPanel::ApplySetting()
{
	if (m_pMultiAllocator)
		m_pMultiAllocator->ApplySetting();
}

void CPanel::SetTip()
{
	m_pShapeDlg->SetTip();
}
