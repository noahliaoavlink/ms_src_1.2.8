#include "stdafx.h"
#include "FFMediaFileDll.h"

FFMediaFileDll::FFMediaFileDll()
{
	m_hInst = 0;
	m_pMFRObj = 0;

	strcpy(m_szFileName,"");

	InitializeCriticalSection(&m_CriticalSection);
}

FFMediaFileDll::~FFMediaFileDll()
{
	if (m_pMFRObj)
		_In_MFR_Destroy(m_pMFRObj);

	FreeLib();
	DeleteCriticalSection(&m_CriticalSection);
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
		_In_MFR_GetPreviousIFramePos = (_In_MFR_GetPreviousIFramePos_Proc)GetProcAddress(m_hInst, "_In_MFR_GetPreviousIFramePos");
		_In_MFR_GetNextFrame = (_In_MFR_GetNextFrame_Proc)GetProcAddress(m_hInst, "_In_MFR_GetNextFrame");
		_In_MFR_IsVideoFrame = (_In_MFR_IsVideoFrame_Proc)GetProcAddress(m_hInst, "_In_MFR_IsVideoFrame");
		_In_MFR_IsAudioFrame = (_In_MFR_IsAudioFrame_Proc)GetProcAddress(m_hInst, "_In_MFR_IsAudioFrame");
		_In_MFR_GetFramePos = (_In_MFR_GetFramePos_Proc)GetProcAddress(m_hInst, "_In_MFR_GetFramePos");
		_In_MFR_SetGOP = (_In_MFR_SetGOP_Proc)GetProcAddress(m_hInst, "_In_MFR_SetGOP");

		_In_MFR_SeekForBackward = (_In_MFR_SeekForBackward_Proc)GetProcAddress(m_hInst, "_In_MFR_SeekForBackward");
		_In_MFR_SeekBackward = (_In_MFR_SeekBackward_Proc)GetProcAddress(m_hInst, "_In_MFR_SeekBackward");
		_In_MFR_SetLastBackwardPos = (_In_MFR_SetLastBackwardPos_Proc)GetProcAddress(m_hInst, "_In_MFR_SetLastBackwardPos");
		_In_MFR_GetFrameBuffer = (_In_MFR_GetFrameBuffer_Proc)GetProcAddress(m_hInst, "_In_MFR_GetFrameBuffer");

		_In_MFR_GetLastBackwardPos = (_In_MFR_GetLastBackwardPos_Proc)GetProcAddress(m_hInst, "_In_MFR_GetLastBackwardPos");

		_In_MFR_SetupDXVA2 = (_In_MFR_SetupDXVA2_Proc)GetProcAddress(m_hInst, "_In_MFR_SetupDXVA2");
		_In_MFR_EnableGPU = (_In_MFR_EnableGPU_Proc)GetProcAddress(m_hInst, "_In_MFR_EnableGPU");
		_In_MFR_GPUIsEnabled = (_In_MFR_GPUIsEnabled_Proc)GetProcAddress(m_hInst, "_In_MFR_GPUIsEnabled");
		_In_MFR_CopyFrame = (_In_MFR_CopyFrame_Proc)GetProcAddress(m_hInst, "_In_MFR_CopyFrame");

		_In_MFR_Suspend = (_In_MFR_Suspend_Proc)GetProcAddress(m_hInst, "_In_MFR_Suspend");
		_In_MFR_Resume = (_In_MFR_Resume_Proc)GetProcAddress(m_hInst, "_In_MFR_Resume");
		_In_MFR_FormatIsUnSupported = (_In_MFR_FormatIsUnSupported_Proc)GetProcAddress(m_hInst, "_In_MFR_FormatIsUnSupported");
		_In_MFR_SetReSampleRate = (_In_MFR_SetReSampleRate_Proc)GetProcAddress(m_hInst, "_In_MFR_SetReSampleRate");

		//_In_MFR_IncreaseSamplesOffset = (_In_MFR_IncreaseSamplesOffset_Proc)GetProcAddress(m_hInst, "_In_MFR_IncreaseSamplesOffset");
		//_In_MFR_DecreaseSamplesOffset = (_In_MFR_DecreaseSamplesOffset_Proc)GetProcAddress(m_hInst, "_In_MFR_DecreaseSamplesOffset");

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
	{
		strcpy(m_szFileName, szFileName);
		return _In_MFR_Open(m_pMFRObj, szFileName);
	}
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

double FFMediaFileDll::GetPreviousIFramePos()
{
	if (m_pMFRObj)
		return _In_MFR_GetPreviousIFramePos(m_pMFRObj);
}

int FFMediaFileDll::GetNextFrame(AVPacket* pkt, bool bUpdatePos)
{
	if (m_pMFRObj)
		return _In_MFR_GetNextFrame(m_pMFRObj, pkt, bUpdatePos);
}

bool FFMediaFileDll::IsVideoFrame(AVPacket* pkt)
{
	if (m_pMFRObj)
		return _In_MFR_IsVideoFrame(m_pMFRObj, pkt);
}

bool FFMediaFileDll::IsAudioFrame(AVPacket* pkt)
{
	if (m_pMFRObj)
		return _In_MFR_IsAudioFrame(m_pMFRObj, pkt);
}

double FFMediaFileDll::GetFramePos(int iType, int64_t pts)
{
	if (m_pMFRObj)
		return _In_MFR_GetFramePos(m_pMFRObj,iType, pts);
}

void FFMediaFileDll::SetGOP(int iValue)
{
	if (m_pMFRObj)
		_In_MFR_SetGOP(m_pMFRObj, iValue);
}

int FFMediaFileDll::SeekForBackward(double pos)
{
	if (m_pMFRObj)
		return _In_MFR_SeekForBackward(m_pMFRObj, pos);
}

int FFMediaFileDll::SeekBackward()
{
	if (m_pMFRObj)
		return _In_MFR_SeekBackward(m_pMFRObj);
}

void FFMediaFileDll::SetLastBackwardPos(double dPos)
{
	if (m_pMFRObj)
	_In_MFR_SetLastBackwardPos(m_pMFRObj, dPos);
}

int FFMediaFileDll::GetFrameBuffer(AVPacket* pkt, MediaFrameBuffer* pFrameBuf)
{
//	EnterCriticalSection(&m_CriticalSection);
	if (m_pMFRObj)
	{
		int iRet = _In_MFR_GetFrameBuffer(m_pMFRObj, pkt, pFrameBuf);
//		LeaveCriticalSection(&m_CriticalSection);
		return iRet;
	}
}

double FFMediaFileDll::GetLastBackwardPos()
{
	if (m_pMFRObj)
		return _In_MFR_GetLastBackwardPos(m_pMFRObj);
}

void FFMediaFileDll::SetupDXVA2(void* pD3DDevice)
{
	if (m_pMFRObj)
		_In_MFR_SetupDXVA2(m_pMFRObj, pD3DDevice);
}

void FFMediaFileDll::EnableGPU(bool bEnable)
{
	if (m_pMFRObj)
		_In_MFR_EnableGPU(m_pMFRObj, bEnable);
}

bool FFMediaFileDll::GPUIsEnabled()
{
	if (m_pMFRObj)
		return _In_MFR_GPUIsEnabled(m_pMFRObj);
}

bool FFMediaFileDll::CopyFrame(AVFrame* pInFrame, unsigned char* pOutBuffer)
{
	if (m_pMFRObj)
		return _In_MFR_CopyFrame(m_pMFRObj, pInFrame, pOutBuffer);
}

void FFMediaFileDll::Suspend()
{
	if (m_pMFRObj)
		_In_MFR_Suspend(m_pMFRObj);
}

void FFMediaFileDll::Resume()
{
	if (m_pMFRObj)
		_In_MFR_Resume(m_pMFRObj);
}

bool FFMediaFileDll::FormatIsUnSupported()
{
	if (m_pMFRObj)
		return _In_MFR_FormatIsUnSupported(m_pMFRObj);
}

void FFMediaFileDll::SetReSampleRate(float fRate)
{
	if (m_pMFRObj)
		_In_MFR_SetReSampleRate(m_pMFRObj, fRate);
}

char* FFMediaFileDll::GetFileName()
{
	return m_szFileName;
}

/*
void FFMediaFileDll::IncreaseSamplesOffset()
{
	if (m_pMFRObj)
		_In_MFR_IncreaseSamplesOffset(m_pMFRObj);
}

void FFMediaFileDll::DecreaseSamplesOffset()
{
	if (m_pMFRObj)
		_In_MFR_DecreaseSamplesOffset(m_pMFRObj);
}
*/
