#pragma once
#include "MyEffect3D.h"

enum PixelFormat
{
	PF_YUV = 0,
	PF_RGB32,
};

class CMixerEffect
{
private:
	int m_ieffectID;
	sEffectParam m_EffectParam;	
	sEffectD3DParam m_D3DParam;
	CMyEffect3D m_ImgEffect3D;
	CMyEffect	m_ImgEffect;
	IDirect3DDevice9* m_pD3DDev;
	D3DFORMAT m_VideoSurfaceFmt;
	int m_iWidth;
	int m_iHeight;
	BYTE* m_pImgBuf;
	CRITICAL_SECTION m_csInUse;
	BOOL Is3DEffect(INT32 iEffectIdx);
	CRect m_rectSrc, m_rectDest;
	INT32 m_bBusyFlag;

#ifdef	_ENABLE_MEDIA_STREAM_EX
	IDirect3DSurface9* m_surBuffaces;     //2D特效 必須在 D3DPOOL_SYSTEMMEM
	IDirect3DSurface9* m_sur3DBuf;        //3D特效 必須在 D3DPOOL_MANAGED	
	IDirect3DTexture9* m_BufTexture;
	IDirect3DTexture9* m_Buf3DTexture;
#endif
	IDirect3DSurface9* m_pEffectSurface;
	IDirect3DSurface9* m_pOutSurface;
	IDirect3DSurface9* m_pTempSurface;
	IDirect3DSurface9* m_pYUVSurface;
	IDirect3DSurface9* m_pTargetSurface;

	bool m_bInit3DEffect;

public:
	
	CMixerEffect();
	~CMixerEffect();

	int  GetEffectID();
	void SetEffectID(int id);
	sEffectParam& GetEffectParam(); 
	void SetEffectParam(const sEffectParam& val);
	BYTE* GetImageBuffer() { return m_pImgBuf;};
	int CreateSurfaces(IDirect3DDevice9* pD3DDev, int iWidth, int iHeight);
	void VideoEffect(BYTE* pData, INT32 iWidth, INT32 iHeight);
	void VideoEffect2(BYTE* pData, INT32 iWidth, INT32 iHeight);
	void VideoEffect3(BYTE* pData, INT32 iWidth, INT32 iHeight, int iForamt = PF_YUV);
	void VideoEffect_Surface(IDirect3DSurface9* pSurface, INT32 iWidth, INT32 iHeight);
	void SetEffectParam(int iLevel, int iParm1, int iParm2, int iParm3, int iParm4, int iParm5);
	void SetEffectParam(int iIndex, int iValue);
	INT32 GetEffectParam(int iIndex);
#ifdef	_ENABLE_MEDIA_STREAM_EX
	int CreateSurfaces(D3DRender* pD3DRender);
	void VideoEffect(struct sEffectD3DParam D3DParam) 
	{ 
		m_D3DParam = D3DParam;
		VideoEffect(); 
	};
#endif
	void SetSrcArea(CRect rectSrc) { m_rectSrc = rectSrc;};
	void SetDestArea(CRect rectDest) { m_rectDest = rectDest;};
	HRESULT LockSurface();
	void DestroySurfaces();
	void Create(IDirect3DDevice9* pD3DDev, D3DFORMAT foramt, int iWidth, int iHeight);
	int GetWidth();
	int GetHeight();
	int CreateSurfaces();
	void ResetSurfaces(int w, int h);
	IDirect3DSurface9* DoVideoEffect(BYTE* pData, INT32 iWidth, INT32 iHeight);
	IDirect3DSurface9* DoVideoEffect2(BYTE* pData, INT32 iWidth, INT32 iHeight, int iForamt = PF_YUV);
	IDirect3DSurface9* DoVideoEffect_Surface(IDirect3DSurface9* pSurface, INT32 iWidth, INT32 iHeight);
	bool IsZoomed()
	{
		CRect rectOrigin = CRect(0, 0, m_iWidth, m_iHeight);

		if (m_rectSrc != rectOrigin || m_rectDest != rectOrigin)
			return true;
		else
			return false;
	}

	void ResetD3DDevice(IDirect3DDevice9* pD3DDev);

	void SetTime(float fTime) 
	{
		m_ImgEffect3D.SetTime(fTime);
	};

	float GetTime()
	{
		return m_ImgEffect3D.GetTime();
	};

};
