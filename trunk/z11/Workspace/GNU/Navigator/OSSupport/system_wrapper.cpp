//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (system_wrapper.cpp)  
/// @brief (Android 的system wrapper)  
///  
///(Android 的system wrapper ，这层基本上可以各平台直接复用)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-07-26)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _SYSTEM_WRAPPER_CPP_
#define _SYSTEM_WRAPPER_CPP_

#include <stdio.h>
#include <stdlib.h>
#include "appMain.h"
#include <set>
#include "ViewManager.h"
#include "../../../../../CommonBase/MGStructure.h"
#include "../../../../../System/Device/MGDevice.h"
#include "../../../../../System/Shared/MGLog.h"
#include "../../../../../GUI/Base/MIView.h"
#include "../../../../../GUI/Events/MITouch.h"
#include "../../../../../GUI/LightGraphic/LightGraphic.h"
#include "../../../../../GraphicCore/MGImage.h"
using namespace z11;

//////////////////////////////////////////////////////////////////////////
//定义
//////////////////////////////////////////////////////////////////////////

using namespace z11;

#ifdef __cplusplus
extern "C" {
#endif

// Called from the app framework.
void appDeinit()
{

}

// Called from the app framework.
void appInit()
{
}
void OnKeyDown(int code)
{
    if(code==67) // 删除键  //
    {
        MIEvent* event = new MIEvent();
        event->_event_type = KEYBOARD_DELETE_BACKWARD;
        event->_responder = NULL;
        event->_content = NULL;
        ViewManager::getInstance()->m_app_main->handleSystemEvent(event);
    }
}

void OnInsertText(const char *text)
{
    MIEvent* event = new MIEvent();
    MGTextContent* content = new MGTextContent();
    content->text_content = string(text);
    event->_event_type = KEYBOARD_INPUT;
    event->_content = content;
    event->_responder = NULL;
    ViewManager::getInstance()->m_app_main->handleSystemEvent(event);
}

void OnMouseDown(unsigned long wParam, long lParam)
{
	std::set<MITouch*> touches;
	MITouch *mi_touch=new MITouch();
	mi_touch->point.x=lParam&0xffff;
	mi_touch->point.y=lParam>>16;
	touches.insert(mi_touch);
	ViewManager::getInstance()->m_app_main->handleTouchesEvent(touches,MITouchEventBegan);

}
void OnMouseMove(unsigned long wParam, long lParam)
{
	std::set<MITouch*> touches;
	MITouch *mi_touch=new MITouch();
	mi_touch->point.x=lParam&0xffff;
	mi_touch->point.y=lParam>>16;
	touches.insert(mi_touch);
	ViewManager::getInstance()->m_app_main->handleTouchesEvent(touches,MITouchEventMoved);
}
void OnMouseUp(unsigned long wParam, long lParam)
{
	std::set<MITouch*> touches;
	MITouch *mi_touch=new MITouch();
	mi_touch->point.x=lParam&0xffff;
	mi_touch->point.y=lParam>>16;
	touches.insert(mi_touch);
	ViewManager::getInstance()->m_app_main->handleTouchesEvent(touches,MITouchEventEnded);
	MGLogD("UP");
}

void OnChar(unsigned long wParam, long lParam)
{
}

void OnHandleEvent()
{
		ViewManager::getInstance()->m_app_main->handleEvent();
}


void appRender( int width, int height)
{

	//glViewport(0, 0, z11::MGDevice::getScreenWidth(),z11::MGDevice::getScreenHeight());//设置视口的大小以及位置,视口：也就是图形最终显示到屏幕的区域，前两个参数是视口的位置，后两个参数是视口的宽和长。
	MGRect rect=MGRectMake(0,0,z11::MGDevice::getScreenWidth(),z11::MGDevice::getScreenHeight());
	if(ViewManager::getInstance()->m_app_main->light_graphic!=NULL)
	{
		ViewManager::getInstance()->m_app_main->light_graphic->threadLoop();
	}

}
#ifdef __cplusplus
}
#endif


#endif //_SYSTEM_WRAPPER_CPP_
