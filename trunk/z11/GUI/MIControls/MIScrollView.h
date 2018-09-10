//
//  MIScrollView.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-15.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "../Base/MIView.h"
#include "../../System/Shared/MGTime.h"
#include <map>

using namespace std;

namespace z11
{
    typedef enum {
        MIScrollViewIndicatorStyleDefault,     // black with white border. good against any background
        MIScrollViewIndicatorStyleBlack,       // black only. smaller. good against a white background
        MIScrollViewIndicatorStyleWhite        // white only. smaller. good against a black background
    } MIScrollViewIndicatorStyle;

    class MIImageView;
    class MGTimer;
    class MIPanGestureRecognizer;
    class MIPinchGestureRecognizer;
    class MIScrollViewDelegate;
    class MIScrollView : public MIView
    {
    public:
        MIScrollView();
        virtual ~MIScrollView();
        virtual void initWithFrame(MGRect frame);
        virtual void frameChanged(const MGRect& frame);
        virtual void subRectTransitionChanged(const MGRectTransition& _transition);
        virtual void subRectScaleChanged(const MGRectScale& _scale);
    public:
        volatile struct {
            byte_8 vertical_scroll_enabled:1;
            byte_8 vertical_bounce_enabled:1;
            byte_8 vertical_bouncing:1;
            byte_8 horizontal_scroll_enabled:1;
            byte_8 horizontal_bounce_enabled:1;
            byte_8 horizontal_bouncing:1;
            byte_8 zoom_enable:1;
            byte_8 zoom_bouncing:1;
            byte_8 always_bounce_horizontal:1;
            byte_8 always_bounce_vertical:1;
            //scroll indicator//
            byte_8 vertical_indicator_show:1;
            byte_8 horizontal_indicator_show:1;
            byte_8 vertical_indicator_always_show:1;
            byte_8 horizontal_indicator_always_show:1;
            //scroll inertia//
            byte_8 inertia_enable:1;
            byte_8 decelarate_enable:1;
            // passing direction scroll //
            byte_8 passing_vertical_scroll_enable:1;
            byte_8 passing_horizontal_scroll_enable:1;
            // keyboard displaying //
            byte_8 hide_keyboard_when_scrolling:1;
        } _scroll_view_flags;
        
    public:
        virtual MIView* hitDetect(MITouch* touch, MIEvent *event, MITouchEventType& type);
    protected:
        //返回false，则触摸被scroll view截取，返回true，则启动长触摸定时器以判断是否选中了subview //
        virtual bool touchesShouldBeginWithEventInContentView(MITouch* touch, MIEvent* event, MIView* content_view);
        //Scroll //
    protected:
        void adjustSubTransitionInVertical(float vertical);
        void adjustSubTransitionInHorizontal(float horizontal);
    protected:
        MGTimer* long_press_timer;
        void longPressEvent(MGObject* sender);
        MIView* selected_view;
    protected:
        // =====触摸跟踪处理===== //
        // 函数 //
        bool startLongPressTimer(MITouch* touch);
    protected:
        // 变量 //
        MITouchTag long_press_tag;
        MGPoint tapping_point;
        // ==================== //
        // 用于精确计算惯性的速度值 //
    private:
        float momentum_orientation_x;
        double momentum_time_x;
        float momentum_orientation_y;
        double momentum_time_y;
        bool need_momentum; // 是否需要执行惯性动力 //
        bool scrolling;
    protected:
        // inertia scroll support //
        float inertia_speed_in_axis_x;
        float inertia_speed_in_axis_y;
        void caculateInertiaWithDisAndTimeInVertical(float dis, double time);
        void caculateInertiaWithDisAndTimeInHorizontal(float dis, double time);
		SELHandler m_did_after_inertia_inScroll;
        //indicator//
    protected:
        // 这两个indicator不放入ScrollView的subviews，但是会在drawAfterSubviews()内引用 //
        MIImageView* _horizontal_indicator;
        MIImageView* _vertical_indicator;
    public:
        void enableHorizontalIndicator(){_scroll_view_flags.horizontal_indicator_show = 1;}
        void disableHorizontalIndicator(){_scroll_view_flags.horizontal_indicator_show = 0;}
        void enableVerticalIndicator(){_scroll_view_flags.vertical_indicator_show = 1;}
        void disableVerticalIndicator(){_scroll_view_flags.vertical_indicator_show = 0;}
        void alwaysShowHorizontalIndicator(){_scroll_view_flags.horizontal_indicator_always_show = 1;}
        void showHorizontalIndicatorInNeed(){_scroll_view_flags.horizontal_indicator_always_show = 0;}
        void alwaysShowVerticalIndicator(){_scroll_view_flags.vertical_indicator_always_show = 1;}
        void showVerticalIndicatorInNeed(){_scroll_view_flags.vertical_indicator_always_show = 0;}
        void adjustIndicatorInVertical();
        void adjustIndicatorInHorizontal();
    public:
        // Animation sequence //
        void inertiaInScroll(MGObject* _sender);
        void bounceInScroll(MGObject* _sender);
        void indicatorInScroll(MGObject* _sender);
        void bounceInZoom(MGObject* _sender);
        
        void setStartBounceValue(float value);
        void setScrollBouncingRate(float value);
        //view drawing//
    protected:
        virtual void drawself();
        virtual void drawAfterSubviews();
        virtual void removeAllSubviews();
        virtual void includeNewSubview(MIView* subview);
        virtual void excludeOldSubview(MIView* subview);
		virtual void scrollViewDraggingWithContentOffset(const MGPoint &targetContentOffset,const MGPoint &velocity);
    protected:
        MGRect2 m_content_size;
        float m_max_zoom_scale;
        float m_min_zoom_scale;
        float start_bounce_value;
        float scroll_damping_rate;
    public:
        MGRect2 displacement_size;
        void setMaxZoomScale(const float& max_scale){m_max_zoom_scale = max_scale > 1.0f ? max_scale : 1.0f;}
    public:
        static float longpress_significant_move_valve_rate; // 长按时的显著移动阀值比率 //
    public:
        MIRESPONDER_TOUCH_DECLARE;
    protected:
        void panCall(MGObject *_sender);
		void zoomCall(MGObject *_sender);
    protected:
        // 手势识别代替touch事件处理的分析来操作scroll view //
        MIPanGestureRecognizer *m_pan_gesture;
        MIPinchGestureRecognizer *m_pinch_gesture;
        MGPoint m_pinch_center;
    private:
        bool passing_move;
    public:
        MIScrollViewDelegate* scroll_view_delegate;
	protected:
		float scroll_view_scroll_bounce_value;
	public:
		void setScrollBounceValue(float bounce_value);
        MGPROPERTY(float, scrollInertiaSpeedValue, ScrollInertiaSpeedValue);
        MGPROPERTY(float, scrollInertiaDurationRate, ScrollInertiaDurationRate);
    };
    
    class MIScrollViewDelegate
    {
    public:
        virtual void scrollViewDidScroll(MIScrollView *view);                                               // any offset changes
        virtual void scrollViewDidZoom(MIScrollView *scrollView); // any zoom scale changes
    
    // called on start of dragging (may require some time and or distance to move)
        virtual void scrollViewWillBeginDragging(MIScrollView *scrollView);
    // called on finger up if the user dragged. velocity is in points/second. targetContentOffset may be changed to adjust where the scroll view comes to rest. not called when pagingEnabled is YES
        virtual void scrollViewWillEndDraggingWithVelocityTargetContentOffset(MIScrollView *scrollView, MGPoint velocity, MGPoint &targetContentOffset);
    // called on finger up if the user dragged. decelerate is true if it will continue moving afterwards
        virtual void scrollViewDidEndDraggingWillDecelerate(MIScrollView *scrollView, bool decelerate);
    
        virtual void scrollViewWillBeginDecelerating(MIScrollView *scrollView);   // called on finger up as we are moving
        virtual void scrollViewDidEndDecelerating(MIScrollView *scrollView);      // called when scroll view grinds to a halt
    
        virtual void scrollViewDidEndScrollingAnimation(MIScrollView *scrollView); // called when setContentOffset/scrollRectVisible:animated: finishes. not called if not animating
    
//        virtual MIView *viewForZoomingInScrollView(MIScrollView *scrollView);     // return a view that will be scaled. if delegate returns nil, nothing happens
//        virtual void scrollViewWillBeginZoomingWithView(MIScrollView *scrollView, MIView *view); // called before the scroll view begins zooming its content
//        virtual void scrollViewDidEndZoomingWithViewAtScale(MIScrollView *scrollView, MIView *view, float scale); // scale between minimum and maximum. called after any 'bounce' animations
        
        virtual void scrollViewWillBeginZooming(MIScrollView *scrollView);
        
        virtual void scrollViewDidEndZooming(MIScrollView *scrollView);
    
        virtual bool scrollViewShouldScrollToTop(MIScrollView *scrollView);   // return a yes if you want to scroll to the top. if not defined, assumes YES
        virtual void scrollViewDidScrollToTop(MIScrollView *scrollView);      // called when scrolling animation finished. may be called immediately if already at top
        
        virtual void scrollViewDidScrollOutofLeft(MIScrollView *scrollView);    // called when scroll out of the left bound of content size.
        virtual void scrollViewDidScrollOutofRight(MIScrollView *scrollView);   // called when scroll out of the right bound of content size.
        virtual void scrollViewDidScrollOutofTop(MIScrollView *scrollView);   // called when scroll out of the top bound of content size.
        virtual void scrollViewDidScrollOutofBottom(MIScrollView *scrollView);   // called when scroll out of the bottom bound of content size.
    
    };
} //namespace z11