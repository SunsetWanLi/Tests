//
//  MITouchTracer.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-7.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MITouchTracer.h"
#include <assert.h>
#include <math.h>
#include "System/Shared/MGLog.h"
namespace z11
{
    uint_32 MITouchTracer::m_touch_tag = 0;
    
    MITouchTracer::MITouchTracer()
    {
        
    }
    
    MITouchTracer::~MITouchTracer()
    {
        
    }
    
    bool MITouchTracer::traceTouchWithTag(MITouch* touch, MITouchEventType type)
    {
		//MGLogD("traceTouchWithTag num=%d",m_touch_map.size());
        bool ret = false;
        AUTO_LOCK_THIS;
        MITouchMapIt it = m_touch_map.find(touch->tag);
        if (it == m_touch_map.end()) {
            ret = false;
        } else {
            it->second->phase = touch->phase;
            touch->point = it->second->point;
            touch->screen_point = it->second->screen_point;
            it->second->maintained_view = touch->maintained_view;
            ret = true;
        }
        return ret;
    }
    
    bool MITouchTracer::traceTouchWithPhase(MITouch *touch, MITouchEventType type)
	{
        bool ret = true;
        switch (type) {
            case MITouchEventBegan:
            {
                touch->tag = ++m_touch_tag;
                if (m_touch_tag >= 0xFFFF) {
                    m_touch_tag = 0;
                }
                touch->phase = MITouchPhaseBegan;
                touch->saved_point = touch->point;
                touch->screen_point = touch->point;
				addTouchToMap(touch);
				//MGLogD("after MITouchEventBegan traceTouchWithPhase num=%d",m_touch_map.size());
                break;
            }
            case MITouchEventMoved:
            {
                MITouch* previous_touch = nearestTouch(touch);
                assert(previous_touch != NULL);
                touch->tag = previous_touch->tag;
                touch->phase = MITouchPhaseMoved;
                touch->saved_point = previous_touch->saved_point;
                touch->screen_point = touch->point;
                touch->maintained_view = previous_touch->maintained_view;
				replaceTouchInMap(touch);
				//MGLogD("after MITouchEventMoved traceTouchWithPhase num=%d",m_touch_map.size());
                break;
            }
            case MITouchEventEnded:
            {
                MITouch* previous_touch = nearestTouch(touch);
                assert(previous_touch != NULL);
                touch->tag = previous_touch->tag;
                touch->phase = MITouchPhaseEnded;
                touch->saved_point = previous_touch->saved_point;
                touch->screen_point = touch->point;
                touch->maintained_view = previous_touch->maintained_view;
				removeTouchFromMap(touch);
				//MGLogD("after MITouchEventEnded traceTouchWithPhase num=%d",m_touch_map.size());
                break;
            }
            case MITouchEventCancelled:
            {
                MITouch* previous_touch = nearestTouch(touch);
                if (previous_touch == NULL) {
                    ret = false;
                    break;
                }
                assert(previous_touch != NULL);
                touch->tag = previous_touch->tag;
                touch->phase = MITouchPhaseCancelled;
                touch->saved_point = previous_touch->saved_point;
                touch->screen_point = touch->point;
                touch->maintained_view = previous_touch->maintained_view;
				removeTouchFromMap(touch);
				//MGLogD("after MITouchEventCancelled traceTouchWithPhase num=%d",m_touch_map.size());
                break;
            }
            default:
                break;
        }
        return ret;
    }
    
    void MITouchTracer::registerTouchWithView(MITouch *touch, MITouchEventType type, MIView* view)
    {
        AUTO_LOCK_THIS;
        MITouchMapIt it = m_touch_map.find(touch->tag);
        if (it == m_touch_map.end()) {
            return;
        }
        if ((type == MITouchEventBegan || touch->phase == MITouchPhaseBegan) && touch->maintained_view == NULL) {
            assert(it->second != NULL);
            it->second->maintained_view = view;
            touch->maintained_view = view;
        }
    }
    void MITouchTracer::updateTouchesPhaseWithTouch(MITouch* touch)
    {
        AUTO_LOCK_THIS;
        MITouchMapIt it;
        for (it = m_touch_map.begin(); it != m_touch_map.end(); ++it) {
            if (it->second == touch) {
                continue;
            }
            if (it->second->phase == MITouchPhaseBegan) {
                // 这个状态只能保存在TouchTracer里面，所以只能用于基于全体触摸序列的触摸跟踪和手势检测，而且由于需要其他触摸触发，因此这个状态还不一定准确 //
                it->second->phase = MITouchPhaseStationary;
            }
        }
    }
    void MITouchTracer::print()
    {
        AUTO_LOCK_THIS;
        for (MITouchMapIt it=m_touch_map.begin(); it!=m_touch_map.end(); it++) {
            MGLogD("MITouchTracer's num=%d",m_touch_map.size());
        }
    }
    void MITouchTracer::addTouchToMap(MITouch *touch)
    {
        AUTO_LOCK_THIS;
        MITouchMapIt it = m_touch_map.find(touch->tag);
        while (it != m_touch_map.end()) {
            it = m_touch_map.find(++touch->tag);
        }
        MITouch* dst_touch = new MITouch(touch);
        m_touch_map[dst_touch->tag] = dst_touch;
        updateTouchesPhaseWithTouch(dst_touch);
    }
    void MITouchTracer::replaceTouchInMap(MITouch *touch)
    {
        AUTO_LOCK_THIS;
        MITouchMapIt it = m_touch_map.find(touch->tag);
        if (it != m_touch_map.end()) {
            assert(it->second != NULL);
            delete it->second;
        }
        MITouch* dst_touch = new MITouch(touch);
        m_touch_map[dst_touch->tag] = dst_touch;
        updateTouchesPhaseWithTouch(dst_touch);
    }
    void MITouchTracer::removeTouchFromMap(MITouch *touch)
    {
        AUTO_LOCK_THIS;
        MITouchMapIt it = m_touch_map.find(touch->tag);
        if (it != m_touch_map.end()) {
            assert(it->second != NULL);
            delete it->second;
            m_touch_map.erase(it);
        }
        updateTouchesPhaseWithTouch(NULL);
    }
    MITouch* MITouchTracer::nearestTouch(MITouch* touch)
    {
        AUTO_LOCK_THIS;
        MITouch* ret = NULL;
        MITouchMapIt it;
        float distance = 0.0f;
        bool func_flag = true;
        for (it = m_touch_map.begin(); it != m_touch_map.end(); ++it) {
            assert(it->second != NULL);
            float tmp_dis = sqrtf(pow((touch->point.x - it->second->point.x) , 2) + pow((touch->point.y - it->second->point.y), 2));
            if (tmp_dis < distance || func_flag) {
                func_flag = false;
                distance = tmp_dis;
                ret = it->second;
            }
        }
        return ret;
    }
    void MITouchTracer::detachAllTouchesInView(MIView* view)
    {
        AUTO_LOCK_THIS;
        MITouchMapIt it;
        for (it = m_touch_map.begin(); it != m_touch_map.end(); ++it) {
            assert(it->second != NULL);
            if (it->second->maintained_view == view) {
                it->second->maintained_view = NULL;
            }
            if (it->second->touched_view == view) {
                it->second->touched_view = NULL;
            }
        }
    }
} //namespace z11