// NWin.cpp : implementation file
//

#include "stdafx.h"
//#include "testCWnd.h"
#include "NWin.h"

#include "../../../Include/StrConv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern wchar_t* ANSIToUnicode( const char* str );
/////////////////////////////////////////////////////////////////////////////
// CNWin

CNWin::CNWin()
{
	m_hBrush = 0;
	char szWindowName[256] = "";
	DWORD dwStyle = WS_POPUP | WS_VISIBLE;
	RECT rect = {0,0,0,0};

	m_iStyle = WT_NORMAL;
	
	strcpy(m_szClassName,"NWin");
	RegisterWindowClass(m_szClassName);

#ifdef _UNICODE
//#if (_USE_UNICODE == 1)
	wchar_t wszClassName[256];
	wchar_t wszWindowName[256];
	wcscpy(wszClassName,ANSIToUnicode(m_szClassName));
	wcscpy(wszWindowName,ANSIToUnicode(szWindowName));
	BOOL bRet = Create(wszClassName,wszWindowName,dwStyle,rect,0,0);
#else
	BOOL bRet = Create(m_szClassName,szWindowName,dwStyle,rect,0,0);
#endif
}

CNWin::CNWin(char* szClassName,int iStyle)
{
	m_hBrush = 0;
	strcpy(m_szClassName,szClassName);
	char szWindowName[256] = "";
	DWORD dwStyle = WS_POPUP | WS_VISIBLE;
	RECT rect = {0,0,0,0};

	m_iStyle = iStyle;

	RegisterWindowClass(m_szClassName);

//#if (_USE_UNICODE == 1)
#ifdef _UNICODE
	wchar_t wszClassName[256];
	wchar_t wszWindowName[256];
	wcscpy(wszClassName,ANSIToUnicode(m_szClassName));
	wcscpy(wszWindowName,ANSIToUnicode(szWindowName));
	BOOL bRet = Create(wszClassName,wszWindowName,dwStyle,rect,0,0);
#else
	BOOL bRet = Create(m_szClassName,szWindowName,dwStyle,rect,0,0);
#endif
}

CNWin::~CNWin()
{
//	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CNWin, CWnd)
	//{{AFX_MSG_MAP(CNWin)
	//ON_WM_LBUTTONDOWN()
	//ON_WM_RBUTTONDOWN()
	//ON_WM_KEYDOWN()
	//ON_WM_MOUSEMOVE()
	//ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CNWin::RegisterWindowClass(char* szClassName)
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();
	wchar_t wszClassName[256];
	wcscpy(wszClassName,ANSIToUnicode(szClassName));

//#if (_USE_UNICODE == 1)
#ifdef _UNICODE
	if (!(::GetClassInfo(hInst, wszClassName, &wndcls)))
#else
	if (!(::GetClassInfo(hInst, szClassName, &wndcls)))
#endif
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;

//#if (_USE_UNICODE == 1)
#ifdef _UNICODE
		wndcls.lpszClassName    = wszClassName;
#else
		wndcls.lpszClassName    = szClassName;
#endif

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNWin message handlers

void CNWin::SetBKColor(unsigned char r,unsigned char g,unsigned char b)
{
	if(m_hBrush)
	{
	    DeleteObject(m_hBrush);
	    m_hBrush = 0;
	}
	
	m_hBrush = CreateSolidBrush(RGB(r,g,b));
	DWORD dwRet = SetClassLongPtr(m_hWnd, GCL_HBRBACKGROUND, (long)m_hBrush);
}

void CNWin::SetWinSize(int width,int high,bool bShow)
{	
	RECT rect;
	::GetWindowRect(m_hWnd,&rect);

	if(bShow)
        ::SetWindowPos(m_hWnd,HWND_TOP,rect.left,rect.top,width,high,SWP_SHOWWINDOW);
	else
		::SetWindowPos(m_hWnd,HWND_TOP,rect.left,rect.top,width,high,SWP_HIDEWINDOW);
}

void CNWin::SetWinPos(int x,int y,bool bShow)
{
	RECT rect;
	::GetWindowRect(m_hWnd,&rect);
    int iWidth = rect.right - rect.left;
	int iHigh = rect.bottom - rect.top;
	if(bShow)
        ::SetWindowPos(m_hWnd,HWND_TOP,x,y,iWidth,iHigh,SWP_SHOWWINDOW);
	else
		::SetWindowPos(m_hWnd,HWND_TOP,x,y,iWidth,iHigh,SWP_HIDEWINDOW);
}

void CNWin::SetCallback(WinMsgCallback* pCallback)
{
	m_pCallback = pCallback;
}

void CNWin::SetTimer1(int iTimeOut)
{
	SetTimer(101,iTimeOut,NULL);
}

BOOL CNWin::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_pCallback = 0;
	LPVOID lpParam = NULL;
	if(m_iStyle == WT_EXT)
		return CWnd::CreateEx(WS_EX_APPWINDOW,lpszClassName, lpszWindowName, dwStyle,rect, pParentWnd,nID,lpParam);
	return CWnd::CreateEx(WS_EX_TOOLWINDOW,lpszClassName, lpszWindowName, dwStyle,rect, pParentWnd,nID,lpParam);
}

BOOL CNWin::DestroyWindow() 
{
//	MessageBox("","",MB_OK);
	return CWnd::DestroyWindow();
}

LRESULT CNWin::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	PAINTSTRUCT ps;
	switch(message)
	{
		case WM_PAINT:
		{
			CDC* pDC = BeginPaint(&ps);
			EndPaint(&ps);
		}
		break;
		case WM_KEYDOWN:
		case WM_KEYUP:
			/*
			{
				if (wParam == 27) //Esc
				{
					HWND hParent = ::GetParent(NULL);
					::PostMessage(hParent,WM_CANCELMODE,0,0);
				}
			}
			break;
			*/
		case WM_TIMER:
		case WM_HOTKEY:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_RBUTTONUP:
		case WM_CHAR:
		case WM_MOUSELEAVE:
		case WM_WINDOWPOSCHANGED:
			{
				if(m_pCallback)
					m_pCallback->WinMsg(message,(void*)wParam,(void*)lParam);
			}
			break;
		case WM_COPYDATA:
			{
				if(m_pCallback)
					m_pCallback->WinMsg(message,(void*)wParam,(void*)lParam);
			}
			break;
		case WM_SYSCOMMAND:
			{
				if(m_pCallback)
					return (long)m_pCallback->WinMsg(message,(void*)wParam,(void*)lParam);
			}
			break;
			/*
		case WM_TIMER:
			{
				TRACE("WM_TIMER..\n");
			}
			break;
			*/
	}
	return CWnd::WindowProc(message, wParam, lParam);
}

