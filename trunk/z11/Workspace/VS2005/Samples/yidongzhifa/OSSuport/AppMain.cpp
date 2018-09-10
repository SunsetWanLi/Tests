//
//  AppMain.cpp
//  iOSApp
//
//  Created by Wang ZhiPeng on 11-5-25.
//  Copyright 2011骞?Zhejiang University. All rights reserved.
//

#include "MIKit.h"
#include "MGFoundation.h"

#include "AppMain.h"
//#include "GUI/Base/MIView.h"
//#include "System/Shared/MGLog.h"
//#include "System/Device/MGDevice.h"
#include "viewcontrollers/GUIRootViewController.h"

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
	//TODO: add your own initialise code here!
	MGBundle::main_bundle_path = MGPath::getInstance()->getRootDriverPath()+"Mobile.App";
	gui_root = GUIRootViewController::getInstance();
}

extern void Swap();
static int count=0;
void AppMain::renderScreen()
{
	//g_pDrawDevice->Blit(g_pDrawDevice->GetMainDC(),g_pDrawDevice->GetMemDC());

#if defined(_WIN32) || defined(_WIN64)
	Swap();
#endif

}

void AppMain::handleSysCallbackEvent(z11::MIEvent *event)
{

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
	//    GLfloat x[3], y[3], z[3];
	//    GLfloat mag;
	//
	//    /* Make rotation matrix */
	//
	//    /* Z vector */
	//    z[0] = eyex - centerx;
	//    z[1] = eyey - centery;
	//    z[2] = eyez - centerz;
	//    mag = (float)sqrtf(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
	//    if (mag) {
	//        z[0] /= mag;
	//        z[1] /= mag;
	//        z[2] /= mag;
	//    }
	//
	//    /* Y vector */
	//    y[0] = upx;
	//    y[1] = upy;
	//    y[2] = upz;
	//
	//    /* X vector = Y cross Z */
	//    x[0] = y[1] * z[2] - y[2] * z[1];
	//    x[1] = -y[0] * z[2] + y[2] * z[0];
	//    x[2] = y[0] * z[1] - y[1] * z[0];
	//
	//    /* Recompute Y = Z cross X */
	//    y[0] = z[1] * x[2] - z[2] * x[1];
	//    y[1] = -z[0] * x[2] + z[2] * x[0];
	//    y[2] = z[0] * x[1] - z[1] * x[0];
	//
	//    /* cross product gives area of parallelogram, which is < 1.0 for
	//     * non-perpendicular unit-length vectors; so normalize x, y here
	//     */
	//
	//    mag = (float)sqrtf(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
	//    if (mag) {
	//        x[0] /= mag;
	//        x[1] /= mag;
	//        x[2] /= mag;
	//    }
	//
	//    mag = (float)sqrtf(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
	//    if (mag) {
	//        y[0] /= mag;
	//        y[1] /= mag;
	//        y[2] /= mag;
	//    }

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

}

void AppMain::afterRenderScreen()
{

}

AppMain::AppMain()
{

}

AppMain::~AppMain()
{

}


void AppMain::preserveEvent(z11::MIEvent *event)
{
	GuiContainer::preserveEvent(event);
	PostMessage((HWND)wnd_handle,WM_USER+1,0,0);
}