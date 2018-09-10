//
//  MITouch.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-22.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "../Base/MIObject.h"
#include "../../CommonBase/MGStructure.h"
#include "../../System/Shared/MGTime.h"
#include <set>

namespace z11
{
    enum {
        MITouchEventBegan = 0,
        MITouchEventMoved = 1,
        MITouchEventEnded = 2,
        MITouchEventCancelled = 3
    };
#define INVALID_TOUCH_TAG -1
    typedef uint_32 MITouchEventType;
    typedef int_32 MITouchTag;
    typedef enum
    {
        MITouchPhaseBegan,             // whenever a finger touches the surface.
        MITouchPhaseMoved,             // whenever a finger moves on the surface.
        MITouchPhaseStationary,        // whenever a finger is touching the surface but hasn't moved since the previous event.
        MITouchPhaseEnded,             // whenever a finger leaves the surface.
        MITouchPhaseCancelled,         // whenever a touch doesn't end but we need stop tracking. (put device to face)
    }MITouchPhase;
    class MIView;
    class MITouch : public MIObject
    {
    public:
        MITouch();
        MITouch(MITouch* touch);
        virtual ~MITouch();
    public:
        MGTimeTicks timestamp;   /* 触摸发生时间 */
        MGPoint point; /*  当前触摸实例的坐标,经过控件触摸检测的各种变形转换后的点 */
        MGPoint screen_point; /*  实际点击的屏幕坐标   */
        MGPoint saved_point; /* 保存的坐标，一般是在Move或者Up实例中会有保存的Down时候的坐标 */
        MITouchTag tag; /* 触摸实例的标识符  */
        MITouchPhase phase; /* 触摸实例所处的生命周期阶段 */
        MIView* maintained_view; /*当前触摸的起始View*/
        MIView* touched_view; /*触摸检测的结果View*/
    public:
        MGPoint locationInView(MIView* view) const; // 从屏幕坐标点，触摸点转到在当前view坐标系变幻下的位置，和view的m_bounds是同一个坐标系 //
        MITouch* nearestTouch(const std::set<MITouch*> touches);
        float nearestDis(const std::set<MITouch*> touches);
    };
} //namespace z11