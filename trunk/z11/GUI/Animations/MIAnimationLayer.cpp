//
//  MIAnimationLayer.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-11.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MIAnimationLayer.h"
#include "../Base/MIView.h"
#include "../Animations/MIAnimationsManager.h"
#include "MIAnimationAlgorithm.h"
#include "../../System/Shared/MGLog.h"
#include <assert.h>

namespace z11
{
    //MIAnimationLayer//
    MIAnimationLayer::MIAnimationLayer()
    {
        animating_frame = false;
        animating_center = false;
        animating_alpha = false;
        animating_transition = false;
        animating_rotate = false;
        animating_scale = false;
        animating_sub_transition = false;
        animating_sub_rotate = false;
        animating_sub_scale = false;
        setted_animate_frame = true;
        setted_animate_center = true;
        setted_animate_alpha = true;
        setted_animate_transition = true;
        setted_animate_rotate = true;
        setted_animate_scale = true;
        setted_animate_sub_transition = true;
        setted_animate_sub_rotate = true;
        setted_animate_sub_scale = true;
    }
    MIAnimationLayer::~MIAnimationLayer()
    {
        
    }
    void MIAnimationLayer::performAnimation(z11::MIView *view, z11::MIAnimationAlgorithm *algorithm, int_16 current_step, int_16 all_step)
    {
        assert(view != NULL);
        assert(algorithm != NULL);
        if (view == NULL || algorithm == NULL) {
            return;
        }
        //frame
        MGRect frame = view->getFrame();
        if (animating_frame) {
            frame.x = algorithm->calculateIntervalInStep(src_frame.x, dst_frame.x, current_step, all_step, frame.x);
            frame.y = algorithm->calculateIntervalInStep(src_frame.y, dst_frame.y, current_step, all_step, frame.y);
            frame.width = algorithm->calculateIntervalInStep(src_frame.width, dst_frame.width, current_step, all_step, frame.width);
            frame.height = algorithm->calculateIntervalInStep(src_frame.height, dst_frame.height, current_step, all_step, frame.height);
            view->setAniFrame(frame);
        }
        //center
        MGPoint center = view->getCenter();
        if (animating_center) {
            center.x = algorithm->calculateIntervalInStep(src_center.x, dst_center.x, current_step, all_step, center.x);
            center.y = algorithm->calculateIntervalInStep(src_center.y, dst_center.y, current_step, all_step, center.y);
            view->setAniCenter(center);
        }
        //alpha
        float alpha = view->getAlpha();
        if (animating_alpha) {
            alpha = algorithm->calculateIntervalInStep(src_alpha, dst_alpha, current_step, all_step, alpha);
            view->setAniAlpha(alpha);
        }
        //transition
        MGRectTransition transition = view->getRectTransition();
        if (animating_transition) {
            float x = algorithm->calculateIntervalInStep(src_transition.axis_x, dst_transition.axis_x, current_step, all_step, transition.axis_x);
            float y = algorithm->calculateIntervalInStep(src_transition.axis_y, dst_transition.axis_y, current_step, all_step, transition.axis_y);
            float z = algorithm->calculateIntervalInStep(src_transition.axis_z, dst_transition.axis_z, current_step, all_step, transition.axis_z);
            view->setAniRectTransition(MGRectTransitionMake(x, y, z));
        }
        //rotate
        MGRectRotate rotate = view->getRectRotate();
        if (animating_rotate) {
            float x = algorithm->calculateIntervalInStep(src_rotate.axis_x, dst_rotate.axis_x, current_step, all_step, rotate.axis_x);
            float y = algorithm->calculateIntervalInStep(src_rotate.axis_y, dst_rotate.axis_y, current_step, all_step, rotate.axis_y);
            float z = algorithm->calculateIntervalInStep(src_rotate.axis_z, dst_rotate.axis_z, current_step, all_step, rotate.axis_z);
            float angle = algorithm->calculateIntervalInStep(src_rotate.angle, dst_rotate.angle, current_step, all_step, rotate.angle);
            view->setAniRectRotate(MGRectRotateMake(x, y, z, angle));
        }
        //scale
        MGRectScale scale = view->getRectScale();
        if (animating_scale) {
            float x = algorithm->calculateIntervalInStep(src_scale.axis_x, dst_scale.axis_x, current_step, all_step, scale.axis_x);
            float y = algorithm->calculateIntervalInStep(src_scale.axis_y, dst_scale.axis_y, current_step, all_step, scale.axis_y);
            float z = algorithm->calculateIntervalInStep(src_scale.axis_z, dst_scale.axis_z, current_step, all_step, scale.axis_z);
            view->setAniRectScale(MGRectScaleMake(x, y, z));
        }
        //sub_transition
        MGRectTransition sub_transition = view->getSubRectTransition();
        if (animating_sub_transition) {
            float x = algorithm->calculateIntervalInStep(src_sub_transition.axis_x, dst_sub_transition.axis_x, current_step, all_step, sub_transition.axis_x);
            float y = algorithm->calculateIntervalInStep(src_sub_transition.axis_y, dst_sub_transition.axis_y, current_step, all_step, sub_transition.axis_y);
            float z = algorithm->calculateIntervalInStep(src_sub_transition.axis_z, dst_sub_transition.axis_z, current_step, all_step, sub_transition.axis_z);
            view->setAniSubRectTransition(MGRectTransitionMake(x, y, z));
        }
        //rotate
        MGRectRotate sub_rotate = view->getSubRectRotate();
        if (animating_sub_rotate) {
            float x = algorithm->calculateIntervalInStep(src_sub_rotate.axis_x, dst_sub_rotate.axis_x, current_step, all_step, sub_rotate.axis_x);
            float y = algorithm->calculateIntervalInStep(src_sub_rotate.axis_y, dst_sub_rotate.axis_y, current_step, all_step, sub_rotate.axis_y);
            float z = algorithm->calculateIntervalInStep(src_sub_rotate.axis_z, dst_sub_rotate.axis_z, current_step, all_step, sub_rotate.axis_z);
            float angle = algorithm->calculateIntervalInStep(src_sub_rotate.angle, dst_sub_rotate.angle, current_step, all_step, sub_rotate.angle);
            view->setAniSubRectRotate(MGRectRotateMake(x, y, z, angle));
        }
        //scale
        MGRectScale sub_scale = view->getSubRectScale();
        if (animating_sub_scale) {
            float x = algorithm->calculateIntervalInStep(src_sub_scale.axis_x, dst_sub_scale.axis_x, current_step, all_step, sub_scale.axis_x);
            float y = algorithm->calculateIntervalInStep(src_sub_scale.axis_y, dst_sub_scale.axis_y, current_step, all_step, sub_scale.axis_y);
            float z = algorithm->calculateIntervalInStep(src_sub_scale.axis_z, dst_sub_scale.axis_z, current_step, all_step, sub_scale.axis_z);
            view->setAniSubRectScale(MGRectScaleMake(x, y, z));
        }
    }
    /****** copyFromView *******
    需要解决两种情况：参见 issue 26
    1、一个控件已经有某些属性设置了动画效果，此时对另外没有设置动画效果的属性进行直接设置。
    2、一个控件已经有某些属性设置了动画效果，此时对设置过动画效果的属性进行直接设置。
    第一种情况下，对于设置了动画的属性，不应该进行copy，即 !animating_attribute。
    第二种情况下，需要copy的属性也被设置了动画，此时又需要copy，因此在copy之前，将animating_attribute置为false。
    同时，动画的执行过程其实就是在不断的设置属性的过程，因此要避免在动画执行过程中的设置影响了直接设置。
    因此在设置时判断是否是执行动画的一部分，如果是则不将animating_attribute置为false，如果不是，则置为false从而实现直接设置。
    ***************************/
    void MIAnimationLayer::copyFromView(z11::MIView *view)
    {
        if (!animating_frame) {// || !MIView::animations_manager->animation_acting) {
            dst_frame = view->getFrame();
            src_frame = dst_frame;
        }
        if (!animating_center) {// || !MIView::animations_manager->animation_acting) {
            dst_center = view->getCenter();
            src_center = dst_center;
        }
        if (!animating_alpha) {// || !MIView::animations_manager->animation_acting) {
            dst_alpha = view->getAlpha();
            src_alpha = dst_alpha;
        }
        if (!animating_transition) {// || !MIView::animations_manager->animation_acting) {
            dst_transition = view->getRectTransition();
            src_transition = dst_transition;
        }
        if (!animating_rotate) {// || !MIView::animations_manager->animation_acting) {
            dst_rotate = view->getRectRotate();
            src_rotate = dst_rotate;
        }
        if (!animating_scale) {// || !MIView::animations_manager->animation_acting) {
            dst_scale = view->getRectScale();
            src_scale = dst_scale;
        }
        if (!animating_sub_transition) {// || !MIView::animations_manager->animation_acting) {
            dst_sub_transition = view->getSubRectTransition();
            src_sub_transition = dst_sub_transition;
        }
        if (!animating_sub_rotate) {// || !MIView::animations_manager->animation_acting) {
            dst_sub_rotate = view->getSubRectRotate();
            src_sub_rotate = dst_sub_rotate;
        }
        if (!animating_sub_scale) {// || !MIView::animations_manager->animation_acting) {
            dst_sub_scale = view->getSubRectScale();
            src_sub_scale = dst_sub_scale;
        }
    }
    void MIAnimationLayer::copyFromViewWhenAnimationFinished(z11::MIView *view)
    {
        dst_frame = view->getFrame();
        src_frame = dst_frame;
        animating_frame = false;
        dst_center = view->getCenter();
        src_center = dst_center;
        animating_center = false;
        dst_alpha = view->getAlpha();
        src_alpha = dst_alpha;
        animating_alpha = false;
        dst_transition = view->getRectTransition();
        src_transition = dst_transition;
        animating_transition = false;
        dst_rotate = view->getRectRotate();
        src_rotate = dst_rotate;
        animating_rotate = false;
        dst_scale = view->getRectScale();
        src_scale = dst_scale;
        animating_scale = false;
        dst_sub_transition = view->getSubRectTransition();
        src_sub_transition = dst_sub_transition;
        animating_sub_transition = false;
        dst_sub_rotate = view->getSubRectRotate();
        src_sub_rotate = dst_sub_rotate;
        animating_sub_rotate = false;
        dst_sub_scale = view->getSubRectScale();
        src_sub_scale = dst_sub_scale;
        animating_sub_scale = false;
    }
} //namespace z11