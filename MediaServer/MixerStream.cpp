#include "stdafx.h"
#include "MixerStream.h"

//extern void gUpdatePosInfo(char* szCurPos, char* szTotalLen);
//extern void gSetSliderCtrlRange(int iPos);
//extern void gSetSliderCtrlPos(int iPos);

CMixerStream::CMixerStream()
{
	m_pD3DRender = 0;
	m_pMFilePlayerDll = 0;
	m_bEnableUpdatePos = true;
}

CMixerStream::~CMixerStream()
{
	if (m_pD3DRender)
		delete m_pD3DRender;
}

void CMixerStream::SetMFilePlayerDll(MixerPlayerDll* pObj)
{
	m_pMFilePlayerDll = pObj;
}

void CMixerStream::Init(HWND hWnd,int iIndex)
{
	m_iIndex = iIndex;

	m_pD3DRender = new D3DRender;
	m_pD3DRender->Init(hWnd, VPF_I420,1920,1080);
	m_pD3DRender->Clear();
	m_pD3DRender->Flip();

	m_pMFilePlayerDll->Create(m_iIndex);
	m_pMFilePlayerDll->Init(m_iIndex,hWnd, this);
	m_pMFilePlayerDll->SetOutputVideoFormat(m_iIndex, VPF_I420);
}

void CMixerStream::Open(char* szFileName)
{
	m_pMFilePlayerDll->Open(m_iIndex, szFileName);

	float fFPS = m_pMFilePlayerDll->GetVideoFPS(m_iIndex);
	int iTotalOfFrames = m_pMFilePlayerDll->GetTotalOfFrames(m_iIndex);
	int w = m_pMFilePlayerDll->GetVideoWidth(m_iIndex);
	int h = m_pMFilePlayerDll->GetVideoHeight(m_iIndex);
}

void CMixerStream::Close()
{
	if(m_pMFilePlayerDll)
		m_pMFilePlayerDll->Close(m_iIndex);
}

void CMixerStream::Play()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->Play(m_iIndex);
}

void CMixerStream::Stop()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->Stop(m_iIndex);
}

void CMixerStream::Seek(int iPos)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->Seek(m_iIndex,iPos);
}

void CMixerStream::EnableAudio(bool bEnable)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->EnableAudio(m_iIndex, bEnable);
}

void CMixerStream::EnableUpdatePos(bool bEnable)
{
	m_bEnableUpdatePos = bEnable;
}

void CMixerStream::SpeedUp()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SpeedUp(m_iIndex);
}

void CMixerStream::SpeedDown()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SpeedDown(m_iIndex);
}

int CMixerStream::GetVolume()
{
	if (m_pMFilePlayerDll)
		return m_pMFilePlayerDll->GetVolume(m_iIndex);
	return 0;
}

void CMixerStream::SetVolume(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetVolume(m_iIndex,iValue);
}

void CMixerStream::SetBrightness(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetBrightness(m_iIndex, iValue);
}

void CMixerStream::SetContrast(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetContrast(m_iIndex, iValue);
}

void CMixerStream::SetSaturation(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetSaturation(m_iIndex, iValue);
}

//format - 00:00 / 02:00
void CMixerStream::UpdatePosInfo(char* szCurPos, char* szTotalLen)
{
	if(m_iIndex == 0)
	{
		;
		//provide mixer UI slider pos infomation
		//gUpdatePosInfo(szCurPos, szTotalLen);
	}
}

//total len
void CMixerStream::SetSliderCtrlRange(int iPos)
{
	if (m_iIndex == 0)
	{
		;
		//set mixer UI slider Range
		//gSetSliderCtrlRange(iPos);
	}
}

//cur pos
void CMixerStream::SetSliderCtrlPos(int iPos)
{
	if (m_iIndex == 0)
	{
		if (m_bEnableUpdatePos)
			;
			//set mixer UI slider pos
			//gSetSliderCtrlPos(iPos);
	}
}

void CMixerStream::DisplayVideo(unsigned char* pBuffer, int w, int h)
{
	if (m_pD3DRender->GetWidth() != w || m_pD3DRender->GetHeight() != h)
	{
		int iPixelFormat = VPF_I420;
		int iRet = m_pD3DRender->RestSurfaceFormat(iPixelFormat, w, h);
	}

	m_pD3DRender->RenderFrame(pBuffer,w,h);
	m_pD3DRender->Flip();
}

