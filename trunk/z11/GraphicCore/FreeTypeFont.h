//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (FreeTypeFont.h)  
/// @brief (freetype字体类)  
///  
///(freetype字体类，基于freetype实现)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-07-22)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _FREETYPEFONT_H_
#define _FREETYPEFONT_H_
#pragma once
typedef struct FT_FaceRec_ *FONT;
typedef struct  FT_Bitmap_ FT_Bitmap;
typedef struct FT_GlyphRec_*  FT_Glyph;

#if defined(_WIN32) || defined(_WIN64)	
typedef unsigned short wchar_t;
#endif

#include "../CommonBase/MGStructure.h"
#include "GraphicStructure.h"
#include "MGPlatformFont.h"
#include <string>
#include <vector>
namespace z11
{
	class MGFont;
	//以ahjy为例子,j和y有adapterHeight，h和j的top最高   
	class  FTFontBitmap 
	{
	public:
		FTFontBitmap(MGRect _pt,FT_Glyph _glyph)
			:pt(_pt),glyph(_glyph)
		{

		}
		FTFontBitmap()
		{
			glyph=NULL;
		}
		bool empty()
		{
			return glyph==NULL;
		}
		const MGRect &getRect()
		{
			return pt;
		}
		void setTop(float top)
		{
			pt.y=top;
		}
		float getTop()
		{
			return pt.y;
		}
		float getLeft()
		{
			return pt.x;
		}
		void setLeft(float left)
		{
			pt.x=left;
		}
		float getAdapterHeight();
		float getWidth();
		void dispose();
		FT_Bitmap* getBitmap();
	private:
		MGRect pt;
		FT_Glyph glyph;
	};
}


#endif //_FREETYPEFONT_H_
