//////////////////////////////////////////////////////////////////////////
//GLDrawDevice :使用opengl es作为绘制引擎
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include "../../../../../System/Shared/MGLog.h"
#include "appMain.h"
#include "../../../../../Gui/Events/MITouch.h"
#include "set"
#include "ViewManager.h"
#include "../../../../../GUI/Events/MIEvent.h"
#include "../../../../../System/Shared/Utility.h"
using namespace z11;

#if defined(_WIN32) || defined(_WIN64)	
#include <GLES/gl.h>
#include <GLES/egl.h>
#elif defined(ANDROID_NDK)
#include <GLES/gl.h>
#else
#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

//#define TestContainer 
//AppMain	* g_pContainer=NULL;


//////////////////////////////////////////////////////////////////////////
//定义
//////////////////////////////////////////////////////////////////////////


using namespace z11;

// Called from the app framework.
void appDeinit()
{

	//delete g_pContainer;
	//g_pContainer = NULL;
}


// Called from the app framework.
void appInit()
{
}


extern void TestMem(wchar_t *str);
#ifdef __cplusplus
extern "C" {
#endif

struct JNIEnv;
extern void JavaRequestBlt( JNIEnv*  env=NULL);
void OnMouseDown(unsigned long wParam, long lParam)
{
	std::set<MITouch*> touches;
	MITouch *mi_touch=new MITouch();
	mi_touch->point.x=(float)(lParam&0xffff);
	mi_touch->point.y=(float)(lParam>>16);
	touches.insert(mi_touch);
	ViewManager::getInstance()->m_app_main->handleTouchesEvent(touches,MITouchEventBegan);
	//TestMem(L"OnMouseDown ");

}
void OnMouseMove(unsigned long wParam, long lParam)
{
	std::set<MITouch*> touches;
	MITouch *mi_touch=new MITouch();
	mi_touch->point.x=(float)(lParam&0xffff);
	mi_touch->point.y=(float)(lParam>>16);
	touches.insert(mi_touch);
	ViewManager::getInstance()->m_app_main->handleTouchesEvent(touches,MITouchEventMoved);
}
void OnMouseUp(unsigned long wParam, long lParam)
{
	std::set<MITouch*> touches;
	MITouch *mi_touch=new MITouch();
	mi_touch->point.x=(float)(lParam&0xffff);
	mi_touch->point.y=(float)(lParam>>16);
	touches.insert(mi_touch);
	ViewManager::getInstance()->m_app_main->handleTouchesEvent(touches,MITouchEventEnded);
}


void OnKeyDown(unsigned long code)
{
	if(code==8) // 删除键  //
	{
		//MIEvent* event = new MIEvent();
		//event->_event_type = KEYBOARD_DELETE_BACKWARD;
		//event->_responder = NULL;
		//event->_content = NULL;
		//ViewManager::getInstance()->m_app_main->handleSystemEvent(event);
	}
}

void OnChar(unsigned long wParam, long lParam)
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
		ViewManager::getInstance()->m_app_main->handleSystemEvent(event);
	}
	else
	{
		MIEvent* event = new MIEvent();
		event->_event_type = KEYBOARD_DELETE_BACKWARD;
		event->_responder = NULL;
		event->_content = NULL;
		ViewManager::getInstance()->m_app_main->handleSystemEvent(event);
	}
}


#ifdef __cplusplus
}
#endif



#ifndef ANDROID_NDK
extern void Swap();
#endif

void OnBlit(unsigned long wParam, long lParam)
{
#ifndef ANDROID_NDK
		Swap();
#endif
}

void appRender(long tick, int width, int height)
{	

}
