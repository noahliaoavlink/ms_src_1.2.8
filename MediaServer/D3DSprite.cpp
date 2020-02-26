#include "stdafx.h"
#include "D3DSprite.h"

#pragma comment(lib, "d3dx9.lib")
//#pragma comment(lib, "d3d9.lib")

D3DSprite::D3DSprite()
{
	Rotation();
}

D3DSprite::~D3DSprite()
{
	if (m_pSprite != NULL)
	{
		m_pSprite->Release();
		m_pSprite = 0;
	}
}

int D3DSprite::Create(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(D3DXCreateSprite(pDevice, &m_pSprite)))
		return E_FAIL;
}

void D3DSprite::Draw(IDirect3DTexture9* pTexture,int x, int y, int iAlpha)
{
	D3DXVECTOR3 vPosition((float)x, (float)y, 0.0f);
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	//D3DXMATRIX tran = m_ScaleMatrix * m_RotationMatrix;
	//m_pSprite->SetTransform(&tran);
	m_pSprite->SetTransform(&m_ScaleMatrix);
	m_pSprite->Draw(pTexture, NULL, NULL, &vPosition, D3DCOLOR_ARGB(iAlpha,255, 255, 255));
	m_pSprite->End();
}

void D3DSprite::Scale(float fH,float fV)
{
	D3DXMatrixScaling(&m_ScaleMatrix, fH, fV, 1.f);
}

void D3DSprite::Rotation(float fX,float fY,float fZ)
{
	//m_RotationMatrix
	D3DXMATRIX X;
	D3DXMatrixRotationZ(&X, fX);

	D3DXMATRIX Y;
	D3DXMatrixRotationZ(&Y, fY);

	D3DXMATRIX Z;
	D3DXMatrixRotationZ(&Z, fZ);

	m_RotationMatrix = X * Y * Z;
}