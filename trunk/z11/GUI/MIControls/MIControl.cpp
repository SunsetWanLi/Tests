//
//  MIControl.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-21.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MIControl.h"
namespace z11
{
    MIControl::MIControl()
    {
        class_name = "MIControl";
        m_is_enabled = true;
        m_is_selected = false;
        m_is_highlighted = false;
        m_action_blend = RGBAColorRef::lightBlue();
        m_control_state = MIControlStateNormal;
        m_action_type = MIActionTypeNormal;
        _view_flags.clip_subviews = 1;
    }
    MIControl::~MIControl()
    {
        
    }
    void MIControl::enabled()
    {
        m_is_enabled = true;
        _view_flags.user_interaction_enabled = 1;
        m_control_state &= ~MIControlStateDisabled;
    }
    void MIControl::disabled()
    {
        m_is_enabled = false;
        _view_flags.user_interaction_enabled = 0;
        m_control_state |= MIControlStateDisabled;
    }
    void MIControl::selected()
    {
        m_is_selected = true;
        m_control_state |= MIControlStateSelected;
    }
    void MIControl::deselected()
    {
        m_is_selected = false;
        m_control_state &= ~MIControlStateSelected;
    }
    void MIControl::highlighted()
    {
        m_is_highlighted = true;
        m_control_state |= MIControlStateHighlighted;
    }
    void MIControl::unhignlighted()
    {
        m_is_highlighted = false;
        m_control_state &= ~MIControlStateHighlighted;
    }
    bool MIControl::registerEventHandler(const MIControlEvents &event, EventHandler handler)
    {
        bool ret = false;
        do {
            EventsMapIterator it = events_dispatch_map.find(event);
			if (it != events_dispatch_map.end()) {
				events_dispatch_map[event] = handler;  // can regist again
                break; //while(0)
            }else{
                events_dispatch_map[event] = handler;
                ret = true;
                break; //while(0)
            }
        } while (0);
        return ret;
    } 
    void MIControl::unregisterEventHandler(const MIControlEvents& event)
	{
		EventsMapIterator it = events_dispatch_map.find(event);
		if(it!=events_dispatch_map.end()){
			events_dispatch_map.erase(it);
		}
	}
    void MIControl::setActionType(uint_32 type)
    {
        m_action_type = type;
    }
    
    void MIControl::addActionType(uint_32 type)
    {
        m_action_type |= type;
    }

    //触摸事件的响应 
    MIEventHandleState MIControl::touchesBegan(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        return MIView::touchesBegan(touches, event);
    }
    MIEventHandleState MIControl::touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        return MIView::touchesMoved(touches, event);
    }
    MIEventHandleState MIControl::touchesEnded(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        return MIView::touchesEnded(touches, event);
    }
    MIEventHandleState MIControl::touchesCancelled(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        return MIView::touchesCancelled(touches, event);
    }
    //事件响应 
    MIEventHandleState MIControl::handleEvent(const MIEvent* const event)
    {
        return MIView::handleEvent(event);
    }
}