#include "MGFont.h"
#include "MGColor.h"
#include "MGPlatformFont.h"
#include "FreeTypeFont.h"
#include "../System/Shared/MGLog.h"

#if defined(_WIN32) || defined(_WIN64)
#define UNICODE_SAMPLE ("\xD6\xD0/Ajk1") //("中/Ajk1") using GB2312
#else
#define UNICODE_SAMPLE ("\xE4\xB8\xAD/Ajk1") //("中/Ajk1") using UTF8
#endif

namespace z11
{
    string MGFont::default_font_path;
	MGFont::MGFont(  int size,const RGBAColorRef& font_color,const std::string& font_type  )
		:m_size(size),
		m_font_color(font_color),
		m_font_type(font_type)
	{
		m_platform_font=MGPlatformFont::create(m_size,m_font_color,m_font_type);
	}

	MGFont::~MGFont( void )
	{
		delete_and_set_null(m_platform_font);
	}

	MGFont * MGFont::create( int size )
	{
		return create(size,RGBAColorRef::white(),"MobileDefaultFont");
	}

	MGFont * MGFont::create( int size,const RGBAColorRef& font_color,const std::string& font_type )
	{

		return new MGFont(size,font_color,font_type);
	}

	MGFont * MGFont::create( int size,const RGBAColorRef& font_color )
	{
		return create(size,font_color,"MobileDefaultFont");
	}

	RGBAColorRef MGFont::getColor() const
	{
		return m_font_color;
	}

	void MGFont::setColor(const RGBAColorRef &font_color)
	{
		m_font_color = font_color;
	}

	int MGFont::getHeight() const
	{
		MGSize size(getStringWidthAndHeight(UNICODE_SAMPLE));
		return size.height;
	}

	uint_32 MGFont::getFontSize() const
	{
		return m_size;
	}

	MGSize MGFont::getStringWidthAndHeight( const std::string& text ) const
	{
		return m_platform_font->getStringWidthAndHeight(text);
	}

	z11::MGSize MGFont::getStringWidthAndHeight( const std::string& text,float width )
	{
		return m_platform_font->getStringWidthAndHeight(text,width);
	}

	const FONT MGFont::getNativeFont() const
	{
		return m_platform_font->getNativeFont();
	}


	z11::MGSize MGFont::getCharWidthAndHeight( wchar_t ch ) const
	{
		return m_platform_font->getCharWidthAndHeight(ch);
	}

	void MGFont::reload() const
	{
		
	}

	MGFont * MGFont::shareFont()
	{
		static MGFont *shared_font=MGFont::create(20,RGBAColorRef::black());
		return shared_font;
	}

	MGPlatformFont * MGFont::getPlatformFont() const
	{
		return m_platform_font;
	}

	void MGFont::render( const string &_text, const MGRect& _rect,MGImage *image, MILineBreakMode mode, MITextAlignment text_alignment , RGBAColorRef blend ) const
	{
		m_platform_font->render(_text,this,_rect,image,mode,text_alignment,blend);
	}

	void MGFont::setFaceSize( size_t size )
	{
		m_size=size;
		m_platform_font->setFaceSize(m_size);
	}

	z11::MGRect MGFont::getStringBound( const std::string& text )
	{
		return m_platform_font->getStringRectForDrawingWidth(text);
	}

	z11::MGRect MGFont::getStringBoundForWidth( const std::string& text,float width )
	{
		return m_platform_font->getStringRectForDrawingWidth(text, width);
	}

	vector<MGRect> MGFont::BBoxVector( const std::string &text,float width )
	{
		return m_platform_font->BBoxVector(text,width);
	}

}