#ifndef _TreeCtrlUI_H_
#define _TreeCtrlUI_H_

//#include "CameraTreeCtrl.h"
#include "VividTree.h"
#include "NWin.h"
#include "SIniFileW.h"
#include "IOSourceCtrlDll.h"
#include "FileListCtrl.h"
#include "../../../Include/FilePlayerCommon.h"
#include "LogFileDll.h"
#include "CSVReader.h"

enum WriteLevel
{
	WL_NONE = 0,
	WL_FOLDER,
	WL_FILE,
	WL_ALL,
};

typedef struct
{
	char szCmd[64];  // add , delete , add_folder , del_folder
	char szFileName[512];
	char szFolderName[512];
}TestItem;

typedef struct
{
	void* pObj;
	HTREEITEM hItem;
}TreeCtrlUIThreadParam;

class TreeCtrlUI : public EventCallback , public WinMsgCallback
{
	
	//	std::list<std::string> m_NVRList;
		
		VividTree  m_TreeCtrl;
		CTreeCtrl* m_pCTreeCtrl;
		long m_lTreeCtrlID;
		HWND m_hWnd;
		HWND m_hParentWnd;

		bool m_bSimpleMode;

		HTREEITEM m_hCurTreeNode;

		wchar_t m_wszParentPath[1024];
		wchar_t m_wszTmpParentPath[1024];
		wchar_t m_wszTmpParentPath2[1024];

		CImageList m_imageList; //image list used by the tree
		CBitmap m_bitmap; //bitmap witch loads 32bits bitmap
		HBITMAP m_hBitmap;

		CNWin* m_pWin;

		SIniFileW* m_pSIniFileW;
		IOSourceCtrlDll* m_pIOSourceCtrlDll;
		EventCallback* m_pEventCallback;
		FileListCtrl* m_pFileListCtrl;

		int m_iDialogMode;
		LogFileDll* m_pLogFileDll;
		void* m_pLogFileObj;

		bool m_bEnableSearchTag;
		CSVReader m_CSVReader;

		TreeCtrlUIThreadParam m_thread_param;
		
   protected:
   public:
		TreeCtrlUI();		
        ~TreeCtrlUI();


		void Init();

		//void SetCTreeCtrl(CTreeCtrl* pObj,long lTreeCtrlID);
		//void SetHWND(HWND hWnd);
		void SetDialogMode(int iMode);
		void SetParent(HWND hWnd);
		void SetIOSourceCtrl(IOSourceCtrlDll* pObj);
		void SetEventCallback(EventCallback* pObj);
		void SetFileListCtrl(FileListCtrl* pObj);
		void MoveWindow(int x,int y,int w,int h);
		void ShowWindow(bool bShow);

		void CheckCurSelItem();
		void TraverseTreeBranch(CTreeCtrl* pTreeCtrl, HTREEITEM hItem, wchar_t* wszParentPath = L"", bool bDoWrite = false);
		void TraverseAllNodes(bool bDoWrite = false);

		void TraverseTreeBranch2(CTreeCtrl* pTreeCtrl, HTREEITEM hItem, wchar_t* wszParentPath, int iWriteLevel = WL_NONE);
		void TraverseAllNodes2(int iWriteLevel = WL_NONE);

		HTREEITEM InsertNode(HTREEITEM hParent, wchar_t* wszText, bool bHasChild, unsigned long ulID);
		//HTREEITEM InsertNode(HTREEITEM hParent, wchar_t* wszText, bool bHasChild);
		void RemoveNode();
		void Save();
		void Load();
		void WriteToFile(wchar_t* wszItem, wchar_t* wszParentPath, bool bIsFolder, unsigned long ulID);
		void LoadSubNodeFromFile(HTREEITEM hRoot, wchar_t* wszParentPath);

		void PopupMenu(HTREEITEM hItem, POINT* pPoint);
		void AddFolder(HTREEITEM hItem);
		void AddFile(HTREEITEM hItem);
		int AddFile(HTREEITEM hItem, wchar_t* wszFileName);
		int AddFile2(HTREEITEM hItem, wchar_t* wszFileName, bool IsEncrypted);
		void AddFile_Thread(HTREEITEM hItem);
		void DeleteItem();
		void EditTag(HTREEITEM hItem);
		void SearchTag(HTREEITEM hItem);
		void UpdateList();
		void DoUpdateList();
		void SelChanged();
		void SearchChild(wchar_t* wszText = L"", bool bDoSearchTag = false);

		void AddCSVNode(char* szFullFilePath, char* szFileFolder, char* szFileName, char* szTag);
		HTREEITEM FindFolder(char* szFolder);
		HTREEITEM FindTreeItem(char* szFolder, char* szItemName);

		void ReDraw();
		void CheckSaveFile();
		bool CheckTreeNode(wchar_t* wszItemPath);
		bool CheckTreeBranchNode(CTreeCtrl* pTreeCtrl, HTREEITEM hItem, wchar_t* wszParentPath, wchar_t* wszItemPath);
		bool CheckSubNode(wchar_t* wszParentPath);
		int RemoveAll();
		wchar_t* GetCurSelFileName();
		void SetLogFileDll(LogFileDll* pObj);
		void* GetLogFileObj();
		void SearchAllFiles(char* szFolder, SQList* pList, SQList* pFolderList = 0);
		void FindFullPath(HTREEITEM hItem,char* szCurItemPath, bool bTreeNode = false);

		HTREEITEM FindSubFolder(HTREEITEM hItem, char* szSubFolder);
		HTREEITEM FindTreeItem(HTREEITEM hFolderItem, char* szItemName);

		void DeleteItem(HTREEITEM hItem);
		void RemoveNode(HTREEITEM hItem);
		void AddFolder(HTREEITEM hItem, char* szFolderName);

		bool IsOpened(char* szFileName);

		void SetSimpleMode(bool bEnable);
		void EnableEnableSearchTag(bool bEnable);
		bool IsReadOnly(HTREEITEM hItem);

		virtual void* Event(int iMsg, void* pParameter1, void* pParameter2);
		virtual void* WinMsg(int iMsg, void* pParameter1, void* pParameter2);

		//rtsp
		void AddRTSPUrl(HTREEITEM hItem);

		void TestAddFile(HTREEITEM hItem, char* szFilePath);
		void TestAddFolder(HTREEITEM hItem, char* szFolderName);
		void TestRemoveFolder(char* szParent, char* szChild);
		void DoAutoTest();
		void Test();
		
};

#endif
