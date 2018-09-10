//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGPlatformFont.h)  
/// @brief (平台字体类)  
///  
///(平台字体类，字体类的基类)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-07-22)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MGPLATFORMFONT_H_
#define _MGPLATFORMFONT_H_
#pragma once

typedef struct FT_FaceRec_*  FT_Face;
typedef FT_Face		FONT;
#include "../CommonBase/CommonDef.h"
#include "../CommonBase/MGStructure.h"
#include "GraphicStructure.h"
#include <string>
#include <vector>
#include "MGGraphicEngine.h"
namespace z11
{
	class MGImage ;
	class MGSize;
	class RGBAColorRef;
	typedef uint_32 MILineBreakMode;
	typedef uint_32 MITextAlignment;
	class MGFont;
	class MGPlatformFont
	{
	public:
		virtual ~MGPlatformFont(void);

	public:
		//Create an appropriate MGPlatformFont implementation.
		static MGPlatformFont *create(int size,const RGBAColorRef& font_color,const std::string& font_type);

		//获取文字高度和宽度，有子类相应函数完成  //
		virtual MGSize getCharWidthAndHeight(wchar_t ch) const=0;
		virtual MGSize getStringWidthAndHeight(const std::string& text,float width = 999999);
        virtual MGRect getStringRectForDrawingWidth(const std::string& text,float width = 999999);
		virtual std::vector<MGRect> BBoxVector(const std::string &text,float width) const;
		virtual void setFaceSize(size_t m_size);
		//获取句柄  
		virtual FONT getNativeFont() const=0;
		virtual void render(const std::string &_text,const z11::MGFont * _font,  const MGRect& _rect,MGImage *cachedImage=NULL,MILineBreakMode mode = MILineBreakModeClip, MITextAlignment text_alignment=MITextAlignmentLeftMiddle, RGBAColorRef blend = RGBAColorMake(255,255,255,255))=0;
	
	protected:
		MGPlatformFont(void);
	};
}
#endif //_MGPLATFORMFONT_H_
