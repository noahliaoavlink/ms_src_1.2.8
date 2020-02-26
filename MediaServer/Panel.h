#ifndef PANNEL_H_INCLUDED
#define PANNEL_H_INCLUDED

#pragma once
#include "Allocator.h"
#include "ShapeUIDlg.h"

#define _ENABLE_CWND 1
			 
extern vector<CComPtr<IBaseFilter>>   g_BaseFilterAry;
extern vector<CComPtr<IGraphBuilder>> g_GraphBuildAry;
extern vector<CComPtr<IMediaControl>> g_MediaControlAry;
extern vector<CComPtr<IMediaSeeking>> g_MediaSeekingAry;
extern vector<CComPtr<IVMRMixerControl9>> g_MixCtrlAry;

#define MAX_D3DNUM 10
// CPanel	  

#ifdef _ENABLE_CWND
class CPanel : public CWnd
#else
class CPanel : public CDialog
#endif
{
	DECLARE_DYNAMIC(CPanel)

public:
	CPanel(CWnd* pParent = NULL);	// 標準建構函式
	virtual ~CPanel();
	void SetIndex(INT32 iIndex)
	{
		m_iIndex = iIndex;

		HRESULT     hr = S_OK;
		CRect rectCur;
		GetClientRect(&rectCur);
		m_rectClient = rectCur;
		m_pMultiAllocator = new CAllocator(hr, m_hWnd, m_iIndex, rectCur, NULL, NULL, NULL, this);

		CClientDC cdc(this);
		m_MemoryDC = new CDC();
		m_MemoryDC->CreateCompatibleDC(&cdc);
		m_Bitmap.CreateCompatibleBitmap(&cdc, m_rectClient.Width() + 1, m_rectClient.Height() + 1);
		m_MemoryDC->SelectObject(&m_Bitmap);
	}
	void ShapeUI(BOOL bShapeUI) { m_bShapeUI = bShapeUI;}
	WCHAR* AsWideString(const char* str);
	HRESULT ShowVideo(CStringArray &strVideoAry);
	HRESULT SetAllocatorPresenter( CComPtr<IBaseFilter> filter, HWND window, IMediaSeeking *pMS, int iIndex);
	HRESULT CloseGraph(HWND window);
	HRESULT RunVideo(BOOL bRun);
	HRESULT RenderFileToVMR9(IGraphBuilder *pGB, WCHAR *wFileName, IBaseFilter *pRenderer, BOOL bRenderAudio=TRUE);
	HRESULT GetUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
	HRESULT FindPinOnFilter( IBaseFilter * pFilter, PIN_DIRECTION PinDir, DWORD dwPin, BOOL fConnected, IPin ** ppPin );
	HRESULT ShowVideoAry(CStringArray &strVideoAry);
	HRESULT ShowTempVideo(CStringArray &strVideoAry, CString strObjPath = "", DOUBLE dZoomRatio = 1.0f, BOOL bExtraVideo = FALSE, bool bCheckStatus = false);
	HRESULT ShowTempVideo(CString strObjPath, CString strVideoPath = "", DOUBLE dZoomRatio = 1.0f, BOOL bExtraVideo = FALSE, bool bCheckStatus = false);
	HRESULT GetNumberConnectedPins(DWORD& number);
	HRESULT AddBaseFilter(IGraphBuilder *pGraphBuild, IBaseFilter* pBaseFilter, HWND window, int iIndex);
	void SetSelectPt(CPoint ptSel, BOOL bNeedAdd)
	{
		if (m_pMultiAllocator && m_pMultiAllocator->HadInited())
			m_pMultiAllocator->SetSelectPt(ptSel, bNeedAdd);
	};
	void LoadPtOffset(CString strProjectPath = "")
	{
		if (m_pMultiAllocator && m_pMultiAllocator->HadInited())
			m_pMultiAllocator->LoadPtOffset(strProjectPath);
	};
	void ResetFromSetting()
	{
		if (m_pMultiAllocator && m_pMultiAllocator->HadInited())
			m_pMultiAllocator->ResetFromSetting();
	};
	void CleanModifyPt()
	{
		if (m_pMultiAllocator && m_pMultiAllocator->HadInited())
			m_pMultiAllocator->CleanModifyPt();
	};
	void SaveModifyPt(CString strSrcPath = "", CString strDestPath = "")
	{
		if (m_pMultiAllocator && m_pMultiAllocator->HadInited())
			m_pMultiAllocator->SaveModifyPt(strSrcPath, strDestPath);
	};
	INT32 GetObjCount() 
	{ 
		if (m_pMultiAllocator)
			return m_pMultiAllocator->GetObjCount();
		else
			return 0;
	};
	void SetObjIdx(INT32 iSelObjIdx)
	{
		if (m_pMultiAllocator)
			return m_pMultiAllocator->SetObjIdx(iSelObjIdx);
	};

	void Init2DMapping(INT32 iStatus)
	{
		if (m_pMultiAllocator)
			return m_pMultiAllocator->Init2DMapping(iStatus);
	};

	void Set2DColor(DOUBLE *pColAry, UINT iAreaIdx, CPoint ptDark)
	{
		if (m_pMultiAllocator)
			return m_pMultiAllocator->Set2DColor(pColAry, iAreaIdx, ptDark);
	}

	void PresetPanelGain(UINT uData) 
	{ 
		if (m_pMultiAllocator)
			return m_pMultiAllocator->PresetPanelGain(uData);
	};

	void Set2DFunction(INT32 iFunIdx, UINT uData)
	{
		if (m_pMultiAllocator)
			return m_pMultiAllocator->Set2DFunction(iFunIdx, uData);
	}

	void Set2DMapAction(INT32 i2DMapAction, UINT32 iVale = 0)
	{
		if (m_pMultiAllocator)
			return m_pMultiAllocator->Set2DMapAction(i2DMapAction, iVale);
	}

	void Create2DMapping(INT32 iGridX, INT32 iGridY)
	{
		if (m_pMultiAllocator)
			m_pMultiAllocator->Create2DMapping(iGridX, iGridY);
	}

	void SetBlendSetting(BLEND_SETTING bsBlendSetting, BOOL bFinalSetting = FALSE);

	void FreezeDisplay(BOOL bFreeze)
	{
		if (m_pMultiAllocator)
			m_pMultiAllocator->FreezeDisplay(bFreeze);
	}

	void DeleteObject(INT32 iDelObjIdx)
	{
		if (m_pMultiAllocator)
			return m_pMultiAllocator->DeleteObject(iDelObjIdx);
	}

	INT32 UndoAction(INT32 iSetIdx = -1)
	{
		if (m_pMultiAllocator)
			return m_pMultiAllocator->UndoAction(iSetIdx);
	}

	INT32 RedoAction(INT32 iSetIdx = -1)
	{
		if (m_pMultiAllocator)
			return m_pMultiAllocator->RedoAction(iSetIdx);
	}

	void LoadPorejct(CString strProjectPath = "")
	{
		if (!m_bShapeUI)
			return;

#if 0
		m_pShapeDlg->LoadProject(strProjectPath);
#else
		extern void GetExecutionPath(HMODULE hModule, char* szPath);

		char szAPPath[512];
		char szIniFileName[512];
		GetExecutionPath(NULL, szAPPath);
		sprintf(szIniFileName,"%s\\Setting.ini", szAPPath);

		bool bEnableMappingPf = GetPrivateProfileInt("Mapping_Setting", "EnablePreloadProjectFile", 0, szIniFileName);
		if (bEnableMappingPf)
		{
			char szProjectFileName[512];
			GetPrivateProfileString("Mapping_Setting", "ProjectFile", "", szProjectFileName, 1024, szIniFileName);

#ifdef _ENABLE_SPF
			m_pShapeDlg->UnZipProjectFile(szProjectFileName);
#endif

			m_pShapeDlg->LoadProject(strProjectPath);
		}
#endif
	}

	void SaveObjectSetting(CString strProjectPath = "Shape.xml")
	{
		if (m_pMultiAllocator)
			return m_pMultiAllocator->SaveObjectSetting(strProjectPath);
	};

	void SelectPannel(BOOL bSelected)
	{
		if (m_pMultiAllocator)
			return m_pMultiAllocator->SelectPannel(bSelected);
	};

	void AddBlendPt(UINT32 iStatus, CPoint pt, UINT32 iArea, BOOL bDarkMask)
	{
		if (m_pMultiAllocator)
		{
			return m_pMultiAllocator->AddBlendPt(iStatus, pt, iArea, bDarkMask);
			if (iStatus == 2)
			{
				m_BlendSetting.iStatus = 0;
			}
		}
	};

	void ReleaseFocus(BOOL bReleaseFocus)
	{
		m_bReleaseFocus = bReleaseFocus;
	};

	void SetBlendPt(BOOL bBlendArea, UINT iArea, UINT iIndex, BOOL bTemp, CPoint ptNew);
	CPoint GetBlendPt(BOOL bBlendArea, UINT iArea, UINT iIndex, BOOL bTemp = TRUE);
	UINT GetBlendPtCount(BOOL bBlendArea, UINT iArea);
	UINT GetBlendLineNum();
	void SetBlendPtFromTemp();
	INT32 GetRefBlendLength(BOOL bWidth)
	{
		if (bWidth)
			return m_BlendSetting.iBlendWidth * 1920 / (1920 - (1920 - m_BlendSetting.iBlendWidth) / 2);
		else
			return m_BlendSetting.iBlendHeight * 1080 / (1080 - (1080 - m_BlendSetting.iBlendHeight) / 2);


	}

	IDirect3DDevice9* GetD3DDevice(BOOL bNeedNewDevice = TRUE, INT32 iIndex = -1)
	{
		if (bNeedNewDevice && iIndex == -1)
		{
			for (int x = 0; x < MAX_D3DNUM; x++)
			{
				if (!m_bD3DPoolFlag[x])
				{
					m_bD3DPoolFlag[x] = TRUE;
					iIndex = x;
					break;
				}
			}

			if (iIndex == -1)
			{
				iIndex = 0;
				AfxMessageBox("Over Panel's Layers Limit", MB_TOPMOST);
			}

		}
		return m_D3DDev[iIndex];

		/*if (m_pMultiAllocator)
			return m_pMultiAllocator->GetD3DDevice();
		else
			return NULL;*/
	};
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnCancel();

#ifdef _ENABLE_CWND
	void Create();
	void Init();
	BOOL RegisterWindowClass(char* szClassName);
#else
	virtual BOOL OnInitDialog();
#endif

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnOK();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void SlaveMsg(UINT nFlags, CPoint point, MOUSE_EVT nEventIdx);
	void DestroyD3D(INT32 iIndex = -1);

	void RecordAVI();
	void StopAVI();
	BOOL IsBlendArea(INT32 iArea)
	{
		if (m_iIndex == 0)
		{
			INT iTempIdx = 0;

			for (int x = 0; x < 4; x++)
			{
				if (m_BlendSetting.iPanelIdxAry[x] == m_iBlendPanelIdx + 1)
				{
					iTempIdx = x;
					break;
				}
			}
			return m_BlendSetting.bBlendArea[iTempIdx][iArea];
		}
		else
			return m_BlendSetting.bBlendArea[m_iBlendPanelIdx][iArea];
	};

//#ifdef _ENABLE_GPU
	DisplayManager* GetDisplayManager();
//#endif

	void DisplayVideo_VW(void* pSurface, int w, int h,bool bDoPresent = true);
	void DisplayVideo_VW(int iIndex, void* pSurface, int w, int h);
	void DisplayVideo_HDEffect(int iIndex, void* pSurface, int w, int h);

	void ApplyMask();
	void Flip();
	int GetLastFrameCount();
	DWORD GetLastPresentTime();
	float GetAvgPresentFPS();
	void EnableDoClear(bool bEnable);
	void CheckEventStatus();
	void SetSyncPBCtrl(SyncPBCtrl* pObj);
	void ApplySetting();
	void SetTip();

	afx_msg LRESULT OnStopAVI(WPARAM wParam, LPARAM lParam);
   	
protected:
	DECLARE_MESSAGE_MAP()
	IDirect3D9*                     m_D3D[MAX_D3DNUM];
	IDirect3DDevice9*               m_D3DDev[MAX_D3DNUM];
	BOOL m_bD3DPoolFlag[MAX_D3DNUM];
	INT32 m_iIndex;
	LONG m_iWidth, m_Height;
	BOOL m_bShapeUI;
	BOOL m_bShowUI;
	BOOL m_bModifyBlendArea;
	CRect m_rectClient;
	CDC*     m_MemoryDC;
	CBitmap  m_Bitmap;
	BLEND_SETTING m_BlendSetting;
	CShapeUIDlg *m_pShapeDlg;
	CArray<CPoint, CPoint> m_pt2DGrid[100];
	CArray<CPoint, CPoint> m_ptBlendArea[4];
	CArray<CPoint, CPoint> m_pt2DGridTemp[100];
	CArray<CPoint, CPoint> m_ptBlendAreaTemp[4];
	CPoint m_ptSelBlend;
	CPoint m_ptClickPos;

	INT32 m_iBlendPanelIdx;
	HRESULT GetVideoSize(CString strAVPath, LONG &iWidth, LONG &iHeight);
	BOOL CreateD3DDevice();
	void CreateBlendObj(BOOL bCreateObject);
	void ModifyPtByMouse(CPoint point);

	// DirectShow interfaces
	CComPtr<IGraphBuilder>          m_GraphBuild;
	CComPtr<IBaseFilter>            m_BaseFilter;
	CComPtr<IMediaControl>          m_MediaControl;
	CComPtr<IVMRSurfaceAllocator9>  m_allocator;
	CComPtr<IVMRWindowlessControl9> m_WindowCtrl;
	CAllocator*                     m_pMultiAllocator;

	IBasicAudio		*m_pBasicAudio;
	IMediaSeeking	*m_pMediaSeeking;
	IVMRMixerControl9 *m_pMixCtrl;
	BOOL               m_bReleaseFocus;
	CComPtr<IVMRSurfaceAllocatorNotify9> lpIVMRSurfAllocNotify;

	bool m_bDoRecordAVI;

	CWnd* m_pParent;

	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL DestroyWindow();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};

#endif


