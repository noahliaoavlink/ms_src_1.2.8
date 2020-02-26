#include "stdafx.h"
#include "AVRecorder.h"

AVRecorder::AVRecorder()
{
	m_pFFMediaFileDll = new FFMediaFileDll;

	m_pI420Buffer = 0;
	m_iWidth = 0;
	m_iHeight = 0;
}

AVRecorder::~AVRecorder()
{
	if (m_pFFMediaFileDll)
		delete m_pFFMediaFileDll;

	if (m_pI420Buffer)
		delete m_pI420Buffer;
}

void AVRecorder::Init()
{
	bool bLoadDecoderRet = m_pFFMediaFileDll->LoadLib();
	if (!bLoadDecoderRet)
		MessageBoxA(NULL, "Load FFMediaFile.dll failed!!", "Error", MB_OK | MB_TOPMOST);

	m_pFFMediaFileDll->AVI_Init();
}

int AVRecorder::Open(char* szFileName)
{
	return m_pFFMediaFileDll->AVI_Open(szFileName);
}

void AVRecorder::Close()
{
	m_pFFMediaFileDll->AVI_Close();
}

void AVRecorder::SetVideoInfo(int iWidth, int iHeight, int iBitRate, float fFps)
{
	if (m_pI420Buffer)
		delete m_pI420Buffer;

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	m_pI420Buffer = new unsigned char[iWidth * iHeight * 3 / 2];

	m_pFFMediaFileDll->AVI_SetVideoInfo(iWidth,iHeight,iBitRate,fFps);
}

void AVRecorder::SetAudioInfo(int iChannels, int iSamples, int iBitRate)
{
	m_pFFMediaFileDll->AVI_SetAudioInfo(iChannels,iSamples,iBitRate);
}

void AVRecorder::WriteHeader()
{
	m_pFFMediaFileDll->AVI_WriteHeader();
}

int AVRecorder::WriteVideoFrame(unsigned char* pBuffer, int iLen)
{
	m_YUVConverter.ARGBToI420(pBuffer, m_pI420Buffer, m_iWidth, m_iHeight);
	return m_pFFMediaFileDll->AVI_WriteVideoFrame(m_pI420Buffer,iLen);
}

int AVRecorder::WriteAudioFrame(unsigned char* pBuffer, int iLen)
{
	return m_pFFMediaFileDll->AVI_WriteAudioFrame(pBuffer,iLen);
}

void AVRecorder::SetOriginalAudioInfo(int iChannelLayout, int iSampleRate, int iNBSamples)
{
	m_pFFMediaFileDll->AVI_SetOriginalAudioInfo(iChannelLayout,iSampleRate,iNBSamples);
}
