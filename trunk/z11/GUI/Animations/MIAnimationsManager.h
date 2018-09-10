//
//  MIAnimationsManager.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-8.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "../Base/MIObject.h"
#include "../../System/Shared/Syncable.h"
#include "../../CommonBase/MGStructure.h"
#include "../../System/Shared/MGTime.h"
#include <map>
#include <string>
#include <deque>

using namespace std;

namespace z11
{
    class MIAnimationAlgorithm;
    class MIView;
    class MIResponder;
    class MIAnimation : public MIObject
    {
    public:
        MIAnimation();
        virtual ~MIAnimation();
    public:
        deque<MIView*> animations_in_view;
        float animation_duration;
        float repeat_count;
        int_16 all_step;
        int_16 step;
        bool first_begin;
        bool force_finished;
    public:
        void act();
        void begin();
        void end();
        void reinit();
        void setAlgorithm(MIAnimationAlgorithm* algorithm);
        void finishAnimation();
        void resetAnimationViews();
        bool isFinished();
    public:
        MIAnimationAlgorithm* animation_algorithm;
        MIResponder* will_start_responder;
        SELHandler will_start_handler;
        MIResponder* did_stop_responder;
        SELHandler did_stop_handler;
    protected:
        MGTimeTicks init_ticks;
    };
    
    class MIAnimationsManager : public MIObject,public Syncable
    {
    public:
        MIAnimationsManager();
        ~MIAnimationsManager();
    public:
        //动画标识操作//
        //push 和 popup必须配对使用，分别在animationBegin和animationCommit中被调用//
        void pushAnimationIndentityToStack(const string& animation_name);
        void popupAnimationIndentityFromStack();
        string topAnimationIndentityInStack();
        //注册动画//
        void registerViewAnimation(const string& animation_name, MIView* view);
        //注销动画//
        void unregisterViewAnimation(const string& animation_name);
        //是否等待需要执行操作动画//
        bool waitActionAnimations();
        //执行动画//
        void actAnimations();
        //动画主体销毁//
        bool isViewStillAttachAnimations(MIView* view);
        void detachAnimationFromView(MIView* view);
        void detachAnimationSelectorFromResponder(MIResponder* responder);
        MIAnimation* animationWithName(const string& name);
        void newAnimationWithName(const string& name);
    public:
        int_16 record_as_animation;
        bool animation_acting;
        bool permit_animation;
    protected:
        map<string, MIAnimation*> m_animation_map;
        deque<string> m_push_stack;
    };
    typedef map<string, MIAnimation*>::iterator AnimationMapIterator;
    typedef deque<MIView*>::iterator AnimationWithViewIterator;
} //namespace z11