//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (MGFont.h)
/// @brief (字体类)
///
///(字体类，根据大小、颜色、字体名称创建所需的字体)
///
/// @version 0.1   (版本声明)
/// @author        (Gu RongFang)
/// @date          (11-07-22)
///
/// 修订说明：最初版本
///
/// @version 0.2   (版本声明)
/// @author        (Wang Zhipeng)
/// @date          (13-01-29)
///
/// 修订说明：完善注释
//////////////////////////////////////////////////////////////////////////
#ifndef _MGFONT_H_
#define _MGFONT_H_
#pragma once
#include "../CommonBase/MGStructure.h"
#include "GraphicStructure.h"
#include <string>
#include <vector>
typedef struct FT_FaceRec_*  FT_Face;
typedef FT_Face		FONT;

#include "MGGraphicEngine.h"
namespace z11
{
	class MGImage ;
	class MGColor;
	class MGPlatformFont;
	typedef uint_32 MILineBreakMode;
	typedef uint_32 MITextAlignment;

	/************************************************************************/
	/* MGFont是根据大小、颜色、字体名称创建所需的字体                              */
	/************************************************************************/
	class MGFont
	{
	public:
		~MGFont(void);

	public:
		//创建字体
		static MGFont *create(int size);
		static MGFont *create(int size,const RGBAColorRef& font_color);
		static MGFont *create(int size,const RGBAColorRef& font_color,const std::string& font_type);
		static MGFont *shareFont();
        static void setDefaultFontPath(const string font_path) { default_font_path = font_path; }
        static const string & getDefaultFontPath() { return default_font_path; }
		//获取字体颜色
		RGBAColorRef getColor() const;

		//设置字体颜色
		void setColor(const RGBAColorRef &font_color);

		// 返回显示文本所需的水平像素.
		MGSize getStringWidthAndHeight(const std::string& text) const;
        // 返回指定宽度条件下，自动换行后的显示文本需要的矩形大小
		MGSize getStringWidthAndHeight(const std::string& text,float width);
        // 返回指定文本显示需要的矩形框大小
		MGRect getStringBound(const std::string& text);
        // 返回指定文本在指定宽度下自动换行显示需要的矩形框大小
		MGRect getStringBoundForWidth(const std::string& text,float width);
        // 返回指定文本在指定宽度下自动换行显示需要的每行矩形框大小集合
		std::vector<MGRect> BBoxVector(const std::string &text,float width);
        // 返回指定字符显示需要的矩形大小
		MGSize getCharWidthAndHeight(wchar_t ch) const;

		//获取字体句柄
		const FONT getNativeFont() const;

		//获取字体大小
		uint_32 getFontSize() const;
		// 慎用,由于只有FTGLES才可以设置字体大小  //
		void setFaceSize(size_t size);

		//重载字体，这是freetype特有问题，就是必须绘制时构造字体！！！      
		void reload() const ;

		MGPlatformFont *getPlatformFont() const;
		void render(const std::string &_text,  const MGRect& _rect,MGImage *cachedImage=NULL,MILineBreakMode mode = MILineBreakModeClip, MITextAlignment text_alignment=MITextAlignmentLeftMiddle, RGBAColorRef blend = RGBAColorMake(255,255,255,255)) const;
	
    private:
        static string default_font_path;
		uint_32 m_size;
		RGBAColorRef m_font_color;
		std::string m_font_type;
		mutable MGPlatformFont *m_platform_font;

	private:
		// 返回显示字符所需的垂直像素. 包含行距, 就是说高度可能比ascent+descent大.
		int getHeight() const;
		//私有构造函数
		MGFont( int size,const RGBAColorRef& font_color,const std::string& font_type );
	};

}

#endif //_MGFONT_H_
