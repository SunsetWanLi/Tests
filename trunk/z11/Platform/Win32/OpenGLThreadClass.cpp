#include "OpenGLThreadClass.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
#include "../../GUI/LightGraphic/LightGraphic.h"
#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif
using namespace z11;

// 构造函数和析构函数。
OpenGLThreadClass::OpenGLThreadClass(HWND hOwner, long lScreenWidth, long lScreenHeight) :
	_m_hOwner			(hOwner),
	_m_lScreenWidth		(lScreenWidth),
	_m_lScreenHeight	(lScreenHeight)
{
}
OpenGLThreadClass::~OpenGLThreadClass(){
}

// 其他非静态成员函数。
void OpenGLThreadClass::_PostCreate(){
	// OpenGL 的初始化操作。
	const HDC hDC = GetDC(_m_hOwner);

	PIXELFORMATDESCRIPTOR pfd;
	pfd.nSize			= sizeof(pfd);
	pfd.nVersion		= 1;
	pfd.dwFlags			= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType		= PFD_TYPE_RGBA;
	pfd.cColorBits		= 32;
	pfd.cRedBits		= 0;
	pfd.cRedShift		= 0;
	pfd.cGreenBits		= 0;
	pfd.cGreenShift		= 0;
	pfd.cBlueBits		= 0;
	pfd.cBlueShift		= 0;
	pfd.cAlphaBits		= 0;
	pfd.cAlphaShift		= 0;
	pfd.cAccumBits		= 0;
	pfd.cAccumRedBits	= 0;
	pfd.cAccumGreenBits	= 0;
	pfd.cAccumBlueBits	= 0;
	pfd.cAccumAlphaBits	= 0;
	pfd.cDepthBits		= 16;
	pfd.cStencilBits	= 0;
	pfd.cAuxBuffers		= 0;
	pfd.iLayerType		= PFD_MAIN_PLANE;
	pfd.bReserved		= 0;
	pfd.dwLayerMask		= 0;
	pfd.dwVisibleMask	= 0;
	pfd.dwDamageMask	= 0;

	SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);

	_m_hglrcContext = wglCreateContext(hDC);
	wglMakeCurrent(hDC, _m_hglrcContext);

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearColor(0, 0, 0, 0);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);

	/*glViewport(0, 0, _m_lScreenWidth, _m_lScreenHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, _m_lScreenWidth, _m_lScreenHeight, 0, -5000, 5000);
	glMatrixMode(GL_MODELVIEW);*/

	MGIntSize winSize(MGDevice::getScreenWidth(),MGDevice::getScreenHeight());
	// set view port for user FBO, fixed bug #543 #544
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)winSize.width/winSize.height, 0.0001f, 2500.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt( winSize.width/2, winSize.height/2, winSize.height / (2 * tan(M_PI / 6)),
		winSize.width/2, winSize.height/2, 0,
		0.0f, 1.0f, 0.0f
		);
	glViewport(0, 0, (GLsizei)winSize.width, (GLsizei)winSize.height);

	// 开启垂直同步。注意某些显卡有独立的设置，可能会覆盖此处的设置。
	const void *lpfnSwapIntervalEXT = wglGetProcAddress("wglSwapIntervalEXT");
	if(lpfnSwapIntervalEXT != NULL){
		(*(BOOL (WINAPI *)(int))lpfnSwapIntervalEXT)(1);
	}

	ReleaseDC(_m_hOwner, hDC);
}
bool OpenGLThreadClass::_DoOnIdle(){
	z11::GuiContainer::container->light_graphic->threadLoop();
	glFlush();

	return true;
}
void OpenGLThreadClass::_DoOnMessage(MSG *lpMsg){
	UNREF_PARAMS(lpMsg);
}
void OpenGLThreadClass::_PreTerminate(int nExitCode){
	UNREF_PARAMS(nExitCode);

	// OpenGL 的清理操作。
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(_m_hglrcContext);
}
