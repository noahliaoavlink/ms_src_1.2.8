#ifndef __OutAudioDevice_H
#define __OutAudioDevice_H

#include "DSound.h"

class OutAudioDevice
{
		DSound* m_pDSound;
		HWND m_hWnd;

		bool m_bIsOpened;

		//for wave out
		int m_nChannels;
		int m_nFrequency;
		int m_nBits;
		int m_iBufferSize;
		int m_iTotalOfBuffer;
   public:
	    OutAudioDevice();
        ~OutAudioDevice();

		void Init(HWND hWnd);
		void Open(bool bReset = false);
		void Close();
		void Output(char* pBuffer,int iLen);
		void Stop();
		void Continue();
		void SetWaveFormat(int nChannels,int nFrequency,int nBits);
		void SetBufferInfo(int iBufferSize,int iTotalOfBuffers);
		bool IsReady();
		void SetVolume(int iValue);
		long GetVolume();
		bool IsOpened();
		void ResetBuffer();
		bool IsFulled();
		int GetBufferCount();
		void AddTimestamp(unsigned long ulTimestamp);
		unsigned long GetLastTimestamp();
};

#endif