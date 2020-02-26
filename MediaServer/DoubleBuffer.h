/********************************************************************\
Project Name: Timeline Prototype

File Name: DoubleBuffer.h

Declaration of following classes:
DoubleBuffer

Copyright:
Media Server
(C) Copyright C&C TECHNIC TAIWAN CO., LTD.
All rights reserved.

Author:
Stone Chang, mail: stonechang.cctch@gmail.com

Other issue:
N/A

\********************************************************************/

#pragma once

// ==== Inlcude Microsoft library ==== //
#include "stdafx.h"

// ==== Include local library ==== //
#include "TLDllExport.h"

namespace Timeline
{

/* ====================================================== *\
          DoubleBuffer
\* ====================================================== */
class TIMELINELIB_EXPORTS DoubleBuffer
{
public:
	DoubleBuffer(CDC* dc, CWnd* wnd);
	DoubleBuffer(CDC* dc, CRect rect);
	virtual ~DoubleBuffer();

	// Operations
public:
	CDC* InitBackBuffer();
	void ReleaseBackBuffer();

	//========== Private device context ========== //
private:
	CDC*       _onScrrenDc;
	CDC        _offScreenDc;

	CRect      _rect;
	CBitmap*   _pOldBitmap;  // Original bitmap
	CBitmap    _bmBack;      // Bitmap for backbuffering	

	bool _isReleased;
};

}