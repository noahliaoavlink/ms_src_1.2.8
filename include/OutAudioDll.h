#ifndef _OutAudioDll_H_
#define _OutAudioDll_H_

//#include "../../../Include/OutAudioDll.h"

typedef void* (*_Out_OAD_Create_Proc)();
typedef void (*_Out_OAD_Destroy_Proc)(void* pOADObj);
typedef void (*_Out_OAD_Init_Proc)(void* pOADObj,HWND hWnd);
typedef void (*_Out_OAD_Open_Proc)(void* pOADObj);
typedef void (*_Out_OAD_Close_Proc)(void* pOADObj);
typedef void (*_Out_OAD_Output_Proc)(void* pOADObj,char* pBuffer,int iLen);
typedef void (*_Out_OAD_Stop_Proc)(void* pOADObj);
typedef void (*_Out_OAD_Continue_Proc)(void* pOADObj);
typedef void (*_Out_OAD_SetWaveFormat_Proc)(void* pOADObj,int nChannels,int nFrequency,int nBits);
typedef void (*_Out_OAD_SetBufferInfo_Proc)(void* pOADObj,int iBufferSize,int iTotalOfBuffers);
typedef bool (*_Out_OAD_IsReady_Proc)(void* pOADObj);
typedef void (*_Out_OAD_SetVolume_Proc)(void* pOADObj,int iValue);
typedef bool (*_Out_OAD_IsOpened_Proc)(void* pOADObj);
typedef void (*_Out_OAD_ResetBuffer_Proc)(void* pOADObj);
typedef long (*_Out_OAD_GetVolume_Proc)(void* pOADObj);
typedef bool (*_Out_OAD_IsFulled_Proc)(void* pOADObj);
typedef int (*_Out_OAD_GetBufferCount_Proc)(void* pOADObj);
typedef void (*_Out_OAD_AddTimestamp_Proc)(void* pOADObj,unsigned long ulTimestamp);
typedef unsigned long (*_Out_OAD_GetLastTimestamp_Proc)(void* pOADObj);

class OutAudioDll
{
		HINSTANCE m_hInst;
		void* m_pOADObj;
   protected:
   public:
		OutAudioDll();
        ~OutAudioDll();
		
		bool LoadLib();
		void FreeLib();

		void Init(HWND hWnd);
		void Open();
		void Close();
		void Output(char* pBuffer,int iLen);
		void Stop();
		void Continue();
		void SetWaveFormat(int nChannels,int nFrequency,int nBits);
		void SetBufferInfo(int iBufferSize,int iTotalOfBuffers);
		bool IsReady();
		void SetVolume(int iValue);
		bool IsOpened();
		void OpenDevice(int iCH,int iFrequency,int iBits,int iBufferLen,int iTotalOfBuffers);
		void ResetBuffer();
		long GetVolume();
		bool IsFulled();
		int GetBufferCount();
		void AddTimestamp(unsigned long ulTimestamp);
		unsigned long GetLastTimestamp();

		_Out_OAD_Create_Proc _Out_OAD_Create;
		_Out_OAD_Destroy_Proc _Out_OAD_Destroy;
		_Out_OAD_Init_Proc _Out_OAD_Init;
		_Out_OAD_Open_Proc _Out_OAD_Open;
		_Out_OAD_Close_Proc _Out_OAD_Close;
		_Out_OAD_Output_Proc _Out_OAD_Output;
		_Out_OAD_Stop_Proc _Out_OAD_Stop;
		_Out_OAD_Continue_Proc _Out_OAD_Continue;
		_Out_OAD_SetWaveFormat_Proc _Out_OAD_SetWaveFormat;
		_Out_OAD_SetBufferInfo_Proc _Out_OAD_SetBufferInfo;
		_Out_OAD_IsReady_Proc _Out_OAD_IsReady;
		_Out_OAD_SetVolume_Proc _Out_OAD_SetVolume;
		_Out_OAD_IsOpened_Proc _Out_OAD_IsOpened;
		_Out_OAD_ResetBuffer_Proc _Out_OAD_ResetBuffer;
		_Out_OAD_GetVolume_Proc _Out_OAD_GetVolume;
		_Out_OAD_IsFulled_Proc _Out_OAD_IsFulled;
		_Out_OAD_GetBufferCount_Proc _Out_OAD_GetBufferCount;
		_Out_OAD_AddTimestamp_Proc _Out_OAD_AddTimestamp;
		_Out_OAD_GetLastTimestamp_Proc _Out_OAD_GetLastTimestamp;
};

#endif

