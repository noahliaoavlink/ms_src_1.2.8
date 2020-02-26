#pragma once

#ifndef _TL_Out_Device_H_
#define _TL_Out_Device_H_

#define _TL_MAX_LAYER 15//3

enum StreamType
{
	ST_MAIN = 0,
	ST_LAYER1,
	ST_LAYER2,
	ST_LAYER3,
	ST_STREAM0,
	ST_STREAM1,
	ST_STREAM2,
	ST_STREAM3,
	ST_STREAM4,
	ST_STREAM5,
	ST_STREAM6,
	ST_STREAM7,
	ST_STREAM8,
	ST_STREAM9,
};

enum PlaybackCommand
{
	PBC_NONE,
	PBC_STOP,
	PBC_PLAY,
	PBC_PAUSE,
	PBC_CLOSE,
};

class OutDevice
{
		char m_szGroupName[128];
	public:
		//OutDevice();
		//~OutDevice();

		virtual void SetGroupName(char* szName) { strcpy(m_szGroupName, szName); };
		virtual char* GetGroupName() { return m_szGroupName; };
		virtual double GetCurPos() = 0;

		//playback functions
		virtual void Open(char* szFileName) = 0;
		virtual void Close() = 0;
		virtual void Play() = 0;
		virtual void Stop() = 0;
		virtual void Stop2() = 0;
		virtual void Pause() = 0;
		virtual void Seek(float fPos) = 0;
		virtual void ResetFilter() = 0;
		virtual void SetStartTime(double dTime) = 0;
		virtual void UpdateGlobalTimeCode(double dTimeCode) = 0;
		virtual void EnableAudio(bool bEnable) = 0;

		//a/v effect functions
		virtual void SetVolume(int iValue) = 0;
		virtual void SetBrightness(int iValue) = 0;
		virtual void SetContrast(int iValue) = 0;
		virtual void SetSaturation(int iValue) = 0;
		virtual void SetHue(int iValue) = 0;
		virtual void SetR(int iValue) = 0;
		virtual void SetG(int iValue) = 0;
		virtual void SetB(int iValue) = 0;
		virtual void SetGray(int iValue) = 0;
		virtual void SetNegative(int iValue) = 0;

		virtual bool IsOpened() = 0;

		virtual void SetEffectIndex(int iID) = 0;
		virtual int GetEffectIndex() = 0;
		//virtual void SetEffectParam(int iID, int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5) = 0;
		virtual void SetEffectParam(int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5) = 0;
		virtual void SetEffectLevel(int iLevel) = 0;
		virtual void SetEffectParam1(int iParm) = 0;
		virtual void SetEffectParam2(int iParm) = 0;
		virtual void SetEffectParam3(int iParm) = 0;
		virtual void SetEffectParam4(int iParm) = 0;
		virtual void SetEffectParam5(int iParm) = 0;

		//fade in / out
		virtual void SetFadeDuration(float fValue) = 0;
		virtual void EnableFadeIn(bool bEnable) = 0;
		virtual void EnableFadeOut(bool bEnable) = 0;
};

class OutDeviceManager
{
		OutDevice* m_pLayerOutDevice[_TL_MAX_LAYER]; //layer1 ~ layer3
		OutDevice* m_pMainStream;
		int m_iLayerPBStatus[_TL_MAX_LAYER];
		int m_iMainStreamPBStatus;
	public:
		OutDeviceManager();
		~OutDeviceManager();

		void SetMainStreamOutDevice(OutDevice* m_pObj);
		void SetLayerOutDevice(int iIndex,OutDevice* m_pObj);

		OutDevice* GetMainStreamOutDevice();
		OutDevice* GetLayerOutDevice(int iIndex);
		void SetLayerPBStatus(int iIndex,int iCmd);
		int GetLayerPBStatus(int iIndex);
		void SetMainStreamPBStatus(int iCmd);
		int GetMainStreamPBStatus();
};

class TL_Out_Device
{
		OutDeviceManager* m_pOutDeviceManager;
	protected:
	public:
		TL_Out_Device();
		~TL_Out_Device();

		void SetOutDevice(int iType,OutDevice* pObj);
		OutDevice* GetOutDevice(int iType);
		void SetLayerPBStatus(int iIndex, int iCmd);
		int GetLayerPBStatus(int iIndex);
		void SetMainStreamPBStatus(int iCmd);
		int GetMainStreamPBStatus();

};

#endif
