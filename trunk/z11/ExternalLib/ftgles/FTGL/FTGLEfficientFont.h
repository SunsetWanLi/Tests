/*
* Efficient glyph for graphic engine
*/


#ifndef __ftgl__
#   warning This header is deprecated. Please use <FTGL/ftgl.h> from now.
#   include <FTGL/ftgl.h>
#endif

#ifndef __FTEfficientFont__
#define __FTEfficientFont__

#ifdef __cplusplus

namespace z11
{
	class MGImage;
}
/**
 * FTEfficientFont is a specialisation of the FTFont class for handling
 * Texture mapped fonts
 *
 * @see     FTFont
 */
class  FTGL_EXPORT FTEfficientFont : public FTFont
{
    public:
        /**
         * Open and read a font file. Sets Error flag.
         *
         * @param fontFilePath  font file path.
         * @param fontFilePath  font type name.
         */
        FTEfficientFont(const char* fontFilePath, const char* fontType);

        /**
         * Open and read a font from a buffer in memory. Sets Error flag.
         * The buffer is owned by the client and is NOT copied by FTGL. The
         * pointer must be valid while using FTGL.
         *
         * @param pBufferBytes  the in-memory buffer
         * @param bufferSizeInBytes  the length of the buffer in bytes
         */
        FTEfficientFont(const unsigned char *pBufferBytes,
                      size_t bufferSizeInBytes);

        /**
         * Destructor
         */
        virtual ~FTEfficientFont();

		void setCanvas(z11::MGImage *image);
    protected:
        /**
         * Construct a glyph of the correct type.
         *
         * Clients must override the function and return their specialised
         * FTGlyph.
         *
         * @param slot  A FreeType glyph slot.
         * @return  An FT****Glyph or <code>null</code> on failure.
         */
        virtual FTGlyph* MakeGlyph(FT_GlyphSlot slot);
};

#define FTGLEfficientFont FTEfficientFont

#endif //__cplusplus

FTGL_BEGIN_C_DECLS

/**
 * Create a specialised FTGLfont object for handling texture-mapped fonts.
 *
 * @param file  The font file name.
 * @return  An FTGLfont* object.
 *
 * @see  FTGLfont
 */
FTGL_EXPORT FTGLfont *ftglCreateEfficientFont(const char *file);

FTGL_END_C_DECLS

#endif // __FTEfficientFont__

