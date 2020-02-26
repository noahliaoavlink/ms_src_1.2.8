#pragma once

#ifndef _D3DSprite_H_
#define _D3DSprite_H_

//#include <d3d9.h>
#include <d3dx9.h>
#include <D3dx9core.h>


class D3DSprite
{
	LPD3DXSPRITE m_pSprite;
	D3DXMATRIX m_ScaleMatrix;
	D3DXMATRIX m_RotationMatrix;
protected:
public:
	D3DSprite();
	~D3DSprite();

	int Create(LPDIRECT3DDEVICE9 pDevice);
	void Draw(IDirect3DTexture9* pTexture, int x = 0, int y = 0, int iAlpha = 255);
	void Scale(float fH = 1.0, float fV = 1.0);
	void Rotation(float fX = 0.0, float fY = 0.0, float fZ = 0.0);
	
};

#endif