#include "stdafx.h"
#include "FFAVCodecDLL.h"

FFAVCodecDLL::FFAVCodecDLL()
{
	m_hInst = 0;
}

FFAVCodecDLL::~FFAVCodecDLL()
{
}

void FFAVCodecDLL::Init()
{
	LoadLib();
	(*FF_Init)();
}

void FFAVCodecDLL::LoadLib()
{
	char szFileName[] = "FFAVCodec.dll";
	m_hInst = LoadLibraryA(szFileName);

	if(m_hInst)
	{
		FF_Init = (Init_Proc)GetProcAddress(m_hInst, "Init");
		CreateCodec = (CreateCodec_Proc)GetProcAddress(m_hInst, "CreateCodec");
		OpenVideoEncoder = (OpenVideoEncoder_Proc)GetProcAddress(m_hInst, "OpenVideoEncoder");
		CloseVideoEncoder = (CloseVideoEncoder_Proc)GetProcAddress(m_hInst, "CloseVideoEncoder");
		EncodeFrame = (EncodeFrame_Proc)GetProcAddress(m_hInst, "EncodeFrame");
		OpenVideoDecoder = (OpenVideoDecoder_Proc)GetProcAddress(m_hInst, "OpenVideoDecoder");
		CloseVideoDecoder = (CloseVideoDecoder_Proc)GetProcAddress(m_hInst, "CloseVideoDecoder");
		DecodeFrame = (DecodeFrame_Proc)GetProcAddress(m_hInst, "DecodeFrame");
		DecodeFrameHW = (DecodeFrameHW_Proc)GetProcAddress(m_hInst, "DecodeFrameHW");
		ResetFrameType = (ResetFrameType_Proc)GetProcAddress(m_hInst, "ResetFrameType");
		SetGOP = (SetGOP_Proc)GetProcAddress(m_hInst, "SetGOP");
		SetQuality = (SetQuality_Proc)GetProcAddress(m_hInst, "SetQuality");
		GetFrameIndex = (GetFrameIndex_Proc)GetProcAddress(m_hInst, "GetFrameIndex");
		GetRealSize = (GetRealSize_Proc)GetProcAddress(m_hInst, "GetRealSize");
		//audio
		OpenAudioDecoder = (OpenAudioDecoder_Proc)GetProcAddress(m_hInst, "OpenAudioDecoder");
		CloseAudioDecoder = (CloseAudioDecoder_Proc)GetProcAddress(m_hInst, "CloseAudioDecoder");
		DecodeAudioFrame = (DecodeAudioFrame_Proc)GetProcAddress(m_hInst, "DecodeAudioFrame");
		GetWaveFormat = (GetWaveFormat_Proc)GetProcAddress(m_hInst, "GetWaveFormat");

		SetEnPixelFormat = (SetEnPixelFormat_Proc)GetProcAddress(m_hInst, "SetEnPixelFormat");
		SetDePixelFormat = (SetDePixelFormat_Proc)GetProcAddress(m_hInst, "SetDePixelFormat");
//		FF_Destroy = (Destroy_Proc)GetProcAddress(m_hInst, "Destroy");

		/*
		//for avi
		FF_OpenAVI = (OpenAVI_Proc)GetProcAddress(m_hInst, "OpenAVI");
		FF_WriteAVIHeader = (WriteAVIHeader_Proc)GetProcAddress(m_hInst, "WriteAVIHeader");
		FF_CloseAVI = (CloseAVI_Proc)GetProcAddress(m_hInst, "CloseAVI");
		FF_WriteVideoFrame = (WriteVideoFrame_Proc)GetProcAddress(m_hInst, "WriteVideoFrame");
		FF_WriteAudioFrame = (WriteAudioFrame_Proc)GetProcAddress(m_hInst, "WriteAudioFrame");
		FF_SetVideoInfo = (SetVideoInfo_Proc)GetProcAddress(m_hInst, "SetVideoInfo");
		FF_SetAudioInfo = (SetAudioInfo_Proc)GetProcAddress(m_hInst, "SetAudioInfo");
		*/

		OpenAudioEncoder = (OpenAudioEncoder_Proc)GetProcAddress(m_hInst, "OpenAudioEncoder");
		CloseAudioEncoder = (CloseAudioEncoder_Proc)GetProcAddress(m_hInst, "CloseAudioEncoder");
		EncodeAudioFrame = (EncodeAudioFrame_Proc)GetProcAddress(m_hInst, "EncodeAudioFrame");

		//for converter
		OpenConverter = (OpenConverter_Proc)GetProcAddress(m_hInst, "OpenConverter");
		CloseConverter = (CloseConverter_Proc)GetProcAddress(m_hInst, "CloseConverter");
		Convert = (Convert_Proc)GetProcAddress(m_hInst, "Convert");

//		FF_Deinterlace = (Deinterlace_Proc)GetProcAddress(m_hInst, "Deinterlace");

		//GPU decode
		EnableGPU = (EnableGPU_Proc)GetProcAddress(m_hInst, "EnableGPU");
		SetupDXVA2 = (SetupDXVA2_Proc)GetProcAddress(m_hInst, "SetupDXVA2");
		GetDePicture420 = (GetDePicture420_Proc)GetProcAddress(m_hInst, "GetDePicture420");
		GPUIsEnabled = (GPUIsEnabled_Proc)GetProcAddress(m_hInst, "GPUIsEnabled");

		//MediaFileReader
		MFR_Open = (MFR_Open_Proc)GetProcAddress(m_hInst, "MFR_Open");
		MFR_Close = (MFR_Close_Proc)GetProcAddress(m_hInst, "MFR_Close");
		MFR_GetNextFrame = (MFR_GetNextFrame_Proc)GetProcAddress(m_hInst, "MFR_GetNextFrame");
		MFR_GetVideoFrameSize = (MFR_GetVideoFrameSize_Proc)GetProcAddress(m_hInst, "MFR_GetVideoFrameSize");

		VideoEncoderIsOpened = (VideoEncoderIsOpened_Proc)GetProcAddress(m_hInst, "VideoEncoderIsOpened");
		VideoDecoderIsOpened = (VideoDecoderIsOpened_Proc)GetProcAddress(m_hInst, "VideoDecoderIsOpened");
		AudioDecoderIsOpened = (AudioDecoderIsOpened_Proc)GetProcAddress(m_hInst, "AudioDecoderIsOpened");
		AudioEncoderIsOpened = (AudioEncoderIsOpened_Proc)GetProcAddress(m_hInst, "AudioEncoderIsOpened");

		SetVideoExtraData = (SetVideoExtraData_Proc)GetProcAddress(m_hInst, "SetVideoExtraData");
		EnableMT = (EnableMT_Proc)GetProcAddress(m_hInst, "EnableMT");

		Scale = (Scale_Proc)GetProcAddress(m_hInst, "Scale");
	}
}

void FFAVCodecDLL::FreeLib()
{
	if(m_hInst)
	{
//		FF_Destroy();
		FreeLibrary(m_hInst);
		m_hInst = 0;
	}
}