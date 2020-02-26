//------------------------------------------------------------------------------
// File: PlaneScene.cpp
//
// Desc: DirectShow sample code - implementation of the CPlaneScene2D class
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "PlaneScene2D.h"

#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

float fWidthF = 72.4f;
float fWidthV = 70.0f;
float fHeightF = 42.0f;
float fHeightV = 39.6f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlaneScene2D::CPlaneScene2D()
{
    m_vertices[0].position = CUSTOMVERTEX::Position(-1.0f,  1.0f, 0.0f); // top left
    m_vertices[1].position = CUSTOMVERTEX::Position(-1.0f, -1.0f, 0.0f); // bottom left
    m_vertices[2].position = CUSTOMVERTEX::Position( 1.0f,  1.0f, 0.0f); // top right
    m_vertices[3].position = CUSTOMVERTEX::Position( 1.0f, -1.0f, 0.0f); // bottom right

    // set up diffusion:
    m_vertices[0].color = 0xffffffff; //³z©ú«×¡A¬õ¡Aºñ¡AÂÅ
    m_vertices[1].color = 0xffffffff;
    m_vertices[2].color = 0xffffffff;
    m_vertices[3].color = 0xffffffff;

    // set up texture coordinates
    m_vertices[0].tu = 0.0f; m_vertices[0].tv = 0.0f; // low left
    m_vertices[1].tu = 0.0f; m_vertices[1].tv = 1.0f; // high left
    m_vertices[2].tu = 1.0f; m_vertices[2].tv = 0.0f; // low right
    m_vertices[3].tu = 1.0f; m_vertices[3].tv = 1.0f; // high right
}

CPlaneScene2D::~CPlaneScene2D()
{

}

HRESULT 
CPlaneScene2D::Init(IDirect3DDevice9* d3ddev, IMediaSeeking *pMS, int iWidth, int iHeight)
{
    HRESULT hr;

    if( ! d3ddev )
        return E_POINTER;

	if (1)
	{
		FAIL_RET( d3ddev->SetRenderState( D3DRS_AMBIENT, 0x00202020 ) );
		FAIL_RET( d3ddev->SetRenderState( D3DRS_LIGHTING, FALSE ) );
		FAIL_RET( d3ddev->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE)); 
		FAIL_RET( d3ddev->SetRenderState( D3DRS_ZENABLE,TRUE)); 
	}
	else
	{
		FAIL_RET(hr = d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
		FAIL_RET(hr = d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE));
		FAIL_RET(hr = d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));
		FAIL_RET(hr = d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
		FAIL_RET(hr = d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));
		FAIL_RET(hr = d3ddev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE));
		FAIL_RET(hr = d3ddev->SetRenderState(D3DRS_ALPHAREF, 0x10));
		FAIL_RET(hr = d3ddev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER));

		FAIL_RET(hr = d3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
		FAIL_RET(hr = d3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));
		FAIL_RET(hr = d3ddev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
		FAIL_RET(hr = d3ddev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
		FAIL_RET(hr = d3ddev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR));
	}

    m_vertexBuffer = NULL;
	m_sizeSrc.cx = iWidth;
	m_sizeSrc.cy = iHeight;

	float fRatio = float(m_sizeSrc.cx)/m_sizeSrc.cy;
	m_vertices[0].position = CUSTOMVERTEX::Position(-fRatio, 1, 0);  // top left
	m_vertices[1].position = CUSTOMVERTEX::Position(-fRatio, -1, 0); // top left
	m_vertices[2].position = CUSTOMVERTEX::Position(fRatio, 1, 0);   // top left
	m_vertices[3].position = CUSTOMVERTEX::Position(fRatio, -1, 0);  // bottom right

    d3ddev->CreateVertexBuffer(sizeof(CUSTOMVERTEX)*4,D3DUSAGE_WRITEONLY,D3DFVF_CUSTOMVERTEX,D3DPOOL_MANAGED,& m_vertexBuffer.p, NULL );
	//d3ddev->CreateVertexBuffer(sizeof(m_vertices),D3DUSAGE_WRITEONLY,D3DFVF_CUSTOMVERTEX,D3DPOOL_MANAGED,& m_vertexBuffer.p, NULL ); jack

    CComPtr<IDirect3DSurface9> backBuffer;
    FAIL_RET( d3ddev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer.p));

    D3DSURFACE_DESC backBufferDesc;
    backBuffer->GetDesc( &backBufferDesc);

    // Set the projection matrix
    D3DXMATRIX matProj;
    FLOAT fAspect = backBufferDesc.Width / (float)backBufferDesc.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/5, fAspect, 1.0f, 5.0f);
    FAIL_RET( d3ddev->SetTransform( D3DTS_PROJECTION, &matProj));

    
    D3DXVECTOR3 from( 0.0f, 0.0f, -3.0f);
    D3DXVECTOR3 at( 0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f);

    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &from, &at, &up);
    FAIL_RET( d3ddev->SetTransform( D3DTS_VIEW, &matView));

	g_pMediaSeeking = pMS;
    m_time = GetTickCount();

    return hr;
}


HRESULT 
CPlaneScene2D::DrawScene( IDirect3DDevice9* d3ddev,
                        IDirect3DTexture9* texture, INT32 iID ) 
{
    HRESULT hr;

    if( !( d3ddev && texture ) )
    {
        return E_POINTER;
    }

    if( m_vertexBuffer == NULL )
    {
        return D3DERR_INVALIDCALL;
    }

    // get the difference in time
    DWORD dwCurrentTime;
    dwCurrentTime = GetTickCount();
    double difference = m_time - dwCurrentTime ;
    
    // figure out the rotation of the plane
    float x = (float) ( -cos(difference / 2000.0 ) ) ;
    float y = (float) ( cos(difference / 2000.0 ) ) ;
    float z = (float) ( sin(difference / 2000.0 ) ) ;

	float fcos = float(cos(difference/2000.0));
	float fsin = float(sin(difference/2000.0));

    // update the two rotating vertices with the new position
	float fPreX[4], fPreY[4], fPreZ[4];
	float fRatio = float(m_sizeSrc.cx)/m_sizeSrc.cy;
	float fOffsetX = 0, fOffsetY = 0;
	float fScaleRatio = g_PannelSetting.fRatio[m_iIndex];

	if (g_PannelSetting.iModelAction & MODEL_FULL_SCREEN)
	{
		fScaleRatio = 2.0;
		fOffsetX = float(0.5 - (m_iIndex%2) + 0.0*(1 - 2*(m_iIndex%2))*(fWidthF - fWidthV)/fWidthF)* fRatio;
		fOffsetY = float(-0.5 + int(m_iIndex/2) + 0.5*(2*int(m_iIndex/2) - 1)*(fHeightF - fHeightV)/fHeightF);
	}

	for (int x = 0; x < 4; x++)
	{		
		if (x == 0)
		{
			fPreX[x] = -fRatio + fOffsetX;
			fPreY[x] = 1 + fOffsetY;
			fPreZ[x] = 0;
		}
		else if (x == 1)
		{
			fPreX[x] = -fRatio + fOffsetX;
			fPreY[x] = -1 + fOffsetY;
			fPreZ[x] = 0;
		}
		else if (x == 2)
		{
			fPreX[x] = fRatio + fOffsetX;
			fPreY[x] = 1 + fOffsetY;
			fPreZ[x] = 0;
		}
		else if (x == 3)
		{
			fPreX[x] = fRatio + fOffsetX;
			fPreY[x] = -1 + fOffsetY;
			fPreZ[x] = 0;
		}

		fPreX[x] *= fScaleRatio;
		fPreY[x] *= fScaleRatio;
		fPreZ[x] *= fScaleRatio;
	}

	m_vertices[0].color = 0xffffffff;
	m_vertices[1].color = 0xffffffff;
	m_vertices[2].color = 0xffffffff;
	m_vertices[3].color = 0xffffffff;
	
	m_vertices[0].position = CUSTOMVERTEX::Position(fPreX[0], fPreY[0], 0);   // top left
	m_vertices[1].position = CUSTOMVERTEX::Position(fPreX[1], fPreY[1], 0);   // top left
	m_vertices[2].position = CUSTOMVERTEX::Position(fPreX[2], fPreY[2], 0);   // top left
	m_vertices[3].position = CUSTOMVERTEX::Position(fPreX[3], fPreY[3], 0);   // bottom right


	if (g_PannelSetting.iEffect[m_iIndex] == 1) //Rotate at Z Axis
	{
		for (int x = 0; x < 4; x++)
		{			
			m_vertices[x].position.x = fcos*fPreX[x] - fsin*fPreY[x];
			m_vertices[x].position.y = fcos*fPreY[x] + fsin*fPreX[x];
			m_vertices[x].position.z = fPreZ[x];
		}
	}
	else if (g_PannelSetting.iEffect[m_iIndex] == 2)//Rotate at X Axis
	{
		for (int x = 0; x < 4; x++)
		{
			m_vertices[x].position.x = fPreX[x];
			m_vertices[x].position.y = fcos*fPreY[x] + fsin*fPreZ[x];
			m_vertices[x].position.z = fcos*fPreZ[x] - fsin*fPreY[x];
		}
	}
	else if (g_PannelSetting.iEffect[m_iIndex] == 3)//Rotate at Y Axis
	{
		for (int x = 0; x < 4; x++)
		{
			m_vertices[x].position.x = fcos*fPreX[x] + fsin*fPreZ[x];
			m_vertices[x].position.y = fPreY[x];
			m_vertices[x].position.z = fcos*fPreZ[x] - fsin*fPreX[x];
		}
	}
	else if (g_PannelSetting.iEffect[m_iIndex] == 4)
	{
		
		DWORD mask0 = (DWORD) (255 * ( ( y + 1.0  )/ 2.0 ));
		DWORD mask3 = DWORD((LONGLONG)(difference) %255);

		m_vertices[0].color = 0x00ffffff | (mask0<<24);
		m_vertices[1].color = 0x00ffffff | (mask0<<24);
		m_vertices[2].color = 0x00ffffff | (mask0<<24);
		m_vertices[3].color = 0x00ffffff | (mask0<<24);
	}

	//m_vertices[0].position = CUSTOMVERTEX::Position(x,  y, z);   // top left
    //m_vertices[3].position = CUSTOMVERTEX::Position(-x, -y, -z); // bottom right

    // Adjust the color so the blue is always on the bottom.
    // As the corner approaches the bottom, get rid of all the other
    // colors besides blue

    // write the new vertex information into the buffer
    void* pData;
    //FAIL_RET( m_vertexBuffer->Lock(0,0, &pData,0) ); jack
    //memcpy(pData,m_vertices,sizeof(m_vertices)); jack
	FAIL_RET( m_vertexBuffer->Lock(0, 0, &pData,0) );
    memcpy(pData,m_vertices,sizeof(CUSTOMVERTEX)*4);
    FAIL_RET( m_vertexBuffer->Unlock() );  

    // clear the scene so we don't have any articats left
    d3ddev->Clear( 0L, NULL, D3DCLEAR_TARGET, 
                   D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

    FAIL_RET( d3ddev->BeginScene() );
    FAIL_RET( d3ddev->SetTexture( 0, texture));

    FAIL_RET(hr = d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE));
    FAIL_RET(hr = d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE));
    FAIL_RET(hr = d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE));
    FAIL_RET(hr = d3ddev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE));

    FAIL_RET( d3ddev->SetStreamSource(0, m_vertexBuffer.p, 0, sizeof(CPlaneScene2D::CUSTOMVERTEX)  ) );            //set next source ( NEW )
    FAIL_RET( d3ddev->SetFVF( D3DFVF_CUSTOMVERTEX ) );
    FAIL_RET( d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2) );  //draw quad 
    FAIL_RET( d3ddev->SetTexture( 0, NULL));
    FAIL_RET( d3ddev->EndScene());

    return hr;
}

void
CPlaneScene2D::SetSrcRect( float fTU, float fTV )
{
    m_vertices[0].tu = 0.0f; m_vertices[0].tv = 0.0f; // low left
    m_vertices[1].tu = 0.0f; m_vertices[1].tv = fTV;  // high left
    m_vertices[2].tu = fTU;  m_vertices[2].tv = 0.0f; // low right
    m_vertices[3].tu = fTU;  m_vertices[3].tv = fTV;  // high right
}

void CPlaneScene2D::OnMouseMove(UINT nFlags, CPoint point)
{
}