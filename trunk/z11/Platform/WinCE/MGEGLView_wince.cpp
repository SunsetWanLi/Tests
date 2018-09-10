#include "../MGPlatformMacros.h"
#include "../MGEGLView_platform.h"

#include <set>
#include <GLES/egl.h>
#include <GLES/gl.h>
#include <aygshell.h>
#pragma comment(lib, "aygshell.lib")

#include "../../System/Shared/MGLog.h"
#include "../../GUI/Events/MIEvent.h"
#include "../../GUI/Events/MITouch.h"

#include "../../GUI/GuiContainer/GuiContainer.h"
#include "../../System/Shared/Utility.h"
#include "../../System/Device/MGDevice.h"
//#pragma comment(lib, "libMGGLES_CM.lib")
namespace z11
{

	//////////////////////////////////////////////////////////////////////////
	// impliment MGEGL
	//////////////////////////////////////////////////////////////////////////

	class MGEGL
	{
	public:
		~MGEGL() 
		{
			if (EGL_NO_SURFACE != m_eglSurface)
			{
				eglDestroySurface(m_eglDisplay, m_eglSurface);
			}
			if (EGL_NO_CONTEXT != m_eglContext)
			{
				eglDestroyContext(m_eglDisplay, m_eglContext);
			}
			eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
			eglTerminate(m_eglDisplay);
			if (m_eglNativeDisplay)
			{
				ReleaseDC(m_eglNativeWindow, m_eglNativeDisplay);
			}

			DestroyWindow(m_eglNativeWindow);
		}

		static MGEGL * create(MGEGLView * pWindow)
		{
			MGEGL * pEGL = new MGEGL;
			BOOL bSuccess = FALSE;
			do 
			{
				MG_BREAK_IF(! pEGL);

				pEGL->m_eglNativeWindow = pWindow->getHWnd();

				pEGL->m_eglNativeDisplay = GetDC(pEGL->m_eglNativeWindow);

				EGLDisplay eglDisplay;
				MG_BREAK_IF(EGL_NO_DISPLAY == (eglDisplay = eglGetDisplay(pEGL->m_eglNativeDisplay)));

				EGLint nMajor, nMinor;
				MG_BREAK_IF(EGL_FALSE == eglInitialize(eglDisplay, &nMajor, &nMinor) || 1 != nMajor);

				const EGLint aConfigAttribs[] =
				{
					EGL_RED_SIZE,       5,
					EGL_GREEN_SIZE,     5,
					EGL_BLUE_SIZE,      5,
					EGL_DEPTH_SIZE, 16,
					EGL_ALPHA_SIZE,     EGL_DONT_CARE,
					EGL_STENCIL_SIZE,   EGL_DONT_CARE,
					EGL_SURFACE_TYPE,   EGL_WINDOW_BIT,
					EGL_NONE
				};
				EGLint iConfigs;
				EGLConfig eglConfig;
				MG_BREAK_IF(EGL_FALSE == eglChooseConfig(eglDisplay, aConfigAttribs, &eglConfig, 1, &iConfigs) 
					|| (iConfigs != 1));

				EGLContext eglContext;
				eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, NULL);
				MG_BREAK_IF(EGL_NO_CONTEXT == eglContext);

				EGLSurface eglSurface;
				eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, pEGL->m_eglNativeWindow, NULL);
				MG_BREAK_IF(EGL_NO_SURFACE == eglSurface);

				MG_BREAK_IF(EGL_FALSE == eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext));

				pEGL->m_eglDisplay = eglDisplay;
				pEGL->m_eglConfig  = eglConfig;
				pEGL->m_eglContext = eglContext;
				pEGL->m_eglSurface = eglSurface;
				bSuccess = TRUE;
			} while (0);

			if (! bSuccess)
			{
				MG_SAFE_DELETE(pEGL);  
			}

			return pEGL;
		}

		void resizeSurface()
		{
			//  		if (! m_eglNativeWindow || EGL_NO_DISPLAY == m_eglDisplay)
			//  		{
			//  			return;
			//  		}
			//  
			//  		// release old surface
			//  		if (EGL_NO_SURFACE != m_eglSurface)
			//  		{
			//  			eglDestroySurface(m_eglDisplay, m_eglSurface);
			//  			m_eglSurface = EGL_NO_SURFACE;
			//  		}
			//  
			//  		// create new surface and make current
			//  		m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, m_eglNativeWindow, NULL);
			//  		eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext);
		}

		void swapBuffers()
		{
			if (EGL_NO_DISPLAY != m_eglDisplay)
			{
				eglSwapBuffers(m_eglDisplay, m_eglSurface);
			}
		}
	private:
		MGEGL() 
			: m_eglNativeWindow(NULL)
			, m_eglNativeDisplay(EGL_DEFAULT_DISPLAY)
			, m_eglDisplay(EGL_NO_DISPLAY)
			, m_eglConfig(0)
			, m_eglSurface(EGL_NO_SURFACE)
			, m_eglContext(EGL_NO_CONTEXT)
		{}

		NativeWindowType     m_eglNativeWindow;
		NativeDisplayType    m_eglNativeDisplay;
		EGLDisplay              m_eglDisplay;
		EGLConfig               m_eglConfig;
		EGLSurface              m_eglSurface;
		EGLContext              m_eglContext;
	};

	//////////////////////////////////////////////////////////////////////////
	// impliment CCEGLView
	//////////////////////////////////////////////////////////////////////////
	static MGEGLView * s_pMainWindow;
	static const WCHAR * kWindowClassName = L"OnWin32";

	float down_x;
	float down_y;


	 LRESULT CALLBACK _WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//return  test_(hWnd,uMsg,wParam,lParam);
		if (s_pMainWindow && s_pMainWindow->getHWnd() == hWnd)
		{
			return s_pMainWindow->WindowProc(uMsg, wParam, lParam);
		}
		else
		{
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

	MGEGLView::MGEGLView()
		: m_bCaptured(false)
		, m_bOrientationReverted(false)
		, m_bOrientationInitVertical(false)
		, m_pEGL(NULL)
		, m_hWnd(NULL)
		, m_fScreenScaleFactor(1.0f)
	{
		m_tSizeInPoints.cx = m_tSizeInPoints.cy = 0;
		SetRectEmpty(&m_rcViewPort);
	}

	MGEGLView::~MGEGLView()
	{
	}

	bool MGEGLView::Create(LPCTSTR pTitle, int w, int h)
	{
		bool bRet = false;
		do 
		{
			MG_BREAK_IF(m_hWnd);

			HINSTANCE hInstance = GetModuleHandle( NULL );
			WNDCLASS  wc;		// Windows Class Structure

			// Redraw On Size, And Own DC For Window.
			wc.style          = CS_HREDRAW | CS_VREDRAW ;  
			wc.lpfnWndProc    = _WindowProc;					// WndProc Handles Messages
			wc.cbClsExtra     = 0;                              // No Extra Window Data
			wc.cbWndExtra     = 0;								// No Extra Window Data
			wc.hInstance      = hInstance;						// Set The Instance
			wc.hIcon          = NULL;	// Load The Default Icon
			wc.hCursor        = NULL;	// Load The Arrow Pointer
			wc.hbrBackground  = (HBRUSH) GetStockObject(BLACK_BRUSH); // No Background Required For GL
			wc.lpszMenuName   = NULL;                           // We Don't Want A Menu
			wc.lpszClassName  = kWindowClassName;               // Set The Class Name

			MG_BREAK_IF(! RegisterClass(&wc) && 1410 != GetLastError());		

			// center window position
			RECT rcDesktop;
			GetWindowRect(GetDesktopWindow(), &rcDesktop);

			// create window
			m_hWnd = CreateWindow(
				kWindowClassName, 
				pTitle, 
				WS_VISIBLE, 
				0, 0,
				w, 
				h,
				NULL,
				NULL, 
				wc.hInstance,
				NULL );

			MG_BREAK_IF(! m_hWnd);

			m_tSizeInPoints.cx = w;
			m_tSizeInPoints.cy = h;
			resize(w, h);
			
			//full screen  
			SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON | SHFS_HIDESTARTICON);
			HWND hFindWnd=FindWindow(L"HHTaskBar",0);
			if (hFindWnd)
			{
				ShowWindow(hFindWnd,SW_HIDE);
			}
			SetFocus(m_hWnd);

			// init egl
			m_pEGL = MGEGL::create(this);

			if (! m_pEGL)
			{
				DestroyWindow(m_hWnd);
				m_hWnd = NULL;
				break;
			}

			s_pMainWindow = this;
			bRet = true;
		} while (0);

		return bRet;
	}
	
	LRESULT MGEGLView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		static bool pressed=false;
		PAINTSTRUCT ps;

		switch (message)
		{
		case WM_LBUTTONDOWN:
			{
				//¿¼ÂÇËõ·ÅÖµ m_fScreenScaleFactor 
				std::set<MITouch*> touches;
				MITouch *mi_touch=new MITouch();
				mi_touch->point.x=(float)(lParam&0xffff);
				mi_touch->point.y=(float)(lParam>>16);
				touches.insert(mi_touch);
				GuiContainer::container->handleTouchesEvent(touches,MITouchEventBegan);

			}
			break;

		case WM_MOUSEMOVE:
			{
				std::set<MITouch*> touches;
				MITouch *mi_touch=new MITouch();
				mi_touch->point.x=(float)(lParam&0xffff);
				mi_touch->point.y=(float)(lParam>>16);
				touches.insert(mi_touch);
				GuiContainer::container->handleTouchesEvent(touches,MITouchEventMoved);
			}
			break;

		case WM_LBUTTONUP:
			{
				pressed=false;
				std::set<MITouch*> touches;
				MITouch *mi_touch=new MITouch();
				mi_touch->point.x=(float)(lParam&0xffff);
				mi_touch->point.y=(float)(lParam>>16);
				touches.insert(mi_touch);
				GuiContainer::container->handleTouchesEvent(touches,MITouchEventEnded);

			}
			break;

		case WM_CHAR:
			{
				if (wParam!=8)
				{
					MIEvent* event = new MIEvent();
					MGTextContent* content = new MGTextContent();
					wchar_t wtext[]={wParam,0};
					char text[8]={0};
					content->text_content = string(Utility::unicode_to_utf8(text,wtext));
					event->_event_type = KEYBOARD_INPUT;
					event->_content = content;
					event->_responder = NULL;
					GuiContainer::container->handleSystemEvent(event);
				}
				else
				{
					MIEvent* event = new MIEvent();
					event->_event_type = KEYBOARD_DELETE_BACKWARD;
					event->_responder = NULL;
					event->_content = NULL;
					GuiContainer::container->handleSystemEvent(event);
				}
			}
			break;

		case WM_PAINT:
			BeginPaint(m_hWnd, &ps);
			EndPaint(m_hWnd, &ps);
			break;

		case WM_KEYDOWN:
			if (wParam==VK_ESCAPE )
			{
				exit(0);
				//PostQuitMessage(1);
			}
			break;

		case WM_CLOSE:
			exit(0);

			//PostQuitMessage(1);
			//CCDirector::sharedDirector()->end();
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(m_hWnd, message, wParam, lParam);
		}
		return 0;
	}

	MGSize MGEGLView::getSize()
	{
		if (m_bOrientationReverted)
		{
			return MGSize((float)(m_tSizeInPoints.cy), (float)(m_tSizeInPoints.cx));
		}
		return MGSize((float)(m_tSizeInPoints.cx), (float)(m_tSizeInPoints.cy));
	}

	bool MGEGLView::isOpenGLReady()
	{
		return (NULL != m_pEGL);
	}

	void MGEGLView::release()
	{
		if (m_hWnd)
		{
			DestroyWindow(m_hWnd);
			m_hWnd = NULL;
		}
		s_pMainWindow = NULL;
		UnregisterClass(kWindowClassName, GetModuleHandle(NULL));

		MG_SAFE_DELETE(m_pEGL);
		delete this;
	}



	void MGEGLView::swapBuffers()
	{
		if (m_pEGL)
		{
			m_pEGL->swapBuffers();
		}
	}

	int MGEGLView::setDeviceOrientation(int eOritation)
	{
		//do 
		//{
		//	bool bVertical = (CCDeviceOrientationPortrait == eOritation
		//		|| kCCDeviceOrientationPortraitUpsideDown == eOritation) ? true : false;

		//	MG_BREAK_IF(m_bOrientationReverted && bVertical != m_bOrientationInitVertical);
		//	MG_BREAK_IF(! m_bOrientationReverted && bVertical == m_bOrientationInitVertical);

		//	m_bOrientationReverted = (bVertical == m_bOrientationInitVertical) ? false : true;

		//	// swap width and height
		//	RECT rc;
		//	GetClientRect(m_hWnd, &rc);
		//	resize(rc.bottom - rc.top, rc.right - rc.left);

		//} while (0);

		return m_eInitOrientation;
	}

	void MGEGLView::setViewPortInPoints(float x, float y, float w, float h)
	{
		if (m_pEGL)
		{
			float factor = 1.0f;//m_fScreenScaleFactor / CC_CONTENT_SCALE_FACTOR();
			glViewport((GLint)(x * factor) + m_rcViewPort.left,
				(GLint)(y * factor) + m_rcViewPort.top,
				(GLint)(w * factor),
				(GLint)(h * factor));
		}
	}

	void MGEGLView::setScissorInPoints(float x, float y, float w, float h)
	{
		if (m_pEGL)
		{
			float factor = 1.0f;//m_fScreenScaleFactor / CC_CONTENT_SCALE_FACTOR();
			glScissor((GLint)(x * factor) + m_rcViewPort.left,
				(GLint)(y * factor) + m_rcViewPort.top,
				(GLint)(w * factor),
				(GLint)(h * factor));
		}
	}

	void MGEGLView::setIMEKeyboardState(bool /*bOpen*/)
	{
	}

	HWND MGEGLView::getHWnd()
	{
		return m_hWnd;
	}

	void MGEGLView::resize(int width, int height)
	{
		if (! m_hWnd)
		{
			return;
		}

		RECT rcClient;
		GetClientRect(m_hWnd, &rcClient);
		if (rcClient.right - rcClient.left == width &&
			rcClient.bottom - rcClient.top == height)
		{
			return;
		}
		// calculate new window width and height
		rcClient.right = rcClient.left + width;
		rcClient.bottom = rcClient.top + height;
		AdjustWindowRectEx(&rcClient, GetWindowLong(m_hWnd, GWL_STYLE), false, GetWindowLong(m_hWnd, GWL_EXSTYLE));

		// change width and height
		SetWindowPos(m_hWnd, 0, 0, 0, rcClient.right - rcClient.left, 
			rcClient.bottom - rcClient.top,  SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

		if (m_pEGL)
		{
			m_pEGL->resizeSurface();
		}

		// calculate view port in pixels
		int viewPortW = (int)(m_tSizeInPoints.cx * m_fScreenScaleFactor);
		int viewPortH = (int)(m_tSizeInPoints.cy * m_fScreenScaleFactor);
		if (m_bOrientationReverted)
		{
			int tmp = viewPortW;
			viewPortW = viewPortH;
			viewPortH = tmp;
		}
		GetClientRect(m_hWnd, &rcClient);

		// calculate client new width and height
		int newW = rcClient.right - rcClient.left;
		int newH = rcClient.bottom - rcClient.top;

		// calculate new view port
		m_rcViewPort.left   = rcClient.left + (newW - viewPortW) / 2;
		m_rcViewPort.top    = rcClient.top + (newH - viewPortH) / 2;
		m_rcViewPort.right  = m_rcViewPort.left + viewPortW;
		m_rcViewPort.bottom = m_rcViewPort.top + viewPortH;
	}

	void MGEGLView::centerWindow()
	{
		return ;
		if (! m_hWnd)
		{
			return;
		}

		//full screen  
		SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON | SHFS_HIDESTARTICON);
		HWND hFindWnd=FindWindow(L"HHTaskBar",0);
		if (hFindWnd)
		{
			ShowWindow(hFindWnd,SW_HIDE);
		}
		SetFocus(m_hWnd);

		RECT rcDesktop, rcWindow;
		GetWindowRect(GetDesktopWindow(), &rcDesktop);

		GetWindowRect(m_hWnd, &rcWindow);

		int offsetX = (rcDesktop.right - rcDesktop.left - (rcWindow.right - rcWindow.left)) / 2;
		offsetX = (offsetX > 0) ? offsetX : rcDesktop.left;
		int offsetY = (rcDesktop.bottom - rcDesktop.top - (rcWindow.bottom - rcWindow.top)) / 2;
		offsetY = (offsetY > 0) ? offsetY : rcDesktop.top;

		SetWindowPos(m_hWnd, 0, offsetX, offsetY, 0, 0,  SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	}

	void MGEGLView::setScreenScale(float factor)
	{
		m_fScreenScaleFactor = factor;
	}

	bool MGEGLView::canSetContentScaleFactor()
	{
		return true;
	}

	void MGEGLView::setContentScaleFactor(float contentScaleFactor)
	{
		m_fScreenScaleFactor = contentScaleFactor;
		if (m_bOrientationReverted)
		{
			resize((int)(m_tSizeInPoints.cy * contentScaleFactor), (int)(m_tSizeInPoints.cx * contentScaleFactor));
		}
		else
		{
			resize((int)(m_tSizeInPoints.cx * contentScaleFactor), (int)(m_tSizeInPoints.cy * contentScaleFactor));
		}
		centerWindow();
	}

	MGEGLView& MGEGLView::sharedOpenGLView()
	{
		MGAssert(s_pMainWindow);
		return *s_pMainWindow;
	}

}
