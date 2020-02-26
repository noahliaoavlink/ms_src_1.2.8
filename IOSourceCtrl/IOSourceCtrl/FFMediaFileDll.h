#pragma once

#ifndef _FFMediaFileDll_H_
#define _FFMediaFileDll_H_

#include "../../Include/MediaFrameInfo.h"

#ifdef __cplusplus
extern "C"
{

#include "libswscale/swscale.h"
}
#endif //__cplusplus

typedef void* (*_In_MFR_Create_Proc)();
typedef void (*_In_MFR_Destroy_Proc)(void* pMFRObj);
typedef int (*_In_MFR_Open_Proc)(void* pMFRObj, char* szFileName);
typedef void (*_In_MFR_Close_Proc)(void* pMFRObj);
typedef void (*_In_MFR_GetVideoFrameSize_Proc)(void* pMFRObj, int* w, int* h);
typedef float (*_In_MFR_GetVideoFPS_Proc)(void* pMFRObj);
typedef int (*_In_MFR_GetNextFrameBuffer_Proc)(void* pMFRObj, MediaFrameBuffer* pFrameBuf);
typedef AVFrame* (*_In_MFR_GetAVFrame_Proc)(void* pMFRObj);
typedef AVCodecContext* (*_In_MFR_GetAVCodecContext_Proc)(void* pMFRObj, int iType);
typedef int (*_In_MFR_SeekTo_Proc)(void* pMFRObj, double pos);
typedef double (*_In_MFR_GetCurVideoFramePos_Proc)(void* pMFRObj);
typedef double (*_In_MFR_GetCurAudioFramePos_Proc)(void* pMFRObj);
typedef double (*_In_MFR_GetLength_Proc)(void* pMFRObj);
typedef void (*_In_MFR_EnableAudio_Proc)(void* pMFRObj, bool bEnable);
typedef double (*_In_MFR_GetAudioLength_Proc)(void* pMFRObj);
typedef int(*_In_MFR_GetTotalOfFrames_Proc)(void* pMFRObj);

class FFMediaFileDll
{
	HINSTANCE m_hInst;
	void* m_pMFRObj;
	char m_szTime[128];

	SIZE m_ScaleSrcSize;
	SIZE m_ScaleDestSize;
	int m_iScalePixFmt;
	struct SwsContext* img_scale_ctx;

	int srcStride[4];
	int dstStride[4];
	uint8_t* src[4];
	uint8_t* dst[4];
public:
	FFMediaFileDll();
	~FFMediaFileDll();

	_In_MFR_Create_Proc _In_MFR_Create;
	_In_MFR_Destroy_Proc _In_MFR_Destroy;
	_In_MFR_Open_Proc _In_MFR_Open;
	_In_MFR_Close_Proc _In_MFR_Close;
	_In_MFR_GetVideoFrameSize_Proc _In_MFR_GetVideoFrameSize;
	_In_MFR_GetVideoFPS_Proc _In_MFR_GetVideoFPS;
	_In_MFR_GetNextFrameBuffer_Proc _In_MFR_GetNextFrameBuffer;
	_In_MFR_GetAVFrame_Proc _In_MFR_GetAVFrame;
	_In_MFR_GetAVCodecContext_Proc _In_MFR_GetAVCodecContext;
	_In_MFR_SeekTo_Proc _In_MFR_SeekTo;
	_In_MFR_GetCurVideoFramePos_Proc _In_MFR_GetCurVideoFramePos;
	_In_MFR_GetCurAudioFramePos_Proc _In_MFR_GetCurAudioFramePos;
	_In_MFR_GetLength_Proc _In_MFR_GetLength;
	_In_MFR_EnableAudio_Proc _In_MFR_EnableAudio;
	_In_MFR_GetAudioLength_Proc _In_MFR_GetAudioLength;
	_In_MFR_GetTotalOfFrames_Proc _In_MFR_GetTotalOfFrames;


	bool LoadLib();
	void FreeLib();

	int Open(char* szFileName);
	void Close();
	void GetVideoFrameSize(int* w, int* h);
	float GetVideoFPS();
	int GetNextFrameBuffer(MediaFrameBuffer* pFrameBuf);
	AVFrame* GetAVFrame();
	AVCodecContext* GetAVCodecContext(int iType);
	int SeekTo(double pos);
	double GetCurVideoFramePos();
	double GetCurAudioFramePos();
	double GetLength();
	void EnableAudio(bool bEnable);
	double GetAudioLength();
	int GetTotalOfFrames();
	char* PosToTimeInfo(double dPos);
	int Scale(int iPixFmt, unsigned char* pSrcBuffer, unsigned char* pDestBuffer, int iSrcW, int iSrcH, int iDestW, int iDestH);

};

#endif
