//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (MGEGLView_android.h)
/// @brief (本文件实现的功能的简述)
///
///(本文件实现的功能的详述)
///
/// @version 0.1   (版本声明)
/// @author        (Gu RongFang)
/// @date          (11-09-01)
///
///
///    修订说明：最初版本
//////////////////////////////////////////////////////////////////////////
#ifndef _MGEGLVIEW_ANDROID_H_
#define _MGEGLVIEW_ANDROID_H_

#include "../../CommonBase/MGStructure.h"
namespace   z11 {

class  MGEGLView
{
public:
    MGEGLView();
    virtual ~MGEGLView();

    MGSize  getSize();
    bool    isOpenGLReady();
	/**
	 * the width and height is the real size of phone
	 */
    void    setFrameWidthAndHeight(int width, int height);
	/**
	 * create a drawing rect,
	 * the width and heiht is the resource size match best
	 */
	void    create(int width, int height);

    // keep compatible
    void    release();
    void    swapBuffers();
    bool    canSetContentScaleFactor();
    void    setContentScaleFactor(float contentScaleFactor);
	void    setViewPortInPoints(float x, float y, float w, float h);
    void    setScissorInPoints(float x, float y, float w, float h);
	MGRect  getViewPort();
	float   getScreenScaleFactor();
    void    setIMEKeyboardState(bool bOpen);

	// static function
	/**
	@brief	get the shared main open gl window
	*/
	static MGEGLView& sharedOpenGLView();

private:
	MGSize m_sSizeInPixel;
	MGSize m_sSizeInPoint;
	MGRect m_rcViewPort;
	bool   m_bNotHVGA;

	float  m_fScreenScaleFactor;
};

}   //


#endif //_MGEGLVIEW_ANDROID_H_
