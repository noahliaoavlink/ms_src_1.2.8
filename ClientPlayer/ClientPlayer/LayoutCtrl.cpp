#include "stdafx.h"
#include "LayoutCtrl.h"
#include <math.h> 

typedef struct
{
	int x;
	int y;
}GridItem;

typedef struct
{
	GridItem base_grid;
	int iTotalOfGrids;
	GridItem grid_range[][2];
}CustomGridInfo;

CustomGridInfo g_CustomGrid;

LayoutCtrl::LayoutCtrl()
{	
	/*
	g_CustomGrid.base_grid.x = 6;
	g_CustomGrid.base_grid.y = 5;
	g_CustomGrid.iTotalOfGrids = 12;
	g_CustomGrid.grid_range[0].x;
	g_CustomGrid.grid_range[0].y;
	*/
}

LayoutCtrl::~LayoutCtrl()
{
}

void LayoutCtrl::MakeInfo(int iType,int w,int h)
{
	m_iWidth = w;
	m_iHeight = h;
	m_iType = iType;

	MakeLayoutInfo();
	MakeLayoutLineInfo();
}

void LayoutCtrl::MakeLayoutInfo()
{
	switch(m_iType)
	{
		case LT_1x1:
			MakeGridInfo(1,1);
			break;
		case LT_2x2:
			MakeGridInfo(2,2);
			break;
		case LT_3x3:
			MakeGridInfo(3,3);
			break;
		case LT_4x4:
			MakeGridInfo(4,4);
			break;
			/*
		case LT_5x5:
			MakeGridInfo(5,5);
			break;
		case LT_6x6:
			MakeGridInfo(6,6);
			break;
		case LT_8x8:
			MakeGridInfo(8,8);
			break;
			*/
		case LT_3x2:
			MakeGridInfo(3,2);
			break;
		case LT_4x3:
			MakeGridInfo(4,3);
			break;
		case LT_5x3:
			MakeGridInfo(5,3);
			break;
			/*
		case LT_8x6:
			MakeGridInfo(8,6);
			break;
		case LT_B1S7://{"0x0-2x2", "3x0", "3x1", "3x2", "0x3", "1x3", "2x3", "3x3"};
			{
				RECT rRect1;
				RECT rRect2;
				GetGridRect(0,&rRect1,4,4);
				GetGridRect(10,&rRect2,4,4);
				m_rtGrid[0].left = rRect1.left;
				m_rtGrid[0].top = rRect1.top;
				m_rtGrid[0].right = rRect2.right;
				m_rtGrid[0].bottom = rRect2.bottom;

				GetGridRect(3,&m_rtGrid[1],4,4);
				GetGridRect(7,&m_rtGrid[2],4,4);
				for(int i = 3;i < 8;i++)
					GetGridRect(i + 8,&m_rtGrid[i],4,4);
				m_iTotal = 8;
			}
			break;
		case LT_B2S7://{"0x0-1x1", "2x0-3x1", "4x0", "4x1", "0x2", "1x2", "2x2", "3x2", "4x2"};
			{
				RECT rRect1;
				RECT rRect2;
				GetGridRect(0,&rRect1,5,3);
				GetGridRect(6,&rRect2,5,3);
				m_rtGrid[0].left = rRect1.left;
				m_rtGrid[0].top = rRect1.top;
				m_rtGrid[0].right = rRect2.right;
				m_rtGrid[0].bottom = rRect2.bottom;

				GetGridRect(2,&rRect1,5,3);
				GetGridRect(8,&rRect2,5,3);
				m_rtGrid[1].left = rRect1.left;
				m_rtGrid[1].top = rRect1.top;
				m_rtGrid[1].right = rRect2.right;
				m_rtGrid[1].bottom = rRect2.bottom;

				GetGridRect(4,&m_rtGrid[2],5,3);
				GetGridRect(9,&m_rtGrid[3],5,3);

				for(int i = 0;i < 5;i++)
					GetGridRect(10+i,&m_rtGrid[4+i],5,3);
				m_iTotal = 9;
			}
			break;
		case LT_B2S8://{"0x0-1x1", "2x0-3x1", "0x2", "1x2", "2x2", "3x2", "0x3", "1x3", "2x3", "3x3"};
			{
				RECT rRect1;
				RECT rRect2;
				GetGridRect(0,&rRect1,4,4);
				GetGridRect(5,&rRect2,4,4);
				m_rtGrid[0].left = rRect1.left;
				m_rtGrid[0].top = rRect1.top;
				m_rtGrid[0].right = rRect2.right;
				m_rtGrid[0].bottom = rRect2.bottom;

				GetGridRect(2,&rRect1,4,4);
				GetGridRect(7,&rRect2,4,4);
				m_rtGrid[1].left = rRect1.left;
				m_rtGrid[1].top = rRect1.top;
				m_rtGrid[1].right = rRect2.right;
				m_rtGrid[1].bottom = rRect2.bottom;

				for(int i = 0;i < 4;i++)
				{
					GetGridRect(8+i,&m_rtGrid[2+i],4,4);//2-5
					GetGridRect(12+i,&m_rtGrid[2+4+i],4,4);//6-9
				}
				m_iTotal = 10;
			}
			break;
		case LT_B1S12://{"1x1-2x2", "0x0", "1x0", "2x0", "3x0", "0x1", "3x1", "0x2", "3x2", "0x3", "1x3", "2x3", "3x3"};
			{
				RECT rRect1;
				RECT rRect2;

				GetGridRect(5,&rRect1,4,4);
				GetGridRect(10,&rRect2,4,4);
				m_rtGrid[0].left = rRect1.left;
				m_rtGrid[0].top = rRect1.top;
				m_rtGrid[0].right = rRect2.right;
				m_rtGrid[0].bottom = rRect2.bottom;

				GetGridRect(4,&m_rtGrid[5],4,4);
				GetGridRect(7,&m_rtGrid[6],4,4);

				GetGridRect(8,&m_rtGrid[7],4,4);
				GetGridRect(11,&m_rtGrid[8],4,4);

				for(int i = 0;i < 4;i++)
				{
					GetGridRect(i,&m_rtGrid[1+i],4,4);
					GetGridRect(12+i,&m_rtGrid[9+i],4,4);
				}
				m_iTotal = 13;
			}
			break;
			*/
	}
}

void LayoutCtrl::MakeGridInfo(int iTotal)
{
	int iRoot = sqrt((double)iTotal);

	int iUnitW = m_iWidth / iRoot;
	int iUnitH = m_iHeight / iRoot;
	int x = 0,y = 0;

	for(int i = 0;i < iTotal;i++)
	{
		x = i % iRoot;
		y = i / iRoot;

		m_rtGrid[i].left = x * iUnitW + 1;
		m_rtGrid[i].top = y * iUnitH + 1;
		m_rtGrid[i].right = m_rtGrid[i].left + iUnitW - 1;
		m_rtGrid[i].bottom = m_rtGrid[i].top + iUnitH - 1;
	}
}

void LayoutCtrl::MakeGridInfo(int iCellW,int iCellH)
{
	int iCount = 0;
	int iHFactor = m_iHeight / iCellH;
	int iWFactor =  m_iWidth / iCellW;
	for(int i = 0;i < iCellH;i++)
	{
		for(int j = 0;j < iCellW;j++)
		{
			m_rtGrid[iCount].left = iWFactor * j + 1;
			m_rtGrid[iCount].top = iHFactor * i + 1;
			m_rtGrid[iCount].right = m_rtGrid[iCount].left + iWFactor - 1;
			m_rtGrid[iCount].bottom = m_rtGrid[iCount].top + iHFactor - 1;
			iCount++;
		}
	}
	m_iTotal = iCount;
}

void LayoutCtrl::GetGridRect(int iIndex,RECT* pRect,int iCellW,int iCellH)
{
	int iCount = 0;
	int iHFactor = m_iHeight / iCellH;
	int iWFactor =  m_iWidth / iCellW;
	for(int i = 0;i < iCellH;i++)
	{
		for(int j = 0;j < iCellW;j++)
		{
			if(iIndex == iCount)
			{
				pRect->left = iWFactor * j;
				pRect->top = iHFactor * i;
				pRect->right = pRect->left + iWFactor;
				pRect->bottom = pRect->top + iHFactor;
				return ;
			}
			iCount++;
		}
	}
}

RECT* LayoutCtrl::GetGridRect(int iIndex)
{
	if(m_iTotal > iIndex)
		return &m_rtGrid[iIndex];
	return 0;
}

int LayoutCtrl::GetType()
{
	return m_iType;
}

int LayoutCtrl::GetWidth()
{
	return m_iWidth;
}

int LayoutCtrl::GetHeight()
{
	return m_iHeight;
}

int LayoutCtrl::GetTotal()
{
	return m_iTotal;
}

void LayoutCtrl::MakeLayoutLineInfo()
{
	switch(m_iType)
	{
		case LT_1x1:
			MakeLayoutLineInfo(1,1);
			break;
		case LT_2x2:
			MakeLayoutLineInfo(2,2);
			break;
		case LT_3x3:
			MakeLayoutLineInfo(3,3);
			break;
		case LT_4x4:
			MakeLayoutLineInfo(4,4);
			break;
			/*
		case LT_5x5:
			MakeLayoutLineInfo(5,5);
			break;
		case LT_6x6:
			MakeLayoutLineInfo(6,6);
			break;
		case LT_8x8:
			MakeLayoutLineInfo(8,8);
			break;
			*/
		case LT_3x2:
			MakeLayoutLineInfo(3,2);
			break;
		case LT_4x3:
			MakeLayoutLineInfo(4,3);
			break;
		case LT_5x3:
			MakeLayoutLineInfo(5,3);
			break;
			/*
		case LT_8x6:
			MakeLayoutLineInfo(8,6);
			break;
		case LT_B1S7:
			{
				m_LayoutLineInfo.iTotal = 6;
				int iHFactor = m_iHeight / 4;
				int iWFactor = m_iWidth / 4;

				int iCount = 0;
				for(int v = 0;v < 2;v++)
				{
					m_LayoutLineInfo.p1[iCount].x = iWFactor * (v + 1);
					m_LayoutLineInfo.p1[iCount].y = iHFactor * 3;
					m_LayoutLineInfo.p2[iCount].x = iWFactor * (v + 1);
					m_LayoutLineInfo.p2[iCount].y = m_iHeight;
					iCount++;
				}

				for(int h = 0;h < 2;h++)
				{
					m_LayoutLineInfo.p1[iCount].x = iWFactor * 3;
					m_LayoutLineInfo.p1[iCount].y = iHFactor * (h + 1);
					m_LayoutLineInfo.p2[iCount].x = m_iWidth;
					m_LayoutLineInfo.p2[iCount].y = iHFactor * (h + 1);
					iCount++;
				}

				//v
				m_LayoutLineInfo.p1[iCount].x = iWFactor * 3;
				m_LayoutLineInfo.p1[iCount].y = 0;
				m_LayoutLineInfo.p2[iCount].x = iWFactor * 3;
				m_LayoutLineInfo.p2[iCount].y = m_iHeight;	
				iCount++;

				//h
				m_LayoutLineInfo.p1[iCount].x = 0;
				m_LayoutLineInfo.p1[iCount].y = iHFactor * 3;
				m_LayoutLineInfo.p2[iCount].x = m_iWidth;
				m_LayoutLineInfo.p2[iCount].y = iHFactor * 3;
				iCount++;
			}
			break;
		case LT_B2S7:
			{
				m_LayoutLineInfo.iTotal = 6;
				int iHFactor = m_iHeight / 3;
				int iWFactor = m_iWidth / 5;

				int iCount = 0;
				for(int v = 0;v < 4;v++)
				{
					m_LayoutLineInfo.p1[iCount].x = iWFactor * (v + 1);

					if(v % 2 == 1)
						m_LayoutLineInfo.p1[iCount].y = 0;
					else
						m_LayoutLineInfo.p1[iCount].y = iHFactor * 2;

					m_LayoutLineInfo.p2[iCount].x = iWFactor * (v + 1);
					m_LayoutLineInfo.p2[iCount].y = m_iHeight;
					iCount++;
				}

				for(int h = 0;h < 2;h++)
				{
					if(h == 0)
						m_LayoutLineInfo.p1[iCount].x = iWFactor * 4;
					else
						m_LayoutLineInfo.p1[iCount].x = 0;
					
					m_LayoutLineInfo.p1[iCount].y = iHFactor * (h + 1);
					m_LayoutLineInfo.p2[iCount].x = m_iWidth;
					m_LayoutLineInfo.p2[iCount].y = iHFactor * (h + 1);
					iCount++;
				}
			}
			break;
		case LT_B2S8:
			{
				m_LayoutLineInfo.iTotal = 5;
				int iHFactor = m_iHeight / 4;
				int iWFactor = m_iWidth / 4;

				int iCount = 0;
				for(int v = 0;v < 3;v++)
				{
					m_LayoutLineInfo.p1[iCount].x = iWFactor * (v + 1);

					if(v == 1)
						m_LayoutLineInfo.p1[iCount].y = 0;
					else
						m_LayoutLineInfo.p1[iCount].y = iHFactor * 2;
					m_LayoutLineInfo.p2[iCount].x = iWFactor * (v + 1);
					m_LayoutLineInfo.p2[iCount].y = m_iHeight;
					iCount++;
				}

				for(int h = 0;h < 2;h++)
				{
					m_LayoutLineInfo.p1[iCount].x = 0;
					m_LayoutLineInfo.p1[iCount].y = iHFactor * (h + 2);
					m_LayoutLineInfo.p2[iCount].x = m_iWidth;
					m_LayoutLineInfo.p2[iCount].y = iHFactor * (h + 2);
					iCount++;
				}
			}
			break;
		case LT_B1S12:
			{
				m_LayoutLineInfo.iTotal = 8;
				int iHFactor = m_iHeight / 4;
				int iWFactor = m_iWidth / 4;

				int iCount = 0;
				for(int v = 0;v < 3;v++)
				{
					if(v == 1)
					{
						m_LayoutLineInfo.p1[iCount].x = iWFactor * (v + 1);
						m_LayoutLineInfo.p1[iCount].y = 0;
						m_LayoutLineInfo.p2[iCount].x = iWFactor * (v + 1);
						m_LayoutLineInfo.p2[iCount].y = iHFactor;
						iCount++;
						m_LayoutLineInfo.p1[iCount].x = iWFactor * (v + 1);
						m_LayoutLineInfo.p1[iCount].y = iHFactor * 3;
						m_LayoutLineInfo.p2[iCount].x = iWFactor * (v + 1);
						m_LayoutLineInfo.p2[iCount].y = m_iHeight;
					}
					else
					{
						m_LayoutLineInfo.p1[iCount].x = iWFactor * (v + 1);
						m_LayoutLineInfo.p1[iCount].y = 0;
						m_LayoutLineInfo.p2[iCount].x = iWFactor * (v + 1);
						m_LayoutLineInfo.p2[iCount].y = m_iHeight;
					}
					iCount++;
				}

				for(int h = 0;h < 3;h++)
				{
					if(h == 1)
					{
						m_LayoutLineInfo.p1[iCount].x = 0;
						m_LayoutLineInfo.p1[iCount].y = iHFactor * (h + 1);
						m_LayoutLineInfo.p2[iCount].x = iWFactor;
						m_LayoutLineInfo.p2[iCount].y = iHFactor * (h + 1);
						iCount++;
						m_LayoutLineInfo.p1[iCount].x = iWFactor * 3;
						m_LayoutLineInfo.p1[iCount].y = iHFactor * (h + 1);
						m_LayoutLineInfo.p2[iCount].x = m_iWidth;
						m_LayoutLineInfo.p2[iCount].y = iHFactor * (h + 1);
					}
					else
					{
						m_LayoutLineInfo.p1[iCount].x = 0;
						m_LayoutLineInfo.p1[iCount].y = iHFactor * (h + 1);
						m_LayoutLineInfo.p2[iCount].x = m_iWidth;
						m_LayoutLineInfo.p2[iCount].y = iHFactor * (h + 1);
					}
					iCount++;
				}
			}
			break;
			*/
	}
}

void LayoutCtrl::MakeLayoutLineInfo(int iCellW,int iCellH)
{
	int iHFactor = m_iHeight / iCellH;
	int iWFactor = m_iWidth / iCellW;

	int iVLines = iCellW - 1;
	int iHLines = iCellH - 1;

	switch(m_iType)
	{
		case LT_1x1:
			m_LayoutLineInfo.iTotal = 0;
			break;
		case LT_2x2:
		case LT_3x3:
		case LT_4x4:
			/*
		case LT_5x5:
		case LT_6x6:
		case LT_8x8:
		*/
		case LT_3x2:
		case LT_4x3:
		case LT_5x3:
		//case LT_8x6:
			{
				m_LayoutLineInfo.iTotal = iVLines + iHLines;

				int iCount = 0;
				for(int v = 0;v < iVLines;v++)
				{
					m_LayoutLineInfo.p1[iCount].x = iWFactor * (v + 1);
					m_LayoutLineInfo.p1[iCount].y = 0;
					m_LayoutLineInfo.p2[iCount].x = iWFactor * (v + 1);
					m_LayoutLineInfo.p2[iCount].y = m_iHeight;	
					iCount++;
				}

				for(int h = 0;h < iHLines;h++)
				{
					m_LayoutLineInfo.p1[iCount].x = 0;
					m_LayoutLineInfo.p1[iCount].y = iHFactor * (h + 1);
					m_LayoutLineInfo.p2[iCount].x = m_iWidth;
					m_LayoutLineInfo.p2[iCount].y = iHFactor * (h + 1);
					iCount++;
				}
			}
			break;

	}
}

LayoutLineInfo* LayoutCtrl::GetLayoutLineInfo()
{
	return &m_LayoutLineInfo;
}
