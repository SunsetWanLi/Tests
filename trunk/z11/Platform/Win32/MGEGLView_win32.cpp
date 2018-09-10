#ifndef _WIN32_WINNT
//#define _WIN32_WINNT 0x0400   //for mouse wheel
#define _WIN32_WINNT 0x0501
#endif
#define DIRECT_OPENGL
#include "../MGPlatformMacros.h"
#include "../MGEGLView_platform.h"
#include "../MGApplication_win32.h"
#include <Windowsx.h>

#include <set>
#include "../MGGL.h"
#ifdef DIRECT_OPENGL
#include <GL/glew.h>
#else
#include <EGL/egl.h>
#include <GLES/gl.h>
#endif

#include "../../System/Shared/MGLog.h"
#include "../../GUI/Events/MIEvent.h"
#include "../../GUI/Events/MITouch.h"

#include "../../GUI/GuiContainer/GuiContainer.h"
#include "../../GUI/Controllers/MIViewController.h"
#include "../../GUI/LightGraphic/LightGraphic.h"
#include "../../System/Shared/Utility.h"
#include "../../System/Device/MGDevice.h"
#include "../../System/ClipBoard/MGClipBoard.h"
#include "../MGMemoryInfo.h"

#include "MCF/GenericHelpers.h"
#include "OpenGLThreadClass.h"

#ifdef DIRECT_OPENGL
	#pragma comment(lib, "glew32.lib")
	#pragma comment(lib, "opengl32.lib")
#else
	//#define SOFT_EMULATE
	#ifdef SOFT_EMULATE
	#pragma comment(lib, "libGLESv1_CM.lib")
	#pragma comment(lib, "libEGL.lib")
	#else
	#pragma comment(lib, "libgles_cm.lib")
	#pragma comment(lib, "libEGL.lib")
	#endif
#endif

/*
** Convert a UTF-8 string to microsoft unicode (UTF-16?). 
**
** Space to hold the returned string is obtained from malloc.
*/
WCHAR *win32_utf8ToUnicode(const char *zFilename){
	size_t nChar = 0;
	WCHAR *zWideFilename;

	nChar = MultiByteToWideChar(CP_UTF8, 0, zFilename, -1, NULL, 0);
	zWideFilename = (WCHAR*)malloc((nChar+1)*sizeof(WCHAR) );
	if( zWideFilename==0 ){
		return 0;
	}
	wmemset(zWideFilename,0,nChar+1);
	nChar = MultiByteToWideChar(CP_UTF8, 0, zFilename, -1, zWideFilename, nChar);
	if( nChar==0 ){
		free(zWideFilename);
		zWideFilename = 0;
	}
	return zWideFilename;
}

/*
** Convert microsoft unicode to UTF-8.  Space to hold the returned string is
** obtained from malloc().
*/
char *win32_unicodeToUtf8(const WCHAR *zWideFilename){
	size_t nByte = 0;
	char *zFilename;

	nByte = WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, 0, 0, 0, 0);
	zFilename = (char*)malloc( nByte+1 );
	if( zFilename==0 ){
		return 0;
	}
	memset(zFilename,0,nByte+1);
	nByte = WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, zFilename, nByte,
		0, 0);
	if( nByte == 0 ){
		free(zFilename);
		zFilename = 0;
	}
	return zFilename;
}

/*
** Convert an ansi string to microsoft unicode, based on the
** current codepage settings for file apis.
** 
** Space to hold the returned string is obtained
** from malloc.
*/
WCHAR *win32_mbcsToUnicode(const char *zFilename){
	size_t nByte = 0;
	WCHAR *zMbcsFilename;
	//int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	int codepage = CP_ACP;

	nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, NULL,0);
	zMbcsFilename = (WCHAR *)malloc((nByte+1)*sizeof(WCHAR) );
	if( zMbcsFilename==0 ){
		return 0;
	}
	wmemset(zMbcsFilename, 0, nByte+1);
	nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, zMbcsFilename, nByte);
	if( nByte==0 ){
		free(zMbcsFilename);
		zMbcsFilename = 0;
	}
	return zMbcsFilename;
}

/*
** Convert microsoft unicode to multibyte character string, based on the
** user's Ansi codepage.
**
** Space to hold the returned string is obtained from
** malloc().
*/
char *win32_unicodeToMbcs(const WCHAR *zWideFilename){
	size_t nByte = 0;
	char *zFilename;
	//int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	int codepage = CP_ACP;

	nByte = WideCharToMultiByte(codepage, 0, zWideFilename, -1, 0, 0, 0, 0);
	zFilename = (char*)malloc( nByte+1 );
	if( zFilename==0 ){
		return 0;
	}
	memset(zFilename, 0, nByte+1);
	nByte = WideCharToMultiByte(codepage, 0, zWideFilename, -1, zFilename, nByte,
		0, 0);
	if( nByte == 0 ){
		free(zFilename);
		zFilename = 0;
	}
	return zFilename;
}

/*
** Convert multibyte character string to UTF-8.  Space to hold the
** returned string is obtained from malloc().
*/
char *win32_mbcs_to_utf8(const char *zFilename){
	char *zFilenameUtf8;
	WCHAR *zTmpWide;

	zTmpWide = win32_mbcsToUnicode(zFilename);
	if( zTmpWide==0 ){
		return 0;
	}
	zFilenameUtf8 = win32_unicodeToUtf8(zTmpWide);
	free(zTmpWide);
	return zFilenameUtf8;
}

/*
** Convert UTF-8 to multibyte character string.  Space to hold the 
** returned string is obtained from malloc().
*/
char *win32_utf8_to_mbcs(const char *zFilename){
	char *zFilenameMbcs;
	WCHAR *zTmpWide;

	zTmpWide = win32_utf8ToUnicode(zFilename);
	if( zTmpWide==0 ){
		return 0;
	}
	zFilenameMbcs = win32_unicodeToMbcs(zTmpWide);
	free(zTmpWide);
	return zFilenameMbcs;
}

namespace z11
{
/*
	//////////////////////////////////////////////////////////////////////////
	// impliment MGEGL
	//////////////////////////////////////////////////////////////////////////
#ifdef DIRECT_OPENGL
	static void SetupPixelFormat(HDC hDC)
	{
		int pixelFormat;

		PIXELFORMATDESCRIPTOR pfd =
		{   
			sizeof(PIXELFORMATDESCRIPTOR),  // size
			1,                          // version
			PFD_SUPPORT_OPENGL |        // OpenGL window
			PFD_DRAW_TO_WINDOW |        // render to window
			PFD_DOUBLEBUFFER,           // support double-buffering
			PFD_TYPE_RGBA,              // color type
			32,                         // prefered color depth
			0, 0, 0, 0, 0, 0,           // color bits (ignored)
			0,                          // no alpha buffer
			0,                          // alpha bits (ignored)
			0,                          // no accumulation buffer
			0, 0, 0, 0,                 // accum bits (ignored)
			16,                         // depth buffer
			0,                          // no stencil buffer
			0,                          // no auxiliary buffers
			PFD_MAIN_PLANE,             // main layer
			0,                          // reserved
			0, 0, 0,                    // no layer, visible, damage masks
		};

		pixelFormat = ChoosePixelFormat(hDC, &pfd);
		SetPixelFormat(hDC, pixelFormat, &pfd);
	}
	class MGEGL
	{
	private:
		HWND m_hWnd;
		HDC m_hDC;
		HGLRC m_hRC;
	public:
		~MGEGL()
		{

		}
		MGEGL(MGEGLView *EGLView)
			:m_EGLView(EGLView)
		{
			m_hWnd=m_EGLView->getHWnd();
			m_hDC=NULL;
			m_hRC=NULL;
		}
		bool init()
		{
			m_hDC = GetDC(m_hWnd);
			SetupPixelFormat(m_hDC);
			//SetupPalette();
			m_hRC = wglCreateContext(m_hDC);
			wglMakeCurrent(m_hDC, m_hRC);

			GLenum GlewInitResult = glewInit();
			if (GLEW_OK != GlewInitResult) 
			{
				fprintf(stderr,"ERROR: %s\n",glewGetErrorString(GlewInitResult));
				return false;
			}

			if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
			{
				printf("Ready for GLSL\n");
			}
			else 
			{
				printf("Not totally ready :( \n");
			}

			if (glewIsSupported("GL_VERSION_2_0"))
			{
				printf("Ready for OpenGL 2.0\n");
			}
			else
			{
				printf("OpenGL 2.0 not supported\n");
			}
		}
		void swapBuffers()
		{
			if (m_hDC != NULL)
			{
				::SwapBuffers(m_hDC);
			}
		}
		static MGEGL *create(MGEGLView *EGLView)
		{
			MGEGL *pEGL=new MGEGL(EGLView);
			pEGL->init();
			return pEGL;

		}
	public:
		MGEGLView *m_EGLView;
	};
#else
 
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
					EGL_RED_SIZE, 8,
					EGL_GREEN_SIZE, 8,
					EGL_BLUE_SIZE, 8,
					EGL_DEPTH_SIZE, 16,
					EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
					EGL_NONE
				};
				EGLint		aEGLContextAttributes[] = {
					EGL_CONTEXT_CLIENT_VERSION, 1,
					EGL_NONE
				};
				EGLint iConfigs;
				EGLConfig eglConfig;
				MG_BREAK_IF(EGL_FALSE == eglChooseConfig(eglDisplay, aConfigAttribs, &eglConfig, 1, &iConfigs) 
					|| (iConfigs != 1));

				EGLContext eglContext;
				eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, aEGLContextAttributes);
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
				MSG msg;
				while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
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

		EGLNativeWindowType     m_eglNativeWindow;
		EGLNativeDisplayType    m_eglNativeDisplay;
		EGLDisplay              m_eglDisplay;
		EGLConfig               m_eglConfig;
		EGLSurface              m_eglSurface;
		EGLContext              m_eglContext;
	};

#endif // DIRECT_OPENGL
*/

	//////////////////////////////////////////////////////////////////////////
	// impliment CCEGLView
	//////////////////////////////////////////////////////////////////////////
	static MGEGLView * s_pMainWindow;
	static const WCHAR * kWindowClassName = L"OnWin32";

	float down_x;
	float down_y;


	 LRESULT CALLBACK MGEGLView::_WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//return  test_(hWnd,uMsg,wParam,lParam);
/*		if (s_pMainWindow && s_pMainWindow->getHWnd() == hWnd)
		{
			return s_pMainWindow->WindowProc(uMsg, wParam, lParam);
		}
		else
		{
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		*/

		if(s_pMainWindow != NULL){
			if(s_pMainWindow->m_hWnd == NULL){
				s_pMainWindow->m_hWnd = hWnd;
			}
			return s_pMainWindow->WindowProc(uMsg, wParam, lParam);
		}
		return 0;
	}

	MGEGLView::MGEGLView()
		: m_bCaptured(false)
		, m_bOrientationReverted(false)
		, m_bOrientationInitVertical(false)
		, m_isSimulateMultiTouch(true)
		, m_is_full_screen(false)
//		, m_pEGL(NULL)
		, m_lpOpenGLThread(NULL)
		, m_hWnd(NULL)
		, m_fScreenScaleFactor(1.0f)
	{
		m_tSizeInPoints.cx = m_tSizeInPoints.cy = 0;
		SetRectEmpty(&m_rcViewPort);
	}

	MGEGLView::~MGEGLView()
	{
		s_pMainWindow = NULL;
	}

	bool MGEGLView::create(LPCTSTR pTitle, int w, int h, uint_8 wnd_type, bool isSimulateMultiTouch)
	{
		if(m_hWnd != NULL){
			return false;
		}

		m_is_full_screen = wnd_type & WINDOWS_FORCE_FULL_SCREEN;
		m_isSimulateMultiTouch = isSimulateMultiTouch;
		s_pMainWindow = this;

		const HINSTANCE hInstance = GetModuleHandle(NULL);

		WNDCLASSEXW WndClassEx;

		WndClassEx.cbSize = sizeof(WNDCLASSEX);
		WndClassEx.style			= 0;
		WndClassEx.lpfnWndProc		= _WindowProc;
		WndClassEx.cbClsExtra		= 0;
		WndClassEx.cbWndExtra		= 0;
		WndClassEx.hInstance		= hInstance;
		WndClassEx.hIcon			= NULL;
		WndClassEx.hCursor			= LoadCursor(NULL, IDC_ARROW);
		WndClassEx.hbrBackground	= NULL;
		WndClassEx.lpszMenuName		= NULL;
		WndClassEx.lpszClassName	= kWindowClassName;
		WndClassEx.hIconSm			= NULL;

		const ATOM atomWndClass = RegisterClassExW(&WndClassEx);

		if(atomWndClass == NULL){
			return false;
		}

		DWORD app_extype = WS_EX_WINDOWEDGE;
		DWORD app_type = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_DLGFRAME | WS_MINIMIZEBOX;
		if (m_is_full_screen || (wnd_type & WINDOWS_POP_UP)) {
			app_extype = 0;
			app_type = WS_POPUP;
		}

		if (!m_is_full_screen && (wnd_type & WINDOWS_CLIENT_EDGE)) {
			app_extype |= WS_EX_CLIENTEDGE;
		}

		const DWORD dwExStyle = WS_EX_APPWINDOW | app_extype;
		const DWORD dwStyle = app_type | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

		const DWORD dwFullExStyle = WS_EX_APPWINDOW | app_extype;
		const DWORD dwFullStyle = app_type | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;

		if (wnd_type & WINDOWS_FULL_SCREEN) {
			RECT rectWindow;
			rectWindow.left = 0;
			rectWindow.top = 0;
			rectWindow.right = w;
			rectWindow.bottom = h;
			AdjustWindowRectEx(&rectWindow, dwFullStyle, FALSE, dwFullExStyle);
			m_hWnd = CreateWindowEx(dwFullExStyle, MAKEINTATOM(atomWndClass), pTitle, dwFullStyle, rectWindow.left, rectWindow.top, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, NULL, NULL, GetModuleHandle(NULL), NULL);
			RECT real_rect;
			GetWindowRect(m_hWnd, &real_rect);
			if (real_rect.bottom < rectWindow.bottom) {
				MoveWindow(m_hWnd, real_rect.left, real_rect.top + (rectWindow.bottom - real_rect.bottom), real_rect.right - real_rect.left, real_rect.bottom - real_rect.top, true);
				MGDevice::setScreenHeight(h + real_rect.bottom - rectWindow.bottom);
			}
		} else {
			RECT rectWindow;
			rectWindow.left		= GetSystemMetrics(SM_CXSCREEN) / 2 - w / 2;
			rectWindow.top		= GetSystemMetrics(SM_CYSCREEN) / 2 - h / 2;
			rectWindow.right = rectWindow.left + w;
			rectWindow.bottom = rectWindow.top + h;
			AdjustWindowRectEx(&rectWindow, dwStyle, FALSE, dwExStyle);
			m_hWnd = CreateWindowEx(dwExStyle, MAKEINTATOM(atomWndClass), pTitle, dwStyle, rectWindow.left, rectWindow.top, rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top, NULL, NULL, GetModuleHandle(NULL), NULL);
		}

		GuiContainer::container->wnd_handle = m_hWnd;
		if(m_hWnd == NULL){
			return false;
		}

		PostMessage(m_hWnd,WM_USER+2,0,0);
		if(wnd_type & WINDOWS_MAX)
			ShowWindow(m_hWnd,SW_MAXIMIZE);
		else
			ShowWindow(m_hWnd, SW_SHOWNORMAL);
		UpdateWindow(m_hWnd);

		m_tSizeInPoints.cx = MGDevice::getScreenWidth();
		m_tSizeInPoints.cy = MGDevice::getScreenHeight();

		return true;
	}
	void initTouch(MITouch *touch,const MGPoint &pt)
	{
		touch->tag=-1;
		touch->maintained_view=NULL;
		touch->saved_point=MGPointMake(0,0);
		touch->point=pt;
		touch->screen_point = pt;
	}
	LRESULT MGEGLView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		static bool pressed=false;
		static bool mid_pressed = false;
		static short zDelta = WHEEL_DELTA;
        
        //		PAINTSTRUCT ps;
        MSG msg;
		msg.hwnd = getHWnd();
		msg.message = message;
		msg.wParam = wParam;
		msg.lParam = lParam;
		if (!MGApplication::sharedApplication().preHandleMessage(msg))
			return 0;

		switch (message)
		{
            case WM_MOUSEWHEEL:
                if (mid_pressed)
                {
                    //short fwKeys = GET_KEYSTATE_WPARAM(wParam);
                    zDelta += GET_WHEEL_DELTA_WPARAM(wParam);
                    
                    short xPos = MGDevice::getScreenWidth()/2;//GET_Y_LPARAM(lParam);
                    short yPos =  MGDevice::getScreenHeight()/2;//GET_X_LPARAM(lParam);
                    short range=50;
                    
                    std::set<MITouch*> touches;
                    MITouch *touch[2];
                    for (int i=0;i<2;i++)
                    {
                        touch[i]=new MITouch();
                        touches.insert(touch[i]);
                    }
                    MGPoint pre_point1=MGPointMake(xPos-range,yPos-range);
                    MGPoint pre_point2=MGPointMake(xPos+range,yPos+range);
                    pre_point1.x-=10*(zDelta/WHEEL_DELTA);
                    pre_point1.y-=10*(zDelta/WHEEL_DELTA);
                    initTouch(touch[0],pre_point1);
                    initTouch(touch[1],pre_point2);
                    touch[0]->phase=MITouchPhaseMoved;
                    touch[1]->phase=MITouchPhaseMoved;
                    GuiContainer::container->handleTouchesEvent(touches,MITouchEventMoved);
                }
                break;
            case WM_LBUTTONDOWN:
                if (!pressed)
                {
                    pressed=true;
                    // 考虑缩放值 m_fScreenScaleFactor
                    std::set<MITouch*> touches;
                    MITouch *mi_touch=new MITouch();
                    mi_touch->point.x=(float)(lParam&0xffff);
                    mi_touch->point.y=(float)(lParam>>16);
                    down_x = mi_touch->point.x;
                    down_y = mi_touch->point.y;
                    touches.insert(mi_touch);
                    GuiContainer::container->handleTouchesEvent(touches,MITouchEventBegan);
                }
                break;
                
            case WM_MOUSEMOVE:
                if(pressed && (wParam &MK_LBUTTON) != 0)
                {
                    static int first_move = 1;
                    float move_x = (float)(lParam&0xffff);
                    float move_y = (float)(lParam>>16);
                    if (move_x == down_x && move_y == down_y && first_move == 1)
                    {
                        break;
                    }
                    first_move = 0;
                    
                    std::set<MITouch*> touches;
                    MITouch *mi_touch=new MITouch();
                    mi_touch->point.x=(float)(lParam&0xffff);
                    mi_touch->point.y=(float)(lParam>>16);
                    touches.insert(mi_touch);
                    GuiContainer::container->handleTouchesEvent(touches,MITouchEventMoved);
                }else if(pressed) {
                    pressed=false;
                    std::set<MITouch*> touches;
                    MITouch *mi_touch=new MITouch();
                    mi_touch->point.x=(float)(lParam&0xffff);
                    mi_touch->point.y=(float)(lParam>>16);
                    touches.insert(mi_touch);
                    GuiContainer::container->handleTouchesEvent(touches,MITouchEventEnded);
                }
                break;
                
            case WM_LBUTTONUP:
                if (pressed) {
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
                if (wParam == VK_BACK)
                {
					MIEvent* event = new MIEvent();
                    event->_event_type = KEYBOARD_DELETE_BACKWARD;
                    event->_responder = NULL;
                    event->_content = NULL;
                    GuiContainer::container->handleSystemEvent(event);
                } else if (wParam != VK_ESCAPE) {
                    MIEvent* event = new MIEvent();
                    MGTextContent* content = new MGTextContent();
                    wchar_t wtext[]={(wchar_t)wParam,0};
                    char text[8]={0};
					content->text_content = Utility::wstr2ustr(wtext);
                    event->_event_type = KEYBOARD_INPUT;
                    event->_content = content;
                    event->_responder = NULL;
                    GuiContainer::container->handleSystemEvent(event);
                }
                break;
                
            case WM_KEYDOWN:
                if (wParam==VK_ESCAPE )
                {
                    //				exit(0);
                    //PostQuitMessage(1);
                    
                    SendMessage(m_hWnd, WM_SYSCOMMAND, MAKEWPARAM(SC_CLOSE, 0), 0);
                }
                else if (wParam==VK_CONTROL && m_isSimulateMultiTouch)
                {
                    if (!mid_pressed)
                    {
                        MGLogD("VK_CONTROL Key Down");
                        mid_pressed = true;
                        short xPos = MGDevice::getScreenWidth()/2;//GET_Y_LPARAM(lParam);
                        short yPos =  MGDevice::getScreenHeight()/2;//GET_X_LPARAM(lParam);
                        short range=50;
                        zDelta = 120;
                        // MGLogD("=================================WM_MOUSEHWHEEL===============================");
                        
                        std::set<MITouch*> touches;
                        MITouch *touch[2];
                        for (int i=0;i<2;i++)
                        {
                            touch[i]=new MITouch();
                            touches.insert(touch[i]);
                        }
                        MGPoint pre_point1=MGPointMake(xPos-range,yPos-range);
                        MGPoint pre_point2=MGPointMake(xPos+range,yPos+range);
                        initTouch(touch[0],pre_point1);
                        initTouch(touch[1],pre_point2);
                        touch[0]->phase=MITouchPhaseBegan;
                        touch[1]->phase=MITouchPhaseBegan;
                        GuiContainer::container->handleTouchesEvent(touches,MITouchEventBegan);
                    }
                }
                break;
            case WM_KEYUP:
                if (wParam==VK_CONTROL && m_isSimulateMultiTouch)
                {
                    if(mid_pressed)
                    {
                        MGLogD("VK_CONTROL Key up");
                        mid_pressed = false;
                        short xPos = MGDevice::getScreenWidth()/2;//GET_Y_LPARAM(lParam);
                        short yPos =  MGDevice::getScreenHeight()/2;//GET_X_LPARAM(lParam);
                        short range=50;
                        std::set<MITouch*> touches;
                        MITouch *touch[2];
                        for (int i=0;i<2;i++)
                        {
                            touch[i]=new MITouch();
                            touches.insert(touch[i]);
                        }
                        MGPoint pre_point1=MGPointMake(xPos-range,yPos-range);
                        MGPoint pre_point2=MGPointMake(xPos+range,yPos+range);
                        pre_point1.x-=10*(zDelta/120);
                        pre_point1.y-=10*(zDelta/120);
                        initTouch(touch[0],pre_point1);
                        initTouch(touch[1],pre_point2);
                        touch[0]->phase=MITouchPhaseEnded;
                        touch[1]->phase=MITouchPhaseEnded;
                        GuiContainer::container->handleTouchesEvent(touches,MITouchEventEnded);
                    }
                }
                break;
            case WM_DESTROY:
				MGLogD("WM_DESTROY ING");
				GuiContainer::container->applicationWillExit();
                m_lpOpenGLThread->PostQuitMsg(0);
                m_lpOpenGLThread->WaitAndGetExitCode();
                delete m_lpOpenGLThread;
                m_lpOpenGLThread = NULL;
                //PostQuitMessage(0);
				GuiContainer::container->light_graphic->requestExitAndWait();
				MGLogD("WM_DESTROY ED");
                break;
            case WM_QUIT:
				MGLogD("WM_QUIT");
				
				//ExitProcess(0);
                break;
            case WM_SETFOCUS:
               { //HWND hWnd = GetSafeHwnd(); // 获取安全窗口句柄
				   ::OpenClipboard(m_hWnd); // 打开剪贴板
				   HANDLE hClipMemory = ::GetClipboardData(CF_TEXT);// 获取剪贴板数据句柄
				   if (hClipMemory != NULL)
				   {
						DWORD dwLength = GlobalSize(hClipMemory); // 返回指定内存区域的当前大小
						LPBYTE lpClipMemory = (LPBYTE)GlobalLock(hClipMemory); // 锁定内存
						if (lpClipMemory != NULL)
						{
							char* utf8_str = win32_mbcs_to_utf8((char*)lpClipMemory);
							MGClipBoard::setString(utf8_str); // 保存得到的文本数据
							free(utf8_str);
							utf8_str = NULL;
						}
						GlobalUnlock(hClipMemory); // 内存解锁
				   }
				   ::CloseClipboard(); // 关闭剪贴板
				   if (GuiContainer::container != NULL && GuiContainer::container->gui_root != NULL) {
					   GuiContainer::container->gui_root->applicationWillEnterForeground();
					   GuiContainer::container->gui_root->applicationDidBecomeActive();
				   }
				   if (m_is_full_screen)
				   {
						HWND task;
						task=FindWindow(L"Shell_TrayWnd",NULL);  
						ShowWindow(task,SW_HIDE);//隐藏任务栏
				   }
			   }
                break;
            case WM_KILLFOCUS:
				{
					string clip_str = MGClipBoard::getString();
					int clip_len = clip_str.length();
					if (clip_len > 0)
					{
						char* mbcs_str = win32_utf8_to_mbcs(clip_str.c_str());
						HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, clip_len);
						memcpy(GlobalLock(hMem), mbcs_str, clip_len);
						free(mbcs_str);
						mbcs_str = NULL;
						GlobalUnlock(hMem);
						OpenClipboard(0);
						//EmptyClipboard();
						SetClipboardData(CF_TEXT, hMem);
						CloseClipboard();
					}
					if (GuiContainer::container != NULL && GuiContainer::container->gui_root != NULL) {
						GuiContainer::container->gui_root->applicationWillResignActive();
						GuiContainer::container->gui_root->applicationDidEnterBackground();
					}
					if (m_is_full_screen)
					{
						HWND task;
						task=FindWindow(L"Shell_TrayWnd",NULL);  
						ShowWindow(task,SW_SHOW);//显示任务栏
					}
				}
                break;
            case WM_USER+1:
                GuiContainer::container->handleEvent();
                break;
			case WM_USER+2:
                m_lpOpenGLThread = new OpenGLThreadClass(m_hWnd, m_tSizeInPoints.cx, m_tSizeInPoints.cy);
                m_lpOpenGLThread->Create(true);
				m_lpOpenGLThread->SetPriority(THREAD_PRIORITY_TIME_CRITICAL);
				m_lpOpenGLThread->Resume();
                break;
			case WM_USER+3:
				onPause();
                break;
			case WM_USER+4:
				onResume();
                break;
            default:
                //MGLogD("message is %d",message);
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
//		return (NULL != m_pEGL);
		return m_lpOpenGLThread != NULL;
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

//		MG_SAFE_DELETE(m_pEGL);
// liuhao: The OpenGL thread object was deleted in WM_DESTROY handler.
//		delete m_lpOpenGLThread;

		delete this;
	}



	void MGEGLView::swapBuffers()
	{
/*		if (m_pEGL)
		{
			m_pEGL->swapBuffers();
		}
*/
		const HDC hDC = ::GetDC(m_hWnd);
		::SwapBuffers(hDC);
		::ReleaseDC(m_hWnd, hDC);
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

/*	void MGEGLView::setViewPortInPoints(float x, float y, float w, float h)
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
	}*/

	void MGEGLView::setIMEKeyboardState(bool /*bOpen*/)
	{
	}

	HWND MGEGLView::getHWnd()
	{
		return m_hWnd;
	}

/*	void MGEGLView::resize(int width, int height)
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
			rcClient.bottom - rcClient.top, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

		if (m_pEGL)
		{
			//m_pEGL->resizeSurface();
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
		if (! m_hWnd)
		{
			return;
		}

		RECT rcDesktop, rcWindow;
		GetWindowRect(GetDesktopWindow(), &rcDesktop);

		// substract the task bar
		HWND hTaskBar = FindWindow(TEXT("Shell_TrayWnd"), NULL);
		if (hTaskBar != NULL)
		{
			APPBARDATA abd;

			abd.cbSize = sizeof(APPBARDATA);
			abd.hWnd = hTaskBar;

			SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
			SubtractRect(&rcDesktop, &rcDesktop, &abd.rc);
		}
		GetWindowRect(m_hWnd, &rcWindow);

		int offsetX = (rcDesktop.right - rcDesktop.left - (rcWindow.right - rcWindow.left)) / 2;
		offsetX = (offsetX > 0) ? offsetX : rcDesktop.left;
		int offsetY = (rcDesktop.bottom - rcDesktop.top - (rcWindow.bottom - rcWindow.top)) / 2;
		offsetY = (offsetY > 0) ? offsetY : rcDesktop.top;

		SetWindowPos(m_hWnd, 0, offsetX, offsetY, 0, 0, SWP_NOCOPYBITS | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
*/
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
/*		m_fScreenScaleFactor = contentScaleFactor;
		if (m_bOrientationReverted)
		{
			resize((int)(m_tSizeInPoints.cy * contentScaleFactor), (int)(m_tSizeInPoints.cx * contentScaleFactor));
		}
		else
		{
			resize((int)(m_tSizeInPoints.cx * contentScaleFactor), (int)(m_tSizeInPoints.cy * contentScaleFactor));
		}
		centerWindow();*/
	}

	MGEGLView& MGEGLView::sharedOpenGLView()
	{
		MGAssert(s_pMainWindow);
		return *s_pMainWindow;
	}

	void MGEGLView::handleEventImmediatelly(MIEvent* event)
	{
		GuiContainer::container->handleEventImmediatelly(event);
	}

	void MGEGLView::onPause()
	{
		if (m_lpOpenGLThread != NULL)
			m_lpOpenGLThread->Suspend();
		m_lpOpenGLThread->Release();
	}

	void MGEGLView::onResume()
	{
		if (m_lpOpenGLThread != NULL && m_lpOpenGLThread->IsSuspended())
			m_lpOpenGLThread->Resume();
	}

}
