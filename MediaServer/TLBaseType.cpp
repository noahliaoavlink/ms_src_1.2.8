/********************************************************************\
Project Name: Timeline Prototype

File Name: TLBaseType.cpp

Definition of following classes:
TLThickness
TLGeometricTransform

Copyright:
Media Server
(C) Copyright C&C TECHNIC TAIWAN CO., LTD.
All rights reserved.

Author:
Stone Chang, mail: stonechang.cctch@gmail.com

Other issue:
N/A

\********************************************************************/

// ==== Inlcude Microsoft MFC library ==== //
#include "stdafx.h"

// ==== Inlcude STL library ==== //
#include <limits>
#include <sstream>

// ==== Include local library ==== //
#include "TLBaseType.h"

using namespace Timeline;

/* ====================================================== *\
          TLThickness
\* ====================================================== */

TLThickness::TLThickness()
{
	Bottom = 0.0f;
	Left = 0.0f;
	Right = 0.0f;
	Top = 0.0f;
}

TLThickness::TLThickness(float thickness)
{
	Bottom = thickness;
	Left = thickness;
	Right = thickness;
	Top = thickness;
}

TLThickness::TLThickness(float left, float top, float right, float bottom)
{
	Bottom = bottom;
	Left = left;
	Right = right;
	Top = top;
}

TLThickness::~TLThickness()
{
}

/* ====================================================== *\
         TLGeometricTransform
\* ====================================================== */

void TLGeometricTransform::Translate(Gdiplus::Point& pt, float x, float y)
{
	pt.X = pt.X + x;
	pt.Y = pt.Y + y;
}

void TLGeometricTransform::Translate(Gdiplus::Rect& rect, float x, float y)
{
	rect.X = rect.X + x;
	rect.Y = rect.Y + y;
}

