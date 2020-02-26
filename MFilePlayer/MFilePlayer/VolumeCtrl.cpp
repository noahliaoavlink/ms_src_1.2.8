#include "stdafx.h"
#include "VolumeCtrl.h"

VolumeCtrl::VolumeCtrl()
{
	CoInitialize(NULL);
	defaultDevice = NULL;
	endpointVolume = NULL;
}

VolumeCtrl::~VolumeCtrl()
{
	if(endpointVolume)
		endpointVolume->Release();
	CoUninitialize();
}

void VolumeCtrl::Init()
{
	HRESULT hr=NULL;
	IMMDeviceEnumerator *deviceEnumerator = NULL;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, 
                          __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
 
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    deviceEnumerator = NULL;

    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), 
         CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
    defaultDevice->Release();
    defaultDevice = NULL;
}

void VolumeCtrl::SetVolume(int nVolume)
{
	HRESULT hr=NULL;
	float fVolume;
    fVolume = nVolume/100.0f;
	hr = endpointVolume->SetMasterVolumeLevelScalar((float)fVolume, NULL);
}

DWORD VolumeCtrl::GetVolume()
{
	HRESULT hr=NULL;
	float currentVolume;
	hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
	m_iCurVolume = currentVolume * 100.0 + 0.1;
	return m_iCurVolume;
}