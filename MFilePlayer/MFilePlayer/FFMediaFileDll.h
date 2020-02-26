#pragma once

#ifndef _FFMediaFileDll_H_
#define _FFMediaFileDll_H_

#include "../../../Include/MediaFrameInfo.h"

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
typedef int (*_In_MFR_GetTotalOfFrames_Proc)(void* pMFRObj);
typedef double (*_In_MFR_GetPreviousIFramePos_Proc)(void* pMFRObj);
typedef int (*_In_MFR_GetNextFrame_Proc)(void* pMFRObj, AVPacket* pkt, bool bUpdatePos);
typedef bool (*_In_MFR_IsVideoFrame_Proc)(void* pMFRObj, AVPacket* pkt);
typedef bool(*_In_MFR_IsAudioFrame_Proc)(void* pMFRObj, AVPacket* pkt);
typedef double (*_In_MFR_GetFramePos_Proc)(void* pMFRObj, int iType, int64_t pts);
typedef void (*_In_MFR_SetGOP_Proc)(void* pMFRObj, int iValue);
typedef int (*_In_MFR_SeekForBackward_Proc)(void* pMFRObj, double pos);
typedef int (*_In_MFR_SeekBackward_Proc)(void* pMFRObj);
typedef void (*_In_MFR_SetLastBackwardPos_Proc)(void* pMFRObj, double dPos);
typedef int (*_In_MFR_GetFrameBuffer_Proc)(void* pMFRObj, AVPacket* pkt, MediaFrameBuffer* pFrameBuf);
typedef double (*_In_MFR_GetLastBackwardPos_Proc)(void* pMFRObj);
typedef void (*_In_MFR_SetupDXVA2_Proc)(void* pMFRObj, void* pD3DDevice);
typedef void (*_In_MFR_EnableGPU_Proc)(void* pMFRObj, bool bEnable);
typedef bool (*_In_MFR_GPUIsEnabled_Proc)(void* pMFRObj);
typedef bool (*_In_MFR_CopyFrame_Proc)(void* pMFRObj, AVFrame* pInFrame, unsigned char* pOutBuffer);
typedef void (*_In_MFR_Suspend_Proc)(void* pMFRObj);
typedef void (*_In_MFR_Resume_Proc)(void* pMFRObj);
typedef bool (*_In_MFR_FormatIsUnSupported_Proc)(void* pMFRObj);
typedef void(*_In_MFR_SetReSampleRate_Proc)(void* pMFRObj, float fRate);
//typedef void(*_In_MFR_IncreaseSamplesOffset_Proc)(void* pMFRObj);
//typedef void(*_In_MFR_DecreaseSamplesOffset_Proc)(void* pMFRObj);

class FFMediaFileDll
{
	HINSTANCE m_hInst;
	void* m_pMFRObj;

	char m_szFileName[512];

	CRITICAL_SECTION m_CriticalSection;
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
	_In_MFR_GetPreviousIFramePos_Proc _In_MFR_GetPreviousIFramePos;
	_In_MFR_GetNextFrame_Proc _In_MFR_GetNextFrame;
	_In_MFR_IsVideoFrame_Proc _In_MFR_IsVideoFrame;
	_In_MFR_IsAudioFrame_Proc _In_MFR_IsAudioFrame;
	_In_MFR_GetFramePos_Proc _In_MFR_GetFramePos;
	_In_MFR_SetGOP_Proc _In_MFR_SetGOP;
	_In_MFR_SeekForBackward_Proc _In_MFR_SeekForBackward;
	_In_MFR_SeekBackward_Proc _In_MFR_SeekBackward;
	_In_MFR_SetLastBackwardPos_Proc _In_MFR_SetLastBackwardPos;
	_In_MFR_GetFrameBuffer_Proc _In_MFR_GetFrameBuffer;
	_In_MFR_GetLastBackwardPos_Proc _In_MFR_GetLastBackwardPos;
	_In_MFR_SetupDXVA2_Proc _In_MFR_SetupDXVA2;
	_In_MFR_EnableGPU_Proc _In_MFR_EnableGPU;
	_In_MFR_GPUIsEnabled_Proc _In_MFR_GPUIsEnabled;
	_In_MFR_CopyFrame_Proc _In_MFR_CopyFrame;
	_In_MFR_Suspend_Proc _In_MFR_Suspend;
	_In_MFR_Resume_Proc _In_MFR_Resume;
	_In_MFR_FormatIsUnSupported_Proc _In_MFR_FormatIsUnSupported;
	_In_MFR_SetReSampleRate_Proc _In_MFR_SetReSampleRate;
	//_In_MFR_IncreaseSamplesOffset_Proc _In_MFR_IncreaseSamplesOffset;
	//_In_MFR_DecreaseSamplesOffset_Proc _In_MFR_DecreaseSamplesOffset;

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
	double GetPreviousIFramePos();
	int GetNextFrame(AVPacket* pkt, bool bUpdatePos);
	bool IsVideoFrame(AVPacket* pkt);
	bool IsAudioFrame(AVPacket* pkt);
	double GetFramePos(int iType, int64_t pts);
	void SetGOP(int iValue);
	int SeekForBackward(double pos);
	int SeekBackward();
	void SetLastBackwardPos(double dPos);
	int GetFrameBuffer(AVPacket* pkt, MediaFrameBuffer* pFrameBuf);
	double GetLastBackwardPos();

	void SetupDXVA2(void* pD3DDevice);
	void EnableGPU(bool bEnable);
	bool GPUIsEnabled();
	bool CopyFrame(AVFrame* pInFrame, unsigned char* pOutBuffer);

	void Suspend();
	void Resume();
	bool FormatIsUnSupported();
	void SetReSampleRate(float fRate);
	char* GetFileName();
	//void IncreaseSamplesOffset();
	//void DecreaseSamplesOffset();
};

#endif
