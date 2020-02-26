#ifndef _MFilePlayerDll_H_
#define _MFilePlayerDll_H_

#include "../../../Include/FilePlayerCommon.h"

#define _MAX_MF_OBJS 6    //timeline(0) + shape(1) + 4port(2,3,4,5)

typedef void* (*_In_MFP_Create_Proc)();
typedef void (*_In_MFP_Destroy_Proc)(void* pMFPObj);
typedef void (*_In_MFP_Init_Proc)(void* pMFPObj,HWND hWnd,FilePlayerCallback* pFilePlayerCallback);
typedef int (*_In_MFP_Open_Proc)(void* pMFPObj,char* szFileName);
typedef void (*_In_MFP_Close_Proc)(void* pMFPObj);
typedef void (*_In_MFP_Play_Proc)(void* pMFPObj);
typedef void (*_In_MFP_Stop_Proc)(void* pMFPObj);
typedef void(*_In_MFP_Stop2_Proc)(void* pMFPObj);
typedef void (*_In_MFP_SetRepeat_Proc)(void* pMFPObj,bool bEnable);
typedef void (*_In_MFP_EnableFadeIn_Proc)(void* pMFPObj,bool bEnable);
typedef void (*_In_MFP_EnableFadeOut_Proc)(void* pMFPObj,bool bEnable);
typedef void (*_In_MFP_SetOutputVideoFormat_Proc)(void* pMFPObj, int iFormat);
typedef void (*_In_MFP_Seek_Proc)(void* pMFPObj, double dPos);
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
typedef void (*_In_MFP_SetStartTime_Proc)(void* pMFPObj, double dTime);
typedef void (*_In_MFP_EnableReSampleRate_Proc)(void* pMFPObj, bool bEnable);
typedef void* (*_In_MFP_GetD3DDevice_Proc)(void* pMFPObj);
typedef bool (*_In_MFP_IsAudioFile_Proc)(void* pMFPObj);
typedef void (*_In_MFP_EnableSuspendDisplay_Proc)(void* pMFPObj, bool bEnable);
typedef void (*_In_MFP_SetAudioDelay_Proc)(void* pMFPObj, int iValue);
typedef void (*_In_MFP_SetReSampleRateOffset_Proc)(void* pMFPObj, float fOffset);
typedef void (*_In_MFP_SetProcessFPS_Proc)(void* pMFPObj, float fFPS);

class MFilePlayerDll
{
	   HINSTANCE m_hInst;
	   void* m_pMFPObjs[_MAX_MF_OBJS];

   protected:
   public:
		MFilePlayerDll();
        ~MFilePlayerDll();
		
		bool LoadLib(CString strLibPath);
		void FreeLib();
		void Create(int iIndex);
		void Init(int iIndex, HWND hWnd,FilePlayerCallback* pFilePlayerCallback);
		int Open(int iIndex, char* szFileName);
		void Close(int iIndex);
		void Play(int iIndex);
		void Stop(int iIndex);
		void Stop2(int iIndex);
		void SetRepeat(int iIndex, bool bEnable);
		void EnableFadeIn(int iIndex, bool bEnable);
		void EnableFadeOut(int iIndex, bool bEnable);
		void SetOutputVideoFormat(int iIndex, int iFormat);
		void Seek(int iIndex, double dPos);
		void EnableAudio(int iIndex, bool bEnable);
		float GetVideoFPS(int iIndex);
		int GetTotalOfFrames(int iIndex);
		int GetVideoWidth(int iIndex);
		int GetVideoHeight(int iIndex);
		void SpeedUp(int iIndex);
		void SpeedDown(int iIndex);
		void SetVolume(int iIndex, int iValue);
		void SetSystemVolume(int iIndex,int iValue);
		int GetVolume(int iIndex);
		int GetSystemVolume(int iIndex);
		void SetBrightness(int iIndex, int iValue);
		void SetContrast(int iIndex, int iValue);
		void SetSaturation(int iIndex, int iValue);
		void SetHue(int iIndex, int iValue);
		void SetR(int iIndex, int iValue);
		void SetG(int iIndex, int iValue);
		void SetB(int iIndex, int iValue);
		void SetGray(int iIndex, int iValue);
		void SetNegative(int iIndex, int iValue);
		void PlayBackward(int iIndex);
		int GetPlaybackMode(int iIndex);
		void Pause(int iIndex); 
		void SetFadeDuration(int iIndex, float fValue);

		void SetupDXVA2(int iIndex, void* pD3DDevice);
		void EnableGPU(int iIndex, bool bEnable);
		bool GPUIsEnabled(int iIndex);

		bool IsOpened(int iIndex);
		void SetScale(int iIndex,float fW, float fH);
		void EnableReSampleRate(int iIndex, bool bEnable);
		void ResetFilter(int iIndex);
		int GetAudioInfo(int iIndex, int& iChannelLayout, int& iSampleRate, int& iNBSamples);
		void SetReferenceTimeCode(int iIndex, double dNewRefTime);

		//void EnableGlobalMode(int iIndex, bool bEnable);
		void UpdateGlobalTimeCode(int iIndex, double dTimeCode);
		void SetDisplayIntervalMode(int iIndex, int iMode);
		void SetStartTime(int iIndex, double dTime);
		void* GetD3DDevice(int iIndex);
		bool IsAudioFile(int iIndex);
		void EnableSuspendDisplay(int iIndex, bool bEnable);
		void SetAudioDelay(int iIndex, int iValue);
		void SetReSampleRateOffset(int iIndex, float fValue);
		void SetProcessFPS(int iIndex, float fFPS);

		_In_MFP_Create_Proc _In_MFP_Create;
		_In_MFP_Destroy_Proc _In_MFP_Destroy;
		_In_MFP_Init_Proc _In_MFP_Init;
		_In_MFP_Open_Proc _In_MFP_Open;
		_In_MFP_Close_Proc _In_MFP_Close;
		_In_MFP_Play_Proc _In_MFP_Play;
		_In_MFP_Stop_Proc _In_MFP_Stop;
		_In_MFP_Stop2_Proc _In_MFP_Stop2;
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
		_In_MFP_EnableReSampleRate_Proc _In_MFP_EnableReSampleRate;
		_In_MFP_GetD3DDevice_Proc _In_MFP_GetD3DDevice;
		_In_MFP_IsAudioFile_Proc _In_MFP_IsAudioFile;
		_In_MFP_EnableSuspendDisplay_Proc _In_MFP_EnableSuspendDisplay;
		_In_MFP_SetAudioDelay_Proc _In_MFP_SetAudioDelay;
		_In_MFP_SetReSampleRateOffset_Proc _In_MFP_SetReSampleRateOffset;
		_In_MFP_SetProcessFPS_Proc _In_MFP_SetProcessFPS;
};

#endif
