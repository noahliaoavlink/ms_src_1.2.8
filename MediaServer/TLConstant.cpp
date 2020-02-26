/********************************************************************\
Project Name: Timeline Prototype

File Name: TConstant.cpp

Definition of following classes:
TLFont

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


// ==== Include local library ==== //
#include "TLUtility.h"
#include "TLConstant.h"

using namespace Timeline;

/* ====================================================== *\
TLFont
\* ====================================================== */
TLFont* TLFont::m_this = nullptr;

TLFont::TLFont()
{
}

TLFont::~TLFont()
{

}

TLFont& TLFont::Instance()
{
	if (m_this == nullptr)
	{
		m_this = new TLFont();
	}
	return *m_this;
}

Gdiplus::Font* TLFont::GetFont(int size)
{
	if (m_fonts.find(size) == m_fonts.end())
	{
		//auto font = std::make_shared<Gdiplus::Font>(Timeline::towstring("Arial").c_str(), size, FontStyleRegular, UnitPoint, NULL);
		auto font = std::make_shared<Gdiplus::Font>(Timeline::towstring("Arial").c_str(), size);
		m_fonts.insert(std::make_pair(size, font));
	}
	return m_fonts[size].get();
}

