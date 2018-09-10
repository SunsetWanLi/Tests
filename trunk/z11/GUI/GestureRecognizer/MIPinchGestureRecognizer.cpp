/*
 * =====================================================================================
 *
 *       Filename:  MIPinchGestureRecognizer.cpp
 *
 *    Description:  pinch gesture detector
 *
 *        Version:  1.0
 *        Created:  04/06/2012 03:44:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include <math.h>
#include "MIPinchGestureRecognizer.h"
#include "../../System/Shared/MGLog.h"
#include "../../System/Device/MGDevice.h"

namespace z11
{

#define PINCH_GESTURE_SIGNIFICANT_MIN_DISTANCE 1.0f
    
    float MIPinchGestureRecognizer::pinch_significant_min_distance_rate = PINCH_GESTURE_SIGNIFICANT_MIN_DISTANCE / 640.0f;
    
	MIPinchGestureRecognizer::MIPinchGestureRecognizer()
	{
		m_scale = 1.0f;
        first_touch.tag = INVALID_TOUCH_TAG;
        second_touch.tag = INVALID_TOUCH_TAG;
	}

	MIPinchGestureRecognizer::~MIPinchGestureRecognizer()
	{

	}
    
    void MIPinchGestureRecognizer::touchesBegan( const std::set<MITouch*> &touches, const MIEvent* event)
	{
		int_32 num_of_touches = m_view->touch_manager.size() + touches.size();
        if (num_of_touches == 2) {
            setState(MIGestureRecognizerStatePossible);
        }else{
            if (state() == MIGestureRecognizerStatePossible) {
                setState(MIGestureRecognizerStateFailed);
            }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
                setState(MIGestureRecognizerStateCancelled);
            }
            first_touch.tag = INVALID_TOUCH_TAG;
            second_touch.tag = INVALID_TOUCH_TAG;
        }
	}
    
	void MIPinchGestureRecognizer::touchesMoved( const std::set<MITouch*> &touches, const MIEvent* event)
	{
        int_32 num_of_touches = m_view->touch_manager.size();
        if (num_of_touches != 2) {
            if (state() == MIGestureRecognizerStatePossible) {
                setState(MIGestureRecognizerStateFailed);
            }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
                setState(MIGestureRecognizerStateCancelled);
            }
            first_touch.tag = INVALID_TOUCH_TAG;
            second_touch.tag = INVALID_TOUCH_TAG;
            return;
        }
		if (first_touch.tag == INVALID_TOUCH_TAG || second_touch.tag == INVALID_TOUCH_TAG) {
            TouchMIt it = m_view->touch_manager.begin();
            first_touch = it->second;
            ++it;
            second_touch = it->second;
            float dis = distanceOfTwoPoint(first_touch.screen_point, second_touch.screen_point);
            if (dis < pinch_significant_min_distance_rate * (MGDevice::getScreenWidth() > 1 ? MGDevice::getScreenWidth() : 640.0f)) {
                first_touch.tag = INVALID_TOUCH_TAG;
                second_touch.tag = INVALID_TOUCH_TAG;
                return;
            }
        } else {
            TouchMIt f_it;
            TouchMIt s_it;
            f_it = m_view->touch_manager.find(first_touch.tag);
            if (f_it == m_view->touch_manager.end()) {
                if (state() == MIGestureRecognizerStatePossible) {
                    setState(MIGestureRecognizerStateFailed);
                }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
                    setState(MIGestureRecognizerStateCancelled);
                }
                first_touch.tag = INVALID_TOUCH_TAG;
                second_touch.tag = INVALID_TOUCH_TAG;
                return;
            }
            s_it = m_view->touch_manager.find(second_touch.tag);
            if (s_it == m_view->touch_manager.end()) {
                if (state() == MIGestureRecognizerStatePossible) {
                    setState(MIGestureRecognizerStateFailed);
                }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
                    setState(MIGestureRecognizerStateCancelled);
                }
                first_touch.tag = INVALID_TOUCH_TAG;
                second_touch.tag = INVALID_TOUCH_TAG;
                return;
            }
            float last_dis = distanceOfTwoPoint(first_touch.screen_point, second_touch.screen_point);
            assert(last_dis >= pinch_significant_min_distance_rate * (MGDevice::getScreenWidth() > 1 ? MGDevice::getScreenWidth() : 640.0f));
            std::set<MITouch*>::const_iterator it;
            MITouch curr_first_touch = first_touch, curr_second_touch = second_touch;
            for (it = touches.begin(); it != touches.end(); ++it) {
                if ((*it)->tag == first_touch.tag) {
                    curr_first_touch = *it;
                }
                if ((*it)->tag == second_touch.tag) {
                    curr_second_touch = *it;
                }
            }
            float curr_dis = distanceOfTwoPoint(curr_first_touch.screen_point, curr_second_touch.screen_point);
            if (curr_dis < pinch_significant_min_distance_rate * (MGDevice::getScreenWidth() > 1 ? MGDevice::getScreenWidth() : 640.0f)) {
                return;
            }
            first_touch = curr_first_touch;
            second_touch = curr_second_touch;
            m_scale = curr_dis / last_dis;
            if (state() == MIGestureRecognizerStatePossible) {
                setState(MIGestureRecognizerStateBegan);
            }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
                setState(MIGestureRecognizerStateChanged);
            }
        }
	}
    
	void MIPinchGestureRecognizer::touchesEnded( const std::set<MITouch*> &touches, const MIEvent* event)
	{
		int_32 num_of_touches = m_view->touch_manager.size() - touches.size();
        if (num_of_touches == 2) {
            setState(MIGestureRecognizerStatePossible);
        }else{
            if (state() == MIGestureRecognizerStatePossible) {
                setState(MIGestureRecognizerStateFailed);
            }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
                setState(MIGestureRecognizerStateEnded);
            }
            first_touch.tag = INVALID_TOUCH_TAG;
            second_touch.tag = INVALID_TOUCH_TAG;
        }
	}
	void MIPinchGestureRecognizer::touchesCancelled(const std::set<MITouch *> & touches, const MIEvent* event)
	{
		m_scale = 1.0f;
        if (state() == MIGestureRecognizerStatePossible) {
            setState(MIGestureRecognizerStateFailed);
        }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
            setState(MIGestureRecognizerStateCancelled);
        }
        first_touch.tag = INVALID_TOUCH_TAG;
        second_touch.tag = INVALID_TOUCH_TAG;
	}
    
    void MIPinchGestureRecognizer::reset()
    {
        m_scale = 1.0f;
        setState(MIGestureRecognizerStatePossible);
        first_touch.tag = INVALID_TOUCH_TAG;
        second_touch.tag = INVALID_TOUCH_TAG;
    }

    MGPoint MIPinchGestureRecognizer::getPinchCenter(MIView* view)
    {
        MGPoint ret;
        if (first_touch.tag != INVALID_TOUCH_TAG && second_touch.tag != INVALID_TOUCH_TAG) {
            ret = MGPointMake((first_touch.screen_point.x + second_touch.screen_point.x) * 0.5, (first_touch.screen_point.y + second_touch.screen_point.y) * 0.5);
        }
        if (view != NULL) {
            MITouch c_touch;
            c_touch.screen_point = ret;
            c_touch.point = ret;
            c_touch.saved_point = ret;
            ret = c_touch.locationInView(view);
        }
        return ret;
    }
}

