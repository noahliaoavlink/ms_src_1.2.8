#include "stdafx.h"
#include "RtspClientDll.h"

RtspClientDll::RtspClientDll()
{
	m_hInst = 0;
	LoadLib();
}

RtspClientDll::~RtspClientDll()
{
	FreeLib();
}

void RtspClientDll::LoadLib()
{
	char szFileName[] = "RtspStreamClient.dll";
	m_hInst = LoadLibraryA(szFileName);

	if(m_hInst)
	{
		Create = (Create_Rtsp_Proc)GetProcAddress(m_hInst, "Create");
		Destroy = (Destroy_Rtsp_Proc)GetProcAddress(m_hInst, "Destroy");
		Open = (Open_Proc)GetProcAddress(m_hInst, "Open");
		Close = (Close_Proc)GetProcAddress(m_hInst, "Close");
		SetUserInfo = (SetUserInfo_Proc)GetProcAddress(m_hInst, "SetUserInfo");
		EnableOverTCP = (EnableOverTCP_Proc)GetProcAddress(m_hInst, "EnableOverTCP");
		EnableAudio = (EnableAudio_Proc)GetProcAddress(m_hInst, "EnableAudio");
		EnableReconnect = (EnableReconnect_Proc)GetProcAddress(m_hInst, "EnableReconnect");

		GetVideoCodecID = (GetVideoCodecID_Proc)GetProcAddress(m_hInst, "GetVideoCodecID");
		GetAudioCodecID = (GetAudioCodecID_Proc)GetProcAddress(m_hInst, "GetAudioCodecID");

		CheckNetworkStatus = (CheckNetworkStatus_Proc)GetProcAddress(m_hInst, "CheckNetworkStatus");
	}
}

void RtspClientDll::FreeLib()
{
	if(m_hInst)
		FreeLibrary(m_hInst);
}
