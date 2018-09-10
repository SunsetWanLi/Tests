//
//  MGActor.cpp
//  Drama
//
//  Created by King Asdy on 12-4-2.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#include "MGActor.h"
#include "../LightGraphic/LightGraphic.h"
#include "../../GraphicCore/MGImage.h"
#include "../GuiContainer/GuiContainer.h"
#include <assert.h>

using namespace z11;


MGAct::MGAct()
{
    m_image = NULL;
    audio = NULL;
    act_duration = 0.0f;
    act_time = 0.0f;
    act_rect.clear();
    num_of_loops = 1.0f;
    one_image = true;
}

MGAct::~MGAct()
{
    if(m_image != NULL)
    {
        delete m_image;
        m_image = NULL;
    }

    if(audio != NULL)
    {
        audio->uninitPlayer();
        delete audio;
        audio = NULL;
    }
    act_duration = 0.0f;
    act_time = 0.0f;

    v_files.clear();
    act_rect.clear();
}

MGActor::MGActor()
{
    act_manager.clear();
    current_act_name = "";
    delegate = NULL;
    draw_image_mode = ImageCustomFit;
    image_rect = MGRectMake(0, 0, 0, 0);
}

MGActor::~MGActor()
{
    ActIter it;
    for (it = act_manager.begin(); it != act_manager.end(); ++it) {
        if(it->second != NULL)
            delete it->second;
    }
    act_manager.clear();
    current_act_name = "";
    delegate = NULL;
}

void MGActor::initWithFrame(MGRect frame)
{
     MIView::initWithFrame(frame);
}

void MGActor::drawself()
{
    //TODO: redraw here!
    if(m_hidden){
        return;
    }
    MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
    if (graphic_engine == NULL) {
        return;
    }
    RGBAColorRef color_ref;
    if (m_background_color != NULL) {
        color_ref = m_background_color->m_color;
        color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
    }else{
        color_ref = RGBAColorMake(0, 0, 0, (byte_8)(m_alpha * 255));
    }
    //先找到需要绘制的act
    do{
    ActIter it = act_manager.find(current_act_name);
    if(it != act_manager.end())
    {   
        //绘制Act动作
        MGAct* act = it->second;
        if(act->num_of_loops == 0.0f)
        {
            break;
        }
        if((m_background_color == NULL || m_background_color->m_image == NULL) && act->m_image == NULL)
            break;
        
        if(act->act_time < 0 || act->act_duration <= 0)
            break;
        uint_16 frame_num = act->act_rect.size();
        if(!act->one_image){
            frame_num = act->v_files.size();
        }
        uint_16 current_frame = frame_num * (act->act_time / act->act_duration);
        if(current_frame >= frame_num)
            break;
        
        MGImage* image;
        if(act->one_image){
            image = act->m_image == NULL? m_background_color->m_image : act->m_image;
        }
        else{
            image = MGImage::imageWithContentsOfFile(act->v_files[current_frame]);
        }
        
        
        MGRect image_rect;
        if(act->one_image){
            image_rect = act->act_rect[current_frame];
        }
        else{
            image_rect = MGRectMake(0, 0, m_frame.width, m_frame.height);
        }
        image->sendDataToContext();
        graphic_engine->drawImage(image, m_bounds, color_ref, draw_image_mode, image_rect);
        //绘制结束，计算动画执行时间  //
        float interval = LightGraphic::getInterval();
        if(act->act_time < act->act_duration)
        {
            act->act_time += interval;
        }
        if(act->act_time >= act->act_duration)
        {   //已经完成一次动作  //
            if(act->num_of_loops >= 1.0f)
            {
                act->num_of_loops -= 1.0f;
            }
            else if (act->num_of_loops > 0.0f)
            {
                act->num_of_loops = 0.0f;
            }
            act->act_time = 0.0f;
            if(act->num_of_loops == 0.0f)
            {
                if (did_act_handler.sel_func != NULL && did_act_responder != NULL) {
                    MIEvent* event = new MIEvent;
                    event->_responder = did_act_responder;
                    event->_handler = did_act_handler;
                    GuiContainer::container->preserveEvent(event);
                }
            }
        }else {
            //一次动作完成过程中
            float elapse_rate = act->act_time / act->act_duration;
            if(act->num_of_loops > 0 && act->num_of_loops <= elapse_rate)
            {
                act->num_of_loops = 0.0f;
                act->act_time = 0.0f;
                if (did_act_handler.sel_func != NULL && did_act_responder != NULL) {
                    MIEvent* event = new MIEvent;
                    event->_responder = did_act_responder;
                    event->_handler = did_act_handler;
                    GuiContainer::container->preserveEvent(event);
                }
            }
        }
        return;
    }
    }while(0);
    //绘制背景图片
    if (m_background_color != NULL && m_background_color->m_image != NULL) {
        m_background_color->m_image->sendDataToContext();
        graphic_engine->drawImage(m_background_color->m_image, m_bounds, color_ref, draw_image_mode, image_rect);
    }else{
        graphic_engine->drawFilledRect(m_bounds, color_ref);
    }
}

bool MGActor::act(const string &act_name, float times)
{
    AUTO_LOCK_THIS;
    bool ret = false;
    do{
        ActIter it = act_manager.find(act_name);
        if(it == act_manager.end())
            break; //while(0)
        if(it->second == NULL)
        {
            act_manager.erase(it);
            break; //while(0)
        }
        current_act_name = act_name;
        if (will_act_handler.sel_func != NULL && will_act_responder != NULL) {
            MIEvent* event = new MIEvent;
            event->_responder = will_act_responder;
            event->_handler = will_act_handler;
            GuiContainer::container->preserveEvent(event);
        }
        it->second->num_of_loops = times;
        it->second->act_time = 0.0f;
        if(it->second->num_of_loops == 0)
            break;
        if(it->second->audio != NULL)
            it->second->audio->play();
        ret = true;
    }while(0);
    return ret;
}

void MGActor::addActWithName(const string &act_name, z11::MGAct *act)
{
    if(act == NULL)
        return;
    AUTO_LOCK_THIS;
    do{
        ActIter it = act_manager.find(act_name);
        if(it == act_manager.end())
        {
            act_manager[act_name] = act;
            break; //while(0)
        }
        if(it->second != NULL)
        {
            delete it->second;
        }
        it->second = act;
    }while(0);
}
void MGActor::removeActWithName(const string& act_name)
{
    AUTO_LOCK_THIS;
    do{
        ActIter it = act_manager.find(act_name);
        if(it != act_manager.end())
        {
            if(it->second != NULL)
            {
                delete it->second;
            }
            act_manager.erase(it);
            break;
        }
    }while(0);
}

void MGActor::setActorWillActSelector(MIResponder* _responder, SELHandler selector)
{
    will_act_responder = _responder;
    will_act_handler = selector;
}

void MGActor::setActorDidActSelector(MIResponder* _responder, SELHandler selector)
{
    did_act_responder = _responder;
    did_act_handler = selector;
}

//触摸事件的响应 
MIEventHandleState MGActor::touchesBegan(const std::set<MITouch*>& touches,
                                          const MIEvent* const event)
{
    MIEventHandleState ret = MIControl::touchesBegan(touches, event);
    std::set<MITouch*>::const_iterator touch_it;
    for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
        MGPoint loc_in_view = (*touch_it)->locationInView(this);
        if (pointInside(loc_in_view, event)) {
            break;
        }
    }
    EventsMapIterator it = events_dispatch_map.find(MIControlEventTouchDown);
	if (it == events_dispatch_map.end()) {
		ret = MIEventHandleStatePass;
    }else{
        assert(it->second.view_controller != NULL);
        assert(it->second.event_func != NULL);
        EventFunc func = it->second.event_func;
        ((it->second.view_controller)->*func)(this);
        ret = MIEventHandleStateDone;
    }
    return ret;
}
MIEventHandleState MGActor::touchesMoved(const std::set<MITouch*>& touches,
                                          const MIEvent* const event)
{
    return  MIControl::touchesMoved(touches, event);
}
MIEventHandleState MGActor::touchesEnded(const std::set<MITouch*>& touches,
                                          const MIEvent* const event)
{
    MIEventHandleState ret = MIControl::touchesEnded(touches, event);
    std::set<MITouch*>::const_iterator touch_it;
    for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
        MGPoint loc_in_view = (*touch_it)->locationInView(this);
        if (!pointInside(loc_in_view, event)) {
            break;
        }
    }
    if (touch_it == touches.end()) {
        EventsMapIterator it = events_dispatch_map.find(MIControlEventTouchUpInside);
        if (it == events_dispatch_map.end()) {
            ret = MIEventHandleStatePass;
        }else{
            assert(it->second.view_controller != NULL);
            assert(it->second.event_func != NULL);
            EventFunc func = it->second.event_func;
            ((it->second.view_controller)->*func)(this);
            ret = MIEventHandleStateDone;
        }
    }else{
        EventsMapIterator it = events_dispatch_map.find(MIControlEventTouchUpOutside);
        if (it == events_dispatch_map.end()) {
            ret = MIEventHandleStateNormal;
        }else{
            assert(it->second.view_controller != NULL);
            assert(it->second.event_func != NULL);
            EventFunc func = it->second.event_func;
            ((it->second.view_controller)->*func)(this);
            ret = MIEventHandleStateDone;
        }
    }
    return ret;
}
MIEventHandleState MGActor::touchesCancelled(const std::set<MITouch*>& touches,
                                              const MIEvent* const event)
{
    return MIControl::touchesCancelled(touches, event);
}

//事件响应 
MIEventHandleState MGActor::handleEvent(const MIEvent* const event)
{
    return MIEventHandleStateNormal;
}

string MGActor::getCurrentActName()
{
    return current_act_name;
}

