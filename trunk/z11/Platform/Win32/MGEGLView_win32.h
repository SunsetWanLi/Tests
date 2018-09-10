//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGEGLView_win32.h)  
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
#ifndef _MGEGLVIEW_WIN32_H_
#define _MGEGLVIEW_WIN32_H_

#include <winsock2.h>
#include <Windows.h>
#include "../../CommonBase/MGStructure.h"
#include "../../GUI/Events/MIEvent.h"
#include "MCF/GenericHelpers.h"

namespace z11
{

	class MGEGL;

	class MGEGLView
	{
	private:
		static LRESULT CALLBACK _WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:

		MGEGLView();
		virtual ~MGEGLView();

		MGSize  getSize();
		bool    isOpenGLReady();
		void    release();
		void    swapBuffers();
		bool    canSetContentScaleFactor();
		void    setContentScaleFactor(float contentScaleFactor);

		virtual bool create(LPCTSTR pTitle, int w, int h, uint_8 wnd_type = 0, bool isSimulateMultiTouch = true);
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

		int setDeviceOrientation(int eOritation);
		void setViewPortInPoints(float x, float y, float w, float h);
		void setScissorInPoints(float x, float y, float w, float h);

		void setIMEKeyboardState(bool bOpen);

		// win32 platform function
		HWND getHWnd();
//		void resize(int width, int height);
//		void centerWindow();
		void setScreenScale(float factor);

		// static function

		/**
		@brief	get the shared main open gl window
		*/
		static MGEGLView& sharedOpenGLView();
	public:
		void handleEventImmediatelly(MIEvent* event);
		void onPause();
		void onResume();
	private:

		bool                m_bCaptured;
		bool				m_bOrientationReverted;
		bool				m_bOrientationInitVertical;

		bool				m_isSimulateMultiTouch;

//		MGEGL *            m_pEGL;
		class OpenGLThreadClass *m_lpOpenGLThread;

		HWND				m_hWnd;

		int					m_eInitOrientation;
		SIZE                m_tSizeInPoints;
		float               m_fScreenScaleFactor;
		RECT                m_rcViewPort;
		bool				m_is_full_screen;
	};
}




#endif //_MGEGLVIEW_WIN32_H_
