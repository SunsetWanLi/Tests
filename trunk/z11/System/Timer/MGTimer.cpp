//
//  MGTimer.cpp
//  System
//
//  Created by Wang ZhiPeng on 11-8-2.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "../Shared/MGLog.h"
#include "MGTimer.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
#include <assert.h>

namespace z11 {
    MGTimer::MGTimerLocker* MGTimer::timer_locker = NULL;
    std::map<MGTimerTag, MGTimer*> MGTimer::timer_map;
    MGTimerTag MGTimer::assign_tag = TIMER_INVALID_TAG;
    
    MGTimer::MGTimer()
    {
        timer_interval = MGTimeDelta::FromSeconds(0);
        is_repeat = false;
        is_valid = false;
        tag = TIMER_INVALID_TAG;
    }
    
    MGTimer::~MGTimer()
    {
        assert(timer_locker != NULL);
        timer_locker->lock();
        if(is_valid)
            invalidate();
        MGTimerMapIt it = timer_map.find(tag);
        if(it != timer_map.end())
            timer_map.erase(it);
        timer_locker->unlock();
    }
    
    void MGTimer::setTimerInterval(const MGTimeDelta& delta)
    {
        assert(timer_locker != NULL);
        timer_locker->lock();
        MGTimeDelta interval = MGTimeDelta::FromSeconds(0);
        if (delta < interval) {
            timer_interval = interval;
        } else {
            timer_interval = delta;
        }
        timer_locker->unlock();
    }
    
    void MGTimer::detachObjectFromTimer(MGObject* object)
    {
        assert(timer_locker != NULL);
        timer_locker->lock();
        MGTimerMapIt it;
        for (it = timer_map.begin(); it != timer_map.end(); ++it) {
            if (it->second->timer_event._responder == object) {
                it->second->timer_event._responder = NULL;
            }
            if (it->second->timer_event._content == object) {
                it->second->timer_event._content = NULL;
            }
            if (it->second->timer_event._handler.sender == object) {
                it->second->timer_event._handler.sender = NULL;
            }
        }
        timer_locker->unlock();
    }
    MGTimer* MGTimer::timerWithTimeInterval(const MGTimeDelta& interval, MIEvent event, bool repeat_or_not)
    {
        assert(timer_locker != NULL);
        timer_locker->lock();
        MGTimer* ret = NULL;
        ret = new MGTimer();
        ret->timer_interval = interval;
        ret->is_repeat = repeat_or_not;
        ret->timer_event = event;
        ret->is_valid = false;
        if(assign_tag >= MAX_TIMER_TAG)
            assign_tag = TIMER_INVALID_TAG;
        ret->tag = ++assign_tag;
        assert(timer_map.find(ret->tag) == timer_map.end());
        timer_map[ret->tag] = ret;
        timer_locker->unlock();
        return ret;
    }
    void MGTimer::clearAllTimers()
    {
        assert(timer_locker != NULL);
        timer_locker->lock();
        while (timer_map.size() > 0) {
			MGTimerMapIt it = timer_map.begin();
            delete it->second;
        }
        timer_map.clear();
        timer_locker->unlock();
    }
    
    bool MGTimer::isValid()
    {
        bool ret = false;
        assert(timer_locker != NULL);
        timer_locker->lock();
        ret = is_valid;
        timer_locker->unlock();
        return ret;
    }
    
    void MGTimer::invalidate()
    {
        assert(timer_locker != NULL);
        timer_locker->lock();
        if(is_valid)
        {
            is_valid = false;
            MIEvent* event = new MIEvent();
            event->_event_type = INVALIDATE_A_TIMER_EVENT;
            MGTimerObj* obj = new MGTimerObj(this);
            event->_content = obj;
            GuiContainer::container->preserveEvent(event);
        }
        timer_locker->unlock();
    }
    
    void MGTimer::validate()
    {
        assert(timer_locker != NULL);
        timer_locker->lock();
        if(!is_valid)
        {
            is_valid = true;
            MIEvent* event = new MIEvent();
            event->_event_type = VALIDATE_A_TIMER_EVENT;
            MGTimerObj* obj = new MGTimerObj(this);
            event->_content = obj;
            GuiContainer::container->preserveEvent(event);
        }
        timer_locker->unlock();
    }
    
    void MGTimer::fire()
    {
        assert(timer_locker != NULL);
        timer_locker->lock();
        if (is_valid == false) {
            timer_locker->unlock();
            return;
        }
        MIEvent* event = new MIEvent(timer_event);
        GuiContainer::container->preserveEvent(event);
        if (is_repeat == false) {
            is_valid = false;
        }
        timer_locker->unlock();
    }
    bool MGTimer::fireTimerWithTag(MGTimerTag tag)
    {
        bool ret = false;
        assert(timer_locker != NULL);
        timer_locker->lock();
        MGTimerMapIt it = timer_map.find(tag);
        if(it != timer_map.end())
        {
            it->second->fire();
            ret = it->second->is_repeat;
        }
        timer_locker->unlock();
        return ret;
    }
    
    MGTimerObj::MGTimerObj(MGTimer* timer)
    {
        tag = timer->tag; 
        timer_interval = timer->timer_interval;
        is_repeat = timer->is_repeat;
    }

} //namespace z11