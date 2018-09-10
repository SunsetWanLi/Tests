
//////////////////////////////////////////////////////////////////////////
//GLDrawDevice :浣跨敤opengl es浣滀负缁樺埗寮曟搸
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>

#include "MIKit.h"
#include "MGFoundation.h"

//#include "System/Shared/MGLog.h"
#include "appMain.h"
//#include "GUI/Events/MITouch.h"
#include "set"
#include "ViewManager.h"
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
//瀹氫箟
//////////////////////////////////////////////////////////////////////////
//int iScreenWidth=480;
//int iScreenHeight=640;

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

extern void MouseDown();

#ifdef __cplusplus
extern "C" {
#endif

	struct JNIEnv;
	extern void JavaRequestBlt( JNIEnv*  env=NULL);

	float down_x;
	float down_y;

	void OnMouseDown(unsigned long wParam, long lParam)
	{
		std::set<MITouch*> touches;
		MITouch *mi_touch=new MITouch();
		mi_touch->point.x=(float)(lParam&0xffff);
		mi_touch->point.y=(float)(lParam>>16);
		touches.insert(mi_touch);
		down_x = mi_touch->point.x;
		down_y = mi_touch->point.y;
		ViewManager::getInstance()->m_app_main->handleTouchesEvent(touches,MITouchEventBegan);
		//TestMem(L"OnMouseDown ");
		//MGLogD("down x %f, y %f", mi_touch->point.x, mi_touch->point.y);
	}
	void OnMouseMove(unsigned long wParam, long lParam)
	{
		static int first_move = 1;
		float move_x = (float)(lParam&0xffff);
		float move_y = (float)(lParam>>16);
		if (move_x == down_x && move_y == down_y && first_move == 1)
		{
			return;
		}
		first_move = 0;
		std::set<MITouch*> touches;
		MITouch *mi_touch=new MITouch();
		mi_touch->point.x=move_x;
		mi_touch->point.y=move_y;
		touches.insert(mi_touch);
		ViewManager::getInstance()->m_app_main->handleTouchesEvent(touches,MITouchEventMoved);
		//MGLogD("move x %f, y %f", mi_touch->point.x, mi_touch->point.y);
	}
	void OnMouseUp(unsigned long wParam, long lParam)
	{
		std::set<MITouch*> touches;
		MITouch *mi_touch=new MITouch();
		mi_touch->point.x=(float)(lParam&0xffff);
		mi_touch->point.y=(float)(lParam>>16);
		touches.insert(mi_touch);
		ViewManager::getInstance()->m_app_main->handleTouchesEvent(touches,MITouchEventEnded);
		//MGLogD("up x %f, y %f", mi_touch->point.x, mi_touch->point.y);
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

