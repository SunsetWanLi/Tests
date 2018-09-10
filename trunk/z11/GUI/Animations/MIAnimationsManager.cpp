//
//  MIAnimationsManager.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-8.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MIAnimationsManager.h"
#include "MIAnimationAlgorithm.h"
#include "../LightGraphic/LightGraphic.h"
#include "../Base/MIView.h"
#include "../GuiContainer/GuiContainer.h"
#include "MIAnimationLayer.h"
#include "../Base/MIResponder.h"
#include <assert.h>

namespace z11
{
//MIAnimation//
    MIAnimation::MIAnimation()
    {
        reinit();
        animation_algorithm = new MIAnimationAlgorithm();
        force_finished = false;
    }
    MIAnimation::~MIAnimation()
    {
        animations_in_view.clear();
        if (animation_algorithm != NULL) {
            delete animation_algorithm;
            animation_algorithm = NULL;
        }
    }
    void MIAnimation::reinit()
    {
        animations_in_view.clear();
        repeat_count = 1.0f;
        animation_duration = 0.0f;
        step = 0;
        all_step = 0;
        first_begin = true;
        will_start_responder = NULL;
        will_start_handler.sender = NULL;
        will_start_handler.sel_func = NULL;
        did_stop_responder = NULL;
        did_stop_handler.sender = NULL;
        did_stop_handler.sel_func = NULL;
    }
    void MIAnimation::setAlgorithm(z11::MIAnimationAlgorithm *algorithm)
    {
        if (algorithm == animation_algorithm || algorithm == NULL) {
            return;
        }
        if (animation_algorithm != NULL) {
            delete animation_algorithm;
            animation_algorithm = NULL;
        }
        animation_algorithm = algorithm;
    }
    void MIAnimation::begin()
    {
        first_begin = false;
        if (will_start_handler.sel_func != NULL) {
            MIEvent* event = new MIEvent;
            event->_event_type = WILL_START_ANIMATION_SELECTOR_EVENT;
            event->_responder = will_start_responder;
            event->_handler = will_start_handler;
            GuiContainer::container->preserveEvent(event);
        }
    }
    void MIAnimation::end()
    {
        if (did_stop_handler.sel_func != NULL) {
            MIEvent* event = new MIEvent;
            event->_event_type = DID_STOP_ANIMATION_SELECTOR_EVENT;
            event->_responder = did_stop_responder;
            event->_handler = did_stop_handler;
            GuiContainer::container->preserveEvent(event);
        }
    }
    bool MIAnimation::isFinished()
    {
        if (force_finished) {
            return true;
        }
        float fps = LightGraphic::getSettingFps();
        int_16 t_all_step = animation_duration * fps;
        if (t_all_step < 1) {
            t_all_step = 1;
        }
        bool ret = false;
        do{
            if (!first_begin && step > t_all_step - all_step) {
                break;
            }
            if (repeat_count == 0.0f) {
                ret = true;
                break;
            }
            //小于0代表无限循环，动画效果持续进行 //
            if (repeat_count < 0.0f) {
                resetAnimationViews();
                break;
            }else{
                if (repeat_count < 1.0f) {
                    resetAnimationViews();
                    all_step = (int_16)(t_all_step * repeat_count);
                    repeat_count = 0.0f;
                    break;
                } else {
                    resetAnimationViews();
                    repeat_count -= 1.0f;
                    break;
                }
            }
        }while(0);
        return ret;
    }
    void MIAnimation::act()
    {
        float fps = LightGraphic::getSettingFps();
        int_16 t_all_step = animation_duration * fps;
        if (t_all_step < 1) {
            t_all_step = 1;
        }
        if (MIView::animations_manager->permit_animation == false) {
            step = 1;
        }
        AnimationWithViewIterator sub_it;
        for (sub_it = animations_in_view.begin(); sub_it != animations_in_view.end(); ++sub_it) {
            MIView* view = (*sub_it);
            assert(view != NULL);
            if (view == NULL || view->m_animation_layer == NULL) {
                continue;
            }
            view->m_animation_layer->performAnimation(view, animation_algorithm, step, t_all_step);
        }
        MGTimeTicks now = MGTimeTicks::Now();
        MGTimeDelta delta = now - init_ticks;
        int_16 current_step = delta.InSecondsF() * fps;
        while (step + current_step >= t_all_step) {
            --step;
            if (step == 1) {
                break;
            }
        }
        
    }
    void MIAnimation::resetAnimationViews()
    {
        step = all_step;
        init_ticks = MGTimeTicks::Now();
        AnimationWithViewIterator sub_it;
        for (sub_it = animations_in_view.begin(); sub_it != animations_in_view.end(); ++sub_it) {
            MIView* view = (*sub_it);
            assert(view != NULL);
            if (view == NULL || view->m_animation_layer == NULL) {
                continue;
            }
            view->initAnimation();
        }
    }
    void MIAnimation::finishAnimation()
    {
        AnimationWithViewIterator sub_it;
        for (sub_it = animations_in_view.begin(); sub_it != animations_in_view.end(); ++sub_it) {
            MIView* view = (*sub_it);
            assert(view != NULL);
            if (view == NULL || view->m_animation_layer == NULL) {
                continue;
            }
            if (!MIView::animations_manager->isViewStillAttachAnimations(view)) {
                view->m_animation_layer->copyFromViewWhenAnimationFinished(view);
            }
        }
    }
//MIAnimationsManager//
    MIAnimationsManager::MIAnimationsManager()
    {
        m_animation_map.clear();
        m_push_stack.clear();
        record_as_animation = 0;
        animation_acting = false;
        permit_animation = true;
    }
    MIAnimationsManager::~MIAnimationsManager()
    {
        m_animation_map.clear();
        m_push_stack.clear();
    }
    void MIAnimationsManager::pushAnimationIndentityToStack(const string &animation_name)
    {
        this->lock();
        ++record_as_animation;
        m_push_stack.push_back(animation_name);
        newAnimationWithName(animation_name);
    }
    void MIAnimationsManager::popupAnimationIndentityFromStack()
	{
        --record_as_animation;
        if (record_as_animation < 0) {
            record_as_animation = 0;
        }
        m_push_stack.pop_back();
        this->unlock();
    }
    string MIAnimationsManager::topAnimationIndentityInStack()
    {
        AUTO_LOCK_THIS;
        return m_push_stack.back();
    }
    void MIAnimationsManager::actAnimations()
    {
        AUTO_LOCK_THIS;
        animation_acting = true;
        AnimationMapIterator it;
        for (it = m_animation_map.begin(); it != m_animation_map.end();) {
            MIAnimation* animation = it->second;
            assert(animation != NULL);
            if (animation == NULL) {
                m_animation_map.erase(it++);
                continue;
            }
            //判断动画是否执行完毕 //
            if (animation->isFinished()) {
                m_animation_map.erase(it++);
                animation->finishAnimation();
                animation->end();
                delete animation;
                continue;
            }
            //该动画第一次被执行 //
            if (animation->first_begin) {
                animation->begin();
            }
            //执行一帧动画 //
            animation->act();
            ++it;
        }
        animation_acting = false;
    }
    bool MIAnimationsManager::isViewStillAttachAnimations(MIView *view)
    {
        bool ret = false;
        AUTO_LOCK_THIS;
        AnimationMapIterator it;
        for (it = m_animation_map.begin(); it != m_animation_map.end(); ++it) {
            MIAnimation* animation = it->second;
            assert(animation != NULL);
            if (animation == NULL) {
                continue;
            }
            AnimationWithViewIterator sub_it;
            for (sub_it = animation->animations_in_view.begin(); sub_it != animation->animations_in_view.end();) {
                MIView* animation_with_view = (*sub_it);
                if (animation_with_view == view) {
                    return true;
                }else{
                    ++sub_it;
                }
            }
        }
        return ret;
    }
    // 不仅在View销毁的时候需要清理动画管理器中涉及该View的动画，当Controller在销毁时，也应该清理涉及该Controller的动画中设置的Responder //
    void MIAnimationsManager::detachAnimationFromView(MIView* view)
    {
        AUTO_LOCK_THIS;
        AnimationMapIterator it;
        for (it = m_animation_map.begin(); it != m_animation_map.end(); ++it) {
            MIAnimation* animation = it->second;
            assert(animation != NULL);
            if (animation == NULL) {
                continue;
            }
            AnimationWithViewIterator sub_it;
            for (sub_it = animation->animations_in_view.begin(); sub_it != animation->animations_in_view.end();) {
                MIView* animation_with_view = (*sub_it);
                if (animation_with_view == view) {
					sub_it = animation->animations_in_view.erase(sub_it);
                }else{
                    ++sub_it;
                }
            }
        }
    }
    void MIAnimationsManager::detachAnimationSelectorFromResponder(z11::MIResponder *responder)
    {
        AUTO_LOCK_THIS;
        AnimationMapIterator it;
        for (it = m_animation_map.begin(); it != m_animation_map.end(); ++it) {
            MIAnimation* animation = it->second;
            assert(animation != NULL);
            if (animation == NULL) {
                continue;
            }
            if (animation->did_stop_responder == responder)
            {
                animation->did_stop_responder=NULL;
                animation->did_stop_handler.sel_func=NULL;
                animation->did_stop_handler.sender=NULL;
            }
            if (animation->did_stop_handler.sender == responder)
            {
                animation->did_stop_handler.sender=NULL;
            }
            if (animation->will_start_responder == responder)
            {
                animation->will_start_responder=NULL;
                animation->will_start_handler.sel_func=NULL;
                animation->will_start_handler.sender=NULL;
            }
            if (animation->will_start_handler.sender == responder)
            {
                animation->will_start_handler.sender=NULL;
            }
        }
    }
    MIAnimation* MIAnimationsManager::animationWithName(const string& name)
    {
        AUTO_LOCK_THIS;
        MIAnimation* ret = NULL;
        AnimationMapIterator it = m_animation_map.find(name);
        if (it != m_animation_map.end()) {
            ret = it->second;
        }else{
            ret = new MIAnimation();
            m_animation_map[name] = ret;
        }
        return ret;
    }
    void MIAnimationsManager::newAnimationWithName(const string &name)
    {
        AUTO_LOCK_THIS;
        AnimationMapIterator it = m_animation_map.find(name);
        if (it != m_animation_map.end()) {
            it->second->animations_in_view.clear();
            it->second->reinit();
        }else{
            MIAnimation* animation = new MIAnimation();
            m_animation_map[name] = animation;
        }
    }
    void MIAnimationsManager::registerViewAnimation(const string &animation_name, z11::MIView *view)
    {
        AUTO_LOCK_THIS;
        MIAnimation* animation = animationWithName(animation_name);
        assert(animation != NULL);
        animation->animations_in_view.push_back(view);
    }
    void MIAnimationsManager::unregisterViewAnimation(const string &animation_name)
    {
        AUTO_LOCK_THIS;
        AnimationMapIterator it = m_animation_map.find(animation_name);
        if (it != m_animation_map.end()) {
            assert(it->second != NULL);
            MIAnimation* animation = it->second;
//            m_animation_map.erase(it);
//            animation->finishAnimation();
//            delete animation;
            animation->force_finished = true;
        }
    }
    bool MIAnimationsManager::waitActionAnimations()
    {
        bool ret = false;
        AUTO_LOCK_THIS;
        for (AnimationMapIterator it = m_animation_map.begin(); it != m_animation_map.end(); ++it) {
            assert(it->second != NULL);
            MIAnimation* animation = it->second;
            if (animation->did_stop_responder != NULL && animation->did_stop_handler.sel_func != NULL) {
                ret = true;
                break;
            }
            if (animation->will_start_responder != NULL && animation->will_start_handler.sel_func != NULL) {
                ret = true;
                break;
            }
        }
        return ret;
    }
    
} //namespace z11