//
//  MKMapScrollView.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-15.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "GUI/Base/MIView.h"
#include "System/Shared/MGTime.h"

namespace z11
{
	class MKMapScrollView;
    class MITapGestureRecognizer;
    class MGTimer;
    
	class MKMapScrollViewDelegate
	{
	public:
		virtual void scrollViewDidScroll(MKMapScrollView *scrollView)
		{
			
		}
		virtual void scrollViewWillBeginDragging(MKMapScrollView *scrollView)
		{

		}
		virtual void  scrollViewWillEndDraggingWithVelocityTargetContentOffset(
			MKMapScrollView *scrollView,
			const MGPoint& velocity,
			MGPoint &targetContentOffset)
		{

		}
		virtual void scrollViewDidEndDraggingWillDecelerate(MKMapScrollView *scrollView,bool decelerate)
		{

		}
		virtual bool scrollViewShouldScrollToTop(MKMapScrollView *scrollView)
		{
			return true;
		}
		virtual void scrollViewDidScrollToTop(MKMapScrollView *scrollView)
		{

		}
		virtual void  scrollViewWillBeginDecelerating(MKMapScrollView *scrollView)
		{

		}
		virtual void  scrollViewDidEndDecelerating(MKMapScrollView *scrollView)
		{

		}

		virtual MIView* viewForZoomingInScrollView(MKMapScrollView *scrollView)
		{
			return NULL;
		}
		virtual void scrollViewWillBeginZoomingWithView(MKMapScrollView *scrollView,MIView *view)
		{

		}
		virtual void scrollViewDidEndZoomingWithViewAtScale(MKMapScrollView *scrollView,MIView *view,float scale)
		{

		}
		virtual void scrollViewDidZoom(MKMapScrollView *scrollView)
		{

		}

		virtual void scrollViewDidEndScrollingAnimation(MKMapScrollView *scrollView)
		{

		}

		//@depreciate method//
		virtual void scrollViewTaped(MKMapScrollView* scrollView, MITapGestureRecognizer* gesture)
		{
			
		}
	};
    typedef enum {
        MKMapScrollViewIndicatorStyleDefault,     // black with white border. good against any background
        MKMapScrollViewIndicatorStyleBlack,       // black only. smaller. good against a white background
        MKMapScrollViewIndicatorStyleWhite        // white only. smaller. good against a black background
    } MKMapScrollViewIndicatorStyle;

    class MIImageView;
	class MGTimer;
	class MIPinchGestureRecognizer;
	class MIPanGestureRecognizer;
    class MITapGestureRecognizer;
    class MILongPressGestureRecognizer;
    class MISwipeGestureRecognizer;
    class MIRotationGestureRecognizer;
    class MKMapScrollView : public MIView
    {
    public:
        MKMapScrollView();
        virtual ~MKMapScrollView();
        virtual void initWithFrame(MGRect frame);
        virtual void frameChanged(const MGRect& frame);
        virtual void subRectTransitionChanged(const MGRectTransition& _transition);
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
            //scroll inertia//
            byte_8 inertia_enable:1;
            // passing direction scroll //
            byte_8 passing_vertical_scroll_enable:1;
            byte_8 passing_horizontal_scroll_enable:1;
        } _scroll_view_flags;
        
    public:
        virtual MIView* hitDetect(MITouch* touch, MIEvent *event, MITouchEventType& type);
    protected:
        //返回false，则触摸被scroll view截取，返回true，则触摸由hitDetect的结果控件处理 //
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
        void adjustIndicatorInVertical();
        void adjustIndicatorInHorizontal();
    public:
        // Animation sequence //
        void inertiaInScroll(MGObject* _sender);
        void bounceInScroll(MGObject* _sender);
        void indicatorInScroll(MGObject* _sender);
        //view drawing//
    protected:
        virtual void drawself();
        virtual void drawBeforeSubviews();
        virtual void drawAfterSubviews();
        virtual void removeAllSubviews();
        virtual void includeNewSubview(MIView* subview);
        virtual void excludeOldSubview(MIView* subview);
        virtual void scrollViewDraggingWithContentOffset(const MGPoint &targetContentOffset,const MGPoint &velocity);
    protected:
        MGRect2 m_content_size;
    public:
        MGRect2 displacement_size;
    public:
        MIRESPONDER_TOUCH_DECLARE;
    protected:
        bool m_moved;
		float maximumZoomScale;
		float minimumZoomScale;
	public:
		MKMapScrollViewDelegate  *map_view_delegate;
		MIPinchGestureRecognizer *m_pinch_gesture;
		MIPanGestureRecognizer *m_pan_gesture;
        MITapGestureRecognizer *m_single_tap_gesture;
        MITapGestureRecognizer *m_double_tap_gesture;
        MITapGestureRecognizer *m_pair_tap_gesture;
        MILongPressGestureRecognizer *m_long_press_gesture;
        //MISwipeGestureRecognizer* m_swipe_gesture;
        MIRotationGestureRecognizer* m_rotation_gesture;

    public:
        // gestures
        void zoomScale(float zoom_scale_value, bool animated, float animation_duration);
    protected:
        void zoomScaleDidStop(MGObject* _sender);
        void scrollCall(MGObject *_sender);
		void zoomCall(MGObject *_sender);
        void tapCall(MGObject *_sender);
        void longPressCall(MGObject *_sender);
        //void swipeCall(MGObject *_sender);
        void rotationCall(MGObject *_sender);
        
    private:
        MGTimer* m_zooming_timer;
        void zoomingRefresh(MGObject* _sender);
    };
} //namespace z11