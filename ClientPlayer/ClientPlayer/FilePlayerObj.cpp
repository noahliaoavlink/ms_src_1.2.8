#include "stdafx.h"
#include "FilePlayerObj.h"
#include "..\\..\\Include\\StrConv.h"

#include "LayoutCtrl.h"

/*
extern void gUpdatePosInfo(char* szCurPos, char* szTotalLen);
extern void gSetSliderCtrlRange(double dPos);
extern void gSetSliderCtrlPos(double dPos);
*/
FilePlayerObj::FilePlayerObj()
{
	m_pD3DRender = 0;
	m_pMFilePlayerDll = 0;

	m_iDisplayMode = DM_NORMAL;

	m_bEnableUpdatePos = true;
}

FilePlayerObj::~FilePlayerObj()
{
	Stop();
	Close();

	if (m_pD3DRender)
		delete m_pD3DRender;

	if (m_pMFilePlayerDll)
		delete m_pMFilePlayerDll;
}

void FilePlayerObj::Init(HWND hWnd)
{
	m_pD3DRender = new D3DRender;
	m_pD3DRender->Init(hWnd, VPF_I420,1920,1080);
	m_pD3DRender->Clear();
	m_pD3DRender->Flip();

	m_pMFilePlayerDll = new MFilePlayerDll;
	m_pMFilePlayerDll->LoadLib();
	m_pMFilePlayerDll->Init(hWnd, this);
	m_pMFilePlayerDll->SetOutputVideoFormat(VPF_I420);
	m_pMFilePlayerDll->SetDisplayIntervalMode(DIM_GOBAL);

	m_pMFilePlayerDll->EnableGPU(true);
	m_pMFilePlayerDll->SetupDXVA2(m_pD3DRender->GetD3DDevice());
}

void FilePlayerObj::SetDisplayMode(int iMode)
{
	m_iDisplayMode = iMode;
}

void FilePlayerObj::SetLayoutInfo(int iID,int iIndex)
{
	m_iLayoutID = iID;
	m_iLayoutIndex = iIndex;
}

void FilePlayerObj::Open(wchar_t* wszFileName)
{
	std::string sFileName = UnicodeToUTF8(wszFileName);
	m_pMFilePlayerDll->Open((char*)sFileName.c_str());

	float fFPS = m_pMFilePlayerDll->GetVideoFPS();
	int iTotalOfFrames = m_pMFilePlayerDll->GetTotalOfFrames();
	int w = m_pMFilePlayerDll->GetVideoWidth();
	int h = m_pMFilePlayerDll->GetVideoHeight();

	m_pMFilePlayerDll->SetRepeat(false);
}

void FilePlayerObj::Close()
{
	if(m_pMFilePlayerDll)
		m_pMFilePlayerDll->Close();
}

void FilePlayerObj::Play()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->Play();
}

void FilePlayerObj::Stop()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->Stop();
}

void FilePlayerObj::Seek(int iPos)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->Seek(iPos);
}

void FilePlayerObj::SpeedUp()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SpeedUp();
}

void FilePlayerObj::SpeedDown()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SpeedDown();
}

int FilePlayerObj::GetVoluem()
{
	if (m_pMFilePlayerDll)
		return m_pMFilePlayerDll->GetVolume();
	return 0;
}

void FilePlayerObj::SetVolume(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetVolume(iValue);
}

int FilePlayerObj::GetSystemVoluem()
{
	if (m_pMFilePlayerDll)
		return m_pMFilePlayerDll->GetSystemVolume();
	return 0;
}

void FilePlayerObj::SetSystemVolume(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetSystemVolume(iValue);
}

void FilePlayerObj::SetBrightness(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetBrightness(iValue);
}

void FilePlayerObj::SetContrast(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetContrast(iValue);
}

void FilePlayerObj::SetSaturation(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetSaturation(iValue);
}

void FilePlayerObj::SetHue(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetHue(iValue);
}

void FilePlayerObj::SetR(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetR(iValue);
}

void FilePlayerObj::SetG(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetG(iValue);
}

void FilePlayerObj::SetB(int iValue)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->SetB(iValue);
}

void FilePlayerObj::PlayBackward()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->PlayBackward();
}

int FilePlayerObj::GetPlaybackMode()
{
	if (m_pMFilePlayerDll)
		return m_pMFilePlayerDll->GetPlaybackMode();
	return 0;
}

void FilePlayerObj::Pause()
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->Pause();
}

void FilePlayerObj::UpdateGlobalTimeCode(double dTimeCode)
{
	if (m_pMFilePlayerDll)
		m_pMFilePlayerDll->UpdateGlobalTimeCode(dTimeCode);
}

void FilePlayerObj::EnableUpdatePos(bool bEnable)
{
	m_bEnableUpdatePos = bEnable;
}

//format - 00:00 / 02:00
void FilePlayerObj::UpdatePosInfo(char* szCurPos, char* szTotalLen)
{
//	gUpdatePosInfo(szCurPos, szTotalLen);
}

//total len
void FilePlayerObj::SetSliderCtrlRange(double dPos)
{
//	gSetSliderCtrlRange(dPos);
}

//cur pos
void FilePlayerObj::SetSliderCtrlPos(double dPos)
{
//	if (m_bEnableUpdatePos)
	//	gSetSliderCtrlPos(dPos);
}

void FilePlayerObj::DisplayVideo(unsigned char* pBuffer, int w, int h)
{
	if (m_pD3DRender->GetWidth() != w || m_pD3DRender->GetHeight() != h)
	{
		int iPixelFormat = VPF_I420;
		int iRet = m_pD3DRender->RestSurfaceFormat(iPixelFormat, w, h);
	}

	m_pD3DRender->RenderFrame(pBuffer,w,h);
	m_pD3DRender->Flip();
}

void FilePlayerObj::DisplayVideo_HW(void* pD3DDevice, unsigned char* pBuffer, int w, int h)
{
	if (m_pMFilePlayerDll->GPUIsEnabled())
	{
		switch (m_iDisplayMode)
		{
			case DM_NORMAL:
				m_pD3DRender->RenderFrame1((IDirect3DSurface9*)pBuffer, w, h);
				break;
			case DM_TVWALL:
				{
					LayoutCtrl layout_ctrl;
					RECT rSrc;

					layout_ctrl.MakeInfo(m_iLayoutID, w, h);
					RECT* pRect = layout_ctrl.GetGridRect(m_iLayoutIndex);
					memcpy(&rSrc, pRect,sizeof(RECT));
					m_pD3DRender->RenderFrame1((IDirect3DSurface9*)pBuffer, w, h, rSrc); 
				}
				break;
		}
	}
	m_pD3DRender->Flip();
}

void FilePlayerObj::PlayAudio(unsigned char* pBuffer, int iLen)
{

}

void FilePlayerObj::Event(int iType)
{
}
