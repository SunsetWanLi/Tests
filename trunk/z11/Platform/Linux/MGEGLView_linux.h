//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (MGEGLView_linux.h)
/// @brief (本文件实现的功能的简述)
///
///(本文件实现的功能的详述)
///
/// @version 0.1   (版本声明)
/// @author        (Gu RongFang)
/// @date          (11-08-30)
///
///
///    修订说明：最初版本
//////////////////////////////////////////////////////////////////////////
#ifndef _MGEGLVIEW_LINUX_H_
#define _MGEGLVIEW_LINUX_H_

#include <X11/Xlib.h>
#include "../../CommonBase/MGStructure.h"


namespace z11
{

	class MGEGL;

	class MGEGLView
	{
	public:

		MGEGLView();
		virtual ~MGEGLView();

		MGSize  getSize();
		bool    isOpenGLReady();
		void    release();
		void    swapBuffers();
		bool    canSetContentScaleFactor();
		void    setContentScaleFactor(float contentScaleFactor);

		virtual bool create(const char* pTitle, int w, int h);
		virtual void WindowProc(const XEvent& event);

		int setDeviceOrientation(int eOritation);
		void setViewPortInPoints(float x, float y, float w, float h);
		void setScissorInPoints(float x, float y, float w, float h);

		void setIMEKeyboardState(bool bOpen);

		// linux platform function
		Display* getDisplay();
		void resize(int width, int height);
		void centerWindow();
		void setScreenScale(float factor);

		// static function

		/**
		@brief	get the shared main open gl window
		*/
		static MGEGLView& sharedOpenGLView();

	protected:

	private:

		bool                m_bCaptured;
		bool				m_bOrientationReverted;
		bool				m_bOrientationInitVertical;

		MGEGL *             m_pEGL;

		Display*			m_pDisplay;

		int					m_eInitOrientation;
		MGSize                m_tSizeInPoints;
		float               m_fScreenScaleFactor;
		MGRect                m_rcViewPort;
	};
}




#endif //_MGEGLVIEW_LINUX_H_
