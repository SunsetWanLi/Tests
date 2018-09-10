//
//  MIPictureViewCell.h
//  Gui
//
//  Created by Hu DanYuan on 11-6-22
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once
#include "MIControl.h"

namespace z11
{
	class MIPinchGestureRecognizer;
	class MIPanGestureRecognizer;

	class MIPictureViewCell : public MIControl
	{
    public:
        MIPictureViewCell();
        virtual ~MIPictureViewCell();
    public:
		virtual void setBackgroundColor(MGColor *_color);
		void panCall(MGObject *_sender);
		void zoomCall(MGObject *_sender);
		void bounceToAdapt(MGObject* _sender);

		MIVIEW_DECLARE
		MIRESPONDER_TOUCH_DECLARE

	private:
		//void adjustFrame(MGPoint curr_offset, float curr_scaling);
		//void adjustFrame(MGRect rect);
		////获得touches的几何中心    
		//void getCentrePointFromTouches(const std::set<MITouch*>& touches, MGPoint &point);
		////获得touches的等效半径 
		//void getRadiusFromTouches(const std::set<MITouch*>& touches, float &radius);
		////处理用户注册事件  
		//MIEventHandleState handleWithControlEvent(const MIControlEvents &event);
		////在m_previous_touches添加移入当前view的touches
		//bool addTouches(const std::set<MITouch*>& touches);
		////在m_previous_touches删除从当前view中移出的touches
		//bool removeTouches(const std::set<MITouch*>& touches);

	private:
		//图片缩放、移动相关  ////
		float	m_picture_min_scaling;
		float	m_picture_max_scaling;
		MGPoint m_picture_current_offset;		//当前触摸位移 
		float	m_picture_current_scaling;		//当前触摸缩放比例 
		MGPoint m_picture_real_offset;			//图片实际位移 
		float	m_picture_real_scaling;			//图片实际缩放比例 

		bool	enable_scale;
		bool	enable_horizontal;
		bool	enable_vertical;

		MGRect m_parent_size;
		MGRect m_content_size;
		MIPinchGestureRecognizer *m_pinch_recognizer;
		MIPanGestureRecognizer *m_pan_gesture;

		float ani_duration;

		MIResponderSEL moved_sel;

		friend class MIPictureView;
    };
}