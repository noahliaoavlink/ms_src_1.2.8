#include "stdafx.h"
#include "../Include/OutVideoDll.h"

OutVideoDll::OutVideoDll()
{
	m_hInst = 0;
//	m_pOVDObj = 0;

	for(int i = 0;i < _MAX_VIDEO_OBJ;i++)
		m_pOVDObjs[i] = 0;
}

OutVideoDll::~OutVideoDll()
{
	//if(m_pOVDObj)
		//_Out_OVD_Destroy(m_pOVDObj);
	for(int i = 0;i < _MAX_VIDEO_OBJ;i++)
	{
		if(m_pOVDObjs[i])
			_Out_OVD_Destroy(m_pOVDObjs[i]);
	}

	FreeLib();
}

bool OutVideoDll::LoadLib()
{
	char szFileName[] = "OutVideo.dll";
	m_hInst = LoadLibraryA(szFileName);
	
	if(m_hInst)
	{
		_Out_OVD_Create = (_Out_OVD_Create_Proc)GetProcAddress(m_hInst, "_Out_OVD_Create");
		_Out_OVD_Destroy = (_Out_OVD_Destroy_Proc)GetProcAddress(m_hInst, "_Out_OVD_Destroy");
		_Out_OVD_Init = (_Out_OVD_Init_Proc)GetProcAddress(m_hInst, "_Out_OVD_Init");
		_Out_OVD_RenderFrame = (_Out_OVD_RenderFrame_Proc)GetProcAddress(m_hInst, "_Out_OVD_RenderFrame");
		_Out_OVD_RenderFrame1 = (_Out_OVD_RenderFrame1_Proc)GetProcAddress(m_hInst, "_Out_OVD_RenderFrame1");
		_Out_OVD_RenderFrame2 = (_Out_OVD_RenderFrame2_Proc)GetProcAddress(m_hInst, "_Out_OVD_RenderFrame2");
		_Out_OVD_RenderFrame3 = (_Out_OVD_RenderFrame3_Proc)GetProcAddress(m_hInst, "_Out_OVD_RenderFrame3");
		_Out_OVD_Flip = (_Out_OVD_Flip_Proc)GetProcAddress(m_hInst, "_Out_OVD_Flip");
		_Out_OVD_UpdateOSD = (_Out_OVD_UpdateOSD_Proc)GetProcAddress(m_hInst, "_Out_OVD_UpdateOSD");
		_Out_OVD_GetD3DDevice = (_Out_OVD_GetD3DDevice_Proc)GetProcAddress(m_hInst, "_Out_OVD_GetD3DDevice");
		_Out_OVD_Clean = (_Out_OVD_Clean_Proc)GetProcAddress(m_hInst, "_Out_OVD_Clean");

		_Out_OVD_Target2RawData = (_Out_OVD_Target2RawData_Proc)GetProcAddress(m_hInst, "_Out_OVD_Target2RawData");
		_Out_OVD_GetBackbufWidth = (_Out_OVD_GetBackbufWidth_Proc)GetProcAddress(m_hInst, "_Out_OVD_GetBackbufWidth");
		_Out_OVD_GetBackbufHeight = (_Out_OVD_GetBackbufHeight_Proc)GetProcAddress(m_hInst, "_Out_OVD_GetBackbufHeight");

		_Out_OVD_EnableOAR = (_Out_OVD_EnableOAR_Proc)GetProcAddress(m_hInst, "_Out_OVD_EnableOAR");
		_Out_OVD_SetOAROffset = (_Out_OVD_SetOAROffset_Proc)GetProcAddress(m_hInst, "_Out_OVD_SetOAROffset");

		_Out_OVD_SetOSDPos = (_Out_OVD_SetOSDPos_Proc)GetProcAddress(m_hInst, "_Out_OVD_SetOSDPos");
		_Out_OVD_SetOSDColor = (_Out_OVD_SetOSDColor_Proc)GetProcAddress(m_hInst, "_Out_OVD_SetOSDColor");

//		m_pOVDObj = _Out_OVD_Create();
		return true;
	}
	return false;
}

void OutVideoDll::FreeLib()
{
	if(m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = 0;
	}
}

void OutVideoDll::Init(int iIndex,HWND hWnd)
{
	if(_MAX_VIDEO_OBJ > iIndex)
	{
		if(m_pOVDObjs[iIndex] == 0)
			m_pOVDObjs[iIndex] = _Out_OVD_Create();

		if(m_pOVDObjs[iIndex])
			_Out_OVD_Init(m_pOVDObjs[iIndex],hWnd);
	}

//	if(m_pOVDObj)
//		_Out_OVD_Init(m_pOVDObj,hWnd);
}

int OutVideoDll::RenderFrame(int iIndex,unsigned char* pBuffer,int iWidth,int iHeight)
{
	//if(m_pOVDObj)
		//return _Out_OVD_RenderFrame(m_pOVDObj,pBuffer,iWidth,iHeight);
	if(m_pOVDObjs[iIndex])
		return _Out_OVD_RenderFrame(m_pOVDObjs[iIndex],pBuffer,iWidth,iHeight);
	return -1;
}

int OutVideoDll::RenderFrame1(int iIndex,void* pSurface,int iWidth,int iHeight)
{
	//if(m_pOVDObj)
		//return _Out_OVD_RenderFrame1(m_pOVDObj,pSurface,iWidth,iHeight);
	if(m_pOVDObjs[iIndex])
		return _Out_OVD_RenderFrame1(m_pOVDObjs[iIndex],pSurface,iWidth,iHeight);
	return -1;
}

int OutVideoDll::RenderFrame2(int iIndex,int iPixelFormat,unsigned char* pBuffer,int iWidth,int iHeight)
{
	if(m_pOVDObjs[iIndex])
		return _Out_OVD_RenderFrame2(m_pOVDObjs[iIndex],iPixelFormat,pBuffer,iWidth,iHeight);
	return -1;
}

int OutVideoDll::RenderFrame3(int iIndex,int iPixelFormat,unsigned char* pBuffer,int iWidth,int iHeight,RECT rSrc)
{
	if(m_pOVDObjs[iIndex])
		return _Out_OVD_RenderFrame3(m_pOVDObjs[iIndex],iPixelFormat,pBuffer,iWidth,iHeight,rSrc);
	return -1;
}

void OutVideoDll::Flip(int iIndex)
{
	if(m_pOVDObjs[iIndex])
		_Out_OVD_Flip(m_pOVDObjs[iIndex]);
}

void OutVideoDll::UpdateOSD(int iIndex,int iType,wchar_t* wszData)
{
	if(m_pOVDObjs[iIndex])
		_Out_OVD_UpdateOSD(m_pOVDObjs[iIndex],iType,wszData);
}

void* OutVideoDll::GetD3DDevice(int iIndex)
{
	if(m_pOVDObjs[iIndex])
		return _Out_OVD_GetD3DDevice(m_pOVDObjs[iIndex]);
	return 0;
}

void OutVideoDll::Clean(int iIndex)
{
	if(m_pOVDObjs[iIndex])
		_Out_OVD_Clean(m_pOVDObjs[iIndex]);
}

void OutVideoDll::Target2RawData(int iIndex,int iWidth,int iHeight,unsigned char* pOutBuffer)
{
	if(m_pOVDObjs[iIndex])
		_Out_OVD_Target2RawData(m_pOVDObjs[iIndex],iWidth,iHeight,pOutBuffer);
}

int OutVideoDll::GetBackbufWidth(int iIndex)
{
	if(m_pOVDObjs[iIndex])
		return _Out_OVD_GetBackbufWidth(m_pOVDObjs[iIndex]);
	return 0;
}

int OutVideoDll::GetBackbufHeight(int iIndex)
{
	if(m_pOVDObjs[iIndex])
		return _Out_OVD_GetBackbufHeight(m_pOVDObjs[iIndex]);
	return 0;
}

void OutVideoDll::EnableOAR(int iIndex,bool bEnable)
{
	if (m_pOVDObjs[iIndex])
		_Out_OVD_EnableOAR(m_pOVDObjs[iIndex], bEnable);
}

void OutVideoDll::SetOAROffset(int iIndex, float fValue)
{
	if (m_pOVDObjs[iIndex])
		_Out_OVD_SetOAROffset(m_pOVDObjs[iIndex],fValue);
}

void OutVideoDll::SetOSDPos(int iIndex, int x, int y)
{
	if (m_pOVDObjs[iIndex])
		_Out_OVD_SetOSDPos(m_pOVDObjs[iIndex],x,y);
}

void OutVideoDll::SetOSDColor(int iIndex, unsigned char r, unsigned char g, unsigned char b)
{
	if (m_pOVDObjs[iIndex])
		_Out_OVD_SetOSDColor(m_pOVDObjs[iIndex],r,g,b);
}