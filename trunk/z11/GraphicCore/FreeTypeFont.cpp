#include "FreeTypeFont.h"
#include "FreeTypeDefinition.h"
#include "../System/IO/MGBundle.h"
#include "../System/Shared/MGLog.h"
#include "../System/Shared/Utility.h"
#include <math.h>
#include "MGFont.h"
#include <stack>
#include "../System/IO/MGFileManager.h"
#include "../Platform/MGFileUtils.h"
using std::string;
using std::stack;
static /*const*/
FT_Matrix	g_Plane_Matrix= {(FT_Fixed)( 1 * 0x10000L ),
(FT_Fixed)(0 * 0x10000L ),
(FT_Fixed)( 0 * 0x10000L ),
(FT_Fixed)(1 * 0x10000L )};
static /*const*/	FT_Vector g_Plane_Vector= {0,0};
#define  RGB16(r, g, b)   (((z11::uint_16)r>>3<<11)|((z11::uint_16)g>>2<<5)|((z11::uint_16)b>>3))
namespace z11
{

	void FTFontBitmap::dispose()
	{
		if (!empty())
		{
			FT_Done_Glyph(glyph);
			glyph=NULL;
		}
	}

	FT_Bitmap* FTFontBitmap::getBitmap()
	{
		if (!empty())
		{
			FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
			return &bitmap_glyph->bitmap;
		}
		return NULL;
	}

	float FTFontBitmap::getAdapterHeight()
	{
		//if (!empty())
		//{
		//	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
		//	if ( bitmap_glyph->top>bitmap_glyph->bitmap.rows)
		//	{
		//		return bitmap_glyph->top;
		//	}
		//	else
		//	{
		//		return bitmap_glyph->bitmap.rows;
		//	}
		//}
		//else
		//	return 0.0f;
		if (!empty())
		{
			FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
			return bitmap_glyph->bitmap.rows+getTop();
		}
		else
			return 0.0f;
	}

	float FTFontBitmap::getWidth()
	{
		return pt.width;
	}

}
