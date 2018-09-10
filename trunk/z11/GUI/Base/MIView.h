//
//  MIView.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-21.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

/*
 所有可见控件和可见页面的基类
 */
#pragma once

#include "MIResponder.h"
#include "../../GraphicCore/MGColor.h"
#include "../../CommonBase/MGStructure.h"
#include "../../GraphicCore/MGGraphicEngine.h"
#include <deque>
#include "MIDataTypeDef.h"
#include "../../System/Shared/Syncable.h"
#include "../../CommonBase/CommonDef.h"
namespace z11 {
    
    class MIViewTreeInfo {
    public:
        MIViewTreeInfo();
        ~MIViewTreeInfo();
    public:
        string toUITree(uint_32 level = 0);
    public:
        string class_name;
        string text;
        string extra_content;
        MGRect bound;
        string background;
        MGRectTransition transition;
        MGRectTransition sub_transition;
        std::deque<MIViewTreeInfo*> subviews;
    };

    enum {
        //animation curve //
        MIViewAnimationOptionCurveLinear = 0, // default
        MIViewAnimationOptionCurveLinearSwing,
        MIViewAnimationOptionCurveEaseIn,
        MIViewAnimationOptionCurveEaseOut,
        MIViewAnimationOptionCurveEaseInOut,
        MIViewAnimationOptionCurveFastInOut,
        MIViewAnimationOptionCurveBounce,
		MIViewAnimationOptionCurveDamping,
		MIViewAnimationOptionCurveDampingOnce,
        MIViewAnimationOptionCurveInertia,
        MIViewAnimationOptionCurveMomentum,
		MIViewAnimationOptionCurvePendulum,
        MIViewAnimationOptionCurveCursor,

        //animation type //
        MIViewAnimationOptionTransitionNone = 0 << 16, // default
        MIViewAnimationOptionTransitionFlipFromLeft = 1 << 16,
        MIViewAnimationOptionTransitionFlipFromRight = 2 << 16,
        MIViewAnimationOptionTransitionCurlUp = 3 << 16,
        MIViewAnimationOptionTransitionCurlDown = 4 << 16,
        MIViewAnimationOptionShowHideTransitionViews = 5 << 16,
        MIViewAnimationOptionScaleTransitionViews = 6 << 16,
		MIViewAnimationOptionTransitionFlipFromDown = 7 << 16,
		MIViewAnimationOptionTransitionFlipFromUp = 8 << 16,
		MIViewAnimationOptionRotateOutTransitionViews = 9 << 16,
		MIViewAnimationOptionRotateInTransitionViews = 10 << 16,
		MIViewAnimationOptionTransitionFlipFromLeftIniOS7Type = 11 << 16,
		MIViewAnimationOptionTransitionFlipFromRightIniOS7Type = 12 << 16

    };
    typedef uint_32 MIViewAnimationOptions;

	class MIGestureRecognizer;
    class MIViewController;
    class MITouchTracer;
    class MIAnimationsManager;
    class MIAnimationLayer;
    class MIAnimationAlgorithm;

    class MIView : public MIResponder,public Syncable
    {
    public:
        MIView();
		//~MIView 会自动删除m_subView;
        virtual ~MIView() = 0;

    //视图基础 //
        virtual void initWithFrame(MGRect frame);

    //位置相关 //
    public:
        void setCenter(const MGPoint& center);
        void setFrame(const MGRect& frame);
        void animateCenter(const MGPoint& center);
        void animateFrame(const MGRect& frame);
        void setAniCenter(const MGPoint& center);
        void setAniFrame(const MGRect& frame);
        MGPoint getCenter(){return m_center;}
        MGRect getFrame(){return m_frame;}
		MGColor *getBackgroundColor();
        MGRectRotate getRectRotate(){return m_rotate;}
        MGRectTransition getRectTransition(){return m_transition;}
        MGRectScale getRectScale(){return m_scale;}
        MGRectRotate getSubRectRotate(){return sub_rotate;}
        MGRectTransition getSubRectTransition(){return sub_transition;}
        MGRectScale getSubRectScale(){return sub_scale;}
        void setRectRotate(MGRectRotate _rotate);
        void setRectTransition(MGRectTransition _transition);
        void setRectScale(MGRectScale _scale);
        void setSubRectRotate(MGRectRotate _rotate);
        void setSubRectTransition(MGRectTransition _transition);
        void setSubRectScale(MGRectScale _scale);
        void setRectRotatePoint(MGPoint point);
        void setRectScalePoint(MGPoint point);
        void setSubRectRotatePoint(MGPoint point);
        void setSubRectScalePoint(MGPoint point);
        void animateRectRotate(MGRectRotate _rotate);
        void animateRectTransition(MGRectTransition _transition);
        void animateRectScale(MGRectScale _scale);
        void animateSubRectRotate(MGRectRotate _rotate);
        void animateSubRectTransition(MGRectTransition _transition);
        void animateSubRectScale(MGRectScale _scale);
        void animateRectRotatePoint(MGPoint point);
        void animateRectScalePoint(MGPoint point);
        void animateSubRectRotatePoint(MGPoint point);
        void animateSubRectScalePoint(MGPoint point);
        void setAniRectRotate(MGRectRotate _rotate);
        void setAniRectTransition(MGRectTransition _transition);
        void setAniRectScale(MGRectScale _scale);
        void setAniSubRectRotate(MGRectRotate _rotate);
        void setAniSubRectTransition(MGRectTransition _transition);
        void setAniSubRectScale(MGRectScale _scale);
        void setAniRectRotatePoint(MGPoint point);
        void setAniRectScalePoint(MGPoint point);
        void setAniSubRectRotatePoint(MGPoint point);
        void setAniSubRectScalePoint(MGPoint point);
		const MGRect& getBounds(){return m_bounds;}
    protected:
        void _setCenter(const MGPoint& center);
        void _setFrame(const MGRect& frame);
        virtual void frameChanged(const MGRect& frame);
        virtual void centerChanged(const MGPoint& center);
        virtual void rectTransitionChanged(const MGRectTransition& _transition);
        virtual void rectScaleChanged(const MGRectScale& _scale);
        virtual void rectRotateChanged(const MGRectRotate& _rotate);
        virtual void subRectTransitionChanged(const MGRectTransition& _transition);
        virtual void subRectScaleChanged(const MGRectScale& _scale);
        virtual void subRectRotateChanged(const MGRectRotate& _rotate);
    protected:
        MGPoint m_center;   //中心坐标，相对于根View的坐标，绝对坐标 //
        MGRect m_frame;     //用于保留应有的位置信息，相对于父View的坐标，相对坐标//
        MGRect m_bounds;    //用于绘制时获取位置信息，HitDetect时也用这个位置信息，相对于根View的坐标，绝对坐标 //
        MGRectRotate m_rotate; //在三维空间中的三轴角度 //
        MGRectTransition m_transition; //在三维空间中的位移量 //
        MGRectScale m_scale; //在三维空间中的缩放向量 //
        MGRectTransition sub_transition;
        MGRectScale sub_scale;
        MGRectRotate sub_rotate;
        MGPoint m_rotate_p; //在三维空间中的旋转中心偏移点 //
        MGPoint m_scale_p; //在三维空间中的缩放中心偏移点 //
        MGPoint sub_rotate_p; //在三维空间中的旋转中心偏移点 //
        MGPoint sub_scale_p; //在三维空间中的缩放中心偏移点 //
        bool m_need_sub_translate;
    public:
        bool touch_hitted; // 在触摸检测阶段，有触摸击中 //
        bool m_have_touches_through_subview; // 任意子控件上是否有触摸发生 //
        bool can_handle_touch_from_superview; // 父View是否没有任何触摸透过，或者支持多子控件触摸 //
        bool can_handle_touch; // 是否能够处理touch //
        static float significant_move_valve_rate;   // 显著移动阀值比率 //
    public:
    // ViewTags
        struct {
            byte_8 view_initialized:1;
            byte_8 user_interaction_enabled:1; //不接受用户得操作，比如触摸和点击 //
            byte_8 clip_subviews:1; //对子视图进行裁剪，子视图不能超出父视图的绘制区域 //
            //view drawing//
            byte_8 sub_transition_enable:1;
            byte_8 sub_scale_enable:1;
            byte_8 sub_rotate_enable:1;
            byte_8 user_multitouch_enabled:1; // 是否支持多点触摸，默认打开，具体的影响要看控件自身的处理代码 //
            byte_8 user_multitouch_subview_enabled:1; // 是否支持多点触摸子控件，比如同时触发多个按钮响应，默认关闭，具体的影响要看控件自身的处理代码 //
            // subview enabled //
            byte_8 subview_enabled:1;
        } _view_flags;

    //视图结构 //
    public:
        /*从父View脱离，不会调用自身的析构*/
        void detachFromSuperview();
        void addSubview(MIView* subview);
        void moveSubviewToIndex(MIView* subview, uint_32 index);
        void moveSubviewToBack(MIView* subview);
        void insertSubviewAtIndex(MIView* subview, uint_32 index);
        MIView *subviewAtIndex(uint_32 index);
        /*将指定View从当前View删除，如果指定View自动释放，则返回NULL，否则返回为指定View*/
        MIView* removeSubview(MIView* subview);
        /*将指定位置的View从当前View中脱离，如果指定View为自动释放，则返回NULL，否则返回指定位置的View*/
        MIView *detachSubviewAtIndex(uint_32 index);
        MIView *subviewWithTag(const string& tag, int_32 index);
        MIView *subviewWithTagRecur(const string& tag);
        MIViewTreeInfo* viewTree();
        virtual void removeAllSubviews();
        virtual void includeNewSubview(MIView* subview);
        virtual void excludeOldSubview(MIView* subview);
        bool containedSubview(MIView* subview);
	public:
		std::deque<MIView*> m_subviews;    //子Views，没有重复的子View public,因为要让controller 操作  //
		MIView *parent()
		{
			return m_superview;
		}
    protected:
        MIView *m_superview;    //父View

    //绘图相关
    public:
        DrawImageMode draw_image_mode;
        DrawRoundRectMode view_round_mode;
        float view_round_rate;
    protected:
        virtual void rotateDrawself(MGRectRotate&);
        virtual void transitionDrawself(MGRectTransition&);
        virtual void scaleDrawself(MGRectScale&);
        virtual void subRotateDrawself(MGRectRotate&);
        virtual void subTransitionDrawself(MGRectTransition&);
        virtual void subScaleDrawself(MGRectScale&);
        virtual void drawself();    //drawself可重载 //
        virtual void drawBeforeSubviews();
        virtual void drawAfterSubviews();
        virtual bool needTranslateDraw(MGRectTransition& transition, MGRectRotate& rotate, MGRectScale& scale);
        virtual bool needSubTranslateDraw(MGRectTransition& transition, MGRectRotate& rotate, MGRectScale& scale);
    public:
        void draw();
        void drawRect(MGRect rect);
		virtual void setBackgroundColor(MGColor *_color);
        void setBackgroundAlphaInByte(byte_8 _alpha);
        void setBackgroundRefInByte(RGBAColorRef ref);
    public:
        void transAndScaleClipRect(MGRect& rect);
    protected:
        //视觉相关
        float m_alpha;  //透明度，取值范围0.0至1.0  //
        MGColor *m_background_color;  //背景颜色  //
        bool m_hidden;       //是否需要隐藏  //
    public:
        void setAlpha(float _alpha);
        void animateAlpha(float _alpha);
        void setAniAlpha(float _alpha);
        float getAlpha(){return m_alpha;}
		void hide();
		void show();
		bool isHide();
    //内存管理相关//
    public:
        bool isAutoClean(){return m_auto_clean;}
        void autoClean(){m_auto_clean = true;}
        void manualClean(){m_auto_clean = false;}
    protected:
        bool m_auto_clean; //是否需要清理，需要清理的View，在从父类remove出的时候要将自己销毁，默认为true   //

    //触摸事件相关//
    public:
        //参数第一个point可以被修改 //
        virtual MIView *hitDetect(MITouch* touch, MIEvent *event, MITouchEventType& type);
        bool checkTouchesInSubview();
		bool pointInView(const MGPoint &point);
        void locationInScreen(MGPoint& point, MIView* view); // 指定点是当前view坐标系下的点，计算在屏幕的坐标位置 //
    protected:
        virtual void translatePoint(MGPoint& point);
        virtual void detranslatePoint(MGPoint& point);
		virtual void detranslatePointWithoutRotate(MGPoint& point);
        bool pointInside(const MGPoint& point, const MIEvent * const event);

	protected:
        //用于在视图发生位移、旋转、缩放等动画效果时，进行触摸点的触发检测 //
        void transitionPoint(MGPoint& current_point);
        void rotatePoint(MGPoint& current_point, const MGPoint& ref_point);
        void scalePoint(MGPoint& current_point, const MGPoint& ref_point);
        void detransitionPoint(MGPoint& current_point);
        void derotatePoint(MGPoint& current_point, const MGPoint& ref_point);
        void descalePoint(MGPoint& current_point, const MGPoint& ref_point);
        
    protected:
        virtual void translateSubPoint(MGPoint& point);
        virtual void detranslateSubPoint(MGPoint& point);
		virtual void detranslateSubPointWithoutRotate(MGPoint& point);
        void transitionSubPoint(MGPoint& current_point);
        void rotateSubPoint(MGPoint& current_point, const MGPoint& ref_point);
        void scaleSubPoint(MGPoint& current_point, const MGPoint& ref_point);
        void detransitionSubPoint(MGPoint& current_point);
        void derotateSubPoint(MGPoint& current_point, const MGPoint& ref_point);
        void descaleSubPoint(MGPoint& current_point, const MGPoint& ref_point);
        
        // 手势识别相关 //
    public:
		void addGestureRecognizer(MIGestureRecognizer *gestureRecognizer);
        // 这里不维护手势识别实例，仅仅是解除绑定，如果需要释放，要创建的模块自行释放 //
		void removeGestureRecognizer(MIGestureRecognizer *gestureRecognizer);
        void removeAndDeleteAllGestureRecognizer();
    protected:
        void robTouchesAndEvent(const std::set<MITouch*>& touches, const MIEvent* const event);
    private:
		std::deque<MIGestureRecognizer *> m_gesture_recognizers;
    public:
        MIRESPONDER_DECLARE;
        MIRESPONDER_TOUCH_DECLARE;
    public:
        //触摸事件的响应============================================//
        MIEventHandleState FrameworkImp view_touchesBegan(const std::set<MITouch*>& touches,
                                                      const MIEvent* const event);
        MIEventHandleState FrameworkImp view_touchesMoved(const std::set<MITouch*>& touches,
                                                      const MIEvent* const event);
        MIEventHandleState FrameworkImp view_touchesEnded(const std::set<MITouch*>& touches,
                                                      const MIEvent* const event);
        MIEventHandleState FrameworkImp view_touchesCancelled(const std::set<MITouch*>& touches,
                                                          const MIEvent* const event);
    //代理设计模式相关//
    public:
        MIViewController* delegate;
    //动画相关//
    public:
        MIAnimationLayer* m_animation_layer;
    public:
        static MIAnimationsManager* animations_manager;
    public:
        void setAnimationAttributes();
        void initAnimation();
        //beginAnimation 和 commitAnimation必须配对，可以嵌套//
        static void beginAnimation(const string& animation_name);
        //  保留，用于设置动画曲线算法 //
        static void setAnimationCurve(MIViewAnimationOptions/*保留， 传入动画曲线算法的标识符*/);
        static void setAnimationAlgorithm(MIAnimationAlgorithm* algorithm);
        static void setAnimationDuration(float seconds);
        static void setAnimationRepeatCount(float repeat_count);
        static void setAnimationWillStartSelector(MIResponder* responder, SELHandler selector);
        static void setAnimationDidStopSelector(MIResponder* responder,SELHandler selector);
        static void commitAnimation();
        static void stopAnimation(const string& animation_name);
        static void waitNeedActionAnimations();
        //页面切换 //
        static void transitionViews(MIView* from_view, MIView* to_view, float duration, MIViewAnimationOptions options = MIViewAnimationOptionCurveLinear, MIResponderSEL will_start_animation=MIResponderSEL(),MIResponderSEL did_stop_animation=MIResponderSEL());// to_view added to from_view->m_superview, from_view removed from its m_superview.
    //触摸跟踪相关//
    public:
        static MITouchTracer* touch_tracer;
        //友元//
        friend class MIViewController;
        friend class MIAnimationsManager;
        friend class MIAnimation;
		friend class MIGestureRecognizer;
		friend class MIPanGestureRecognizer;
		friend class MIPinchGestureRecognizer;
        friend class MITapGestureRecognizer;
        friend class MILongPressGestureRecognizer;
        friend class MISwipeGestureRecognizer;
        friend class MIRotationGestureRecognizer;
        friend class MITouch;
    };
    typedef std::deque<MIView*>::iterator SubviewIterator;
    typedef std::deque<MIView*>::reverse_iterator SubviewReverseIterator;
    typedef std::deque<MIGestureRecognizer*>::iterator GestureRecognizerIterator;
#define MIVIEW_DECLARE \
	public:  virtual void initWithFrame(z11::MGRect frame);\
	protected:        virtual void drawself();\
	public:
}
//namespace z11
