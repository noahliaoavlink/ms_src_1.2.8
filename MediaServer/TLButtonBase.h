/********************************************************************\
Project Name: Timeline Prototype

File Name: TLButtonBase.h

Declaration of following classes:
TLButtonBase

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
// ==== Inlcude STL library ==== //
#include <memory>
#include <vector>

// ==== Include local library ==== //
#include "TLDllExport.h"
#include "TLBaseType.h"
#include "TLShape.h"

namespace Timeline
{

	/* ====================================================== *\
	TLButtonBase
	\* ====================================================== */
	class TIMELINELIB_EXPORTS TLButtonBase : public TLShape
	{
		// ==== Constructor & Destructor ==== //
	public:
		TLButtonBase();
		~TLButtonBase();

		// ==== Public member methods ==== //
	public:
		void SetImage(const std::string& filename);
		void Draw(Gdiplus::Graphics& graphics);

		//========== Protected member fields ========== //
	protected:
		bool m_hasImage;
		std::shared_ptr<Gdiplus::Image> m_image;
	};

	/* ====================================================== *\
	TLButtonBase2
	\* ====================================================== */
	class TIMELINELIB_EXPORTS TLBmpButtonBase : public TLShape
	{
		// ==== Constructor & Destructor ==== //
	public:
		TLBmpButtonBase();
		~TLBmpButtonBase();

		// ==== Public member methods ==== //
	public:
		void SetImage(int iIndex , const std::string& filename);
		void Draw(Gdiplus::Graphics& graphics);
		void DrawActive(Gdiplus::Graphics& graphics);
		void DrawDown(Gdiplus::Graphics& graphics);
		void DrawNormal(Gdiplus::Graphics& graphics);

		//========== Protected member fields ========== //
	protected:
		//bool m_hasImage;
		std::shared_ptr<Gdiplus::Image> m_normal_image;
		std::shared_ptr<Gdiplus::Image> m_active_image;
		std::shared_ptr<Gdiplus::Image> m_down_image;
		std::shared_ptr<Gdiplus::Image> m_disable_image;
	};
}