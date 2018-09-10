//
//  MIPictureViewCell.h
//  Gui
//
//  Created by Hu DanYuan on 11-6-22
//  Copyright 2011年 Zhejiang University. All rights reserved.
#include "MIPictureView.h"
#include "MIPictureViewCell.h"
#include "../../GraphicCore/MGImage.h"
#include "../../System/Shared/MGLog.h"
#include "../../GUI/GUIContainer/GUIContainer.h"
#include "../GestureRecognizer/MIPinchGestureRecognizer.h"
#include "../GestureRecognizer/MIPanGestureRecognizer.h"
#include "stdio.h"//不能去掉 //
namespace z11
{

	MIPictureViewCell::MIPictureViewCell()
	{
		class_name = "MIPictureViewCell";
		m_picture_min_scaling = 0.25f;
		m_picture_max_scaling = 2.0f;
		m_picture_real_scaling = 1.0f;
		m_picture_current_scaling = 1.0f;
		enable_scale = true;
		enable_horizontal = true;
		enable_vertical = true;

		ani_duration = 0.2f;
	}

	MIPictureViewCell::~MIPictureViewCell()
	{
		delete_and_set_null(m_pinch_recognizer);
		delete_and_set_null(m_pan_gesture);
	}

	void MIPictureViewCell::drawself()
	{
		MIView::drawself();
	}

	void MIPictureViewCell::initWithFrame(MGRect frame)
	{
		MIControl::initWithFrame(frame);

		m_pinch_recognizer = new MIPinchGestureRecognizer();
		SELHandler SEL_handler;
		SEL_handler.sender = m_pinch_recognizer;
		SEL_handler.sel_func = (SELFunc)&MIPictureViewCell::zoomCall;
		m_pinch_recognizer->initWithTarget(this,SEL_handler);
		addGestureRecognizer(m_pinch_recognizer);

		m_pan_gesture = new MIPanGestureRecognizer();
		SEL_handler.sender = m_pan_gesture;
		SEL_handler.sel_func = (SELFunc)&MIPictureViewCell::panCall;
		m_pan_gesture->initWithTarget(this,SEL_handler);
		addGestureRecognizer(m_pan_gesture);

		m_content_size = frame;
		m_parent_size = frame;
	}

	void MIPictureViewCell::setBackgroundColor(MGColor *_color)
	{
		MGRect rect;
		if(_color != NULL && _color->m_image != NULL) 
		{
			if (_view_flags.view_initialized == 1) {
				PixelInt picture_width = _color->m_image->getWidth();
				PixelInt picture_height = _color->m_image->getHeight();
				rect = MGRectMake((m_parent_size.width-picture_width)/2, (m_parent_size.height-picture_height)/2, 
					(float)picture_width, (float)picture_height);
				_setFrame(rect);

			//	float min_scaling = MINVALUE(m_parent_size.width/picture_width, m_parent_size.height/picture_height);
				

				float init_scale = MAXVALUE(m_parent_size.width/picture_width, m_parent_size.height/picture_height);
				m_picture_min_scaling = init_scale;

				if(init_scale < 1.0f)
					m_picture_max_scaling = 2.0f;
				else
					m_picture_max_scaling = init_scale*2.0f;

				m_picture_real_scaling = init_scale;
				setRectScale(MGRectScaleMake(init_scale, init_scale, 1));
				//m_pinch_recognizer->setScale(init_scale);
				m_content_size.width = picture_width * init_scale;
				m_content_size.height = picture_height * init_scale;
				m_content_size.x = (m_parent_size.width - m_content_size.width) / 2;
				m_content_size.y = (m_parent_size.height - m_content_size.height) / 2;
				if (m_content_size.y < 0) {
					setRectTransition(MGRectTransitionMake(0, -m_content_size.y, 0));
					m_picture_real_offset = MGPointMake(0, -m_content_size.y);
				}
			}
		}
 		MIView::setBackgroundColor(_color);
	}

	void MIPictureViewCell::panCall(MGObject *_sender)
	{
		if(enable_horizontal || enable_vertical) 
		{
			MIPanGestureRecognizer *recognizer = dynamic_cast<MIPanGestureRecognizer *>(_sender);
			if (recognizer != NULL)
			{
				m_picture_current_offset = recognizer->translationInView(parent());

				// 更新x坐标的offset  ////
				do 
				{
					if (!enable_horizontal) 
					{
						m_picture_current_offset.x = 0.0f;
						break;
					} 

					float x_right_blank = m_parent_size.width-(m_content_size.x+m_content_size.width);
					if(m_content_size.x > 0 || x_right_blank > 0)
					{
						m_picture_current_offset.x *= 0.5f;
					}
				} 
				while(false);
				// 更新y坐标的offset  ////
				do 
				{
					if(!enable_vertical) 
					{
						m_picture_current_offset.y = 0.0f;
						break;
					}

					float y_bottom_blank = m_parent_size.height-(m_content_size.y+m_content_size.height);
					if(m_content_size.y > 0 || y_bottom_blank > 0)
					{
						m_picture_current_offset.y *= 0.5f;
					}
				} 
				while(false);

				m_picture_real_offset.x += m_picture_current_offset.x;
				m_picture_real_offset.y += m_picture_current_offset.y;
				setRectTransition(MGRectTransitionMake(m_picture_real_offset.x, m_picture_real_offset.y, 0.0));
				//recognizer->setTranslationInView(MGPoint(),NULL);
				m_content_size.x = m_frame.x + m_picture_real_offset.x + (m_frame.width - m_content_size.width) / 2;
				m_content_size.y = m_frame.y + m_picture_real_offset.y + (m_frame.height - m_content_size.height) / 2;
				//bounceInScroll(_sender);
			}
		}
	}

	void MIPictureViewCell::zoomCall( MGObject *_sender )
	{
		if (enable_scale)
		{
			MIPinchGestureRecognizer *recognizer = dynamic_cast<MIPinchGestureRecognizer *>(_sender);
			if (recognizer != NULL) 
			{
				m_picture_real_scaling = recognizer->getScale();
				setRectScale(MGRectScaleMake(m_scale.axis_x * m_picture_real_scaling,  m_scale.axis_y * m_picture_real_scaling, 1));
				m_content_size.width = m_frame.width * m_picture_real_scaling;
				m_content_size.height = m_frame.height * m_picture_real_scaling;
				m_content_size.x = m_frame.x + m_picture_real_offset.x + (m_frame.width - m_content_size.width) / 2;
				m_content_size.y = m_frame.y + m_picture_real_offset.y + (m_frame.height - m_content_size.height) / 2;
				//bounceInZoom(_sender);
			}
		}
	}

	void MIPictureViewCell::bounceToAdapt(MGObject* _sender)
	{
		MGRectTransition transition_now = m_transition;
		bool bounce_flag = false;

		if (m_picture_real_scaling > m_picture_max_scaling || m_picture_real_scaling < m_picture_min_scaling) 
		{
			m_picture_real_scaling = MAXVALUE(m_picture_real_scaling, m_picture_min_scaling);
			m_picture_real_scaling = MINVALUE(m_picture_real_scaling, m_picture_max_scaling);
			m_content_size.width = m_frame.width * m_picture_real_scaling;
			m_content_size.height = m_frame.height * m_picture_real_scaling;
			m_content_size.x = m_frame.x + m_picture_real_offset.x + (m_frame.width - m_content_size.width) / 2;
			m_content_size.y = m_frame.y + m_picture_real_offset.y + (m_frame.height - m_content_size.height) / 2;
			//m_pinch_recognizer->setScale(m_picture_real_scaling);
			bounce_flag = true;
		}
		if (m_content_size.x + m_content_size.width < m_parent_size.width) {
			transition_now.axis_x = -(m_content_size.width - m_parent_size.width) / 2;
			bounce_flag = true;
		}
		if (m_content_size.y + m_content_size.height < m_parent_size.height) {
			transition_now.axis_y = -(m_content_size.height - m_parent_size.height) / 2;
			bounce_flag = true;
		}
		if (m_content_size.x > 0) {
			transition_now.axis_x = (m_content_size.width - m_parent_size.width) / 2;
			bounce_flag = true;
		}
		if (m_content_size.y > 0) {
			transition_now.axis_y = (m_content_size.height - m_parent_size.height) / 2;
			bounce_flag = true;
		}
		if (m_content_size.width < m_parent_size.width) {
			transition_now.axis_x = 0;
			bounce_flag = true;
		}
		if (m_content_size.height < m_parent_size.height) {
			transition_now.axis_y = 0;
			bounce_flag = true;
		}

		if (bounce_flag) {
			m_picture_real_offset.x = transition_now.axis_x;
			m_picture_real_offset.y = transition_now.axis_y;

			char pointer[128];
			sprintf(pointer, "%p", this);
			string str_pointer = pointer;
			MIView::beginAnimation("Picture View Bounce In Scroll." + str_pointer);
			MIView::setAnimationDuration(ani_duration);
			MIView::setAnimationCurve(MIViewAnimationOptionCurveEaseInOut);
			setRectTransition(transition_now);
			setRectScale(MGRectScaleMake(m_picture_real_scaling, m_picture_real_scaling, 1));
			MIView::commitAnimation();
		}
	}

	//void MIPictureViewCell::adjustFrame(MGPoint curr_offset, float curr_scaling)
	//{
	//	MGRect tmp_frame = getFrame();
	//	//MGRect super_frame = m_superview->getFrame();
	//	tmp_frame.height = m_frame.height * curr_scaling;
	//	tmp_frame.width = m_frame.width * curr_scaling;
	//	tmp_frame.x = tmp_frame.x - (tmp_frame.width - m_frame.width)/2 + curr_offset.x;
	//	tmp_frame.y = tmp_frame.y - (tmp_frame.height - m_frame.height)/2 + curr_offset.y;
	//	_setFrame(tmp_frame);
	//}

	//触摸事件的响应 
	MIEventHandleState MIPictureViewCell::touchesBegan(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		if(moved_sel.responder != NULL && moved_sel.action.sel_func != NULL && moved_sel.action.sender != NULL)
		{
			MIEvent* respond_event = new MIEvent();
			respond_event->_responder = moved_sel.responder;
			respond_event->_handler = moved_sel.action;
			GuiContainer::container->preserveEvent(respond_event);
		}
		
		MIEventHandleState ret = MIControl::touchesBegan(touches, event);
		//std::set<MITouch*> tmp_touches;
		//std::set<MITouch*>::const_iterator touch_it;
		//for (touch_it=touches.begin(); touch_it!=touches.end(); ++touch_it) {
		//	if ((*touch_it) == NULL)
		//		continue;
		//	if (m_previous_touches.size() >= PICTURE_VIEW_MAX_TOUCH_NUM) {
		//		break;
		//	}
		//	//if ((*touch_it)->maintained_view == this) {
		//		m_previous_touches.insert(*touch_it);
		//	//}
		//}

		//getCentrePointFromTouches(m_previous_touches, m_previous_centra_point);
		//getRadiusFromTouches(m_previous_touches, m_previous_radius);
		////MGLogD("point(%f,%f), %f", m_previous_centra_point.x, m_previous_centra_point.y, m_previous_radius);
		return ret;
	}
	MIEventHandleState MIPictureViewCell::touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		//MGLogD("touches size========%d", touches.size());
		MIEventHandleState ret = MIControl::touchesMoved(touches, event);
		//bool is_moved = false;
		//bool is_zoomed = false;
		//MGPoint tmp_point, tmp_offset;
		//float tmp_radius = 0, tmp_scaling = 1.0;

		//std::set<MITouch*> tmp_touches;
		//std::set<MITouch*>::const_iterator touch_it;
		//for (touch_it=touches.begin(); touch_it!=touches.end(); ++touch_it) {
		//	if ((*touch_it) == NULL)
		//		continue;
		//	if (pointInside((*touch_it)->point, NULL)) {
		//		if (tmp_touches.size() >= PICTURE_VIEW_MAX_TOUCH_NUM) {
		//			break;
		//		}
		//		//if ((*touch_it)->maintained_view == this) {
		//			tmp_touches.insert(*touch_it);
		//		//}
		//	}
		//}

		//if (tmp_touches.size() != m_previous_touches.size()) {
		//	getCentrePointFromTouches(tmp_touches, m_previous_centra_point);
		//	getRadiusFromTouches(tmp_touches, m_previous_radius);
		//}
		////MGLogD("size===%d  m_previous_centra_point(%f,%f), %f",m_previous_touches.size(), 
		////	m_previous_centra_point.x, m_previous_centra_point.y, m_previous_radius);

		//getCentrePointFromTouches(tmp_touches, tmp_point);
		//getRadiusFromTouches(tmp_touches, tmp_radius);
		////MGLogD("size===%d  tmp_point(%f,%f), %f",touches.size(), tmp_point.x, tmp_point.y, tmp_radius);
		//
		////计算缩放比例  
		//if (tmp_touches.size()==2 && m_previous_touches.size()==2) {
		//	m_picture_current_scaling = 1.0f;
		//	tmp_scaling = tmp_radius / m_previous_radius;
		//	m_previous_radius = tmp_radius;
		//	if (fabsf(tmp_scaling - 1.0f) > PICTURE_VIEW_MIN_ZOOM_VALUE) {
		//		float _real_scaling = m_picture_real_scaling * tmp_scaling;
		//		_real_scaling = MINVALUE(m_picture_max_scaling, _real_scaling);
		//		_real_scaling = MAXVALUE(m_picture_min_scaling, _real_scaling);
		//		m_picture_current_scaling = _real_scaling / m_picture_real_scaling;
		//		m_picture_real_scaling = _real_scaling;
		//		is_zoomed = true;
		//           //MGLogD("=============Picture Scale==============");
		//	}
		//}
		//
		////计算移动距离  
		//if (tmp_touches.size() == 1) {		//只有一个touch的时候出发Move 
		//	m_picture_current_offset = MGPointMake(0.0f, 0.0f);
		//	tmp_offset.x = tmp_point.x - m_previous_centra_point.x;
		//	tmp_offset.y = tmp_point.y - m_previous_centra_point.y;
		//	m_previous_centra_point = tmp_point;
		//	if (fabsf(tmp_offset.x) > PICTURE_VIEW_MIN_MOVE_VALUE
		//		|| fabsf(tmp_offset.y) > PICTURE_VIEW_MIN_MOVE_VALUE) {
		//		m_picture_current_offset = tmp_offset;
		//		m_picture_real_offset.x += m_picture_current_offset.x;
		//		m_picture_real_offset.y += m_picture_current_offset.y;
		//		is_moved = true;
		//           //MGLogD("=============Picture Move==============");
		//	}
		//}
		//m_previous_touches = tmp_touches;

		//if (enable_move && is_moved) {
		//	adjustFrame(m_picture_current_offset, 1.0f);
		//	ret = handleWithControlEvent(MIPictureEventMove);
		//}
		//if(enable_scale && is_zoomed) {
		//	adjustFrame(MGPointMake(0,0), m_picture_current_scaling);
		//	ret = handleWithControlEvent(MIPictureEventZoom);
		//}
		return ret;
	}
	MIEventHandleState MIPictureViewCell::touchesEnded(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesEnded(touches, event);
		bounceToAdapt(NULL);
		return ret;
	}
	MIEventHandleState MIPictureViewCell::touchesCancelled(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesCancelled(touches, event);
		return ret;
	}
	//事件响应   
	MIEventHandleState MIPictureViewCell::handleEvent(const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::handleEvent(event);
		return ret;
	}

	//MIEventHandleState MIPictureViewCell::handleWithControlEvent(const MIControlEvents &event)
	//{
	//	MIPictureView* pic_view = static_cast<MIPictureView*>(parent());
	//	MIEventHandleState ret = pic_view->handleWithControlEvent(event);
	//	return ret;
	//}

	//bool MIPictureViewCell::addTouches(const std::set<MITouch*>& touches)
	//{
	//	bool ret = false;
	//	std::set<MITouch*> tmp_touches;
	//	std::set<MITouch*>::const_iterator touch_it;
	//	for (touch_it=touches.begin(); touch_it!=touches.end(); ++touch_it) {
	//		assert((*touch_it) != NULL);
	//		std::set<MITouch*>::const_iterator curr_touch_it;
	//		for (curr_touch_it=m_previous_touches.begin(); curr_touch_it!=m_previous_touches.end(); ++curr_touch_it) {
	//			assert((*curr_touch_it) != NULL);
	//			if((*curr_touch_it)->tag == (*touch_it)->tag) {
	//				(*curr_touch_it)->point = (*touch_it)->point;
	//				MGLogD("m_previous_touches modify");
	//				break;
	//			}
	//		}
	//		if (curr_touch_it == m_previous_touches.end()) {
	//			m_previous_touches.insert(*touch_it);
	//			MGLogD("m_previous_touches.insert");
	//			ret = true;
	//		}
	//	}
	//	return ret;
	//}
	//
	//bool MIPictureViewCell::removeTouches(const std::set<MITouch*>& touches)
	//{
	//	bool ret = false;
	//	std::set<MITouch*> tmp_touches;
	//	std::set<MITouch*>::const_iterator touch_it;
	//	for (touch_it=touches.begin(); touch_it!=touches.end(); ++touch_it) {
	//		assert((*touch_it) != NULL);
	//		std::set<MITouch*>::const_iterator curr_touch_it;
	//		for (curr_touch_it=m_previous_touches.begin(); curr_touch_it!=m_previous_touches.end(); ++curr_touch_it) {
	//			assert((*curr_touch_it) != NULL);
	//			if((*curr_touch_it)->tag == (*touch_it)->tag) {
	//				m_previous_touches.erase(*curr_touch_it);
	//				MGLogD("m_previous_touches.erase");
	//				ret = true;
	//				break;
	//			}
	//		}
	//	}
	//	return ret;
	//}
	//
	//void MIPictureViewCell::getCentrePointFromTouches(const std::set<MITouch*>& touches, MGPoint &point)
	//{
	//	std::set<MITouch*>::const_iterator touch_it;
	//	point.x = point.y = 0;
	//	if (touches.size() == 0)
	//		return;
	//	for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
	//		assert((*touch_it) != NULL);
	//		point.x += (*touch_it)->point.x;
	//		point.y += (*touch_it)->point.y;
	//	}
	//	point.x = point.x / touches.size();
	//	point.y = point.y / touches.size();
	//}
	//
	//void MIPictureViewCell::getRadiusFromTouches(const std::set<MITouch*>& touches, float &radius)
	//{
	//	radius = 0.0f;
	//	if (touches.size() == 0)
	//		return;
	//	MGPoint centra_point;
	//	getCentrePointFromTouches(touches, centra_point);
	//	std::set<MITouch*>::const_iterator touch_it;
	//	for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
	//		assert((*touch_it) != NULL);
	//		radius += sqrtf((float)pow((centra_point.x - (*touch_it)->point.x) , 2) + (float)pow((centra_point.y - (*touch_it)->point.y), 2));
	//	}
	//	radius = radius / touches.size();
	//}
}