//
//  MIAnimationLayer.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-11.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "../../System/Shared/Syncable.h"
#include "../../CommonBase/CommonDef.h"
#include "../Base/MIObject.h"
#include "../../CommonBase/MGStructure.h"

namespace z11
{
    class MIView;
    class MIAnimationAlgorithm;
    //可以进行动画效果的层，所有能够有动画效果的变量全部在此定义//
    class MIAnimationLayer : public MIObject, Syncable
    {
    public:
        MIAnimationLayer();
        virtual ~MIAnimationLayer();
    public:
        //可设置动画效果的变量//
        MGRect src_frame;
        MGRect dst_frame;
        MGRect ani_frame;
        bool setted_animate_frame;
        bool animating_frame;
        MGPoint src_center;
        MGPoint dst_center;
        MGPoint ani_center;
        bool setted_animate_center;
        bool animating_center;
        float src_alpha;
        float dst_alpha;
        float ani_alpha;
        bool setted_animate_alpha;
        bool animating_alpha;
        MGRectRotate src_rotate;
        MGRectRotate dst_rotate; //在三维空间中的三轴角度 //
        MGRectRotate ani_rotate;
        bool setted_animate_rotate;
        bool animating_rotate;
        MGRectTransition src_transition;
        MGRectTransition dst_transition; //在三维空间中的位移量 //
        MGRectTransition ani_transition;
        bool setted_animate_transition;
        bool animating_transition;
        MGRectScale src_scale;
        MGRectScale dst_scale; //在三维空间中的缩放向量 //
        MGRectScale ani_scale;
        bool setted_animate_scale;
        bool animating_scale;
        MGRectRotate src_sub_rotate;
        MGRectRotate dst_sub_rotate; //在三维空间中的三轴角度 //
        MGRectRotate ani_sub_rotate;
        bool setted_animate_sub_rotate;
        bool animating_sub_rotate;
        MGRectTransition src_sub_transition;
        MGRectTransition dst_sub_transition; //在三维空间中的位移量 //
        MGRectTransition ani_sub_transition;
        bool setted_animate_sub_transition;
        bool animating_sub_transition;
        MGRectScale src_sub_scale;
        MGRectScale dst_sub_scale; //在三维空间中的缩放向量 //
        MGRectScale ani_sub_scale;
        bool setted_animate_sub_scale;
        bool animating_sub_scale;
    public:
        void performAnimation(MIView* view, MIAnimationAlgorithm* algorithm, int_16 current_step, int_16 all_step);
        void copyFromView(MIView* view);
        void copyFromViewWhenAnimationFinished(MIView* view);
    };

} //namespace z11
