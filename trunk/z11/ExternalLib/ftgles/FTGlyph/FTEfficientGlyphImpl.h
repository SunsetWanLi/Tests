/*
* it's for GraphicEngine
 */

#ifndef __FTEfficientGlyphImpl__
#define __FTEfficientGlyphImpl__

#include "FTGlyphImpl.h"
class FTEfficientGlyphImpl : public FTGlyphImpl
{
    friend class FTEfficientGlyph;
    friend class FTEfficientFontImpl;

    protected:
        FTEfficientGlyphImpl(FT_GlyphSlot glyph, int id, int xOffset,
                           int yOffset, int width, int height,FTEfficientFontImpl *impl);

        virtual ~FTEfficientGlyphImpl();

        virtual const FTPoint& RenderImpl(const FTPoint& pen, int renderMode);

    private:
        /**
         * Vector from the pen position to the topleft corner of the pixmap
         */
        FTPoint corner;
	
	//extra data field
	public:
		FT_Bitmap      bitmap;
		FTEfficientFontImpl *m_font_impl;
};

#endif  //  __FTEfficientGlyphImpl__

