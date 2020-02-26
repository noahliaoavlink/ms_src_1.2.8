#ifndef _VolumeCtrl_H_
#define _VolumeCtrl_H_

#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <mmsystem.h>

class VolumeCtrl
{
		IMMDevice *defaultDevice;
		IAudioEndpointVolume *endpointVolume;
		//float currentVolume;
		int m_iCurVolume;
	public:
		VolumeCtrl();
        ~VolumeCtrl();

		void Init();
		void SetVolume(int nVolume);
		DWORD GetVolume();
};

#endif