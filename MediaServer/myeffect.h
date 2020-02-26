#pragma once

#ifndef _MyEffect_H_
#define _MyEffect_H_

//#include <d3d9.h>
#include <strsafe.h>
#include <math.h>
#include "stdafx.h"

void LOG( char* strMsg, ... );


#ifndef LevAdj
#define LevAdj(iNewValue, iOldValue, iLevel) BYTE((iOldValue) +(BYTE)((iLevel)*((iNewValue) - (iOldValue))))
#endif

#ifndef V
#define V(x)    { hr = (x); if( FAILED(hr) ) { LOG( "%s (0x%x), Line:%i", __func__   , hr,  __LINE__); } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p)=NULL; } }
#endif


//extern struct sEffectParam g_EffectParam;

struct sEffectD3DParam
{
	IDirect3DDevice9	*d3ddev;
	
	IDirect3DSurface9	*pVideoStream;

	IDirect3DSurface9	*pOffscreenSurface2D; //2D特效
	IDirect3DTexture9	*pOffscreenTexture3D; //3D特效  新增
	IDirect3DTexture9	*pOutFrameTexture;	  //輸出的結果
	
	//新增
	IDirect3DSurface9		*pzSurface;
	
	sEffectD3DParam()
	{
		d3ddev=NULL;
		pOffscreenSurface2D=NULL;
		pOutFrameTexture=NULL;
		pOffscreenTexture3D=NULL;
	}
};

typedef struct sEffectParam
{
	int iLevel, iParm1, iParm2, iParm3, iParm4, iParm5;

} _sEffectParam;

////////////////////////////////////////////////////////////////////////////////////////////////
 
class CMyEffect
{
	HRESULT hr;

	D3DSURFACE_DESC				m_rcVideo;
	D3DLOCKED_RECT				m_offScreenLockedRc;

	
	//pFrameTexture; //  把video的影片畫面抓進來， 必須是D3DPOOL_DEFAULT
	//pOffscreenSurface;// D3DPOOL_SYSTEMMEM
	DWORD* LockOffsereenData( const sEffectD3DParam &D3DParam  )
	{
		IDirect3DSurface9 *pOutFrameSurface;
		V( D3DParam.pOutFrameTexture->GetSurfaceLevel( 0, &pOutFrameSurface ) );
		V( D3DParam.d3ddev->StretchRect( D3DParam.pVideoStream, NULL, pOutFrameSurface, NULL, D3DTEXF_NONE ) ); 

		V( D3DParam.d3ddev->GetRenderTargetData( pOutFrameSurface, D3DParam.pOffscreenSurface2D ) );

		V( D3DParam.pOffscreenSurface2D->GetDesc( &m_rcVideo ) );
		V( D3DParam.pOffscreenSurface2D->LockRect( &m_offScreenLockedRc, 0,0) );
		pOutFrameSurface->Release();
		return (DWORD *)m_offScreenLockedRc.pBits;
	}
	void UnLockOffSereenAndUpdate( const sEffectD3DParam &D3DParam )
	{
		V( D3DParam.pOffscreenSurface2D->UnlockRect() );

		POINT destPT; destPT.x=destPT.y=0;
		
		IDirect3DSurface9 *pOutFrameSurface;
		V( D3DParam.pOutFrameTexture->GetSurfaceLevel( 0, &pOutFrameSurface ) );
		V( D3DParam.d3ddev->UpdateSurface( D3DParam.pOffscreenSurface2D, NULL, pOutFrameSurface, &destPT ) );
		pOutFrameSurface->Release();
	}
	

	int Chk(int v, int minv, int maxv)
	{
		if(v<minv)return minv;
		if(v>maxv)return maxv;
		return v;
	}
	
public:
	void Create(IDirect3DDevice9 *pDevice ){}
	void Destroy(){}

	void NormalVideo( const sEffectD3DParam &D3DParam, sEffectParam effectParam )
	{
		IDirect3DSurface9 *pOutFrameSurface;
		V( D3DParam.pOutFrameTexture->GetSurfaceLevel( 0, &pOutFrameSurface ) );
		V( D3DParam.d3ddev->StretchRect( D3DParam.pVideoStream, NULL, pOutFrameSurface, NULL, D3DTEXF_NONE ) ); 
	}
	

	//暫停畫格	多少張秀一次
	// Really sleazy emboss ?�起?��?
	void SleazyEmboss( const sEffectD3DParam &D3DParam, struct sEffectParam effectParam)
	{
		float  level  = (float)effectParam.iLevel/100.f;
		DWORD *imagedata = LockOffsereenData( D3DParam );
		DWORD color = *(imagedata+0);
		BYTE  r = BYTE((color & 0x00ff0000)>>16); 	BYTE  g = BYTE((color & 0x0000ff00)>>8); 	BYTE  b = BYTE((color & 0x000000ff)); 
		unsigned int grey2 = (r + g) >> 1;
		r = g = b = (BYTE) 128;
		int grey;
		int temp;
		BYTE iNewR, iNewG, iNewB;

		for(DWORD y = 0; y < m_rcVideo.Height; y++)
		{
			for(DWORD x = 1; x < m_rcVideo.Width; x++)
			{
				color = *(imagedata+x);
				r = BYTE((color & 0x00ff0000)>>16);  g = BYTE((color & 0x0000ff00)>>8); b = BYTE((color & 0x000000ff)); 

				grey = (r + g) >> 1;
				temp = grey - grey2;
				if (temp >  127) temp = 127;
				if (temp < -127) temp = -127;
				temp += 127*(100 - effectParam.iParm1)/100;
				grey2 = grey;

				iNewR = LevAdj(temp, r, level);
				iNewG = LevAdj(temp, g, level);
				iNewB = LevAdj(temp, b, level);

				*(imagedata+x) = 0xff000000 | (iNewR<<16) | (iNewG<<8) | (iNewB);

				r = g = b = (BYTE) temp;
			}
			imagedata = imagedata + m_rcVideo.Width;
		}

		UnLockOffSereenAndUpdate( D3DParam );
	}

	void DelayMovie(  const sEffectD3DParam &D3DParam, struct sEffectParam effectParam )
	{
		//int delay = g_EffectParam.iLevel; 
		int delay = effectParam.iLevel;
		static int s_movietime=0;
		s_movietime++;
		if(s_movietime>=delay)
		{
			IDirect3DSurface9 *pOutFrameSurface;
			V( D3DParam.pOutFrameTexture->GetSurfaceLevel( 0, &pOutFrameSurface ) );
			V( D3DParam.d3ddev->StretchRect( D3DParam.pVideoStream, NULL, pOutFrameSurface, NULL, D3DTEXF_NONE ) ); 
			s_movietime=0;
			pOutFrameSurface->Release();
		}	 
	}
	 
	 
};


//extern class CMyEffect  C_MyEffect;

#endif
