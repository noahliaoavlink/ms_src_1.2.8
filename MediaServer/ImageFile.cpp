#include "stdafx.h"
#include "ImageFile.h"
//#include "JpegFile.h"
#include <stdio.h>

ImageFile::ImageFile()
{
	m_buf = 0;
	m_iWidth = 0;
	m_iHeight = 0;
	m_hBitmap = 0;
	m_hPalette = 0;

	m_pBitmap = new BYTE[3000 * 3000 * 4];
}

ImageFile::~ImageFile()
{
	if (m_buf != NULL) 
	{
		delete [] m_buf;
		m_buf = NULL;
	}

	if(m_hBitmap)
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = 0;
	}

	if(m_hPalette)
	{
		DeleteObject(m_hPalette);
		m_hPalette = 0;
	}

	delete m_pBitmap;
}

bool ImageFile::Load(char* szFileName)
{
	char szExtension[100];

	char* szRet = strrchr(szFileName,'.');
	if(szRet)
	{
		strcpy(szExtension,szRet + 1);
	}

//	if(strcmp(_strupr(szExtension),"JPG") == 0)
//		return LoadJPG(szFileName);
//	else
		return LoadBmp(szFileName);

	return false;
}
/*
bool ImageFile::LoadJPG(char* szFileName)
{
	// m_buf is the global buffer
	if (m_buf != NULL) 
	{
		delete [] m_buf;
		m_buf = NULL;
	}

	// read to buffer tmp
	m_buf = JpegFile::JpegFileToRGB(szFileName, &m_iWidth, &m_iHeight);

	if(!m_buf)
		return false;

	//////////////////////
	// set up for display

	// do this before DWORD-alignment!!!
	// this works on packed (not DWORD-aligned) buffers
	// swap red and blue for display
	JpegFile::BGRFromRGB(m_buf, m_iWidth, m_iHeight);

	// vertical flip for display
	JpegFile::VertFlipBuf(m_buf, m_iWidth * 3, m_iHeight);

	MakeDDB();

	return true;
}

void ImageFile::MakeDDB()
{
	HDC hDC = ::GetDC(NULL);
	BYTE *tmp;
	// DWORD-align for display
    tmp = JpegFile::MakeDwordAlignedBuf(m_buf,
									 m_iWidth,
									 m_iHeight,
 	    	    					 &m_iWidthDW);
	// set up a DIB 
	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = m_iWidth;
	bmiHeader.biHeight = m_iHeight;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 32;//24;
	bmiHeader.biCompression = BI_RGB;
	bmiHeader.biSizeImage = 0;
	bmiHeader.biXPelsPerMeter = 0;
	bmiHeader.biYPelsPerMeter = 0;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;

	if(m_hBitmap)
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = 0;
	}

	m_hBitmap = CreateDIBitmap(hDC,&bmiHeader,CBM_INIT,tmp,(LPBITMAPINFO)&bmiHeader,DIB_RGB_COLORS);

	if(m_hPalette)
	{
		DeleteObject(m_hPalette);
		m_hPalette = 0;
	}

	//memcpy(m_pBitmap,tmp,m_iWidth * m_iHeight * 3);
	memcpy(m_pBitmap,tmp,m_iWidth * m_iHeight * 4);

	GetObject(m_hBitmap,sizeof(HPALETTE),&m_hPalette);
	::ReleaseDC(NULL,hDC);
	delete tmp;
}
*/

HBITMAP ImageFile::GetHBITMAP()
{
	return m_hBitmap;
}

HPALETTE ImageFile::GetHPALETTE()
{
	return m_hPalette;
}

bool ImageFile::LoadBmp( char* szFileName)
{
	HDC hDC = ::GetDC(NULL);
	FILE* pFile;
	if( (pFile = fopen( szFileName, "rb")) == NULL)
		return false;

	BITMAPFILEHEADER bmfHeader;
    if (fread((LPSTR)&bmfHeader,1,sizeof(bmfHeader),pFile) != sizeof(bmfHeader))
	{
		fclose(pFile);
		return false;
	}

    if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
	{
		fclose(pFile);
		return false;
	}

	DWORD nPackedDIBLen = bmfHeader.bfSize - sizeof(BITMAPFILEHEADER);
	HGLOBAL hDIB = ::GlobalAlloc(GMEM_FIXED, nPackedDIBLen);
	if (hDIB == 0)
	{
		fclose(pFile);
		return false;
	}
	
	int iLen = fread((LPSTR)hDIB,1,nPackedDIBLen,pFile);
	if (iLen != nPackedDIBLen )
	{
		::GlobalFree(hDIB);
		fclose(pFile);
		return false;
	}
	
	BITMAPINFOHEADER &bmiHeader = *(LPBITMAPINFOHEADER)hDIB ;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;

	int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed : 1 << bmiHeader.biBitCount;
    LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmInfo.bmiHeader.biClrUsed) + 
		((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);

	// Create the palette
    if( nColors <= 256 )
    {
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
        LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

        pLP->palVersion = 0x300;
        pLP->palNumEntries = nColors;

        for(register int i=0; i < nColors; i++)
        {
			pLP->palPalEntry[i].peRed = bmInfo.bmiColors[i].rgbRed;
            pLP->palPalEntry[i].peGreen = bmInfo.bmiColors[i].rgbGreen;
            pLP->palPalEntry[i].peBlue = bmInfo.bmiColors[i].rgbBlue;
            pLP->palPalEntry[i].peFlags = 0;
        }

		if(m_hPalette)
		{
			DeleteObject(m_hPalette);
			m_hPalette = 0;
		}

        m_hPalette = CreatePalette( pLP );
        delete pLP;
    }

	if(m_hBitmap)
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = 0;
	}

	m_hBitmap = CreateDIBitmap(hDC,&bmiHeader,CBM_INIT,lpDIBBits,&bmInfo,DIB_RGB_COLORS);

	m_iWidth = bmiHeader.biWidth;
	m_iHeight = bmiHeader.biHeight;

	memcpy(m_pBitmap,lpDIBBits,m_iWidth * m_iHeight * (bmInfo.bmiHeader.biBitCount / 8));

	::GlobalFree(hDIB);
    ::ReleaseDC(NULL,hDC);

	fclose(pFile);
	return true;
}

int ImageFile::GetWidth()
{
	return m_iWidth;
}

int ImageFile::GetHeight()
{
	return m_iHeight;
}

void ImageFile::GetBmpBuffer(HBITMAP hBitmap,int* iWidth,int* iHeight,unsigned char* pBuffer,bool bDoSwitch)
{
	HDC hDC = GetDC(NULL);
	BITMAP Bm;
    BITMAPINFO BitInfo;
    ZeroMemory(&BitInfo, sizeof(BITMAPINFO));
    BitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    BitInfo.bmiHeader.biBitCount = 0;

    if(!::GetDIBits(hDC, hBitmap, 0, 0, NULL, &BitInfo, DIB_RGB_COLORS))
        return ;

    Bm.bmHeight = BitInfo.bmiHeader.biHeight;
    Bm.bmWidth  = BitInfo.bmiHeader.biWidth;

    BITMAPFILEHEADER    BmHdr;
    
    BmHdr.bfType        = 0x4d42;   // 'BM' WINDOWS_BITMAP_SIGNATURE
    BmHdr.bfSize        = (((3 * Bm.bmWidth + 3) & ~3) * Bm.bmHeight) 
                          + sizeof(BITMAPFILEHEADER) 
                          + sizeof(BITMAPINFOHEADER);
    BmHdr.bfReserved1    = BmHdr.bfReserved2 = 0;
    BmHdr.bfOffBits      = (DWORD) sizeof(BITMAPFILEHEADER) 
                          + sizeof(BITMAPINFOHEADER);
    
    BitInfo.bmiHeader.biCompression = 0;

    //BYTE *pData = new BYTE[BitInfo.bmiHeader.biSizeImage + 5];
	BitInfo.bmiHeader.biBitCount = 24;    //32 -> 24 在16的模式下會有問題
    if(!::GetDIBits(hDC, hBitmap, 0, Bm.bmHeight,pBuffer, &BitInfo, DIB_RGB_COLORS))
        return ;

    //if(pData != NULL)
	  //  SaveJpgFileEx(szFileName,pData,Bm.bmWidth,Bm.bmHeight,80);
	*iWidth = Bm.bmWidth;
	*iHeight = Bm.bmHeight;

	if(bDoSwitch)
		RGB2BGR(pBuffer,Bm.bmWidth,Bm.bmHeight);

//	delete (pData);
	ReleaseDC(NULL,hDC);
}

void ImageFile::RGB2BGR(unsigned char* pBuffer,int iWidth,int iHeight)
{
	for(int y = 0;y < iHeight;y++)
	{
		int iScanLine = y * iWidth * 3;
		for(int x = 0;x < iWidth;x++)
		{
			unsigned char* p = pBuffer + iScanLine + x * 3;
			unsigned char ucTemp = p[0];
			p[0] = p[2];
			p[2] = ucTemp;
		}
	}
}

void ImageFile::SaveBmpFile(char* szFileName,unsigned char* pBuffer,int iWidth,int iHeight)
{
	int iLen = 0;
	int iPaletteSize = 0;
	FILE *pFile = fopen(szFileName, "wb");

	BITMAPINFOHEADER BMIH;
	BMIH.biSize = sizeof(BITMAPINFOHEADER);
    BMIH.biBitCount = 24;
    BMIH.biPlanes = 1;
    BMIH.biCompression = BI_RGB;
    BMIH.biWidth = iWidth;
    BMIH.biHeight = iHeight;
    BMIH.biSizeImage = iHeight * iWidth * 3;
	BMIH.biClrUsed = 0;

	DWORD dwBitmapInfoSize = sizeof(BITMAPINFO) + BMIH.biClrUsed * sizeof(RGBQUAD);
    DWORD dwFileHeaderSize = dwBitmapInfoSize + sizeof(BITMAPFILEHEADER);

	BITMAPFILEHEADER bmfh;
    int nBitsOffset = sizeof(BITMAPFILEHEADER) + BMIH.biSize; 
    bmfh.bfType = 0x4d42;
    bmfh.bfOffBits = nBitsOffset;
    bmfh.bfSize = 40; //dwFileHeaderSize + BMIH.biSizeImage;
    bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;

    //Write the bitmap file header
    iLen = fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), pFile);

    //And then the bitmap info header
    iLen = fwrite(&BMIH, 1, sizeof(BITMAPINFOHEADER), pFile);

    //Finally, write the image data itself 
    //-- the data represents our drawing
    iLen = fwrite(pBuffer, 1, BMIH.biSizeImage, pFile);

    fclose(pFile);
}
/*
void ImageFile::SaveJPG(char* szFileName,unsigned char* pBuffer,int iWidth,int iHeight,int iQulity)
{
	bool bColor = true;
	int iLen = iWidth * iHeight * 3;
    unsigned char* pTempBuffer = new unsigned char[iLen];
	memcpy(pTempBuffer,pBuffer,iLen);

	// we vertical flip for display. undo that.
	JpegFile::VertFlipBuf(pTempBuffer, iWidth * 3, iHeight);

	// we swap red and blue for display, undo that.
	JpegFile::BGRFromRGB(pTempBuffer, iWidth, iHeight);


	// save RGB packed buffer to JPG
	BOOL ok = JpegFile::RGBToJpegFile(szFileName, 
									pTempBuffer,
									iWidth,
									iHeight,
									bColor, 
									iQulity);			// quality value 1-100.
	delete pTempBuffer;
}
*/
BYTE* ImageFile::GetBuffer()
{
	return m_pBitmap;
}