/****************************************************************************
Copyright (c) 2010 z11-x.org

http://www.z11-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "MGEGLView_android.h"
#include "GLES/gl.h"
#include "jni/IMEJni.h"
#include "jni/MessageJni.h"

#include <stdlib.h>

namespace z11 {

MGEGLView::MGEGLView()
    : m_bNotHVGA(false),
      m_fScreenScaleFactor(1.0)
{
}

void MGEGLView::setFrameWidthAndHeight(int width, int height)
{
	m_sSizeInPixel.width = width;
	m_sSizeInPixel.height = height;
}

void MGEGLView::create(int width, int height)
{
	if (width == 0 || height == 0)
	{
		return;
	}

	m_sSizeInPoint.width = width;
	m_sSizeInPoint.height = height;

	// calculate the factor and the rect of viewport
	m_fScreenScaleFactor =  MINVALUE((float)m_sSizeInPixel.width / m_sSizeInPoint.width,
		                         (float)m_sSizeInPixel.height / m_sSizeInPoint.height);
	int viewPortW = (int)(m_sSizeInPoint.width * m_fScreenScaleFactor);
	int viewPortH = (int)(m_sSizeInPoint.height * m_fScreenScaleFactor);
	m_rcViewPort.x = (m_sSizeInPixel.width - viewPortW) / 2;
	m_rcViewPort.y = (m_sSizeInPixel.height - viewPortH) / 2;
	m_rcViewPort.width = viewPortW;
	m_rcViewPort.height = viewPortH;

	m_bNotHVGA = true;

}

MGEGLView::~MGEGLView()
{
}

MGSize  MGEGLView::getSize()
{
        if (m_bNotHVGA)
        {
                MGSize size(m_sSizeInPoint.width, m_sSizeInPoint.height);
	        return size;
        }
        else
        {
                MGSize size(m_sSizeInPixel.width, m_sSizeInPixel.height);
	        return size;
        }

}

bool MGEGLView::isOpenGLReady()
{
	return (m_sSizeInPixel.width != 0 && m_sSizeInPixel.height != 0);
}

void MGEGLView::release()
{
	terminateProcessJNI();
}

void MGEGLView::swapBuffers()
{
}

bool MGEGLView::canSetContentScaleFactor()
{
    // can scale content?
    return false;
}

void MGEGLView::setContentScaleFactor(float contentScaleFactor)
{
	m_fScreenScaleFactor = contentScaleFactor;
}

void MGEGLView::setViewPortInPoints(float x, float y, float w, float h)
{
    if (m_bNotHVGA)
    {
        float factor = m_fScreenScaleFactor /1.0f;// CC_CONTENT_SCALE_FACTOR();
        glViewport((GLint)(x * factor) + m_rcViewPort.x,
            (GLint)(y * factor) + m_rcViewPort.y,
            (GLint)(w * factor),
            (GLint)(h * factor));
    }
    else
    {
        glViewport((GLint)x,
            (GLint)y,
            (GLint)w,
            (GLint)h);
    }
}

void MGEGLView::setScissorInPoints(float x, float y, float w, float h)
{
    if (m_bNotHVGA)
    {
        float factor = m_fScreenScaleFactor / 1.0f;//CC_CONTENT_SCALE_FACTOR();
        glScissor((GLint)(x * factor) + m_rcViewPort.x,
            (GLint)(y * factor) + m_rcViewPort.y,
            (GLint)(w * factor),
            (GLint)(h * factor));
    }
    else
    {
        glScissor((GLint)x,
            (GLint)y,
            (GLint)w,
            (GLint)h);
    }
}

MGEGLView& MGEGLView::sharedOpenGLView()
{
	static MGEGLView instance;
	return instance;
}

float MGEGLView::getScreenScaleFactor()
{
        return m_fScreenScaleFactor;
}

MGRect MGEGLView::getViewPort()
{
    if (m_bNotHVGA)
    {
        return m_rcViewPort;
    }
    else
    {
        MGRect rect=MGRectMake(0, 0, 0, 0);
        return rect;
    }
}

void MGEGLView::setIMEKeyboardState(bool bOpen)
{

    setKeyboardStateJNI((int)bOpen);
}

}       // end of namespace z11

