#ifndef _DiBDraw_H
#define _DiBDraw_H

#include <Vfw.h>

class DiBDraw
{
	    HDRAWDIB m_hDrawDib;
		BITMAPINFOHEADER m_BmpInfHdr;

		HWND m_hWnd;
		RECT m_rcDrawRect;
   public:
	    DiBDraw();
        ~DiBDraw();

		void Init(HWND hWnd,RECT rect);
		void Open(int iW,int iH,bool bIsNV12 = false);
		void Close();
		void Draw(unsigned char* pBuffer,int iW,int iH,bool bIsNV12 = false);
};

#endif