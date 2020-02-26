#pragma once
#include "MyEffect.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "d3dx9shader.h"
#include "d3dx9tex.h"
#pragma comment(lib,"D3dx9.lib")

#include <deque>
using namespace std;

#define PI 3.14159265f



void DefaultViewMatrix(IDirect3DDevice9* d3ddev);
void DefaultProjMatrix(IDirect3DDevice9* d3ddev, float asp);

const D3DXVECTOR4 c_eyepos = { 0, 0, -2.4f, 0 };

struct HP_VERTEX
{
	float	x,y,z;
	DWORD	color;
	float   tu, tv;

	void Set(float _x, float _y, float _z, DWORD _color, float _tu, float _tv)
	{
		x=_x;	y=_y;	z=_z;  	color=_color; 	tu=_tu;	tv=_tv;
	}
};

const int NUM_VERTICES=18;

#include "cylinder.h"

#define D3DFVF_HP_VERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

const D3DVERTEXELEMENT9 VERTEX_DECL[] =
{
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};


class CMyEffect3D
{
	HRESULT hr;
	ID3DXEffect					*m_pEffect;
	IDirect3DVertexDeclaration9	*m_pDecl;

	WCHAR					m_szWorkDir[MAX_PATH];

	int						m_video_w, m_video_h;

	static const int MAX_NEONLIGHT=5; //霓虹燈
	static const int MAX_WIN_NORMAP=7;//玻璃窗
	IDirect3DTexture9			*m_pTexMirage, *m_pTexBlind, *m_szpTexNeonlight[MAX_NEONLIGHT], *m_szpNormalmap[MAX_WIN_NORMAP];

	IDirect3DSurface9 *m_pOldZBuffer;
	static const int MAX_BLUR_TEX=16;//Blur
	deque<IDirect3DTexture9*>	m_quBlurTex;


	D3DXMATRIX					m_Identity;


	static const int			MAX_PUZZLE_INT = 60;
	int							m_szPuzzle[MAX_PUZZLE_INT];

	int m_iStatus;
	float m_fTime;

	//***重新開啟影片,m_pEffect, m_pDecl 會LOST ***
	void CreateShader(IDirect3DDevice9 *d3ddev  )
	{
		if(m_pEffect==NULL )
		{
			WCHAR sz[MAX_PATH];	swprintf_s(sz, L"%s/MyShader.hlsl", m_szWorkDir);
			DWORD dwFlags = D3DXFX_NOT_CLONEABLE;
			V( D3DXCreateEffectFromFileW( d3ddev,  sz, NULL, NULL, dwFlags, NULL, &m_pEffect, NULL ) );

			V( d3ddev->CreateVertexDeclaration( VERTEX_DECL, &m_pDecl ) );
			V( d3ddev->SetVertexDeclaration( m_pDecl ));
		}
	}

	//***重新開啟影片,texture會LOST ***
	void CreateTexture(IDirect3DDevice9 *d3ddev  )
	{
		WCHAR sz[MAX_PATH];	
		if(m_pTexBlind==NULL)
		{ 
#if 0
			swprintf_s(sz, L"%s/pic/blind.png", m_szWorkDir); 
			V( D3DXCreateTextureFromFileExW( d3ddev, sz, D3DX_DEFAULT, D3DX_DEFAULT,	1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,  D3DX_DEFAULT, D3DX_DEFAULT, 0,	NULL, NULL, &m_pTexBlind ) );
		
			swprintf_s(sz, L"%s/pic/mirage.png", m_szWorkDir); 
			V( D3DXCreateTextureFromFileExW( d3ddev, sz, D3DX_DEFAULT, D3DX_DEFAULT,	1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,  D3DX_DEFAULT, D3DX_DEFAULT, 0,	NULL, NULL, &m_pTexMirage ) );
			
			for (int i=0; i<MAX_WIN_NORMAP;i++)
			{
				swprintf_s(sz, L"%s/pic/window-normal%d.jpg", m_szWorkDir, i); 
				V( D3DXCreateTextureFromFileExW( d3ddev, sz, D3DX_DEFAULT, D3DX_DEFAULT,	1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,  D3DX_DEFAULT, D3DX_DEFAULT, 0,	NULL, NULL, &m_szpNormalmap[i] ) );
			}
			
			for (int i=0; i<MAX_NEONLIGHT;i++)
			{
				swprintf_s(sz, L"%s/pic/Neonlight%d.png", m_szWorkDir, i); 
				V( D3DXCreateTextureFromFileExW( d3ddev, sz, D3DX_DEFAULT, D3DX_DEFAULT,	1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,  D3DX_DEFAULT, D3DX_DEFAULT, 0,	NULL, NULL, &m_szpTexNeonlight[i] ) );
			}
#else
			swprintf_s(sz, L"%s/pic/blind.png", m_szWorkDir);
			V( D3DXCreateTextureFromFileExW( d3ddev, sz, D3DX_DEFAULT, D3DX_DEFAULT,	1, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,  D3DX_DEFAULT, D3DX_DEFAULT, 0,	NULL, NULL, &m_pTexBlind ) );
			
			swprintf_s(sz, L"%s/pic/mirage.png", m_szWorkDir);
			V(D3DXCreateTextureFromFileExW(d3ddev, sz, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_pTexMirage));

			for (int i = 0; i<MAX_WIN_NORMAP; i++)
			{
				swprintf_s(sz, L"%s/pic/window-normal%d.jpg", m_szWorkDir, i);
				V(D3DXCreateTextureFromFileExW(d3ddev, sz, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_szpNormalmap[i]));
			}

			for (int i = 0; i<MAX_NEONLIGHT; i++)
			{
				swprintf_s(sz, L"%s/pic/Neonlight%d.png", m_szWorkDir, i);
				V(D3DXCreateTextureFromFileExW(d3ddev, sz, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_szpTexNeonlight[i]));
			}
#endif

		}
	}

	void ShaderDraw(IDirect3DDevice9 *d3ddev,  IDirect3DTexture9 *pVideoTex, IDirect3DTexture9 *pSecondTex, 
					char *techname, const HP_VERTEX szV[], const D3DXMATRIX &mWorld, int nTinangleTrip=2 )
	{
		D3DXMATRIX  mView, mProj, mVP;

		m_iStatus = 0;
		
		V( d3ddev->GetTransform( D3DTS_PROJECTION,	&mProj ) );
		V( d3ddev->GetTransform( D3DTS_VIEW,		&mView ) );
		
		mVP = mView * mProj;

		ID3DXEffect *p = m_pEffect;
		V( p->SetTexture( p->GetParameterByName( NULL, "g_texVideo"),			pVideoTex) );
		V( p->SetMatrix(  p->GetParameterByName( NULL, "g_mObjWorld"),			&mWorld  ) );
		V( p->SetMatrix(  p->GetParameterByName( NULL, "g_mCameraViewProj"),	&mVP     ) );

		if(pSecondTex!=NULL)V( p->SetTexture(   p->GetParameterByName( NULL, "g_texSecond"),  pSecondTex   ) );

		V( d3ddev->SetVertexDeclaration( m_pDecl ) );
		V( m_pEffect->SetTechnique( p->GetTechniqueByName( techname) ) );

		if( SUCCEEDED( d3ddev->BeginScene() ) )
		{
			 UINT cPasses;
			 if( SUCCEEDED ( m_pEffect->Begin( &cPasses, 0 ) ) )
			 {
				 for( UINT iPass = 0; iPass < cPasses; iPass++ )
				 {
					 V( m_pEffect->BeginPass( iPass ) );
					 //V( d3ddev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, nTinangleTrip, szV, sizeof(HP_VERTEX) ) );
					 hr = d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, nTinangleTrip, szV, sizeof(HP_VERTEX));
					 if (FAILED(hr))
					 {
						 char szMsg[512];
						 sprintf(szMsg, "Effect3D - DrawPrimitiveUP Failed!!\n");
						 OutputDebugStringA(szMsg);
						 m_iStatus = -1;
					 }

					 V( m_pEffect->EndPass() );
				 }
				 V( m_pEffect->End() );
			 }
			 //V( d3ddev->EndScene() );
			 hr = d3ddev->EndScene();
			 if (FAILED(hr))
			 {
				 char szMsg[512];
				 sprintf(szMsg, "Effect3D - EndScene Failed!!\n");
				 OutputDebugStringA(szMsg);
				 m_iStatus = -2;
			 }
		 }
	}


	void StartRender( const sEffectD3DParam &D3DParam )
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;

		if (!D3DParam.pVideoStream)
			return;
		//noah
#if 0
		//1
		IDirect3DSurface9 *pOutFrameSurface;//D3DUSAGE_RENDERTARGETT
		V( D3DParam.pOutFrameTexture->GetSurfaceLevel( 0, &pOutFrameSurface ) ); //D3DUSAGE_RENDERTARGETT
		V( D3DParam.d3ddev->StretchRect( D3DParam.pVideoStream, NULL, pOutFrameSurface, NULL, D3DTEXF_NONE ) ); 
		D3DSURFACE_DESC rc; pOutFrameSurface->GetDesc(&rc);  m_video_w =rc.Width; m_video_h =rc.Height;
		//2
		IDirect3DSurface9 *offTextureSurface3D; //D3DPOOL_SYSTEMMEM

		if (!D3DParam.pOffscreenTexture3D)
			return;

		V( D3DParam.pOffscreenTexture3D->GetSurfaceLevel( 0,  &offTextureSurface3D ) );
		V( d3ddev->StretchRect( pOutFrameSurface, NULL, offTextureSurface3D, NULL, D3DTEXF_NONE ) ); 
#else
		IDirect3DSurface9 *pOutFrameSurface;//D3DUSAGE_RENDERTARGETT
		V(D3DParam.pOutFrameTexture->GetSurfaceLevel(0, &pOutFrameSurface)); //D3DUSAGE_RENDERTARGETT
/*
		IDirect3DSurface9 *offTextureSurface3D; //D3DPOOL_SYSTEMMEM

		if (!D3DParam.pOffscreenTexture3D)
			return;

		V(D3DParam.pOffscreenTexture3D->GetSurfaceLevel(0, &offTextureSurface3D));
		V(d3ddev->StretchRect(D3DParam.pVideoStream, NULL, offTextureSurface3D, NULL, D3DTEXF_NONE));
		*/
#endif
		m_pOldZBuffer = NULL;
		d3ddev->GetDepthStencilSurface(&m_pOldZBuffer);
		//3
		V( d3ddev->SetRenderTarget(0, pOutFrameSurface ) );//D3DUSAGE_RENDERTARGETT
		V( d3ddev->SetDepthStencilSurface( D3DParam.pzSurface) );
		V( d3ddev->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L ) );
//		offTextureSurface3D->Release();
		pOutFrameSurface->Release();
	}

	IDirect3DSurface9 *CommonStart(IDirect3DDevice9 *d3ddev, float &asp)
	{
		IDirect3DSurface9 *backBuffer;   V( d3ddev->GetRenderTarget( 0, &backBuffer ) );
		D3DSURFACE_DESC backBufferDesc; backBuffer->GetDesc( &backBufferDesc );
		asp = (float)backBufferDesc.Width/(float)backBufferDesc.Height;
		DefaultViewMatrix( d3ddev );
		DefaultProjMatrix( d3ddev,  asp );
		
		return backBuffer;
	}

	void CommonEnd(IDirect3DDevice9 *d3ddev, IDirect3DSurface9 *backBuffer)
	{
		//Restore
		V( d3ddev->SetRenderTarget(0,backBuffer) );
		V( d3ddev->SetDepthStencilSurface(m_pOldZBuffer) );//***增加這個就沒問題了***
		DefaultViewMatrix(d3ddev);
	}

	void DefalutVertexData(float asp, struct HP_VERTEX szV[], float tex, float offtx=0.f, float offty=0.f )
	{
		szV[0].Set(-asp,  1.0f, 0.0f, 0xffffffff, 0.f, 0.f); // top left
		szV[1].Set(-asp, -1.0f, 0.0f, 0xffffffff, 0.f, tex); // bottom left
		szV[2].Set( asp,  1.0f, 0.0f, 0xffffffff, tex, 0.f); // top right
		szV[3].Set( asp, -1.0f, 0.0f, 0xffffffff, tex, tex); // bottom right
		
		for (int i=0;i<4;i++)
		{
			szV[i].tu+=offtx;
			szV[i].tv+=offty;
		}
	}
public:

	void Create( IDirect3DDevice9 *d3ddev)
	{
		//***重新開啟影片,m_pEffect, m_pDecl  texture 會LOST ***
		CreateShader( d3ddev );
		CreateTexture( d3ddev );
	}


	CMyEffect3D()
	{
		for (int i = 0; i < MAX_PUZZLE_INT; i++)m_szPuzzle[i] = i;

		m_video_w=1280; 
		m_video_h=720;

		m_pEffect=NULL;
		m_pDecl=NULL;

		m_pTexBlind=NULL;
		m_pTexMirage=NULL;

		m_fTime = 0.0;

		for (int i=0;i<MAX_NEONLIGHT;i++ )m_szpTexNeonlight[i]=NULL;
		for (int i=0;i<MAX_WIN_NORMAP;i++)m_szpNormalmap[i]=NULL;
		
		//std::wstring wsTmp(theApp.m_strCurPath.begin(), theApp.m_strCurPath.end());
		wchar_t drive[_MAX_DRIVE];
		wchar_t dir[_MAX_DIR];
		wchar_t fname[_MAX_FNAME];
		wchar_t ext[_MAX_EXT];
		wchar_t szModuleFileName[_MAX_PATH];
		if (0 == GetModuleFileNameW(NULL, szModuleFileName, _MAX_PATH))
			;
		_wsplitpath_s(szModuleFileName, drive, dir, fname, ext);

		std::wstring wsTmp = drive;
		wsTmp += dir;
		wcscpy(m_szWorkDir, wsTmp.c_str());
		//GetCurrentDirectoryW(MAX_PATH, m_szWorkDir);
			
		
		D3DXMatrixIdentity(&m_Identity);
	}

	void Destroy()
	{
		SAFE_RELEASE(m_pEffect);
		SAFE_RELEASE(m_pDecl);

		SAFE_RELEASE(m_pTexBlind);
		SAFE_RELEASE(m_pTexMirage);

		for (int i=0;i<MAX_NEONLIGHT;i++)SAFE_RELEASE(m_szpTexNeonlight[i]);
		for (int i=0;i<MAX_WIN_NORMAP;i++)SAFE_RELEASE(m_szpNormalmap[i]);
		for (DWORD i=0; i<m_quBlurTex.size();i++)SAFE_RELEASE(m_quBlurTex[i]);
		m_quBlurTex.clear();
	}

	~CMyEffect3D()
	{
		Destroy();
	}

	int GetStatus()
	{
		return m_iStatus;
	}

	void SetStatus(int iStatus)
	{
		m_iStatus = iStatus;
	}

	void SetTime(float fTime)
	{
		m_fTime = fTime;
	}

	float GetTime()
	{
		return m_fTime;
	}

	void ResetTime()
	{
		m_fTime = 0.0;
	}

	//多重影像 影片切割特效(3D)   iLevel:切割特效比例;  iParm1,iParm2( 貼圖做標位移,可以超過0~100); 
	void MultiMovie( const sEffectD3DParam &D3DParam, struct sEffectParam effectParam)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);

		float tex = (float)effectParam.iLevel/10.f; if(tex<1.f)tex=1.f;
		struct HP_VERTEX szV[4];
		float otu = (float)(effectParam.iParm1-50.f)/20.f;
		float otv = (float)(effectParam.iParm2-50.f)/20.f;
		DefalutVertexData(asp, szV, tex, otu, -otv );

		if(effectParam.iParm3>50)
		{
			V( d3ddev->SetSamplerState(	0,D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR ) );
			V( d3ddev->SetSamplerState(	0,D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR ) );
		}

		//5
		V( d3ddev->SetFVF( D3DFVF_HP_VERTEX ) );
		V( d3ddev->SetTexture( 0, D3DParam.pOffscreenTexture3D ) );		//SetTexture is not allowed with a pool type of D3DPOOL_SYSTEMMEM texture 
		V( d3ddev->BeginScene() );
		//V( d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2) );
		V( d3ddev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, szV, sizeof(HP_VERTEX) ) );
		V( d3ddev->EndScene());

		//Restore
		CommonEnd(d3ddev, backBuffer);
		V( d3ddev->SetSamplerState(	0,D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP ) );
		V( d3ddev->SetSamplerState(	0,D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP ) );
	}

	 
	
	//負片
	void Negative(const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp = 1.f;
		IDirect3DSurface9 *backBuffer = CommonStart(d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4];		DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_level", Scale(Param.iLevel, 0.f, 1.f)));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_negative", szV, m_Identity);
		}
		CommonEnd(d3ddev, backBuffer);
	}


	//交叉網格 iLevel:漸層  p1:密度
	void Crosshatching(const sEffectD3DParam &D3DParam, struct sEffectParam effectParam)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp = 1.f;
		IDirect3DSurface9 *backBuffer = CommonStart(d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4];		DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_level", Scale(effectParam.iLevel, 0.f, 1.f)));
			int mod_dist = max(2, effectParam.iParm1 / 8);

			V(m_pEffect->SetFloat("g_offset", (float)mod_dist));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_crosshatching", szV, m_Identity);
		}
		CommonEnd(d3ddev, backBuffer);
	}

	//灰階
	void Gray(const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp = 1.f;
		IDirect3DSurface9 *backBuffer = CommonStart(d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4];		DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_level", Scale(Param.iLevel, 0.f, 1.f)));

			float contrast = (Param.iParm1 - 50)*255.f / 50.f;
			float factor = (259.f * (contrast + 255.f)) / (255.f * (259.f - contrast));
			V(m_pEffect->SetFloat("g_factor", factor));
	
			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_gray", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//RGB 色偏 
	void RGBColor(const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp = 1.f;
		IDirect3DSurface9 *backBuffer = CommonStart(d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4];		DefalutVertexData(asp, szV, 1.f);
	 
		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_level", Scale(Param.iLevel, 0.f, 1.f)));
			V(m_pEffect->SetFloat("g_r", Scale(Param.iParm1, 0.f, 1.f)));
			V(m_pEffect->SetFloat("g_g", Scale(Param.iParm2, 0.f, 1.f)));
			V(m_pEffect->SetFloat("g_b", Scale(Param.iParm3, 0.f, 1.f)));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_rgbColor", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}
	
	//
	void RectangleBrightDark(const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp = 1.f;
		IDirect3DSurface9 *backBuffer = CommonStart(d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4];		DefalutVertexData(asp, szV, 1.f);
		
		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_level", Scale(Param.iLevel, 0.f, 1.f)));
			V(m_pEffect->SetFloat("g_dest_bright", Scale(Param.iParm1, -1.f, 1.f)));

			float minx = min(Param.iParm2, Param.iParm3) / 100.0f * (float)m_video_w;
			float maxx = max(Param.iParm2, Param.iParm3) / 100.0f * (float)m_video_h;


			float aminy = (float)Param.iParm4 / 100.f;
			float amaxy = (float)Param.iParm5 / 100.f;

			//opengles 左下角(00)
			float my = 1.f - aminy;
			float My = 1.f - amaxy;
			float miny = min(my, My);
			float maxy = max(my, My);

			miny *= (float)m_video_w;
			maxy *= (float)m_video_h;

			V(m_pEffect->SetFloat("g_minx", minx));
			V(m_pEffect->SetFloat("g_maxx", maxx));
			V(m_pEffect->SetFloat("g_miny", miny));
			V(m_pEffect->SetFloat("g_maxy", maxy));


			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_rectbrightdark", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}


	//畫面閃爍  level:特效程度	p1:閃爍頻率	p2:閃爍亮度幅度
	void BrightnessFlicker(const sEffectD3DParam &D3DParam, struct sEffectParam effectParam)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp = 1.f;
		IDirect3DSurface9 *backBuffer = CommonStart(d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4];		DefalutVertexData(asp, szV, 1.f);

	 
		float flickerspeed = (float)effectParam.iParm1 / 3000.f;

		float brightness = (float)effectParam.iParm2 * sin(flickerspeed * GetTickCount() ) + 50.0f;
		float dest_brightness = (255.f / 50.f*(brightness - 50.0f));
		dest_brightness /= 255.f;

		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_fLevel", Scale(effectParam.iLevel, 0.f, 1.0f)));
			V(m_pEffect->SetFloat("g_dest_bright", dest_brightness));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_brightflicker", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//iLevel:旋轉;  iParm1,iParm2:scal2 W H; iParm3,iParm4:位移x,y
	void RotateScale( const sEffectD3DParam &D3DParam, struct sEffectParam effectParam)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp = 1.f;
		IDirect3DSurface9 *backBuffer = CommonStart(d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4];		DefalutVertexData(asp, szV, 1.f);


		float radian = Scale(effectParam.iLevel, -180.f, 180.f) / 57.295779513f;

		float sx = Scale(effectParam.iParm1, 0.1f, 2.0f);
		float sy = Scale(effectParam.iParm2, 0.1f, 2.0f);

		float ox = Scale(effectParam.iParm3, -1.f, 1.f); ox *= asp;
		float oy = Scale(effectParam.iParm4, -1.f, 1.f);

		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_rotate", radian));
			V(m_pEffect->SetFloat("g_sx", sx));
			V(m_pEffect->SetFloat("g_sy", sy));

			V(m_pEffect->SetFloat("g_ox", ox));
			V(m_pEffect->SetFloat("g_oy", oy));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_rotatescale", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//像素化
	void Pixelate( const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp = 1.f;
		IDirect3DSurface9 *backBuffer = CommonStart(d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4];		DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_level", Scale(Param.iLevel, 1.f, 100.f)));
			V(m_pEffect->SetFloat("g_viewport_w", (float)m_video_w));
			V(m_pEffect->SetFloat("g_viewport_h", (float)m_video_h));


			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_pixelate", szV, m_Identity);
		}
		CommonEnd(d3ddev, backBuffer);
	}

	//扭曲: iLevel:扭曲比例, iParam1:扭曲半徑
	void SwirlTexture( const sEffectD3DParam &D3DParam, struct sEffectParam effectParam)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4];		DefalutVertexData(asp, szV, 1.f);
 
		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("swirl_amount", (float)effectParam.iLevel / 20.f));
			V(m_pEffect->SetFloat("swirl_radius", (float)effectParam.iParm1 / 10.f));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_swirl", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
 	}

	//百葉 iLevel:百葉窗的透明度, iParam1:百葉窗的密度,  iParam2:影片亮度
	void BlindTexture( const sEffectD3DParam &D3DParam, struct sEffectParam effectParam)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4];	DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_blend", (float)effectParam.iLevel / 100.f));
			float sx = (float)effectParam.iParm1 / 30.f; if (sx == 0.f)sx = 0.1f; 	V(m_pEffect->SetFloat("g_multiTex", sx));
			V(m_pEffect->SetFloat("g_bright", (float)effectParam.iParm2 / 100.f));


			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, m_pTexBlind, "tech_blind", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//霓虹燈 iLevel:混色, iParam1:縮放    iParm2:貼圖X旋轉   iParm3:貼圖Y旋轉 
	void NeonLightTexture( const sEffectD3DParam &D3DParam, struct sEffectParam effectParam)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			//static float s_time = 0.f;	s_time += (float)1.f / 30.f;	V(m_pEffect->SetFloat("g_time", s_time));
			m_fTime += (float)1.f / 30.f;	V(m_pEffect->SetFloat("g_time", m_fTime));

			V(m_pEffect->SetFloat("g_videoAsp", asp));//還跟視窗比例有關

			V(m_pEffect->SetFloat("g_blend", Scale(effectParam.iLevel, 0.01f, 0.9f)));
			V(m_pEffect->SetFloat("g_multiTex", Scale(effectParam.iParm1, 0.1f, 1.5f)));
			V(m_pEffect->SetFloat("g_neno_offx", Scale(effectParam.iParm2, 0.f, 2.f)));
			V(m_pEffect->SetFloat("g_neno_offy", Scale(effectParam.iParm3, 0.f, 2.f)));

			int iTex = rand() % MAX_NEONLIGHT;
			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, m_szpTexNeonlight[iTex], "tech_neon", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//HSL<-->RGB
	//iLevel:漸層,  iParm1:Lightness,  iParm2:Hue  iParm3:Saturation
	void HSL_RGB( const sEffectD3DParam &D3DParam, struct sEffectParam effectParam)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_fLevel", (float)effectParam.iLevel / 100.f));//[0,100]
			V(m_pEffect->SetFloat("g_l", Scale(effectParam.iParm1, -100, 100)));//[0,100]
			V(m_pEffect->SetFloat("g_h", Scale(effectParam.iParm2, -180, 180)));//[0,360]
			V(m_pEffect->SetFloat("g_s", Scale(effectParam.iParm3, -100, 100)));//[0,100]

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_hsl_to_rgb", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//iLevel:窗的密度,   iParm1:換窗的材質, 最多3種: pic\window-normalX.jpg 
	void WindowsNormal( const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);
		
		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_multiTex", Scale(Param.iLevel, 0.1f, 4)));
			int div = 100 / MAX_WIN_NORMAP;
			int st = min(Param.iParm1 / div, MAX_WIN_NORMAP - 1);
			V(m_pEffect->SetFloat("g_videoAsp", asp));//還跟視窗比例有關

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, m_szpNormalmap[st], "tech_window", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//水波紋 iLevel:  iParam1:速度  iParam2:密度,  iParam3:x位移, iParam4:y位移 
	void Ripple( const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			//static float s_time = 0.f;	s_time += (float)1.f / 30.f;	V(m_pEffect->SetFloat("g_time", s_time));
			m_fTime += (float)1.f / 30.f;	V(m_pEffect->SetFloat("g_time", m_fTime));

			V(m_pEffect->SetFloat("g_ripplelen", (float)Param.iParm2 + 20.f));
			V(m_pEffect->SetFloat("g_rippletime", (float)Param.iParm1 / 4.f));
			V(m_pEffect->SetFloat("g_ripplescale", (float)Param.iLevel / 1000.f));
			V(m_pEffect->SetFloat("g_ripplex", Scale(Param.iParm3, asp, -asp)));
			V(m_pEffect->SetFloat("g_rippley", Scale(Param.iParm4, -1.f, 1.f)));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_ripple", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//i=[0,100]
	float Scale(int i, float start, float end)
	{
		return start + (float)i/100.f *(end-start);
	}
	
	//魚眼 iLevel:凹凸變化，   iParm1:層數
	void FishEye( const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_fisheye", Scale(Param.iLevel, 1.5f, 1.f)));
			V(m_pEffect->SetFloat("g_fisheyeD", Scale(Param.iParm1, 0.4f, 0.7f)));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_fisheye", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//iLevel:點的密度 iParm1:飽和度
	void Halftone( const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_video_w", (float)m_video_w));
			V(m_pEffect->SetFloat("g_video_h", (float)m_video_h));
			V(m_pEffect->SetFloat("g_halftone", Scale(Param.iLevel, 1, 10)));
			V(m_pEffect->SetFloat("g_l", Scale(Param.iParm1, -200, 200)));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_halftone", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//iLevel:R iParm1:G iParm2:B   iParm3:x木紋的數量 iParm4:y木紋的數量
	void WoodNoise( const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			//static float s_time = 0.f;	s_time += (float)1.f / 30.f; V(m_pEffect->SetFloat("g_time", fabs(sin(s_time))));
			m_fTime += (float)1.f / 30.f; V(m_pEffect->SetFloat("g_time", fabs(sin(m_fTime))));

			V(m_pEffect->SetFloat("g_woodR", Scale(Param.iLevel, 0, 1.f)));
			V(m_pEffect->SetFloat("g_woodG", Scale(Param.iParm1, 0, 1.f)));
			V(m_pEffect->SetFloat("g_woodB", Scale(Param.iParm2, 0, 1.f)));
			V(m_pEffect->SetFloat("g_woodx", (float)((float)Param.iParm3 + 1) / 10.f));
			V(m_pEffect->SetFloat("g_woody", (float)((float)Param.iParm4 + 1) / 10.f));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_woodnoise", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//iLevel:線寬度
	void Edge( const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_video_w", (float)m_video_w));
			V(m_pEffect->SetFloat("g_video_h", (float)m_video_h));
			V(m_pEffect->SetFloat("g_edgewidth", (float)((float)Param.iLevel + 0.8f) / 100.f));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_edge", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//iLevel:波浪數目  iParm1:速度(頻率)  iParm2:強波幅度
	void Wave( const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			//static float s_time = 0.f;	s_time += (float)1.f / 30.f; 	V(m_pEffect->SetFloat("g_time", s_time));
			m_fTime += (float)1.f / 30.f; 	V(m_pEffect->SetFloat("g_time", m_fTime));
			V(m_pEffect->SetFloat("g_waveNum", Scale(Param.iLevel, 1, 20)));
			V(m_pEffect->SetFloat("g_waveFreq", Scale(Param.iParm1, 1, 10)));
			V(m_pEffect->SetFloat("g_waveTwist", Scale(Param.iParm2, 0.01f, 0.2f)));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_wave", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//水彩畫  iLevel:變化   iParm1:飽和度
	void WaterColor(const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_video_w", (float)m_video_w));
			V(m_pEffect->SetFloat("g_video_h", (float)m_video_h));
			V(m_pEffect->SetFloat("g_colorpaint", Scale(Param.iLevel, 1.f, 14.f)));
			V(m_pEffect->SetFloat("g_s", Scale(Param.iParm1, -100.f, 100.f)));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_watercolor", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//萬花筒 iLevel:變化   iParam1:速度
	void Kaleidoscope( const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			//static float s_time = 0.f;	s_time += (float)1.f / 20.f; V(m_pEffect->SetFloat("g_time", sin(s_time)));
			m_fTime += (float)1.f / 20.f; V(m_pEffect->SetFloat("g_time", sin(m_fTime)));

			V(m_pEffect->SetFloat("g_kaleidoscope", Scale(Param.iLevel, 1.f, 10.f)));
			V(m_pEffect->SetFloat("g_kaleidTime", Scale(Param.iParm1, 0.01f, 2.f)));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_kaleidoscope", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//同心圓 iLevel:分成多少個圓		iParm1:奇數圈旋轉方向   iParm2:奇數圈旋轉速度[0,100]0:不旋轉
	//								iParm3:偶數圈旋轉方向   iParm4:偶數圈旋轉速度[0,100]0:不旋轉
	void CircleCenter( const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);

		//static float s_time = 0.f;	s_time += (float)1.f / 30.f;
		m_fTime += (float)1.f / 30.f;

		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_circle_radius", Scale(Param.iLevel, 0.5f, 0.01f)));
			//奇數圈
			V(m_pEffect->SetFloat("g_circle_angle", Scale(Param.iParm1, -300.f, 300.f)));
			float speed = Scale(Param.iParm2, 0.f, 3.f); V(m_pEffect->SetFloat("g_time", m_fTime*speed));

			//偶數圈
			V(m_pEffect->SetFloat("g_circle_angleEven", Scale(Param.iParm3, -300.f, 300.f)));
			speed = Scale(Param.iParm4, 0.f, 3.f);	V(m_pEffect->SetFloat("g_timeEven", m_fTime*speed));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_circlecenter", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	float ToRadius(float angle)
	{
		return angle *(PI/180.f);
	}

	//隧道: iLevel:multiTex  iParm1:(左右)旋轉速度  iParm2:(前進後退)速度   
	void CylinderRotate( const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		//struct HP_VERTEX szV[4]; DefalutVertexData(asp, szV, 1.f);
		//float tex=1.f;
		
		int a = (int)Scale(Param.iLevel, 1.f, 10.f); if (a % 2 == 1)a += 1; //取偶數
		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_multiTex", float(a)));
			V(m_pEffect->SetVector("g_eyepos", &c_eyepos));
			V(m_pEffect->SetFloat("g_cyl_x", Scale(Param.iParm1, -0.8f, 0.8f)));
			V(m_pEffect->SetFloat("g_cyl_y", Scale(Param.iParm2, -4.f, 4.0f)));



			D3DXMATRIX W, R, S, T;
			D3DXMatrixScaling(&S, 1.f, 1.f, 1.f);
			D3DXMatrixRotationX(&R, ToRadius(-90.f));

			//改做貼圖做標的旋轉就好
			D3DXMatrixTranslation(&T, 0, 0, 6);
			W = S*R*T;

			//static float s_time = 0.f;	s_time += (float)1.f / 30.f;	V(m_pEffect->SetFloat("g_time", s_time));
			m_fTime += (float)1.f / 30.f;	V(m_pEffect->SetFloat("g_time", m_fTime));
			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_cylinder", s_szCylinder, W, CYLINDER_NUM_STRIP);

		}
		//Restore
		CommonEnd(d3ddev, backBuffer);

	}
	
	//平面旋轉:  iLevel:multi_tex(mirror)     iParm1:正反轉 iParm2:貼圖位移左右  iParm2:貼圖位移前進 iParm4:平面的距離
	void PlaneRotate(const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp = 1.f;
		IDirect3DSurface9 *backBuffer = CommonStart(d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4];  
		float tex=1;
		szV[0].Set(-asp,  0.0f,  1.0f, 0xffffffff, 0.f, 0.f); // top left
		szV[1].Set(-asp,  0.0f, -1.0f, 0xffffffff, 0.f, tex); // bottom left
		szV[2].Set( asp,  0.0f,  1.0f, 0xffffffff, tex, 0.f); // top right
		szV[3].Set( asp,  0.0f, -1.0f, 0xffffffff, tex, tex); // bottom right
		
		if (m_pEffect)
		{
			V(m_pEffect->SetVector("g_eyepos", &c_eyepos));

			int a = (int)Scale(Param.iLevel, 1.f, 40.f); if (a % 2 == 1)a += 1; //取偶數
			V(m_pEffect->SetFloat("g_multiTex", float(a)));

			float angle = Scale(Param.iParm1, -360, 360);

			float dir = 1; if (angle > 0)dir = -1;


			V(m_pEffect->SetFloat("g_plane_x", Scale(Param.iParm2, -0.3f, 0.3f)));
			V(m_pEffect->SetFloat("g_plane_y", Scale(Param.iParm3, -0.3f, 0.3f)));

			float planeDist = Scale(Param.iParm4, 0.5f, 2.6f);

			//static float s_time = 0.f;	s_time += (float)1.f / 30.f;	V(m_pEffect->SetFloat("g_time", s_time));
			m_fTime += (float)1.f / 30.f;	V(m_pEffect->SetFloat("g_time", m_fTime));

			D3DXMATRIX W, S, T, R2, T2;
			D3DXMatrixScaling(&S, 10.f, 10.f, 10.f);

			D3DXMatrixRotationZ(&R2, ToRadius(angle*m_fTime)); //旋轉角度

			float off[2] = { -1.0f*planeDist, 1.0f*planeDist };//上面跟下面
			for (int i = 0; i < 2; i++)
			{
				D3DXMatrixTranslation(&T, 0, off[i], 0);	W = S*T*R2;
				ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_planerotate", szV, W);
			}
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//圓形馬賽克   iLevel:圓點的大小   iParm1:亮度  iParm2:燈光硬度
	void DotMosaics( const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp=1.f;
		IDirect3DSurface9 *backBuffer = CommonStart( d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_video_w", (float)m_video_w));
			V(m_pEffect->SetFloat("g_video_h", (float)m_video_h));
			V(m_pEffect->SetFloat("g_videoAsp", asp));//還跟視窗比例有關


			V(m_pEffect->SetInt("g_dotmosaics", (int)Scale(Param.iLevel, 0, 30)));
			V(m_pEffect->SetFloat("g_dotmosaics_bright", Scale(Param.iParm1, 0.5, 7.f)));
			V(m_pEffect->SetInt("g_dotmosaics_kind", (int)Scale(Param.iParm2, 0, 4)));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_dotmosaics", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	//保留單一色 iLevel:range(誤差),  iParm1:R  iParm2:G iParm3:B   iParm2:飽和度  
	void SingleColor(const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp = 1.f;
		IDirect3DSurface9 *backBuffer = CommonStart(d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);
		
		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_singlediff", Scale(Param.iLevel, 0.05f, 0.8f)));
			float r = Scale(Param.iParm1, 0, 1.f);
			float g = Scale(Param.iParm2, 0, 1.f);
			float b = Scale(Param.iParm3, 0, 1.f);

			D3DXVECTOR4 rgb(r, g, b, 1);
			V(m_pEffect->SetVector("g_singleColor", &rgb));

			V(m_pEffect->SetFloat("g_s", Scale(Param.iParm4, -100, 100)));

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_singlecolor", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
	}

	int mina(int a,int b)
	{
		if(a<b)return a;  return b;
	}
	
	//動態模糊  iLevel:多少張混合
	void Blur(const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp = 1.f;
		IDirect3DSurface9 *backBuffer = CommonStart(d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);
		

		IDirect3DTexture9	*pTex=NULL;
		if( m_quBlurTex.size() <MAX_BLUR_TEX)//new
		{
			D3DDISPLAYMODE dm; 
			V( d3ddev->GetDisplayMode(NULL,  &dm ) );
			V( d3ddev->CreateTexture(m_video_w, m_video_h, 1, D3DUSAGE_RENDERTARGET, dm.Format,  D3DPOOL_DEFAULT,	&pTex, NULL ) );
			
			m_quBlurTex.push_back(pTex);
		}
		else if( m_quBlurTex.size() >= MAX_BLUR_TEX)
		{
			pTex = m_quBlurTex[0];
			m_quBlurTex.pop_front();
			m_quBlurTex.push_back(pTex);
		}

		IDirect3DSurface9 *pBlurSurface=NULL;
		V( pTex->GetSurfaceLevel( 0, &pBlurSurface ) );
		V( D3DParam.d3ddev->StretchRect( D3DParam.pVideoStream, NULL, pBlurSurface, NULL, D3DTEXF_NONE ) ); 

		
		int nBlur = (int)Scale(Param.iLevel, 0, MAX_BLUR_TEX-1);

		if (m_pEffect)
		{
			ID3DXEffect *p = m_pEffect;

			int n = mina(nBlur, m_quBlurTex.size());
			V(m_pEffect->SetFloat("g_nBlur", n));

			for (int i = 0; i < n; i++)
			{
				char sz[200]; sprintf_s(sz, "g_texBlur%d", i);

				int st = m_quBlurTex.size() - i - 1;
				V(p->SetTexture(p->GetParameterByName(NULL, sz), m_quBlurTex[st]));
			}

			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, NULL, "tech_blur", szV, m_Identity);
		}
		//Restore
		CommonEnd(d3ddev, backBuffer);
		pBlurSurface->Release();
	}

	//熱輻射  iLevel:頻率   iParm1:旋轉角度   iParm2:影響範圍   iParm3:振幅  iParm4:垂直位移
	void Mirage(const sEffectD3DParam &D3DParam, struct sEffectParam Param)
	{
		IDirect3DDevice9 *d3ddev = D3DParam.d3ddev;
		float asp = 1.f;
		IDirect3DSurface9 *backBuffer = CommonStart(d3ddev, asp);
		StartRender(D3DParam);
		struct HP_VERTEX szV[4]; 	DefalutVertexData(asp, szV, 1.f);

		if (m_pEffect)
		{
			V(m_pEffect->SetFloat("g_video_w", (float)m_video_w));
			V(m_pEffect->SetFloat("g_video_h", (float)m_video_h));

			float speed = Scale(Param.iLevel, 1, 16);
			//static float s_time = 0.f;	s_time += (float)1.f / 30.f*speed;	V(m_pEffect->SetFloat("g_time", s_time));
			m_fTime += (float)1.f / 30.f*speed;	V(m_pEffect->SetFloat("g_time", m_fTime));

			V(m_pEffect->SetFloat("g_mirage_angle", Scale(Param.iParm1, 0, 360)));
			V(m_pEffect->SetFloat("g_mirage_range", Scale(Param.iParm2, 1.5f, 0.4f)));
			V(m_pEffect->SetFloat("g_mirage_height", Scale(Param.iParm3, 0.5f, 4.f)));
			V(m_pEffect->SetFloat("g_mirage_xOffset", Scale(Param.iParm4, -0.5f, 0.5f)));


			V(m_pEffect->SetFloat("g_videoAsp", asp));//還跟視窗比例有關


			ShaderDraw(d3ddev, D3DParam.pOffscreenTexture3D, m_pTexMirage, "tech_mirage", szV, m_Identity);
		}

		//Restore
		CommonEnd(d3ddev, backBuffer);
	}
	 

};


//extern class CMyEffect3D  C_MyEffect3D;
