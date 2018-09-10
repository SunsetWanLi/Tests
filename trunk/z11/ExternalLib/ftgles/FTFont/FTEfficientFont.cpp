/*
 * FTGL - OpenGL font library
 *
 * Copyright (c) 2001-2004 Henry Maddocks <ftgl@opengl.geek.nz>
 * Copyright (c) 2008 Sam Hocevar <sam@zoy.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "config.h"

#include <cassert>
#include <string> // For memset

#include "FTGL/ftgles.h"

#include "FTInternals.h"

#include "../FTGlyph/FTEfficientGlyphImpl.h"
#include "./FTEfficientFontImpl.h"

#include "../../../System/Shared/MGLog.h"
#include "../../../GraphicCore/MGPersistenceImage.h"
#include "../../../GraphicCore/MGPNGCodec.h"
#include "../../../GraphicCore/MGPPMCodec.h"
#include "../../../GraphicCore/MGFTGLESFont.h"
#include "../../../System/Shared/Utility.h"
using namespace z11;

//
//  FTEfficientFont
//


FTEfficientFont::FTEfficientFont(const char *fontFilePath, const char *fontType) :
    FTFont(new FTEfficientFontImpl(this, fontFilePath, fontType))
{}


FTEfficientFont::FTEfficientFont(const unsigned char *pBufferBytes,
                             size_t bufferSizeInBytes) :
    FTFont(new FTEfficientFontImpl(this, pBufferBytes, bufferSizeInBytes))
{}


FTEfficientFont::~FTEfficientFont()
{}


FTGlyph* FTEfficientFont::MakeGlyph(FT_GlyphSlot ftGlyph)
{
    FTEfficientFontImpl *myimpl = dynamic_cast<FTEfficientFontImpl *>(impl);
    if(!myimpl)
    {
        return NULL;
    }

    return myimpl->MakeGlyphImpl(ftGlyph);
}

void FTEfficientFont::setCanvas(z11::MGImage *image)
{

	FTEfficientFontImpl *myimpl = dynamic_cast<FTEfficientFontImpl *>(impl);
	if(!myimpl)
	{
		return ;
	}

	 myimpl->m_buffer_image=image;
}

//
//  FTEfficientFontImpl
//


static inline GLuint NextPowerOf2(GLuint in)
{
     in -= 1;

     in |= in >> 16;
     in |= in >> 8;
     in |= in >> 4;
     in |= in >> 2;
     in |= in >> 1;

     return in + 1;
}


FTEfficientFontImpl::FTEfficientFontImpl(FTFont *ftFont, const char* fontFilePath, const char* fontType)
:   FTFontImpl(ftFont, fontFilePath, fontType),
    maximumGLTextureSize(0),
    textureWidth(0),
    textureHeight(0),
    glyphHeight(0),
    glyphWidth(0),
    padding(3),
    xOffset(0),
    yOffset(0)
{
    load_flags = FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP;
    remGlyphs = numGlyphs = face.GlyphCount();
	preRendered = false;

	//just add something
	m_buffer_image=NULL;//MGPersistenceImage::fromBlank(32,1024,1024);
	m_need_render_image=true;
}

FTEfficientFontImpl::FTEfficientFontImpl(FTFont *ftFont,
                                     const unsigned char *pBufferBytes,
                                     size_t bufferSizeInBytes)
:   FTFontImpl(ftFont, pBufferBytes, bufferSizeInBytes),
    maximumGLTextureSize(0),
    textureWidth(0),
    textureHeight(0),
    glyphHeight(0),
    glyphWidth(0),
    padding(3),
    xOffset(0),
    yOffset(0)
{
    load_flags = FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP;
    remGlyphs = numGlyphs = face.GlyphCount();

	//just add something
	m_buffer_image=NULL;//MGPersistenceImage::fromBlank(32,1024,1024);
	m_need_render_image=true;
}


FTEfficientFontImpl::~FTEfficientFontImpl()
{
    if(textureIDList.size())
    {
        glDeleteTextures((GLsizei)textureIDList.size(),
                         (const GLuint*)&textureIDList[0]);
    }
}


FTGlyph* FTEfficientFontImpl::MakeGlyphImpl(FT_GlyphSlot ftGlyph)
{
    glyphHeight = static_cast<int>(charSize.Height() + 0.5);
    glyphWidth = static_cast<int>(charSize.Width() + 0.5);

    if(glyphHeight < 1) glyphHeight = 1;
    if(glyphWidth < 1) glyphWidth = 1;

    if(textureIDList.empty())
    {
        textureIDList.push_back(CreateTexture());
        xOffset = yOffset = padding;
    }

    if(xOffset > (textureWidth - glyphWidth))
    {
        xOffset = padding;
        yOffset += glyphHeight;

        if(yOffset > (textureHeight - glyphHeight))
        {
            textureIDList.push_back(CreateTexture());
            yOffset = padding;
        }
    }

    FTEfficientGlyph* tempGlyph = new FTEfficientGlyph(ftGlyph, textureIDList[textureIDList.size() - 1],
                                                    xOffset, yOffset, textureWidth, textureHeight,this);
    xOffset += static_cast<int>(tempGlyph->BBox().Upper().X() - tempGlyph->BBox().Lower().X() + padding + 0.5);

	--remGlyphs;

    return tempGlyph;
}


void FTEfficientFontImpl::CalculateTextureSize()
{
    //if(!maximumGLTextureSize)
   // {
    //    maximumGLTextureSize = 512;
   //     glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&maximumGLTextureSize);
    //    assert(maximumGLTextureSize); // If you hit this then you have an invalid OpenGL context.
   // }
	maximumGLTextureSize = 512;
	int maximumGLTextureHeightSize = 1024;
	//maximumGLTextureSize = 1024;
    textureWidth = NextPowerOf2((remGlyphs * glyphWidth) + (padding * 2));
    textureWidth = textureWidth > maximumGLTextureSize ? maximumGLTextureSize : textureWidth;

    int h = static_cast<int>((textureWidth - (padding * 2)) / glyphWidth + 0.5);

    textureHeight = NextPowerOf2(((numGlyphs / h) + 1) * glyphHeight);
    textureHeight = textureHeight > maximumGLTextureHeightSize ? maximumGLTextureHeightSize : textureHeight;
}


GLuint FTEfficientFontImpl::CreateTexture()
{
	CalculateTextureSize();

	return 0;
}


bool FTEfficientFontImpl::FaceSize(const unsigned int size, const unsigned int res)
{
	xOffset = yOffset = padding;

    return FTFontImpl::FaceSize(size, res);
}

template <typename T>
inline FTPoint FTEfficientFontImpl::RenderI(const T* string, const int len,
                                          FTPoint position, FTPoint spacing,
                                          int renderMode)
{
	FTPoint tmp;

	if (preRendered)
	{
		tmp = FTFontImpl::Render(string, len, position, spacing, renderMode);
	}
	else
	{
		
		PreRender();
		tmp = FTFontImpl::Render(string, len, position, spacing, renderMode);
		PostRender();

	}
    return tmp;
}


void FTEfficientFontImpl::PreRender()
{
	preRendered = true;

	if(m_need_render_image&&m_buffer_image!=NULL&&m_buffer_image->content_info.format==PF_RGBA0008)
	{
		memset(m_buffer_image->getContentData(),0,m_buffer_image->getImageSize());
	}

}


void FTEfficientFontImpl::PostRender()
{

	if (m_need_render_image&&m_buffer_image!=NULL)
	{
		m_need_render_image=false;
		//static int i=0;
		//char str[256];
		//sprintf(str,"D:/image/%d.png",i++);
		//if (!Utility::isFileORDirExist(str))
		//{
		//	std::vector<unsigned char> vt;
		//	z11::MGPNGCodec::encode(m_buffer_image,vt,2);
		//	FILE *file=fopen(str,"wb");
		//	if(file!=NULL)
		//	{
		//		fwrite(&vt[0],vt.size(),1,file);
		//		fclose(file);
		//	}
		//}
		
	}
	preRendered = false;

}


FTPoint FTEfficientFontImpl::Render(const char * string, const int len,
                                  FTPoint position, FTPoint spacing,
                                  int renderMode)
{
    return RenderI(string, len, position, spacing, renderMode);
}


FTPoint FTEfficientFontImpl::Render(const wchar_t * string, const int len,
                                  FTPoint position, FTPoint spacing,
                                  int renderMode)
{
    return RenderI(string, len, position, spacing, renderMode);
}

