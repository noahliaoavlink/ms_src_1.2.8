#include "stdafx.h"
#include "DiBDraw.h"

#pragma comment(lib, "Vfw32.lib")
/*
RECT	rcDraw[4][4] = {
0,0,320,240,//0,0,256,180,
320,0,320,240,
0,240,320,240,
320,240,320,240,

0,180,256,180,
256,180,256,180,
512,180,256,180,
768,180,256,180,

0,360,256,180,
256,360,256,180,
512,360,256,180,
768,360,256,180,

0,540,256,180,
256,540,256,180,
512,540,256,180,
768,540,256,180,
};
*/
DiBDraw::DiBDraw()
{
	m_BmpInfHdr.biHeight = 0;
	m_BmpInfHdr.biWidth = 0;

	m_hDrawDib = 0;

	m_hWnd = 0;
}

DiBDraw::~DiBDraw()
{
}

void DiBDraw::Init(HWND hWnd,RECT rect)
{
	m_hWnd = hWnd;

	memcpy(&m_rcDrawRect,&rect,sizeof(RECT));
}

void DiBDraw::Open(int iW,int iH,bool bIsNV12)
{
#if 0
	m_BmpInfHdr.biBitCount = 16;
	m_BmpInfHdr.biClrImportant = 0;
	m_BmpInfHdr.biClrUsed = 0;
	m_BmpInfHdr.biCompression = MAKEFOURCC('Y','U','Y','2');
	m_BmpInfHdr.biHeight = iH;
	m_BmpInfHdr.biWidth = iW;
	m_BmpInfHdr.biPlanes = 1;
	m_BmpInfHdr.biSize = sizeof(BITMAPINFOHEADER);
	m_BmpInfHdr.biSizeImage = iW * iH * 2;
	m_BmpInfHdr.biXPelsPerMeter = 0;
	m_BmpInfHdr.biYPelsPerMeter = 0;
#else
	m_BmpInfHdr.biBitCount = 12;
	m_BmpInfHdr.biClrImportant = 0;
	m_BmpInfHdr.biClrUsed = 0;
	if(bIsNV12)
		m_BmpInfHdr.biCompression = MAKEFOURCC('N','V','1','2');
	else
		m_BmpInfHdr.biCompression = MAKEFOURCC('I','4','2','0');
	m_BmpInfHdr.biHeight = iH;
	m_BmpInfHdr.biWidth = iW;
	m_BmpInfHdr.biPlanes = 1;
	m_BmpInfHdr.biSize = sizeof(BITMAPINFOHEADER);
	m_BmpInfHdr.biSizeImage = iW * iH * 3 / 2;
	m_BmpInfHdr.biXPelsPerMeter = 0;
	m_BmpInfHdr.biYPelsPerMeter = 0;
#endif

	m_hDrawDib = DrawDibOpen();

	char szMsg[512];
	sprintf(szMsg,"#DIB# m_hDrawDib = %d \n",m_hDrawDib);
	//OutputDebugStringA(szMsg);
}

void DiBDraw::Close()
{
	if(m_hDrawDib)
	{
		DrawDibClose(m_hDrawDib);
		m_hDrawDib = 0;
	}
}

void DiBDraw::Draw(unsigned char* pBuffer,int iW,int iH,bool bIsNV12)
{
//	if(!m_bFinished[iChannel])
	//	return ;

//	m_bFinished[iChannel] = false;

	if(m_BmpInfHdr.biHeight != iH || 
		m_BmpInfHdr.biWidth != iW)
	{
		Close();
		Open(iW,iH,bIsNV12);
	}
	
	//RECT	rc = {0,0,320,240};

	//memcpy(&rc,&rcDraw[0][0],sizeof(RECT));

	HDC hDC = GetDC(m_hWnd);

	DrawDibBegin(m_hDrawDib, hDC, 0, 0, &m_BmpInfHdr, 0, 0, 0x0200);
	DrawDibDraw(m_hDrawDib, hDC, m_rcDrawRect.left, m_rcDrawRect.top
				, m_rcDrawRect.right, m_rcDrawRect.bottom
				, &m_BmpInfHdr, pBuffer, 0, 0,
				iW, iH, 0x0200);

	ReleaseDC(m_hWnd,hDC);

//	m_bFinished[iChannel] = true;
}