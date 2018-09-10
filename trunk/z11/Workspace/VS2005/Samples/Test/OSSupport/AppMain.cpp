//
//  AppMain.cpp
//  iOSApp
//
//  Created by Wang ZhiPeng on 11-5-25.
//  Copyright 2011年Zhejiang University. All rights reserved.
//

#include "AppMain.h"
#include "windows.h"
#include <sipapi.h>
#include "../../../../../GUI/Base/MIView.h"
#include "../../../../../GUI/Events/MIEvent.h"
#include "../../../../../GUI/GuiContainer/GuiContainer.h"
#include "../../../../../System/Shared/MGLog.h"
#include "../../../../../System/Device/MGDevice.h"
#include "../../../../../System/Shared/MGLog.h"
#include "../../../../../System/Shared/MGTime.h"
#include "../../../../../GUI/Animations/MIAnimationsManager.h"
#include "GUIRootView/GUIRootViewController.h"
#if defined(_WIN32) || defined(_WIN64)	//windows
#include <GLES/gl.h>
#include <GLES/egl.h>
#include <math.h>
#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288   /* pi */
#endif
#elif defined(ANDROID_NDK) //android 
#include <GLES/gl.h>
#else //mac 
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

using namespace z11;
extern bool initEGL(WndHandle wnd);
void AppMain::initialise(WndHandle wnd)
{
    GuiContainer::initialise(wnd);

	gui_root=GUIRootViewController::getInstance();



	//TODO: add your own initialise code here!
}
extern void Swap();
extern void SetDrawableRect(int x,int y,int width,int height);
static int count=0;
void AppMain::renderScreen()
{
	#if defined(_WIN32) || defined(_WIN64)	
			Swap();
	#endif
}
void AppMain::handleSysCallbackEvent(z11::MIEvent *event)
{
// 	MGAssert(event != NULL,"event != NULL");
	switch (event->_event_type) {
		case BECOME_FIRST_RESPONDER_EVENT: //弹出键盘  
			{
				MGLogD("BECOME_FIRST_RESPONDER_EVENT");
				MIView *text_view=static_cast<MIView *>(event->_responder);
				MGRect rect=text_view->getBounds();
				SetDrawableRect(rect.x,rect.y,rect.width,rect.height);
				SipShowIM(SIPF_ON);
				break;
			}
		case RESIGN_FIRST_RESPONDER_EVENT: //收回键盘     
			{
				MGLogD("RESIGN_FIRST_RESPONDER_EVENT");
				SipShowIM(SIPF_OFF);
				SetDrawableRect(0,0,MGDevice::getScreenWidth(),MGDevice::getScreenHeight());
				break;
			}
		default:
			break;
	}

}
void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{	
	GLfloat xmin, xmax, ymin, ymax;

	ymax = zNear * (GLfloat)tan(fovy * M_PI / 360);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;

	glFrustumf(xmin, xmax,
		ymin, ymax,
		zNear, zFar);	
}

void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
			   GLfloat centerx, GLfloat centery, GLfloat centerz,
			   GLfloat upx, GLfloat upy, GLfloat upz)
{
	GLfloat m[16];

#define M(row,col)  m[col*4+row]
	M(0, 0) = 1.0f;//x[0];
	M(0, 1) = 0.0f;//x[1];
	M(0, 2) = 0.0f;//x[2];
	M(0, 3) = 0.0f;
	M(1, 0) = 0.0f;//y[0];
	M(1, 1) = -1.0f;//y[1];
	M(1, 2) = 0.0f;//y[2];
	M(1, 3) = 0.0f;
	M(2, 0) = 0.0f;//z[0];
	M(2, 1) = 0.0f;//z[1];
	M(2, 2) = 1.0f;//z[2];
	M(2, 3) = 0.0f;
	M(3, 0) = 0.0f;
	M(3, 1) = 0.0f;
	M(3, 2) = 0.0f;
	M(3, 3) = 1.0f;
#undef M

	glMultMatrixf(m);


	/* Translate Eye to Origin */
	glTranslatef(-eyex, -eyey, -eyez);
}

void AppMain::setProjectionMode()
{
	//glEnable(GL_DEPTH_TEST);

	// This give us the size of the iPhone display
	//EAGLView * view = ((iOSAppViewController*)wnd_handle).gl_view;
	//CGSize size = view.bounds.size;

	MGSize size(MGDevice::getScreenWidth(),MGDevice::getScreenHeight());
	glViewport(0, 0, size.width, size.height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)size.width/size.height, 0.5f, 1500.0f);
	                             


	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	gluLookAt( size.width/2, size.height/2, size.height / 1.1566f,
		size.width/2, size.height/2, 0,
		0.0f, -1.0f, 0.0f);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}
bool AppMain::initOpenGLES()
{
	OutputDebugMessage("initOpenGLES ");
	initEGL(wnd_handle);
	glEnable(GL_TEXTURE_2D);
	setProjectionMode();
	return true;
}
void AppMain::beforeRenderScreen()
{
	//handleEvent();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void AppMain::afterRenderScreen()
{

}
AppMain::AppMain()
{
	MGTimeTicks tick=MGTimeTicks::Now();
	MGTimeTicks new_tick=MGTimeTicks::Now();
	MGLogD("delta time = %lld ms",(new_tick-tick).InMilliseconds());
	MGLogD("delta time = %lf ms",(new_tick-tick).InMillisecondsF());
	MGLogD("delta time = %lld mms",(new_tick-tick).InMicroseconds());
	disableAnimation();
}

AppMain::~AppMain()
{

}


void AppMain::preserveEvent(z11::MIEvent *event)
{
	GuiContainer::preserveEvent(event);
	PostMessage((HWND)wnd_handle,WM_USER+1,0,0);
}