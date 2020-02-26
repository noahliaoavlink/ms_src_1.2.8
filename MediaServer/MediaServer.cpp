// MediaServer.cpp : �w�q���ε{�������O�欰�C
//

#include "stdafx.h"
#include "MediaServer.h"
#include "MediaServerDlg.h"
#include "LogoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define DEF_SAVELOG


// CMediaServerApp

BEGIN_MESSAGE_MAP(CMediaServerApp, CWinApp)
	//ON_COMMAND(ID_HELP, &CWinApp::OnHelp) //remove F1 key function on MediaServer main function page
END_MESSAGE_MAP()


// CMediaServerApp �غc

CMediaServerApp::CMediaServerApp()
{
	// TODO: �b���[�J�غc�{���X�A
	// �N�Ҧ����n����l�]�w�[�J InitInstance ��
	
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	m_bSampleVersion = FALSE;

	m_bLogoMode = true;
	m_bSwitchToShape = false;
	m_fPresentFPS = 0.0;

	InitializeCriticalSection(&m_csInUse);

	// Get our exe file name and directory
	//
	TCHAR szModuleFileName[_MAX_PATH];
	if (0 == GetModuleFileName(NULL, szModuleFileName, _MAX_PATH))
	{
		// show error message
		//

		return;
	}

	// Get the exe file directory
	//
	_splitpath_s(szModuleFileName, drive, dir, fname, ext);

	m_strCurPath = drive;
	m_strCurPath += dir;

}


// �Ȧ����@�� CMediaServerApp ����

CMediaServerApp theApp;

void CMediaServerApp::SaveLog(CString strLog, CString strLogPath)
{
	CFileFind fFind;
	CStdioFile sfFile;

#ifndef DEF_SAVELOG
	return;
#endif

	EnterCriticalSection(&m_csInUse);

	if (!fFind.FindFile(strLogPath))
		sfFile.Open(strLogPath, CFile::modeCreate | CFile::modeReadWrite);
	else
	{
		sfFile.Open(strLogPath, CFile::CFile::modeReadWrite);
		sfFile.SeekToEnd();
	}

	sfFile.WriteString(strLog + "\n");
	sfFile.Close();
	LeaveCriticalSection(&m_csInUse);
}

// CMediaServerApp ��l�]�w

BOOL CMediaServerApp::InitInstance()
{
	// ���p���ε{����T�M����w�ϥ� ComCtl32.dll 6 (�t) �H�᪩���A
	// �ӱҰʵ�ı�Ƽ˦��A�b Windows XP �W�A�h�ݭn InitCommonControls()�C
	// �_�h����������إ߳��N���ѡC

	m_pLogFileDll = new LogFileDll;
	m_bEnableLog = false;
	m_pLogFileObj = 0;

	m_hShareMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(65535), _T("MediaServer SharedMemory"));
	if (m_hShareMemory == NULL || m_hShareMemory == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	// if the object already exists
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		MessageBox(NULL, "Media Server is running.Please wait a moment.", "Warning", MB_OK|MB_TOPMOST);

		return FALSE;
	}


	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �]�w�n�]�t�Ҧ��z�Q�n�Ω����ε{������
	// �q�α�����O�C
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// �зǪ�l�]�w
	// �p�G�z���ϥγo�ǥ\��åB�Q���
	// �̫᧹�����i�����ɤj�p�A�z�i�H
	// �q�U�C�{���X�������ݭn����l�Ʊ`���A
	// �ܧ��x�s�]�w�Ȫ��n�����X
	// TODO: �z���ӾA�׭ק惡�r��
	// (�Ҧp�A���q�W�٩β�´�W��)
	SetRegistryKey(_T("���� AppWizard �Ҳ��ͪ����ε{��"));	

	ULONG_PTR m_gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	ParseCommandLine();

	
	CLogoDlg dlgLogo;
	//dlgLogo.Create(IDD_TEMP_DLG);
	//dlgLogo.SetWindowPos(NULL, 0, 0, 1920, 1080, SWP_SHOWWINDOW);
	char szData[256];

	m_bSampleVersion = GetPrivateProfileInt("System", "Sample Version", 0, theApp.m_strCurPath + "Setting.ini");
	m_bEnableEdgeBlending = GetPrivateProfileInt("System", "EnableEdgeBlending", 0, theApp.m_strCurPath + "Setting.ini");
	m_bImportAutoTesting = GetPrivateProfileInt("System", "ImportAutoTesting", 0, theApp.m_strCurPath + "Setting.ini");
	m_bEnableVWallDisplay = GetPrivateProfileInt("System", "EnableVWallDisplay", 0, theApp.m_strCurPath + "Setting.ini");
	m_iVWallOffsetIndex = GetPrivateProfileInt("System", "VWallOffsetIndex", 0, theApp.m_strCurPath + "Setting.ini");

	GetPrivateProfileString("System", "VWallIndexOrder", "0,1,2,3", szData, 32, theApp.m_strCurPath + "Setting.ini");

	for (int i = 0; i < 4; i++)
		m_iVWallIndexOrder[i] = GetNextNumberFromString(',', szData);

	//CMediaServerDlg dlg;
	m_pMainWnd = &dlgLogo;

	dlgLogo.EnableShowLogo(m_bLogoMode);

	INT_PTR nResponse = dlgLogo.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �b����m��ϥ� [�T�w] �Ӱ���ϥι�ܤ����
		// �B�z���{���X
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �b����m��ϥ� [����] �Ӱ���ϥι�ܤ����
		// �B�z���{���X
	}
	
	GdiplusShutdown(m_gdiplusToken);

	Sleep(3000);

	// �]���w�g������ܤ���A�Ǧ^ FALSE�A�ҥH�ڭ̷|�������ε{���A
	// �ӫD���ܶ}�l���ε{�����T���C
	return FALSE;
}

int CMediaServerApp::ExitInstance()
{
	Add2LogFile(LL_INFO, "CMediaServerApp::ExitInstance() ");
	if (m_pLogFileDll)
		delete m_pLogFileDll;
	return CWinApp::ExitInstance();
}

void CMediaServerApp::SetLogFileInfo(bool bEnable,void* pObj)
{
	m_bEnableLog = bEnable;
	m_pLogFileObj = pObj;
}

void CMediaServerApp::Add2LogFile(int iLevel, char* szMsg)
{
	if (m_bEnableLog && m_pLogFileDll && m_pLogFileObj)
	{
		m_pLogFileDll->_Out_LGD_AddLog(m_pLogFileObj, iLevel, szMsg);
	}
}

void CMediaServerApp::DeleteObject(INT iPanelIdx, INT iObjIdx)
{
	INT32 iFindIdx = -1;
	INT32 iMediaIdx = -1;
	CString strMediaPath = "", strTemp;

	if (iPanelIdx == -1)
	{
		g_StreamMappingAry.RemoveAll();
		SaveShapeProject(m_strCurPath + "Mapping.xml");
		return;
	}

	for (int x = 0; x < g_StreamMappingAry.GetCount(); x++)
	{
		if (g_StreamMappingAry.GetAt(x).iPanelIdx == iPanelIdx &&
			g_StreamMappingAry.GetAt(x).iObjIdx == iObjIdx)
		{
			iFindIdx = x;

			if (g_StreamMappingAry.GetAt(x).strVideoPath.Left(6).Find("Video ") == -1)
			{
				iMediaIdx = g_StreamMappingAry.GetAt(x).iMediaIdx;
				strMediaPath = g_StreamMappingAry.GetAt(x).strVideoPath;
			}
		}
		else if (iFindIdx != -1 && g_StreamMappingAry.GetAt(x).iPanelIdx == iPanelIdx)
		{
			g_StreamMappingAry.GetAt(x).iObjIdx -= 1;

			strTemp = g_StreamMappingAry.GetAt(x).strVideoPath;

			if (strTemp.Left(6).Find("Video ") != -1)
			{
				if (iMediaIdx == g_StreamMappingAry.GetAt(x).iMediaIdx)
				{
					if (strMediaPath.Left(6).Find("Video ") == -1)
					{
						strMediaPath.Format("Video %i (%s)", iMediaIdx + 1, strMediaPath);
					}

					g_StreamMappingAry.GetAt(x).strVideoPath = strMediaPath;
				}
			}			
		}
	}

	if (iFindIdx != -1)
		g_StreamMappingAry.RemoveAt(iFindIdx);
	
	SaveShapeProject(m_strCurPath + "Mapping.xml");
}

void CMediaServerApp::SaveShapeProject(CString strProjectPath)
{
	tinyxml2::XMLDocument xmlDoc;

	auto pProject = xmlDoc.NewElement("Project");
	pProject->SetAttribute("Version", "1.0");
	xmlDoc.InsertFirstChild(pProject);
	CString strTemp, strVal, strIdx;
	STREM_MAPPING StreamMap;

	for (int x = 0; x < g_StreamMappingAry.GetCount(); x++)
	{
		StreamMap = g_StreamMappingAry.GetAt(x);
		strIdx.Format("%i", x);
		auto pMapItem = xmlDoc.NewElement("Mapping");
		pMapItem->SetAttribute("Id", strIdx);

		auto pSubItem = xmlDoc.NewElement("Video");
		pSubItem->SetAttribute("Id", strIdx);
		strTemp.Format("%i", StreamMap.iMediaIdx);

		pSubItem->SetAttribute("Index", strTemp);
		pSubItem->SetAttribute("Path", StreamMap.strVideoPath);
		strTemp.Format("%i", StreamMap.bExtraVideo);
		pSubItem->SetAttribute("Extra", strTemp);
		pMapItem->InsertEndChild(pSubItem);

		pSubItem = xmlDoc.NewElement("Object");
		pSubItem->SetAttribute("Id", strIdx);
		strTemp.Format("%i", StreamMap.iObjIdx);
		pSubItem->SetAttribute("Index", strTemp);
		pSubItem->SetAttribute("Path", StreamMap.strObjPath);
		pMapItem->InsertEndChild(pSubItem);

		pSubItem = xmlDoc.NewElement("Pannel");
		pSubItem->SetAttribute("Id", strIdx);
		strTemp.Format("%i", StreamMap.iPanelIdx);
		pSubItem->SetAttribute("Index", strTemp);
		pMapItem->InsertEndChild(pSubItem);
		pProject->InsertEndChild(pMapItem);
	}

	auto eResult = xmlDoc.SaveFile(strProjectPath);
}


STREM_MAPPING CMediaServerApp::GetStreamMapByIdx(INT iPanelIdx, INT iObjIdx)
{
	STREM_MAPPING smResult;
	smResult.iPanelIdx = smResult.iMediaIdx = smResult.iObjIdx = smResult.iWidth = smResult.iHeight = 0;

	for (int x = 0; x < g_StreamMappingAry.GetCount(); x++)
	{
		if (g_StreamMappingAry.GetAt(x).iPanelIdx == iPanelIdx &&
			g_StreamMappingAry.GetAt(x).iObjIdx == iObjIdx)
		{
			smResult = g_StreamMappingAry.GetAt(x);

			if (smResult.iMediaIdx & DEF_GLOBAL_MEDIA)
			{
				smResult = g_StreamMappingAry.GetAt(smResult.iMediaIdx - DEF_GLOBAL_MEDIA);
				smResult.iPanelIdx = iPanelIdx;
				smResult.iObjIdx = iObjIdx;
			}
			break;
		}
	}

	return smResult;
}

LogFileDll* CMediaServerApp::GetLogFileDll()
{
	return m_pLogFileDll;
}

void CMediaServerApp::ParseCommandLine()
{
	char szPath[512];
	char szParam[64] = "";
	char* pCmdLine = GetCommandLineA();
	sscanf(pCmdLine, "%s %s", szPath, szParam);

	if (strcmp(szParam, "none_logo") == 0)
		m_bLogoMode = false;
	else if (strcmp(szParam, "switch_to_shape") == 0)
		m_bSwitchToShape = true;
}