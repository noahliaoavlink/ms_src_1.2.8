#ifndef _MFilePlayerDll_H_
#define _MFilePlayerDll_H_

#include "../../Include/FilePlayerCommon.h"

typedef void* (*_In_MFP_Create_Proc)();
typedef void(*_In_MFP_Destroy_Proc)(void* pMFPObj);
typedef void(*_In_MFP_Init_Proc)(void* pMFPObj, HWND hWnd, FilePlayerCallback* pFilePlayerCallback);
typedef int(*_In_MFP_Open_Proc)(void* pMFPObj, char* szFileName);
typedef void(*_In_MFP_Close_Proc)(void* pMFPObj);
typedef void(*_In_MFP_Play_Proc)(void* pMFPObj);
typedef void(*_In_MFP_Stop_Proc)(void* pMFPObj);
typedef void(*_In_MFP_SetRepeat_Proc)(void* pMFPObj, bool bEnable);
typedef void(*_In_MFP_EnableFadeIn_Proc)(void* pMFPObj, bool bEnable);
typedef void(*_In_MFP_EnableFadeOut_Proc)(void* pMFPObj, bool bEnable);
typedef void(*_In_MFP_SetOutputVideoFormat_Proc)(void* pMFPObj, int iFormat);
typedef void(*_In_MFP_Seek_Proc)(void* pMFPObj, double dPos);
typedef void(*_In_MFP_EnableAudio_Proc)(void* pMFPObj, bool bEnable);
typedef float(*_In_MFP_GetVideoFPS_Proc)(void* pMFPObj);
typedef int(*_In_MFP_GetTotalOfFrames_Proc)(void* pMFPObj);
typedef int(*_In_MFP_GetVideoWidth_Proc)(void* pMFPObj);
typedef int(*_In_MFP_GetVideoHeight_Proc)(void* pMFPObj);
typedef void(*_In_MFP_SpeedUp_Proc)(void* pMFPObj);
typedef void(*_In_MFP_SpeedDown_Proc)(void* pMFPObj);
typedef void(*_In_MFP_SetVolume_Proc)(void* pMFPObj, int iValue);
typedef int(*_In_MFP_GetVolume_Proc)(void* pMFPObj);
typedef void(*_In_MFP_SetBrightness_Proc)(void* pMFPObj, int iValue);
typedef void(*_In_MFP_SetContrast_Proc)(void* pMFPObj, int iValue);
typedef void(*_In_MFP_SetSaturation_Proc)(void* pMFPObj, int iValue);
typedef void(*_In_MFP_PlayBackward_Proc)(void* pMFPObj);
typedef int(*_In_MFP_GetPlaybackMode_Proc)(void* pMFPObj);
typedef void(*_In_MFP_Pause_Proc)(void* pMFPObj);
typedef void(*_In_MFP_SetHue_Proc)(void* pMFPObj, int iValue);
typedef void(*_In_MFP_SetR_Proc)(void* pMFPObj, int iValue);
typedef void(*_In_MFP_SetG_Proc)(void* pMFPObj, int iValue);
typedef void(*_In_MFP_SetB_Proc)(void* pMFPObj, int iValue);
typedef void(*_In_MFP_SetSystemVolume_Proc)(void* pMFPObj, int iValue);
typedef int(*_In_MFP_GetSystemVolume_Proc)(void* pMFPObj);
typedef void(*_In_MFP_SetFadeDuration_Proc)(void* pMFPObj, float fDuration);
typedef void(*_In_MFP_SetupDXVA2_Proc)(void* pMFPObj, void* pD3DDevice);
typedef void(*_In_MFP_EnableGPU_Proc)(void* pMFPObj, bool bEnable);
typedef bool(*_In_MFP_GPUIsEnabled_Proc)(void* pMFPObj);
typedef bool(*_In_MFP_IsOpened_Proc)(void* pMFPObj);
typedef void(*_In_MFP_SetScale_Proc)(void* pMFPObj, float fW, float fH);
typedef void(*_In_MFP_SetGray_Proc)(void* pMFPObj, int iValue);
typedef void(*_In_MFP_SetNegative_Proc)(void* pMFPObj, int iValue);
typedef void(*_In_MFP_ResetFilter_Proc)(void* pMFPObj);
typedef int(*_In_MFP_GetAudioInfo_Proc)(void* pMFPObj, int& iChannelLayout, int& iSampleRate, int& iNBSamples);
typedef void(*_In_MFP_SetRefTimeCode_Proc)(void* pMFPObj, double dNewRefTime);
//typedef void(*_In_MFP_EnableGlobalMode_Proc)(void* pMFPObj, bool bEnable);
typedef void(*_In_MFP_UpdateGlobalTimeCode_Proc)(void* pMFPObj, double dTimeCode);
typedef void(*_In_MFP_SetDisplayIntervalMode_Proc)(void* pMFPObj, int iMode);
typedef void(*_In_MFP_SetStartTime_Proc)(void* pMFPObj, double dTime);


class MFilePlayerDll
{
		HINSTANCE m_hInst;
		void* m_pMFPObj;
   protected:
   public:
		MFilePlayerDll();
        ~MFilePlayerDll();
		
		bool LoadLib();
		void FreeLib();
		
		void Init(HWND hWnd,FilePlayerCallback* pFilePlayerCallback);
		void Open(char* szFileName);
		void Close();
		void Play();
		void Stop();
		void SetRepeat(bool bEnable);
		void EnableFadeIn(bool bEnable);
		void EnableFadeOut(bool bEnable);
		void SetOutputVideoFormat(int iFormat);
		void Seek(double dPos);
		void EnableAudio(bool bEnable);
		float GetVideoFPS();
		int GetTotalOfFrames();
		//int GetVideoFrame(unsigned char* pBuffer, int* w, int* h, double* pos, char* szTime);
		int GetVideoWidth();
		int GetVideoHeight();
		void SpeedUp();
		void SpeedDown();
		void SetVolume(int iValue);
		int GetVolume();
		void SetBrightness(int iValue);
		void SetContrast(int iValue);
		void SetSaturation(int iValue);
		void SetHue(int iValue);
		void SetR(int iValue);
		void SetG(int iValue);
		void SetB(int iValue);
		void PlayBackward();
		int GetPlaybackMode();
		void Pause();

		void SetSystemVolume(int iValue);
		int GetSystemVolume();

		void UpdateGlobalTimeCode(double dTimeCode);
		void SetDisplayIntervalMode(int iMode);

		void SetupDXVA2(void* pD3DDevice);
		void EnableGPU(bool pD3DDevice);
		bool GPUIsEnabled();

		_In_MFP_Create_Proc _In_MFP_Create;
		_In_MFP_Destroy_Proc _In_MFP_Destroy;
		_In_MFP_Init_Proc _In_MFP_Init;
		_In_MFP_Open_Proc _In_MFP_Open;
		_In_MFP_Close_Proc _In_MFP_Close;
		_In_MFP_Play_Proc _In_MFP_Play;
		_In_MFP_Stop_Proc _In_MFP_Stop;
		_In_MFP_SetRepeat_Proc _In_MFP_SetRepeat;
		_In_MFP_EnableFadeIn_Proc _In_MFP_EnableFadeIn;
		_In_MFP_EnableFadeOut_Proc _In_MFP_EnableFadeOut;
		_In_MFP_SetOutputVideoFormat_Proc _In_MFP_SetOutputVideoFormat;
		_In_MFP_Seek_Proc _In_MFP_Seek;
		_In_MFP_EnableAudio_Proc _In_MFP_EnableAudio;
		_In_MFP_GetVideoFPS_Proc _In_MFP_GetVideoFPS;
		_In_MFP_GetTotalOfFrames_Proc _In_MFP_GetTotalOfFrames;
		_In_MFP_GetVideoWidth_Proc _In_MFP_GetVideoWidth;
		_In_MFP_GetVideoHeight_Proc _In_MFP_GetVideoHeight;
		_In_MFP_SpeedUp_Proc _In_MFP_SpeedUp;
		_In_MFP_SpeedDown_Proc _In_MFP_SpeedDown;
		_In_MFP_SetVolume_Proc _In_MFP_SetVolume;
		_In_MFP_SetSystemVolume_Proc _In_MFP_SetSystemVolume;
		_In_MFP_GetVolume_Proc _In_MFP_GetVolume;
		_In_MFP_GetSystemVolume_Proc _In_MFP_GetSystemVolume;
		_In_MFP_SetBrightness_Proc _In_MFP_SetBrightness;
		_In_MFP_SetContrast_Proc _In_MFP_SetContrast;
		_In_MFP_SetSaturation_Proc _In_MFP_SetSaturation;
		_In_MFP_PlayBackward_Proc _In_MFP_PlayBackward;
		_In_MFP_GetPlaybackMode_Proc _In_MFP_GetPlaybackMode;
		_In_MFP_Pause_Proc _In_MFP_Pause;
		_In_MFP_SetHue_Proc _In_MFP_SetHue;
		_In_MFP_SetR_Proc _In_MFP_SetR;
		_In_MFP_SetG_Proc _In_MFP_SetG;
		_In_MFP_SetB_Proc _In_MFP_SetB;
		_In_MFP_SetFadeDuration_Proc _In_MFP_SetFadeDuration;
		_In_MFP_SetupDXVA2_Proc _In_MFP_SetupDXVA2;
		_In_MFP_EnableGPU_Proc _In_MFP_EnableGPU;
		_In_MFP_GPUIsEnabled_Proc _In_MFP_GPUIsEnabled;
		_In_MFP_IsOpened_Proc _In_MFP_IsOpened;
		_In_MFP_SetScale_Proc _In_MFP_SetScale;
		_In_MFP_SetGray_Proc _In_MFP_SetGray;
		_In_MFP_SetNegative_Proc _In_MFP_SetNegative;
		_In_MFP_ResetFilter_Proc _In_MFP_ResetFilter;
		_In_MFP_GetAudioInfo_Proc _In_MFP_GetAudioInfo;
		_In_MFP_SetRefTimeCode_Proc _In_MFP_SetRefTimeCode;
		//_In_MFP_EnableGlobalMode_Proc _In_MFP_EnableGlobalMode;
		_In_MFP_UpdateGlobalTimeCode_Proc _In_MFP_UpdateGlobalTimeCode;
		_In_MFP_SetDisplayIntervalMode_Proc _In_MFP_SetDisplayIntervalMode;
		_In_MFP_SetStartTime_Proc _In_MFP_SetStartTime;

};

#endif