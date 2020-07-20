// LogoDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MediaServer.h"
#include "LogoDlg.h"
#include "afxdialogex.h"
#include <afxinet.h>


// CLogoDlg 對話方塊

IMPLEMENT_DYNAMIC(CLogoDlg, CDialog)

CLogoDlg::CLogoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TEMP_DLG, pParent)
{
	m_bShowLogo = true;
}

CLogoDlg::~CLogoDlg()
{
}

void CLogoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLogoDlg, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CLogoDlg 訊息處理常式

CString GetVersion(IN LPCTSTR lpszFileName)
{
	LPVOID versionInfo = NULL;
	DWORD vLen;
	DWORD vSize = ::GetFileVersionInfoSize((LPSTR)lpszFileName, &vLen);
	CString strVersion;

	if (vSize)
	{
		LPVOID version;
		versionInfo = new BYTE[vSize + 1];

		if (::GetFileVersionInfo((LPSTR)lpszFileName, vLen, vSize, versionInfo))
		{
			TCHAR fileVersion[256];

			_stprintf(fileVersion, _T("\\VarFileInfo\\Translation"));

			if (::VerQueryValue(versionInfo, fileVersion, &version, (UINT*)&vLen) && vLen == 4)
			{
				DWORD langD;
				CopyMemory(&langD, version, 4);
				_stprintf(fileVersion, _T("\\StringFileInfo\\%02X%02X%02X%02X\\FileVersion"),
					(langD & 0xFF00) >> 8, langD & 0xff, (langD & 0xFF000000) >> 24, (langD & 0xFF0000) >> 16);
			}
			else
			{
				_stprintf(fileVersion, _T("\\StringFileInfo\\%04X04B0\\FileVersion"), ::GetUserDefaultLangID());
			}

			if (::VerQueryValue(versionInfo, fileVersion, &version, (UINT*)&vLen))
			{
				_tcscpy(fileVersion, (LPSTR)version);

				strVersion = fileVersion;
			}
		}
	}

	if (versionInfo)
		delete[]versionInfo;

	return strVersion;
}

BOOL CLogoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化

	if (!m_bShowLogo)
	{
		ShowWindow(SW_HIDE);
		MoveWindow(0,0,0,0);
	}

	DISPLAY_DEVICE dispDev;
	BOOL bResult;
	ZeroMemory(&dispDev, sizeof(DISPLAY_DEVICE));
	dispDev.cb = sizeof(DISPLAY_DEVICE);
	DEVMODE *lpDevMode;
	lpDevMode = new DEVMODE;
	int i = 0;
	CString Str[500];
	CString str;
	CRect rectTemp;
	rectTemp.left = 0;
	rectTemp.top = 0;

#if 0
	try
	{
		CInternetSession m_sess;// (AfxGetAppName(), 1, PRE_CONFIG_INTERNET_ACCESS);
		CFtpConnection *m_pConnect;
		//m_pConnect = m_sess.GetFtpConnection("FTP://ftp.hostedftp.com",
		//	"jack.cheng@cctch.com.tw", "12345678", 22);
		m_pConnect = m_sess.GetFtpConnection("ftp.bruvis.net",
			"STARE@bruvis.net", "avlink035601528");
		//m_pConnect->SetCurrentDirectory("/STARE");

		CFtpFileFind finder(m_pConnect);

		if (finder.FindFile(_T("Release Note*")))
		{
			finder.FindNextFile();

			CString strCurVer, strLastVer;
			strLastVer = finder.GetFileName().Mid(14, 4);
			strCurVer = GetVersion(theApp.m_strCurPath + "MediaServer.exe");
			strCurVer.Replace(".", "");


			if (atoi(strLastVer) > atoi(strCurVer) && MessageBox("STARE Has New Version, Update?", "Update", MB_OKCANCEL | MB_TASKMODAL) == IDOK)
			{
				if (finder.FindFile(_T("STARE_UPDATE.exe")))
				{
					finder.FindNextFile();
					m_pConnect->GetFile(finder.GetFilePath(), theApp.m_strCurPath + "STARE_UPDATE.exe", FALSE);
				}

				STARTUPINFO				si = { 0 };
				PROCESS_INFORMATION		pi = { 0 };
				si.cb = sizeof(STARTUPINFO);
				CString strTemp = theApp.m_strCurPath + "STARE_UPDATE.exe";
				CreateProcess(NULL, strTemp.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
				m_pConnect->Close();
				exit(0);
			}
		}

		m_pConnect->Close();
	}
	catch (...)
	{
		printf("Can't connect to server");
	}
#endif

	for (int x = 0; x < 16; x++)
	{
		bResult = EnumDisplayDevices(NULL, x, &dispDev, 0);

		if (!bResult)
			break;

		bResult = EnumDisplaySettings(dispDev.DeviceName, ENUM_CURRENT_SETTINGS, lpDevMode);

		if (!bResult)
			continue;

		rectTemp.right = lpDevMode->dmPosition.x + lpDevMode->dmPelsWidth;
		rectTemp.bottom = lpDevMode->dmPosition.y + lpDevMode->dmPelsHeight;
		rectTemp.left = lpDevMode->dmPosition.x;
		rectTemp.top = lpDevMode->dmPosition.y;

		if (g_DispArys.GetCount() > 0)
		{
			CRect curRect;
			int iIndex = -1;
			for (int j = 0; j < g_DispArys.GetCount(); j++)
			{
				curRect = g_DispArys.GetAt(j);
				if (curRect.left > lpDevMode->dmPosition.x)
				{
					iIndex = j;
					break;
				}
			}

			if(iIndex == -1)
				g_DispArys.Add(rectTemp);
			else
				g_DispArys.InsertAt(iIndex,rectTemp);
		}
		else
			g_DispArys.Add(rectTemp);

		i = 0;
		/*while(bResult)
		{
		str.Format("%d_(%d, %d)", x, lpDevMode->dmPelsWidth, lpDevMode->dmPelsHeight);
		Str[i] = str;

		bResult = EnumDisplaySettings(dispDev.DeviceName,++i,lpDevMode);
		}

		for(int j=0;j<=i;j++)
		{
		OutputDebugString(Str[j]);
		}
		OutputDebugString("\n");*/
	}

	delete lpDevMode;

	/*DEVMODE *dm = new DEVMODE();
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, dm);
	dm->dmPelsWidth = 1440;
	dm->dmPelsHeight = 900;
	ChangeDisplaySettings(dm, CDS_UPDATEREGISTRY);
	delete dm;*/
	
	
	if (m_bShowLogo)
	{
		SetWindowLong(this->m_hWnd, GWL_EXSTYLE, GetWindowLong(this->m_hWnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
		SetLayeredWindowAttributes(0, 220, LWA_ALPHA);
		CRgn NullRgn;
		NullRgn.CreateRectRgn(0, 0, 0, 0);
		m_FramePic = new CMenu_Class(this, m_hWnd, 500, theApp.m_strCurPath + "UI Folder\\Main UI\\LOGO.bmp");
		//m_FramePic->OffsetObject(CPoint(300, 0));
		m_FramePic->Initialize();

		rectTemp = g_DispArys.GetAt(0);
		INT32 iWidth = rectTemp.Width();
		INT32 iHeight = rectTemp.Height();

		if (iWidth != 1920 || iHeight != 1080)
			m_FramePic->OnZoom(0, 0, float(iWidth) / 1920, float(iHeight) / 1080);

		m_FramePic->ResetEffectRegion();
		m_FramePic->SetDlgRegion(this);
		m_FramePic->SetWindowRgn(NullRgn, true);
	}
	
	SetTimer(0, 100, NULL);

	m_pMediaServerThread = AfxBeginThread(MediaServerThread, this);
	return TRUE;  // return TRUE unless you set the focus to a control

				  // EXCEPTION: OCX 屬性頁應傳回 FALSE
}
UINT CLogoDlg::MediaServerThread(LPVOID pParam)
{
	CLogoDlg *pThis = reinterpret_cast<CLogoDlg *>(pParam);
	pThis->ShowLoop();
	return 1;
}


void CLogoDlg::ShowLoop()
{
	theApp.m_iAppStatus = -1;
	m_dlgMediaServer.DoModal();
	//m_dlgMediaServer->Create(IDD_MEDIASERVER_DIALOG);
	//m_dlgMediaServer->SetWindowPos(NULL, rectTemp.left, rectTemp.top,
		//rectTemp.Width(), rectTemp.Height(), SWP_SHOWWINDOW);

	//while (theApp.m_iAppStatus == -1)
		//Sleep(100);

	//m_FramePic->ShowWindow(SW_HIDE);

	//while (theApp.m_iAppStatus == 1)
		//Sleep(100);

	OnCancel();



}

void CLogoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	if (nIDEvent == 0 && theApp.m_iAppStatus == 1)
	{
		ShowWindow(SW_HIDE);
		KillTimer(0);
	}

	CDialog::OnTimer(nIDEvent);
}

void CLogoDlg::EnableShowLogo(bool bEnable)
{
	m_bShowLogo = bEnable;
}
