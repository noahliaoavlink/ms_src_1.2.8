//------------------------------------------------------------------------------
// File: PlaneScene.h
//
// Desc: DirectShow sample code - interface for the CPlaneScene2D class
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#if !defined(AFX_PLANESCENE_H__B0ED1D62_D626_479A_925D_7488767DF129__INCLUDED_)
#define AFX_PLANESCENE_H__B0ED1D62_D626_479A_925D_7488767DF129__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3dx9.h>
#include <strmif.h>

#define FAIL_RET(x) do { if( FAILED( hr = ( x  ) ) ) \
    return hr; } while(0)


typedef enum
{
	STATUS_NO_INITIAL = 0,  //尚未Initial
	STATUS_NO_PREVIEW,      //已Initial，但並未Preview
	STATUS_PREVIEWING,      //已Initial，並正在Preview中
	STATUS_LOADING,			//已Loading中
	STATUS_BUSYING			//已Initial，Busy中

}CLASS_STATUS;

class CPlaneScene2D  
{
public:

    CPlaneScene2D();
    virtual ~CPlaneScene2D();

    HRESULT Init(IDirect3DDevice9* d3ddev, IMediaSeeking *pMS, int iWidth, int iHeight);

    HRESULT DrawScene(  IDirect3DDevice9* d3ddev,
                        IDirect3DTexture9* texture, INT32 iID = -1 );

    void SetSrcRect( float fTU, float fTV );
	void SetIndex(INT32 iIndex){m_iIndex = iIndex;}
	void OnMouseMove(UINT nFlags, CPoint point);

private:
	INT32 m_iIndex;

    struct CUSTOMVERTEX
    {
        struct Position {
            Position() : 
                x(0.0f),y(0.0f),z(0.0f) {            
            };
            Position(float x_, float y_, float z_) :
                x(x_),y(y_),z(z_) {
            };
            float x,y,z;
        };

        Position    position; // The position
        D3DCOLOR    color;    // The color
        FLOAT       tu, tv;   // The texture coordinates
    };

	CSize                           m_sizeSrc;
    CUSTOMVERTEX                    m_vertices[4];
    CComPtr<IDirect3DVertexBuffer9> m_vertexBuffer;

    DWORD  m_time;
	IMediaSeeking	*g_pMediaSeeking;

};

#endif // !defined(AFX_PLANESCENE_H__B0ED1D62_D626_479A_925D_7488767DF129__INCLUDED_)
