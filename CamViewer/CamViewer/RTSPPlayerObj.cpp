#include "stdafx.h"
#include "RTSPPlayerObj.h"
#include "../../Include/RenderCommon.h"
#include "../../Include/SCreateWin.h"

LRESULT WINAPI _RTSPDisplayWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

RTSPPlayerObj::RTSPPlayerObj()
{
	m_pRTSPPlayerDll = 0;
	m_pRTSPObj = 0;
	m_pOutVideoDll = 0;
}

RTSPPlayerObj::~RTSPPlayerObj()
{
	if (m_pRTSPPlayerDll)
	{
		if (m_pRTSPObj)
			m_pRTSPPlayerDll->_In_RTSP_Destroy(m_pRTSPObj);
		delete m_pRTSPPlayerDll;
	}

	if (m_pOutVideoDll)
		delete m_pOutVideoDll;
}

void RTSPPlayerObj::Init(HWND hWnd)
{
	RECT rect;
	char szWinName[256];

	m_pRTSPPlayerDll = new RTSPPlayerDll;
	m_pRTSPPlayerDll->LoadLib();
	m_pRTSPObj = m_pRTSPPlayerDll->_In_RTSP_Create();
	m_pRTSPPlayerDll->_In_RTSP_Init(m_pRTSPObj,this);
	m_pRTSPPlayerDll->_In_RTSP_SetOutputVideoFormat(m_pRTSPObj, VPF_I420);

	m_pOutVideoDll = new OutVideoDll;

	if (m_pOutVideoDll)
	{
		sprintf(szWinName, "RTSPPlayerWnd-%d", GetTickCount());
		m_hDisplayWnd = CreateWin(_RTSPDisplayWinProc, szWinName);

		SetParent(m_hDisplayWnd, hWnd);
		GetClientRect(hWnd, &rect);

		SetWinBKColor(m_hDisplayWnd, RGB(0, 0, 0));
		SetWinSize(m_hDisplayWnd, rect.right, rect.bottom, false);
		SetWinPos(m_hDisplayWnd, 0, 0, true);

		//out video
		bool bLoadOutVideoRet = m_pOutVideoDll->LoadLib();
		if (bLoadOutVideoRet)
			m_pOutVideoDll->Init(0, m_hDisplayWnd);
		else
			MessageBoxA(NULL, "Load OutVideo.dll failed!!", "Error", MB_OK | MB_TOPMOST);
	}
}

void RTSPPlayerObj::Open(char* szUrl)
{
	char szMsg[512];
	if (m_pRTSPPlayerDll)
	{
		m_pRTSPPlayerDll->_In_RTSP_Open(m_pRTSPObj, szUrl);
		m_pRTSPPlayerDll->_In_RTSP_Play(m_pRTSPObj);
		/*
		Sleep(1000);

		float fFPS = m_pRTSPPlayerDll->_In_RTSP_GetFPS(m_pRTSPObj);
		int iAudioCodecID = m_pRTSPPlayerDll->_In_RTSP_GetAudioCodecID(m_pRTSPObj);
		int iVideoCodecID = m_pRTSPPlayerDll->_In_RTSP_GetVideoCodecID(m_pRTSPObj);


		sprintf(szMsg, "RTSPPlayerObj::Open fps:%3.2f video:%d audio:%d\n", fFPS, iVideoCodecID, iAudioCodecID);
		OutputDebugStringA(szMsg);
		*/
	}
}

int RTSPPlayerObj::CheckNetworkStatus(char* szUrl)
{
	int iRet = -1;
	//_In_RTSP_CheckNetworkStatus
	if (m_pRTSPPlayerDll)
	{
		iRet = m_pRTSPPlayerDll->_In_RTSP_CheckNetworkStatus(m_pRTSPObj, szUrl);
		m_pRTSPPlayerDll->_In_RTSP_Stop(m_pRTSPObj);
		m_pRTSPPlayerDll->_In_RTSP_Close(m_pRTSPObj);
	}
	return iRet;
}

void RTSPPlayerObj::Close()
{
	if (m_pRTSPPlayerDll)
	{
		m_pRTSPPlayerDll->_In_RTSP_Stop(m_pRTSPObj);
		m_pRTSPPlayerDll->_In_RTSP_Close(m_pRTSPObj);
	}
}

void RTSPPlayerObj::UpdatePosInfo(char* szCurPos, char* szTotalLen)
{
	//gUpdatePosInfo(szCurPos, szTotalLen);
}

//total len
void RTSPPlayerObj::SetSliderCtrlRange(double dPos)
{
	//gSetSliderCtrlRange(dPos);
}

//cur pos
void RTSPPlayerObj::SetSliderCtrlPos(double dPos)
{
	//if (m_bEnableUpdatePos)
		//gSetSliderCtrlPos(dPos);
}

void RTSPPlayerObj::DisplayVideo(unsigned char* pBuffer, int w, int h)
{
	m_pOutVideoDll->RenderFrame2(0, VPF_I420, pBuffer, w, h);
	m_pOutVideoDll->Flip(0);
}

void RTSPPlayerObj::PlayAudio(unsigned char* pBuffer, int iLen)
{

}

void RTSPPlayerObj::Event(int iType)
{

}

void RTSPPlayerObj::DisplayVideo_HW(void* pD3DDevice, unsigned char* pBuffer, int w, int h)
{
}

LRESULT WINAPI _RTSPDisplayWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	switch (msg)
	{
	case WM_CLOSE:
	{
		DestroyWindow(hwnd);
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT	ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
	}
	break;
	case WM_CREATE:
		DragAcceptFiles(hwnd, TRUE);
		break;
	case WM_WINDOWPOSCHANGING:
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_RBUTTONUP:
		break;
	case WM_KEYDOWN:
	{
	}
	break;
	case WM_COPYDATA:
	{
	}
	break;
	case WM_DROPFILES:
	{
		/*
		char szFile[MAX_PATH];
		TCHAR lpszFile[MAX_PATH] = {0};
		UINT uFile = 0;
		HDROP hDrop = (HDROP)wParam;

		uFile = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NULL);

		lpszFile[0] = '\0';
		if (DragQueryFile(hDrop, 0, lpszFile, MAX_PATH))
		;
		DragFinish(hDrop);

		if(g_pUIController)
		g_pUIController->OpenFileW(lpszFile);
		*/
	}
	break;
	default:
	{
		result = DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	}
	}

	return result;
}