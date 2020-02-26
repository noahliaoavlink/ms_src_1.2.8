/********************************************************************\
Project Name: Timeline Prototype

File Name: DoubleBuffer.h

Definition of following classes:
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

// ==== Inlcude Microsoft library ==== //
#include "stdafx.h"

// ==== Include local library ==== //
#include "DoubleBuffer.h"

using namespace Timeline;


DoubleBuffer::DoubleBuffer(CDC* dc, CWnd* wnd)
{
	CRect rect;
	wnd->GetClientRect(&rect);
	_onScrrenDc = dc;
	_rect.SetRect(rect.left, rect.top, rect.right, rect.bottom);
	_isReleased = false;
}

DoubleBuffer::DoubleBuffer(CDC* dc, CRect rect)
{
	_onScrrenDc = dc;
	_rect.SetRect(rect.left, rect.top, rect.right, rect.bottom);
	_isReleased = false;
}

DoubleBuffer::~DoubleBuffer()
{
	if (!_isReleased)
	{
		ReleaseBackBuffer();
	}
}

CDC* DoubleBuffer::InitBackBuffer()
{
	/* Create the offscreen DC and associated bitmap */

	_offScreenDc.CreateCompatibleDC(_onScrrenDc);
	_bmBack.CreateCompatibleBitmap(_onScrrenDc, _rect.Width(),
		_rect.Height());

	_pOldBitmap = _offScreenDc.SelectObject(&_bmBack);
	return &_offScreenDc;
}

void DoubleBuffer::ReleaseBackBuffer()
{
	/* Copy the offscreen buffer to the onscreen CPaintDC.
	Then free the back buffer objects. */

	_onScrrenDc->BitBlt(_rect.left, _rect.top,
		_rect.Width(), _rect.Height(),
		&_offScreenDc, _rect.left, _rect.top,
		SRCCOPY);

	_onScrrenDc->SelectObject(_pOldBitmap);

	_bmBack.DeleteObject();
	_offScreenDc.DeleteDC();
	_isReleased = true;
}

