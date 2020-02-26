#include "stdafx.h"
#include "FFMediaFileDll.h"

#pragma comment(lib, "swscale.lib")

void GetPixFmtDescriptor(int iPixFmt, int& iWidth, int& iHeight)
{
	switch (iPixFmt)
	{
	case AV_PIX_FMT_YUV420P:
	case AV_PIX_FMT_YUVJ420P:
	case AV_PIX_FMT_NV12:
	case AV_PIX_FMT_NV21:
	{
		iWidth = 1;
		iHeight = 1;
	}
	break;
	case AV_PIX_FMT_YUYV422:
	case AV_PIX_FMT_YUV422P:
	case AV_PIX_FMT_YUVJ422P:
	case AV_PIX_FMT_UYVY422:
	{
		iWidth = 1;
		iHeight = 0;
	}
	break;
	case AV_PIX_FMT_YUV410P:
	{
		iWidth = 2;
		iHeight = 2;
	}
	break;
	case AV_PIX_FMT_YUV411P:
	case AV_PIX_FMT_UYYVYY411:
	{
		iWidth = 2;
		iHeight = 0;
	}
	break;
	default:
	{
		iWidth = 0;
		iHeight = 0;
	}
	}
}

FFMediaFileDll::FFMediaFileDll()
{
	m_hInst = 0;
	m_pMFRObj = 0;

	m_ScaleSrcSize.cx = 0;
	m_ScaleSrcSize.cy = 0;
	m_ScaleDestSize.cx = 0;
	m_ScaleDestSize.cy = 0;
	m_iScalePixFmt = 0;
	img_scale_ctx = 0;
}

FFMediaFileDll::~FFMediaFileDll()
{
	if (m_pMFRObj)
		_In_MFR_Destroy(m_pMFRObj);

	if (img_scale_ctx)
		sws_freeContext(img_scale_ctx);

	FreeLib();
}

bool FFMediaFileDll::LoadLib()
{
	char szFileName[] = "FFMediaFile.dll";
	m_hInst = LoadLibraryA(szFileName);

	if (m_hInst)
	{
		_In_MFR_Create = (_In_MFR_Create_Proc)GetProcAddress(m_hInst, "_In_MFR_Create");
		_In_MFR_Destroy = (_In_MFR_Destroy_Proc)GetProcAddress(m_hInst, "_In_MFR_Destroy");
		_In_MFR_Open = (_In_MFR_Open_Proc)GetProcAddress(m_hInst, "_In_MFR_Open");
		_In_MFR_Close = (_In_MFR_Close_Proc)GetProcAddress(m_hInst, "_In_MFR_Close");
		_In_MFR_GetVideoFrameSize = (_In_MFR_GetVideoFrameSize_Proc)GetProcAddress(m_hInst, "_In_MFR_GetVideoFrameSize");
		_In_MFR_GetVideoFPS = (_In_MFR_GetVideoFPS_Proc)GetProcAddress(m_hInst, "_In_MFR_GetVideoFPS");
		_In_MFR_GetNextFrameBuffer = (_In_MFR_GetNextFrameBuffer_Proc)GetProcAddress(m_hInst, "_In_MFR_GetNextFrameBuffer");
		_In_MFR_GetAVFrame = (_In_MFR_GetAVFrame_Proc)GetProcAddress(m_hInst, "_In_MFR_GetAVFrame");
		_In_MFR_GetAVCodecContext = (_In_MFR_GetAVCodecContext_Proc)GetProcAddress(m_hInst, "_In_MFR_GetAVCodecContext");
		_In_MFR_SeekTo = (_In_MFR_SeekTo_Proc)GetProcAddress(m_hInst, "_In_MFR_SeekTo");
		_In_MFR_GetCurVideoFramePos = (_In_MFR_GetCurVideoFramePos_Proc)GetProcAddress(m_hInst, "_In_MFR_GetCurVideoFramePos");
		_In_MFR_GetCurAudioFramePos = (_In_MFR_GetCurAudioFramePos_Proc)GetProcAddress(m_hInst, "_In_MFR_GetCurAudioFramePos");
		_In_MFR_GetLength = (_In_MFR_GetLength_Proc)GetProcAddress(m_hInst, "_In_MFR_GetLength");
		_In_MFR_EnableAudio = (_In_MFR_EnableAudio_Proc)GetProcAddress(m_hInst, "_In_MFR_EnableAudio");
		_In_MFR_GetAudioLength = (_In_MFR_GetAudioLength_Proc)GetProcAddress(m_hInst, "_In_MFR_GetAudioLength");
		_In_MFR_GetTotalOfFrames = (_In_MFR_GetTotalOfFrames_Proc)GetProcAddress(m_hInst, "_In_MFR_GetTotalOfFrames");

		m_pMFRObj = _In_MFR_Create();
		return true;
	}
	return false;
}

void FFMediaFileDll::FreeLib()
{
	if (m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = 0;
	}
}

int FFMediaFileDll::Open(char* szFileName)
{
	if (m_pMFRObj)
		return _In_MFR_Open(m_pMFRObj, szFileName);
}

void FFMediaFileDll::Close()
{
	if (m_pMFRObj)
		_In_MFR_Close(m_pMFRObj);
}

void FFMediaFileDll::GetVideoFrameSize(int* w, int* h)
{
	if (m_pMFRObj)
		_In_MFR_GetVideoFrameSize(m_pMFRObj, w, h);
}

float FFMediaFileDll::GetVideoFPS()
{
	if (m_pMFRObj)
		return _In_MFR_GetVideoFPS(m_pMFRObj);
}

int FFMediaFileDll::GetNextFrameBuffer(MediaFrameBuffer* pFrameBuf)
{
	if (m_pMFRObj)
		return _In_MFR_GetNextFrameBuffer(m_pMFRObj, pFrameBuf);
}

AVFrame* FFMediaFileDll::GetAVFrame()
{
	if (m_pMFRObj)
		return _In_MFR_GetAVFrame(m_pMFRObj);
}

AVCodecContext* FFMediaFileDll::GetAVCodecContext(int iType)
{
	if (m_pMFRObj)
		return _In_MFR_GetAVCodecContext(m_pMFRObj, iType);
}

int FFMediaFileDll::SeekTo(double pos)
{ 
	if (m_pMFRObj)
		return _In_MFR_SeekTo(m_pMFRObj, pos);
}

double FFMediaFileDll::GetCurVideoFramePos()
{
	if (m_pMFRObj)
		return _In_MFR_GetCurVideoFramePos(m_pMFRObj);
}

double FFMediaFileDll::GetCurAudioFramePos()
{
	if (m_pMFRObj)
		return _In_MFR_GetCurAudioFramePos(m_pMFRObj);
}

double FFMediaFileDll::GetLength()
{
	if (m_pMFRObj)
		return _In_MFR_GetLength(m_pMFRObj);
}

void FFMediaFileDll::EnableAudio(bool bEnable)
{
	if (m_pMFRObj)
		_In_MFR_EnableAudio(m_pMFRObj, bEnable);
}

double FFMediaFileDll::GetAudioLength()
{
	if (m_pMFRObj)
		return _In_MFR_GetAudioLength(m_pMFRObj);
}

int FFMediaFileDll::GetTotalOfFrames()
{
	if (m_pMFRObj)
		return _In_MFR_GetTotalOfFrames(m_pMFRObj);
}

char* FFMediaFileDll::PosToTimeInfo(double dPos)
{
	int iHour = dPos / (60 * 60);

	double dNum = dPos - 3600 * iHour;
	int iMinute = dNum / 60;

	double dNum1 = dNum - 60 * iMinute;
	int iSecond = dNum1;

	if (iHour > 0)
		sprintf(m_szTime, "%02d:%02d:%02d", iHour, iMinute, iSecond);
	else
		sprintf(m_szTime, "%02d:%02d", iMinute, iSecond);
	return m_szTime;
}

int FFMediaFileDll::Scale(int iPixFmt, unsigned char* pSrcBuffer, unsigned char* pDestBuffer, int iSrcW, int iSrcH, int iDestW, int iDestH)
{
	int w2, h2;
	int size, size2;
	int chroma_w = 0;
	int chroma_h = 0;
	int srcStride[4];
	int dstStride[4];
	int sws_flags = SWS_FAST_BILINEAR;
	if (m_ScaleSrcSize.cx != iSrcW || m_ScaleSrcSize.cy != iSrcH
		|| m_ScaleDestSize.cx != iDestW || m_ScaleDestSize.cy != iDestH
		|| m_iScalePixFmt != iPixFmt)
	{
		m_ScaleSrcSize.cx = iSrcW;
		m_ScaleSrcSize.cy = iSrcH;
		m_ScaleDestSize.cx = iDestW;
		m_ScaleDestSize.cy = iDestH;
		m_iScalePixFmt = iPixFmt;

		if (img_scale_ctx)
			sws_freeContext(img_scale_ctx);

		img_scale_ctx = sws_getContext(iSrcW, iSrcH,
			(enum AVPixelFormat)iPixFmt,
			iDestW, iDestH,
			(enum AVPixelFormat)iPixFmt,
			sws_flags, NULL, NULL, NULL);
	}

	void GetPixFmtDescriptor(int iPixFmt, int& iWidth, int& iHeight);
	GetPixFmtDescriptor(m_iScalePixFmt, chroma_w, chroma_h);

	switch (m_iScalePixFmt)
	{
	case AV_PIX_FMT_YUV420P:
	case AV_PIX_FMT_YUV422P:
	case AV_PIX_FMT_YUV444P:
	case AV_PIX_FMT_YUV410P:
	case AV_PIX_FMT_YUV411P:
	case AV_PIX_FMT_YUV440P:
	case AV_PIX_FMT_YUVJ420P:
	case AV_PIX_FMT_YUVJ422P:
	case AV_PIX_FMT_YUVJ444P:
	case AV_PIX_FMT_YUVJ440P:
		//case PIX_FMT_YUYV422:
	{
		w2 = (iSrcW + (1 << chroma_w) - 1) >> chroma_w;
		srcStride[0] = iSrcW;
		srcStride[1] = w2;
		srcStride[2] = w2;

		size = srcStride[0] * iSrcH;
		h2 = (iSrcH + (1 << chroma_h) - 1) >> chroma_h;
		size2 = srcStride[1] * h2;

		src[0] = pSrcBuffer;
		src[1] = src[0] + size;
		src[2] = src[1] + size2;
		src[3] = NULL;
	}
	break;
	case AV_PIX_FMT_BGR555:
	case AV_PIX_FMT_BGR565:
	case AV_PIX_FMT_RGB555:
	case AV_PIX_FMT_RGB565:
	case AV_PIX_FMT_YUYV422:
	{
		srcStride[0] = iSrcW * 2;

		src[0] = pSrcBuffer;
		src[1] = NULL;
		src[2] = NULL;
		src[3] = NULL;
	}
	break;
	case AV_PIX_FMT_RGB24:
	case AV_PIX_FMT_BGR24:
	{
		srcStride[0] = iSrcW * 3;

		src[0] = pSrcBuffer;
		src[1] = NULL;
		src[2] = NULL;
		src[3] = NULL;
	}
	break;
	case AV_PIX_FMT_ARGB:
	case AV_PIX_FMT_ABGR:
	case AV_PIX_FMT_RGBA:
	case AV_PIX_FMT_BGRA:
	{
		srcStride[0] = iSrcW * 4;

		src[0] = pSrcBuffer;
		src[1] = NULL;
		src[2] = NULL;
		src[3] = NULL;
	}
	break;
	case AV_PIX_FMT_NV12:
	case AV_PIX_FMT_NV21:
	{
		srcStride[0] = iSrcW;
		srcStride[1] = iSrcW / 2;

		size = srcStride[0] * iSrcH;
		src[0] = pSrcBuffer;
		src[1] = src[0] + size;
		src[2] = NULL;
		src[3] = NULL;
	}
	break;
	}

	switch (m_iScalePixFmt)
	{
	case AV_PIX_FMT_YUV420P:
	case AV_PIX_FMT_YUV422P:
	case AV_PIX_FMT_YUV444P:
	case AV_PIX_FMT_YUV410P:
	case AV_PIX_FMT_YUV411P:
	case AV_PIX_FMT_YUV440P:
	case AV_PIX_FMT_YUVJ420P:
	case AV_PIX_FMT_YUVJ422P:
	case AV_PIX_FMT_YUVJ444P:
	case AV_PIX_FMT_YUVJ440P:
		//case PIX_FMT_YUYV422:
	{
		w2 = (iDestW + (1 << chroma_w) - 1) >> chroma_w;
		//dstStride[0] = iDestW * 2;
		dstStride[0] = iDestW;
		dstStride[1] = w2;
		dstStride[2] = w2;

		size = dstStride[0] * iDestH;
		h2 = (iDestH + (1 << chroma_h) - 1) >> chroma_h;
		size2 = dstStride[1] * h2;

		dst[0] = pDestBuffer;
		dst[1] = dst[0] + size;
		dst[2] = dst[1] + size2;
		dst[3] = NULL;
	}
	break;
	case AV_PIX_FMT_BGR555:
	case AV_PIX_FMT_BGR565:
	case AV_PIX_FMT_RGB555:
	case AV_PIX_FMT_RGB565:
	case AV_PIX_FMT_YUYV422:
	{
		dstStride[0] = iDestW * 2;

		dst[0] = pDestBuffer;
		dst[1] = NULL;
		dst[2] = NULL;
		dst[3] = NULL;
	}
	break;
	case AV_PIX_FMT_RGB24:
	case AV_PIX_FMT_BGR24:
	{
		dstStride[0] = iDestW * 3;

		dst[0] = pDestBuffer;
		dst[1] = NULL;
		dst[2] = NULL;
		dst[3] = NULL;
	}
	break;
	case AV_PIX_FMT_ARGB:
	case AV_PIX_FMT_ABGR:
	case AV_PIX_FMT_RGBA:
	case AV_PIX_FMT_BGRA:
	{
		dstStride[0] = iDestW * 4;

		dst[0] = pDestBuffer;
		dst[1] = NULL;
		dst[2] = NULL;
		dst[3] = NULL;
	}
	break;
	case AV_PIX_FMT_NV12:
	case AV_PIX_FMT_NV21:
	{
		dstStride[0] = iDestW;
		dstStride[1] = iDestW / 2;

		size = dstStride[0] * iDestH;
		dst[0] = pDestBuffer;
		dst[1] = dst[0] + size;
		dst[2] = NULL;
		dst[3] = NULL;
	}
	break;
	}

	int iRet = sws_scale(img_scale_ctx, src, srcStride,
		0, iSrcH, dst, dstStride);
	return iRet;
}