#ifndef _ImageFile_H_
#define _ImageFile_H_

//#define BYTE unsigned long
#include <Windows.h>

class ImageFile
{
	// global image params
	BYTE *m_buf;
	unsigned int m_iWidth;
	unsigned int m_iHeight;
	unsigned int m_iWidthDW;

	HBITMAP m_hBitmap;
	HPALETTE m_hPalette;


	BYTE* m_pBitmap;
   public:
	    ImageFile();
        ~ImageFile();

		bool Load(char* szFileName);
//		bool LoadJPG(char* szFileName);

//		void MakeDDB();
		HBITMAP GetHBITMAP();
		HPALETTE GetHPALETTE();
		int GetWidth();
		int GetHeight();

		bool LoadBmp( char* szFileName);
		void SaveBmpFile(char* szFileName,unsigned char* pBuffer,int iWidth,int iHeight);
//		void SaveJPG(char* szFileName,unsigned char* pBuffer,int iWidth,int iHeight,int iQulity);

		BYTE* GetBuffer();
		void GetBmpBuffer(HBITMAP hBitmap,int* iWidth,int* iHeight,unsigned char* pBuffer,bool bDoSwitch);
		void RGB2BGR(unsigned char* pBuffer,int iWidth,int iHeight);
};

#endif