#include "config.h"

#include <math.h>

#include "FTGL/ftgles.h"

#include "FTInternals.h"
#include "FTEfficientGlyphImpl.h"

#include "../FTFont/FTEfficientFontImpl.h"

#include "../../../GraphicCore/MGPersistenceImage.h"
using namespace z11;


//
//  FTGLTextureGlyph
//


FTEfficientGlyph::FTEfficientGlyph(FT_GlyphSlot glyph, int id, int xOffset,
                               int yOffset, int width, int height,FTEfficientFontImpl *impl) :
    FTGlyph(new FTEfficientGlyphImpl(glyph, id, xOffset, yOffset, width, height,impl))
{}


FTEfficientGlyph::~FTEfficientGlyph()
{}


const FTPoint& FTEfficientGlyph::Render(const FTPoint& pen, int renderMode)
{
    FTEfficientGlyphImpl *myimpl = dynamic_cast<FTEfficientGlyphImpl *>(impl);
    return myimpl->RenderImpl(pen, renderMode);
}


//
//  FTGLTextureGlyphImpl
//


FTEfficientGlyphImpl::FTEfficientGlyphImpl(FT_GlyphSlot glyph, int id, int xOffset,
                                       int yOffset, int width, int height,FTEfficientFontImpl *impl)
:   FTGlyphImpl(glyph),
	m_font_impl(impl)
{
    /* FIXME: need to propagate the render mode all the way down to
     * here in order to get FT_RENDER_MODE_MONO aliased fonts.
     */

    err = FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);
    if(err || glyph->format != ft_glyph_format_bitmap)
    {
        return;
    }

    corner = FTPoint(glyph->bitmap_left, glyph->bitmap_top);

	//GRF
	if(true)
	{
		FT_Glyph _glyph=NULL;
		FT_Get_Glyph( glyph, &_glyph );
		FT_Glyph_To_Bitmap( &_glyph, FT_RENDER_MODE_NORMAL, 0, 1 );
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)_glyph;
		bitmap = bitmap_glyph->bitmap;
		bitmap.buffer=new unsigned char[bitmap.pitch*bitmap.rows];
		memcpy(bitmap.buffer,bitmap_glyph->bitmap.buffer,bitmap.pitch*bitmap.rows);
		FT_Done_Glyph(_glyph);

	}
}


FTEfficientGlyphImpl::~FTEfficientGlyphImpl()
{
	delete_and_set_null(bitmap.buffer);
}

void AlphaBlend565(uint_16 *p,byte_8 r,byte_8 g,byte_8 b,byte_8 a) 
{
	if (a>5)
	{
		unsigned int x=*p;
		//RRRRRGGGGGGBBBBB==>>00000GGGGG00000RRRRR000000BBBBB
		x = (x | (x <<16)) & 0x07E0F81F;
		unsigned int y=(((uint_32)r>>3<<11)|((uint_32)g>>2<<21)|((uint_32)b>>3));
		y=((((y-x)*(a>>3))>>5)+x)&0x07E0F81F;//y as result
		//00000GGGGG00000RRRRR000000BBBBB==>>RRRRRGGGGGGBBBBB
		*p= (uint_16)((y&0xFFFF)|(y>>16));
	}
}
const FTPoint& FTEfficientGlyphImpl::RenderImpl(const FTPoint& pen,
                                              int renderMode)
{
    float dx, dy;
	
    dx = floor(pen.Xf() + corner.Xf());
	dy = floor(pen.Yf() + corner.Yf());
	
	if(m_font_impl!=NULL)
	{
		MGImage *m_buffer_image=m_font_impl->m_buffer_image;
		if (m_buffer_image!=NULL)
		{
			if (m_buffer_image->content_info.format==PF_RGBA0008)
			{

				//MGLogD("pen=(%lf,%lf) corner=(%lf,%lf) size=(%d,%d) pitch=%d",pen.Xf(),pen.Yf(),corner.Xf(),corner.Yf(),bitmap.width,bitmap.rows,bitmap.pitch);
				//MGLogD("image_height=%d char_size=%d",m_buffer_image->getHeight(),m_font_impl->charSize.CharSize());
				byte_8 *data=(byte_8 *)m_buffer_image->getContentData();
                if(data != NULL)
                {
                    size_t width=m_buffer_image->getAdapterWidth();
                    byte_8 *src=bitmap.buffer;
                    for (int j=0;j<bitmap.rows;j++)
                    {
                        int yoffset=dy-j+m_buffer_image->getHeight()-m_font_impl->charSize.CharSize();
                        if (yoffset>=0&&yoffset<m_buffer_image->getHeight())
                        {
                            byte_8 *dest=data+(int)(yoffset)*width+int(dx+0.5);
                            
                            for (int i=0;i<bitmap.width;i++)
                            {
                                //RGBAColorRef ref(255,0,0,src[i]);
                                //dest[i]=ref.convertToARGB();
                                dest[i]=src[i];
                            }
                        } else {
                            //MGLogD("Cannot Draw Text Here!");
                        }
                        
                        src+=bitmap.pitch;
                    }
                }
			}
			else if (m_buffer_image->content_info.format==PF_RGBA5650)
			{
				//return advance;
				uint_16 *data=(uint_16 *)m_buffer_image->getContentData();
                if(data != NULL)
                {
                    size_t width=m_buffer_image->getAdapterWidth();
                    byte_8 *src=bitmap.buffer;//+bitmap.pitch*(bitmap.rows-1);
                    for (int j=0;j<bitmap.rows;j++)
                    {
                        int yoffset=(int)dy-j+m_buffer_image->getHeight();
                        if (yoffset>=0&&yoffset<m_buffer_image->getHeight())
                        {
                            uint_16 *dest=data+(int)(m_buffer_image->getHeight()-yoffset-1)*width+int(dx+0.5);
                            int xoffset=MINVALUE(bitmap.width,m_buffer_image->getWidth()-(int)dx-1);
                            for (int i=0;i<xoffset;i++)
                            {
                                //RGBAColorRef ref(255,0,0,src[i]);
                                //dest[i]=ref.convertToARGB();
                                int a=src[i];
                                //dest[i]=(a>>3<<11)|(a>>2<<5)|(a>>3);
                                AlphaBlend565(dest+i,0,0,0,a);
                            }
                        }
                        
                        src+=bitmap.pitch;
                    }
                }
				
			}
			
		}
		

	}
	
    return advance;
}

