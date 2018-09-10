#include "MGPlatformFont.h"
#include "FreeTypeFont.h"
#include "MGFTGLESFont.h"
#ifdef WIN32

#ifdef WINCE
#define USE_FTGLES 0
#else
#define USE_FTGLES 1

#endif
#elif defined (ANDROID_NDK)
#define USE_FTGLES 1
#elif defined (linux)
#define USE_FTGLES 0
#else //mac
#define USE_FTGLES 1
#endif
namespace z11
{
	MGPlatformFont * MGPlatformFont::create( int size,const RGBAColorRef& font_color,const std::string& font_type )
	{
#if USE_FTGLES
		MGFTGLESFont *font=new MGFTGLESFont(font_type);
		font->setFaceSize(size);
		return font;
#else
		return new FreeTypeFont(size/1.3,font_color,font_type);
#endif
	}

	MGPlatformFont::~MGPlatformFont( void )
	{

	}

	MGPlatformFont::MGPlatformFont( void )
	{

	}

	z11::MGSize MGPlatformFont::getStringWidthAndHeight( const std::string& text,float width )
	{
		static MGSize size;
		return size;
	}

	void MGPlatformFont::setFaceSize( size_t m_size )
	{
		//only for ftgles
	}

	vector<MGRect> MGPlatformFont::BBoxVector( const std::string &text,float width ) const
	{
		vector<MGRect> ret;
		return ret;
	}
    
    MGRect MGPlatformFont::getStringRectForDrawingWidth(const std::string& text,float width)
    {
        static MGRect rect;
        return rect;
    }

}
