#include "../MGPlatformMacros.h"
#include "../MGEGLView_platform.h"

#include <set>
#include <GLES/gl.h>
#include <EGL/egl.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include "../../System/Shared/MGLog.h"
#include "../../GUI/Events/MIEvent.h"
#include "../../GUI/Events/MITouch.h"

#include "../../GUI/GuiContainer/GuiContainer.h"
#include "../../System/Shared/Utility.h"
#include "../../System/Device/MGDevice.h"
namespace z11
{

//////////////////////////////////////////////////////////////////////////
// impliment MGEGL
//////////////////////////////////////////////////////////////////////////

static const char *sAppName=NULL;

class MGEGL
{
public:
    ~MGEGL()
    {
        /* EGL clean up */
        eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(m_eglDisplay, m_eglContext);
        eglDestroySurface(m_eglDisplay, m_eglSurface);
        eglTerminate(m_eglDisplay);

        /* X Windows clean up */
        XCloseDisplay(sDisplay);
        XDestroyWindow(sDisplay, sWindow);
        XFreeColormap(sDisplay, colormap);
        XFree(vi);
    }

    static MGEGL * create(MGEGLView * pWindow)
    {
        MGEGL * pEGL = new MGEGL;
        GLint width = MGDevice::getScreenWidth() , height = MGDevice::getScreenHeight();
        EGLint attributeList[] = { EGL_RED_SIZE, 1, EGL_DEPTH_SIZE, 1, EGL_NONE };
        EGLint		aEGLAttributes[] =
        {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
            EGL_NONE
        };
        EGLint		aEGLContextAttributes[] =
        {
            EGL_CONTEXT_CLIENT_VERSION, 1,
            EGL_NONE
        };
        EGLConfig m_eglConfig[1];
        EGLint nConfigs;

        /* Open X Display */
        pEGL->sDisplay = XOpenDisplay(NULL);
        if(pEGL->sDisplay == NULL)
        {
            /* Environment variable DISPLAY is not set */
            printf("Display not set.\n");
            delete_and_set_null(pEGL);
            return NULL;
            //deinitGraphics();
        }
        printf("Native Display created");

        XSynchronize(pEGL->sDisplay, True);

        /* EGL initialise */
        pEGL->m_eglDisplay = eglGetDisplay(pEGL->sDisplay);
        eglInitialize(pEGL->m_eglDisplay, NULL, NULL);
        eglChooseConfig(pEGL->m_eglDisplay, aEGLAttributes, m_eglConfig, 1, &nConfigs);

        /* Create X window */
        {
            XSetWindowAttributes swa;
            XVisualInfo  tmp;
            XSizeHints sh;
            int n;
            EGLint vID;


            eglGetConfigAttrib(pEGL->m_eglDisplay, m_eglConfig[0], EGL_NATIVE_VISUAL_ID, &vID);
            tmp.visualid = vID;
            pEGL->vi = XGetVisualInfo(pEGL->sDisplay, VisualIDMask, &tmp, &n);
            pEGL->colormap = XCreateColormap(pEGL->sDisplay,
                                       RootWindow(pEGL->sDisplay, pEGL->vi->screen),
                                       pEGL->vi->visual,AllocNone);
            swa.colormap = pEGL->colormap;
            swa.background_pixel = 0xFFFFFFFF;
            swa.border_pixel = 0;
            swa.event_mask = StructureNotifyMask | ExposureMask|
                             KeyPressMask | ButtonPressMask | ButtonReleaseMask |ButtonMotionMask;

            sh.flags = PMinSize | PMaxSize;
            sh.min_width = sh.max_width = width;
            sh.min_height = sh.max_height = height;

            pEGL->sWindow = XCreateWindow(pEGL->sDisplay,RootWindow(pEGL->sDisplay, pEGL->vi->screen),
                                    0,0,width, height,
                                    0,pEGL->vi->depth,InputOutput,pEGL->vi->visual,
                                    CWBackPixel | CWBorderPixel | CWEventMask | CWColormap,
                                    &swa);


            XMapWindow(pEGL->sDisplay, pEGL->sWindow);
            XSetStandardProperties(pEGL->sDisplay, pEGL->sWindow, sAppName, sAppName,
                                   None, 0, 0, &sh);
        }
        if (!pEGL->sWindow)
        {
            /* Failed to create X window */
            printf("Failed to create X window.\n");
            delete_and_set_null(pEGL);
            return NULL;
            //deinitGraphics();
        }
        printf("Created native window.\n");

        pEGL->m_eglSurface = eglCreateWindowSurface(pEGL->m_eglDisplay, m_eglConfig[0], pEGL->sWindow, 0);
        pEGL->m_eglContext = eglCreateContext(pEGL->m_eglDisplay, m_eglConfig[0],
                                        EGL_NO_CONTEXT, aEGLContextAttributes);
        printf("EGLContext = %p\n", pEGL->m_eglContext);
        eglMakeCurrent(pEGL->m_eglDisplay, pEGL->m_eglSurface, pEGL->m_eglSurface, pEGL->m_eglContext);
        return pEGL;
    }

    void resizeSurface()
    {
    }

    void swapBuffers()
    {
        if (EGL_NO_DISPLAY != m_eglDisplay)
        {
            eglSwapBuffers(m_eglDisplay, m_eglSurface);
        }
    }
    Display *getDisplay()
    {
        return sDisplay;
    }
private:
    MGEGL()
        : m_eglNativeWindow(NULL)
        , m_eglNativeDisplay(EGL_DEFAULT_DISPLAY)
        , m_eglDisplay(EGL_NO_DISPLAY)
        , m_eglConfig(0)
        , m_eglSurface(EGL_NO_SURFACE)
        , m_eglContext(EGL_NO_CONTEXT)
        ,sDisplay(NULL)
        ,sWindow(0)
        ,colormap(0)
        ,vi(NULL)
    {}
private:
    EGLNativeWindowType     m_eglNativeWindow;
    EGLNativeDisplayType    m_eglNativeDisplay;
    EGLDisplay              m_eglDisplay;
    EGLConfig               m_eglConfig;
    EGLSurface              m_eglSurface;
    EGLContext              m_eglContext;
    Display *sDisplay;
    Window sWindow;
    Colormap colormap;
    XVisualInfo *vi;


};
//////////////////////////////////////////////////////////////////////////
// impliment CCEGLView
//////////////////////////////////////////////////////////////////////////
static MGEGLView * s_pMainWindow;
static const char * kWindowClassName = "OnLinux";

float down_x;
float down_y;

MGEGLView::MGEGLView()
    : m_bCaptured(false)
    , m_bOrientationReverted(false)
    , m_bOrientationInitVertical(false)
    , m_pEGL(NULL)
    , m_pDisplay(NULL)
    , m_fScreenScaleFactor(1.0f)
{
}

MGEGLView::~MGEGLView()
{
}

bool MGEGLView::create(const char* pTitle, int w, int h)
{
    bool bRet = false;
    do
    {
        MG_BREAK_IF(m_pDisplay);

//        HINSTANCE hInstance = GetModuleHandle( NULL );
//        WNDCLASS  wc;		// Windows Class Structure
//
//        // Redraw On Size, And Own DC For Window.
//        wc.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
//        wc.lpfnWndProc    = _WindowProc;					// WndProc Handles Messages
//        wc.cbClsExtra     = 0;                              // No Extra Window Data
//        wc.cbWndExtra     = 0;								// No Extra Window Data
//        wc.hInstance      = hInstance;						// Set The Instance
//        wc.hIcon          = LoadIcon( NULL, IDI_WINLOGO );	// Load The Default Icon
//        wc.hCursor        = LoadCursor( NULL, IDC_ARROW );	// Load The Arrow Pointer
//        wc.hbrBackground  = NULL;                           // No Background Required For GL
//        wc.lpszMenuName   = NULL;                           // We Don't Want A Menu
//        wc.lpszClassName  = kWindowClassName;               // Set The Class Name
//
//        MG_BREAK_IF(! RegisterClass(&wc) && 1410 != GetLastError());
//
//        // center window position
//        RECT rcDesktop;
//        GetWindowRect(GetDesktopWindow(), &rcDesktop);
//
//        // create window
//        m_hWnd = CreateWindowEx(
//                     WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,	// Extended Style For The Window
//                     kWindowClassName,									// Class Name
//                     pTitle,												// Window Title
//                     WS_OVERLAPPEDWINDOW 	| WS_CLIPSIBLINGS | WS_CLIPCHILDREN,	// Defined Window Style
//                     0, 0,								                // Window Position
//                     w,                                                  // Window Width
//                     h,                                                  // Window Height
//                     NULL,												// No Parent Window
//                     NULL,												// No Menu
//                     hInstance,											// Instance
//                     NULL );
//
//        MG_BREAK_IF(! m_hWnd);
//
//        m_tSizeInPoints.cx = w;
//        m_tSizeInPoints.cy = h;
//        resize(w, h);
        sAppName=pTitle;
        // init egl
        m_pEGL = MGEGL::create(this);
        m_pDisplay=m_pEGL->getDisplay();
        m_tSizeInPoints.width=MGDevice::getScreenWidth();
        m_tSizeInPoints.height=MGDevice::getScreenHeight();

//        if (! m_pEGL)
//        {
//            DestroyWindow(m_hWnd);
//            m_hWnd = NULL;
//            break;
//        }

        s_pMainWindow = this;
        bRet = true;
    }
    while (0);

    return bRet;
}

void MGEGLView::WindowProc(const XEvent& event)
{
    static bool pressed=false;
	MGLogD("MGEGLView::WindowProc");
    switch (event.type)
    {
     case ButtonPress:
    {
        pressed=true;
        //¿¼ÂÇËõ·ÅÖµ m_fScreenScaleFactor
        std::set<MITouch*> touches;
        MITouch *mi_touch=new MITouch();
        mi_touch->point.x=(float)(event.xbutton.x);
        mi_touch->point.y=(float)(event.xbutton.y);
        down_x = mi_touch->point.x;
        down_y = mi_touch->point.y;
        touches.insert(mi_touch);
        GuiContainer::container->handleTouchesEvent(touches,MITouchEventBegan);

    }
    break;

    case MotionNotify:
        //break;
        if(pressed)
        {
            static int first_move = 1;
            float move_x = (float)(event.xbutton.x);
            float move_y = (float)(event.xbutton.y);
            MGLogD("move_x=%d",move_x);
            MGLogD("move_y=%d",move_y);
			if (move_x == down_x && move_y == down_y && first_move == 1)
            {
                break;
            }
            first_move = 0;

            std::set<MITouch*> touches;
            MITouch *mi_touch=new MITouch();
            mi_touch->point.x=(float)(event.xbutton.x);
            mi_touch->point.y=(float)(event.xbutton.y);
            touches.insert(mi_touch);
            GuiContainer::container->handleTouchesEvent(touches,MITouchEventMoved);
        }
        break;

    case ButtonRelease:
    {
        pressed=false;
        std::set<MITouch*> touches;
        MITouch *mi_touch=new MITouch();
        mi_touch->point.x=(float)(event.xbutton.x);
        mi_touch->point.y=(float)(event.xbutton.y);
        touches.insert(mi_touch);
        GuiContainer::container->handleTouchesEvent(touches,MITouchEventEnded);

    }
    break;

//    case WM_CHAR:
//    {
//        if (wParam!=8)
//        {
//            MIEvent* event = new MIEvent();
//            MGTextContent* content = new MGTextContent();
//            wchar_t wtext[]= {wParam,0};
//            char text[8]= {0};
//            content->text_content = string(Utility::unicode_to_utf8(text,wtext));
//            event->_event_type = KEYBOARD_INPUT;
//            event->_content = content;
//            event->_responder = NULL;
//            GuiContainer::container->handleSystemEvent(event);
//        }
//        else
//        {
//            MIEvent* event = new MIEvent();
//            event->_event_type = KEYBOARD_DELETE_BACKWARD;
//            event->_responder = NULL;
//            event->_content = NULL;
//            GuiContainer::container->handleSystemEvent(event);
//        }
//    }
//    break;

    case KeyPress:
        unsigned int keycode, keysym;
        keycode = ((XKeyEvent *)&event)->keycode;
        keysym = XKeycodeToKeysym(m_pEGL->getDisplay(), keycode, 0);
        if (keysym == XK_Return || keysym == XK_Escape)
                    ;//break;
        break;
    }
}

MGSize MGEGLView::getSize()
{
    if (m_bOrientationReverted)
    {
        return MGSize((float)(m_tSizeInPoints.width), (float)(m_tSizeInPoints.height));
    }
    return MGSize((float)(m_tSizeInPoints.width), (float)(m_tSizeInPoints.height));
}

bool MGEGLView::isOpenGLReady()
{
    return (NULL != m_pEGL);
}

void MGEGLView::release()
{
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
        glViewport((GLint)(x * factor) + m_rcViewPort.x,
                   (GLint)(y * factor) + m_rcViewPort.y,
                   (GLint)(w * factor),
                   (GLint)(h * factor));
    }
}

void MGEGLView::setScissorInPoints(float x, float y, float w, float h)
{
    if (m_pEGL)
    {
        float factor = 1.0f;//m_fScreenScaleFactor / CC_CONTENT_SCALE_FACTOR();
        glScissor((GLint)(x * factor) + m_rcViewPort.x,
                  (GLint)(y * factor) + m_rcViewPort.y,
                  (GLint)(w * factor),
                  (GLint)(h * factor));
    }
}

void MGEGLView::setIMEKeyboardState(bool /*bOpen*/)
{
}

Display* MGEGLView::getDisplay()
{
    if(m_pEGL)
        return m_pEGL->getDisplay();
    return NULL;
}

void MGEGLView::resize(int width, int height)
{
//    if (! m_hWnd)
//    {
//        return;
//    }
//
//    RECT rcClient;
//    GetClientRect(m_hWnd, &rcClient);
//    if (rcClient.right - rcClient.left == width &&
//            rcClient.bottom - rcClient.top == height)
//    {
//        return;
//    }
//    // calculate new window width and height
//    rcClient.right = rcClient.left + width;
//    rcClient.bottom = rcClient.top + height;
//    AdjustWindowRectEx(&rcClient, GetWindowLong(m_hWnd, GWL_STYLE), false, GetWindowLong(m_hWnd, GWL_EXSTYLE));
//
//    // change width and height
//    SetWindowPos(m_hWnd, 0, 0, 0, rcClient.right - rcClient.left,
//                 rcClient.bottom - rcClient.top, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
//
//    if (m_pEGL)
//    {
//        m_pEGL->resizeSurface();
//    }
//
//    // calculate view port in pixels
//    int viewPortW = (int)(m_tSizeInPoints.cx * m_fScreenScaleFactor);
//    int viewPortH = (int)(m_tSizeInPoints.cy * m_fScreenScaleFactor);
//    if (m_bOrientationReverted)
//    {
//        int tmp = viewPortW;
//        viewPortW = viewPortH;
//        viewPortH = tmp;
//    }
//    GetClientRect(m_hWnd, &rcClient);
//
//    // calculate client new width and height
//    int newW = rcClient.right - rcClient.left;
//    int newH = rcClient.bottom - rcClient.top;
//
//    // calculate new view port
//    m_rcViewPort.left   = rcClient.left + (newW - viewPortW) / 2;
//    m_rcViewPort.top    = rcClient.top + (newH - viewPortH) / 2;
//    m_rcViewPort.right  = m_rcViewPort.left + viewPortW;
//    m_rcViewPort.bottom = m_rcViewPort.top + viewPortH;
}

void MGEGLView::centerWindow()
{
//    if (! m_hWnd)
//    {
//        return;
//    }
//
//    RECT rcDesktop, rcWindow;
//    GetWindowRect(GetDesktopWindow(), &rcDesktop);
//
//    // substract the task bar
//    HWND hTaskBar = FindWindow(TEXT("Shell_TrayWnd"), NULL);
//    if (hTaskBar != NULL)
//    {
//        APPBARDATA abd;
//
//        abd.cbSize = sizeof(APPBARDATA);
//        abd.hWnd = hTaskBar;
//
//        SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
//        SubtractRect(&rcDesktop, &rcDesktop, &abd.rc);
//    }
//    GetWindowRect(m_hWnd, &rcWindow);
//
//    int offsetX = (rcDesktop.right - rcDesktop.left - (rcWindow.right - rcWindow.left)) / 2;
//    offsetX = (offsetX > 0) ? offsetX : rcDesktop.left;
//    int offsetY = (rcDesktop.bottom - rcDesktop.top - (rcWindow.bottom - rcWindow.top)) / 2;
//    offsetY = (offsetY > 0) ? offsetY : rcDesktop.top;
//
//    SetWindowPos(m_hWnd, 0, offsetX, offsetY, 0, 0, SWP_NOCOPYBITS | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER);
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
        resize((int)(m_tSizeInPoints.width * contentScaleFactor), (int)(m_tSizeInPoints.width * contentScaleFactor));
    }
    else
    {
        resize((int)(m_tSizeInPoints.height* contentScaleFactor), (int)(m_tSizeInPoints.height * contentScaleFactor));
    }
    centerWindow();
}

MGEGLView& MGEGLView::sharedOpenGLView()
{
    MGAssert(s_pMainWindow);
    return *s_pMainWindow;
}

}
