//
//  MITouchTracer.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-7.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

/*
 MITouchTracer跟踪的是由系统产生的简单触摸事件生成的MITouch对象。
 */

#pragma once

#include "MIObject.h"
#include "../../CommonBase/CommonDef.h"
#include "../Events/MITouch.h"
#include "../../System/Shared/Syncable.h"
#include <map>

using namespace std;

namespace z11
{
    class MITouchTracer : public MIObject, public Syncable
    {
    public:
        MITouchTracer();
        ~MITouchTracer();
    protected:
        map<MITouchTag, MITouch*> m_touch_map;
        static uint_32 m_touch_tag;
    public:
        //  依据触摸的Tag值来跟踪触摸    //
        bool traceTouchWithTag(MITouch* touch, MITouchEventType type);
        //  跟踪触摸并付值状态  //
        bool traceTouchWithPhase(MITouch* touch, MITouchEventType type);
        //  注册之前的MITouch只有坐标信息  //
        void registerTouchWithView(MITouch* touch, MITouchEventType type, MIView* view);
    protected:
        void updateTouchesPhaseWithTouch(MITouch* touch);
        void addTouchToMap(MITouch* touch);
        void replaceTouchInMap(MITouch* touch);
        void removeTouchFromMap(MITouch* touch);
        MITouch* nearestTouch(MITouch* touch);
    public:
        void detachAllTouchesInView(MIView* view);
        void print();
    };
    typedef map<MITouchTag, MITouch*>::iterator MITouchMapIt;
} //namespace z11