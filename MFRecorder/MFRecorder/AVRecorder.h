#pragma once

#ifndef _AVRecorder_H_
#define _AVRecorder_H_

#include "FFMediaFileDll.h"
#include "YUVConverter.h"

class AVRecorder
{
	FFMediaFileDll* m_pFFMediaFileDll;

	int m_iWidth;
	int m_iHeight;
	YUVConverter m_YUVConverter;
	unsigned char* m_pI420Buffer;
public:
	AVRecorder();
	~AVRecorder();
	
	void Init();
	int Open(char* szFileName);
	void Close();
	void SetVideoInfo(int iWidth, int iHeight, int iBitRate, float fFps);
	void SetAudioInfo(int iChannels, int iSamples, int iBitRate);
	void WriteHeader();
	int WriteVideoFrame(unsigned char* pBuffer, int iLen);
	int WriteAudioFrame(unsigned char* pBuffer, int iLen);
	void SetOriginalAudioInfo(int iChannelLayout, int iSampleRate, int iNBSamples);
};

#endif
