#include "stdafx.h"
#include "..\\Include\\FFVideoFilterDll.h"

FFVideoFilterDll::FFVideoFilterDll()
{
	m_hInst = 0;
	m_pVFObj = 0;
}

FFVideoFilterDll::~FFVideoFilterDll()
{
	if(m_pVFObj)
		_VF_Destroy(m_pVFObj);

	FreeLib();
}

bool FFVideoFilterDll::LoadLib()
{
	
	char szFileName[] = "FFVideoFilter.dll";
	m_hInst = LoadLibraryA(szFileName);
	
	if(m_hInst)
	{
		_VF_Create = (_VF_Create_Proc)GetProcAddress(m_hInst, "_VF_Create");
		_VF_Destroy = (_VF_Destroy_Proc)GetProcAddress(m_hInst, "_VF_Destroy");
		_VF_CreateFadeInFilter = (_VF_CreateFadeInFilter_Proc)GetProcAddress(m_hInst, "_VF_CreateFadeInFilter");
		_VF_CreateFadeOutFilter = (_VF_CreateFadeOutFilter_Proc)GetProcAddress(m_hInst, "_VF_CreateFadeOutFilter");
		_VF_CreateEQFilter = (_VF_CreateEQFilter_Proc)GetProcAddress(m_hInst, "_VF_CreateEQFilter");
		_VF_CreateAformatFilter = (_VF_CreateAformatFilter_Proc)GetProcAddress(m_hInst, "_VF_CreateAformatFilter");
		_VF_CreateAtempoFilter = (_VF_CreateAtempoFilter_Proc)GetProcAddress(m_hInst, "_VF_CreateAtempoFilter");
		_VF_CloseFilter = (_VF_CloseFilter_Proc)GetProcAddress(m_hInst, "_VF_CloseFilter");
		_VF_InputFrame = (_VF_InputFrame_Proc)GetProcAddress(m_hInst, "_VF_InputFrame");
		_VF_OutputFrameBuffer = (_VF_OutputFrameBuffer_Proc)GetProcAddress(m_hInst, "_VF_OutputFrameBuffer");
		_VF_OutputFrame = (_VF_OutputFrame_Proc)GetProcAddress(m_hInst, "_VF_OutputFrame");
		_VF_InputFrame2 = (_VF_InputFrame2_Proc)GetProcAddress(m_hInst, "_VF_InputFrame2");
		_VF_OutputFrameBuffer2 = (_VF_OutputFrameBuffer2_Proc)GetProcAddress(m_hInst, "_VF_OutputFrameBuffer2");
		_VF_OutputFrame2 = (_VF_OutputFrame2_Proc)GetProcAddress(m_hInst, "_VF_OutputFrame2");
		_VF_AVFrameToBuffer = (_VF_AVFrameToBuffer_Proc)GetProcAddress(m_hInst, "_VF_AVFrameToBuffer");
		_VF_UnRefBuffer = (_VF_UnRefBuffer_Proc)GetProcAddress(m_hInst, "_VF_UnRefBuffer");
		
		_VF_SetBrightness = (_VF_SetBrightness_Proc)GetProcAddress(m_hInst, "_VF_SetBrightness");
		_VF_SetContrast = (_VF_SetContrast_Proc)GetProcAddress(m_hInst, "_VF_SetContrast");
		_VF_SetSaturation = (_VF_SetSaturation_Proc)GetProcAddress(m_hInst, "_VF_SetSaturation");

		_VF_SetGammaR = (_VF_SetGammaR_Proc)GetProcAddress(m_hInst, "_VF_SetGammaR");
		_VF_SetGammaG = (_VF_SetGammaG_Proc)GetProcAddress(m_hInst, "_VF_SetGammaG");
		_VF_SetGammaB = (_VF_SetGammaB_Proc)GetProcAddress(m_hInst, "_VF_SetGammaB");

		_VF_CreateHueFilter = (_VF_CreateHueFilter_Proc)GetProcAddress(m_hInst, "_VF_CreateHueFilter");
		_VF_SetHue = (_VF_SetHue_Proc)GetProcAddress(m_hInst, "_VF_SetHue");

		_VF_BeginFilter = (_VF_BeginFilter_Proc)GetProcAddress(m_hInst, "_VF_BeginFilter");
		_VF_EndFilter = (_VF_EndFilter_Proc)GetProcAddress(m_hInst, "_VF_EndFilter");

		_VF_RestartFadeIn = (_VF_RestartFadeIn_Proc)GetProcAddress(m_hInst, "_VF_RestartFadeIn");
		_VF_RestartFadeOut = (_VF_RestartFadeOut_Proc)GetProcAddress(m_hInst, "_VF_RestartFadeOut");
		_VF_SetFadeDuration = (_VF_SetFadeDuration_Proc)GetProcAddress(m_hInst, "_VF_SetFadeDuration");
		
		_VF_SetATempo = (_VF_SetATempo_Proc)GetProcAddress(m_hInst, "_VF_SetATempo");
		_VF_OutputFrameBuffer_Audio = (_VF_OutputFrameBuffer_Audio_Proc)GetProcAddress(m_hInst, "_VF_OutputFrameBuffer_Audio");

		_VF_CreateLutYUVFilter = (_VF_CreateLutYUVFilter_Proc)GetProcAddress(m_hInst, "_VF_CreateLutYUVFilter");
		_VF_SetGray = (_VF_SetGray_Proc)GetProcAddress(m_hInst, "_VF_SetGray");

		_VF_CreateLutRGBFilter = (_VF_CreateLutRGBFilter_Proc)GetProcAddress(m_hInst, "_VF_CreateLutRGBFilter");
		_VF_SetNegative = (_VF_SetNegative_Proc)GetProcAddress(m_hInst, "_VF_SetNegative");

		m_pVFObj = _VF_Create();
		
		return true;
	}
	return false;
}

void FFVideoFilterDll::FreeLib()
{
	if(m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = 0;
	}
}

void FFVideoFilterDll::CreateFadeInFilter(AVCodecContext* pDecCtx)
{
	if(m_pVFObj)
		_VF_CreateFadeInFilter(m_pVFObj,pDecCtx);
}

void FFVideoFilterDll::CreateFadeOutFilter(AVCodecContext* pDecCtx)
{
	if(m_pVFObj)
		_VF_CreateFadeOutFilter(m_pVFObj,pDecCtx);
}

void FFVideoFilterDll::CreateEQFilter(AVCodecContext* pDecCtx)
{
	if(m_pVFObj)
		_VF_CreateEQFilter(m_pVFObj,pDecCtx);
}

void FFVideoFilterDll::CreateHueFilter(AVCodecContext* pDecCtx)
{
	if (m_pVFObj)
		_VF_CreateHueFilter(m_pVFObj, pDecCtx);
}

void FFVideoFilterDll::CreateLutYUVFilter(AVCodecContext* pDecCtx)
{
	if (m_pVFObj)
		_VF_CreateLutYUVFilter(m_pVFObj, pDecCtx);
}

void FFVideoFilterDll::CreateLutRGBFilter(AVCodecContext* pDecCtx)
{
	if (m_pVFObj)
		_VF_CreateLutRGBFilter(m_pVFObj, pDecCtx);
}

void FFVideoFilterDll::CreateAformatFilter(AVCodecContext* pDecCtx,char* szChannelLayouts,char* szSampleFmts,char* szSampleRates)
{
	if(m_pVFObj)
		_VF_CreateAformatFilter(m_pVFObj,pDecCtx,szChannelLayouts,szSampleFmts,szSampleRates);
}

void FFVideoFilterDll::CreateAtempoFilter(AVCodecContext* pDecCtx)
{
	if(m_pVFObj)
		_VF_CreateAtempoFilter(m_pVFObj,pDecCtx);
}

void FFVideoFilterDll::CloseFilter()
{
	if(m_pVFObj)
		_VF_CloseFilter(m_pVFObj);
}

int FFVideoFilterDll::InputFrame(AVFrame* pAVFrame)
{
	if(m_pVFObj)
		return _VF_InputFrame(m_pVFObj,pAVFrame);
	return 0;
}

int FFVideoFilterDll::InputFrame2(int iFilterID,AVFrame* pAVFrame)
{
	if(m_pVFObj)
		return _VF_InputFrame2(m_pVFObj,iFilterID,pAVFrame);
	return 0;
}

int FFVideoFilterDll::OutputFrameBuffer(unsigned char* pBuffer,int* w,int* h)
{
	if(m_pVFObj)
		return _VF_OutputFrameBuffer(m_pVFObj,pBuffer,w,h);
	return 0;
}

int FFVideoFilterDll::OutputFrameBuffer2(int iFilterID,unsigned char* pBuffer,int* w,int* h)
{
	if(m_pVFObj)
		return _VF_OutputFrameBuffer2(m_pVFObj, iFilterID, pBuffer,w,h);
	return 0;
}

int FFVideoFilterDll::OutputFrame(AVFrame* pAVFrame)
{
	if(m_pVFObj)
		return _VF_OutputFrame(m_pVFObj,pAVFrame);
	return 0;
}

int FFVideoFilterDll::OutputFrame2(int iFilterID,AVFrame* pAVFrame)
{
	if (m_pVFObj)
		return _VF_OutputFrame2(m_pVFObj, iFilterID, pAVFrame);
	return 0;
}

int FFVideoFilterDll::AVFrameToBuffer(int iFilterID, AVFrame* pInAVFrame,unsigned char* pBuffer, int* w, int* h)
{
	if (m_pVFObj)
		return _VF_AVFrameToBuffer(m_pVFObj, iFilterID, pInAVFrame,pBuffer, w, h);
	return 0;
}

void FFVideoFilterDll::UnRefBuffer(int iFilterID)
{
	if (m_pVFObj)
		_VF_UnRefBuffer(m_pVFObj, iFilterID);
}

void FFVideoFilterDll::SetBrightness(int iValue)
{
	if(m_pVFObj)
		_VF_SetBrightness(m_pVFObj,iValue);
}

void FFVideoFilterDll::SetContrast(int iValue)
{
	if(m_pVFObj)
		_VF_SetContrast(m_pVFObj,iValue);
}

void FFVideoFilterDll::SetSaturation(int iValue)
{
	if(m_pVFObj)
		_VF_SetSaturation(m_pVFObj,iValue);
}

void FFVideoFilterDll::SetGammaR(int iValue)
{
	if (m_pVFObj)
		_VF_SetGammaR(m_pVFObj, iValue);
}

void FFVideoFilterDll::SetGammaG(int iValue)
{
	if (m_pVFObj)
		_VF_SetGammaG(m_pVFObj, iValue);
}

void FFVideoFilterDll::SetGammaB(int iValue)
{
	if (m_pVFObj)
		_VF_SetGammaB(m_pVFObj, iValue);
}

void FFVideoFilterDll::SetHue(int iValue)
{
	if (m_pVFObj)
		_VF_SetHue(m_pVFObj, iValue);
}

void FFVideoFilterDll::SetGray(int iValue)
{
	if (m_pVFObj)
		_VF_SetGray(m_pVFObj, iValue);
}

void FFVideoFilterDll::SetNegative(int iValue)
{
	if (m_pVFObj)
		_VF_SetNegative(m_pVFObj, iValue);
}

void FFVideoFilterDll::BeginFilter()
{
	if (m_pVFObj)
		_VF_BeginFilter(m_pVFObj);
}

void FFVideoFilterDll::EndFilter()
{
	if (m_pVFObj)
		_VF_EndFilter(m_pVFObj);
}

void FFVideoFilterDll::RestartFadeIn(AVCodecContext* pDecCtx)
{
	if (m_pVFObj)
		_VF_RestartFadeIn(m_pVFObj, pDecCtx);
}

void FFVideoFilterDll::RestartFadeOut(AVCodecContext* pDecCtx)
{
	if (m_pVFObj)
		_VF_RestartFadeOut(m_pVFObj, pDecCtx);
}

void FFVideoFilterDll::SetFadeDuration(float fDuration)
{
	if (m_pVFObj)
		_VF_SetFadeDuration(m_pVFObj, fDuration);
}

void FFVideoFilterDll::SetATempo(float fValue)
{
	if (m_pVFObj)
		_VF_SetATempo(m_pVFObj, fValue);
}

int FFVideoFilterDll::OutputFrameBuffer_Audio(int iFilterID, unsigned char* pBuffer)
{
	if (m_pVFObj)
		return _VF_OutputFrameBuffer_Audio(m_pVFObj, iFilterID, pBuffer);
	return 0;
}
