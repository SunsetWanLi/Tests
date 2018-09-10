//
//  MIView.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-23.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MIView.h"
#include "../../GraphicCore/MGImage.h"
#include "MITouchTracer.h"
#include "../Animations/MIAnimationsManager.h"
#include "../Controllers/MIViewController.h"
#include "../LightGraphic/LightGraphic.h"
#include "../../System/Shared/MGLog.h"
#include "../Animations/MIAnimationLayer.h"
#include "../GuiContainer/GuiContainer.h"
#include "../../GraphicCore/MGGraphicEngine.h"
#include "../Animations/MIAnimationAlgorithm.h"
#include "../MIControls/MIScreenCaptureView.h"
#include "../MIControls/MITextView.h"
#include "../MIControls/MITextField.h"
#include "../MIControls/MIButton.h"
#include "../MIControls/MILabel.h"
#include "../GestureRecognizer/MIGestureRecognizer.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "../../System/Shared/Utility.h"
#undef M_PI
#define M_PI        3.14159265358979323846264338327950288

#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif

#define MIVIEW_MOVE_SIGNIFICANT_CHANGE_VALVE 6.0f

namespace z11
{
#pragma mark - MIViewTreeInfo -
    
    MIViewTreeInfo::MIViewTreeInfo()
    {
        class_name = "";
        background = "";
        text = "";
        extra_content = "";
        subviews.clear();
    }
    
    MIViewTreeInfo::~MIViewTreeInfo()
    {
        std::deque<MIViewTreeInfo*>::iterator it;
        for (it = subviews.begin(); it != subviews.end(); ++it) {
            delete_and_set_null(*it);
        }
        subviews.clear();
    }
    
    string MIViewTreeInfo::toUITree(uint_32 level)
    {
        string ret = "";
        for (int i = 0; i < level; ++i) {
            ret.append("|");
        }
        // Rect
        ret.append(class_name+"\t Background: ");
        ret.append(background+"\t Rect: {");
        ret.append(Utility::BuiltInTypeToString(bound.x)+",");
        ret.append(Utility::BuiltInTypeToString(bound.y)+",");
        ret.append(Utility::BuiltInTypeToString(bound.width)+",");
        ret.append(Utility::BuiltInTypeToString(bound.height)+"} ");
        // Transition;
        ret.append("{"+Utility::BuiltInTypeToString(transition.axis_x)+",");
        ret.append(Utility::BuiltInTypeToString(transition.axis_y)+",");
        ret.append(Utility::BuiltInTypeToString(transition.axis_z)+"} ");
        // Sub Transition
        ret.append("{"+Utility::BuiltInTypeToString(sub_transition.axis_x)+",");
        ret.append(Utility::BuiltInTypeToString(sub_transition.axis_y)+",");
        ret.append(Utility::BuiltInTypeToString(sub_transition.axis_z)+"}\r\n");
        ++level;
        std::deque<MIViewTreeInfo*>::iterator it;
        for (it = subviews.begin(); it != subviews.end(); ++it) {
            ret.append((*it)->toUITree(level));
        }
        return ret;
    }
    
#pragma mark - MIView -
    
    MITouchTracer* MIView::touch_tracer = NULL;
    MIAnimationsManager* MIView::animations_manager = NULL;
    float MIView::significant_move_valve_rate = MIVIEW_MOVE_SIGNIFICANT_CHANGE_VALVE / 640.0f;
    
#pragma mark - constructor -
    MIView::MIView()
    {
        class_name = "MIView";
        m_center = MGPointMake(0, 0);
        m_frame = MGRectMake(0, 0, 0, 0);
        m_bounds = MGRectMake(0, 0, 0, 0);
        m_rotate = MGRectRotateMake(0, 0, 0, 0);
        m_transition = MGRectTransitionMake(0, 0, 0);
        m_scale = MGRectScaleMake(1,1,1);
        sub_rotate = MGRectRotateMake(0, 0, 0, 0);
        sub_transition = MGRectTransitionMake(0, 0, 0);
        sub_scale = MGRectScaleMake(1, 1, 1);
        m_superview = NULL;
        m_alpha = 1.0f;
        m_hidden = false;
        m_auto_clean = true;
		m_background_color=NULL;
        m_animation_layer = new MIAnimationLayer();
        delegate = NULL;
        draw_image_mode = ImageScaleToFill;
        view_round_rate = _DEFAULT_ROUND_RATE_;
        view_round_mode = RoundRectNone;
        // View flags
        _view_flags.view_initialized = 0;
        _view_flags.user_interaction_enabled = 1;
        _view_flags.clip_subviews = 0;
        _view_flags.sub_transition_enable = 1;
        _view_flags.sub_scale_enable = 0;
        _view_flags.sub_rotate_enable = 0;
        _view_flags.user_multitouch_enabled = 1;
        _view_flags.user_multitouch_subview_enabled = 0;
        _view_flags.subview_enabled = 1;
        m_rotate_p = MGPointMake(0, 0);
        m_scale_p = MGPointMake(0, 0);
        m_need_sub_translate = false;
        m_have_touches_through_subview = false;
        can_handle_touch_from_superview = true;
        can_handle_touch = true;
        touch_hitted = false;
    }
    MIView::~MIView()
    {
        if (touch_tracer != NULL) {
            touch_tracer->detachAllTouchesInView(this);
        }
        if (animations_manager != NULL) {
            animations_manager->detachAnimationFromView(this);
            animations_manager->detachAnimationSelectorFromResponder(this);
        }
        //GuiContainer::container->clearEventWithObject(this); // 已经在MGObject的析构中处理 //
		GuiContainer::container->clearTouchesEventInView(this);
        
        AUTO_LOCK_THIS;
        SubviewIterator it;
        for (it = m_subviews.begin(); it != m_subviews.end(); ++it) {
            if(*it != NULL)
            {
                (*it)->m_superview = NULL;
                if((*it)->m_auto_clean)
                {
                    delete *it;
                }
            }
        }
        m_subviews.clear();
        if (m_animation_layer != NULL) {
            delete m_animation_layer;
            m_animation_layer = NULL;
        }
        if(m_background_color != NULL)
        {
            delete m_background_color;
            m_background_color=NULL;
        }
        m_gesture_recognizers.clear();

		if (delegate != NULL && delegate->view == this)
			delegate->view = NULL;
    }
    
#pragma mark - initialize -
    
    void MIView::initWithFrame(MGRect frame)
    {
        assert(frame.width >= 0.0f && frame.height >= 0.0f);
        assert(_view_flags.view_initialized == 0);
        AUTO_LOCK_THIS;
        m_frame = frame;
        if (m_superview != NULL) {
            m_bounds.x = frame.x + m_superview->m_bounds.x;
            m_bounds.y = frame.y + m_superview->m_bounds.y;
            m_bounds.width = frame.width;
            m_bounds.height = frame.height;
        }else{
            m_bounds = frame;
        }
        m_center = MGPointMake(m_bounds.width * 0.5f + m_bounds.x, m_bounds.height * 0.5f + m_bounds.y);
        SubviewIterator it;
        for (it = m_subviews.begin(); it != m_subviews.end(); ++it) {
            assert((*it) != NULL);
            (*it)->setFrame((*it)->m_frame);
        }
        assert(m_animation_layer != NULL);
        m_animation_layer->copyFromView(this);
        _view_flags.view_initialized = 1;
    }
    
#pragma mark - manage subviews -
    
    void MIView::removeAllSubviews()
    {
        AUTO_LOCK_THIS;
        SubviewIterator it;
        for (it = m_subviews.begin(); it != m_subviews.end();) {
            excludeOldSubview(*it);
            if(*it != NULL && (*it)->m_auto_clean)
            {
                delete *it;
            }
			it = m_subviews.erase(it);
        }
        m_subviews.clear();
    }
    
    //视图结构
    void MIView::detachFromSuperview()
    {
        if (m_superview == NULL) {
            return;
        }
        bool auto_clean = this->m_auto_clean;
        this->m_auto_clean = false;
        m_superview->removeSubview(this);
        this->m_auto_clean = auto_clean;
		m_superview = NULL;
    }
    void MIView::addSubview(MIView* subview)
    {
        if(containedSubview(subview)) {
            return;
        }
        assert(subview->_view_flags.view_initialized == 1);
        assert(subview->m_superview == NULL);
        subview->detachFromSuperview();
        
        AUTO_LOCK_THIS;

        m_subviews.push_back(subview);
		subview->m_superview=this;//让子view的m_superview指向自己 //
        subview->setFrame(subview->m_frame);
        includeNewSubview(subview);
    }
    void MIView::moveSubviewToIndex(z11::MIView *subview, uint_32 index)
    {
        AUTO_LOCK_THIS;
        if(subview == NULL || subview->m_superview != this)
            return;
        SubviewIterator it;
        for (it = m_subviews.begin(); it != m_subviews.end(); ++it) {
            assert((*it) != NULL);
            if(*it == subview)
            {
                break; //for
            }
        }
        if(it != m_subviews.end()) {
            m_subviews.erase(it);
        }
        it = m_subviews.begin() + index;
        if(it != m_subviews.end())
        {
            m_subviews.insert(it, subview);
        }
        else
        {
            m_subviews.push_back(subview);
        }
    }
    void MIView::moveSubviewToBack(z11::MIView *subview)
    {
        AUTO_LOCK_THIS;
        if(subview == NULL || subview->m_superview != this)
            return;
        SubviewIterator it;
        for (it = m_subviews.begin(); it != m_subviews.end(); ++it) {
            assert((*it) != NULL);
            if(*it == subview)
            {
                break; //for
            }
        }
        if(it != m_subviews.end()) {
            m_subviews.erase(it);
        }
        m_subviews.push_back(subview);
    }
    void MIView::insertSubviewAtIndex(MIView *subview, uint_32 index)
    {
        if (containedSubview(subview)) {
            return;
        }
        assert(subview->_view_flags.view_initialized == 1);
        AUTO_LOCK_THIS;
        SubviewIterator it = m_subviews.begin() + index;
        if(it != m_subviews.end())
        {
            m_subviews.insert(it, subview);
        }
        else
        {
            m_subviews.push_back(subview);
        }
        subview->m_superview=this;
        subview->setFrame(subview->m_frame);
        includeNewSubview(subview);
    }
    MIView* MIView::subviewAtIndex(uint_32 index)
    {
        AUTO_LOCK_THIS;
        MIView* ret = NULL;
        ret = m_subviews.at(index);
        return ret;
    }
    MIView* MIView::removeSubview(MIView* subview)
    {
        MIView* ret = subview;
        AUTO_LOCK_THIS;
        if(subview == NULL)
            return ret;
        SubviewIterator it;
        for (it = m_subviews.begin(); it != m_subviews.end(); ++it) {
            assert((*it) != NULL);
            if(*it == subview)
            {
                break; //for
            }
        }
        if(it != m_subviews.end()) {
            m_subviews.erase(it);
            excludeOldSubview(ret);
            if (ret->m_auto_clean) {
                delete ret;
                ret = NULL;
            }
        }
        return ret;
    }
    MIView* MIView::detachSubviewAtIndex(uint_32 index)
    {
        MIView * ret = NULL;
        AUTO_LOCK_THIS;
        if(index >= m_subviews.size())
            return ret;
        SubviewIterator it;
        it = m_subviews.begin()+index;
        assert((*it) != NULL);
        ret = *it;
        m_subviews.erase(it);
        excludeOldSubview(ret);
        if(ret->m_auto_clean) {
            delete ret;
            ret = NULL;
        }
        return ret;
    }
    MIView* MIView::subviewWithTag(const string& tag, int_32 index)
    {
        AUTO_LOCK_THIS;
        MIView* ret = NULL;
        SubviewIterator it;
        int_32 idx = 0;
        for (it = m_subviews.begin(); it != m_subviews.end(); ++it) {
            assert((*it) != NULL);
            if((*it)->class_name.className().compare(tag) == 0)
            {
                if (index < 0 || idx == index) {
                    ret = *it;
                    break;
                } else {
                    ++idx;
                }
            }
        }
        return ret;
    }
    MIView* MIView::subviewWithTagRecur(const string& tag)
    {
        AUTO_LOCK_THIS;
        MIView* ret = NULL;
        SubviewIterator it;
        for (it = m_subviews.begin(); it != m_subviews.end(); ++it) {
            assert((*it) != NULL);
            if((*it)->class_name.className().compare(tag) == 0)
            {
                ret = *it;
                break;
            }
        }
        if (ret == NULL) {
            for (it = m_subviews.begin(); it != m_subviews.end(); ++it) {
                assert((*it) != NULL);
                ret = (*it)->subviewWithTagRecur(tag);
                if (ret != NULL) {
                    break;
                }
            }
        }
        return ret;
    }
    MIViewTreeInfo* MIView::viewTree()
    {
        AUTO_LOCK_THIS;
        MIViewTreeInfo* ret = new MIViewTreeInfo();
        
        MGRect rect = getBounds();;
        transAndScaleClipRect(rect);
        ret->class_name = class_name.className();
        
        if (m_background_color != NULL) {
            if (m_background_color->m_image != NULL) {
                ret->background = m_background_color->m_image->getFilename();
            } else {
                RGBAColorRef ref = m_background_color->m_color;
                string r_str = Utility::BuiltInTypeToString<int>(ref.m_red_component, 2, std::hex);
                string g_str = Utility::BuiltInTypeToString<int>(ref.m_green_component, 2, std::hex);
                string b_str = Utility::BuiltInTypeToString<int>(ref.m_blue_component, 2, std::hex);
                string a_str = Utility::BuiltInTypeToString<int>(ref.m_alpha_component, 2, std::hex);
                ret->background = "0x" + r_str + g_str + b_str + a_str;
            }
        } else {
            ret->background = "default";
        }
        
        ret->bound = rect;
        ret->transition = m_transition;
        ret->sub_transition = sub_transition;
        
        MITextField* text_field = dynamic_cast<MITextField*>(this);
        if (text_field != NULL) {
            ret->text = text_field->getText();
            ret->extra_content = text_field->getPlaceholder();
        }
        
        MITextView* text_view = dynamic_cast<MITextView*>(this);
        if (text_view != NULL) {
            ret->text = text_view->getText();
            ret->extra_content = text_view->getPlaceholder();
        }
        
        MILabel* label = dynamic_cast<MILabel*>(this);
        if (label != NULL) {
            ret->text = label->getText();
        }
        
        MIButton* button = dynamic_cast<MIButton*>(this);
        if (button != NULL) {
            ret->text = button->titleLabel()->getText();
        }
        
        SubviewIterator it;
        for (it = m_subviews.begin(); it != m_subviews.end(); ++it) {
            assert((*it) != NULL);
            ret->subviews.push_back((*it)->viewTree());
        }
        
        return ret;
    }
    bool MIView::containedSubview(MIView *subview)
    {
        if (subview == NULL) {
            return false;
        }
        AUTO_LOCK_THIS;
        bool ret = false;
        SubviewIterator it;
        for (it = m_subviews.begin(); it != m_subviews.end(); ++it) {
            assert((*it) != NULL);
            if(*it == subview)
            {
                ret = true;
                break; //for
            }
        }
        return ret;
    }
    void MIView::includeNewSubview(MIView* subview)
    {
        
    }
    void MIView::excludeOldSubview(MIView* subview)
    {
        
    }
    
#pragma mark - view position & size & trasition setting -
//位置相关
    void MIView::setAniCenter(const MGPoint& center)
    {
        assert(m_animation_layer != NULL);
		m_animation_layer->setted_animate_center = false;
        m_animation_layer->ani_center = center;
    }
    void MIView::setAniFrame(const MGRect &frame)
    {
		assert(m_animation_layer != NULL);
		m_animation_layer->setted_animate_frame = false;
        m_animation_layer->ani_frame = frame;
    }
    void MIView::animateCenter(const MGPoint& center)
    {
        _setCenter(center);
    }
    void MIView::animateFrame(const MGRect &frame)
    {
		_setFrame(frame);
    }
    void MIView::setCenter(const MGPoint& center)
    {
        assert(m_animation_layer != NULL);
		animations_manager->lock();
        if (animations_manager->record_as_animation <= 0) {
            //不记录动画//
            if (!MIView::animations_manager->animation_acting) {
                m_animation_layer->animating_center = false;
            }
            animations_manager->unlock();
            _setCenter(center);
            m_animation_layer->copyFromView(this);
        }else{
            //正在记录为动画//
            m_animation_layer->src_center = m_center;
            m_animation_layer->dst_center = center;
            m_animation_layer->animating_center = true;
            string animation_name = animations_manager->topAnimationIndentityInStack();
            animations_manager->registerViewAnimation(animation_name, this);
            animations_manager->unlock();
		}
    }
    void MIView::setFrame(const MGRect &frame)
    {
		assert(m_animation_layer != NULL);
		animations_manager->lock();
		//MGLogD("animations_manager->record_as_animation =%d",animations_manager->record_as_animation);
        if (animations_manager->record_as_animation <= 0) {
            //不记录动画//
            if (!MIView::animations_manager->animation_acting) {
                m_animation_layer->animating_frame = false;
            }
            animations_manager->unlock();
            _setFrame(frame);
            m_animation_layer->copyFromView(this);
        }else{
            //正在记录为动画//
            m_animation_layer->src_frame = m_frame;
            m_animation_layer->dst_frame = frame;
            m_animation_layer->animating_frame = true;
            string animation_name = animations_manager->topAnimationIndentityInStack();
            animations_manager->registerViewAnimation(animation_name, this);
            animations_manager->unlock();
		}
    }
    void MIView::_setCenter(const MGPoint& center)
    {
        // 自适应的位置调整代码，位置要结合自适应设置和父View位置 //
        AUTO_LOCK_THIS;
        float d_x = center.x - m_center.x;
        float d_y = center.y - m_center.y;
        m_center = center;
        centerChanged(center);
        m_bounds = MGRectMake(m_bounds.x + d_x, m_bounds.y + d_y, m_bounds.width, m_bounds.height);
        if (m_superview != NULL) {
            m_frame.x = m_bounds.x - m_superview->m_bounds.x;
            m_frame.y = m_bounds.y - m_superview->m_bounds.y;
        }
        SubviewIterator it;
        for (it = m_subviews.begin(); it != m_subviews.end(); ++it) {
            assert((*it) != NULL);
            (*it)->_setCenter(MGPointMake((*it)->m_center.x + d_x, (*it)->m_center.y + d_y));
        }
    }
    void MIView::_setFrame(const MGRect &frame)
    {
        // 自适应的位置调整代码，位置要结合自适应设置和父View位置 //
        AUTO_LOCK_THIS;
        if (m_superview != NULL) {
            m_bounds.x = frame.x + m_superview->m_bounds.x;
            m_bounds.y = frame.y + m_superview->m_bounds.y;
        } else {
            m_bounds.x += frame.x - m_frame.x;
            m_bounds.y += frame.y - m_frame.y;
        }
        m_bounds.width = frame.width;
        m_bounds.height = frame.height;
        m_frame = frame;
        frameChanged(frame);
        m_center = MGPointMake(m_bounds.x + m_bounds.width * 0.5f, m_bounds.y + m_bounds.height * 0.5f);
        SubviewIterator it;
        for (it = m_subviews.begin(); it != m_subviews.end(); ++it) {
            assert((*it) != NULL);
            (*it)->_setFrame((*it)->m_frame);
        }
    }
    
    void MIView::setRectRotate(MGRectRotate _rotate)
    {
        assert(m_animation_layer != NULL);
		animations_manager->lock();
        if (animations_manager->record_as_animation <= 0) {
            //不记录动画//
            if (!MIView::animations_manager->animation_acting) {
                m_animation_layer->animating_rotate = false;
            }
            animations_manager->unlock();
            m_rotate = _rotate;
            if (m_rotate.axis_x >= 360)
            {
                m_rotate.axis_x -= 360;
            }
            if (m_rotate.axis_y >= 360)
            {
                m_rotate.axis_y -= 360;
            }
            if (m_rotate.axis_z >= 360)
            {
                m_rotate.axis_z -= 360;
            }
            rectRotateChanged(m_rotate);
            m_animation_layer->copyFromView(this);
        }else{
            //正在记录为动画//
            if (m_rotate.axis_x >= 360)
            {
                m_rotate.axis_x -= 360;
            }
            if (m_rotate.axis_y >= 360)
            {
                m_rotate.axis_y -= 360;
            }
            if (m_rotate.axis_z >= 360)
            {
                m_rotate.axis_z -= 360;
            }
            m_animation_layer->src_rotate = m_rotate;
            m_animation_layer->dst_rotate = _rotate;
            m_animation_layer->animating_rotate = true;
            string animation_name = animations_manager->topAnimationIndentityInStack();
            animations_manager->registerViewAnimation(animation_name, this);
            animations_manager->unlock();
		}
    }
    void MIView::setRectTransition(MGRectTransition _transition)
    {
		assert(m_animation_layer != NULL);
		animations_manager->lock();
        if (animations_manager->record_as_animation <= 0) {
            //不记录动画//
            if (!MIView::animations_manager->animation_acting) {
                m_animation_layer->animating_transition = false;
            }
            animations_manager->unlock();
            m_transition = _transition;
            rectTransitionChanged(_transition);
            m_animation_layer->copyFromView(this);
        }else{
            //正在记录为动画//
            m_animation_layer->src_transition = m_transition;
            m_animation_layer->dst_transition = _transition;
            m_animation_layer->animating_transition = true;
            string animation_name = animations_manager->topAnimationIndentityInStack();
            animations_manager->registerViewAnimation(animation_name, this);
            animations_manager->unlock();
		}
    }
    void MIView::setRectScale(MGRectScale _scale)
    {
        assert(m_animation_layer != NULL);
		animations_manager->lock();
        if (animations_manager->record_as_animation <= 0) {
            //不记录动画//
            if (!MIView::animations_manager->animation_acting) {
                m_animation_layer->animating_scale = false;
            }
            animations_manager->unlock();
            m_scale = _scale;
            rectScaleChanged(_scale);
            m_animation_layer->copyFromView(this);
        }else{
            //正在记录为动画//
            m_animation_layer->src_scale = m_scale;
            m_animation_layer->dst_scale = _scale;
            m_animation_layer->animating_scale = true;
            string animation_name = animations_manager->topAnimationIndentityInStack();
            animations_manager->registerViewAnimation(animation_name, this);
            animations_manager->unlock();
		}
    }
    void MIView::setSubRectRotate(MGRectRotate _rotate)
    {
		assert(m_animation_layer != NULL);
		animations_manager->lock();
        if (animations_manager->record_as_animation <= 0) {
            //不记录动画//
            if (!MIView::animations_manager->animation_acting) {
                m_animation_layer->animating_sub_rotate = false;
            }
            animations_manager->unlock();
            sub_rotate = _rotate;
            if (sub_rotate.axis_x >= 360)
            {
                sub_rotate.axis_x -= 360;
            }
            if (sub_rotate.axis_y >= 360)
            {
                sub_rotate.axis_y -= 360;
            }
            if (sub_rotate.axis_z >= 360)
            {
                sub_rotate.axis_z -= 360;
            }
            subRectRotateChanged(m_rotate);
            m_animation_layer->copyFromView(this);
        }else{
            //正在记录为动画//
            m_animation_layer->src_sub_rotate = sub_rotate;
            m_animation_layer->dst_sub_rotate = _rotate;
            m_animation_layer->animating_sub_rotate = true;
            string animation_name = animations_manager->topAnimationIndentityInStack();
            animations_manager->registerViewAnimation(animation_name, this);
            animations_manager->unlock();
		}
    }
    void MIView::setSubRectTransition(MGRectTransition _transition)
    {
		assert(m_animation_layer != NULL);
		animations_manager->lock();
        if (animations_manager->record_as_animation <= 0) {
            //不记录动画//
            if (!MIView::animations_manager->animation_acting) {
                m_animation_layer->animating_sub_transition = false;
            }
            animations_manager->unlock();
            sub_transition = _transition;
            subRectTransitionChanged(_transition);
            m_animation_layer->copyFromView(this);
        }else{
            //正在记录为动画//
            m_animation_layer->src_sub_transition = sub_transition;
            m_animation_layer->dst_sub_transition = _transition;
            m_animation_layer->animating_sub_transition = true;
            string animation_name = animations_manager->topAnimationIndentityInStack();
            animations_manager->registerViewAnimation(animation_name, this);
            animations_manager->unlock();
		}
    }
    void MIView::setSubRectScale(MGRectScale _scale)
    {
        assert(m_animation_layer != NULL);
		animations_manager->lock();
        if (animations_manager->record_as_animation <= 0) {
            //不记录动画//
            if (!MIView::animations_manager->animation_acting) {
                m_animation_layer->animating_sub_scale = false;
            }
            animations_manager->unlock();
            sub_scale = _scale;
            subRectScaleChanged(_scale);
            m_animation_layer->copyFromView(this);
        }else{
            //正在记录为动画//
            m_animation_layer->src_sub_scale = sub_scale;
            m_animation_layer->dst_sub_scale = _scale;
            m_animation_layer->animating_sub_scale = true;
            string animation_name = animations_manager->topAnimationIndentityInStack();
            animations_manager->registerViewAnimation(animation_name, this);
            animations_manager->unlock();
		}
    }
    
    void MIView::setRectRotatePoint(MGPoint point)
    {
        m_rotate_p = point;
    }
    void MIView::setRectScalePoint(MGPoint point)
    {
        m_scale_p = point;
    }
    
    void MIView::setSubRectRotatePoint(MGPoint point)
    {
        sub_rotate_p = point;
    }
    void MIView::setSubRectScalePoint(MGPoint point)
    {
        sub_scale_p = point;
    }
    
    void MIView::setAniRectRotate(MGRectRotate _rotate)
    {
        assert(m_animation_layer != NULL);
		m_animation_layer->setted_animate_rotate = false;
        m_animation_layer->ani_rotate = _rotate;
    }
    void MIView::setAniRectTransition(MGRectTransition _transition)
    {
		assert(m_animation_layer != NULL);
		m_animation_layer->setted_animate_transition = false;
        m_animation_layer->ani_transition = _transition;
    }
    void MIView::setAniRectScale(MGRectScale _scale)
    {
        assert(m_animation_layer != NULL);
		m_animation_layer->setted_animate_scale = false;
        m_animation_layer->ani_scale = _scale;
    }
    void MIView::setAniSubRectRotate(MGRectRotate _rotate)
    {
		assert(m_animation_layer != NULL);
		m_animation_layer->setted_animate_sub_rotate = false;
        m_animation_layer->ani_sub_rotate = _rotate;
    }
    void MIView::setAniSubRectTransition(MGRectTransition _transition)
    {
		assert(m_animation_layer != NULL);
		m_animation_layer->setted_animate_sub_transition = false;
        m_animation_layer->ani_sub_transition = _transition;
    }
    void MIView::setAniSubRectScale(MGRectScale _scale)
    {
        assert(m_animation_layer != NULL);
		m_animation_layer->setted_animate_sub_scale = false;
        m_animation_layer->ani_sub_scale = _scale;
    }
    
#pragma mark - position & size changed callback -
    void MIView::frameChanged(const MGRect &frame)
    {
        // Do anything you want here when frame changed.
    }
    void MIView::centerChanged(const MGPoint &center)
    {
        // Do anything you want here when center changed.
    }
    void MIView::rectTransitionChanged(const MGRectTransition& _transition)
    {
        // Do anything you want here when rect_transition changed.
    }
    void MIView::rectScaleChanged(const MGRectScale& _scale)
    {
        // Do anything you want here when rect_scale changed.
    }
    void MIView::rectRotateChanged(const MGRectRotate& _rotate)
    {
        // Do anything you want here when rect_rotate changed.
    }
    void MIView::subRectTransitionChanged(const MGRectTransition& _transition)
    {
        // Do anything you want here when sub_rect_transition changed.
    }
    void MIView::subRectScaleChanged(const MGRectScale& _scale)
    {
        // Do anything you want here when sub_rect_scale changed.
    }
    void MIView::subRectRotateChanged(const MGRectRotate& _rotate)
    {
        // Do anything you want here when sub_rect_rotate changed.
    }

#pragma mark - display property -
    void MIView::setAlpha(float _alpha)
    {
		assert(m_animation_layer != NULL);
		animations_manager->lock();
        if (animations_manager->record_as_animation <= 0) {
            //不记录动画//
            if (!MIView::animations_manager->animation_acting) {
                m_animation_layer->animating_alpha = false;
            }
            animations_manager->unlock();
            m_alpha = _alpha;
            m_animation_layer->copyFromView(this);
        }else{
            //正在记录为动画//
            m_animation_layer->src_alpha = m_alpha;
            m_animation_layer->dst_alpha = _alpha;
            m_animation_layer->animating_alpha = true;
            string animation_name = animations_manager->topAnimationIndentityInStack();
            animations_manager->registerViewAnimation(animation_name, this);
            animations_manager->unlock();
		}
    }
    
    void MIView::animateAlpha(float _alpha)
    {
		assert(m_animation_layer != NULL);
        m_alpha = _alpha;
    }
    
    void MIView::setAniAlpha(float _alpha)
    {
		assert(m_animation_layer != NULL);
		m_animation_layer->setted_animate_alpha = false;
        m_animation_layer->ani_alpha = _alpha;
        m_alpha = _alpha;
    }
    
    void MIView::setBackgroundColor( MGColor *_color )
	{
        AUTO_LOCK_THIS;
        if (m_background_color != NULL && m_background_color != _color) {
            delete m_background_color;
            m_background_color = NULL;
        }
        
		m_background_color = _color;
	}
    void MIView::setBackgroundAlphaInByte(byte_8 _alpha)
    {
        AUTO_LOCK_THIS;
        if (m_background_color != NULL) {
            m_background_color->m_color.m_alpha_component = _alpha;
        } else {
            m_background_color = MGColor::fromRGBA(255,255,255,_alpha);
        }
    }
    void MIView::setBackgroundRefInByte(z11::RGBAColorRef ref)
    {
        AUTO_LOCK_THIS;
        if (m_background_color != NULL) {
            m_background_color->m_color = ref;
        } else {
            m_background_color = MGColor::fromColorRef(ref);
        }
    }
    
    MGColor * MIView::getBackgroundColor()
	{
		return m_background_color;
	}
    
    void MIView::hide()
	{
		m_hidden=true;
	}
    
	void MIView::show()
	{
		m_hidden=false;
	}
    
	bool MIView::isHide()
	{
		return m_hidden;
	}
    
#pragma mark - display function -
//绘图相关 //
    void MIView::transitionDrawself(MGRectTransition& transition)
    {
        //transition before drawself.
        if (transition == MGRectTransitionMake(0, 0, 0)) {
            return;
        }
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        if (graphic_engine == NULL) {
            return;
        }
        graphic_engine->transition(transition);
    }
    void MIView::rotateDrawself(MGRectRotate& rotate)
    {
        //rotate before drawself.
        if (rotate == MGRectRotateMake(0, 0, 0, 0)) {
            return;
        }
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        if (graphic_engine == NULL) {
            return;
        }
        graphic_engine->rotate(MGPointMake(m_center.x + m_rotate_p.x, m_center.y + m_rotate_p.y),rotate);
    }
    void MIView::scaleDrawself(MGRectScale& scale)
    {
        //scale before drawself.
        if (scale == MGRectScaleMake(1, 1, 1)) {
            return;
        }
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        if (graphic_engine == NULL) {
            return;
        }
        graphic_engine->scale(MGPointMake(m_center.x + m_scale_p.x, m_center.y + m_scale_p.y),scale);
    }
    void MIView::subTransitionDrawself(MGRectTransition& transition)
    {
        //transition before drawself.
        if (transition == MGRectTransitionMake(0, 0, 0)) {
            return;
        }
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        if (graphic_engine == NULL) {
            return;
        }
        if (_view_flags.sub_transition_enable == 1) {
            graphic_engine->transition(transition);
        }
    }
    void MIView::subRotateDrawself(MGRectRotate& rotate)
    {
        //rotate before drawself.
        if (rotate == MGRectRotateMake(0, 0, 0, 0)) {
            return;
        }
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        if (graphic_engine == NULL) {
            return;
        }
        if (_view_flags.sub_rotate_enable == 1) {
            graphic_engine->rotate(MGPointMake(m_center.x + sub_rotate_p.x, m_center.y + sub_rotate_p.y), rotate);
        }
    }
    void MIView::subScaleDrawself(MGRectScale& scale)
    {
        //scale before drawself.
        if (scale == MGRectScaleMake(1, 1, 1)) {
            return;
        }
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        if (graphic_engine == NULL) {
            return;
        }
        if (_view_flags.sub_scale_enable == 1) {
            graphic_engine->scale(MGPointMake(m_center.x + sub_scale_p.x, m_center.y + sub_scale_p.y), scale);
        }
    }
    void MIView::drawself()
    {
        // redraw here!
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        if (graphic_engine == NULL) {
            return;
        }
        RGBAColorRef color_ref;
        if (m_background_color != NULL) {
            color_ref = m_background_color->m_color;
            color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
        }else{
            color_ref = RGBAColorMake(0, 0, 0, 0);//(byte_8)(m_alpha * 255));
        }
        if (m_background_color != NULL && m_background_color->m_image != NULL) {
            m_background_color->m_image->sendDataToContext();
            MGRect image_rect;
            image_rect.width=m_background_color->m_image->getWidth();
            image_rect.height=m_background_color->m_image->getHeight();
			//LogStart();
			graphic_engine->drawImage(m_background_color->m_image, m_bounds, color_ref, draw_image_mode,image_rect);
			//LogEnd("MIView drawImage");
        }else{
            switch (view_round_mode) {
                case RoundRectNone:
                {
                    graphic_engine->drawFilledRect(m_bounds, color_ref);
                    break;
                }
                default:
                {
                    graphic_engine->drawFilledRoundRect(m_bounds, color_ref, view_round_rate, RGBAColorMake(255,255,255,255), view_round_mode);
                }
                    break;
            }
        }
    }
    void MIView::drawBeforeSubviews()
    {
        // Do something else in your own wish.
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        assert(graphic_engine != NULL);
        MGRectTransition s_transition = getSubRectTransition();
        MGRectRotate s_rotate = getSubRectRotate();
        MGRectScale s_scale = getSubRectScale();
        m_need_sub_translate = needSubTranslateDraw(s_transition, s_rotate, s_scale);
        if (m_need_sub_translate) {
            graphic_engine->beginDraw();
        }
        subTransitionDrawself(s_transition);
        subRotateDrawself(s_rotate);
        subScaleDrawself(s_scale);
    }
    void MIView::drawAfterSubviews()
    {
        // Do something else in your own wish.
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        assert(graphic_engine != NULL);
        if (m_need_sub_translate) {
            graphic_engine->endDraw();
        }
    }
    
    bool MIView::needSubTranslateDraw(MGRectTransition& transition, MGRectRotate& rotate, MGRectScale& scale)
    {
        bool ret = false;
        if (transition != MGRectTransitionMake(0, 0, 0)) {
            return true;
        }
        if (rotate != MGRectRotateMake(0, 0, 0, 0)) {
            return true;
        }
        if (scale != MGRectScaleMake(1, 1, 1)) {
            return true;
        }
        return ret;
    }
    
    bool MIView::needTranslateDraw(MGRectTransition& transition, MGRectRotate& rotate, MGRectScale& scale)
    {
        bool ret = false;
        if (transition != MGRectTransitionMake(0, 0, 0)) {
            return true;
        }
        if (rotate != MGRectRotateMake(0, 0, 0, 0)) {
            return true;
        }
        if (scale != MGRectScaleMake(1, 1, 1)) {
            return true;
        }
        return ret;
    }
    
    void MIView::draw()
    {
        /*父窗口不可见，则全部子窗口都不重绘 */
		if(m_hidden == true || m_alpha < 0.1)
        {
            return;
        }
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        if (graphic_engine == NULL) {
            return;
        }
        AUTO_LOCK_THIS;
        // 对于没有进行坐标变换的View就不进行矩阵堆栈的操作了，否则OpenGLES的矩阵堆栈很容易溢出，这样可以保证子View自己的变换可以顺利进行，但是由于矩阵堆栈深度有限，所以变换嵌套的深度也是有限的，目前无解 //
        MGRectTransition transition = getRectTransition();
        MGRectRotate rotate = getRectRotate();
        MGRectScale scale = getRectScale();
        bool need_translate = needTranslateDraw(transition, rotate, scale);
        if (need_translate) {
            graphic_engine->beginDraw();
        }
        transitionDrawself(transition);
        rotateDrawself(rotate);
        scaleDrawself(scale);
        drawself();
        byte_8 clip_subviews = _view_flags.clip_subviews;
        if (clip_subviews != 0) {
			MGRect rect = m_bounds;
            // 利用函数完成裁剪区域的计算，裁剪区域只根据位移和缩放的参数进行计算，不计算旋转造成的影响 //
            // 实际旋转造成的影响也在计算，但是没有测试OpenGLES是否支持扭曲矩形裁剪，因此如果用到这个裁剪还是不要使用旋转的特性了 //
            // OpenGLES无法直接支持3D裁剪，因此涉及到裁剪的操作，只能是视图平面裁剪 //
            transAndScaleClipRect(rect);
            //MGLogD("%p:%s clip rect : (%f, %f, %f, %f).", this, class_name.className().c_str(), rect.x, rect.y, rect.width, rect.height);
            graphic_engine->enableClip(rect);
        }
        drawBeforeSubviews();
        SubviewIterator it;
        for (it = m_subviews.begin(); it != m_subviews.end(); ++it) {
            assert((*it) != NULL);
            (*it)->setAnimationAttributes();
            float tmp_alpha = (*it)->m_alpha;
            (*it)->m_alpha = (*it)->m_alpha * m_alpha;
            (*it)->draw();
            (*it)->m_alpha = tmp_alpha;
        }
        drawAfterSubviews();
        if (clip_subviews != 0) {
            graphic_engine->disableClip();
        }
        if (need_translate) {
            graphic_engine->endDraw();
        }
    }
    void MIView::drawRect(MGRect rect)
    {

    }
    
    void MIView::transAndScaleClipRect(MGRect &rect)
    {
        MGPoint lt = MGPointMake(rect.x, rect.y);
        do {
            MIView* view = this;
            deque<MIView*> view_tree;
            while (view != NULL) {
                view_tree.push_front(view);
                view = view->parent();
            }
            view = view_tree.back();
            view->detranslatePointWithoutRotate(lt);
            view_tree.pop_back();
            while (view_tree.size() > 0) {
                view = view_tree.back();
                view->detranslateSubPointWithoutRotate(lt);
                view->detranslatePointWithoutRotate(lt);
                view_tree.pop_back();
            }
        } while (0);
        
        MGPoint br = MGPointMake(rect.x + rect.width, rect.y + rect.height);
        do {
            MIView* view = this;
            deque<MIView*> view_tree;
            while (view != NULL) {
                view_tree.push_front(view);
                view = view->parent();
            }
            view = view_tree.back();
            view->detranslatePointWithoutRotate(br);
            view_tree.pop_back();
            while (view_tree.size() > 0) {
                view = view_tree.back();
                view->detranslateSubPointWithoutRotate(br);
                view->detranslatePointWithoutRotate(br);
                view_tree.pop_back();
            }
        } while (0);
        
        rect.x = lt.x;
        rect.y = lt.y;
        rect.width = br.x - lt.x;
        rect.height = br.y - lt.y;
    }

#pragma mark - hit detect -
//触摸事件相关//
    MIView* MIView::hitDetect(MITouch* touch, MIEvent *event, MITouchEventType& type)
    {
        AUTO_LOCK_THIS;
        MGPoint& point = touch->point;
        MIView* ret = NULL;
        if (m_scale.axis_x == 0 || m_scale.axis_y == 0) {
            return ret;
        }
        if (this->m_hidden == true || this->m_alpha < 0.1 || _view_flags.user_interaction_enabled == 0) {
            return ret;
        }
        if (_view_flags.user_multitouch_enabled == 0 && touch_manager.size() >= 1) {
            map<MITouchTag, MITouch>::iterator it = touch_manager.begin();
            if (it->first != touch->tag) {
                return ret;
            }
        }
        MGPoint current_point = point;
        translatePoint(current_point);
        if (pointInside(current_point, event)) {
            if (_view_flags.subview_enabled == 0) {
                if (can_handle_touch) {
                    ret = this;
                    touch_hitted = true;
                }
                touch->point = current_point;
                return ret;
            }
            point = current_point;
            translateSubPoint(point);
            SubviewReverseIterator it;
            for (it = m_subviews.rbegin(); it != m_subviews.rend(); ++it) {
                assert((*it) != NULL);
                if(*it != NULL)
                {
                    (*it)->can_handle_touch_from_superview = can_handle_touch_from_superview;
                    if (_view_flags.user_multitouch_subview_enabled == 0 && m_have_touches_through_subview) {
                        (*it)->can_handle_touch_from_superview = (*it)->m_have_touches_through_subview || (*it)->touch_manager.size() > 0 || (*it)->touch_hitted;
                    }
                    (*it)->can_handle_touch = (*it)->can_handle_touch_from_superview && ((*it)->touch_manager.size() > 0 || (*it)->touch_hitted || !m_have_touches_through_subview || _view_flags.user_multitouch_subview_enabled == 1);
                    ret = (*it)->hitDetect(touch, event, type);
                    if (ret != NULL) {
                        break;
                    }
                }
            }
            if (ret == NULL) {
                if (can_handle_touch) {
                    ret = this;
                    touch_hitted = true;
                }
                touch->point = current_point;
            }
        }
        return ret;
    }
    bool MIView::checkTouchesInSubview()
    {
        AUTO_LOCK_THIS;
        m_have_touches_through_subview = false;
        bool ret = false;
        if (m_scale.axis_x == 0 || m_scale.axis_y == 0) {
            return ret;
        }
        if (this->m_hidden == true || this->m_alpha < 0.1 || _view_flags.user_interaction_enabled == 0) {
            return ret;
        }
        SubviewReverseIterator it;
        for (it = m_subviews.rbegin(); it != m_subviews.rend(); ++it) {
            assert((*it) != NULL);
            if(*it != NULL && (*it)->checkTouchesInSubview())
            {
                m_have_touches_through_subview = true;
            }
        }
        ret = m_have_touches_through_subview;
        if (!ret) {
            uint_32 size = this->touch_manager.size();
            if (size > 0 || touch_hitted)
            {
                ret = true;
            }
        }
        return ret;
    }
    
    void MIView::locationInScreen(MGPoint &point, z11::MIView *view)
    {
        do {
            if (view == NULL) {
                break;
            }
            deque<MIView*> view_tree;
            while (view != NULL) {
                view_tree.push_front(view);
                view = view->parent();
            }
            view = view_tree.back();
            view_tree.pop_back();
            view->detranslatePoint(point);
            while (view_tree.size() > 0) {
                view = view_tree.back();
                view_tree.pop_back();
                view->detranslateSubPoint(point);
                view->detranslatePoint(point);
            }
        } while (0);
    }
    void MIView::translatePoint(MGPoint& point)
    {
        if (m_transition != MGRectTransitionMake(0, 0, 0)) {
            transitionPoint(point);
        }
        if (m_rotate != MGRectRotateMake(0, 0, 0, 0)) {
            rotatePoint(point, MGPointMake(m_bounds.x + m_bounds.width / 2 + m_rotate_p.x, m_bounds.y + m_bounds.height / 2 + m_rotate_p.y));
        }
        if (m_scale != MGRectScaleMake(1, 1, 1)) {
            scalePoint(point, MGPointMake(m_bounds.x + m_bounds.width / 2 + m_scale_p.x, m_bounds.y + m_bounds.height / 2 + m_scale_p.y));
        }
    }
    void MIView::detranslatePoint(MGPoint& point)
    {
        if (m_scale != MGRectScaleMake(1, 1, 1)) {
            descalePoint(point, MGPointMake(m_bounds.x + m_bounds.width / 2 + m_scale_p.x, m_bounds.y + m_bounds.height / 2 + m_scale_p.y));
        }
        if (m_rotate != MGRectRotateMake(0, 0, 0, 0)) {
            derotatePoint(point, MGPointMake(m_bounds.x + m_bounds.width / 2 + m_rotate_p.x, m_bounds.y + m_bounds.height / 2 + m_rotate_p.y));
        }
        if (m_transition != MGRectTransitionMake(0, 0, 0)) {
            detransitionPoint(point);
        }
    }
	void MIView::detranslatePointWithoutRotate(MGPoint& point)
	{
		if (m_scale != MGRectScaleMake(1, 1, 1)) {
			descalePoint(point, MGPointMake(m_bounds.x + m_bounds.width / 2 + m_scale_p.x, m_bounds.y + m_bounds.height / 2 + m_scale_p.y));
		}
		if (m_transition != MGRectTransitionMake(0, 0, 0)) {
			detransitionPoint(point);
		}
	}
    bool MIView::pointInside(const MGPoint& point,const MIEvent* const event)
    {
        bool ret = true;
        if (point.x < m_bounds.x || point.y < m_bounds.y ||
            point.x > m_bounds.width + m_bounds.x ||
            point.y > m_bounds.height + m_bounds.y) {
            ret = false;
        }
        return ret;
    }
    void MIView::transitionPoint(MGPoint &current_point)
    {
        current_point.x -= m_transition.axis_x;
        current_point.y -= m_transition.axis_y;
    }
    // 暂时无法实现从屏幕坐标来hitDetect空间View的功能，因为需要判断和三维空间的那个View相交，并且要求获得相交点的Z坐标后，才可能实现hitDetect的反算，因此目前只支持Z轴的旋转后还能操作，其他旋转后操作会出现异常，因此其他旋转只用于效果显示，不能用于交互设计 //
    void MIView::rotatePoint(MGPoint &current_point, const MGPoint &ref_point)
    {
        if (m_rotate == MGRectRotateMake(0.0f, 0.0f, 0.0f, 0.0f)) {
            return;
        }
        float base_angle = -m_rotate.angle;
        if (base_angle == 0.0f || base_angle == 360.0)
            return;

        float l = sqrt(pow(current_point.x - ref_point.x , 2) + pow(current_point.y - ref_point.y, 2));
        if (l != 0.0f)
        {
            base_angle = 360 - base_angle;
            float b = acos((ref_point.x - current_point.x) / l);
            float m = 2 * l * sin(base_angle * M_PI / 360);
            float beta = b + (base_angle * M_PI / 360);
            if (current_point.y < ref_point.y) {
                current_point.x = current_point.x + m * sin(beta);
                current_point.y = current_point.y - m * cos(beta);
            } else {
                beta = b - (float)(base_angle * M_PI / 360);
                current_point.x = current_point.x - m * sin(beta);
                current_point.y = current_point.y - m * cos(beta);
            }
        }
    }
    void MIView::scalePoint(MGPoint &current_point, const MGPoint &ref_point)
    {
        if (m_scale.axis_x != 0.0) {
            current_point.x = (current_point.x - ref_point.x) / m_scale.axis_x + ref_point.x;
        }
        if (m_scale.axis_y != 0.0) {
            current_point.y = (current_point.y - ref_point.y) / m_scale.axis_y + ref_point.y;
        }
    }
    void MIView::detransitionPoint(MGPoint &current_point)
    {
        current_point.x += m_transition.axis_x;
        current_point.y += m_transition.axis_y;
    }
    void MIView::derotatePoint(MGPoint &current_point, const MGPoint &ref_point)
    {
        if (m_rotate == MGRectRotateMake(0.0f, 0.0f, 0.0f, 0.0f)) {
            return;
        }
        float base_angle = -m_rotate.angle;
        if (base_angle == 0.0f || base_angle == 360.0)
            return;

        float l = sqrt(pow(current_point.x - ref_point.x , 2) + pow(current_point.y - ref_point.y, 2));
        if (l != 0.0f)
        {
            float b = acos((ref_point.x - current_point.x) / l);
            float m = 2 * l * sin(base_angle * M_PI / 360);
            float beta = b + (base_angle * M_PI / 360);
            if (current_point.y < ref_point.y) {
                current_point.x = current_point.x + m * sin(beta);
                current_point.y = current_point.y - m * cos(beta);
            } else {
                beta = b - (float)(base_angle * M_PI / 360);
                current_point.x = current_point.x - m * sin(beta);
                current_point.y = current_point.y - m * cos(beta);
            }
        }
    }
    void MIView::descalePoint(MGPoint &current_point, const MGPoint &ref_point)
    {
        if (m_scale.axis_x != 0.0) {
            current_point.x = (current_point.x - ref_point.x) * m_scale.axis_x + ref_point.x;
        }
        if (m_scale.axis_y != 0.0) {
            current_point.y = (current_point.y - ref_point.y) * m_scale.axis_y + ref_point.y;
        }
    }
    
    void MIView::translateSubPoint(MGPoint& point)
    {
        if (sub_transition != MGRectTransitionMake(0, 0, 0)) {
            transitionSubPoint(point);
        }
        if (sub_rotate != MGRectRotateMake(0, 0, 0, 0)) {
            rotateSubPoint(point, MGPointMake(m_center.x + sub_rotate_p.x, m_center.y + sub_rotate_p.y));
        }
        if (sub_scale != MGRectScaleMake(1, 1, 1)) {
            scaleSubPoint(point, MGPointMake(m_center.x + sub_scale_p.x, m_center.y + sub_scale_p.y));
        }
    }
    void MIView::detranslateSubPoint(MGPoint& point)
    {
        if (sub_scale != MGRectScaleMake(1, 1, 1)) {
            descaleSubPoint(point, MGPointMake(m_center.x + sub_scale_p.x, m_center.y + sub_scale_p.y));
        }
        if (sub_rotate != MGRectRotateMake(0, 0, 0, 0)) {
            derotateSubPoint(point, MGPointMake(m_center.x + sub_rotate_p.x, m_center.y + sub_rotate_p.y));
        }
        if (sub_transition != MGRectTransitionMake(0, 0, 0)) {
            detransitionSubPoint(point);
        }
    }
	void MIView::detranslateSubPointWithoutRotate(MGPoint& point)
	{
		if (sub_scale != MGRectScaleMake(1, 1, 1)) {
			descaleSubPoint(point, MGPointMake(m_center.x + sub_scale_p.x, m_center.y + sub_scale_p.y));
		}
		if (sub_transition != MGRectTransitionMake(0, 0, 0)) {
			detransitionSubPoint(point);
		}
	}
    void MIView::transitionSubPoint(MGPoint &current_point)
    {
        current_point.x -= sub_transition.axis_x;
        current_point.y -= sub_transition.axis_y;
    }
    void MIView::rotateSubPoint(MGPoint &current_point, const MGPoint &ref_point)
    {
        if (sub_rotate == MGRectRotateMake(0.0f, 0.0f, 0.0f, 0.0f)) {
            return;
        }
        float base_angle = -sub_rotate.angle;
        if (base_angle == 0.0f || base_angle == 360.0)
            return;
        
        float l = sqrt(pow(current_point.x - ref_point.x , 2) + pow(current_point.y - ref_point.y, 2));
        if (l != 0.0f)
        {
            base_angle = 360 - base_angle;
            float b = acos((ref_point.x - current_point.x) / l);
            float m = 2 * l * sin(base_angle * M_PI / 360);
            float beta = b + (base_angle * M_PI / 360);
            if (current_point.y < ref_point.y) {
                current_point.x = current_point.x + m * sin(beta);
                current_point.y = current_point.y - m * cos(beta);
            } else {
                beta = b - (float)(base_angle * M_PI / 360);
                current_point.x = current_point.x - m * sin(beta);
                current_point.y = current_point.y - m * cos(beta);
            }
        }
    }
    void MIView::scaleSubPoint(MGPoint &current_point, const MGPoint &ref_point)
    {
        if (sub_scale.axis_x != 0.0) {
            current_point.x = (current_point.x - ref_point.x) / sub_scale.axis_x + ref_point.x;
        }
        if (sub_scale.axis_y != 0.0) {
            current_point.y = (current_point.y - ref_point.y) / sub_scale.axis_y + ref_point.y;
        }
    }
    void MIView::detransitionSubPoint(MGPoint &current_point)
    {
        current_point.x += sub_transition.axis_x;
        current_point.y += sub_transition.axis_y;
    }
    void MIView::derotateSubPoint(MGPoint &current_point, const MGPoint &ref_point)
    {
        if (sub_rotate == MGRectRotateMake(0.0f, 0.0f, 0.0f, 0.0f)) {
            return;
        }
        float base_angle = -sub_rotate.angle;
        if (base_angle == 0.0f || base_angle == 360.0)
            return;
        
        float l = sqrt(pow(current_point.x - ref_point.x , 2) + pow(current_point.y - ref_point.y, 2));
        if (l != 0.0f)
        {
            float b = acos((ref_point.x - current_point.x) / l);
            float m = 2 * l * sin(base_angle * M_PI / 360);
            float beta = b + (base_angle * M_PI / 360);
            if (current_point.y < ref_point.y) {
                current_point.x = current_point.x + m * sin(beta);
                current_point.y = current_point.y - m * cos(beta);
            } else {
                beta = b - (float)(base_angle * M_PI / 360);
                current_point.x = current_point.x - m * sin(beta);
                current_point.y = current_point.y - m * cos(beta);
            }
        }
    }
    void MIView::descaleSubPoint(MGPoint &current_point, const MGPoint &ref_point)
    {
        if (sub_scale.axis_x != 0.0) {
            current_point.x = (current_point.x - ref_point.x) * sub_scale.axis_x + ref_point.x;
        }
        if (sub_scale.axis_y != 0.0) {
            current_point.y = (current_point.y - ref_point.y) * sub_scale.axis_y + ref_point.y;
        }
    }
    
    bool MIView::pointInView( const MGPoint &point )
	{
		MGPoint pt=point;
		transitionPoint(pt);
		return pointInside(pt,NULL);
        
	}
    
#pragma mark - gesture recognizer -
    
	void MIView::removeGestureRecognizer( MIGestureRecognizer *gestureRecognizer )
	{
		GestureRecognizerIterator it;
        for (it = m_gesture_recognizers.begin(); it != m_gesture_recognizers.end(); ++it) {
            if (*it == gestureRecognizer) {
				(*it)->clearRequirementsAndDepends();
                m_gesture_recognizers.erase(it);
                break;
            }
        }
	}
    
    void MIView::removeAndDeleteAllGestureRecognizer()
    {
        for (GestureRecognizerIterator it = m_gesture_recognizers.begin(); it != m_gesture_recognizers.end(); ++it) {
            (*it)->clearRequirementsAndDepends();
            m_gesture_recognizers.erase(it);
            delete_and_set_null(*it);
        }
    }
    
	void MIView::addGestureRecognizer( MIGestureRecognizer *gestureRecognizer )
	{
        assert(gestureRecognizer != NULL);
        gestureRecognizer->m_view = this;
		m_gesture_recognizers.push_back(gestureRecognizer);
	}
    
    void MIView::robTouchesAndEvent(const std::set<MITouch *> &touches, const z11::MIEvent *const event)
    {
        MIEvent* c_event = new MIEvent();
        MIEvent* b_event = new MIEvent();
        c_event->_event_type = CONTROL_PURE_TOUCHES_EVENT;
        b_event->_event_type = CONTROL_PURE_TOUCHES_EVENT;
        for (std::set<MITouch*>::const_iterator it = touches.begin(); it != touches.end(); ++it) {
            
            MITouch* touch = new MITouch(*it);
            touch->phase = MITouchPhaseCancelled;
            c_event->touches_in_event.insert(touch);
            //============================================//
            touch = new MITouch(*it);
            touch->phase = MITouchPhaseBegan;
            touch->maintained_view = this;
            b_event->touches_in_event.insert(touch);
        }
        GuiContainer::container->preserveEvent(c_event);
        GuiContainer::container->preserveEvent(b_event);
    }
    
    //手势识别触摸事件的响应============================================//
    MIEventHandleState FrameworkImp MIView::view_touchesBegan(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        GestureRecognizerIterator it;
        for (it = m_gesture_recognizers.begin(); it != m_gesture_recognizers.end(); ++it)
        {
            (*it)->touchesBegan(touches, event);
        }
        return _touchesBegan(touches, event);
    }
    MIEventHandleState FrameworkImp MIView::view_touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        GestureRecognizerIterator it;
        for (it = m_gesture_recognizers.begin(); it != m_gesture_recognizers.end(); ++it)
        {
            (*it)->touchesMoved(touches, event);
        }
        return _touchesMoved(touches, event);
    }
    MIEventHandleState FrameworkImp MIView::view_touchesEnded(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        GestureRecognizerIterator it;
        for (it = m_gesture_recognizers.begin(); it != m_gesture_recognizers.end(); ++it)
        {
            (*it)->touchesEnded(touches, event);
        }
        return _touchesEnded(touches, event);
    }
    MIEventHandleState FrameworkImp MIView::view_touchesCancelled(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        GestureRecognizerIterator it;
        for (it = m_gesture_recognizers.begin(); it != m_gesture_recognizers.end(); ++it)
        {
            (*it)->touchesCancelled(touches, event);
        }
        return _touchesCancelled(touches, event);
    }
    
#pragma mark - event handler -
    //管理响应链  //
    void MIView::handleTouchesPassEvent(std::set<MITouch*> touches)
    {
        std::set<MITouch*>::iterator it;
        for (it = touches.begin(); it != touches.end(); ++it) {
            detranslatePoint((*it)->point);
        }
    }
    
    void MIView::handleSubTouchesPassEvent(std::set<MITouch*> touches)
    {
        std::set<MITouch*>::iterator it;
        for (it = touches.begin(); it != touches.end(); ++it) {
            detranslateSubPoint((*it)->point);
        }
    }

    //返回响应链的下一个响应者  //
    MIResponder* MIView::nextResponder()
    {
        if (delegate != NULL) {
            return delegate;
        }
        if (m_superview != NULL) {
            return m_superview;
        }
        return NULL;
    }
    bool MIView::isFirstResponder()
    {
        return MIResponder::isFirstResponder();
    }
    //是否能够被设置为第一响应者 return false; in default
    bool MIView::canBecomeFirstResponder()
    {
        return false;
    }
    void MIView::becomeFirstResponder()
    {
        MIResponder::becomeFirstResponder();
    }
    //是否能够被取消当前对象为第一响应者 return true; in default
    bool MIView::canResignFirstResponder()
    {
        return true;
    }
    //取消当前对象为第一响应者  //
    void MIView::resignFirstResponder(bool closeKB)
    {
        MIResponder::resignFirstResponder(closeKB);
    }
    //事件响应
    MIEventHandleState MIView::handleEvent(const MIEvent* const event)
    {
        return MIEventHandleStatePass;
    }
    
#pragma mark - Touches handle -
    //触摸事件的响应  //
    MIEventHandleState MIView::touchesBegan(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
		MIEventHandleState ret = MIEventHandleStatePass;
        return ret;
    }
    MIEventHandleState MIView::touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		MIEventHandleState ret = MIEventHandleStateDone;
        if ((*touches.begin())->maintained_view != this) {
            return MIEventHandleStatePass;
        }
        // 触摸移动在上传递过程中，如果需要Rob，也得是MIView检测后认为是明显移动后才能被上级View Rob，不能一个细微移动就触发上层View的Rob行为 //
        for (std::set<MITouch*>::const_iterator it = touches.begin(); it != touches.end(); ++it) {
            MITouch* touch = *it;
            TouchMIt t_it = touch_manager.find(touch->tag);
            if(t_it != touch_manager.end())
            {
                float dis = distanceOfTwoPoint(t_it->second.locationInView(this), touch->locationInView(this));
                if(dis > significant_move_valve_rate * (MGDevice::getScreenWidth() > 1 ? MGDevice::getScreenWidth() : 640.0f))
                {
                    ret = MIEventHandleStatePass;
                    break;
                }
            }
        }
        return ret;
    }
    MIEventHandleState MIView::touchesEnded(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		MIEventHandleState ret = MIEventHandleStatePass;
        return ret;
    }
    MIEventHandleState MIView::touchesCancelled(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		MIEventHandleState ret = MIEventHandleStatePass;
		return ret;
    }

#pragma mark - animation -
    
    void MIView::animateRectRotate(MGRectRotate _rotate)
    {
        m_rotate = _rotate;
        if (m_rotate.axis_x >= 360)
        {
            m_rotate.axis_x -= 360;
        }
        if (m_rotate.axis_y >= 360)
        {
            m_rotate.axis_y -= 360;
        }
        if (m_rotate.axis_z >= 360)
        {
            m_rotate.axis_z -= 360;
        }
       rectRotateChanged(m_rotate);
    }
    void MIView::animateRectTransition(MGRectTransition _transition)
    {
		m_transition = _transition;
        rectTransitionChanged(_transition);
    }
    void MIView::animateRectScale(MGRectScale _scale)
    {
        m_scale = _scale;
        rectScaleChanged(_scale);
    }
    void MIView::animateSubRectRotate(MGRectRotate _rotate)
    {
		sub_rotate = _rotate;
        if (sub_rotate.axis_x >= 360)
        {
            sub_rotate.axis_x -= 360;
        }
        if (sub_rotate.axis_y >= 360)
        {
            sub_rotate.axis_y -= 360;
        }
        if (sub_rotate.axis_z >= 360)
        {
            sub_rotate.axis_z -= 360;
        }
        subRectRotateChanged(m_rotate);
    }
    void MIView::animateSubRectTransition(MGRectTransition _transition)
    {
		sub_transition = _transition;
        subRectTransitionChanged(_transition);
    }
    void MIView::animateSubRectScale(MGRectScale _scale)
    {
        sub_scale = _scale;
        subRectScaleChanged(_scale);
    }
    
    void MIView::setAnimationAttributes()
    {
        AUTO_LOCK_THIS;
        assert(m_animation_layer != NULL);
        if (!m_animation_layer->setted_animate_alpha) {
            animateAlpha(m_animation_layer->ani_alpha);
            m_animation_layer->setted_animate_alpha = true;
        }
        if (!m_animation_layer->setted_animate_center) {
            animateCenter(m_animation_layer->ani_center);
            m_animation_layer->setted_animate_center = true;
        }
        if (!m_animation_layer->setted_animate_frame) {
            animateFrame(m_animation_layer->ani_frame);
            m_animation_layer->setted_animate_frame = true;
        }
        if (!m_animation_layer->setted_animate_rotate) {
            animateRectRotate(m_animation_layer->ani_rotate);
            m_animation_layer->setted_animate_rotate = true;
        }
        if (!m_animation_layer->setted_animate_transition) {
            animateRectTransition(m_animation_layer->ani_transition);
            m_animation_layer->setted_animate_transition = true;
        }
        if (!m_animation_layer->setted_animate_scale) {
            animateRectScale(m_animation_layer->ani_scale);
            m_animation_layer->setted_animate_scale = true;
        }
        if (!m_animation_layer->setted_animate_sub_rotate) {
            animateSubRectRotate(m_animation_layer->ani_sub_rotate);
            m_animation_layer->setted_animate_sub_rotate = true;
        }
        if (!m_animation_layer->setted_animate_sub_transition) {
            animateSubRectTransition(m_animation_layer->ani_sub_transition);
            m_animation_layer->setted_animate_sub_transition = true;
        }
        if (!m_animation_layer->setted_animate_sub_scale) {
            animateSubRectScale(m_animation_layer->ani_sub_scale);
            m_animation_layer->setted_animate_sub_scale = true;
        }
    }
    void MIView::initAnimation()
    {
        m_alpha = m_animation_layer->src_alpha;
        m_frame = m_animation_layer->src_frame;
        m_center = m_animation_layer->src_center;
        m_transition = m_animation_layer->src_transition;
        m_rotate = m_animation_layer->src_rotate;
        m_scale = m_animation_layer->src_scale;
        sub_transition = m_animation_layer->src_sub_transition;
        sub_rotate = m_animation_layer->src_sub_rotate;
        sub_scale = m_animation_layer->src_sub_scale;
    }
//动画相关//
    void MIView::beginAnimation(const string& animation_name)
    {
        assert(animations_manager != NULL);
        animations_manager->pushAnimationIndentityToStack(animation_name);
    }
    void MIView::setAnimationCurve(MIViewAnimationOptions option)
	{
		animations_manager->lock();
		if (animations_manager->record_as_animation > 0) {
			// 根据设置的curve来配置MIAnimation中的动画曲线类型，两个要匹配，全部为框架自实现的内置类型 //
			string name = animations_manager->topAnimationIndentityInStack();
			MIAnimation* animation = animations_manager->animationWithName(name);
			assert(animation != NULL);
			if (animation->animation_algorithm != NULL) {
				animation->animation_algorithm->animation_curve = (uint_16)option;
			}
		}
		animations_manager->unlock();
    }
    void MIView::setAnimationAlgorithm(MIAnimationAlgorithm *algorithm)
	{
		animations_manager->lock();
		if (animations_manager->record_as_animation > 0) {
			string name = animations_manager->topAnimationIndentityInStack();
			MIAnimation* animation = animations_manager->animationWithName(name);
			animation->setAlgorithm(algorithm);
		}
		animations_manager->unlock();
    }
    void MIView::setAnimationDuration(float seconds)
	{
		animations_manager->lock();
		if (animations_manager->record_as_animation > 0) {
			string name = animations_manager->topAnimationIndentityInStack();
			MIAnimation* animation = animations_manager->animationWithName(name);
			animation->animation_duration = seconds;
			float fps = LightGraphic::getSettingFps();
			animation->all_step = (int_16)(seconds * fps);
            if (animation->all_step < 1) {
                animation->all_step = 1;
            }
			animation->step = animation->all_step;
		}
		animations_manager->unlock();
    }
    void MIView::setAnimationRepeatCount(float repeat_count)
	{
		animations_manager->lock();
		if (animations_manager->record_as_animation > 0) {
			string name = animations_manager->topAnimationIndentityInStack();
			MIAnimation* animation = animations_manager->animationWithName(name);
			animation->repeat_count = repeat_count;
		}
		animations_manager->unlock();
    }
    void MIView::setAnimationWillStartSelector(MIResponder* responder, SELHandler selector)
	{
		animations_manager->lock();
		if (animations_manager->record_as_animation > 0) {
			string name = animations_manager->topAnimationIndentityInStack();
			MIAnimation* animation = animations_manager->animationWithName(name);
			animation->will_start_responder = responder;
			animation->will_start_handler = selector;
		}
		animations_manager->unlock();
    }
    void MIView::setAnimationDidStopSelector(MIResponder* responder, SELHandler selector)
	{
		animations_manager->lock();
		if (animations_manager->record_as_animation > 0) {
			string name = animations_manager->topAnimationIndentityInStack();
			MIAnimation* animation = animations_manager->animationWithName(name);
			animation->did_stop_responder = responder;
			animation->did_stop_handler = selector;
		}
		animations_manager->unlock();
    }
    void MIView::commitAnimation()
    {
        assert(animations_manager != NULL);
        animations_manager->popupAnimationIndentityFromStack();
    }
    void MIView::stopAnimation(const string &animation_name)
    {
        animations_manager->unregisterViewAnimation(animation_name);
    }
    void MIView::waitNeedActionAnimations()
    {
        while(animations_manager->waitActionAnimations())
        {
            MGSleep(50);
        }
    }
    
#pragma mark - view trasition -
    //页面切换 //
    void MIView::transitionViews(MIView* from_view, MIView* to_view, float duration, MIViewAnimationOptions options, MIResponderSEL will_start_animation,MIResponderSEL did_stop_animation)
    {
        assert(from_view != NULL);
        assert(to_view != NULL);
        if (from_view == NULL || to_view == NULL) {
            return;
        }
        switch (options) {
            case MIViewAnimationOptionTransitionFlipFromRight:
            {
                MIView* super_view = from_view->parent();
                super_view->addSubview(to_view);
                to_view->setRectTransition(MGRectTransitionMake(to_view->getFrame().width, 0, 0));
                char pointer[128];
                sprintf(pointer, "%p", super_view);
                string str_pointer = pointer;
                MIView::beginAnimation("Transition Views for Foundation Call Only." + str_pointer);
                MIView::setAnimationCurve(options);
                MIView::setAnimationDuration(duration);
                from_view->setRectTransition(MGRectTransitionMake(0 - from_view->getFrame().width, 0, 0));
                to_view->setRectTransition(MGRectTransitionMake(0, 0, 0));
                if(!will_start_animation.isNull())
                    MIView::setAnimationWillStartSelector(will_start_animation.responder,will_start_animation.action);
                if(!did_stop_animation.isNull())
                    MIView::setAnimationDidStopSelector(did_stop_animation.responder,did_stop_animation.action);
                MIView::commitAnimation();
                break;
            }
            case MIViewAnimationOptionTransitionFlipFromLeft:
            {
                MIView* super_view = from_view->parent();
                super_view->addSubview(to_view);
                to_view->setRectTransition(MGRectTransitionMake(0 - to_view->getFrame().width, 0, 0));
                char pointer[128];
                sprintf(pointer, "%p", super_view);
                string str_pointer = pointer;
                MIView::beginAnimation("Transition Views for Foundation Call Only." + str_pointer);
                MIView::setAnimationCurve(options);
                MIView::setAnimationDuration(duration);
                from_view->setRectTransition(MGRectTransitionMake(from_view->getFrame().width, 0, 0));
                to_view->setRectTransition(MGRectTransitionMake(0, 0, 0));
                if(!will_start_animation.isNull())
                    MIView::setAnimationWillStartSelector(will_start_animation.responder,will_start_animation.action);
                if(!did_stop_animation.isNull())
                    MIView::setAnimationDidStopSelector(did_stop_animation.responder,did_stop_animation.action);
                MIView::commitAnimation();
                break;
            }
            case MIViewAnimationOptionShowHideTransitionViews:
            {
                MIView* super_view = from_view->parent();
                super_view->addSubview(to_view);
                to_view->setAlpha(0.0f);
                char pointer[128];
                sprintf(pointer, "%p", super_view);
                string str_pointer = pointer;
                MIView::beginAnimation("Transition Views for Foundation Call Only." + str_pointer);
                MIView::setAnimationCurve(options);
                MIView::setAnimationDuration(duration);
                from_view->setAlpha(0.0f);
                to_view->setAlpha(1.0f);
                to_view->setRectTransition(MGRectTransitionMake(0, 0, 0));
                if(!will_start_animation.isNull())
                    MIView::setAnimationWillStartSelector(will_start_animation.responder,will_start_animation.action);
                if(!did_stop_animation.isNull())
                    MIView::setAnimationDidStopSelector(did_stop_animation.responder,did_stop_animation.action);
                MIView::commitAnimation();
                break;
            }
            case MIViewAnimationOptionScaleTransitionViews:
            {
                MIView* super_view = from_view->parent();
                super_view->addSubview(to_view);
                to_view->setRectScale(MGRectScaleMake(0, 0, 1));
                char pointer[128];
                sprintf(pointer, "%p", super_view);
                string str_pointer = pointer;
                MIView::beginAnimation("Transition Views for Foundation Call Only." + str_pointer);
                MIView::setAnimationCurve(options);
                MIView::setAnimationDuration(duration);
                from_view->setRectScale(MGRectScaleMake(0, 0, 0));
                to_view->setRectScale(MGRectScaleMake(1, 1, 1));
                if(!will_start_animation.isNull())
                    MIView::setAnimationWillStartSelector(will_start_animation.responder,will_start_animation.action);
                if(!did_stop_animation.isNull())
                    MIView::setAnimationDidStopSelector(did_stop_animation.responder,did_stop_animation.action);
                MIView::commitAnimation();
                break;
            }
			case MIViewAnimationOptionRotateOutTransitionViews:
			{
				MIView* super_view = from_view->parent();
				if(super_view->containedSubview(to_view) || super_view->containedSubview(from_view))
				{
					to_view->detachFromSuperview();
					from_view->detachFromSuperview();
					super_view->addSubview(from_view);
					super_view->addSubview(to_view);
				}
				
				to_view->setRectScale(MGRectScaleMake(0, 0, 1));
				char pointer[128];
				sprintf(pointer, "%p", super_view);
				string str_pointer = pointer;
				MIView::beginAnimation("Transition Views for Foundation Call Only." + str_pointer);
				MIView::setAnimationCurve(options);
				MIView::setAnimationDuration(duration);
				to_view->setRectRotate(MGRectRotateMake(0,0,1,360));
				to_view->setRectScale(MGRectScaleMake(1, 1, 1));
				if(!will_start_animation.isNull())
					MIView::setAnimationWillStartSelector(will_start_animation.responder,will_start_animation.action);
				if(!did_stop_animation.isNull())
					MIView::setAnimationDidStopSelector(did_stop_animation.responder,did_stop_animation.action);
				MIView::commitAnimation();
				break;
			}

			case MIViewAnimationOptionRotateInTransitionViews:
			{
				MIView* super_view = from_view->parent();
				if(super_view->containedSubview(to_view) || super_view->containedSubview(from_view))
				{
					to_view->detachFromSuperview();
					from_view->detachFromSuperview();
					super_view->addSubview(to_view);
					super_view->addSubview(from_view);
				}

				from_view->setRectScale(MGRectScaleMake(1, 1, 1));
				char pointer[128];
				sprintf(pointer, "%p", super_view);
				string str_pointer = pointer;
				MIView::beginAnimation("Transition Views for Foundation Call Only." + str_pointer);
				MIView::setAnimationCurve(options);
				MIView::setAnimationDuration(duration);
				from_view->setRectRotate(MGRectRotateMake(0,0,1,-360));
				from_view->setRectScale(MGRectScaleMake(0, 0, 1));
				if(!will_start_animation.isNull())
					MIView::setAnimationWillStartSelector(will_start_animation.responder,will_start_animation.action);
				if(!did_stop_animation.isNull())
					MIView::setAnimationDidStopSelector(did_stop_animation.responder,did_stop_animation.action);
				MIView::commitAnimation();
				break;
			}
			
			case MIViewAnimationOptionTransitionFlipFromDown:
			{
				MIView* super_view = from_view->parent();
				super_view->addSubview(to_view);
				to_view->setRectTransition(MGRectTransitionMake(0, 0 - from_view->getFrame().height, 0));
				char pointer[128];
                sprintf(pointer, "%p", super_view);
                string str_pointer = pointer;
                MIView::beginAnimation("Transition Views for Foundation Call Only." + str_pointer);
				MIView::setAnimationCurve(options);
				MIView::setAnimationDuration(duration);
				from_view->setRectTransition(MGRectTransitionMake(0, to_view->getFrame().height, 0));
				to_view->setRectTransition(MGRectTransitionMake(0, 0, 0));
				to_view->setRectScale(MGRectScaleMake(1, 1, 1));
                if(!will_start_animation.isNull())
                    MIView::setAnimationWillStartSelector(will_start_animation.responder,will_start_animation.action);
                if(!did_stop_animation.isNull())
                    MIView::setAnimationDidStopSelector(did_stop_animation.responder,did_stop_animation.action);
				MIView::commitAnimation();
				break;
			}
			case MIViewAnimationOptionTransitionFlipFromUp:
			{
				MIView* super_view = from_view->parent();
				super_view->addSubview(to_view);
				to_view->setRectTransition(MGRectTransitionMake(0, to_view->getFrame().height, 0));
				char pointer[128];
                sprintf(pointer, "%p", super_view);
                string str_pointer = pointer;
                MIView::beginAnimation("Transition Views for Foundation Call Only." + str_pointer);
				MIView::setAnimationCurve(options);
				MIView::setAnimationDuration(duration);
				from_view->setRectTransition(MGRectTransitionMake(0, 0 - from_view->getFrame().height, 0));
				to_view->setRectTransition(MGRectTransitionMake(0, 0, 0));
				to_view->setRectScale(MGRectScaleMake(1, 1, 1));
                if(!will_start_animation.isNull())
                    MIView::setAnimationWillStartSelector(will_start_animation.responder,will_start_animation.action);
                if(!did_stop_animation.isNull())
                    MIView::setAnimationDidStopSelector(did_stop_animation.responder,did_stop_animation.action);
				MIView::commitAnimation();
				break;
			}
			case MIViewAnimationOptionTransitionCurlUp:
			{
				MIScreenCaptureView *saved_view=new MIScreenCaptureView(true);
				//saved_view->m_level=0.4;
				saved_view->initWithFromViewAndToView(from_view,to_view);
				saved_view->setDuration(duration);
                if(!will_start_animation.isNull())
                    saved_view->setAnimationStartSelector(will_start_animation.responder,will_start_animation.action);
                if(!did_stop_animation.isNull())
                    saved_view->setAnimationStopSelector(did_stop_animation.responder,did_stop_animation.action);
				break;
			}
			case MIViewAnimationOptionTransitionCurlDown:
			{
					MIPageTurnView *saved_view=new MIPageTurnView(false);
					saved_view->initWithFromViewAndToView(from_view,to_view);
					saved_view->setDuration(duration);
                if(!will_start_animation.isNull())
                    saved_view->setAnimationStartSelector(will_start_animation.responder,will_start_animation.action);
                if(!did_stop_animation.isNull())
                    saved_view->setAnimationStopSelector(did_stop_animation.responder,did_stop_animation.action);
					break;
			}
			case MIViewAnimationOptionTransitionFlipFromRightIniOS7Type:
				{
					MIView* super_view = from_view->parent();
					super_view->addSubview(to_view);
					to_view->setRectTransition(MGRectTransitionMake(to_view->getFrame().width, 0, 0));
					char pointer[128];
					sprintf(pointer, "%p", super_view);
					string str_pointer = pointer;
					MIView::beginAnimation("Transition Views for Foundation Call Only." + str_pointer);
					MIView::setAnimationCurve(MIViewAnimationOptionCurveInertia);
					MIView::setAnimationDuration(duration);
					from_view->setRectTransition(MGRectTransitionMake(0 - 0.2*from_view->getFrame().width, 0, 0));
					to_view->setRectTransition(MGRectTransitionMake(0, 0, 0));
					if(!will_start_animation.isNull())
						MIView::setAnimationWillStartSelector(will_start_animation.responder,will_start_animation.action);
					if(!did_stop_animation.isNull())
						MIView::setAnimationDidStopSelector(did_stop_animation.responder,did_stop_animation.action);
					MIView::commitAnimation();
					break;
				}
			case MIViewAnimationOptionTransitionFlipFromLeftIniOS7Type:
				{
					MIView* super_view = from_view->parent();
					super_view->addSubview(to_view);

					from_view->detachFromSuperview();
					super_view->addSubview(from_view);
					to_view->setRectTransition(MGRectTransitionMake(0 - 0.2*to_view->getFrame().width, 0, 0));
					char pointer[128];
					sprintf(pointer, "%p", super_view);
					string str_pointer = pointer;
					MIView::beginAnimation("Transition Views for Foundation Call Only." + str_pointer);
					MIView::setAnimationCurve(MIViewAnimationOptionCurveInertia);
					MIView::setAnimationDuration(duration);
					from_view->setRectTransition(MGRectTransitionMake(from_view->getFrame().width, 0, 0));
					to_view->setRectTransition(MGRectTransitionMake(0, 0, 0));
					if(!will_start_animation.isNull())
						MIView::setAnimationWillStartSelector(will_start_animation.responder,will_start_animation.action);
					if(!did_stop_animation.isNull())
						MIView::setAnimationDidStopSelector(did_stop_animation.responder,did_stop_animation.action);
					MIView::commitAnimation();
					break;
				}
            default:
                break;
        }

    }

} //namespace z11
