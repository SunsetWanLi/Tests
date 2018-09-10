#include "MIPanGestureRecognizer.h"
#include "../../System/Shared/MGLog.h"
#include "../../System/Device/MGDevice.h"
namespace z11
{
#define PAN_GESTURE_SIGNIFICANT_MOVE_VALVE 6.0f
    
    float MIPanGestureRecognizer::pan_significant_move_rate = PAN_GESTURE_SIGNIFICANT_MOVE_VALVE / 640.0f;
    
	MIPanGestureRecognizer::MIPanGestureRecognizer(void)
	{
		maximumNumberOfTouches = 1;
		minimumNumberOfTouches = 1;
        begin_touch.tag = INVALID_TOUCH_TAG;
        move_touch.tag = INVALID_TOUCH_TAG;
	}

	MIPanGestureRecognizer::~MIPanGestureRecognizer(void)
	{
	}
    
    void MIPanGestureRecognizer::touchesBegan( const std::set<MITouch*> &touches, const MIEvent* event)
	{
		int_32 num_of_touches = m_view->touch_manager.size() + touches.size();
        if (num_of_touches >= minimumNumberOfTouches && num_of_touches <= maximumNumberOfTouches) {
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
    
	void MIPanGestureRecognizer::touchesMoved( const std::set<MITouch*> &touches, const MIEvent* event)
	{
        int_32 num_of_touches = touches.size();
        if (num_of_touches < minimumNumberOfTouches || num_of_touches > maximumNumberOfTouches || begin_touch.tag == INVALID_TOUCH_TAG) {
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
                    if (dis < pan_significant_move_rate * (MGDevice::getScreenWidth() > 1 ? MGDevice::getScreenWidth() : 640.0f)) {
                        return;
                    }
                    setState(MIGestureRecognizerStateBegan);
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
    
	void MIPanGestureRecognizer::touchesEnded( const std::set<MITouch*> &touches, const MIEvent* event)
	{
		int_32 num_of_touches = m_view->touch_manager.size() - touches.size();
        if (num_of_touches < minimumNumberOfTouches) {
            if (state() == MIGestureRecognizerStatePossible) {
                setState(MIGestureRecognizerStateFailed);
            }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
                setState(MIGestureRecognizerStateEnded);
            }
            begin_touch.tag = INVALID_TOUCH_TAG;
            move_touch.tag = INVALID_TOUCH_TAG;
        }
	}
	void MIPanGestureRecognizer::touchesCancelled(const std::set<MITouch *> & touches, const MIEvent* event)
	{
		if (state() == MIGestureRecognizerStatePossible) {
            setState(MIGestureRecognizerStateFailed);
        }else if(state() == MIGestureRecognizerStateBegan || state() == MIGestureRecognizerStateChanged){
            setState(MIGestureRecognizerStateCancelled);
        }
        begin_touch.tag = INVALID_TOUCH_TAG;
        move_touch.tag = INVALID_TOUCH_TAG;
	}
    
    void MIPanGestureRecognizer::reset()
    {
        setState(MIGestureRecognizerStatePossible);
        begin_touch.tag = INVALID_TOUCH_TAG;
        move_touch.tag = INVALID_TOUCH_TAG;
    }
    
    MGPoint MIPanGestureRecognizer::translationInView(z11::MIView *view)
    {
        MGPoint ret = MGPointMake(0, 0);
        if (begin_touch.tag == INVALID_TOUCH_TAG || move_touch.tag == INVALID_TOUCH_TAG) {
            return ret;
        }
        MGPoint begin_p = begin_touch.locationInView(view);
        MGPoint move_p = move_touch.locationInView(view);
        ret.x = move_p.x - begin_p.x;
        ret.y = move_p.y - begin_p.y;
        return ret;
    }
    
    MGTimeDelta MIPanGestureRecognizer::timeDelta()
    {
        MGTimeDelta ret = MGTimeDelta::FromSecondsF(0);
        if (begin_touch.tag != INVALID_TOUCH_TAG && move_touch.tag != INVALID_TOUCH_TAG) {
            ret = move_touch.timestamp - begin_touch.timestamp;
        }
        return ret;
    }
}
