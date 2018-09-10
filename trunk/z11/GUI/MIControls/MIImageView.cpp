//
//  MIImageView.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-8-18.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MIImageView.h"
#include "../../GraphicCore/MGImage.h"
#include "../../System/Shared/MGLog.h"

namespace z11 {
    MIImageView::MIImageView()
    {
        class_name = "MIImageView";
    }
    MIImageView::~MIImageView()
    {
        
	}
	void MIImageView::drawself()
    {
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
        if (m_background_color != NULL && m_background_color->m_image != NULL) {
            m_background_color->m_image->sendDataToContext();
            graphic_engine->drawImage(m_background_color->m_image, m_bounds, color_ref, draw_image_mode);
        }else{
            switch (view_round_mode) {
                case RoundRectNone:
				{
                    graphic_engine->drawFilledRect(m_bounds, color_ref);
                    break;
                }
                default:
                {
                    graphic_engine->drawFilledRoundRect(m_bounds, color_ref, view_round_rate, RGBAColorMake(255,255,255,255), view_round_mode);
                }
                    break;
            }
        }
    }
    
    //触摸事件的响应 
    MIEventHandleState MIImageView::touchesBegan(const std::set<MITouch*>& touches,
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
            ret = MIEventHandleStateNormal;
        }else{
            assert(it->second.view_controller != NULL);
            assert(it->second.event_func != NULL);
            EventFunc func = it->second.event_func;
            ((it->second.view_controller)->*func)(this);
            ret = MIEventHandleStateDone;
        }
        return ret;
    }
    MIEventHandleState MIImageView::touchesMoved(const std::set<MITouch*>& touches,
                                             const MIEvent* const event)
    {
        return  MIControl::touchesMoved(touches, event);
    }
    MIEventHandleState MIImageView::touchesEnded(const std::set<MITouch*>& touches,
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
                ret = MIEventHandleStateNormal;
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
    MIEventHandleState MIImageView::touchesCancelled(const std::set<MITouch*>& touches,
                                                 const MIEvent* const event)
    {
        return MIControl::touchesCancelled(touches, event);
    }
    
    //事件响应 
    MIEventHandleState MIImageView::handleEvent(const MIEvent* const event)
    {
        return MIEventHandleStateNormal;
    }
    
} //namespace z11