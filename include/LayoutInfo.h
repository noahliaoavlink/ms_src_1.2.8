#ifndef _LayoutInfo_H
#define _LayoutInfo_H

enum LayoutType
{
	LT_Unknow = -1,
	LT_1x1 = 1,
	LT_2x2,
	LT_3x3,
	LT_4x4,
	LT_3x2,
	LT_4x3,
	LT_5x3,
};

typedef struct
{
	int iIndex;
	int iID;
	char szText[20];
}LayoutItem;

static LayoutItem g_LayoutItems [] = 
{
	{0,LT_1x1,"1x1"},
	{1,LT_2x2,"2x2"},
	{2,LT_3x3,"3x3"},
	{3,LT_4x4,"4x4"},
	{4,LT_3x2,"3x2"},
	{5,LT_4x3,"4x3"},
	{6,LT_5x3,"5x3"},
};

static int g_iTotalOfLayoutItems = sizeof(g_LayoutItems)/sizeof(LayoutItem);


#endif