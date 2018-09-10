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

#include <ctype.h>
#include <wctype.h>

#include "FTInternals.h"
#include "FTUnicode.h"

#include "FTGlyphContainer.h"
#include "FTCustomLayoutImpl.h"


//
//  FTCustomLayout
//


FTCustomLayout::FTCustomLayout() :
FTLayout(new FTCustomLayoutImpl())
{}


FTCustomLayout::~FTCustomLayout()
{}


FTBBox FTCustomLayout::BBox(const char *string, const int len, FTPoint pos)
{
    return dynamic_cast<FTCustomLayoutImpl*>(impl)->BBox(string, len, pos);
}


FTBBox FTCustomLayout::BBox(const wchar_t *string, const int len, FTPoint pos)
{
    return dynamic_cast<FTCustomLayoutImpl*>(impl)->BBox(string, len, pos);
}

FTVector<FTBBox> FTCustomLayout::BBoxVector(const char *string, const int len, FTPoint pos)
{
	return dynamic_cast<FTCustomLayoutImpl*>(impl)->BBoxVector(string, len, pos);
}


void FTCustomLayout::Render(const char *string, const int len, FTPoint pos,
                            int renderMode)
{
    return dynamic_cast<FTCustomLayoutImpl*>(impl)->Render(string, len, pos,
                                                           renderMode);
}


void FTCustomLayout::Render(const wchar_t* string, const int len, FTPoint pos,
                            int renderMode)
{
    return dynamic_cast<FTCustomLayoutImpl*>(impl)->Render(string, len, pos,
                                                           renderMode);
}


void FTCustomLayout::SetFont(FTFont *fontInit)
{
    dynamic_cast<FTCustomLayoutImpl*>(impl)->currentFont = fontInit;
}


FTFont *FTCustomLayout::GetFont()
{
    return dynamic_cast<FTCustomLayoutImpl*>(impl)->currentFont;
}


void FTCustomLayout::SetLineLength(const float LineLength)
{
    dynamic_cast<FTCustomLayoutImpl*>(impl)->lineLength = LineLength;
}


float FTCustomLayout::GetLineLength() const
{
    return dynamic_cast<FTCustomLayoutImpl*>(impl)->lineLength;
}


void FTCustomLayout::SetAlignment(const FTGL::TextAlignment Alignment)
{
    dynamic_cast<FTCustomLayoutImpl*>(impl)->alignment = Alignment;
}


FTGL::TextAlignment FTCustomLayout::GetAlignment() const
{
    return dynamic_cast<FTCustomLayoutImpl*>(impl)->alignment;
}


void FTCustomLayout::SetLineSpacing(const float LineSpacing)
{
    dynamic_cast<FTCustomLayoutImpl*>(impl)->lineSpacing = LineSpacing;
}


float FTCustomLayout::GetLineSpacing() const
{
    return dynamic_cast<FTCustomLayoutImpl*>(impl)->lineSpacing;
}


//
//  FTCustomLayoutImpl
//


FTCustomLayoutImpl::FTCustomLayoutImpl()
{
    currentFont = NULL;
    lineLength = 100.0f;
    alignment = FTGL::ALIGN_LEFT;
    lineSpacing = 1.0f;
	stringCacheCount = 0;
}


template <typename T>
inline FTBBox FTCustomLayoutImpl::BBoxI(const T* string, const int len,
                                        FTPoint position)
{
    FTBBox tmp;

	//fix me ? GRF »Áπ˚≤ªº”’‚º∏æ‰±‰ªª◊¯±Íµƒ”Ôæ‰,µ˜”√bbox -> renderÀ˘”–image∂º≤ªƒ‹’˝≥£œ‘ æ¡À,µ´ «µ˜”√¡À÷Æ∫Ûµƒª∞ª·∂ÓÕ‚‘ˆº”ªÊ÷∆¥Œ ˝     
	//glPushMatrix();
	//glRotatef(180,1,1,1);
	//glTranslatef(1000, 1000, 1000);
    WrapText(string, len, position, 0, &tmp);
	//glPopMatrix();

    return tmp;
}
template <typename T>
inline FTVector<FTBBox> FTCustomLayoutImpl::BBoxVectorI(const T* string, const int len,
										FTPoint position)
{
	FTVector<FTBBox>  tmp;
	WrapTextI(string, len, position, 0, &tmp);
	return tmp;
}


FTBBox FTCustomLayoutImpl::BBox(const char *string, const int len,
                                FTPoint position)
{
    return BBoxI(string, len, position);
}

FTVector<FTBBox> FTCustomLayoutImpl::BBoxVector(const char *string, const int len,
									FTPoint position)
{
	return BBoxVectorI(string, len, position);
}

FTBBox FTCustomLayoutImpl::BBox(const wchar_t *string, const int len,
                                FTPoint position)
{
    return BBoxI(string, len, position);
}


template <typename T>
inline void FTCustomLayoutImpl::RenderI(const T *string, const int len,
                                        FTPoint position, int renderMode)
{
    pen = FTPoint(0.0f, 0.0f);
    WrapText(string, len, position, renderMode, NULL);
}


void FTCustomLayoutImpl::Render(const char *string, const int len,
                                FTPoint position, int renderMode)
{
    RenderI(string, len, position, renderMode);
}


void FTCustomLayoutImpl::Render(const wchar_t* string, const int len,
                                FTPoint position, int renderMode)
{
    RenderI(string, len, position, renderMode);
}

/**
 * DAVID PETRIE NOTE: This function has been altered to ensure efficient rendering
 * speeds on iOS devices.  The changes are the following:
 * - A cache is populated with linebreak and spacing info; it must be invalidated and 
 *   refreshed when text changes.
 * - PreRender() and PostRender() have been added to the FTFont and FTFontImpl classes.  
 *   These functions allow all FTFont implementations to perform pre-rendering setup 
 *   tasks specific to their requirements.  For example, when rendering a texture font
 *	 it is not necessary to call ftglEnd() unless the loaded texture id changes.
 *
 * With these changes in place, I can now get 60fps for a full screen of SimpleLayout
 * text on an iPad.
 */
template <typename T>
inline void FTCustomLayoutImpl::WrapTextI(const T *buf, const int len,
                                          FTPoint position, int renderMode,
                                           FTVector<FTBBox> *bounds)
{
    FTUnicodeStringItr<T> breakItr(buf);          // points to the last break character
    FTUnicodeStringItr<T> lineStart(buf);         // points to the line start
    float nextStart = 0.0;     // total width of the current line
    float breakWidth = 0.0;    // width of the line up to the last word break
    float currentWidth = 0.0;  // width of all characters on the current line
    float prevWidth;           // width of all characters but the current glyph
    float wordLength = 0.0;    // length of the block since the last break char
    int charCount = 0;         // number of characters so far on the line
    int breakCharCount = 0;    // number of characters before the breakItr
    float glyphWidth, advance;
    FTBBox glyphBounds;
	bool refresh = false;
	
    // Reset the pen position
    pen.Y(0);
	
    // If we have bounds mark them invalid
  //  if(bounds)
  //  {
		//bounds->Invalidate();
  //  }
	
	// Check if the incoming string is different to the previously
	// cached string.
//	unsigned int i = 0;
//	for (FTUnicodeStringItr<T> itr(buf); *itr; itr++)
//	{
//		if (i >= stringCacheCount ||
//			stringCache[i++] != (unsigned int)*itr)
//		{
//			refresh = true;
//			break;
//		}
//	}
	refresh=true;//add by GRF
	
	if (refresh)
	{
		stringCacheCount = 0;
        layoutGlyphCache.clear();
		
		// Scan the input for all characters that need output
		FTUnicodeStringItr<T> prevItr(buf);
		for (FTUnicodeStringItr<T> itr(buf); *itr; prevItr = itr++, charCount++)
		{
			stringCache[stringCacheCount++] = (unsigned int)*itr;
			
			// Find the width of the current glyph
			glyphBounds = currentFont->BBox(itr.getBufferFromHere(), 1);
			glyphWidth = glyphBounds.Upper().Xf();// - glyphBounds.Lower().Xf();
			
			advance = currentFont->Advance(itr.getBufferFromHere(), 1);
			prevWidth = currentWidth;
			// Compute the width of all glyphs up to the end of buf[i]
			currentWidth = nextStart + glyphWidth;
			// Compute the position of the next glyph
			nextStart += advance;
			
			// See if the current character is a space, a break or a regular character
            if((currentWidth > lineLength) || (*itr == '\n'))
			{
				// A non whitespace character has exceeded the line length.  Or a
				// newline character has forced a line break.  Output the last
				// line and start a new line after the break character.
				// If we have not yet found a break, break on the last character
				if(breakItr == lineStart || (*itr == '\n'))
				{
					// Break on the previous character
					breakItr = prevItr;
					breakCharCount = charCount;
					breakWidth = prevWidth;
					// None of the previous words will be carried to the next line
					wordLength = 0;
					// If the current character is a newline discard its advance
					if(*itr == '\n') advance = 0;
				}
				
				float remainingWidth = lineLength - breakWidth;
				
				// Render the current substring
				FTUnicodeStringItr<T> breakChar = breakItr;
				// move past the break character and don't count it on the next line either
				++breakChar;
				// If the break character is a newline do not render it
				if(*breakChar == '\n')
				{
					++breakChar; --charCount;
				}
				CustomLayoutGlyphCacheItem_t cacheItem;
				cacheItem.buf = (char*)lineStart.getBufferFromHere();
				cacheItem.charCount = breakCharCount;
				if (cacheItem.charCount!=-1)
				{
					cacheItem.buf=cacheItem.buf.substr(0,cacheItem.charCount*3+3);
				}
				
				cacheItem.position = FTPoint(position.X(), position.Y(), position.Z());
				cacheItem.remainingWidth = remainingWidth;
				cacheItem.penDiff = FTPoint(0, currentFont->LineHeight() * lineSpacing);
				layoutGlyphCache.push_back(cacheItem);
				
				lineStart = breakChar;
				// The current width is the width since the last break
				nextStart = wordLength + advance;
				wordLength += advance;
				currentWidth = wordLength + advance;
				// Reset the safe break for the next line
				breakItr = lineStart;
				charCount -= breakCharCount;
			}
			else
			{
				wordLength += advance;
			}
		}
		
		float remainingWidth = lineLength - currentWidth;
		// Render any remaining text on the last line
		// Disable justification for the last row
		if(alignment == FTGL::ALIGN_JUSTIFY)
		{
			alignment = FTGL::ALIGN_LEFT;
			CustomLayoutGlyphCacheItem_t cacheItem;
			cacheItem.buf = (char *)lineStart.getBufferFromHere();
			cacheItem.charCount = -1;
			//cacheItem.buf=cacheItem.buf.substr(0,cacheItem.charCount);
			cacheItem.position = FTPoint(position.X(), position.Y(), position.Z());
			cacheItem.penDiff = FTPoint(0,0,0);
			cacheItem.remainingWidth = remainingWidth;
			layoutGlyphCache.push_back(cacheItem);
			alignment = FTGL::ALIGN_JUSTIFY;
		}
		else
		{
			CustomLayoutGlyphCacheItem_t cacheItem;
			cacheItem.buf = (char *)lineStart.getBufferFromHere();
			cacheItem.charCount = -1;
			//cacheItem.buf=cacheItem.buf.substr(0,cacheItem.charCount);
			cacheItem.position = FTPoint(position.X(), position.Y(), position.Z());
			cacheItem.penDiff = FTPoint(0,0,0);
			cacheItem.remainingWidth = remainingWidth;
			layoutGlyphCache.push_back(cacheItem);
		}
	}
	
	// Draw each of the glyphs in the cache.
	currentFont->PreRender();
    std::list<CustomLayoutGlyphCacheItem_t>::iterator it;
	FTBBox box;
	for (it = layoutGlyphCache.begin(); it != layoutGlyphCache.end(); it++)
	{
		//layoutGlyphCacheItem_t cacheItem = (*it);
		if (bounds!=NULL)
		{
            //printf("charCount : %d, position : %f, %f, %f, remainingWidth : %f.\n", it->charCount, it->position.Xf(), it->position.Yf(), it->position.Zf(), it->remainingWidth);
			box.Invalidate();
			OutputWrapped((T*)it->buf.c_str(), 
				it->charCount,
				it->position,
				renderMode,
				it->remainingWidth,
				&box);
            //printf("box lower : %f, %f, %f, upper : %f, %f, %f.\n", box.Lower().Xf(), box.Lower().Yf(), box.Lower().Zf(), box.Upper().Xf(), box.Upper().Yf(), box.Upper().Zf());
			bounds->push_back(box);
		}
		else
		{

			OutputWrapped((T*)it->buf.c_str(), 
				it->charCount,
				it->position,
				renderMode,
				it->remainingWidth,
				NULL);
		}
		pen -= it->penDiff;
	}
	currentFont->PostRender();
}


void FTCustomLayoutImpl::WrapText(const char *buf, const int len,
                                  FTPoint position, int renderMode,
                                  FTBBox *bounds)
{
	if (bounds==NULL)
	{
		WrapTextI(buf, len, position, renderMode, NULL);
	}
	else
	{
		FTVector<FTBBox> vt;
		WrapTextI(buf, len, position, renderMode, &vt);
		for (size_t i=0;i<vt.size();i++)
		{
			*bounds|=vt[i];
		}
		
	}
	
}


void FTCustomLayoutImpl::WrapText(const wchar_t* buf, const int len,
                                  FTPoint position, int renderMode,
                                  FTBBox *bounds)
{
	if (bounds==NULL)
	{
		WrapTextI(buf, len, position, renderMode, NULL);
	}
	else
	{
		FTVector<FTBBox> vt;
		WrapTextI(buf, len, position, renderMode, &vt);
		for (size_t i=0;i<vt.size();i++)
		{
			*bounds|=vt[i];
		}
	}
}


template <typename T>
inline void FTCustomLayoutImpl::OutputWrappedI(const T *buf, const int len,
                                               FTPoint position, int renderMode,
                                               const float remaining,
                                               FTBBox *bounds)
{
    float distributeWidth = 0.0;
    // Align the text according as specified by Alignment
    switch (alignment)
    {
        case FTGL::ALIGN_LEFT:
            pen.X(0);
            break;
        case FTGL::ALIGN_CENTER:
            pen.X(remaining / 2);
            break;
        case FTGL::ALIGN_RIGHT:
            pen.X(remaining);
            break;
        case FTGL::ALIGN_JUSTIFY:
            pen.X(0);
            distributeWidth = remaining;
            break;
    }
	
    // If we have bounds expand them by the line's bounds, otherwise render
    // the line.
    if(bounds)
    {
        FTBBox temp = currentFont->BBox(buf, len);
		
        // Add the extra space to the upper x dimension
        temp = FTBBox(temp.Lower() + pen,
                      temp.Upper() + pen + FTPoint(distributeWidth, 0));
		
        // See if this is the first area to be added to the bounds
        if(bounds->IsValid())
        {
            *bounds |= temp;
        }
        else
        {
            *bounds = temp;
		}
		//fix me ? GRF »Áπ˚≤ªº”’‚“ªæ‰,µ˜”√bbox -> renderÀ˘”–image∂º≤ªƒ‹’˝≥£œ‘ æ¡À,µ´ «µ˜”√¡À÷Æ∫Ûµƒª∞ª·∂ÓÕ‚‘ˆº”ªÊ÷∆¥Œ ˝     
		//RenderSpace(buf, len, position, renderMode, distributeWidth);
    }
    else
    {
        RenderSpace(buf, len, position, renderMode, distributeWidth);
    }
}


void FTCustomLayoutImpl::OutputWrapped(const char *buf, const int len,
                                       FTPoint position, int renderMode,
                                       const float remaining, FTBBox *bounds)
{
    OutputWrappedI(buf, len, position, renderMode, remaining, bounds);
}


void FTCustomLayoutImpl::OutputWrapped(const wchar_t *buf, const int len,
                                       FTPoint position, int renderMode,
                                       const float remaining, FTBBox *bounds)
{
    OutputWrappedI(buf, len, position, renderMode, remaining, bounds);
}


template <typename T>
inline void FTCustomLayoutImpl::RenderSpaceI(const T *string, const int len,
                                             FTPoint position, int renderMode,
                                             const float extraSpace)
{
    float space = 0.0;
	
    // If there is space to distribute, count the number of spaces
    if(extraSpace > 0.0)
    {
        int numSpaces = 0;
		
        // Count the number of space blocks in the input
        FTUnicodeStringItr<T> prevItr(string), itr(string);
        for(int i = 0; ((len < 0) && *itr) || ((len >= 0) && (i <= len));
            ++i, prevItr = itr++)
        {
            // If this is the end of a space block, increment the counter
            if((i > 0) && !iswspace(*itr) && iswspace(*prevItr))
            {
                numSpaces++;
            }
        }
		
        space = extraSpace/numSpaces;
    }
	
	// Output all characters of the string
    FTUnicodeStringItr<T> prevItr(string), itr(string);
	//fix me? modified by GRF
	for(int i = 0; ((len < 0) && *itr) || ((len >= 0) && (i < len));
		++i, prevItr = itr++)
    //for(int i = 0; ((len < 0) && *itr) || ((len >= 0) && (i <= len));
    //    ++i, prevItr = itr++)
    {
        // If this is the end of a space block, distribute the extra space
        // inside it
        if((i > 0) && !iswspace(*itr) && iswspace(*prevItr))
        {
            pen += FTPoint(space, 0);
        }
		
		/**
		 * We want to minimise repeated calls to this function -- is it possible to
		 * set up a cache using Advance?
		 */
		pen.X(pen.X()+position.X());
		pen.Y(pen.Y()-position.Y());
		pen = currentFont->Render(itr.getBufferFromHere(), 1, pen, FTPoint(), renderMode);
		pen.X(pen.X()-position.X());
		pen.Y(pen.Y()+position.Y());
    }
}


void FTCustomLayoutImpl::RenderSpace(const char *string, const int len,
                                     FTPoint position, int renderMode,
                                     const float extraSpace)
{
    RenderSpaceI(string, len, position, renderMode, extraSpace);
}


void FTCustomLayoutImpl::RenderSpace(const wchar_t *string, const int len,
                                     FTPoint position, int renderMode,
                                     const float extraSpace)
{
    RenderSpaceI(string, len, position, renderMode, extraSpace);
}

