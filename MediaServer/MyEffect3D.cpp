#include "stdafx.h"

#ifdef _DISABLE_PLANE_SCENE3D
#else
#include "PlaneScene3D.h"
#endif

#include "MyEffect3D.h"

#include <math.h>


//class CMyEffect3D		C_MyEffect3D;

#define PI 3.14159265f
////////////////////////////////////////////////////////////////////////////////////////////////////
static HRESULT hr;
#ifndef SQUARED
#define SQUARED(iValue) (iValue*iValue)
#endif

D3DVECTOR* VecSubtract(D3DVECTOR *pOut, const D3DVECTOR *pV1, const D3DVECTOR *pV2)
{
    pOut->x = pV1->x - pV2->x;
    pOut->y = pV1->y - pV2->y;
    pOut->z = pV1->z - pV2->z;
    return pOut;
}

D3DVECTOR* VecNormalize(D3DVECTOR *pOut, const D3DVECTOR *pV1)
{
    FLOAT norm_sq = SQUARED(pV1->x) + SQUARED(pV1->y) + SQUARED(pV1->z);

    if (norm_sq > FLT_MIN)
    {
        FLOAT f = sqrtf(norm_sq);
        pOut->x = pV1->x / f;
        pOut->y = pV1->y / f;
        pOut->z = pV1->z / f;
    }
    else
    {
        pOut->x = 0.0f;
        pOut->y = 0.0f;
        pOut->z = 0.0f;
    }
    return pOut;
}

D3DVECTOR* VecCross(D3DVECTOR *pOut, const D3DVECTOR *pV1, const D3DVECTOR *pV2)
{
    pOut->x = pV1->y * pV2->z - pV1->z * pV2->y;
    pOut->y = pV1->z * pV2->x - pV1->x * pV2->z;
    pOut->z = pV1->x * pV2->y - pV1->y * pV2->x;

    return pOut;
}

FLOAT VecDot(const D3DVECTOR *pV1, const D3DVECTOR *pV2)
{
    return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z;
}

D3DMATRIX* MatrixLookAtLH( 
    D3DMATRIX *pOut, 
    const D3DVECTOR *pEye, 
    const D3DVECTOR *pAt,
    const D3DVECTOR *pUp 
    )
{

    D3DVECTOR vecX, vecY, vecZ;

    // Compute direction of gaze. (+Z)

    VecSubtract(&vecZ, pAt, pEye);
    VecNormalize(&vecZ, &vecZ);

    // Compute orthogonal axes from cross product of gaze and pUp vector.
    VecCross(&vecX, pUp, &vecZ);
    VecNormalize(&vecX, &vecX);
    VecCross(&vecY, &vecZ, &vecX);

    // Set rotation and translate by pEye
    pOut->_11 = vecX.x;
    pOut->_21 = vecX.y;
    pOut->_31 = vecX.z;
    pOut->_41 = -VecDot(&vecX, pEye);

    pOut->_12 = vecY.x;
    pOut->_22 = vecY.y;
    pOut->_32 = vecY.z;
    pOut->_42 = -VecDot(&vecY, pEye);

    pOut->_13 = vecZ.x;
    pOut->_23 = vecZ.y;
    pOut->_33 = vecZ.z;
    pOut->_43 = -VecDot(&vecZ, pEye);

    pOut->_14 = 0.0f;
    pOut->_24 = 0.0f;
    pOut->_34 = 0.0f;
    pOut->_44 = 1.0f;

    return pOut;
}


// MatrixPerspectiveFovLH: Approximately equivalent to D3DXMatrixPerspectiveFovLH.

D3DMATRIX* MatrixPerspectiveFovLH(
    D3DMATRIX * pOut,
    FLOAT fovy,
    FLOAT Aspect,
    FLOAT zn,
    FLOAT zf
    )
{   
    // yScale = cot(fovy/2)

    FLOAT yScale = cosf(fovy * 0.5f) / sinf(fovy * 0.5f);
    FLOAT xScale = yScale / Aspect;

    ZeroMemory(pOut, sizeof(D3DMATRIX));

    pOut->_11 = xScale;

    pOut->_22 = yScale;

    pOut->_33 = zf / (zf - zn);
    pOut->_34 = 1.0f;

    pOut->_43 = -pOut->_33 * zn;

    return pOut;
}

void ChangeViewMatrix( IDirect3DDevice9* d3ddev )
{
	float x = float( 0 ) / 100 ;
	float y = float( 0 ) / 100 ;
	D3DVECTOR from = { x, y, -2.4f };
	D3DVECTOR at = { 0.0f, 0.0f, 0.0f };
	D3DVECTOR up = { 0.0f, 1.0f, 0.0f };

	D3DMATRIX matView;   MatrixLookAtLH(&matView, &from, &at, &up);
	V( d3ddev->SetTransform( D3DTS_VIEW, &matView ) );
}

void DefaultViewMatrix( IDirect3DDevice9* d3ddev )
{
	float x = float(0) / 100 ;
	float y = float(0) / 100 ;
	D3DVECTOR from = { x, y, -2.4f };
	D3DVECTOR at = { 0.0f, 0.0f, 0.0f };
	D3DVECTOR up = { 0.0f, 1.0f, 0.0f };

	D3DMATRIX matView;   MatrixLookAtLH(&matView, &from, &at, &up);
	V( d3ddev->SetTransform( D3DTS_VIEW, &matView ) );
}


void DefaultProjMatrix( IDirect3DDevice9* d3ddev,  float asp )
{
	D3DMATRIX matProj;
	MatrixPerspectiveFovLH( &matProj, (float)M_PI_4, asp, 1.0f, 100.0f );
	V( d3ddev->SetTransform( D3DTS_PROJECTION, &matProj ) );
}

D3DMATRIX* WINAPI D3DXMatrixRotationX( D3DMATRIX *pOut, float angle )
{
	angle = angle *(PI/180.f);
	float sin1=sin(angle), cos1=cos(angle);
	//sincosf(angle, &sin, &cos);  // Determine sin and cos of angle

	pOut->_11 = 1.0f; pOut->_12 =  0.0f;   pOut->_13 = 0.0f; pOut->_14 = 0.0f;
	pOut->_21 = 0.0f; pOut->_22 =  cos1;   pOut->_23 = sin1;  pOut->_24 = 0.0f;
	pOut->_31 = 0.0f; pOut->_32 = -sin1;   pOut->_33 = cos1;  pOut->_34 = 0.0f;
	pOut->_41 = 0.0f; pOut->_42 =  0.0f;   pOut->_43 = 0.0f; pOut->_44 = 1.0f;

	return pOut;
}

D3DMATRIX* WINAPI D3DXMatrixRotationY( D3DMATRIX *pOut, float angle )
{
	angle = angle *(PI/180.f);
	float sin1=sin(angle), cos1=cos(angle);
	//sincosf(angle, &sin, &cos);  // Determine sin and cos of angle

	pOut->_11 = cos1; pOut->_12 =  0.0f;   pOut->_13 =-sin1;  pOut->_14 = 0.0f;
	pOut->_21 = 0.0f; pOut->_22 =  1.f;    pOut->_23 = 0.0f;  pOut->_24 = 0.0f;
	pOut->_31 = sin1; pOut->_32 =  0;      pOut->_33 = cos1;  pOut->_34 = 0.0f;
	pOut->_41 = 0.0f; pOut->_42 =  0.0f;   pOut->_43 = 0.0f;  pOut->_44 = 1.0f;

	return pOut;
}


D3DMATRIX* WINAPI D3DXMatrixRotationZ( D3DMATRIX *pOut, float angle )
{
	angle = angle *(PI/180.f);
	float sin1=sin(angle), cos1=cos(angle);
	//sincosf(angle, &sin, &cos);  // Determine sin and cos of angle

	pOut->_11 = cos1; pOut->_12 = sin1;   pOut->_13 = 0.0f; pOut->_14 = 0.0f;
	pOut->_21 =-sin1; pOut->_22 = cos1;   pOut->_23 = 0.f;  pOut->_24 = 0.0f;
	pOut->_31 = 0.0f; pOut->_32 = 0.f;    pOut->_33 = 1.f;	pOut->_34 = 0.0f;
	pOut->_41 = 0.0f; pOut->_42 = 0.0f;   pOut->_43 = 0.0f; pOut->_44 = 1.0f;

	return pOut;
}


D3DMATRIX* WINAPI D3DXMatrixScale( D3DMATRIX *pOut, float x, float y, float z )
{

	pOut->_11 = x;   pOut->_12 = 0.f;   pOut->_13 = 0.f;  pOut->_14 = 0.f;
	pOut->_21 = 0.f; pOut->_22 = y;     pOut->_23 = 0.f;  pOut->_24 = 0.f;
	pOut->_31 = 0.f; pOut->_32 = 0.f;   pOut->_33 = z;	  pOut->_34 = 0.f;
	pOut->_41 = 0.f; pOut->_42 = 0.f;   pOut->_43 = 0.f;  pOut->_44 = 1.0f;

	return pOut;
}

D3DMATRIX* D3DXMatrixTranslation( D3DMATRIX *pOut, float x, float y, float  z )
{
	pOut->_11 = 1.0f;    pOut->_12 = 0.0f;    pOut->_13 = 0.0f;    pOut->_14 = 0.0f;
	pOut->_21 = 0.0f;    pOut->_22 = 1.0f;    pOut->_23 = 0.0f;    pOut->_24 = 0.0f;
	pOut->_31 = 0.0f;    pOut->_32 = 0.0f;    pOut->_33 = 1.0f;    pOut->_34 = 0.0f;
	pOut->_41 = x;       pOut->_42 = y;       pOut->_43 = z;       pOut->_44 = 1.0f;

	return pOut;
}

D3DMATRIX* WINAPI D3DXMatrixMultiply(D3DMATRIX *pout, CONST D3DMATRIX *pm1, CONST D3DMATRIX *pm2)
{
	int i,j;

	for (i=0; i<4; i++)
	{
		for (j=0; j<4; j++)
		{
			pout->m[i][j] = pm1->m[i][0] * pm2->m[0][j] + pm1->m[i][1] * pm2->m[1][j] + pm1->m[i][2] * pm2->m[2][j] + pm1->m[i][3] * pm2->m[3][j];
		}
	}
	return pout;
}
