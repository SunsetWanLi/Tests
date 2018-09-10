#pragma once
#include "GraphicStructure.h"
#include "MGPlatformFont.h"
#include "../System/Thread/MGMutex.h"
#include <map>
class FTFont;
class FTCustomLayout;
class FTSimpleLayout;
class FTBBox;
namespace z11
{
	class MGFTGLESFontContextManager;
	class MGImage;
	class MGFTGLESFontIndex
	{
	public:
		int size;
		std::string name;
        float rect_width;

		bool operator <(const MGFTGLESFontIndex &rhs) const
		{
			if (size!=rhs.size)
			{
				return size<rhs.size;
			}
			else if (name != rhs.name)
			{
				return name < rhs.name;
			}else{
                return rect_width < rhs.rect_width;
            }
		}

	};
	class MGFTGLESFont:public MGPlatformFont
	{
	public:
		virtual void setFaceSize(size_t size);
		size_t getFaceSize();
		MGFTGLESFont(const std::string &font_type);
		virtual ~MGFTGLESFont(void);
		//获取文字高度和宽度，有子类相应函数完成  
		virtual MGSize getCharWidthAndHeight(wchar_t ch) const;
		virtual MGSize getStringWidthAndHeight(const std::string& text,float width = 999999);
        virtual MGRect getStringRectForDrawingWidth(const std::string& text,float width = 999999);
		virtual std::vector<MGRect> BBoxVector(const std::string &text,float width) const;
		virtual FONT getNativeFont() const;
		FTFont *getNativeFont();
		virtual void render(const std::string &_text,const z11::MGFont * _font,  const MGRect& _rect,MGImage *cachedImage=NULL,MILineBreakMode mode = MILineBreakModeClip, MITextAlignment text_alignment=MITextAlignmentLeftMiddle, RGBAColorRef blend = RGBAColorMake(255,255,255,255));
		
		static MGMutex image_locker;
		static std::map<MGFTGLESFontIndex,FTBBox> font2bbox;
		typedef std::map<MGFTGLESFontIndex,FTBBox>::iterator MapIterator;
		static void recoveryContext();
        static void cleanCache();

		static std::map<MGFTGLESFontIndex,MGImage *> font2texture;
		typedef std::map<MGFTGLESFontIndex,MGImage *>::iterator TextureIterator;
	private:
	private:
		size_t m_size;
		RGBAColorRef m_font_color;
		std::string m_font_type;
		static FTFont* m_font;
		static FTCustomLayout* layouts;
		static MGMutex layout_locker;

		void init(const std::string &font_type);
		friend class MGFTGLESFontContextManager;
	};
}
