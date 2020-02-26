#include "stdafx.h"
#include "YUVConverter.h"

#pragma comment(lib, "libyuv.lib")

YUVConverter::YUVConverter()
{
	m_pTempBuf = 0; 
	m_iBufLen = 0;
}

YUVConverter::~YUVConverter()
{
	if (m_pTempBuf)
		delete m_pTempBuf;
}

void YUVConverter::I420ToNV12(unsigned char* yuv420p, unsigned char* yuv420sp, int width, int height)
{
    int i, j;
    int y_size = width * height;

    unsigned char* y = yuv420p;
    unsigned char* u = yuv420p + y_size;
    unsigned char* v = yuv420p + y_size * 5 / 4;

    unsigned char* y_tmp = yuv420sp;
    unsigned char* uv_tmp = yuv420sp + y_size;

    // y
    memcpy(y_tmp, y, y_size);

    // u
    for (j = 0, i = 0; j < y_size/2; j+=2, i++)
    {
	// 此處可調整U、V的位置，變成NV12或NV21
#if 01
        uv_tmp[j] = u[i];
        uv_tmp[j+1] = v[i];
#else
        uv_tmp[j] = v[i];
        uv_tmp[j+1] = u[i];
#endif
    }
}

void YUVConverter::NV12ToI420(unsigned char* yuv420sp, unsigned char* yuv420p, int width, int height)
{
    int i, j;
    int y_size = width * height;

    unsigned char* y = yuv420sp;
    unsigned char* uv = yuv420sp + y_size;

    unsigned char* y_tmp = yuv420p;
    unsigned char* u_tmp = yuv420p + y_size;
    unsigned char* v_tmp = yuv420p + y_size * 5 / 4;

    // y
    memcpy(y_tmp, y, y_size);

    // u
    for (j = 0, i = 0; j < y_size/2; j+=2, i++)
    {
        u_tmp[i] = uv[j];
        v_tmp[i] = uv[j+1];
    }
}

void YUVConverter::NV12ToI420_MMX(unsigned char* yuv420sp, unsigned char* yuv420p, int width, int height)
{
	int chroma_w = 1;
	int chroma_h = 1;
	int w2 = (width + (1 << chroma_w) - 1) >> chroma_w;	

	int dstStride[4];

	dstStride[0] = width;
	dstStride[1] = w2;
	dstStride[2] = w2;

	int size = dstStride[0] * height;
    int h2 = (height + (1 << chroma_h) - 1) >> chroma_h;
	int size2 = dstStride[1] * h2; 

	unsigned char* pDstY = yuv420p;
    unsigned char* pDstU = pDstY + size;
    unsigned char* pDstV = pDstU + size2;

	int crop_x = 0;
	int crop_y = 0;
	int src_size = width * height;

	libyuv::RotationMode rotation;
    rotation = libyuv::kRotate0;
    

	int format = libyuv::FOURCC_NV12;

	int iRet = libyuv::ConvertToI420(yuv420sp, src_size,
                  pDstY, dstStride[0],
                  pDstU, dstStride[1],
                  pDstV, dstStride[2],
                  crop_x, crop_y,
                  width, height,
                  width, height,
                  rotation,
                  format);
}

//nv12_nearest_scale
void YUVConverter::NV12Scale(unsigned char* __restrict src, unsigned char* __restrict dst,int srcWidth, int srcHeight, int dstWidth, int dstHeight)      
{   //restrict keyword is for compiler to optimize program
    register int sw = srcWidth;  //register keyword is for local var to accelorate 
    register int sh = srcHeight;
    register int dw = dstWidth;
    register int dh = dstHeight;
    register int y, x;
    unsigned long int srcy, srcx, src_index, dst_index;
    unsigned long int xrIntFloat_16 = (sw << 16) / dw + 1; //better than float division
    unsigned long int yrIntFloat_16 = (sh << 16) / dh + 1;

    unsigned char* dst_uv = dst + dh * dw; //memory start pointer of dest uv
    unsigned char* src_uv = src + sh * sw; //memory start pointer of source uv
    unsigned char* dst_uv_yScanline = 0;
    unsigned char* src_uv_yScanline = 0;
    unsigned char* dst_y_slice = dst; //memory start pointer of dest y
    unsigned char* src_y_slice;
    unsigned char* sp;
    unsigned char* dp;
 
    for (y = 0; y < (dh & ~7); ++y)  //'dh & ~7' is to generate faster assembly code
    {
        srcy = (y * yrIntFloat_16) >> 16;
        src_y_slice = src + srcy * sw;

        if((y & 1) == 0)
        {
#if 0
            dst_uv_yScanline = dst_uv + (y / 2) * dw;
            src_uv_yScanline = src_uv + (srcy / 2) * sw;
#else
			dst_uv_yScanline = dst_uv + (y >> 1) * dw;
            src_uv_yScanline = src_uv + (srcy >> 1) * sw;
#endif
        }

        for(x = 0; x < (dw & ~7); ++x)
        {
            srcx = (x * xrIntFloat_16) >> 16;
            dst_y_slice[x] = src_y_slice[srcx];

            if((y & 1) == 0) //y is even
            {
                if((x & 1) == 0) //x is even
                {
#if 1
                    src_index = (srcx / 2) * 2;
#else
					//src_index = (srcx >> 1) << 1;
					src_index = srcx;
#endif
            
                    sp = dst_uv_yScanline + x;
                    dp = src_uv_yScanline + src_index;
                    *sp = *dp;
                    ++sp;
                    ++dp;
                    *sp = *dp;
                }
             }
         }
         dst_y_slice += dw;
    }
}

void YUVConverter::YUV_Scale(unsigned char* pSrc, unsigned char* pDest,int srcWidth, int srcHeight, int dstWidth, int dstHeight)
{
	int chroma_w = 1;
	int chroma_h = 1;
	int srcStride[4];
	int dstStride[4];
	unsigned char* src[4];
	unsigned char* dst[4];

#if 1
	int w2 = (srcWidth + (1 << chroma_w) - 1) >> chroma_w;
	srcStride[0] = srcWidth;
	srcStride[1] = w2;
	srcStride[2] = w2;

	int size = srcStride[0] * srcHeight;
    int h2 = (srcHeight + (1 << chroma_h) - 1) >> chroma_h;
	int size2 = srcStride[1] * h2; 

	src[0] = pSrc;
	src[1] = src[0] + size;
	src[2] = src[1] + size2;
	src[3] = NULL;

	w2 = (dstWidth + (1 << chroma_w) - 1) >> chroma_w;
	//dstStride[0] = iWidth * 2;
	dstStride[0] = dstWidth;
	dstStride[1] = w2;
	dstStride[2] = w2;

	size = dstStride[0] * dstHeight;
    h2 = (dstHeight + (1 << chroma_h) - 1) >> chroma_h;
	size2 = dstStride[1] * h2; 

	dst[0] = pDest;
	dst[1] = dst[0] + size;
	dst[2] = dst[1] + size2;
	dst[3] = NULL;
#else
	srcStride[0] = srcWidth;
	srcStride[1] = srcWidth / 2;
	srcStride[2] = NULL;

	int size = srcStride[0] * srcHeight;

	src[0] = pSrc;
	src[1] = src[0] + size;
	src[2] = NULL;
	src[3] = NULL;

	dstStride[0] = dstWidth;
	dstStride[1] = dstWidth / 2;
	dstStride[2] = NULL;

	size = dstStride[0] * dstHeight;

	dst[0] = pDest;
	dst[1] = dst[0] + size;
	dst[2] = NULL;
	dst[3] = NULL;

#endif
	
#if 0
	bool interpolate = false;
	int iRet = libyuv::Scale(src[0], src[1], src[2],
          srcStride[0], srcStride[1], srcStride[2],
          srcWidth, srcHeight,
          dst[0], dst[1], dst[2],
          dstStride[0], dstStride[1], dstStride[2],
          dstWidth, dstHeight,
          interpolate);
#else
	libyuv::FilterMode filtering = libyuv::FilterMode::kFilterNone;
	int iRet = libyuv::I420Scale(src[0], srcStride[0],
              src[1], srcStride[1],
              src[2], srcStride[2],
              srcWidth, srcHeight,
              dst[0], dstStride[0],
              dst[1], dstStride[1],
              dst[2], dstStride[2],
              dstWidth, dstHeight,
              filtering);
#endif
}

void YUVConverter::RGB24ToI420(unsigned char* rgb, unsigned char* yuv420p, int width, int height)
{
	int chroma_w = 1;
	int chroma_h = 1;
	int w2 = (width + (1 << chroma_w) - 1) >> chroma_w;	

	int dstStride[4];

	dstStride[0] = width;
	dstStride[1] = w2;
	dstStride[2] = w2;

	int size = dstStride[0] * height;
    int h2 = (height + (1 << chroma_h) - 1) >> chroma_h;
	int size2 = dstStride[1] * h2; 

	unsigned char* pDstY = yuv420p;
	//RGB
    unsigned char* pDstU = pDstY + size;
    unsigned char* pDstV = pDstU + size2;
	//BGR
	//unsigned char* pDstV = pDstY + size;
    //unsigned char* pDstU = pDstV + size2;

	int crop_x = 0;
	int crop_y = 0;
	//int src_size = width * height;
	int src_size = width * 3;

	int iRet = libyuv::RGB24ToI420(rgb, src_size,
                pDstY, dstStride[0],
                  pDstU, dstStride[1],
                  pDstV, dstStride[2],
                width, height);
}

void YUVConverter::ARGBToI420(unsigned char* rgb, unsigned char* yuv420p, int width, int height)
{
	int chroma_w = 1;
	int chroma_h = 1;
	int w2 = (width + (1 << chroma_w) - 1) >> chroma_w;	

	int dstStride[4];

	dstStride[0] = width;
	dstStride[1] = w2;
	dstStride[2] = w2;

	int size = dstStride[0] * height;
    int h2 = (height + (1 << chroma_h) - 1) >> chroma_h;
	int size2 = dstStride[1] * h2; 

	unsigned char* pDstY = yuv420p;
	//RGB
    unsigned char* pDstU = pDstY + size;
    unsigned char* pDstV = pDstU + size2;
	//BGR
	//unsigned char* pDstV = pDstY + size;
    //unsigned char* pDstU = pDstV + size2;

	int crop_x = 0;
	int crop_y = 0;
	//int src_size = width * height;
	int src_size = width * 4;

	int iRet = libyuv::ARGBToI420(rgb, src_size,
                pDstY, dstStride[0],
                  pDstU, dstStride[1],
                  pDstV, dstStride[2],
                width, height);
}

void YUVConverter::I420ToARGB(unsigned char* yuv420p, unsigned char* rgb, int width, int height)
{
	int chroma_w = 1;
	int chroma_h = 1;
	int w2 = (width + (1 << chroma_w) - 1) >> chroma_w;

	int srcStride[4];

	srcStride[0] = width;
	srcStride[1] = w2;
	srcStride[2] = w2;

	int size = srcStride[0] * height;
	int h2 = (height + (1 << chroma_h) - 1) >> chroma_h;
	int size2 = srcStride[1] * h2;

	unsigned char* pSrcY = yuv420p;
	unsigned char* pSrcU = pSrcY + size;
	unsigned char* pSrcV = pSrcU + size2;

	int format = libyuv::FOURCC_ARGB;

	int iRet = libyuv::ConvertFromI420(pSrcY, srcStride[0],
		pSrcU, srcStride[1],
		pSrcV, srcStride[2],
		rgb, width * 4,
		width, height,
		format);
}

void YUVConverter::I420ToRGB24(unsigned char* yuv420p, unsigned char* rgb, int width, int height)
{
	int chroma_w = 1;
	int chroma_h = 1;
	int w2 = (width + (1 << chroma_w) - 1) >> chroma_w;

	int srcStride[4];

	srcStride[0] = width;
	srcStride[1] = w2;
	srcStride[2] = w2;

	int size = srcStride[0] * height;
	int h2 = (height + (1 << chroma_h) - 1) >> chroma_h;
	int size2 = srcStride[1] * h2;

	unsigned char* pSrcY = yuv420p;
	unsigned char* pSrcU = pSrcY + size;
	unsigned char* pSrcV = pSrcU + size2;

	int format = libyuv::FOURCC_ARGB;

	int iRet = libyuv::I420ToRGB24(pSrcY, srcStride[0],
		pSrcU, srcStride[1],
		pSrcV, srcStride[2],
		rgb, width * 3,
		width, height);
}

int YUVConverter::Flip(BYTE* inbuf, long iWidth, UINT height,int iBits)
{
	BYTE  *tb1;
	BYTE  *tb2;

	if (inbuf == NULL)
		return FALSE;

	UINT bufsize;

	bufsize = iWidth * (iBits / 8);

	if (m_iBufLen < bufsize)
	{
		m_iBufLen = bufsize;
		if (m_pTempBuf)
			delete m_pTempBuf;
		m_pTempBuf = (BYTE *)new BYTE[m_iBufLen];
		if (m_pTempBuf == NULL)
			return FALSE;
	}

	tb1 = m_pTempBuf;

	UINT row_cnt;
	ULONG off1 = 0;
	ULONG off2 = (height - 1) * bufsize;
	BYTE* pBuff1;
	BYTE* pBuff2;

	int iTotal = (height + 1) >> 1;
	for (row_cnt = 0; row_cnt < iTotal; row_cnt++)
	{
		off1 += bufsize;
		off2 -= bufsize;

		pBuff1 = inbuf + off1;
		pBuff2 = inbuf + off2;

		memcpy(tb1, pBuff1, bufsize);
		memcpy(pBuff1, pBuff2, bufsize);
		memcpy(pBuff2, tb1, bufsize);
	}
	return TRUE;
}


/*
void nv12_bilinear_scale (uint8_t* src, uint8_t* dst,
        int srcWidth, int srcHeight, int dstWidth,int dstHeight)
{
    int x, y;
    int ox, oy;
    int tmpx, tmpy;
    int xratio = (srcWidth << 8)/dstWidth;
    int yratio = (srcHeight << 8)/dstHeight;
    uint8_t* dst_y = dst;
    uint8_t* dst_uv = dst + dstHeight * dstWidth;
    uint8_t* src_y = src;
    uint8_t* src_uv = src + srcHeight * srcWidth;

    uint8_t y_plane_color[2][2];
    uint8_t u_plane_color[2][2];
    uint8_t v_plane_color[2][2];
    int j,i;
    int size = srcWidth * srcHeight;
    int offsetY;
    int y_final, u_final, v_final; 
    int u_final1 = 0;
    int v_final1 = 0;
    int u_final2 = 0;
    int v_final2 = 0;
    int u_final3 = 0;
    int v_final3 = 0;
    int u_final4 = 0;
    int v_final4 = 0;
    int u_sum = 0;
    int v_sum = 0;


    tmpy = 0;
    for (j = 0; j < (dstHeight & ~7); ++j)
    {
        //tmpy = j * yratio;
    oy = tmpy >> 8;
    y = tmpy & 0xFF;

    tmpx = 0;
    for (i = 0; i < (dstWidth & ~7); ++i)
    {
        // tmpx = i * xratio;
        ox = tmpx >> 8;
        x = tmpx & 0xFF;
    
        offsetY = oy * srcWidth;
            //YYYYYYYYYYYYYYYY
        y_plane_color[0][0] = src[ offsetY + ox ];
        y_plane_color[1][0] = src[ offsetY + ox + 1 ];
        y_plane_color[0][1] = src[ offsetY + srcWidth + ox ];
        y_plane_color[1][1] = src[ offsetY + srcWidth + ox + 1 ];
            
        int y_final = (0x100 - x) * (0x100 - y) * y_plane_color[0][0]
            + x * (0x100 - y) * y_plane_color[1][0]
            + (0x100 - x) * y * y_plane_color[0][1]
            + x * y * y_plane_color[1][1];
        y_final = y_final >> 16;
        if (y_final>255)
            y_final = 255;
        if (y_final<0)
            y_final = 0;
        dst_y[ j * dstWidth + i] = (uint8_t)y_final; //set Y in dest array 
            //UVUVUVUVUVUV
        if((j & 1) == 0) //j is even
        {
            if((i & 1) == 0) //i is even
            {
                u_plane_color[0][0] = src[ size + offsetY + ox ];
                u_plane_color[1][0] = src[ size + offsetY + ox ];
                u_plane_color[0][1] = src[ size + offsetY + ox ];
                u_plane_color[1][1] = src[ size + offsetY + ox ];

                v_plane_color[0][0] = src[ size + offsetY + ox + 1];
                v_plane_color[1][0] = src[ size + offsetY + ox + 1];
                v_plane_color[0][1] = src[ size + offsetY + ox + 1];
                v_plane_color[1][1] = src[ size + offsetY + ox + 1];
            }
            else //i is odd
            {
                u_plane_color[0][0] = src[ size + offsetY + ox - 1 ];
                u_plane_color[1][0] = src[ size + offsetY + ox + 1 ];
                u_plane_color[0][1] = src[ size + offsetY + ox - 1 ];
                u_plane_color[1][1] = src[ size + offsetY + ox + 1 ];

                v_plane_color[0][0] = src[ size + offsetY + ox ];
                v_plane_color[1][0] = src[ size + offsetY + ox + 1 ];
                v_plane_color[0][1] = src[ size + offsetY + ox ];
                v_plane_color[1][1] = src[ size + offsetY + ox + 1 ];
            }
        }
        else // j is odd
        {
            if((i & 1) == 0) //i is even
            {
                u_plane_color[0][0] = src[ size + offsetY + ox ];
                u_plane_color[1][0] = src[ size + offsetY + ox ];
                u_plane_color[0][1] = src[ size + offsetY + srcWidth + ox ];
                u_plane_color[1][1] = src[ size + offsetY + srcWidth + ox ];
                    
                v_plane_color[0][0] = src[ size + offsetY + ox + 1];
                v_plane_color[1][0] = src[ size + offsetY + ox + 1];
                v_plane_color[0][1] = src[ size + offsetY + srcWidth + ox + 1];
                v_plane_color[1][1] = src[ size + offsetY + srcWidth + ox + 1];                                                    
            }
            else //i is odd
            {
                u_plane_color[0][0] = src[ size + offsetY + ox - 1 ];
                u_plane_color[1][0] = src[ size + offsetY + srcWidth + ox - 1 ];
                u_plane_color[0][1] = src[ size + offsetY + ox + 1];
                u_plane_color[1][1] = src[ size + offsetY + srcWidth + ox + 1];

                v_plane_color[0][0] = src[ size + offsetY + ox ];
                v_plane_color[1][0] = src[ size + offsetY + srcWidth + ox ];
                v_plane_color[0][1] = src[ size + offsetY + ox + 2 ];
                v_plane_color[1][1] = src[ size + offsetY + srcWidth + ox + 2 ];
            }
        }

       int u_final = (0x100 - x) * (0x100 - y) * u_plane_color[0][0]
                     + x * (0x100 - y) * u_plane_color[1][0]
                     + (0x100 - x) * y * u_plane_color[0][1]
                     + x * y * u_plane_color[1][1];
       u_final = u_final >> 16;

       int v_final = (0x100 - x) * (0x100 - y) * v_plane_color[0][0]
                      + x * (0x100 - y) * v_plane_color[1][0]
                      + (0x100 - x) * y * v_plane_color[0][1]
                      + x * y * v_plane_color[1][1];
       v_final = v_final >> 16;
       if((j & 1) == 0)
       {
           if((i & 1) == 0)
           {    
               //set U in dest array  
               dst_uv[(j / 2) * dstWidth + i ] = (uint8_t)(u_sum / 4);
               //set V in dest array
               dst_uv[(j / 2) * dstWidth + i + 1] = (uint8_t)(v_sum / 4);
               u_sum = 0;
               v_sum = 0;
           }
       }
       else
       {
           u_sum += u_final;
           v_sum += v_final;
       }
       tmpx += xratio;
    }
    tmpy += yratio;
    }
}
*/