#ifndef _YUVConverter_H
#define _YUVConverter_H

#include "libyuv.h"

class YUVConverter
{
		int m_iBufLen;
		unsigned char* m_pTempBuf;
   public:
	    YUVConverter();
        ~YUVConverter();

		//C
		void I420ToNV12(unsigned char* yuv420p, unsigned char* yuv420sp, int width, int height);
		void NV12ToI420(unsigned char* yuv420sp, unsigned char* yuv420p, int width, int height);
		void NV12Scale(unsigned char* __restrict src, unsigned char* __restrict dst,int srcWidth, int srcHeight, int dstWidth, int dstHeight);

		void NV12ToI420_MMX(unsigned char* yuv420sp, unsigned char* yuv420p, int width, int height);
		void YUV_Scale(unsigned char* src, unsigned char* dest,int srcWidth, int srcHeight, int dstWidth, int dstHeight);
		void RGB24ToI420(unsigned char* rgb, unsigned char* yuv420p, int width, int height);
		void ARGBToI420(unsigned char* rgb, unsigned char* yuv420p, int width, int height);

		void I420ToARGB(unsigned char* yuv420p, unsigned char* rgb, int width, int height);
		void I420ToRGB24(unsigned char* yuv420p, unsigned char* rgb, int width, int height);

		int Flip(BYTE* inbuf, long iWidth, UINT height, int iBits);
};

#endif