//------------------------------------------------------------------------------
// File: PlaneScene.h
//
// Desc: DirectShow sample code - interface for the CPlaneScene3D class
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


//#if !defined(AFX_PLANESCENE_H__B0ED1D62_D626_479A_925D_7488767DF129__INCLUDED_)
//#define AFX_PLANESCENE_H__B0ED1D62_D626_479A_925D_7488767DF129__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3dx9.h>
#include <strmif.h>
#include <vmr9.h>

#define FAIL_RET(x) do { if( FAILED( hr = ( x  ) ) ) \
    return hr; } while(0)

//const int NUM_VERTICES_3D = 50000;//500000
const int NUM_VERTICES_3D = 150000;//500000
const int MAX_Model = 40;//12
const int AXIS_VETNUM = 708;//354
const int MAP_WIDTH = 1920;
const int MAP_HEIGHT = 1080;

#define _MAX_MAPPING_POINTS 4 


#include "..\\lib\\HASP\\hasp_api.h"
#ifdef PlaneScene_EXPORTS
#define PlaneScene_API __declspec(dllexport)
#else
#define PlaneScene_API __declspec(dllimport)
#ifdef _DEBUG

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment(lib,"..\\x64\\Debug\\3DWorld.lib")
#else
#pragma comment(lib,"..\\x64\\Release\\3DWorld.lib")
#endif

#else
#pragma comment(lib,"..\\Debug\\3DWorld.lib")
#endif

#else

#ifdef _WIN64
#pragma comment(lib,"..\\x64\\Release\\3DWorld.lib")
#else
#pragma comment(lib,"..\\Release\\3DWorld.lib")
#endif

#endif	// _DEBUG

#endif

typedef enum
{
	STATUS_NO_INITIAL = 0,  //尚未Initial
	STATUS_NO_PREVIEW,      //已Initial，但並未Preview
	STATUS_PREVIEWING,      //已Initial，並正在Preview中
	STATUS_LOADING,			//已Loading中
	STATUS_BUSYING			//已Initial，Busy中

}CLASS_STATUS;


#ifdef _DISABLE_PLANE_SCENE_3D
#else

#ifdef _WIN64
	class PlaneScene_API CPlaneScene3D
#else
	class CPlaneScene3D
#endif
{
public:

    CPlaneScene3D();
    virtual ~CPlaneScene3D();

    HRESULT Init(IDirect3DDevice9* d3ddev, CWnd *pParentWnd, CString strCurPath, int iWidth, int iHeight,
		         CString strObjPath = "", DOUBLE dZoomRatio = 1.0, INT32 iPannelIdx = 0);
	void LoadObj(CString strFilePath, DOUBLE dZoomRatio = 1.0, INT32 iPannelIdx = 0);

    HRESULT DrawScene(IDirect3DDevice9* d3ddev, IDirect3DTexture9* texture, INT32 iSelIdx
		              , UINT uModelAction, INT32 iID = -1);

	HRESULT DrawMultiScene(IDirect3DDevice9* d3ddev, VMR9VideoStreamInfo* pVideoStreamInfo, UINT cStreams);

    void SetSrcRect( float fTU, float fTV );
	void SetIndex(INT32 iIndex){m_iIndex = iIndex;}
	void RotateModel(INT iModelNum, double fXAngle, double fYAngle, double fZAngle, 
		             BOOL bAxisOnly = FALSE);
	void ZoomModel(INT iModelNum, float fRatioX, float fRatioY, float fRatioZ, BOOL bAxisOnly = FALSE);
	void ZoomModel(INT iModelNum, float fRatio, BOOL bAxisOnly = FALSE);
	void OffsetModel(INT iModelNum, float fOffsetX, float fOffsetY, float fOffsetZ,
		             BOOL bAxisOnly = FALSE, BOOL bNeedLock = FALSE, BOOL bOffsetSource = FALSE);
	void LoadObjFiles(IDirect3DDevice9* d3ddev);
	void OnMouseMovePre(UINT nFlags, CPoint point, UINT nEventIdx, UINT uModelAction);
	UINT OnMouseMove(UINT nFlags, CPoint point, UINT nEventIdx, UINT uModelAction);
	BOOL SelectAreaPt(CRect rectArea);
	INT32 PTtoModal(CPoint point, INT32 &iModalIdx);
	UINT GetCurStatus() {return m_iCurStatus;};
	INT32 GetObjCount() { return m_ModelNumAry.GetCount();};
	void SetObjIdx(INT32 iSelObjIdx) { m_iSelObjIdx = iSelObjIdx;};
	void DeleteObject(INT32 iDelObjIdx);
	void CleanSelectPt();
	BOOL AddSelectPt(CPoint ptSelect) 
	{ 
		BOOL bNotify = FALSE;
		m_ptSelView.Add(ptSelect);
		if (m_ptSelView.GetCount() == _MAX_MAPPING_POINTS) //3
		{
			bNotify = PtToVector();
		}

		return bNotify;
	};

	void SetAxisOffset(INT32 iModelIdx, float fAxisOffset[3][3])
	{
		for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			m_fAxisOffset[x][y][iModelIdx] = fAxisOffset[x][y];
	}

	void ResetFromSetting(INT32 iObjIdx = -1);
	static CString FileEncoder(CString strFilePath, BOOL bModifyAllFile = TRUE, BOOL bReName = FALSE);
	static CString FileDecoder(CString strSrcPath, CString strDestPath = "",
		                    BOOL bModifyAllFile = TRUE, BOOL bReName = FALSE);
	void LoadPtOffset(CString strProjectPath);
	void SaveModifyPt(CString strSrcPath, CString strDestPath);
	static INT32 GetMaxViewNum();
	INT32 UndoAction(INT32 iSetIdx = -1);
	INT32 RedoAction(INT32 iSetIdx = -1);
	void SaveObjectSetting(CString strProjectPath = "Shape.xml");
	void SelectPannel(BOOL bSelected);

	void IncreaseDrawSceneCount();
	void DecreaseDrawSceneCount();
	int GetDrawSceneCount();

private:
	INT32 m_iIndex;
	INT32 m_iCurStatus;
	INT32 m_iSelObjIdx;
	CString m_strCurPath;
	INT32 m_iCurActIdx;
	BOOL  m_bSelected;

	struct Action3D
	{
		INT32 iIndex;
		CString strObjPath;
		CString strSahpePath;
	};

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
		D3DXVECTOR3 n;        // Referenced as v3 in the vertex shader
        D3DCOLOR    color;    // The color
        FLOAT       tu, tv;   // The texture coordinates
    };

	CSize                           m_sizeSrc;
	CSize                           m_sizeView;
	CArray<INT32, INT32>			m_ModelNumAry;
	CArray<INT32, INT32>			m_ModelLineNumAry;
	CArray<INT32, INT32>            m_PannelMaping;
	CArray<Action3D, Action3D>      m_ActionArray;
    CUSTOMVERTEX                    m_vertices[MAX_Model][NUM_VERTICES_3D];
	CUSTOMVERTEX                    m_TempVerx[NUM_VERTICES_3D];
	CUSTOMVERTEX                    m_Linevertices[MAX_Model][NUM_VERTICES_3D];
	CUSTOMVERTEX                    m_versGround[100*4];
	CUSTOMVERTEX                    m_versAxis[MAX_Model][2000];
	float							m_ModelAreaAry[MAX_Model][2][3];
	float							m_SrcAreaAry[MAX_Model][2][3];
	float							m_fAxisOffset[3][3][MAX_Model];
	double							m_ModelAngle[MAX_Model][3];
	double                          m_ObjSrcOffset[MAX_Model][3];
	D3DXMATRIX						m_ModelMatrix[MAX_Model];
    CComPtr<IDirect3DVertexBuffer9> m_vertexBuffer[MAX_Model];
	CComPtr<IDirect3DVertexBuffer9> m_verAxisBuffer[MAX_Model];
	CComPtr<IDirect3DVertexBuffer9> m_verLineBuffer[MAX_Model];
	IDirect3DVertexBuffer9* m_vxGround;
	IDirect3DTexture9* m_texGround;
	IDirect3DTexture9* m_texRed;
	IDirect3DTexture9* m_texGreen;
	IDirect3DTexture9* m_texBlue;
	IDirect3DTexture9* m_texRGB;
	IDirect3DTexture9* m_texYellow;
	IDirect3DTexture9* m_texWhite;
	IDirect3DTexture9* m_texGray;
	IDirect3DDevice9* m_D3DDev;

	ID3DXMesh* m_meshBox;
	D3DXMATRIX m_matProj;
	D3DXMATRIX m_matView;
	INT32 m_iObjMapAry[MAP_WIDTH][MAP_HEIGHT][MAX_Model];
	CPoint m_iObjPt2DMapAry[NUM_VERTICES_3D][MAX_Model];
	float m_fViewAngle;
	
	DWORD  m_time;
	CWnd *m_pParentWnd;
	CPoint m_ptPrePos;
	CPoint m_ptSelPtOffset;
	INT32 m_iSelIdx;
	D3DXVECTOR4 m_d3dvtOffset;

	int m_iDrawSceneCount;
	bool m_bIsFinished;

	int LoadObjFile(IDirect3DDevice9* d3ddev, CString strObjPath, INT32 iModelIdx, 
		             IDirect3DVertexBuffer9** ppVertexBuffer, CUSTOMVERTEX* pVerTexAry, DOUBLE dZoomRatio = 1.0f);
	void ObjMapping(INT32 iObjIdx, BOOL bSaveBMP = FALSE);
	BOOL PtToVector();
	BOOL PtToVectorNew();
	void ResetModel(INT32 iModelNum);
	void LoadObjFromSetting(INT32 iObjIdx);
	void NotifyOtherPannel(INT32 iObjIdx, D3DXMATRIX d3dMatrix, D3DXVECTOR3 verOffset);
	void WriteProfileFloat(CString strApp, CString strKey, float fValue, CString strIniPath);
	float GetPrivateProfileFloat(CString strApp, CString strKey, CString strIniPath);
	void LoadObjectSetting(INT32 iObjIdx, CString strProjectPath);
	void SaveObjectPtOffset(CString strProjectPath = "");
	void SortSelAry();
	void SortSelAry2();

	CArray<D3DXVECTOR4, D3DXVECTOR4> m_ptSelModel;
	CArray<CPoint, CPoint> m_ptSelView;
	CArray<INT32, INT32> m_iSelLineAry;
	CArray<INT32, INT32> m_iSelModelAry;

};

#endif

//#endif // !defined(AFX_PLANESCENE_H__B0ED1D62_D626_479A_925D_7488767DF129__INCLUDED_)
