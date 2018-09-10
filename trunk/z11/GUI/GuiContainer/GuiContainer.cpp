//
//  GuiContainer.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-22.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "../../CommonBase/CommonDef.h"
#include "GuiContainer.h"
#include "../Base/MIView.h"
#include "../Controllers/MIViewController.h"
#include "../Events/MIEventQueue.h"
#include "../LightGraphic/LightGraphic.h"
#include "../Base/MITouchTracer.h"
#include "../Animations/MIAnimationsManager.h"
#include "../../System/Timer/MGTimer.h"
#include "../../System/Shared/MGLog.h"
#include "../../GraphicCore/MGFTGLESFont.h"
#include "../../Platform/MGPlatformConfig.h"
#include "../../System/IO/MGFileManager.h"
#include <assert.h>

namespace z11
{
    GuiContainer* GuiContainer::container = NULL;
    uint_32 GuiContainer::enabled_orientation = 0;
    bool GuiContainer::keyboard_shown = false;
    GuiContainer::GuiContainer()
    {
        m_event_queue = new MIEventQueue();
        gui_root = NULL;
        wnd_handle = NULL;
        MIView::touch_tracer = new MITouchTracer();
        MIView::animations_manager = new MIAnimationsManager();
        light_graphic = NULL;
    }
    GuiContainer::~GuiContainer()
    {
        if (gui_root != NULL) {
            delete gui_root;
        }
        if(light_graphic != NULL)
        {
			light_graphic->requestExitAndWait();
            delete light_graphic;
            light_graphic = NULL;
			LightGraphic::setGuiContainer(NULL);
        }
        
        if (MIView::touch_tracer != NULL) {
            delete MIView::touch_tracer;
            MIView::touch_tracer = NULL;
        }
        if (MIView::animations_manager != NULL) {
            delete MIView::animations_manager;
            MIView::animations_manager = NULL;
        }
        if (MIResponder::_first_responder_locker != NULL) {
            delete MIResponder::_first_responder_locker;
            MIResponder::_first_responder_locker = NULL;
        }
        MGTimer::clearAllTimers();
        if (MGTimer::timer_locker != NULL) {
            delete MGTimer::timer_locker;
            MGTimer::timer_locker = NULL;
        }
        if (m_event_queue != NULL) {
            delete m_event_queue;
            m_event_queue = NULL;
        }
        container = NULL;
    }
    void GuiContainer::initialise(WndHandle wnd)
    {
        wnd_handle = wnd;
        if (MIResponder::_first_responder_locker == NULL) {
            MIResponder::_first_responder_locker = new MIResponder::FirstResponderLock();
        }
        if (MGTimer::timer_locker == NULL) {
            MGTimer::timer_locker = new MGTimer::MGTimerLocker();
        }
        light_graphic = LightGraphic::sharedLightGraphic(this);
        container = this;
#if defined(__IPHONE_2_0) //目前只有iOS需要在主线程中初始化资源   
		initOpenGLES();
#endif
		z11::MGFileManager::defaultManager()->createDirectoryAtPath(z11::MGBundle::storage_card_bundle_path+"controls/");
    }
    void GuiContainer::launchAppWithURL(const string& identity, const string &url)
    {
        if(container != NULL)
        {
            MIEvent* event = new MIEvent();
            event->_event_type = LAUNCH_APP_WITH_URL_EVENT;
            event->_responder = NULL;
            MGRemoteAppContent* content = new MGRemoteAppContent();
            content->url_content = url;
            content->id_content = identity;
            event->_content = content;
            container->preserveEvent(event);
        }
    }
    void GuiContainer::enableAnimation()
    {
        if (MIView::animations_manager == NULL) {
            return;
        } else {
            MIView::animations_manager->permit_animation = true;
        }
    }
    void GuiContainer::disableAnimation()
    {
        if (MIView::animations_manager == NULL) {
            return;
        } else {
            MIView::animations_manager->permit_animation = false;
        }
    }
    
    void GuiContainer::lockScreenWhenIdle()
    {
        if(container != NULL)
        {
            MIEvent* event = new MIEvent();
            event->_event_type = APP_LOCK_SCREEN_WHEN_IDLE_EVENT;
            event->_responder = NULL;
            event->_content = NULL;
            container->preserveEvent(event);
        }
    }
    
    void GuiContainer::unlockScreenWhenIdle()
    {
        if(container != NULL)
        {
            MIEvent* event = new MIEvent();
            event->_event_type = APP_UNLOCK_SCREEN_WHEN_IDLE_EVENT;
            event->_responder = NULL;
            event->_content = NULL;
            container->preserveEvent(event);
        }
    }
    
    void GuiContainer::showSystemStatusBar()
    {
        if(container != NULL)
        {
            MIEvent* event = new MIEvent();
            event->_event_type = APP_SHOW_SYSTEM_STATUS_BAR_EVENT;
            event->_responder = NULL;
            event->_content = NULL;
            container->preserveEvent(event);
        }
    }
    
    void GuiContainer::hideSystemStatusBar()
    {
        if(container != NULL)
        {
            MIEvent* event = new MIEvent();
            event->_event_type = APP_HIDE_SYSTEM_STATUS_BAR_EVENT;
            event->_responder = NULL;
            event->_content = NULL;
            container->preserveEvent(event);
        }
    }
    
    void GuiContainer::foreignerSDKApiCall(const string &api, bool immediately)
    {
        if(container != NULL)
        {
            MIEvent* event = new MIEvent();
            MGTextContent * content = new MGTextContent();
            content->text_content = api;
            if (immediately)
                event->_event_type = FOREIGNER_SDK_API_IMMEDIATELY_CALL_EVENT;
            else
                event->_event_type = FOREIGNER_SDK_API_CALL_EVENT;
            event->_responder = NULL;
            event->_content = content;
            container->preserveEvent(event);
        }
    }
    
    void GuiContainer::foreignerSDKApiBack(const string& response)
    {
        if (container != NULL && container->gui_root != NULL) {
            container->gui_root->handleForeignerSDKApiBack(response);
        }
    }
    void GuiContainer::applicationWillExit()
	{
		if (container != NULL && container->gui_root != NULL) {
			container->gui_root->applicationWillExit();
        }
	}
    void GuiContainer::enableOrientation(uint_32 orientation)
    {
        enabled_orientation = orientation;
    }
    
    uint_32 GuiContainer::getEnabledOrientation()
    {
        return enabled_orientation;
    }
    
    void GuiContainer::setOrientation(MIInterfaceOrientation orientation)
    {
        enabled_orientation = orientation;
    }
    
    void GuiContainer::applicationDidReceiveMemoryWarning()
    {
        MGFTGLESFont::cleanCache();
    }
    
    void GuiContainer::moveEntirViewForKeyboardWithHeight(int height)
    {
        if (gui_root == NULL || gui_root->view == NULL) {
            return;
        }
        
        if (MG_TARGET_PLATFORM == MG_PLATFORM_WIN32) {
            return;
        }
        
        if (height > 0) {
            keyboard_shown = true;
        } else {
            keyboard_shown = false;
        }
        
        MGRectTransition transition = MGRectTransitionMake(0, 0, 0);
        gui_root->view->setSubRectTransition(transition);
        
        MIResponder * res = MIResponder::getFirstResponder();
        float view_height = 0;
        if (res != NULL) {
            view_height = res->adjustWithKeyboardHeight(height);
        }
        MIView* view = dynamic_cast<MIView*>(res);
        MGPoint screen_position;
        if (view != NULL) {
            screen_position = MGPointMake(view->getBounds().x, view->getBounds().y);
            view->locationInScreen(screen_position, view);
        }
        MIResponder::setFirstResponder(res);
        
        if (view == NULL) {
            return;
        }
        
        MGPoint screen = MGPointMake(MGDevice::getScreenWidth(), MGDevice::getScreenHeight());
        
        view_height += screen_position.y;
        // make sure view will not move out of screen. //
        view_height = view_height < screen.y ? view_height : screen.y;
		
		if (view_height < (screen.y - height)) {
			return;
		}
        if (height == 0) {
            transition.axis_y = 0;
        } else {
            transition.axis_y = screen.y - height - view_height;
        }
        gui_root->view->setSubRectTransition(transition);
    }
    
    void GuiContainer::resetEntirViewForKeyboardWithView()
    {
        if (gui_root == NULL) {
            return;
        }
        keyboard_shown = false;
        MGRectTransition transition = MGRectTransitionMake(0, 0, 0);
        gui_root->view->setSubRectTransition(transition);
    }
    
    bool GuiContainer::needResetEntirViewForKeyboardWithView()
    {
        if (gui_root == NULL) {
            return false;
        }
        MGRectTransition transition = gui_root->view->getSubRectTransition();
        return transition != MGRectTransitionMake(0, 0, 0);
    }
    
    void GuiContainer::preserveEvent(MIEvent* event)
    {
        if (event->_event_type == RESIGN_FIRST_RESPONDER_EVENT) {
            // reset move view for editing //
            GuiContainer::container->resetEntirViewForKeyboardWithView();
        }
        m_event_queue->eventEnque(event);
    }
    void GuiContainer::clearEventWithObject(z11::MGObject *object)
    {
        m_event_queue->clearEventWithObject(object);
    }
    void GuiContainer::handleTouchesEvent(const std::set<MITouch*>&touches, MITouchEventType type)
    {
        MIEvent *event = NULL;
        std::set<MITouch*>::const_iterator it;
        
        // ======================== Touch Tracer Test ========================== //
//        static int touch_num = 0;
//        it = touches.begin();
//        if ((*it)->tag == INVALID_TOUCH_TAG) {
//            switch (type) {
//                case MITouchEventBegan:
//                    MGLog(MGLog::MGINFO, "touch began %d.", touches.size());
//                    touch_num += touches.size();
//                    MGLog(MGLog::MGINFO, "current touch_num is %d.", touch_num);
//                    break;
//                case MITouchEventMoved:
//                    MGLog(MGLog::MGVERBOSE, "touch moved %d.", touches.size());
//                    break;
//                case MITouchEventEnded:
//                    MGLog(MGLog::MGINFO, "touch ended %d.", touches.size());
//                    touch_num -= touches.size();
//                    MGLog(MGLog::MGINFO, "current touch_num is %d.", touch_num);
//                    break;
//                case MITouchEventCancelled:
//                    MGLog(MGLog::MGINFO, "touch cancelled %d.", touches.size());
//                    touch_num -= touches.size();
//                    MGLog(MGLog::MGINFO, "current touch_num is %d.", touch_num);
//                    break;
//                default:
//                    MGLog(MGLog::MGINFO, "touch default %d.", touches.size());
//                    break;
//            }
//        }
        // ==================================================================== //
        
//        MGTimeTicks tick = MGTimeTicks::Now();
        previous_view_touches_map.clear();
        ViewTouchesMapIt map_it;
        MIView::touch_tracer->lock();
       // MIView::touch_tracer->print();
        for (it = touches.begin(); it != touches.end(); ++it) {
            assert(MIView::touch_tracer != NULL);
            MIView* view = NULL;
            //首先进行触摸跟踪的状态检测 //
            if (MIView::touch_tracer->traceTouchWithTag((*it), type) == false) {
                if (MIView::touch_tracer->traceTouchWithPhase((*it), type) == false)
                {
                    MITouch *touch = *it;
                    delete_and_set_null(touch);
                    continue;
                }
            }
            //进行触摸监测，找到触摸的View//
            assert(gui_root->view != NULL);
            if ((*it)->maintained_view == NULL) {
                gui_root->view->checkTouchesInSubview();
            }
            gui_root->view->can_handle_touch = (gui_root->view->touch_manager.size() > 0 || gui_root->view->touch_hitted || !gui_root->view->m_have_touches_through_subview);
            view = gui_root->view->hitDetect((*it), event, type);
            MIResponder* first_responder = MIResponder::getFirstResponder();
            if (first_responder != NULL && first_responder != view) {
                first_responder->clearContext(view);
            }
            MIResponder::setFirstResponder(first_responder);
            MIView::touch_tracer->registerTouchWithView((*it), type, view);
            
//            string c_name = "NULL";
//            if (view != NULL) {
//                c_name = view->class_name.className();
//            }
//            
//            MGLogD("%d touch on view %s: %p.", (*it)->phase, c_name.c_str(), view);
            
            (*it)->touched_view = view;
            
            if (view != (*it)->maintained_view && view != NULL)
            {
                view->touch_hitted = false;
            }

            if((*it)->maintained_view == NULL)
            {
                MITouch *touch = *it;
                delete_and_set_null(touch);
                continue;
            }
            //对多点触摸进行按起始触发View进行归类//
            map_it = previous_view_touches_map.find((*it)->maintained_view);
            if (map_it == previous_view_touches_map.end()) {
                std::set<MITouch*> touch_set;
                touch_set.insert(*it);
                previous_view_touches_map[(*it)->maintained_view] = touch_set;
            }else {
                map_it->second.insert(*it);
            }
        }

        //在起始View上进行触摸事件处理，比如DragOut//
        map_it = previous_view_touches_map.begin();
        while (map_it != previous_view_touches_map.end())
        {
            // 处理这样的事件时，因为前面已经在hitDetect里面进行了坐标变换，因此这里的坐标不能直接用Point，用Screen_Point更准确，根据事件类型进行区分 //
			MIView* view = map_it->first;
			set<MITouch*> touch_set = map_it->second;
			previous_view_touches_map.erase(map_it);
            view->touch_hitted = false;
			switch (type) {
				case MITouchEventBegan:
					view->view_touchesBegan(touch_set, event);
					break;
				case MITouchEventMoved:
					view->view_touchesMoved(touch_set, event);
					break;
				case MITouchEventEnded:
					view->view_touchesEnded(touch_set, event);
					break;
				case MITouchEventCancelled:
					view->view_touchesCancelled(touch_set, event);
					break;
				default:
					break;
			}
            for (set<MITouch*>::iterator it = touch_set.begin(); it != touch_set.end(); ++it) {
                MITouch* touch = *it;
                delete_and_set_null(touch);
            }
            map_it = previous_view_touches_map.begin();
        }
        previous_view_touches_map.clear();
        MIView::touch_tracer->unlock();
//        MGTimeDelta delta = MGTimeTicks::Now() - tick;
//        MGLogD("Delta is %d.", delta.InMilliseconds());
    }
    void GuiContainer::handleEventImmediatelly(MIEvent* event)
    {
        if (event == NULL) {
            return;
		}
        if(!event->isControlRetransTouchesEvent())
            handleSystemEvent(event);
        if (event->touches_in_event.size() > 0) {
            set<MITouch*> begin_set;
            set<MITouch*> moved_set;
            set<MITouch*> ended_set;
            set<MITouch*> cancelled_set;
            for(TouchesInEventIt it = event->touches_in_event.begin(); it != event->touches_in_event.end(); ++it)
            {
                switch ((*it)->phase) {
                    case MITouchPhaseBegan:
                    {
                        begin_set.insert(*it);
                        break;
                    }
                    case MITouchPhaseStationary:
                    {
                        begin_set.insert(*it);
                        break;
                    }
                    case MITouchPhaseMoved:
                    {
                        moved_set.insert(*it);
                        break;
                    }
                    case MITouchPhaseEnded:
                    {
                        ended_set.insert(*it);
                        break;
                    }
                    case MITouchPhaseCancelled:
                    {
                        cancelled_set.insert(*it);
                        break;
                    }
                    default:
                        break;
                }
            }
            event->touches_in_event.clear();
            if (!begin_set.empty()) {
                handleTouchesEvent(begin_set, MITouchEventBegan);
            }
            if (!moved_set.empty()) {
                handleTouchesEvent(moved_set, MITouchEventMoved);
            }
            if (!ended_set.empty()) {
                handleTouchesEvent(ended_set, MITouchEventEnded);
            }
            if (!cancelled_set.empty()) {
                handleTouchesEvent(cancelled_set, MITouchEventCancelled);
            }
        }
        if(event->_event_type == CONTROL_PURE_TOUCHES_DELAY_EVENT)
            MGSleep(200);
        delete event;
    }
    void GuiContainer::handleEvent()
	{
		if (gui_root == NULL) {
			MGLogD("===============Alert!!!!! gui_root==NULL while GuiContainer::handleEvent==========");
			assert(gui_root != NULL);
            return;
		}
        MIEvent* event = m_event_queue->eventDeque();
        if (event == NULL) {
            return;
		}
        handleEventImmediatelly(event);
    }
    void GuiContainer::handleSystemEvent(MIEvent* event)
    {
        if (gui_root == NULL) {
            return;
        }
        assert(event != NULL);
		
        if (event->isSysCallbackEvent()) {
            return handleSysCallbackEvent(event);
        }
        MIResponder *current_responder = MIResponder::getFirstResponder();
        MIResponder *responder = current_responder;
        MIResponder::setFirstResponder(current_responder);
        assert(MIResponder::_first_responder_locker != NULL);
        MIResponder::_first_responder_locker->lock();
        MIEventHandleState state = MIEventHandleStateNormal;
        do{
            if (responder != NULL && event->_responder == NULL) {
                state = responder->_handleEvent(event);
                while (state != MIEventHandleStateDone && responder->nextResponder() != NULL) {
                    responder = responder->nextResponder();
                    state = responder->_handleEvent(event);
                }
                break; //while(0)
            }else {
                if (event->_responder == NULL) {
                    break; //while(0)
                }
                responder = dynamic_cast<MIResponder*>(event->_responder);
                if (responder != NULL) {
                    state = responder->_handleEvent(event);
                    while (state != MIEventHandleStateDone && responder->nextResponder() != NULL) {
                        responder = responder->nextResponder();
                        state = responder->_handleEvent(event);
                    }
                    break; //while(0)
                }else{
                    if (event->_handler.sel_func != NULL) {
                        SELFunc sel_func = event->_handler.sel_func;
                        (event->_responder->*sel_func)(event->_handler.sender);
                        break; // while(0);
                    }
                }
            }
        }while(0);
        MIResponder::_first_responder_locker->unlock();
        return;
    }
    void GuiContainer::handleSysCallbackEvent(z11::MIEvent *event)
    {

    }
    void GuiContainer::clearTouchesEventInView(z11::MIView *view)
    {
        MIView::touch_tracer->lock();
        ViewTouchesMapIt it = previous_view_touches_map.find(view);
        if (it != previous_view_touches_map.end()) {
            previous_view_touches_map.erase(it);
        }
        MIView::touch_tracer->unlock();
    }
    void GuiContainer::clearTouches(std::set<MITouch*> touches)
    {
        std::set<MITouch*>::iterator it;
        for (it = touches.begin(); it != touches.end(); ++it) {
            delete (*it);
        }
    }
    bool GuiContainer::initOpenGLES()
    {
        return false;
    }
    void GuiContainer::beforeRenderScreen()
    {

    }
    void GuiContainer::renderScreen()
    {

    }
    bool GuiContainer::afterRenderScreen()
    {
        return false;
    }
    void GuiContainer::runLoop()
    {
        //light_graphic->firstFrameLoop();
        light_graphic->run("GuiContainer::runLoop");
    }
    void GuiContainer::pauseLoop()
    {
        light_graphic->pauseAndWait();
    }
    void GuiContainer::resumeLoop()
    {
        light_graphic->resume();
    }
    
    bool GuiContainer::applicationNeedHandleBackKeyDown()
    {
        if (gui_root != NULL) {
            return gui_root->applicationNeedHandleBackKeyDown();
        }
        return false;
    }
    
    bool GuiContainer::applicationNeedHandleMenuKeyDown()
    {
        if (gui_root != NULL) {
            return gui_root->applicationNeedHandleMenuKeyDown();
        }
        return false;
    }
} //namespace z11
