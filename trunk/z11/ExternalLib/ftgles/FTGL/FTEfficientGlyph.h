/*
* Efficient glyph for graphic engine
 */

#ifndef __ftgl__
#   warning This header is deprecated. Please use <FTGL/ftgl.h> from now.
#   include <FTGL/ftgl.h>
#endif

#ifndef __FTEfficientGlyph__
#define __FTEfficientGlyph__

#ifdef __cplusplus

namespace z11
{
	class MGImage;
}
class FTEfficientFontImpl;
/**
 * FTEfficientGlyph is a specialisation of FTGlyph for creating texture
 * glyphs.
 */
class FTGL_EXPORT FTEfficientGlyph : public FTGlyph
{
    public:
        /**
         * Constructor
         *
         * @param glyph     The Freetype glyph to be processed
         * @param id        The id of the texture that this glyph will be
         *                  drawn in
         * @param xOffset   The x offset into the parent texture to draw
         *                  this glyph
         * @param yOffset   The y offset into the parent texture to draw
         *                  this glyph
         * @param width     The width of the parent texture
         * @param height    The height (number of rows) of the parent texture
         */
        FTEfficientGlyph(FT_GlyphSlot glyph, int id, int xOffset, int yOffset,
                       int width, int height,FTEfficientFontImpl * const impl=NULL);

        /**
         * Destructor
         */
        virtual ~FTEfficientGlyph();

        /**
         * Render this glyph at the current pen position.
         *
         * @param pen  The current pen position.
         * @param renderMode  Render mode to display
         * @return  The advance distance for this glyph.
         */
        virtual const FTPoint& Render(const FTPoint& pen, int renderMode);
};

#endif //__cplusplus

FTGL_BEGIN_C_DECLS

/**
 * Create a specialisation of FTGLglyph for creating pixmaps.
 *
 * @param glyph The Freetype glyph to be processed.
 * @param id  The id of the texture that this glyph will be drawn in.
 * @param xOffset  The x offset into the parent texture to draw this glyph.
 * @param yOffset  The y offset into the parent texture to draw this glyph.
 * @param width  The width of the parent texture.
 * @param height  The height (number of rows) of the parent texture.
 * @return  An FTGLglyph* object.
 */
FTGL_EXPORT FTGLglyph *ftglCreateEfficientGlyph(FT_GlyphSlot glyph, int id,
                                              int xOffset, int yOffset,
                                              int width, int height);

FTGL_END_C_DECLS

#endif  //  __FTEfficientGlyph__

