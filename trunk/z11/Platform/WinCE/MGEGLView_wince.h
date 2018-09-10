//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGEGLView_wince.h)  
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
#ifndef _MGEGLVIEW_WINCE_H_
#define _MGEGLVIEW_WINCE_H_

#include <Windows.h>
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

		virtual bool Create(LPCTSTR pTitle, int w, int h);
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

		int setDeviceOrientation(int eOritation);
		void setViewPortInPoints(float x, float y, float w, float h);
		void setScissorInPoints(float x, float y, float w, float h);

		void setIMEKeyboardState(bool bOpen);

		// win32 platform function
		HWND getHWnd();
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

		MGEGL *            m_pEGL;

		HWND				m_hWnd;

		int					m_eInitOrientation;
		SIZE                m_tSizeInPoints;
		float               m_fScreenScaleFactor;
		RECT                m_rcViewPort;
	};
}


#endif //_MGEGLVIEW_WINCE_H_
