#ifndef _FFAVCodecDLL_H
#define _FFAVCodecDLL_H

#include "../../../Include/MediaFrameInfo.h"

typedef void (*Init_Proc)();
typedef unsigned long (*CreateCodec_Proc)();
typedef int (*OpenVideoEncoder_Proc)(unsigned long ulID,int iCodecID,int iWidth,int iHeight);
typedef void (*CloseVideoEncoder_Proc)(unsigned long ulID);
typedef int (*EncodeFrame_Proc)(unsigned long ulID,unsigned char* pInBuf,unsigned char* pOutBuf,int iLen,int* iKey);
typedef int (*OpenVideoDecoder_Proc)(unsigned long ulID,int iCodecID,int iWidth,int iHeight);
typedef void (*CloseVideoDecoder_Proc)(unsigned long ulID);
typedef int (*DecodeFrame_Proc)(unsigned long ulID,unsigned char* pInBuf,unsigned char* pOutBuf,int iLen);
typedef int (*DecodeFrameHW_Proc)(unsigned long ulID,unsigned char* pInBuf,int iLen);
typedef void (*ResetFrameType_Proc)(unsigned long ulID);
typedef void (*SetGOP_Proc)(unsigned long ulID,int iValue);
typedef void (*SetQuality_Proc)(unsigned long ulID,int iValue);
typedef int (*GetFrameIndex_Proc)(unsigned long ulID);
typedef void (*GetRealSize_Proc)(unsigned long ulID,int* iWidth,int* iHeight,int* iKey);

//audio
typedef int (*OpenAudioDecoder_Proc)(unsigned long ulID,int iCodecID,int iChannels,int iSample,int iBitRate);
typedef void (*CloseAudioDecoder_Proc)(unsigned long ulID);
typedef int (*DecodeAudioFrame_Proc)(unsigned long ulID,unsigned char* pInBuf,unsigned char* pOutBuf,int iLen);
typedef void (*GetWaveFormat_Proc)(unsigned long ulID,int* iChannel,int* iSampleRate,int* iBits);

typedef int (*OpenAudioEncoder_Proc)(unsigned long ulID,int iCodecID,int iSample,int iChannel,int iBitRate);
typedef void (*CloseAudioEncoder_Proc)(unsigned long ulID);
typedef int (*EncodeAudioFrame_Proc)(unsigned long ulID,unsigned char* pInBuf,unsigned char* pOutBuf,int iLen);

typedef void (*SetEnPixelFormat_Proc)(unsigned long ulID,int iValue);
typedef void (*SetDePixelFormat_Proc)(unsigned long ulID,int iValue);
typedef void (*Destroy_Proc)();
/*
//for avi
typedef bool (*OpenAVI_Proc)(char* szFileName);
typedef bool (*WriteAVIHeader_Proc)();
typedef void (*CloseAVI_Proc)();
typedef void (*WriteVideoFrame_Proc)(unsigned char* pBuffer,int iLen,bool bKeyFrame);
typedef bool (*WriteAudioFrame_Proc)(unsigned char* pBuffer,int iLen);
typedef void (*SetVideoInfo_Proc)(int iWidth,int iHeight,int iCodecType,float fFps);
typedef void (*SetAudioInfo_Proc)(int iChannels,int iSamples,int iBits,int iBitRate,int iCodecType);
*/
//for converter
typedef void (*OpenConverter_Proc)(int iIndex,int iSrcPixFmt,int iDestPixFmt,int iWidth,int iHeight);
typedef void (*CloseConverter_Proc)(int iIndex);
typedef int (*Convert_Proc)(int iIndex,unsigned char* pSrcBuffer,unsigned char* pDestBuffer,int iWidth,int iHeight);

typedef int (*ScaleYUY2_Proc)(int iIndex,unsigned char* pSrcBuffer,unsigned char* pDestBuffer,int iSrcW,int iSrcH,int iDestW,int iDestH);
typedef int (*Scale_Proc)(int iIndex,int iPixFmt,unsigned char* pSrcBuffer,unsigned char* pDestBuffer,int iSrcW,int iSrcH,int iDestW,int iDestH);

//GPU decode
typedef void (*EnableGPU_Proc)(unsigned long ulID,bool bEnable);
typedef void (*SetupDXVA2_Proc)(unsigned long ulID,void* pD3DDevice);
typedef AVFrame* (*GetDePicture420_Proc)(unsigned long ulID);
typedef bool (*GPUIsEnabled_Proc)(unsigned long ulID);

//MediaFileReader
typedef int (*MFR_Open_Proc)(char* szFileName);
typedef void (*MFR_Close_Proc)();
typedef int (*MFR_GetNextFrame_Proc)(MediaFrameBuffer* pFrameBuf);
typedef void (*MFR_GetVideoFrameSize_Proc)(int* w,int* h);

typedef bool (*VideoEncoderIsOpened_Proc)(unsigned long ulID);
typedef bool (*VideoDecoderIsOpened_Proc)(unsigned long ulID);
typedef bool (*AudioDecoderIsOpened_Proc)(unsigned long ulID);
typedef bool (*AudioEncoderIsOpened_Proc)(unsigned long ulID);

typedef void (*SetVideoExtraData_Proc)(unsigned long ulID,unsigned char* pData,int iLen);
//void SetVideoExtraData(unsigned long ulID,unsigned char* pData,int iLen)
typedef void(*EnableMT_Proc)(unsigned long ulID, bool bEnable);

class FFAVCodecDLL
{
		HINSTANCE m_hInst;
   public:
	    FFAVCodecDLL();
        ~FFAVCodecDLL();

		Init_Proc FF_Init;
		CreateCodec_Proc CreateCodec;
		OpenVideoEncoder_Proc OpenVideoEncoder;
		CloseVideoEncoder_Proc CloseVideoEncoder;
		EncodeFrame_Proc EncodeFrame;
		OpenVideoDecoder_Proc OpenVideoDecoder;
		CloseVideoDecoder_Proc CloseVideoDecoder;
		DecodeFrame_Proc DecodeFrame;
		DecodeFrameHW_Proc DecodeFrameHW;
		ResetFrameType_Proc ResetFrameType;
		SetGOP_Proc SetGOP;
		SetQuality_Proc SetQuality;
		GetFrameIndex_Proc GetFrameIndex;
		GetRealSize_Proc GetRealSize;

		//audio
		OpenAudioDecoder_Proc OpenAudioDecoder;
		CloseAudioDecoder_Proc CloseAudioDecoder;
		DecodeAudioFrame_Proc DecodeAudioFrame;
		GetWaveFormat_Proc GetWaveFormat;

		OpenAudioEncoder_Proc OpenAudioEncoder;
		CloseAudioEncoder_Proc CloseAudioEncoder;
		EncodeAudioFrame_Proc EncodeAudioFrame;

		SetEnPixelFormat_Proc SetEnPixelFormat;
		SetDePixelFormat_Proc SetDePixelFormat;
		Destroy_Proc Destroy;

		/*
		//for avi
		OpenAVI_Proc FF_OpenAVI;
		WriteAVIHeader_Proc FF_WriteAVIHeader;
		CloseAVI_Proc FF_CloseAVI;
		WriteVideoFrame_Proc FF_WriteVideoFrame;
		WriteAudioFrame_Proc FF_WriteAudioFrame;
		SetVideoInfo_Proc FF_SetVideoInfo;
		SetAudioInfo_Proc FF_SetAudioInfo;
		*/

		//for converter
        OpenConverter_Proc OpenConverter;
		CloseConverter_Proc CloseConverter;
		Convert_Proc Convert;

		ScaleYUY2_Proc ScaleYUY2;

		//GPU decode
		EnableGPU_Proc EnableGPU;
		SetupDXVA2_Proc SetupDXVA2;
		GetDePicture420_Proc GetDePicture420;
		GPUIsEnabled_Proc GPUIsEnabled;

		//MediaFileReader
		MFR_Open_Proc MFR_Open;
		MFR_Close_Proc MFR_Close;
		MFR_GetNextFrame_Proc MFR_GetNextFrame;
		MFR_GetVideoFrameSize_Proc MFR_GetVideoFrameSize;

		VideoEncoderIsOpened_Proc VideoEncoderIsOpened;
		VideoDecoderIsOpened_Proc VideoDecoderIsOpened;
		AudioDecoderIsOpened_Proc AudioDecoderIsOpened;
		AudioEncoderIsOpened_Proc AudioEncoderIsOpened;

		SetVideoExtraData_Proc SetVideoExtraData;
		EnableMT_Proc EnableMT;

		Scale_Proc Scale;

		void Init();
		void LoadLib();
		void FreeLib();
};

#endif

/*
//MediaFileReader
extern "C" __declspec( dllexport )int MFR_Open(char* szFileName)
extern "C" __declspec( dllexport )void MFR_Close()
extern "C" __declspec( dllexport )int MFR_GetNextFrame(MediaFrameBuffer* pFrameBuf)
*/