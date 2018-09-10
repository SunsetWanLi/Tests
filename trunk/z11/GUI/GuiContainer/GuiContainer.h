//
//  GuiContainer.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-22.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "../Base/MIDataTypeDef.h"
#include "../Events/MITouch.h"
#include "../../System/Device/MGDevice.h"
#include <set>
#include <map>

namespace z11
{
class MIView;
class MIViewController;
class MIResponder;
class MIEventQueue;
class MIEvent;
class LightGraphic;
class RealTimeLoop;
class MITouchTracer;
    
    typedef enum {
        MIInterfaceOrientationPortrait           = MIDeviceOrientationPortrait,
        MIInterfaceOrientationPortraitUpsideDown = MIDeviceOrientationPortraitUpsideDown,
        MIInterfaceOrientationLandscapeLeft      = MIDeviceOrientationLandscapeRight,
        MIInterfaceOrientationLandscapeRight     = MIDeviceOrientationLandscapeLeft
    } MIInterfaceOrientation;
    
    class GuiContainer : public MIObject
    {
    public:
        GuiContainer();
        virtual ~GuiContainer();
        virtual void initialise(WndHandle);
    public:
        static GuiContainer* container;
    public:
        static void launchAppWithURL(const string& identity, const string& url);
        static void enableAnimation();
        static void disableAnimation();
        static void lockScreenWhenIdle();
        static void unlockScreenWhenIdle();
        static void showSystemStatusBar();
        static void hideSystemStatusBar();
        static void foreignerSDKApiCall(const string& api, bool immediately = false);
        static void foreignerSDKApiBack(const string& response);
		static void applicationWillExit();
        static void enableOrientation(uint_32 orientation);
        static uint_32 getEnabledOrientation();
        static void setOrientation(MIInterfaceOrientation orientation);
        virtual void setProjectionMode(){}
        virtual void resizeScreen(uint_32 w, uint_32 h){}
        bool applicationNeedHandleBackKeyDown();
        bool applicationNeedHandleMenuKeyDown();
    protected:
        static uint_32 enabled_orientation;
    //容器根//
    public:
        //需要清理//
        MIViewController* gui_root;
        // 存储键盘状态 //
        static bool keyboard_shown;
    //消息相关//
    protected:
        MIEventQueue *m_event_queue;
    public:
        void applicationDidReceiveMemoryWarning();
        void moveEntirViewForKeyboardWithHeight(int height);
        void resetEntirViewForKeyboardWithView();
        bool needResetEntirViewForKeyboardWithView();
        virtual void preserveEvent(MIEvent* event);
        void clearEventWithObject(MGObject* object);
        // 触摸消息由系统传来，不进入事件队列，由框架直接处理，由应用程序调用 //
        void handleTouchesEvent(const std::set<MITouch*>& touches, MITouchEventType type);
        // 控件触发的其他事件进入队列，周期处理，由Container自己调度调用 //
        void handleEvent();
        // z11框架形成的需要同步处理的事件 //
        void handleEventImmediatelly(MIEvent* event);
        // 系统传来的消息形成的事件，直接处理 //
        void handleSystemEvent(MIEvent* event);
    protected:
        virtual void handleSysCallbackEvent(MIEvent* event);
    protected:
        std::map<MIView*, std::set<MITouch*> > previous_view_touches_map;
    public:
        void clearTouchesEventInView(MIView* view);
        void clearTouches(std::set<MITouch*> touches);
    public:
        //轻量级绘制线程//
        LightGraphic* light_graphic;
    //OpenGL/ES 初始化//
    public:
        WndHandle wnd_handle;
    public:
        virtual bool initOpenGLES() = 0;
        virtual void beforeRenderScreen() = 0;
        virtual void renderScreen() = 0;
        virtual bool afterRenderScreen() = 0;
    //进入绘制循环//
        virtual void runLoop();
        virtual void pauseLoop();
        virtual void resumeLoop();
    };
    typedef std::map<MIView*, std::set<MITouch*> >::iterator ViewTouchesMapIt;
#define GUICONTAINER_DECLARE \
    public: \
        virtual bool initOpenGLES();\
        virtual void beforeRenderScreen(); \
        virtual void renderScreen(); \
        virtual bool afterRenderScreen(); \
    protected: \
        virtual void handleSysCallbackEvent(MIEvent* event);\
	public:
} //namespace z11
