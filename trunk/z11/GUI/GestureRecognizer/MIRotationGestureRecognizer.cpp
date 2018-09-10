#include <math.h>
#include "MIRotationGestureRecognizer.h"
#include "../../System/Device/MGDevice.h"
#include "../../System/Shared/MGLog.h"
#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif
namespace z11
{

#define ROTATION_GESTURE_SIGNIFICANT_MIN_DISTANCE 1.0f
    
    float MIRotationGestureRecognizer::rotation_significant_min_distance_rate = ROTATION_GESTURE_SIGNIFICANT_MIN_DISTANCE / 640.0f;
    
	MIRotationGestureRecognizer::MIRotationGestureRecognizer()
	{
		m_rotation = 0.0f;
        first_touch.tag = INVALID_TOUCH_TAG;
        second_touch.tag = INVALID_TOUCH_TAG;
	}

	MIRotationGestureRecognizer::~MIRotationGestureRecognizer()
	{

	}
    
    void MIRotationGestureRecognizer::touchesBegan( const std::set<MITouch*> &touches, const MIEvent* event)
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
    
	void MIRotationGestureRecognizer::touchesMoved( const std::set<MITouch*> &touches, const MIEvent* event)
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
            if (dis < rotation_significant_min_distance_rate * (MGDevice::getScreenWidth() > 1 ? MGDevice::getScreenWidth() : 640.0f)) {
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
            assert(last_dis >= ROTATION_GESTURE_SIGNIFICANT_MIN_DISTANCE);
            MGPoint last_point = MGPointMake(second_touch.screen_point.x - first_touch.screen_point.x, second_touch.screen_point.y - first_touch.screen_point.y);
            float last_angle = acosf(- last_point.y / last_dis);
            if (last_point.x <= 0) {
                last_angle = 2 * M_PI - last_angle;
            }
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
            if (curr_dis < ROTATION_GESTURE_SIGNIFICANT_MIN_DISTANCE) {
                return;
            }
            MGPoint curr_point = MGPointMake(curr_second_touch.screen_point.x - curr_first_touch.screen_point.x, curr_second_touch.screen_point.y - curr_first_touch.screen_point.y);
            float curr_angle = acosf(- curr_point.y / curr_dis);
            if (curr_point.x <= 0) {
                curr_angle = 2 * M_PI - curr_angle;
            }
            first_touch = curr_first_touch;
            second_touch = curr_second_touch;
            //MGLogD("curr_angle : %f , last_angle : %f .", curr_angle / M_PI * 180, last_angle / M_PI * 180);
            m_rotation = curr_angle - last_angle;
            if (state() == MIGestureRecognizerStatePossible) {
                setState(MIGestureRecognizerStateBegan);
            }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
                setState(MIGestureRecognizerStateChanged);
            }
        }
	}
    
	void MIRotationGestureRecognizer::touchesEnded( const std::set<MITouch*> &touches, const MIEvent* event)
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
	void MIRotationGestureRecognizer::touchesCancelled(const std::set<MITouch *> & touches, const MIEvent* event)
	{
		m_rotation = 0.0f;
        if (state() == MIGestureRecognizerStatePossible) {
            setState(MIGestureRecognizerStateFailed);
        }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
            setState(MIGestureRecognizerStateCancelled);
        }
        first_touch.tag = INVALID_TOUCH_TAG;
        second_touch.tag = INVALID_TOUCH_TAG;
	}
    
    void MIRotationGestureRecognizer::reset()
    {
        m_rotation = 0.0f;
        setState(MIGestureRecognizerStatePossible);
        first_touch.tag = INVALID_TOUCH_TAG;
        second_touch.tag = INVALID_TOUCH_TAG;
    }

    MGPoint MIRotationGestureRecognizer::getRotationCenter()
    {
        MGPoint ret;
        if (first_touch.tag != INVALID_TOUCH_TAG && second_touch.tag != INVALID_TOUCH_TAG) {
            ret = MGPointMake((first_touch.screen_point.x + second_touch.screen_point.x) * 0.5, (first_touch.screen_point.y + second_touch.screen_point.y) * 0.5);
        }
        return ret;
    }
}

