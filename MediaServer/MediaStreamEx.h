#pragma once

#include "MediaStream.h"
#include "../../../Include/FilePlayerCommon.h"

#include "D3DRender.h"

class CMediaStreamEx : public VideoRawDataCallback , public CMediaStream
{
	friend class CMixerEffect;

	D3DRender* m_pD3DRender;
	CMixerEffect* m_pMixerEffect;
	VideoRawDataCallback* m_pVideoRawDataCallback;
	void SetVideoRawDataCallback(void* pObj);
public:
	CMediaStreamEx(HWND hWnd);
	~CMediaStreamEx();

	void Init(HWND hWnd);

	void InitRender(HWND hWnd, void* pVoid = NULL);
	D3DRender* GetD3DRender() { return m_pD3DRender; };
	BOOL HasRender() { return (m_pD3DRender != NULL); }
	void SetMixerEffect(CMixerEffect* pObj);
	void SetEffectIndex(int iID);
	int GetEffectIndex();
	void SetEffectParam(int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5);

	virtual void DisplayVideoRawData(unsigned char* pBuffer, int w, int h);
	virtual void DisplayVideoSurfaceData(void* pSurface, int w, int h);
	virtual void DisplayNV12SurfaceData(void* pSurface, int w, int h);
};
