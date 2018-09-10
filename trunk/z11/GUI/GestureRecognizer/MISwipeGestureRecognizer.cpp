#include "MISwipeGestureRecognizer.h"
#include "../../System/Shared/MGLog.h"
#include "../../System/Device/MGDevice.h"
#include <math.h>

using namespace std;

#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif
namespace z11
{
#define SWIPE_GESTURE_SIGNIFICANT_MOVE_VALVE 12.0f
    
    float MISwipeGestureRecognizer::swipe_significant_move_rate = SWIPE_GESTURE_SIGNIFICANT_MOVE_VALVE / 640.0f;
    
	MISwipeGestureRecognizer::MISwipeGestureRecognizer(void)
	{
		numberOfTouchesRequired = 1;
		direction = MISwipeGestureRecognizerDirectionLeft;
        begin_touch.tag = INVALID_TOUCH_TAG;
        move_touch.tag = INVALID_TOUCH_TAG;
		m_angle = 15.0;
	}

	MISwipeGestureRecognizer::~MISwipeGestureRecognizer(void)
	{
	}
    
    void MISwipeGestureRecognizer::touchesBegan( const std::set<MITouch*> &touches, const MIEvent* event)
	{
		int_32 num_of_touches = m_view->touch_manager.size() + touches.size();
        if (num_of_touches == numberOfTouchesRequired) {
            if(begin_touch.tag == INVALID_TOUCH_TAG)
            {
                setState(MIGestureRecognizerStatePossible);
                begin_touch = *(*touches.begin());
            }
        }else{
            if (state() == MIGestureRecognizerStatePossible) {
                setState(MIGestureRecognizerStateFailed);
            }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
                setState(MIGestureRecognizerStateCancelled);
            }
            begin_touch.tag = INVALID_TOUCH_TAG;
            move_touch.tag = INVALID_TOUCH_TAG;
        }
	}
    
	void MISwipeGestureRecognizer::touchesMoved( const std::set<MITouch*> &touches, const MIEvent* event)
	{
        int_32 num_of_touches = touches.size();
        if (num_of_touches != numberOfTouchesRequired || begin_touch.tag == INVALID_TOUCH_TAG) {
            // 没有合适数量的移动触摸，或者一开始有太多触摸导致没有进入移动状态，都直接返回 //
            if (state() == MIGestureRecognizerStatePossible) {
                setState(MIGestureRecognizerStateFailed);
            }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
                setState(MIGestureRecognizerStateCancelled);
            }
            return;
        }

        std::set<MITouch*>::const_iterator it;
        for (it = touches.begin(); it != touches.end(); ++it) {
            if (begin_touch.tag == (*it)->tag) {
                move_touch = *(*it);
                if (state() == MIGestureRecognizerStatePossible) {
                    float dis = distanceOfTwoPoint(move_touch.screen_point, begin_touch.screen_point);
                    if (dis < swipe_significant_move_rate * (MGDevice::getScreenWidth() > 1 ? MGDevice::getScreenWidth() : 640.0f)) {
                        return;
                    }
                    // 达到了移动范围，继续判断移动的角度，以确定是否需要设置为识别或者失败 //
                    float dis_x = move_touch.screen_point.x - begin_touch.screen_point.x;
                    float dis_y = move_touch.screen_point.y - begin_touch.screen_point.y;
                    float angle = acosf(- dis_y / dis);
                    if (dis_x <= 0) {
                        angle = 2 * M_PI - angle;
                    }
                    //MGLogD("move_touch : %f, %f.", move_touch.screen_point.x, move_touch.screen_point.y);
                    //MGLogD("begin_touch : %f, %f.", begin_touch.screen_point.x, begin_touch.screen_point.y);
                    angle = angle / M_PI * 180;
                    //MGLogD("angle : %f.", angle);
                    // 垂直角度 //
                    if (((0 <= angle && angle <= m_angle) || ((360.0-m_angle) <= angle && angle <= 360.0)) && ((direction & MISwipeGestureRecognizerDirectionUp) != 0)) {
                        setState(MIGestureRecognizerStateBegan);
                        return;
                    }
                    if (((180.0-m_angle) <= angle && angle <= (180.0+m_angle)) && ((direction & MISwipeGestureRecognizerDirectionDown) != 0)) {
                        setState(MIGestureRecognizerStateBegan);
                        return;
                    }
                    // 水平角度 //
                    if (((270.0-m_angle) <= angle && angle <= (270.0+m_angle)) && ((direction & MISwipeGestureRecognizerDirectionLeft) != 0)) {
                        setState(MIGestureRecognizerStateBegan);
                        return;
                    }
                    if (((90-m_angle) <= angle && angle <= (90+m_angle)) && ((direction & MISwipeGestureRecognizerDirectionRight) != 0)) {
                        setState(MIGestureRecognizerStateBegan);
                        return;
                    }
                    setState(MIGestureRecognizerStateFailed);
                }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
                    setState(MIGestureRecognizerStateChanged);
                }
                begin_touch = move_touch;
                return;
            }
        }
        if (state() == MIGestureRecognizerStatePossible) {
            setState(MIGestureRecognizerStateFailed);
        }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
            setState(MIGestureRecognizerStateCancelled);
        }
	}
    
	void MISwipeGestureRecognizer::touchesEnded( const std::set<MITouch*> &touches, const MIEvent* event)
	{
		int_32 num_of_touches = m_view->touch_manager.size() - touches.size();
        if (num_of_touches < numberOfTouchesRequired) {
            if (state() == MIGestureRecognizerStatePossible) {
                setState(MIGestureRecognizerStateFailed);
            }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
                setState(MIGestureRecognizerStateEnded);
            }
            begin_touch.tag = INVALID_TOUCH_TAG;
            move_touch.tag = INVALID_TOUCH_TAG;
        }
	}
	void MISwipeGestureRecognizer::touchesCancelled(const std::set<MITouch *> & touches, const MIEvent* event)
	{
		if (state() == MIGestureRecognizerStatePossible) {
            setState(MIGestureRecognizerStateFailed);
        }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
            setState(MIGestureRecognizerStateCancelled);
        }
        begin_touch.tag = INVALID_TOUCH_TAG;
        move_touch.tag = INVALID_TOUCH_TAG;
	}
    
    void MISwipeGestureRecognizer::reset()
    {
        setState(MIGestureRecognizerStatePossible);
        begin_touch.tag = INVALID_TOUCH_TAG;
        move_touch.tag = INVALID_TOUCH_TAG;
    }

	void MISwipeGestureRecognizer::setSwipeAngle(float _angle)
	{
		m_angle = _angle;
	}
}
