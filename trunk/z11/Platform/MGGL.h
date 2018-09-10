//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (MGGL.h)
/// @brief (本文件实现的功能的简述)
///
///(本文件实现的功能的详述)
///
/// @version 0.1   (版本声明)
/// @author        (Gu RongFang)
/// @date          (11-08-31)
///
///
///    修订说明：最初版本
//////////////////////////////////////////////////////////////////////////
#ifndef _MGGL_H_
#define _MGGL_H_

//
// Common layer for OpenGL stuff
//

#include "MGPlatformConfig.h"

#define MG_GL_FRAMEBUFFER			GL_FRAMEBUFFER_OES
#define MG_GL_FRAMEBUFFER_BINDING	GL_FRAMEBUFFER_BINDING_OES
#define MG_GL_COLOR_ATTACHMENT0		GL_COLOR_ATTACHMENT0_OES
#define MG_GL_FRAMEBUFFER_COMPLETE	GL_FRAMEBUFFER_COMPLETE_OES


#if (MG_TARGET_PLATFORM == MG_PLATFORM_ANDROID)
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#elif (MG_TARGET_PLATFORM == MG_PLATFORM_WIN32  )
#define DIRECT_OPENGL
#ifdef DIRECT_OPENGL
#include <GL/glew.h>
#else
#define glOrtho glOrthof
#include <EGL/egl.h>
#include <GLES/gl.h>
#endif

#elif (MG_TARGET_PLATFORM == MG_PLATFORM_WINCE)
#include <GLES/gl.h>
#include <GLES/egl.h>

#elif (MG_TARGET_PLATFORM == MG_PLATFORM_LINUX)
#include <GLES/gl.h>
#include <EGL/egl.h>
//#if (MG_TARGET_PLATFORM == MG_PLATFORM_IOS)
#else
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#include "../ExternalLib/ftgles/iGLU-1.0.0/include/glu.h"
/*
OpenGL GLU implementation
*/
//#ifdef __cplusplus
//extern "C"
//{
//#endif
///** OpenGL gluLookAt implementation */
//void gluLookAt(GLfloat fEyeX, GLfloat fEyeY, GLfloat fEyeZ,
//    GLfloat fLookAtX, GLfloat fLookAtY, GLfloat fLookAtZ,
//    GLfloat fUpX, GLfloat fUpY, GLfloat fUpZ);
//
///** OpenGL gluPerspective implementation */
//void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);
//
//#ifdef __cplusplus
//}
//#endif





#endif //_MGGL_H_
