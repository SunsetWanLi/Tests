//
//  MGTimer.h
//  System
//
//  Created by Wang ZhiPeng on 11-8-2.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../CommonBase/MGObject.h"
#include "../Shared/Syncable.h"
#include "../Shared/MGTime.h"
#include "../../GUI/Events/MIEvent.h"
#include <map>

using namespace std;

namespace z11 
{
    typedef int_32 MGTimerTag;
#define TIMER_INVALID_TAG -1
#define MAX_TIMER_TAG 0x0FFFFFFF
    class MGTimer : public MGObject
    {
    protected:
        MGTimer();
    public:
        virtual ~MGTimer();
    public:
        void setTimerInterval(const MGTimeDelta& delta);
        MGTimeDelta getTimerInterval(){return timer_interval;}
        bool getRepeatOrNot(){return is_repeat;}
    protected:
        MGTimeDelta timer_interval;
    protected:
        bool is_repeat;
        bool is_valid;
        MIEvent timer_event;
    protected:
        MGTimerTag tag;
        static MGTimerTag assign_tag;
    public:
        MGTimerTag getTag(){return tag;}
        bool isValid();
        void invalidate();
        void validate();
    public:
        class MGTimerLocker : public Syncable
        {
        public:
            MGTimerLocker(){}
            virtual ~MGTimerLocker(){}
        };
        static MGTimerLocker* timer_locker;
        static std::map<MGTimerTag, MGTimer*> timer_map;
        // 返回true，代表需要重复执行 //
        static bool fireTimerWithTag(MGTimerTag);
    public:
        static void detachObjectFromTimer(MGObject* object);
        static MGTimer* timerWithTimeInterval(const MGTimeDelta& interval, MIEvent event, bool repeat_or_not);
        static void clearAllTimers();
    protected:
        void fire();
        friend class MGTimerObj;
    };
    typedef std::map<MGTimerTag, MGTimer*>::iterator MGTimerMapIt;
    class MGTimerObj : public MGObject
    {
    public:
        MGTimerObj(MGTimer*);
    public:
        MGTimerTag getTag(){return tag;}
        MGTimeDelta getInterval(){return timer_interval;}
        bool getRepeatOrNot(){return is_repeat;}
    protected:
        MGTimerTag tag;
        MGTimeDelta timer_interval;
        bool is_repeat;
    };
} //namespace z11