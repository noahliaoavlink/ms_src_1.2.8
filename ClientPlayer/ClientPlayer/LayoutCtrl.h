#ifndef _LayoutCtrl_H
#define _LayoutCtrl_H

#define MAX_GRIDS 128
#define MAX_LINES 32

#include "../../Include/LayoutInfo.h"

typedef struct
{
	int iTotal;
	POINT p1[MAX_LINES];
	POINT p2[MAX_LINES];
}LayoutLineInfo;

class LayoutCtrl
{
		int m_iType;
		int m_iTotal;
		RECT m_rtGrid[MAX_GRIDS];

		LayoutLineInfo m_LayoutLineInfo;

		int m_iWidth;
		int m_iHeight;
   public:
	    LayoutCtrl();
        ~LayoutCtrl();

		void MakeInfo(int iType,int w,int h);
		void MakeLayoutInfo();
		void MakeGridInfo(int iTotal);
		void MakeGridInfo(int iCellW,int iCellH);
		RECT* GetGridRect(int iIndex);
		void GetGridRect(int iIndex,RECT* pRect,int iCellW,int iCellH);
		int GetType();
		int GetWidth();
		int GetHeight();
		int GetTotal();

		void MakeLayoutLineInfo();
		void MakeLayoutLineInfo(int iCellW,int iCellH);
		LayoutLineInfo* GetLayoutLineInfo();
};

#endif