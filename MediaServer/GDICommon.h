#ifndef _GDICommon_H_
#define _GDICommon_H_

__inline void DrawRect(HWND hWnd,HPEN hPen)
{
	RECT rect;
	HDC hDC = ::GetDC(hWnd);
	::GetClientRect(hWnd,&rect);
	HPEN hOldPen = (HPEN)SelectObject(hDC,hPen);

	MoveToEx(hDC,rect.left,rect.top,NULL);
	LineTo(hDC,rect.right - 1,rect.top);
	LineTo(hDC,rect.right - 1,rect.bottom - 1);
	LineTo(hDC,rect.left,rect.bottom - 1);
	LineTo(hDC,rect.left,rect.top);

	SelectObject(hDC,hOldPen);
	::ReleaseDC(hWnd,hDC);
}

__inline void DrawRect(HWND hWnd,HDC hDC,HPEN hPen)
{
	RECT rect;
	//HDC hDC = ::GetDC(hWnd);
	::GetClientRect(hWnd,&rect);
	HPEN hOldPen = (HPEN)SelectObject(hDC,hPen);

	MoveToEx(hDC,rect.left,rect.top,NULL);
	LineTo(hDC,rect.right - 1,rect.top);
	LineTo(hDC,rect.right - 1,rect.bottom - 1);
	LineTo(hDC,rect.left,rect.bottom - 1);
	LineTo(hDC,rect.left,rect.top);

	SelectObject(hDC,hOldPen);
	//::ReleaseDC(hWnd,hDC);
}

__inline void DrawRect(HDC hDC,HPEN hPen,RECT rect)
{
	//RECT rect;
	//HDC hDC = ::GetDC(hWnd);
	//::GetClientRect(hWnd,&rect);
	HPEN hOldPen = (HPEN)SelectObject(hDC,hPen);

	MoveToEx(hDC,rect.left,rect.top,NULL);
	LineTo(hDC,rect.right - 1,rect.top);
	LineTo(hDC,rect.right - 1,rect.bottom - 1);
	LineTo(hDC,rect.left,rect.bottom - 1);
	LineTo(hDC,rect.left,rect.top);

	SelectObject(hDC,hOldPen);
	//::ReleaseDC(hWnd,hDC);
}

__inline void DrawLine(HDC hDC,int x1,int y1,int x2,int y2)
{
	MoveToEx(hDC,x1,y1,NULL);
	LineTo(hDC,x2,y2);
}

__inline void BrushRect(HWND hWnd,HBRUSH hBrush)
{
	RECT rect;
	HDC hDC = ::GetDC(hWnd);
	::GetClientRect(hWnd,&rect);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC,hBrush);

	int iRet = FillRect(hDC,&rect,hBrush);

	SelectObject(hDC,hOldBrush);
	::ReleaseDC(hWnd,hDC);
}

__inline void BrushRect(HWND hWnd,HDC hDC,HBRUSH hBrush)
{
	RECT rect;
	//HDC hDC = ::GetDC(hWnd);
	::GetClientRect(hWnd,&rect);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC,hBrush);

	int iRet = FillRect(hDC,&rect,hBrush);

	SelectObject(hDC,hOldBrush);
	//::ReleaseDC(hWnd,hDC);
}

#endif