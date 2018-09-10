//
//  MKMapScrollView.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-15.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MKMapScrollView.h"
#include "GUI/MIControls/MIImageView.h"
#include "GraphicCore/MGImage.h"
#include "System/Timer/MGTimer.h"
#include "System/Shared/MGLog.h"
#include "GUI/GuiContainer/GuiContainer.h"
#include "GUI/GestureRecognizer/MIPinchGestureRecognizer.h"
#include "GUI/GestureRecognizer/MIPanGestureRecognizer.h"
#include "GUI/GestureRecognizer/MITapGestureRecognizer.h"
#include "GUI/GestureRecognizer/MILongPressGestureRecognizer.h"
#include "GUI/GestureRecognizer/MISwipeGestureRecognizer.h"
#include "GUI/GestureRecognizer/MIRotationGestureRecognizer.h"
#include "MapKit/Fetcher/Pyramid/MKGlobalMapTiles.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include "MKTileMapView.h"
#include "MKMapView.h"
#define SCROLL_VIEW_LONG_PRESS_VALVE 0.2f

#define SCROLL_VIEW_SCROLL_BOUNCE_VALVE 180.0f
#define SCROLL_VIEW_SCROLL_DAMPING_RATE 0.3f // 滚动到开头结尾的bouncing阻尼系数 //
#define SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_WIDTH 6.0f
#define SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER 2.0f // 默认滚动条与scroll view边缘的间隔 //
#define SCROLL_VIEW_SCROLL_INERTIA_SPEED_RATE 5.0f // 滚动惯性时通过触摸获取的速度值的调整系数 //
#define SCROLL_VIEW_SCROLL_INERTIA_SPEED_MAX_VALVE 200.0f // 滚动惯性初始速度的最大阀值 //
#define SCROLL_VIEW_SCROLL_INERTIA_SPEED_CACULATE_RATE 10.0f // 滚动惯性初始速度计算调整系数 //
#define SCROLL_VIEW_SCROLL_INERTIA_SPEED_CACULATE_VALVE 40.0f // 惯性计算累计阀值 //

#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288   /* pi */
#endif

namespace z11
{
    extern int z_min;
    extern int z_max;
    extern int z_base;
    
    MKMapScrollView::MKMapScrollView()
	{
        class_name = "MKMapScrollView";
        //初始化计时器 //
        long_press_tag = INVALID_TOUCH_TAG;
        MIEvent long_press_event;
        long_press_event._event_type = SCROLL_VIEW_LONG_PRESS_EVENT;
        long_press_event._content = NULL;
        long_press_event._responder = this;
        SELHandler selector;
        selector.sender = this;
        selector.sel_func = (SELFunc)&MKMapScrollView::longPressEvent;
        long_press_event._handler = selector;
        long_press_timer = MGTimer::timerWithTimeInterval(MGTimeDelta::FromSecondsF(SCROLL_VIEW_LONG_PRESS_VALVE), long_press_event, false);
        sub_rotate = MGRectRotateMake(0, 0, 0, 0);
        sub_transition = MGRectTransitionMake(0, 0, 0);
        sub_scale = MGRectScaleMake(1,1,1);
        //
        selected_view = NULL;
        // TODO: initial flags
        // MIView flags
        _view_flags.clip_subviews = 0;
        _view_flags.user_multitouch_subview_enabled = 1;
        // MKMapScrollView flags
        _view_flags.sub_transition_enable = 1;
        _view_flags.sub_scale_enable = 0;
        _view_flags.sub_rotate_enable = 0;
        _scroll_view_flags.vertical_bounce_enabled = 1;
        _scroll_view_flags.horizontal_bounce_enabled = 1;
        _scroll_view_flags.vertical_indicator_show = 1;
        _scroll_view_flags.horizontal_indicator_show = 1;
        _scroll_view_flags.inertia_enable = 1;
        _scroll_view_flags.zoom_enable = 1;
        _scroll_view_flags.vertical_scroll_enabled = 1;
        _scroll_view_flags.horizontal_scroll_enabled = 1;
        _scroll_view_flags.passing_vertical_scroll_enable = 0;
        _scroll_view_flags.passing_horizontal_scroll_enable = 0;
        // //
        _vertical_indicator = NULL;
        _horizontal_indicator = NULL;

		//
		m_did_after_inertia_inScroll.sender = this;
		m_did_after_inertia_inScroll.sel_func = (SELFunc)&MKMapScrollView::bounceInScroll;
        //
        momentum_orientation_x = 0;
        momentum_orientation_y = 0;
        momentum_time_x = 1;
        momentum_time_y = 1;
        need_momentum = false;

		//
		delegate=NULL;
        
        // gesture
        m_single_tap_gesture = NULL;
        m_double_tap_gesture = NULL;
        m_pair_tap_gesture = NULL;
        m_pinch_gesture = NULL;
        m_pan_gesture = NULL;
        m_long_press_gesture = NULL;
        //m_swipe_gesture = NULL;
        m_rotation_gesture = NULL;
        
        m_moved = false;
		minimumZoomScale=1/(1<<30);
		maximumZoomScale=1<<30;
        
        MIEvent zooming_event;
        zooming_event._responder = this;
        zooming_event._handler.sender = this;
        zooming_event._handler.sel_func = (SELFunc)&MKMapScrollView::zoomingRefresh;
        m_zooming_timer = MGTimer::timerWithTimeInterval(MGTimeDelta::FromSecondsF(0.05f), zooming_event, true);
    }
    
    MKMapScrollView::~MKMapScrollView()
    {
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
        if (m_single_tap_gesture != NULL) {
            delete_and_set_null(m_single_tap_gesture);
        }
        if (m_double_tap_gesture != NULL) {
            delete_and_set_null(m_double_tap_gesture);
        }
        if (m_pair_tap_gesture != NULL) {
            delete_and_set_null(m_pair_tap_gesture);
        }
        if (m_pinch_gesture != NULL) {
            delete_and_set_null(m_pinch_gesture);
        }
        if (m_long_press_gesture != NULL) {
            delete_and_set_null(m_long_press_gesture);
        }
//        if (m_swipe_gesture != NULL) {
//            delete_and_set_null(m_swipe_gesture);
//        }
        if (m_rotation_gesture != NULL) {
            delete_and_set_null(m_rotation_gesture);
        }
        if(m_zooming_timer != NULL)
        {
            m_zooming_timer->invalidate();
            delete_and_set_null(m_zooming_timer);
        }
    }
    void MKMapScrollView::initWithFrame(MGRect frame)
    {
        MIView::initWithFrame(frame);
        m_content_size.right = (long)frame.width;
        m_content_size.bottom = (long)frame.height;
        if (_vertical_indicator != NULL || _horizontal_indicator != NULL) {
            return;
        }
        if (_scroll_view_flags.vertical_indicator_show == 1) {
            _vertical_indicator = new MIImageView();
            _vertical_indicator->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::lightBlack()));
            _vertical_indicator->view_round_mode = RoundRectAll;
            _vertical_indicator->view_round_rate = SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_WIDTH * 0.5f;
            _vertical_indicator->setAlpha(0.0f);
            float x_value = frame.width - SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_WIDTH - SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
            float height_value = frame.height - (2 * SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER);
            float y_value = SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
            if ((m_content_size.bottom - m_content_size.top) > frame.height) {
                y_value += height_value * (m_content_size.top - sub_transition.axis_y) / (m_content_size.bottom - m_content_size.top);
                height_value = height_value * frame.height / (m_content_size.bottom - m_content_size.top);
                if (y_value < SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER) {
                    height_value = height_value * (1 - (sub_transition.axis_y / SCROLL_VIEW_SCROLL_BOUNCE_VALVE));
                    y_value = SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
                } else if (y_value + height_value > (m_frame.height - (2 * SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER))) {
                    height_value = height_value * (1 - ((m_frame.height - sub_transition.axis_y - m_content_size.bottom + m_content_size.top) / SCROLL_VIEW_SCROLL_BOUNCE_VALVE));
                    y_value = m_frame.height - height_value - SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
                }
            }
            _vertical_indicator->initWithFrame(MGRectMake(x_value + m_frame.x, y_value + m_frame.y, SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_WIDTH, height_value));
        }
        if (_scroll_view_flags.horizontal_indicator_show == 1) {
            _horizontal_indicator = new MIImageView();
            _horizontal_indicator->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::lightBlack()));
            _horizontal_indicator->view_round_mode = RoundRectAll;
            _horizontal_indicator->view_round_rate = SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_WIDTH * 0.5f;
            _horizontal_indicator->setAlpha(0.0f);
            float y_value = frame.height - SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_WIDTH - SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
            float width_value = frame.width - (2 * SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER);
            float x_value = SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
            if ((m_content_size.right - m_content_size.left) > frame.width) {
                x_value = width_value * (m_content_size.left - sub_transition.axis_x) / (m_content_size.right - m_content_size.left);
                width_value = width_value * frame.width / (m_content_size.right - m_content_size.left);
                if (x_value < SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER) {
                    width_value = width_value * (1 - (sub_transition.axis_x / SCROLL_VIEW_SCROLL_BOUNCE_VALVE));
                    x_value = SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
                } else if (x_value + width_value > (m_frame.width - (2 * SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER))) {
                    width_value = width_value * (1 - ((m_frame.width - sub_transition.axis_x - m_content_size.right + m_content_size.left) / SCROLL_VIEW_SCROLL_BOUNCE_VALVE));
                    x_value = m_frame.width - width_value - SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
                }
            }
            _horizontal_indicator->initWithFrame(MGRectMake(x_value + m_frame.x, y_value + m_frame.y, width_value, SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_WIDTH));
        }

		//Gesture recognizer
		m_pan_gesture = new MIPanGestureRecognizer();
		m_pinch_gesture = new MIPinchGestureRecognizer();
        m_single_tap_gesture = new MITapGestureRecognizer();
        m_double_tap_gesture = new MITapGestureRecognizer();
        m_pair_tap_gesture = new MITapGestureRecognizer();
        m_long_press_gesture = new MILongPressGestureRecognizer();
        //m_swipe_gesture = new MISwipeGestureRecognizer();
        m_rotation_gesture = new MIRotationGestureRecognizer();
        
        m_double_tap_gesture->numberOfTapsRequired = 2;
        m_pair_tap_gesture->numberOfTouchesRequired = 2;

		SELHandler SEL_handler;
		SEL_handler.sender=m_pinch_gesture;
		SEL_handler.sel_func=(SELFunc)&MKMapScrollView::zoomCall;
		m_pinch_gesture->initWithTarget(this,SEL_handler);
		addGestureRecognizer(m_pinch_gesture);

		SEL_handler.sender=m_pan_gesture;
		SEL_handler.sel_func=(SELFunc)&MKMapScrollView::scrollCall;
		m_pan_gesture->initWithTarget(this,SEL_handler);
		addGestureRecognizer(m_pan_gesture);
        
        SEL_handler.sender=m_single_tap_gesture;
		SEL_handler.sel_func=(SELFunc)&MKMapScrollView::tapCall;
		m_single_tap_gesture->initWithTarget(this,SEL_handler);
		addGestureRecognizer(m_single_tap_gesture);
        
        SEL_handler.sender=m_double_tap_gesture;
		SEL_handler.sel_func=(SELFunc)&MKMapScrollView::tapCall;
		m_double_tap_gesture->initWithTarget(this,SEL_handler);
		addGestureRecognizer(m_double_tap_gesture);
        
        SEL_handler.sender=m_pair_tap_gesture;
		SEL_handler.sel_func=(SELFunc)&MKMapScrollView::tapCall;
		m_pair_tap_gesture->initWithTarget(this,SEL_handler);
		addGestureRecognizer(m_pair_tap_gesture);
        
        m_single_tap_gesture->requireGestureRecognizerToFail(m_pair_tap_gesture);
        m_single_tap_gesture->requireGestureRecognizerToFail(m_double_tap_gesture);
        
        SEL_handler.sender=m_long_press_gesture;
		SEL_handler.sel_func=(SELFunc)&MKMapScrollView::longPressCall;
		m_long_press_gesture->initWithTarget(this,SEL_handler);
		addGestureRecognizer(m_long_press_gesture);
        
        m_single_tap_gesture->requireGestureRecognizerToFail(m_long_press_gesture);
        
//        SEL_handler.sender=m_swipe_gesture;
//		SEL_handler.sel_func=(SELFunc)&MKMapScrollView::swipeCall;
//		m_swipe_gesture->initWithTarget(this,SEL_handler);
//		addGestureRecognizer(m_swipe_gesture);
//        
//        m_pan_gesture->requireGestureRecognizerToFail(m_swipe_gesture);
        
        SEL_handler.sender=m_rotation_gesture;
		SEL_handler.sel_func=(SELFunc)&MKMapScrollView::rotationCall;
		m_rotation_gesture->initWithTarget(this,SEL_handler);
		addGestureRecognizer(m_rotation_gesture);

    }

    void MKMapScrollView::adjustIndicatorInVertical()
    {
        if (_scroll_view_flags.vertical_indicator_show == 0 || _vertical_indicator == NULL) {
            return;
        }
        float x_value = m_frame.width - SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_WIDTH - SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
        float height_value = m_frame.height - (2 * SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER);
        float y_value = SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
        if ((m_content_size.bottom - m_content_size.top) > m_frame.height) {
            y_value = height_value * (m_content_size.top - sub_transition.axis_y) / (m_content_size.bottom - m_content_size.top);
            height_value = height_value * m_frame.height / (m_content_size.bottom - m_content_size.top);
            if (y_value < SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER) {
                height_value = height_value * (1 - (sub_transition.axis_y / SCROLL_VIEW_SCROLL_BOUNCE_VALVE));
                y_value = SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
            } else if (y_value + height_value > (m_frame.height - (2 * SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER))) {
                height_value = height_value * (1 - ((m_frame.height - sub_transition.axis_y - m_content_size.bottom + m_content_size.top) / SCROLL_VIEW_SCROLL_BOUNCE_VALVE));
                y_value = m_frame.height - height_value - SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
            }
        }
        _vertical_indicator->setFrame(MGRectMake(x_value + m_frame.x, y_value + m_frame.y,  SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_WIDTH, height_value));
    }

    void MKMapScrollView::adjustIndicatorInHorizontal()
    {
        if (_scroll_view_flags.horizontal_indicator_show == 0 || _horizontal_indicator == NULL) {
            return;
        }
        float y_value = m_frame.height - SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_WIDTH - SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
        float width_value = m_frame.width - (2 * SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER);
        float x_value = SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
        if ((m_content_size.right - m_content_size.left) > m_frame.width) {
            x_value = width_value * (m_content_size.left - sub_transition.axis_x) / (m_content_size.right - m_content_size.left);
            width_value = width_value * m_frame.width / (m_content_size.right - m_content_size.left);
            if (x_value < SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER) {
                width_value = width_value * (1 - (sub_transition.axis_x / SCROLL_VIEW_SCROLL_BOUNCE_VALVE));
                x_value = SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
            } else if (x_value + width_value > m_frame.width) {
                width_value = width_value * (1 - ((m_frame.width - sub_transition.axis_x - m_content_size.right + m_content_size.left) / SCROLL_VIEW_SCROLL_BOUNCE_VALVE));
                x_value = m_frame.width - width_value - SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_BORDER;
            }
        }
        _horizontal_indicator->setFrame(MGRectMake(x_value + m_frame.x, y_value + m_frame.y, width_value, SCROLL_VIEW_SCROLL_INDICATOR_DEFAULT_WIDTH));
    }

    void MKMapScrollView::frameChanged(const MGRect &frame)
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
    void MKMapScrollView::subRectTransitionChanged(const MGRectTransition &_transition)
    {
        adjustIndicatorInVertical();
        adjustIndicatorInHorizontal();
    }
    
    // caculate inertia //
    void MKMapScrollView::caculateInertiaWithDisAndTimeInVertical(float dis, double time)
    {
        if (dis < SCROLL_VIEW_SCROLL_INERTIA_SPEED_CACULATE_VALVE && dis > -SCROLL_VIEW_SCROLL_INERTIA_SPEED_CACULATE_VALVE) {
            inertia_speed_in_axis_y = 0;
            momentum_time_y = 0;
            momentum_orientation_y = 0;
            return;
        }
        if((dis < 0 && momentum_orientation_y > 0) || (dis > 0 && momentum_orientation_y < 0))
        {
            inertia_speed_in_axis_y = 0;
            momentum_time_y = 0;
            momentum_orientation_y = 0;
        }
        momentum_orientation_y += dis;
        momentum_time_y += time;
        if (momentum_time_y == 0) {
            return;
        }
        inertia_speed_in_axis_y = momentum_orientation_y / momentum_time_y;
    }
    
    void MKMapScrollView::caculateInertiaWithDisAndTimeInHorizontal(float dis, double time)
    {
        if (dis < SCROLL_VIEW_SCROLL_INERTIA_SPEED_CACULATE_VALVE && dis > -SCROLL_VIEW_SCROLL_INERTIA_SPEED_CACULATE_VALVE) {
            inertia_speed_in_axis_x = 0;
            momentum_time_x = 0;
            momentum_orientation_x = 0;
            return;
        }
        if((dis < 0 && momentum_orientation_x > 0) || (dis > 0 && momentum_orientation_x < 0))
        {
            inertia_speed_in_axis_x = 0;
            momentum_time_x = 0;
            momentum_orientation_x = 0;
        }
        momentum_orientation_x += dis;
        momentum_time_x += time;
        if (momentum_time_x == 0) {
            return;
        }
        inertia_speed_in_axis_x = momentum_orientation_x / momentum_time_x;
    }
    
    //Scroll Animation squence//
    void MKMapScrollView::inertiaInScroll(MGObject *_sender)
    {
        float duration = 0.0f;
        float duration_x = 0.0f;
        float duration_y = 0.0f;
        MGRectTransition transition_now =  sub_transition;
        if (_scroll_view_flags.horizontal_bounce_enabled == 1) {
            if (fabs(inertia_speed_in_axis_x) > SCROLL_VIEW_SCROLL_INERTIA_SPEED_MAX_VALVE) {
                inertia_speed_in_axis_x = inertia_speed_in_axis_x > 0 ? SCROLL_VIEW_SCROLL_INERTIA_SPEED_MAX_VALVE : - SCROLL_VIEW_SCROLL_INERTIA_SPEED_MAX_VALVE;
            }
            transition_now.axis_x += inertia_speed_in_axis_x * SCROLL_VIEW_SCROLL_INERTIA_SPEED_RATE;
            duration_x = inertia_speed_in_axis_x * 0.005f;
        }
        if (_scroll_view_flags.vertical_bounce_enabled == 1) {
            if (fabs(inertia_speed_in_axis_y) > SCROLL_VIEW_SCROLL_INERTIA_SPEED_MAX_VALVE) {
                inertia_speed_in_axis_y = inertia_speed_in_axis_y > 0 ? SCROLL_VIEW_SCROLL_INERTIA_SPEED_MAX_VALVE : - SCROLL_VIEW_SCROLL_INERTIA_SPEED_MAX_VALVE;
            }
            transition_now.axis_y += inertia_speed_in_axis_y * SCROLL_VIEW_SCROLL_INERTIA_SPEED_RATE;
            duration_y = inertia_speed_in_axis_y * 0.005f;
        }
        if (transition_now.axis_x > m_content_size.left + SCROLL_VIEW_SCROLL_BOUNCE_VALVE + displacement_size.left) {
            //
            duration_x -= (transition_now.axis_x - (m_content_size.left + displacement_size.left)) / 1000;
            transition_now.axis_x = m_content_size.left + (SCROLL_VIEW_SCROLL_BOUNCE_VALVE * 0.5f) + displacement_size.left;
        }
        if (m_frame.width - transition_now.axis_x > m_content_size.right + SCROLL_VIEW_SCROLL_BOUNCE_VALVE + displacement_size.right) {
            //
            duration_x += (m_frame.width - transition_now.axis_x - (m_content_size.right + displacement_size.right)) / 1000;
            transition_now.axis_x = m_frame.width - m_content_size.right - (SCROLL_VIEW_SCROLL_BOUNCE_VALVE * 0.5f) - displacement_size.right;
        }
        if (transition_now.axis_y > m_content_size.top + SCROLL_VIEW_SCROLL_BOUNCE_VALVE + displacement_size.top) {
            //
            MGLogD("duration_y is : %f", duration_y);
            duration_y -= (transition_now.axis_y - (m_content_size.top + displacement_size.top)) / 1000;
            MGLogD("after duration_y is : %f", duration_y);
            transition_now.axis_y = m_content_size.top + (SCROLL_VIEW_SCROLL_BOUNCE_VALVE * 0.5f) + displacement_size.top;
        }
        if (m_frame.height - transition_now.axis_y > m_content_size.bottom + SCROLL_VIEW_SCROLL_BOUNCE_VALVE + displacement_size.bottom) {
            //
            MGLogD("duration_y is : %f", duration_y);
            duration_y += (m_frame.height - transition_now.axis_y - (m_content_size.bottom + displacement_size.bottom)) / 1000;
            MGLogD("after duration_y is : %f", duration_y);
            transition_now.axis_y = m_frame.height - m_content_size.bottom - (SCROLL_VIEW_SCROLL_BOUNCE_VALVE * 0.5f) - displacement_size.bottom;
        }
        duration = sqrt((duration_x * duration_x) + (duration_y * duration_y));
        if (duration > 1.0f) {
            duration = 1.0f;
        }
        char pointer[128];
        sprintf(pointer, "%p", this);
        string str_pointer = pointer;
        MIView::beginAnimation("Scroll View Inertia In Scroll." + str_pointer);
        MIView::setAnimationDuration(duration);
        MIView::setAnimationCurve(MIViewAnimationOptionCurveMomentum);
        setSubRectTransition(transition_now);
        //SELHandler selector;
        //selector.sender = this;
        //selector.sel_func = (SELFunc)&MIScrollView::bounceInScroll;
        MIView::setAnimationDidStopSelector(this,m_did_after_inertia_inScroll);
		MIView::commitAnimation();
    }
    void MKMapScrollView::bounceInScroll(MGObject* _sender)
    {
        MGRectTransition transition_now =  sub_transition;
        if (sub_transition.axis_x > m_content_size.left + displacement_size.left) {
            //
            transition_now.axis_x = m_content_size.left + displacement_size.left;
        }
        if (m_frame.width - sub_transition.axis_x > m_content_size.right + displacement_size.right) {
            //
            transition_now.axis_x = m_frame.width - m_content_size.right - displacement_size.right;
        }
        if (sub_transition.axis_y > m_content_size.top + displacement_size.top) {
            //
            transition_now.axis_y = m_content_size.top + displacement_size.top;
        }
        if (m_frame.height - sub_transition.axis_y > m_content_size.bottom + displacement_size.bottom) {
            //
            transition_now.axis_y = m_frame.height - m_content_size.bottom - displacement_size.bottom;
        }
        float duration = 0.1f;
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
        selector.sel_func = (SELFunc)&MKMapScrollView::indicatorInScroll;
        MIView::setAnimationDidStopSelector(this,selector);
        MIView::commitAnimation();
    }
    void MKMapScrollView::indicatorInScroll(MGObject *_sender)
    {
        char pointer[128];
        sprintf(pointer, "%p", this);
        string str_pointer = pointer;
        MIView::beginAnimation("Scroll View Indicator In Scroll." + str_pointer);
        MIView::setAnimationDuration(0.1f);
        if (_scroll_view_flags.vertical_indicator_show == 1 && _vertical_indicator != NULL) {
            _vertical_indicator->setAlpha(0.0f);
        }
        if (_scroll_view_flags.horizontal_indicator_show == 1 && _horizontal_indicator != NULL) {
            _horizontal_indicator->setAlpha(0.0f);
        }
        MIView::commitAnimation();
    }

//#pragma mark -Touch event handle
    
    //触摸事件的响应  //
    MIEventHandleState MKMapScrollView::touchesBegan(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        return MIView::touchesBegan(touches, event);
    }
    MIEventHandleState MKMapScrollView::touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		if ((*touches.begin())->maintained_view != this) {
            robTouchesAndEvent(touches, event);
        }
		return MIView::touchesMoved(touches, event);
    }
    MIEventHandleState MKMapScrollView::touchesEnded(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
        return MIView::touchesEnded(touches, event);
    }
    MIEventHandleState MKMapScrollView::touchesCancelled(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        return MIView::touchesCancelled(touches, event);
    }
    //事件响应
    MIEventHandleState MKMapScrollView::handleEvent(const MIEvent* const event)
    {
        return MIView::handleEvent(event);
    }
    void MKMapScrollView::adjustSubTransitionInVertical(float value_in_vertical)
    {
        float diff = 0.0f;
        if (value_in_vertical <= 0) { // Pull Up //
            diff = m_frame.height - value_in_vertical - m_content_size.bottom - displacement_size.bottom;
        } else { // Pull Down //
            diff = value_in_vertical - m_content_size.top - displacement_size.top;
        }
        if (diff >= (_scroll_view_flags.vertical_bounce_enabled == 1 ? SCROLL_VIEW_SCROLL_BOUNCE_VALVE : 0)) {
            return;
        }
        MGRectTransition transition = sub_transition;
        if (_scroll_view_flags.vertical_bounce_enabled == 1 && diff > 0) {
            transition.axis_y += (value_in_vertical - sub_transition.axis_y) * (1 - (diff / SCROLL_VIEW_SCROLL_BOUNCE_VALVE)) * SCROLL_VIEW_SCROLL_DAMPING_RATE;
        } else {
            transition.axis_y = value_in_vertical;
        }
        setSubRectTransition(transition);
    }
    void MKMapScrollView::adjustSubTransitionInHorizontal(float value_in_horizontal)
    {
        float diff = 0.0f;
        if (value_in_horizontal <= 0) { // Pull Left //
            diff = m_frame.width - value_in_horizontal - m_content_size.right + displacement_size.right;
        } else { // Pull Right //
            diff = value_in_horizontal - m_content_size.left - displacement_size.left;
        }
        if (diff >= (_scroll_view_flags.horizontal_bounce_enabled == 1 ? SCROLL_VIEW_SCROLL_BOUNCE_VALVE : 0)) {
            return;
        }
        MGRectTransition transition = sub_transition;
        if (_scroll_view_flags.horizontal_bounce_enabled == 1 && diff > 0) {
            transition.axis_x += (value_in_horizontal - sub_transition.axis_x) * (1 - (diff / SCROLL_VIEW_SCROLL_BOUNCE_VALVE)) * SCROLL_VIEW_SCROLL_DAMPING_RATE;
        } else {
            transition.axis_x = value_in_horizontal;
        }
        setSubRectTransition(transition);
    }
    //触摸事件 //
    MIView* MKMapScrollView::hitDetect(MITouch* touch, MIEvent *event, MITouchEventType& type)
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
    
    bool MKMapScrollView::startLongPressTimer(z11::MITouch *touch)
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
    
    void MKMapScrollView::longPressEvent(z11::MGObject *sender)
    {
        {
            AUTO_LOCK_THIS;
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
    bool MKMapScrollView::touchesShouldBeginWithEventInContentView(z11::MITouch *touch, z11::MIEvent *event, MIView* content_view)
    {
        return true;
    }

    void MKMapScrollView::drawself()
    {
        MIView::drawself();
    }
    void MKMapScrollView::drawBeforeSubviews()
    {
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        if (graphic_engine == NULL) {
            return;
        }
		graphic_engine->beginDraw();
		if (_view_flags.sub_scale_enable == 1) {
			graphic_engine->scale(MGPointMake((m_bounds.width / 2) + m_scale_p.x, (m_bounds.height / 2) + m_scale_p.y),
                                  sub_scale);
		}
        if (_view_flags.sub_transition_enable == 1) {
            graphic_engine->transition(sub_transition);
		}
		//if (_scroll_view_flags.sub_scale_enable == 1) {
		//	graphic_engine->scale(MGPointMake(-sub_transition.axis_x + (m_bounds.width / 2) + m_scale_p.x, -sub_transition.axis_y + (m_bounds.height / 2) + m_scale_p.y),
		//		sub_scale);
		//}
        if (_view_flags.sub_rotate_enable == 1) {
            graphic_engine->rotate(sub_rotate_p, sub_rotate);
        }
    }
    void MKMapScrollView::drawAfterSubviews()
    {
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        assert(graphic_engine != NULL);
        graphic_engine->endDraw();
        if (_scroll_view_flags.vertical_indicator_show == 1 && _vertical_indicator != NULL) {
            _vertical_indicator->draw();
        }
        if (_scroll_view_flags.horizontal_indicator_show == 1 && _horizontal_indicator != NULL) {
            _horizontal_indicator->draw();
        }
    }

    void MKMapScrollView::removeAllSubviews()
    {
        selected_view = NULL;
        MIView::removeAllSubviews();
        m_content_size.left = 0;
        m_content_size.top = 0;
        m_content_size.right = (long)m_frame.width;
        m_content_size.bottom = (long)m_frame.height;
    }
    void MKMapScrollView::includeNewSubview(z11::MIView *subview)
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
    void MKMapScrollView::excludeOldSubview(z11::MIView *subview)
    {
        AUTO_LOCK_THIS;
        SubviewIterator it;
        if (selected_view == subview) {
            selected_view = NULL;
        }
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
    
    void MKMapScrollView::scrollViewDraggingWithContentOffset( const MGPoint &targetContentOffset,const MGPoint &velocity )
	{
        
	}
	static z11::MGLongPoint convertCoordinateToPoint( const MKLocationCoordinate2D &coord )
	{
		GlobalMercator mercator;
		MGDoublePoint m=mercator.LatLonToMeters(coord.latitude,coord.longitude);
		MGLongPoint p=mercator.MetersToPixels(m.x,m.y,z_base);
		p.y= (1<<(z_base+8))-p.y;
		return p;
	}
	void MKMapScrollView::scrollCall( MGObject *_sender )
	{
		//static int step = 0;
		//MGLogD("Map Scroll %d.", step++);
		MIPanGestureRecognizer *recognizer=dynamic_cast<MIPanGestureRecognizer *>(_sender);
		if (recognizer!=NULL)
		{
            if (recognizer->state() == MIGestureRecognizerStateEnded ||
                recognizer->state() == MIGestureRecognizerStateCancelled)
            {
                m_moved = false;
            }
            else
                m_moved = true;
            
            MGRectScale sub_scale = this->getSubRectScale();
			MGPoint trans = recognizer->translationInView(this);
            MGPoint offset;
            
			trans.x /= sub_scale.axis_x;
			trans.y /= sub_scale.axis_y;
            
			if (true)
			{
				float dis_in_x = trans.x;
				float dis_in_y = trans.y;
				float value_in_horizontal = sub_transition.axis_x + dis_in_x;
				float value_in_vertical = sub_transition.axis_y + dis_in_y;

				MKTileMapView* tile_map = dynamic_cast<MKTileMapView *>(this);
				if(tile_map != NULL)
				{
					MKMapView *map_view=tile_map->getParentMapView();
					const MKCoordinateRegion & region = map_view->getMapBounds();
					MGLongPoint lt=convertCoordinateToPoint(
						MKLocationCoordinate2D(
						region.center.latitude+region.span.latitudeDelta,
						region.center.longitude-region.span.longitudeDelta));
					MGLongPoint rb=convertCoordinateToPoint(
						MKLocationCoordinate2D(
						region.center.latitude-region.span.latitudeDelta,
						region.center.longitude+region.span.longitudeDelta));

                    // get value_in_horizontal
                    float margin_in_x = (sub_scale.axis_x-1.0f)*m_frame.width/sub_scale.axis_x/2.0f;
                    if(dis_in_x > 0.0f)
                    {
                        if(value_in_horizontal > margin_in_x-lt.x)
                        {
                            value_in_horizontal = margin_in_x-lt.x;
                        }
                    }
                    else if(dis_in_x < 0.0f)
                    {
                        if(value_in_horizontal < m_frame.width - margin_in_x - rb.x)
                        {
                            value_in_horizontal = m_frame.width - margin_in_x - rb.x;
                        }
                    }
                    
                    // get value_in_vertical
                    float margin_in_y = (sub_scale.axis_y-1.0f)*m_frame.height/sub_scale.axis_y/2.0f;
                    if(dis_in_y > 0.0f)
                    {
                        if(value_in_vertical > margin_in_y-lt.y)
                        {
                            value_in_vertical = margin_in_y-lt.y;
                        }
                    }
                    else if(dis_in_y < 0.0f)
                    {
                        if(value_in_vertical < m_frame.height - margin_in_y - rb.y)
                        {
                            value_in_vertical = m_frame.height - margin_in_y - rb.y;
                        }
                    }
                }
				
                offset.x = (value_in_horizontal-sub_transition.axis_x)*sub_scale.axis_x;
                offset.y = (value_in_vertical-sub_transition.axis_y)*sub_scale.axis_y;
				
				adjustSubTransitionInVertical(value_in_vertical);
				adjustSubTransitionInHorizontal(value_in_horizontal);
                
				if (_scroll_view_flags.vertical_indicator_show == 1 && _vertical_indicator != NULL) {
					_vertical_indicator->setAlpha(1.0f);
				}
				if (_scroll_view_flags.horizontal_indicator_show == 1 && _horizontal_indicator != NULL) {
					_horizontal_indicator->setAlpha(1.0f);
				}
			}
			
			if (map_view_delegate!=NULL)
			{
				map_view_delegate->scrollViewWillEndDraggingWithVelocityTargetContentOffset(this, MGPoint(), offset);
			}
            if (map_view_delegate!=NULL)
			{
				map_view_delegate->scrollViewDidEndDraggingWillDecelerate(this,true);
			}
		}
	}

	void MKMapScrollView::zoomCall( MGObject *_sender )
	{
		MIPinchGestureRecognizer *recognizer=dynamic_cast<MIPinchGestureRecognizer *>(_sender);
		if (recognizer != NULL)
		{
			float scale_value = recognizer->getScale();
			zoomScale(scale_value, false, 0.2);
		}
	}
    
    void MKMapScrollView::zoomScale(float zoom_scale_value, bool animated, float animation_duration)
    {
        if(sub_scale.axis_x * zoom_scale_value < minimumZoomScale ||
           sub_scale.axis_x * zoom_scale_value > maximumZoomScale)
        {
            return;
        }
        
        if(animated)
        {
            char animation_name[256];
            sprintf(animation_name, "tap_animation: %p", this);
            MIView::beginAnimation(animation_name);
            MIView::setAnimationCurve(MIViewAnimationOptionCurveLinear);
            MIView::setAnimationDuration(animation_duration);
            setSubRectScale(MGRectScaleMake(sub_scale.axis_x * zoom_scale_value, sub_scale.axis_y * zoom_scale_value, 1));
            SELHandler handler;
            handler.sel_func = (SELFunc)&MKMapScrollView::zoomScaleDidStop;
            handler.sender = this;
            MIView::setAnimationDidStopSelector(this, handler);
            MIView::commitAnimation();
            
            m_zooming_timer->validate();
        }
        else
        {
            setSubRectScale(MGRectScaleMake(sub_scale.axis_x * zoom_scale_value, sub_scale.axis_y * zoom_scale_value, 1));
            zoomScaleDidStop(NULL);
        }
    }
    
    
    void MKMapScrollView::zoomingRefresh(MGObject* _sender)
    {
        if (map_view_delegate != NULL)
        {
            map_view_delegate->scrollViewWillBeginZoomingWithView(this, this);
        }
        if (map_view_delegate != NULL)
        {
            float scale_value = getSubRectScale().axis_x;
            map_view_delegate->scrollViewDidEndZoomingWithViewAtScale(this, this, scale_value);
        }
    }
    
    void MKMapScrollView::zoomScaleDidStop(MGObject* _sender)
    {
        zoomingRefresh(_sender);
        m_zooming_timer->invalidate();
    }
    
    void MKMapScrollView::tapCall( MGObject *_sender )
	{
		MITapGestureRecognizer *recognizer = dynamic_cast<MITapGestureRecognizer *>(_sender);
		if (recognizer != NULL)
		{
            if (recognizer->numberOfTapsRequired == 2 && recognizer->numberOfTouchesRequired == 1)
            {
                zoomScale(2.0f, true, 0.2f);
            }
            else if (recognizer->numberOfTapsRequired == 1 && recognizer->numberOfTouchesRequired == 2)
            {
                zoomScale(0.5f, true, 0.2f);
			}
            
            if (map_view_delegate != NULL)
            {
                map_view_delegate->scrollViewTaped(this, recognizer);
            }
		}
	}
    
    void MKMapScrollView::longPressCall( MGObject *_sender )
	{
		MILongPressGestureRecognizer *recognizer=dynamic_cast<MILongPressGestureRecognizer *>(_sender);
		if (recognizer!=NULL)
		{
			MKTileMapView *tile_map=dynamic_cast<MKTileMapView *>(this);
			if (tile_map!=NULL)
			{
                MKMapView* parent_view = tile_map->getParentMapView();
                if (parent_view != NULL && parent_view->delegate != NULL) {
                    parent_view->delegate->mapViewLongPressed(parent_view,recognizer);
                }
			}
			
            MGLogD("longPressCall");
		}
	}
    
//    void MKMapScrollView::swipeCall( MGObject *_sender )
//	{
//		MISwipeGestureRecognizer *recognizer=dynamic_cast<MISwipeGestureRecognizer *>(_sender);
//		if (recognizer!=NULL && recognizer->state() == MIGestureRecognizerStateBegan)
//		{
//            MGLogD("swipeCall, %d.", recognizer->direction);
//		}
//		
//	}
    
    void MKMapScrollView::rotationCall( MGObject *_sender )
	{
		MIRotationGestureRecognizer *recognizer=dynamic_cast<MIRotationGestureRecognizer *>(_sender);
		if (recognizer!=NULL)
		{
            MGLogD("rotationCall, %f.", recognizer->getRotation() / M_PI * 180);
		}
	}
} //namespace z11
