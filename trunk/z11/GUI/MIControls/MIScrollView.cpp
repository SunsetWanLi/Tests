//
//  MIScrollView.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-15.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MIScrollView.h"
#include "MIImageView.h"
#include "../../GraphicCore/MGImage.h"
#include "../../System/Timer/MGTimer.h"
#include "../../System/Shared/MGLog.h"
#include "../GuiContainer/GuiContainer.h"
#include "../GestureRecognizer/MIPanGestureRecognizer.h"
#include "../GestureRecognizer/MIPinchGestureRecognizer.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>
#define SCROLL_LONGPRESS_SIGNIFICANT_CHANGE_VALVE 6.0f
#define SCROLL_VIEW_LONG_PRESS_VALVE 0.2f

#define SCROLL_VIEW_SCROLL_BOUNCE_VALVE  180.0f
#define SCROLL_VIEW_SCROLL_DAMPING_RATE 0.3f // 滚动到开头结尾的bouncing阻尼系数 //
#define SCROLL_VIEW_SCROLL_INDICATOR__WIDTH 6.0f/640.0f
#define SCROLL_VIEW_SCROLL_INDICATOR__BORDER 2.0f/640.0f // 默认滚动条与scroll view边缘的间隔 //
#define SCROLL_VIEW_SCROLL_INERTIA_SPEED_RATE 0.5f // 滚动惯性时通过触摸获取的速度值的调整系数 //
#define SCROLL_VIEW_SCROLL_INERTIA_SPEED_MAX_VALVE 200.0f // 滚动惯性初始速度的最大阀值 //
#define SCROLL_VIEW_SCROLL_INERTIA_SPEED_CACULATE_RATE 10.0f // 滚动惯性初始速度计算调整系数 //
#define SCROLL_VIEW_SCROLL_INERTIA_SPEED_CACULATE_VALVE 10.0f // 惯性计算累计阀值 //

#define SCROLL_VIEW_SCROLL_INERTIA_DURATION_RATE   0.0005f  //滚动惯性时间的调整系数

static float scroll_view_scroll_indicator_width;
static float scroll_view_scroll_indicator_border;

#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288   /* pi */
#endif

namespace z11
{
    float MIScrollView::longpress_significant_move_valve_rate = SCROLL_LONGPRESS_SIGNIFICANT_CHANGE_VALVE / 640.0f;
    
    MIScrollView::MIScrollView()
    {
        class_name = "MIScrollView";
        scroll_view_scroll_bounce_value=SCROLL_VIEW_SCROLL_BOUNCE_VALVE;
        start_bounce_value = 0;
        scroll_damping_rate = SCROLL_VIEW_SCROLL_DAMPING_RATE;
        
        scrollInertiaSpeedValue = SCROLL_VIEW_SCROLL_INERTIA_SPEED_RATE;
        scrollInertiaDurationRate = SCROLL_VIEW_SCROLL_INERTIA_DURATION_RATE;
        //初始化计时器 //
        long_press_tag = INVALID_TOUCH_TAG;
        MIEvent long_press_event;
        long_press_event._event_type = SCROLL_VIEW_LONG_PRESS_EVENT;
        long_press_event._content = NULL;
        long_press_event._responder = this;
        SELHandler selector;
        selector.sender = this;
        selector.sel_func = (SELFunc)&MIScrollView::longPressEvent;
        long_press_event._handler = selector;
        long_press_timer = MGTimer::timerWithTimeInterval(MGTimeDelta::FromSecondsF(SCROLL_VIEW_LONG_PRESS_VALVE), long_press_event, false);
        sub_rotate = MGRectRotateMake(0, 0, 0, 0);
        sub_transition = MGRectTransitionMake(0, 0, 0);
        sub_scale = MGRectScaleMake(1,1,1);
        //
        selected_view = NULL;
        // initial flags
        // MIView flags
        _view_flags.clip_subviews = 1;
        _view_flags.sub_scale_enable = 1;
        // MIScrollView flags
        _scroll_view_flags.vertical_bounce_enabled = 1;
        _scroll_view_flags.horizontal_bounce_enabled = 1;
        _scroll_view_flags.vertical_indicator_show = 1;
        _scroll_view_flags.horizontal_indicator_show = 1;
        _scroll_view_flags.vertical_indicator_always_show = 0;
        _scroll_view_flags.horizontal_indicator_always_show = 0;
        _scroll_view_flags.inertia_enable = 1;
        _scroll_view_flags.decelarate_enable = 1;
        _scroll_view_flags.zoom_enable = 1;
        _scroll_view_flags.vertical_scroll_enabled = 1;
        _scroll_view_flags.horizontal_scroll_enabled = 1;
        _scroll_view_flags.passing_vertical_scroll_enable = 0;
        _scroll_view_flags.passing_horizontal_scroll_enable = 0;
        _scroll_view_flags.hide_keyboard_when_scrolling = 1;
        // //
        _vertical_indicator = NULL;
        _horizontal_indicator = NULL;
		//
		m_did_after_inertia_inScroll.sender = this;
		m_did_after_inertia_inScroll.sel_func = (SELFunc)&MIScrollView::bounceInScroll;
        //
        momentum_orientation_x = 0;
        momentum_orientation_y = 0;
        momentum_time_x = 0;
        momentum_time_y = 0;
        need_momentum = false;
        m_pan_gesture = NULL;
        m_pinch_gesture = NULL;
        passing_move = false;
        scroll_view_delegate = NULL;
        m_max_zoom_scale = 2.0f;
        m_min_zoom_scale = 0.0f;

		scrolling = false;
        scroll_view_scroll_indicator_width = MINVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight())*SCROLL_VIEW_SCROLL_INDICATOR__WIDTH;
        scroll_view_scroll_indicator_border = MINVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight())*SCROLL_VIEW_SCROLL_INDICATOR__BORDER;
    }
    MIScrollView::~MIScrollView()
    {
        if (long_press_timer != NULL) {
            delete long_press_timer;
            long_press_timer = NULL;
        }
        if (_horizontal_indicator != NULL) {
            delete _horizontal_indicator;
            _horizontal_indicator = NULL;
        }
        if (_vertical_indicator != NULL) {
            delete _vertical_indicator;
            _vertical_indicator = NULL;
        }
        if (m_pan_gesture != NULL) {
            delete_and_set_null(m_pan_gesture);
        }
        if (m_pinch_gesture != NULL) {
            delete_and_set_null(m_pinch_gesture);
        }
        scroll_view_delegate = NULL;
    }
    void MIScrollView::initWithFrame(MGRect frame)
    {
        MIView::initWithFrame(frame);
        m_content_size.right = (long)frame.width;
        m_content_size.bottom = (long)frame.height;
        if (_vertical_indicator != NULL || _horizontal_indicator != NULL) {
            return;
        }
        if (_scroll_view_flags.vertical_indicator_show == 1) {
            _vertical_indicator = new MIImageView();
            _vertical_indicator->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::darkGray()));
            _vertical_indicator->view_round_mode = RoundRectAll;
            _vertical_indicator->view_round_rate = scroll_view_scroll_indicator_width * 0.5f;
            if (_scroll_view_flags.vertical_indicator_always_show == 0) {
                _vertical_indicator->setAlpha(0.0f);
            }
            float x_value = frame.width - scroll_view_scroll_indicator_width - scroll_view_scroll_indicator_border;
            float height_value = frame.height - (2 * scroll_view_scroll_indicator_border);
			if (height_value <= 0)
			{
				height_value = frame.height;
			}
            float y_value = scroll_view_scroll_indicator_border;
            if ((m_content_size.bottom - m_content_size.top) > frame.height) {
                y_value += height_value * (m_content_size.top - sub_transition.axis_y) / (m_content_size.bottom - m_content_size.top);
                height_value = height_value * frame.height / (m_content_size.bottom - m_content_size.top);
                if (y_value < scroll_view_scroll_indicator_border) {
                    height_value = height_value * (1 - (sub_transition.axis_y / scroll_view_scroll_bounce_value));
                    y_value = scroll_view_scroll_indicator_border;
                } else if (y_value + height_value > (m_frame.height - (2 * scroll_view_scroll_indicator_border))) {
                    height_value = height_value * (1 - ((m_frame.height - sub_transition.axis_y - m_content_size.bottom + m_content_size.top) / scroll_view_scroll_bounce_value));
                    y_value = m_frame.height - height_value - scroll_view_scroll_indicator_border;
                }
            }
            _vertical_indicator->initWithFrame(MGRectMake(x_value + m_bounds.x, y_value + m_bounds.y, scroll_view_scroll_indicator_width, height_value));
        }
        if (_scroll_view_flags.horizontal_indicator_show == 1) {
            _horizontal_indicator = new MIImageView();
            _horizontal_indicator->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::darkGray()));
            _horizontal_indicator->view_round_mode = RoundRectAll;
            _horizontal_indicator->view_round_rate = scroll_view_scroll_indicator_width * 0.5f;
            if (_scroll_view_flags.horizontal_indicator_always_show == 0) {
                _horizontal_indicator->setAlpha(0.0f);
            }
            float y_value = frame.height - scroll_view_scroll_indicator_width - scroll_view_scroll_indicator_border;
            float width_value = frame.width - (2 * scroll_view_scroll_indicator_border);
			if (width_value <= 0)
			{
				width_value = frame.width;
			}
            float x_value = scroll_view_scroll_indicator_border;
            if ((m_content_size.right - m_content_size.left) > frame.width) {
                x_value = width_value * (m_content_size.left - sub_transition.axis_x) / (m_content_size.right - m_content_size.left);
                width_value = width_value * frame.width / (m_content_size.right - m_content_size.left);
                if (x_value < scroll_view_scroll_indicator_border) {
                    width_value = width_value * (1 - (sub_transition.axis_x / scroll_view_scroll_bounce_value));
                    x_value = scroll_view_scroll_indicator_border;
                } else if (x_value + width_value > (m_frame.width - (2 * scroll_view_scroll_indicator_border))) {
                    width_value = width_value * (1 - ((m_frame.width - sub_transition.axis_x - m_content_size.right + m_content_size.left) / scroll_view_scroll_bounce_value));
                    x_value = m_frame.width - width_value - scroll_view_scroll_indicator_border;
                }
            }
            _horizontal_indicator->initWithFrame(MGRectMake(x_value + m_bounds.x, y_value + m_bounds.y, width_value, scroll_view_scroll_indicator_width));
        }
        if (m_pan_gesture == NULL) {
            m_pan_gesture = new MIPanGestureRecognizer();
            SELHandler SEL_handler;
            SEL_handler.sender=m_pan_gesture;
            SEL_handler.sel_func=(SELFunc)&MIScrollView::panCall;
            m_pan_gesture->initWithTarget(this,SEL_handler);
            addGestureRecognizer(m_pan_gesture);
        }
        if (m_pinch_gesture == NULL) {
            m_pinch_gesture = new MIPinchGestureRecognizer();
            SELHandler SEL_handler;
            SEL_handler.sender=m_pinch_gesture;
            SEL_handler.sel_func=(SELFunc)&MIScrollView::zoomCall;
            m_pinch_gesture->initWithTarget(this,SEL_handler);
            addGestureRecognizer(m_pinch_gesture);
        }
    }

    void MIScrollView::adjustIndicatorInVertical()
    {
        if (_scroll_view_flags.vertical_indicator_show == 0 || _vertical_indicator == NULL) {
            return;
        }
        float x_value = m_frame.width - scroll_view_scroll_indicator_width - scroll_view_scroll_indicator_border;
        // 获得垂直进度指示条的可显示区域高度 //
        float available_height_value = 0;
        if (_scroll_view_flags.horizontal_indicator_show != 0 && _horizontal_indicator != NULL) {
            available_height_value = m_frame.height - (3 * scroll_view_scroll_indicator_border) - scroll_view_scroll_indicator_width;
        }else{
            available_height_value = m_frame.height - (2 * scroll_view_scroll_indicator_border);
        }
        
        if (available_height_value <= 0) {
            available_height_value = 0;
        }
        
        float height_value = available_height_value;
        float y_value = scroll_view_scroll_indicator_border;
        
        float new_top = (m_content_size.top - (m_frame.height * 0.5 + sub_scale_p.y)) * sub_scale.axis_y + (m_frame.height * 0.5 + sub_scale_p.y) + sub_transition.axis_y;
        float new_bottom = (m_content_size.bottom - (m_frame.height * 0.5 + sub_scale_p.y)) * sub_scale.axis_y + (m_frame.height * 0.5 + sub_scale_p.y) + sub_transition.axis_y;
        if (new_top > displacement_size.top) {
            new_top = displacement_size.top;
        }
        if (new_bottom < m_frame.height - displacement_size.bottom) {
            new_bottom = m_frame.height - displacement_size.bottom;
        }
        // 获得当前内容需要的垂直大小 //
        float y_size = new_bottom - new_top;
        
        // 计算超出的部分占据整个可显示区域的比例，并相应的计算出指示条的垂直位置 //
        if (y_size > m_frame.height - (displacement_size.top + displacement_size.bottom)) {
            y_value = height_value * (0 - new_top) / y_size;
            height_value = height_value * m_frame.height / y_size;
            if (y_value < scroll_view_scroll_indicator_border) {
                height_value -= scroll_view_scroll_indicator_border - y_value;
                if (height_value < scroll_view_scroll_indicator_width)
                    height_value = scroll_view_scroll_indicator_width;
                y_value = scroll_view_scroll_indicator_border;
            } else if (y_value + height_value > available_height_value) {
                height_value -= y_value + height_value - available_height_value;
                if (height_value < scroll_view_scroll_indicator_width)
                    height_value = scroll_view_scroll_indicator_width;
                y_value = available_height_value - height_value;
            }
        }
        
        _vertical_indicator->setFrame(MGRectMake(x_value + m_bounds.x, y_value + m_bounds.y,  scroll_view_scroll_indicator_width, height_value));
    }

    void MIScrollView::adjustIndicatorInHorizontal()
    {
        if (_scroll_view_flags.horizontal_indicator_show == 0 || _horizontal_indicator == NULL) {
            return;
        }
        float y_value = m_frame.height - scroll_view_scroll_indicator_width - scroll_view_scroll_indicator_border;
        // 获得水平进度指示条的可显示区域宽度 //
        float available_width_value = 0;
        if (_scroll_view_flags.vertical_indicator_show != 0 && _vertical_indicator != NULL) {
            available_width_value = m_frame.width - (3 * scroll_view_scroll_indicator_border) - scroll_view_scroll_indicator_width;
        }else{
            available_width_value = m_frame.width - (2 * scroll_view_scroll_indicator_border);
        }
        
        if (available_width_value <= 0) {
            available_width_value = 0;
        }
        
        float width_value = available_width_value;
        float x_value = scroll_view_scroll_indicator_border;
        
        float new_left = (m_content_size.left - (m_frame.width * 0.5 + sub_scale_p.x)) * sub_scale.axis_x + (m_frame.width * 0.5 + sub_scale_p.x) + sub_transition.axis_x;
        float new_right = (m_content_size.right - (m_frame.width * 0.5 + sub_scale_p.x)) * sub_scale.axis_x + (m_frame.width * 0.5 + sub_scale_p.x) + sub_transition.axis_x;
        if (new_left > displacement_size.left) {
            new_left = displacement_size.left;
        }
        if (new_right < m_frame.width - displacement_size.right) {
            new_right = m_frame.width - displacement_size.right;
        }
        // 获得当前内容需要的水平大小 //
        float x_size = new_right - new_left;
        
        if (x_size > m_frame.width - (displacement_size.left + displacement_size.right)) {
            // 计算超出的部分占据整个可显示区域的比例，并相应的计算出指示条的水平位置 //
            x_value = width_value * (0 - new_left) / x_size;
            width_value = width_value * m_frame.width / x_size;
            if (x_value < scroll_view_scroll_indicator_border) {
                width_value -= scroll_view_scroll_indicator_border - x_value;
                if (width_value < scroll_view_scroll_indicator_width)
                    width_value = scroll_view_scroll_indicator_width;
                x_value = scroll_view_scroll_indicator_border;
            } else if (x_value + width_value > available_width_value) {
                width_value -= x_value + width_value - available_width_value;
                if (width_value < scroll_view_scroll_indicator_width)
                    width_value = scroll_view_scroll_indicator_width;
                x_value = available_width_value - width_value;
            }
        }
        
        _horizontal_indicator->setFrame(MGRectMake(x_value + m_bounds.x, y_value + m_bounds.y, width_value, scroll_view_scroll_indicator_width));
    }

    void MIScrollView::frameChanged(const MGRect &frame)
    {
        MIView::frameChanged(frame);
        if (m_content_size.right < frame.width) {
            m_content_size.right = (long)frame.width;
        }
        if (m_content_size.bottom < frame.height) {
            m_content_size.bottom = (long)frame.height;
        }
        adjustIndicatorInVertical();
        adjustIndicatorInHorizontal();
    }
    void MIScrollView::subRectTransitionChanged(const MGRectTransition &_transition)
    {
        bool need_stop_scroll_animiation = false;
        MGRectTransition transition_now = getSubRectTransition();
        MGPoint target_offset=MGPointMake(transition_now.axis_x,transition_now.axis_y);
        scrollViewDraggingWithContentOffset(target_offset,MGPoint());
        float new_top = (m_content_size.top - (m_frame.height * 0.5 + sub_scale_p.y)) * sub_scale.axis_y + (m_frame.height * 0.5 + sub_scale_p.y);
        float new_bottom = (m_content_size.bottom - (m_frame.height * 0.5 + sub_scale_p.y)) * sub_scale.axis_y + (m_frame.height * 0.5 + sub_scale_p.y);
        float new_left = (m_content_size.left - (m_frame.width * 0.5 + sub_scale_p.x)) * sub_scale.axis_x + (m_frame.width * 0.5 + sub_scale_p.x);
        float new_right = (m_content_size.right - (m_frame.width * 0.5 + sub_scale_p.x)) * sub_scale.axis_x + (m_frame.width * 0.5 + sub_scale_p.x);
        if (transition_now.axis_x > new_left + scroll_view_scroll_bounce_value * scroll_damping_rate + displacement_size.left) {
            need_stop_scroll_animiation = true;
        }
        if (m_frame.width - transition_now.axis_x > new_right + scroll_view_scroll_bounce_value * scroll_damping_rate + displacement_size.right) {
            need_stop_scroll_animiation = true;
        }
        if (transition_now.axis_y > new_top + scroll_view_scroll_bounce_value * scroll_damping_rate + displacement_size.top) {
            need_stop_scroll_animiation = true;
        }
        if (m_frame.height - transition_now.axis_y > new_bottom + scroll_view_scroll_bounce_value * scroll_damping_rate + displacement_size.bottom) {
            need_stop_scroll_animiation = true;
        }
        if (need_stop_scroll_animiation) {
            char pointer[128];
            sprintf(pointer, "%p", this);
            string str_pointer = pointer;
            MIView::stopAnimation("Scroll View Inertia In Scroll." + str_pointer);
        }
        adjustIndicatorInVertical();
        adjustIndicatorInHorizontal();
    }
    
    void MIScrollView::subRectScaleChanged(const MGRectScale &_scale)
    {
        adjustIndicatorInVertical();
        adjustIndicatorInHorizontal();
    }
    
    // caculate inertia //
    void MIScrollView::caculateInertiaWithDisAndTimeInVertical(float dis, double time)
    {
        if (dis < SCROLL_VIEW_SCROLL_INERTIA_SPEED_CACULATE_VALVE && dis > -SCROLL_VIEW_SCROLL_INERTIA_SPEED_CACULATE_VALVE) {
            inertia_speed_in_axis_y = 0;
            momentum_time_y = 0;
            momentum_orientation_y = 0;
            return;
        }
        if((dis < 0 && momentum_orientation_y > 0) || (dis > 0 && momentum_orientation_y < 0))
        {
            momentum_time_y = 0;
            momentum_orientation_y = 0;
        }
        momentum_orientation_y += dis;
        momentum_time_y += time;
    }
    
    void MIScrollView::caculateInertiaWithDisAndTimeInHorizontal(float dis, double time)
    {
        if (dis < SCROLL_VIEW_SCROLL_INERTIA_SPEED_CACULATE_VALVE && dis > -SCROLL_VIEW_SCROLL_INERTIA_SPEED_CACULATE_VALVE) {
            inertia_speed_in_axis_x = 0;
            momentum_time_x = 0;
            momentum_orientation_x = 0;
            return;
        }
        if((dis < 0 && momentum_orientation_x > 0) || (dis > 0 && momentum_orientation_x < 0))
        {
            momentum_time_x = 0;
            momentum_orientation_x = 0;
        }
        momentum_orientation_x += dis;
        momentum_time_x += time;
    }
    
    //Scroll Animation squence//
    void MIScrollView::inertiaInScroll(MGObject *_sender)
    {
        inertia_speed_in_axis_x = 0;
        if (momentum_time_x > 0.0001) {
            inertia_speed_in_axis_x = momentum_orientation_x / momentum_time_x;
        }
        inertia_speed_in_axis_y = 0;
        if (momentum_time_y > 0.0001) {
            inertia_speed_in_axis_y = momentum_orientation_y / momentum_time_y;
        }
//        MGLogD("FINALLY!!! inertia speed is %.2lf / %.4lf =\t%.2lf \tpix/s.",momentum_orientation_y, momentum_time_y, inertia_speed_in_axis_y);
        
        float duration = 1.0f;
        float duration_x = 0.0f;
        float duration_y = 0.0f;
        MGRectTransition transition_now =  sub_transition;
        if (_scroll_view_flags.horizontal_scroll_enabled == 1) {
            transition_now.axis_x += inertia_speed_in_axis_x * scrollInertiaSpeedValue;
            duration_x = inertia_speed_in_axis_x * scrollInertiaDurationRate;
        }
        if (_scroll_view_flags.vertical_scroll_enabled == 1) {
            transition_now.axis_y += inertia_speed_in_axis_y * scrollInertiaSpeedValue;
            duration_y = inertia_speed_in_axis_y * scrollInertiaDurationRate;
        }
        duration = sqrt((duration_x * duration_x) + (duration_y * duration_y));
        if (duration > 2.0f) {
            duration = 2.0f;
        }
        
        if (_scroll_view_flags.horizontal_bounce_enabled == 0) {
            float new_left = (m_content_size.left - (m_frame.width * 0.5 + sub_scale_p.x)) * sub_scale.axis_x + (m_frame.width * 0.5 + sub_scale_p.x);
            float new_right = (m_content_size.right - (m_frame.width * 0.5 + sub_scale_p.x)) * sub_scale.axis_x + (m_frame.width * 0.5 + sub_scale_p.x);
            if (transition_now.axis_x > new_left + displacement_size.left) {
                transition_now.axis_x = new_left + displacement_size.left;
            }
            if (m_frame.width - transition_now.axis_x > new_right + displacement_size.right) {
                transition_now.axis_x = m_frame.width -  new_right + displacement_size.right;
            }
        }
        if (_scroll_view_flags.vertical_bounce_enabled == 0) {
            float new_top = (m_content_size.top - (m_frame.height * 0.5 + sub_scale_p.y)) * sub_scale.axis_y + (m_frame.height * 0.5 + sub_scale_p.y);
            float new_bottom = (m_content_size.bottom - (m_frame.height * 0.5 + sub_scale_p.y)) * sub_scale.axis_y + (m_frame.height * 0.5 + sub_scale_p.y);
            if (transition_now.axis_y > new_top + displacement_size.top) {
                transition_now.axis_y = new_top + displacement_size.top;
            }
            if (m_frame.height - transition_now.axis_y > new_bottom + displacement_size.bottom) {
                transition_now.axis_y = m_frame.height - new_bottom + displacement_size.bottom;
            }
        }
        
        MGPoint target_offset=MGPointMake(transition_now.axis_x,transition_now.axis_y);
        if (scroll_view_delegate != NULL) {
            scroll_view_delegate->scrollViewWillEndDraggingWithVelocityTargetContentOffset(this, MGPointMake(inertia_speed_in_axis_x, inertia_speed_in_axis_y), target_offset);
        }
        
        char pointer[128];
        sprintf(pointer, "%p", this);
        string str_pointer = pointer;
        MIView::beginAnimation("Scroll View Inertia In Scroll." + str_pointer);
        MIView::setAnimationDuration(duration);
        if (_scroll_view_flags.decelarate_enable == 1) {
            if (scroll_view_delegate != NULL) {
                scroll_view_delegate->scrollViewWillBeginDecelerating(this);
            }
            MIView::setAnimationCurve(MIViewAnimationOptionCurveInertia);
        }else{
            MIView::setAnimationCurve(MIViewAnimationOptionCurveLinear);
        }
//        MGLogD("set sub rect transition %.2f pix in %.4lf second.", transition_now.axis_y, duration);
        setSubRectTransition(transition_now);
        MIView::setAnimationDidStopSelector(this,m_did_after_inertia_inScroll);
		MIView::commitAnimation();
		{
			scrollViewDraggingWithContentOffset(target_offset,MGPointMake(inertia_speed_in_axis_x,inertia_speed_in_axis_y));
		}
    }
    void MIScrollView::bounceInScroll(MGObject* _sender)
    {
        if (_scroll_view_flags.decelarate_enable == 1)
        {
            if (scroll_view_delegate != NULL) {
                scroll_view_delegate->scrollViewDidEndDecelerating(this);
            }
        }
        float new_top = (m_content_size.top - (m_frame.height * 0.5 + sub_scale_p.y)) * sub_scale.axis_y + (m_frame.height * 0.5 + sub_scale_p.y) + sub_transition.axis_y;
        float new_bottom = (m_content_size.bottom - (m_frame.height * 0.5 + sub_scale_p.y)) * sub_scale.axis_y + (m_frame.height * 0.5 + sub_scale_p.y) + sub_transition.axis_y;
        float new_left = (m_content_size.left - (m_frame.width * 0.5 + sub_scale_p.x)) * sub_scale.axis_x + (m_frame.width * 0.5 + sub_scale_p.x) + sub_transition.axis_x;
        float new_right = (m_content_size.right - (m_frame.width * 0.5 + sub_scale_p.x)) * sub_scale.axis_x + (m_frame.width * 0.5 + sub_scale_p.x) + sub_transition.axis_x;
        MGRectTransition transition_now =  sub_transition;
        
        do{
        if (new_left > displacement_size.left) {
            //
            transition_now.axis_x -= new_left - displacement_size.left;
            if (new_right - new_left < m_frame.width - (displacement_size.left + displacement_size.right)) {
                transition_now.axis_x += ((m_frame.width - (displacement_size.left + displacement_size.right)) - (new_right - new_left)) * 0.5;
            }
            break;
        }
        if (new_right < m_frame.width - displacement_size.right) {
            //
            transition_now.axis_x -= new_right - (m_frame.width - displacement_size.right);
            if (new_right - new_left < m_frame.width - (displacement_size.left + displacement_size.right)) {
                transition_now.axis_x -= ((m_frame.width - (displacement_size.left + displacement_size.right)) - (new_right - new_left)) * 0.5;
            }
            break;
        }
        }while(0);
        do{
        if (new_top > displacement_size.top) {
            //
            transition_now.axis_y -= new_top - displacement_size.top;
            if (new_bottom - new_top < m_frame.height - (displacement_size.bottom + displacement_size.top)) {
                transition_now.axis_y += ((m_frame.height - (displacement_size.top + displacement_size.bottom)) - (new_bottom - new_top)) * 0.5;
            }
            break;
        }
        if (new_bottom < m_frame.height - displacement_size.bottom) {
            //
            transition_now.axis_y -= new_bottom - (m_frame.height - displacement_size.bottom);
            if (new_bottom - new_top < m_frame.height - (displacement_size.bottom + displacement_size.top)) {
                transition_now.axis_y -= ((m_frame.height - (displacement_size.top + displacement_size.bottom)) - (new_bottom - new_top)) * 0.5;
            }
            break;
        }
        }while(0);
        
        float duration = 0.2f;
        if (transition_now == sub_transition) {
            duration = 0.0f;
        }
        char pointer[128];
        sprintf(pointer, "%p", this);
        string str_pointer = pointer;
        MIView::beginAnimation("Scroll View Bounce In Scroll." + str_pointer);
        MIView::setAnimationDuration(duration);
        setSubRectTransition(transition_now);
        SELHandler selector;
        selector.sender = this;
        selector.sel_func = (SELFunc)&MIScrollView::indicatorInScroll;
        MIView::setAnimationDidStopSelector(this,selector);
        MIView::commitAnimation();

		{
			MGPoint target_offset=MGPointMake(transition_now.axis_x,transition_now.axis_y);
			scrollViewDraggingWithContentOffset(target_offset,MGPoint());
		}
    }
    void MIScrollView::bounceInZoom(MGObject* _sender)
    {
        if (_scroll_view_flags.decelarate_enable == 1)
        {
            if (scroll_view_delegate != NULL) {
                scroll_view_delegate->scrollViewDidEndDecelerating(this);
            }
        }
        
        MGRectScale scale_now =  sub_scale;
        MGRectTransition transition_now =  sub_transition;
        // get target scale
        float new_top = (m_content_size.top - (m_frame.height * 0.5 + sub_scale_p.y)) * scale_now.axis_y + (m_frame.height * 0.5 + sub_scale_p.y) + transition_now.axis_y;
        float new_bottom = (m_content_size.bottom - (m_frame.height * 0.5 + sub_scale_p.y)) * scale_now.axis_y + (m_frame.height * 0.5 + sub_scale_p.y) + transition_now.axis_y;
        float new_left = (m_content_size.left - (m_frame.width * 0.5 + sub_scale_p.x)) * scale_now.axis_x + (m_frame.width * 0.5 + sub_scale_p.x) + transition_now.axis_x;
        float new_right = (m_content_size.right - (m_frame.width * 0.5 + sub_scale_p.x)) * scale_now.axis_x + (m_frame.width * 0.5 + sub_scale_p.x) + transition_now.axis_x;
        
        if (scale_now.axis_x > m_max_zoom_scale) {
            scale_now.axis_x = m_max_zoom_scale;
        }
        if (scale_now.axis_y > m_max_zoom_scale) {
            scale_now.axis_y = m_max_zoom_scale;
        }
        
        if (new_right - new_left <= 0) {
            return;
        }
        if (new_bottom - new_top <= 0) {
            return;
        }
        
        float h_need_scale = 1.0f;
        if (new_right - new_left < m_frame.width - (displacement_size.left + displacement_size.right)) {
             h_need_scale = (m_frame.width - (displacement_size.left + displacement_size.right)) / (new_right - new_left);
        }
        
        float v_need_scale = 1.0f;
        if (new_bottom - new_top < m_frame.height - (displacement_size.bottom + displacement_size.top)) {
            v_need_scale = (m_frame.height - (displacement_size.bottom + displacement_size.top)) / (new_bottom - new_top);
        }
        
        float need_scale = h_need_scale > v_need_scale ? v_need_scale : h_need_scale;
        // scale_now is the target scale //
        scale_now.axis_x *= need_scale;
        scale_now.axis_y *= need_scale;
        
        if (scale_now.axis_x < m_min_zoom_scale) {
            scale_now.axis_x = m_min_zoom_scale;
        }
        if (scale_now.axis_y < m_min_zoom_scale) {
            scale_now.axis_y = m_min_zoom_scale;
        }
        
        MGPoint curr_center_point = m_pinch_center;
        translateSubPoint(curr_center_point);
        //setSubRectScale(scale_now);
        {
            MIScrollView tmp_view;
            tmp_view.m_frame = m_frame;
            tmp_view.m_bounds = m_bounds;
            tmp_view.m_center = m_center;
            tmp_view.sub_transition = sub_transition;
            tmp_view.sub_rotate = sub_rotate;
            tmp_view.sub_rotate_p = sub_rotate_p;
            tmp_view.sub_scale = scale_now;
            tmp_view.sub_scale_p = sub_scale_p;
            tmp_view.detranslateSubPoint(curr_center_point);
        }
        transition_now.axis_x += (m_pinch_center.x - curr_center_point.x);
        transition_now.axis_y += (m_pinch_center.y - curr_center_point.y);
        
        // get new transition
        new_top = (m_content_size.top - (m_frame.height * 0.5 + sub_scale_p.y)) * scale_now.axis_y + (m_frame.height * 0.5 + sub_scale_p.y) + transition_now.axis_y;
        new_bottom = (m_content_size.bottom - (m_frame.height * 0.5 + sub_scale_p.y)) * scale_now.axis_y + (m_frame.height * 0.5 + sub_scale_p.y) + transition_now.axis_y;
        new_left = (m_content_size.left - (m_frame.width * 0.5 + sub_scale_p.x)) * scale_now.axis_x + (m_frame.width * 0.5 + sub_scale_p.x) + transition_now.axis_x;
        new_right = (m_content_size.right - (m_frame.width * 0.5 + sub_scale_p.x)) * scale_now.axis_x + (m_frame.width * 0.5 + sub_scale_p.x) + transition_now.axis_x;
        
        do{
            if (new_left > displacement_size.left) {
                //
                transition_now.axis_x -= new_left - displacement_size.left;
                if (new_right - new_left < m_frame.width - (displacement_size.left + displacement_size.right)) {
                    transition_now.axis_x += ((m_frame.width - (displacement_size.left + displacement_size.right)) - (new_right - new_left)) * 0.5;
                }
                break;
            }
            if (new_right < m_frame.width - displacement_size.right) {
                //
                transition_now.axis_x -= new_right - (m_frame.width - displacement_size.right);
                if (new_right - new_left < m_frame.width - (displacement_size.left + displacement_size.right)) {
                    transition_now.axis_x -= ((m_frame.width - (displacement_size.left + displacement_size.right)) - (new_right - new_left)) * 0.5;
                }
                break;
            }
        }while(0);
        do{
            if (new_top > displacement_size.top) {
                //
                transition_now.axis_y -= new_top - displacement_size.top;
                if (new_bottom - new_top < m_frame.height - (displacement_size.bottom + displacement_size.top)) {
                    transition_now.axis_y += ((m_frame.height - (displacement_size.top + displacement_size.bottom)) - (new_bottom - new_top)) * 0.5;
                }
                break;
            }
            if (new_bottom < m_frame.height - displacement_size.bottom) {
                //
                transition_now.axis_y -= new_bottom - (m_frame.height - displacement_size.bottom);
                if (new_bottom - new_top < m_frame.height - (displacement_size.bottom + displacement_size.top)) {
                    transition_now.axis_y -= ((m_frame.height - (displacement_size.top + displacement_size.bottom)) - (new_bottom - new_top)) * 0.5;
                }
                break;
            }
        }while(0);
        
        float duration = 0.1f;
        if (scale_now == sub_scale) {
            duration = 0.0f;
        }
        // set new scale
        char pointer[128];
        sprintf(pointer, "%p", this);
        string str_pointer = pointer;
        MIView::beginAnimation("Scroll View Bounce In Zoom." + str_pointer);
        MIView::setAnimationDuration(duration);
        setSubRectScale(scale_now);
        MIView::commitAnimation();
        
        duration = 0.1f;
        if (transition_now == sub_transition) {
            duration = 0.0f;
        }
        // set new transition
        sprintf(pointer, "%p", this);
        str_pointer = pointer;
        MIView::beginAnimation("Scroll View Bounce In Scroll." + str_pointer);
        MIView::setAnimationDuration(duration);
        setSubRectTransition(transition_now);
        SELHandler selector;
        selector.sender = this;
        selector.sel_func = (SELFunc)&MIScrollView::indicatorInScroll;
        MIView::setAnimationDidStopSelector(this,selector);
        MIView::commitAnimation();
        
		{
			MGPoint target_offset=MGPointMake(transition_now.axis_x,transition_now.axis_y);
			scrollViewDraggingWithContentOffset(target_offset,MGPoint());
		}
    }
    void MIScrollView::indicatorInScroll(MGObject *_sender)
    {
        char pointer[128];
        sprintf(pointer, "%p", this);
        string str_pointer = pointer;
        MIView::beginAnimation("Scroll View Indicator In Scroll." + str_pointer);
        MIView::setAnimationDuration(0.1f);
        if (_scroll_view_flags.vertical_indicator_show == 1 && _vertical_indicator != NULL && _scroll_view_flags.vertical_indicator_always_show == 0) {
            _vertical_indicator->setAlpha(0.0f);
        }
        if (_scroll_view_flags.horizontal_indicator_show == 1 && _horizontal_indicator != NULL && _scroll_view_flags.horizontal_indicator_always_show == 0) {
            _horizontal_indicator->setAlpha(0.0f);
        }
        MIView::commitAnimation();
        scrolling = false;
        if (scroll_view_delegate != NULL)
        {
            scroll_view_delegate->scrollViewDidScroll(this);
            scroll_view_delegate->scrollViewDidEndScrollingAnimation(this);
        }
    }

#pragma mark - Touch event handle -
    
    //触摸事件的响应  //
    MIEventHandleState MIScrollView::touchesBegan(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        MIEventHandleState ret = MIEventHandleStateDone;
        return ret;
    }
    MIEventHandleState MIScrollView::touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        MIEventHandleState ret = MIEventHandleStateDone;
        MITouch* touch = *touches.begin();
        if(touch->maintained_view != this)
        {
            // subview had got the touch maintained, but it didn't handle it. scroll view should receive this touch and get maintained. //
            robTouchesAndEvent(touches, event);
            return ret;
        }

        // have another move touch, cancel long press judgment //
        if ((touch->tag != long_press_tag || touches.size() > 1) && long_press_tag != INVALID_TOUCH_TAG) {
            long_press_timer->invalidate();
            long_press_tag = INVALID_TOUCH_TAG;
        }else
        {
            TouchMIt t_it = touch_manager.find(touch->tag);
            if(t_it != touch_manager.end())
            {
                float dis = distanceOfTwoPoint(t_it->second.locationInView(this), touch->locationInView(this));
                if(dis > longpress_significant_move_valve_rate * (MGDevice::getScreenWidth() > 1 ? MGDevice::getScreenWidth() : 640.0f))
                {
                    long_press_timer->invalidate();
                    long_press_tag = INVALID_TOUCH_TAG;
                }
            }
        }
        
        if (passing_move) {
            ret = MIEventHandleStatePass;
        }
        
        return ret;
    }
    MIEventHandleState MIScrollView::touchesEnded(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        MIEventHandleState ret = MIEventHandleStateDone;
        MITouch* touch = *touches.begin();
        if(touch->maintained_view != this)
        {
            return ret;
        }
        
        for (std::set<MITouch*>::const_iterator it = touches.begin(); it != touches.end(); ++it) {
            if((*it)->tag == long_press_tag)
            {
                long_press_timer->invalidate();
                long_press_tag = INVALID_TOUCH_TAG;
                MIEvent* t_event = new MIEvent();
                t_event->_event_type = CONTROL_PURE_TOUCHES_EVENT;
                MITouch* touch = new MITouch(*it);
                touch->point = touch->screen_point;
                touch->phase = MITouchPhaseBegan;
                touch->maintained_view = selected_view;
                t_event->touches_in_event.insert(touch);
                GuiContainer::container->preserveEvent(t_event);
                //============================================//
                t_event = new MIEvent();
                t_event->_event_type = CONTROL_PURE_TOUCHES_DELAY_EVENT;
                GuiContainer::container->preserveEvent(t_event);
                //============================================//
                t_event = new MIEvent();
                t_event->_event_type = CONTROL_PURE_TOUCHES_EVENT;
                touch = new MITouch(*it);
                touch->point = touch->screen_point;
                touch->phase = MITouchPhaseEnded;
                touch->maintained_view = selected_view;
                t_event->touches_in_event.insert(touch);
                GuiContainer::container->preserveEvent(t_event);
                break;
            }
        }
        
        return ret;
    }
    MIEventHandleState MIScrollView::touchesCancelled(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        MIEventHandleState ret = MIEventHandleStateDone;
        MITouch* touch = *touches.begin();
        if(touch->maintained_view != this)
        {
            return ret;
        }
        
        for (std::set<MITouch*>::const_iterator it = touches.begin(); it != touches.end(); ++it) {
            if((*it)->tag == long_press_tag)
            {
                long_press_timer->invalidate();
                long_press_tag = INVALID_TOUCH_TAG;
                break;
            }
        }
        
        return ret;
    }
    //事件响应
    MIEventHandleState MIScrollView::handleEvent(const MIEvent* const event)
    {
        return MIView::handleEvent(event);
    }
    
#pragma mark - Drawing Setting -
    
    void MIScrollView::adjustSubTransitionInVertical(float value_in_vertical)
    {
        if(_scroll_view_flags.vertical_scroll_enabled == 0)
            return;
        MGRectTransition transition = sub_transition;
        transition.axis_y += value_in_vertical;
        float diff = 0.0f;
        if (value_in_vertical <= 0) { // Pull Up //
            diff = m_frame.height - displacement_size.bottom  - (transition.axis_y + ((m_content_size.bottom - (m_frame.height * 0.5 + sub_scale_p.y)) * sub_scale.axis_y + (m_frame.height * 0.5 + sub_scale_p.y)));
            if (((int)diff) >= (_scroll_view_flags.vertical_bounce_enabled == 1 ? scroll_view_scroll_bounce_value : 0)) {
                transition.axis_y += diff;
                if (scroll_view_delegate != NULL) {
                    scroll_view_delegate->scrollViewDidScrollOutofTop(this);
                }
            }
        } else { // Pull Down //
            diff = ((m_content_size.top - (m_frame.height * 0.5 + sub_scale_p.y)) * sub_scale.axis_y + (m_frame.height * 0.5 + sub_scale_p.y)) + transition.axis_y - displacement_size.top;
            if (((int)diff) >= (_scroll_view_flags.vertical_bounce_enabled == 1 ? scroll_view_scroll_bounce_value : 0)) {
                transition.axis_y -= diff;
                if (scroll_view_delegate != NULL) {
                    scroll_view_delegate->scrollViewDidScrollOutofBottom(this);
                }
            }
        }
        
        if (_scroll_view_flags.vertical_bounce_enabled == 1 && diff > start_bounce_value) {
            transition.axis_y = sub_transition.axis_y + value_in_vertical * (1 - (diff / scroll_view_scroll_bounce_value)) * scroll_damping_rate;
        }
        setSubRectTransition(transition);
    }
    void MIScrollView::adjustSubTransitionInHorizontal(float value_in_horizontal)
    {
        if(_scroll_view_flags.horizontal_scroll_enabled == 0)
            return;
        MGRectTransition transition = sub_transition;
        transition.axis_x += value_in_horizontal;
        float diff = 0.0f;
        if (value_in_horizontal <= 0) { // Pull Left //
            diff = m_frame.width - displacement_size.right - (transition.axis_x + ((m_content_size.right - (m_frame.width * 0.5 + sub_scale_p.x)) * sub_scale.axis_x + (m_frame.width * 0.5 + sub_scale_p.x)));
            if (((int)diff) >= (_scroll_view_flags.horizontal_bounce_enabled == 1 ? scroll_view_scroll_bounce_value : 0)) {
                transition.axis_x += diff;
                if (scroll_view_delegate != NULL) {
                    scroll_view_delegate->scrollViewDidScrollOutofLeft(this);
                }
            }
        } else { // Pull Right //
            diff = ((m_content_size.left - (m_frame.width * 0.5 + sub_scale_p.x)) * sub_scale.axis_x + (m_frame.width * 0.5 + sub_scale_p.x)) + transition.axis_x - displacement_size.left;
            if (((int)diff) >= (_scroll_view_flags.horizontal_bounce_enabled == 1 ? scroll_view_scroll_bounce_value : 0)) {
                transition.axis_x -= diff;
                if (scroll_view_delegate != NULL) {
                    scroll_view_delegate->scrollViewDidScrollOutofRight(this);
                }
            }
        }
        
        if (_scroll_view_flags.horizontal_bounce_enabled == 1 && diff > start_bounce_value) {
            transition.axis_x = sub_transition.axis_x + value_in_horizontal * (1 - (diff / scroll_view_scroll_bounce_value)) * scroll_damping_rate;
        }
        setSubRectTransition(transition);
    }
    //触摸事件 //
    MIView* MIScrollView::hitDetect(MITouch* touch, MIEvent *event, MITouchEventType& type)
    {
        AUTO_LOCK_THIS;
        MGPoint& point = touch->point;
        MIView* ret = NULL;
        if (m_scale.axis_x == 0 || m_scale.axis_y == 0 || _view_flags.user_interaction_enabled == 0) {
            return ret;
        }
        if (this->m_hidden == true || this->m_alpha < 0.1) {
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
            if (_view_flags.subview_enabled == 0 || scrolling) {
                if (can_handle_touch) {
                    ret = this;
                    touch_hitted = true;
                }
                touch->point = current_point;
                selected_view = NULL;
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
                selected_view = NULL;
            } else if ((touch->phase == MITouchPhaseBegan || type == MITouchEventBegan) && touch->maintained_view == NULL && (!ret->isObjectOfClass("MIScrollView"))){
                // scroll view specially handle, intercept judgment //
                // only handle with touch begin from user action not from z11 framework //
                if(touchesShouldBeginWithEventInContentView(touch, event, ret))
                {
                    if(startLongPressTimer(touch))
                    {
                        selected_view = ret;
                        tapping_point = touch->screen_point;
                        long_press_tag = touch->tag;
                    }
                }
                touch->point = current_point;
                ret->touch_hitted = false;
                if (can_handle_touch) {
                    ret = this;
                    touch_hitted = true;
                }
            }
        }
        return ret;
    }
    
    bool MIScrollView::startLongPressTimer(z11::MITouch *touch)
    {
        bool ret = false;
        do{
            if (long_press_tag != INVALID_TOUCH_TAG || touch_manager.size() > 0) {
                break;
            }
            long_press_timer->validate();
            ret = true;
        }while(0);
        return ret;
    }

    void MIScrollView::longPressEvent(z11::MGObject *sender)
    {
        {
            AUTO_LOCK_THIS;
            if (selected_view == NULL) {
                long_press_tag = INVALID_TOUCH_TAG;
                return;
            }
            TouchMIt t_it = touch_manager.find(long_press_tag);
            if(t_it != touch_manager.end())
                touch_manager.erase(t_it);
        }
        MGLogD("fire long press event.");
        MIEvent* event = new MIEvent();
        event->_event_type = CONTROL_REPLACE_TOUCHES_EVENT;
        TouchMIt it = touch_manager.find(long_press_tag);
        if (it != touch_manager.end()) {
            touch_manager.erase(it);
        }
        MITouch* touch = new MITouch();
        touch->tag = long_press_tag;
        touch->point = tapping_point;
        touch->screen_point = tapping_point;
        touch->phase = MITouchPhaseBegan;
        touch->maintained_view = selected_view;
        event->touches_in_event.insert(touch);
        GuiContainer::container->preserveEvent(event);
        long_press_tag = INVALID_TOUCH_TAG;
    }

    //false截取，true不截取 //
    bool MIScrollView::touchesShouldBeginWithEventInContentView(z11::MITouch *touch, z11::MIEvent *event, MIView* content_view)
    {
        return true;
    }

    void MIScrollView::drawself()
    {
        MIView::drawself();
    }
    
    void MIScrollView::drawAfterSubviews()
    {
        MIView::drawAfterSubviews();
        if (_scroll_view_flags.vertical_indicator_show == 1 && _vertical_indicator != NULL) {
            _vertical_indicator->draw();
        }
        if (_scroll_view_flags.horizontal_indicator_show == 1 && _horizontal_indicator != NULL) {
            _horizontal_indicator->draw();
        }
    }

    void MIScrollView::removeAllSubviews()
    {
        selected_view = NULL;
        MIView::removeAllSubviews();
        m_content_size.left = 0;
        m_content_size.top = 0;
        m_content_size.right = (long)m_frame.width;
        m_content_size.bottom = (long)m_frame.height;
    }
    void MIScrollView::includeNewSubview(z11::MIView *subview)
    {
        MGRect rect = subview->getFrame();
        if(rect.x + rect.width > m_content_size.right)
        {
            m_content_size.right = (long)(rect.x + rect.width);
        }
        if(rect.y + rect.height > m_content_size.bottom)
        {
            m_content_size.bottom = (long)(rect.y + rect.height);
        }
        if (rect.x < m_content_size.left) {
            m_content_size.left = (long)rect.x;
        }
        if (rect.y < m_content_size.top) {
            m_content_size.top = (long)rect.y;
        }
    }
    void MIScrollView::excludeOldSubview(z11::MIView *subview)
    {
        AUTO_LOCK_THIS;
        SubviewIterator it;
        if (selected_view == subview) {
            selected_view = NULL;
        }
        m_content_size.top = 0;
        m_content_size.left = 0;
        m_content_size.bottom = m_frame.height;
        m_content_size.right = m_frame.width;
        for (it = m_subviews.begin(); it != m_subviews.end(); ++it) {
            assert((*it) != NULL);
            MGRect rect = (*it)->getFrame();
            if (rect.x < m_content_size.left) {
                m_content_size.left = (long)rect.x;
            }
            if (rect.y < m_content_size.top) {
                m_content_size.top = (long)rect.y;
            }
            if (rect.x + rect.width > m_content_size.right) {
                m_content_size.right = (long)(rect.x + rect.width);
            }
            if (rect.y + rect.height > m_content_size.bottom) {
                m_content_size.bottom =(long)(rect.y + rect.height) ;
            }
        }
    }

	void MIScrollView::scrollViewDraggingWithContentOffset( const MGPoint &targetContentOffset,const MGPoint &velocity )
	{

	}
    
#pragma mark - Gesture Handle -
    
    void MIScrollView::panCall( MGObject *_sender )
	{
        if (long_press_timer->isValid()) {
            long_press_timer->invalidate();
        }
        if (_scroll_view_flags.vertical_scroll_enabled == 0 && _scroll_view_flags.horizontal_scroll_enabled == 0) {
            return;
        }
        
		MIPanGestureRecognizer *recognizer=dynamic_cast<MIPanGestureRecognizer *>(_sender);
		if (recognizer!=NULL)
		{
            if (recognizer->state() == MIGestureRecognizerStateEnded || recognizer->state() == MIGestureRecognizerStateCancelled) {
                if (!passing_move) {
                    scrolling = true;
                    if (_scroll_view_flags.inertia_enable == 1)
                        inertiaInScroll(this);
                    else
                        bounceInScroll(this);
                }
                passing_move = false;
                inertia_speed_in_axis_x = 0;
                inertia_speed_in_axis_y = 0;
                momentum_time_x = 0;
                momentum_time_y = 0;
                momentum_orientation_x = 0;
                momentum_orientation_y = 0;
                if (_scroll_view_flags.hide_keyboard_when_scrolling) {
                    MIEvent *_event = new MIEvent;
                    _event->_event_type = RESIGN_FIRST_RESPONDER_EVENT;
                    GuiContainer::container->preserveEvent(_event);
                }
                return;
            }
            
            MGPoint trans = recognizer->translationInView(this);
            if (recognizer->state() == MIGestureRecognizerStateBegan) {
                passing_move = false;
                if (scroll_view_delegate != NULL) {
                    scroll_view_delegate->scrollViewWillBeginDragging(this);
                }
                if (_scroll_view_flags.passing_horizontal_scroll_enable == 1) {
                    if (fabs(trans.y) < fabs(trans.x)) {
                        passing_move = true;
                        return;
                    }
                }
                if (_scroll_view_flags.passing_vertical_scroll_enable == 1) {
                    if (fabs(trans.x) < fabs(trans.y)) {
                        passing_move = true;
                        return;
                    }
                }
            }
            
            if (passing_move) {
                return;
            }
            
			{
                adjustSubTransitionInHorizontal(trans.x);
				adjustSubTransitionInVertical(trans.y);
                
                if (scroll_view_delegate != NULL) {
                    scroll_view_delegate->scrollViewDidScroll(this);
                }
                
                MGTimeDelta delta = recognizer->timeDelta();
                caculateInertiaWithDisAndTimeInHorizontal(trans.x, delta.InSecondsF());
                caculateInertiaWithDisAndTimeInVertical(trans.y, delta.InSecondsF());
                
//                MGLogD("move \t%.2f \tpix in \t%.4lf \tsecond.", trans.y, delta.InSecondsF());
                
				if (_scroll_view_flags.vertical_indicator_show == 1 && _vertical_indicator != NULL) {
					_vertical_indicator->setAlpha(1.0f);
				}
				if (_scroll_view_flags.horizontal_indicator_show == 1 && _horizontal_indicator != NULL) {
					_horizontal_indicator->setAlpha(1.0f);
				}
			}
		}
	}
    
	void MIScrollView::zoomCall( MGObject *_sender )
	{
        if (long_press_timer->isValid()) {
            long_press_timer->invalidate();
        }
        if (_scroll_view_flags.zoom_enable == 0 || _view_flags.sub_scale_enable == 0) {
            return;
        }
		MIPinchGestureRecognizer *recognizer=dynamic_cast<MIPinchGestureRecognizer *>(_sender);
		if (recognizer!=NULL)
		{
            float scale_value = recognizer->getScale();
            MGPoint pinch_center = recognizer->getPinchCenter(this);
            
            if (recognizer->state() == MIGestureRecognizerStateEnded || recognizer->state() == MIGestureRecognizerStateCancelled) {
                m_pinch_center = pinch_center;
                bounceInZoom(this);
                inertia_speed_in_axis_x = 0;
                inertia_speed_in_axis_y = 0;
                momentum_time_x = 0;
                momentum_time_y = 0;
                momentum_orientation_x = 0;
                momentum_orientation_y = 0;
                return;
            }
            
            if (recognizer->state() == MIGestureRecognizerStateBegan) {
                m_pinch_center = pinch_center;
            }
            float diff_x = pinch_center.x - m_pinch_center.x;
            float diff_y = pinch_center.y - m_pinch_center.y;
            adjustSubTransitionInHorizontal(diff_x);
            adjustSubTransitionInVertical(diff_y);
            
            MGPoint curr_center_point = pinch_center;
            translateSubPoint(curr_center_point);
            setSubRectScale(MGRectScaleMake(sub_scale.axis_x * scale_value, sub_scale.axis_y * scale_value,1));
            
            detranslateSubPoint(curr_center_point);
            adjustSubTransitionInHorizontal(pinch_center.x - curr_center_point.x);
            adjustSubTransitionInVertical(pinch_center.y - curr_center_point.y);
			m_pinch_center = pinch_center;
            
            if (scroll_view_delegate != NULL) {
                if (recognizer->state() == MIGestureRecognizerStateEnded || recognizer->state() == MIGestureRecognizerStateCancelled) {
                    scroll_view_delegate->scrollViewDidEndZooming(this);
                }else if (recognizer->state() == MIGestureRecognizerStateBegan)
                    scroll_view_delegate->scrollViewWillBeginZooming(this);
                else
                    scroll_view_delegate->scrollViewDidEndZooming(this);
            }
            
            if (_scroll_view_flags.vertical_indicator_show == 1 && _vertical_indicator != NULL) {
                _vertical_indicator->setAlpha(1.0f);
            }
            if (_scroll_view_flags.horizontal_indicator_show == 1 && _horizontal_indicator != NULL) {
                _horizontal_indicator->setAlpha(1.0f);
            }
		}
		
	}

	void MIScrollView::setScrollBounceValue(float bounce_value)
	{
		scroll_view_scroll_bounce_value=bounce_value;
	}
    
    void MIScrollView::setStartBounceValue(float value)
    {
        start_bounce_value = value;
    }
    
    void MIScrollView::setScrollBouncingRate(float value)
    {
        scroll_damping_rate = value;
    }

#pragma mark - MIScrollViewDelegate -
    void MIScrollViewDelegate::scrollViewDidScroll(MIScrollView *scrollView)
    {
        
    }
    void MIScrollViewDelegate::scrollViewDidZoom(MIScrollView *scrollView)
    {
        
    }
    void MIScrollViewDelegate::scrollViewWillBeginDragging(MIScrollView *scrollView)
    {
        
    }
    void MIScrollViewDelegate::scrollViewWillEndDraggingWithVelocityTargetContentOffset(MIScrollView *scrollView, MGPoint velocity, MGPoint &targetContentOffset)
    {
        
    }
    void MIScrollViewDelegate::scrollViewDidEndDraggingWillDecelerate(MIScrollView *scrollView, bool decelerate)
    {
        
    }
    void MIScrollViewDelegate::scrollViewWillBeginDecelerating(MIScrollView *scrollView)
    {
        
    }
    void MIScrollViewDelegate::scrollViewDidEndDecelerating(MIScrollView *scrollView)
    {
        
    }
    void MIScrollViewDelegate::scrollViewDidEndScrollingAnimation(MIScrollView *scrollView)
    {
        
    }
//    MIView* MIScrollViewDelegate::viewForZoomingInScrollView(MIScrollView *scrollView)
//    {
//        MIView* ret = NULL;
//        return ret;
//    }
//    void MIScrollViewDelegate::scrollViewWillBeginZoomingWithView(MIScrollView *scrollView, MIView *view)
//    {
//        
//    }
//    void MIScrollViewDelegate::scrollViewDidEndZoomingWithViewAtScale(MIScrollView *scrollView, MIView *view, float scale)
//    {
//        
//    }
    void MIScrollViewDelegate::scrollViewDidEndZooming(MIScrollView *scrollView)
    {
        
    }
    void MIScrollViewDelegate::scrollViewWillBeginZooming(MIScrollView *scrollView)
    {
        
    }
    bool MIScrollViewDelegate::scrollViewShouldScrollToTop(MIScrollView *scrollView)
    {
        bool ret = true;
        return ret;
    }
    void MIScrollViewDelegate::scrollViewDidScrollToTop(MIScrollView *scrollView)
    {
        
    }
    void MIScrollViewDelegate::scrollViewDidScrollOutofLeft(MIScrollView *scrollView)
    {
        
    }
    void MIScrollViewDelegate::scrollViewDidScrollOutofRight(MIScrollView *scrollView)
    {
        
    }
    
    void MIScrollViewDelegate::scrollViewDidScrollOutofTop(MIScrollView *scrollView)
    {
        
    }
    void MIScrollViewDelegate::scrollViewDidScrollOutofBottom(MIScrollView *scrollView)
    {
        
    }
} //namespace z11
