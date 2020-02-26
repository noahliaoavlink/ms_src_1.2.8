#include "stdafx.h"
#include "Allocator.h"

void NegativeEffect(DWORD * pImageData, INT32 iWidth, INT32 iHeight, INT32 iLevel, INT32 iParm1, INT32 iParm2, INT32 iParm3, INT32 iParm4, INT32 iParm5)
{
	for(DWORD y = 0; y < iHeight; y++)
	{
		for(DWORD x = 0; x < iWidth; x++)
		{
			DWORD color = *(pImageData+x);

			BYTE  r = (color & 0x00ff0000)>>16; 
			BYTE  g = (color & 0x0000ff00)>>8; 
			BYTE  b = (color & 0x000000ff); 
			r = 255-r; // ==(r ^ 0xff)
			g = 255-g; // ==(g ^ 0xff)
			b = 255-b; // ==(b ^ 0xff)

			color =  0xff000000 | (r<<16) | (g<<8) | (b); 
			*(pImageData+x) = color;
		}
		pImageData = pImageData + iWidth;
	}
}

void CAllocator::VideoEffect(INT32 iEffectNum, DWORD * pImageData, INT32 iWidth, INT32 iHeight, INT32 iLevel, INT32 iParm1, INT32 iParm2, INT32 iParm3, INT32 iParm4, INT32 iParm5)
{
	switch(iEffectNum)
	{
		case 1: //­t¤ù¯S®Ä
			NegativeEffect(pImageData, iWidth, iHeight, iLevel, iParm1, iParm2, iParm3, iParm4, iParm5);
			break;

	}
}
