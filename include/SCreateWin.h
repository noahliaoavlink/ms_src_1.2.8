#ifndef SCreateWin_H
#define SCreateWin_H

#include <stdio.h>

#ifdef _WIN64 
#define GCL_HBRBACKGROUND GCLP_HBRBACKGROUND 
#define GCL_HICON GCLP_HICON
#endif

static WNDCLASSA	wc;
static HWND hWin;
static HBRUSH g_hBrush = 0;
static char g_szClassName[256];
static char* SetRegisterInfo(WNDPROC pWndProc,char* str)
{
	static char szClass[256];
	SYSTEMTIME sys;
	GetSystemTime(&sys);
	int systime = sys.wHour * 360000 + sys.wMinute * 60000 + sys.wSecond * 1000 + sys.wMilliseconds;
	sprintf(szClass,"%s%d",str,systime);
	ZeroMemory(&wc,sizeof(wc));
    // Set up and register window class
    wc.style = CS_OWNDC | CS_DBLCLKS;
    wc.lpfnWndProc = pWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = NULL;//hInstance;
    wc.hIcon = NULL;
	wc.hCursor = 0;//LoadCursor( NULL, IDC_ARROW);
    wc.hbrBackground = NULL;//CreateSolidBrush(RGB(255,255,223));
    wc.lpszMenuName = (LPSTR)NULL;
    wc.lpszClassName = szClass;
	return szClass;
}

static char* SetRegisterInfoEx(WNDPROC pWndProc,char* str)
{
	static char szClass[256];
//	SYSTEMTIME sys;
	//GetSystemTime(&sys);
	//int systime = sys.wHour * 10000 + sys.wMinute * 100 + sys.wSecond;
	sprintf(szClass,"%s",str);
	ZeroMemory(&wc,sizeof(wc));
    // Set up and register window class
    wc.style = CS_OWNDC | CS_DBLCLKS;
    wc.lpfnWndProc = pWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = NULL;//hInstance;
    wc.hIcon = NULL;
	wc.hCursor = 0;//LoadCursor( NULL, IDC_ARROW);
    wc.hbrBackground = NULL;//CreateSolidBrush(RGB(255,255,223));
    wc.lpszMenuName = (LPSTR)NULL;
    wc.lpszClassName = szClass;
	return szClass;
}

static HWND CreateWin(WNDPROC pWndProc,char* name)
{
	//if(hwnd == NULL)
	//{
	  //char* szClassName = (char*)new char[256]; 
	  strcpy(g_szClassName,SetRegisterInfoEx(pWndProc,name));
	  if(RegisterClassA(&wc)){
	       hWin = CreateWindowExA(
	   			   WS_EX_TOOLWINDOW,
				   g_szClassName,
                   NULL,
                   WS_POPUP | WS_VISIBLE, 
			       0, 
			       0, 
			       0, 
			       0,
                   NULL, 
                   NULL, 
                   NULL,//hInstance, 
                   NULL );
	  }
      else
	     hWin = NULL; 
	  //delete szClassName; 
	  return hWin;
	//}
}

static void CreateWin(HWND hwnd,WNDPROC pWndProc,char* name)
{
	if(hwnd == NULL)
	{
	  //char* szClassName = (char*)new char[256]; 
	  strcpy(g_szClassName,SetRegisterInfoEx(pWndProc,name));
	  if(RegisterClassA(&wc)){
	       hwnd = CreateWindowExA(
	   			   WS_EX_TOOLWINDOW,
				   g_szClassName,
                   NULL,
                   WS_POPUP | WS_VISIBLE, 
			       0, 
			       0, 
			       0, 
			       0,
                   NULL, 
                   NULL, 
                   NULL,//hInstance, 
                   NULL );
	  }
      else
	     hwnd = NULL; 
	  //delete szClassName; 
	}
}

static void CreateWinEx(HWND hwnd,WNDPROC pWndProc,char* name)
{
	if(hwnd == NULL)
	{
	  //char* szClassName = (char*)new char[256]; 
	  strcpy(g_szClassName,SetRegisterInfoEx(pWndProc,name));
	  if(RegisterClassA(&wc)){
	       hwnd = CreateWindowExA(
	   			   WS_EX_APPWINDOW,
				   g_szClassName,
                   NULL,
                   WS_POPUP | WS_VISIBLE, 
			       0, 
			       0, 
			       0, 
			       0,
                   NULL, 
                   NULL, 
                   NULL,//hInstance, 
                   NULL );
	  }
      else
	     hwnd = NULL; 
	//  delete szClassName; 
	}
}

static HWND CreateWinEx(WNDPROC pWndProc,char* name)
{
	//if(hwnd == NULL)
	//{
	  //char* szClassName = (char*)new char[256]; 
	  strcpy(g_szClassName,SetRegisterInfo(pWndProc,name));
	  if(RegisterClassA(&wc)){
	       hWin = CreateWindowExA(
	   			   WS_EX_APPWINDOW,
				   g_szClassName,
                   NULL,
                   WS_POPUP | WS_VISIBLE, 
			       0, 
			       0, 
			       0, 
			       0,
                   NULL, 
                   NULL, 
                   NULL,//hInstance, 
                   NULL );
	  }
      else
	     hWin = NULL; 
	  //delete szClassName;
	  return hWin; 
	//}
}

static HWND CreateWin1(WNDPROC pWndProc,char* name)
{
	//if(hwnd == NULL)
	//{
	  //char* szClassName = (char*)new char[256]; 
	  strcpy(g_szClassName,SetRegisterInfoEx(pWndProc,name));
	  if(RegisterClassA(&wc)){
	       hWin = CreateWindowExA(
	   			   WS_EX_TOOLWINDOW,
				   g_szClassName,
                   NULL,
                   WS_POPUP | WS_VISIBLE, 
			       0, 
			       0, 
			       0, 
			       0,
                   NULL, 
                   NULL, 
                   NULL,//hInstance, 
                   NULL );
	  }
      else
	     hWin = NULL; 
	  //delete szClassName;
	  return hWin; 
	//}
}

static void SetWinBKColor(HWND hwnd,unsigned char color[3])
{
	if(g_hBrush)
	{
	    DeleteObject(g_hBrush);
	    g_hBrush = 0;
	}
	
	g_hBrush = CreateSolidBrush(RGB(color[0],color[1],color[2]));
	SetClassLong(hwnd,GCL_HBRBACKGROUND,(long)g_hBrush);
}

static void SetWinBKColor(HWND hwnd,unsigned char r,unsigned char g,unsigned char b)
{
	if(g_hBrush)
	{
	    DeleteObject(g_hBrush);
	    g_hBrush = 0;
	}
	
	g_hBrush = CreateSolidBrush(RGB(r,g,b));
	SetClassLong(hwnd,GCL_HBRBACKGROUND,(long)g_hBrush);
}

static void SetWinBKColor(HWND hwnd,unsigned long ulColor)
{
	if(g_hBrush)
	{
	    DeleteObject(g_hBrush);
	    g_hBrush = 0;
	}
	
	g_hBrush = CreateSolidBrush(ulColor);
	SetClassLong(hwnd,GCL_HBRBACKGROUND,(long)g_hBrush);
}

/*
因g_hBrush會沒釋放所以，新增一個function
*/
static void SetWinBKColor(HWND hwnd,HBRUSH hBrush)
{
	SetClassLong(hwnd,GCL_HBRBACKGROUND,(long)hBrush);
}

static void SetWinSize(HWND hwnd,int width,int high,bool bShow = false)
{
	RECT rect;
	GetWindowRect(hwnd,&rect);

	if(bShow)
        SetWindowPos(hwnd,HWND_TOP,rect.left,rect.top,width,high,SWP_SHOWWINDOW);
	else
		SetWindowPos(hwnd,HWND_TOP,rect.left,rect.top,width,high,SWP_HIDEWINDOW);
}

static void SetWinParent(HWND hChild,HWND hParent)
{
	SetParent(hChild,hParent);
}

static void SetWinPos(HWND hwnd,int x,int y,bool bShow = false)
{
	RECT rect;
	GetWindowRect(hwnd,&rect);
    int iWidth = rect.right - rect.left;
	int iHigh = rect.bottom - rect.top;
	if(bShow)
        SetWindowPos(hwnd,HWND_TOP,x,y,iWidth,iHigh,SWP_SHOWWINDOW);
	else
		SetWindowPos(hwnd,HWND_TOP,x,y,iWidth,iHigh,SWP_HIDEWINDOW);
}

static void SetTopWin(HWND hwnd,bool bTop)
{
	RECT rect;
	GetWindowRect(hwnd,&rect);
  int iWidth = rect.right - rect.left;
	int iHigh = rect.bottom - rect.top;
	
	if(bTop)
	    SetWindowPos(hwnd,HWND_TOPMOST,rect.left,rect.top,iWidth,iHigh,SWP_SHOWWINDOW);
	else    
	    SetWindowPos(hwnd,HWND_TOP,rect.left,rect.top,iWidth,iHigh,SWP_SHOWWINDOW);
}

static void DestroyObjs()
{
	if(g_hBrush)
	{
	    DeleteObject(g_hBrush);
		g_hBrush = 0;
	}
}

static void SetWinIcon(HWND hWnd,HICON hIcon)
{
	SetClassLong(hWnd,GCL_HICON,(long)hIcon);
}

__inline void MoveToCenter(HWND hWnd)
{
	//將視窗的位置置中
	HDC hDC = ::GetDC(NULL);
	int iScreenWidth = GetDeviceCaps(hDC,HORZRES);
	int iScreenHeight = GetDeviceCaps(hDC,VERTRES);
	::ReleaseDC(NULL,hDC);

	RECT rect;
	GetWindowRect(hWnd,&rect);

	int iX = iScreenWidth / 2 - (rect.right - rect.left) / 2; 
	int iY = iScreenHeight / 2 - (rect.bottom - rect.top) / 2;
	MoveWindow(hWnd,iX,iY,rect.right - rect.left,rect.bottom - rect.top,true);
}
#endif