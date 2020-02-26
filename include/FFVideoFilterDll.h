#ifndef _FFVideoFilterDll_H_
#define _FFVideoFilterDll_H_

//#include "../../../Include/FilePlayerCommon.h"

#ifdef __cplusplus
extern "C"
{
	#include "libavcodec/avcodec.h"
}
#endif //__cplusplus

typedef void* (*_VF_Create_Proc)();
typedef void (*_VF_Destroy_Proc)(void* pVFObj);
typedef void (*_VF_CreateFadeInFilter_Proc)(void* pVFObj,AVCodecContext* pDecCtx);
typedef void (*_VF_CreateFadeOutFilter_Proc)(void* pVFObj,AVCodecContext* pDecCtx);
typedef void (*_VF_CreateAformatFilter_Proc)(void* pVFObj,AVCodecContext* pDecCtx,char* szChannelLayouts,char* szSampleFmts,char* szSampleRates);
typedef void (*_VF_CreateAtempoFilter_Proc)(void* pVFObj, AVCodecContext* pDecCtx);
typedef void (*_VF_CloseFilter_Proc)(void* pVFObj);
typedef int (*_VF_InputFrame_Proc)(void* pVFObj,AVFrame* pAVFrame);
typedef int (*_VF_OutputFrameBuffer_Proc)(void* pVFObj,unsigned char* pBuffer,int* w,int* h);
typedef int (*_VF_OutputFrame_Proc)(void* pVFObj,AVFrame* pAVFrame);
typedef int (*_VF_OutputFrame2_Proc)(void* pVFObj, int iFilterID,AVFrame* pAVFrame);

typedef void (*_VF_CreateEQFilter_Proc)(void* pVFObj,AVCodecContext* pDecCtx);
typedef int (*_VF_InputFrame2_Proc)(void* pVFObj,int iFilterID,AVFrame* pAVFrame);
typedef int (*_VF_OutputFrameBuffer2_Proc)(void* pVFObj,int iFilterID,unsigned char* pBuffer,int* w,int* h);
typedef int (*_VF_AVFrameToBuffer_Proc)(void* pVFObj, int iFilterID, AVFrame* pInAVFrame, unsigned char* pBuffer, int* w, int* h);
typedef void (*_VF_UnRefBuffer_Proc)(void* pVFObj, int iFilterID);

typedef void (*_VF_SetBrightness_Proc)(void* pVFObj, int iValue);
typedef void (*_VF_SetContrast_Proc)(void* pVFObj, int iValue);
typedef void (*_VF_SetSaturation_Proc)(void* pVFObj, int iValue);

typedef void (*_VF_SetGammaR_Proc)(void* pVFObj, int iValue);
typedef void (*_VF_SetGammaG_Proc)(void* pVFObj, int iValue);
typedef void (*_VF_SetGammaB_Proc)(void* pVFObj, int iValue);

typedef void (*_VF_CreateHueFilter_Proc)(void* pVFObj, AVCodecContext* pDecCtx);
typedef void (*_VF_SetHue_Proc)(void* pVFObj, int iValue);

typedef void (*_VF_BeginFilter_Proc)(void* pVFObj);
typedef void (*_VF_EndFilter_Proc)(void* pVFObj);
typedef void (*_VF_RestartFadeIn_Proc)(void* pVFObj,AVCodecContext* pDecCtx);
typedef void (*_VF_RestartFadeOut_Proc)(void* pVFObj, AVCodecContext* pDecCtx);
typedef void (*_VF_SetFadeDuration_Proc)(void* pVFObj, float fDuration);

typedef void (*_VF_SetATempo_Proc)(void* pVFObj, float fValue);
typedef int (*_VF_OutputFrameBuffer_Audio_Proc)(void* pVFObj, int iFilterID, unsigned char* pBuffer);

typedef void (*_VF_CreateLutYUVFilter_Proc)(void* pVFObj, AVCodecContext* pDecCtx);
typedef void (*_VF_SetGray_Proc)(void* pVFObj, int iValue);

typedef void (*_VF_CreateLutRGBFilter_Proc)(void* pVFObj, AVCodecContext* pDecCtx);
typedef void (*_VF_SetNegative_Proc)(void* pVFObj, int iValue);

class FFVideoFilterDll
{
		HINSTANCE m_hInst;
		void* m_pVFObj;
   protected:
   public:
		FFVideoFilterDll();
        ~FFVideoFilterDll();
		
		bool LoadLib();
		void FreeLib();

		_VF_Create_Proc _VF_Create;
		_VF_Destroy_Proc _VF_Destroy;
		_VF_CreateFadeInFilter_Proc _VF_CreateFadeInFilter;
		_VF_CreateFadeOutFilter_Proc _VF_CreateFadeOutFilter;
		_VF_CreateAformatFilter_Proc _VF_CreateAformatFilter;
		_VF_CreateAtempoFilter_Proc _VF_CreateAtempoFilter;
		_VF_CloseFilter_Proc _VF_CloseFilter;
		_VF_InputFrame_Proc _VF_InputFrame;
		_VF_OutputFrameBuffer_Proc _VF_OutputFrameBuffer;
		_VF_OutputFrame_Proc _VF_OutputFrame;
		_VF_OutputFrame2_Proc _VF_OutputFrame2;
		_VF_AVFrameToBuffer_Proc _VF_AVFrameToBuffer;
		_VF_UnRefBuffer_Proc _VF_UnRefBuffer;
		
		_VF_CreateEQFilter_Proc _VF_CreateEQFilter;
		_VF_InputFrame2_Proc _VF_InputFrame2;
		_VF_OutputFrameBuffer2_Proc _VF_OutputFrameBuffer2;
		_VF_SetBrightness_Proc _VF_SetBrightness;
		_VF_SetContrast_Proc _VF_SetContrast;
		_VF_SetSaturation_Proc _VF_SetSaturation;
		
		_VF_SetGammaR_Proc _VF_SetGammaR;
		_VF_SetGammaG_Proc _VF_SetGammaG;
		_VF_SetGammaB_Proc _VF_SetGammaB;
		
		_VF_CreateHueFilter_Proc _VF_CreateHueFilter;
		_VF_SetHue_Proc _VF_SetHue;
		_VF_BeginFilter_Proc _VF_BeginFilter;
		_VF_EndFilter_Proc _VF_EndFilter;
		_VF_RestartFadeIn_Proc _VF_RestartFadeIn;
		_VF_RestartFadeOut_Proc _VF_RestartFadeOut;
		_VF_SetFadeDuration_Proc _VF_SetFadeDuration;
		
		_VF_SetATempo_Proc _VF_SetATempo;
		_VF_OutputFrameBuffer_Audio_Proc _VF_OutputFrameBuffer_Audio;
		
		_VF_CreateLutYUVFilter_Proc _VF_CreateLutYUVFilter;
		_VF_SetGray_Proc _VF_SetGray;
		
		_VF_CreateLutRGBFilter_Proc _VF_CreateLutRGBFilter;
		_VF_SetNegative_Proc _VF_SetNegative;

		void CreateFadeInFilter(AVCodecContext* pDecCtx);
		void CreateFadeOutFilter(AVCodecContext* pDecCtx);
		void CreateEQFilter(AVCodecContext* pDecCtx);
		void CreateHueFilter(AVCodecContext* pDecCtx);
		void CreateLutYUVFilter(AVCodecContext* pDecCtx);
		void CreateLutRGBFilter(AVCodecContext* pDecCtx);
		void CreateAformatFilter(AVCodecContext* pDecCtx,char* szChannelLayouts,char* szSampleFmts,char* szSampleRates);
		void CreateAtempoFilter(AVCodecContext* pDecCtx);
		void CloseFilter();
		int InputFrame(AVFrame* pAVFrame);
		int InputFrame2(int iFilterID,AVFrame* pAVFrame);
		int OutputFrameBuffer(unsigned char* pBuffer,int* w,int* h);
		int OutputFrameBuffer2(int iFilterID,unsigned char* pBuffer,int* w,int* h);
		int OutputFrame(AVFrame* pAVFrame);
		int OutputFrame2(int iFilterID,AVFrame* pAVFrame);
		int AVFrameToBuffer(int iFilterID, AVFrame* pInAVFrame,unsigned char* pBuffer, int* w, int* h);
		void UnRefBuffer(int iFilterID);
		void SetBrightness(int iValue);
		void SetContrast(int iValue);
		void SetSaturation(int iValue);
		void SetHue(int iValue);
		void SetGray(int iValue);
		void SetNegative(int iValue);
		
		void SetGammaR(int iValue);
		void SetGammaG(int iValue);
		void SetGammaB(int iValue);
		void BeginFilter();
		void EndFilter();
		void RestartFadeIn(AVCodecContext* pDecCtx);
		void RestartFadeOut(AVCodecContext* pDecCtx);
		void SetFadeDuration(float fDuration);
		void SetATempo(float fValue);
		int OutputFrameBuffer_Audio(int iFilterID, unsigned char* pBuffer);
};

#endif