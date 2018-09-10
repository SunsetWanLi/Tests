#include "MITapGestureRecognizer.h"
#include "../../System/Shared/MGLog.h"

#define SIGNIFICANT_MOVE 10 // 如果move小于此值，则认为还是Tap手势 //

namespace z11
{
	MITapGestureRecognizer::MITapGestureRecognizer(void)
	{
		numberOfTapsRequired = 1;
        numberOfTouchesRequired = 1;
        num_of_taps = 0;
        intervalOfTaps = MGTimeDelta::FromMilliseconds(300); // 默认Tap的间隔最大不得超过500ms才能识别，可以用户自行设置 //
        stayOfTaps = MGTimeDelta::FromMilliseconds(500);
        allowableMovement = 50;
        
        MIEvent long_interval_event;
        long_interval_event._event_type = TAP_GESTURE_LONG_INTERVAL_EVENT;
        long_interval_event._content = NULL;
        long_interval_event._responder = this;
        SELHandler selector;
        selector.sender = this;
        selector.sel_func = (SELFunc)&MITapGestureRecognizer::longIntervalCall;
        long_interval_event._handler = selector;
        interval_timer = MGTimer::timerWithTimeInterval(intervalOfTaps, long_interval_event, false);
        MIEvent long_stay_event;
        long_stay_event._event_type = TAP_GESTURE_LONG_STAY_EVENT;
        long_stay_event._content = NULL;
        long_stay_event._responder = this;
        selector.sender = this;
        selector.sel_func = (SELFunc)&MITapGestureRecognizer::longStayCall;
        long_stay_event._handler = selector;
        stay_timer = MGTimer::timerWithTimeInterval(stayOfTaps, long_stay_event, false);
        last_tap.tag = INVALID_TOUCH_TAG;
        upping = false;
	}
    
	MITapGestureRecognizer::~MITapGestureRecognizer(void)
	{
        if (interval_timer != NULL) {
            delete_and_set_null(interval_timer);
        }
        if (stay_timer != NULL) {
            delete_and_set_null(stay_timer);
        }
	}
    
    void MITapGestureRecognizer::touchesBegan( const std::set<MITouch*> &touches, const MIEvent* event)
	{
        if (interval_timer->isValid()) {
            interval_timer->invalidate();
        }
        if (stay_timer->isValid()) {
            stay_timer->invalidate();
        }
        int_32 num = m_view->touch_manager.size() + touches.size();
        if(num > numberOfTouchesRequired)
        {
            setState(MIGestureRecognizerStateFailed);
            last_tap.tag = INVALID_TOUCH_TAG;
            num_of_taps = 0;
            upping = false;
            return;
        }
        stay_timer->validate();
        if (num < numberOfTouchesRequired) {
            setState(MIGestureRecognizerStatePossible);
            return;
        }
        if (num == numberOfTouchesRequired) {
            if (num_of_taps <= 0) {
                setState(MIGestureRecognizerStatePossible);
            }else{
                if (last_tap.tag != INVALID_TOUCH_TAG && last_tap.nearestDis(touches) > allowableMovement) {
                    setState(MIGestureRecognizerStateFailed);
                    last_tap.tag = INVALID_TOUCH_TAG;
                    num_of_taps = 0;
                }else{
                    MGTimeTicks tick = (*touches.begin())->timestamp;
                    if ((tick - last_tap.timestamp) > intervalOfTaps) {
                        num_of_taps = 0;
                    }
                    setState(MIGestureRecognizerStatePossible);
                }
            }
        }
	}
    
	void MITapGestureRecognizer::touchesMoved( const std::set<MITouch*> &touches, const MIEvent* event)
	{
        std::set<MITouch*>::const_iterator it;
        for (it = touches.begin(); it != touches.end(); ++it) {
            TouchMIt t_it = m_view->touch_manager.find((*it)->tag);
            if (t_it != m_view->touch_manager.end()) {
                float dis = distanceOfTwoPoint((*it)->screen_point, t_it->second.screen_point);
                if (dis <= SIGNIFICANT_MOVE) {
                    continue;
                }
            }
            if (stay_timer->isValid()) {
                stay_timer->invalidate();
            }
            setState(MIGestureRecognizerStateFailed);
            last_tap.tag = INVALID_TOUCH_TAG;
            num_of_taps = 0;
            upping = false;
            break;
        }
	}
    
	void MITapGestureRecognizer::touchesEnded( const std::set<MITouch*> &touches, const MIEvent* event)
	{
        if (interval_timer->isValid()) {
            interval_timer->invalidate();
        }
        if (stay_timer->isValid()) {
            stay_timer->invalidate();
        }
		int_32 num = m_view->touch_manager.size();
        if (num > numberOfTouchesRequired) {
            setState(MIGestureRecognizerStateFailed);
            last_tap.tag = INVALID_TOUCH_TAG;
            num_of_taps = 0;
            upping = false;
            return;
        }
        
        if (num == numberOfTouchesRequired && state() == MIGestureRecognizerStatePossible) {
            upping = true;
        }
        if (!upping) {
            if (num < numberOfTouchesRequired && state() == MIGestureRecognizerStatePossible) {
                setState(MIGestureRecognizerStateFailed);
            }
            return;
        }
        num -= touches.size();
        if (num == 0) {
            if (state() == MIGestureRecognizerStatePossible) {
                ++num_of_taps;
                if (num_of_taps == numberOfTapsRequired) {
					last_tap = (*touches.begin());
                    setState(MIGestureRecognizerStateRecognized);
                    num_of_taps = 0;
                    upping = false;
                    return;
                }
                setState(MIGestureRecognizerStatePossible);
                last_tap = (*touches.begin());
                interval_timer->validate();
            }
        }else{
            if (state() == MIGestureRecognizerStatePossible) {
                interval_timer->validate();
                return;
            }
            setState(MIGestureRecognizerStateFailed);
            last_tap.tag = INVALID_TOUCH_TAG;
            num_of_taps = 0;
            upping = false;
        }
	}
	void MITapGestureRecognizer::touchesCancelled(const std::set<MITouch *> & touches, const MIEvent* event)
	{
		setState(MIGestureRecognizerStateFailed);
        if (interval_timer->isValid()) {
            interval_timer->invalidate();
        }
        if (stay_timer->isValid()) {
            stay_timer->invalidate();
        }
        last_tap.tag = INVALID_TOUCH_TAG;
        num_of_taps = 0;
        upping = false;
	}
    
    void MITapGestureRecognizer::reset()
    {
        setState(MIGestureRecognizerStatePossible);
        if (interval_timer->isValid()) {
            interval_timer->invalidate();
        }
        if (stay_timer->isValid()) {
            stay_timer->invalidate();
        }
        interval_timer->setTimerInterval(intervalOfTaps);
        stay_timer->setTimerInterval(stayOfTaps);
        last_tap.tag = INVALID_TOUCH_TAG;
        num_of_taps = 0;
        upping = false;
    }
    
    void MITapGestureRecognizer::longIntervalCall(z11::MGObject *sender)
    {
        setState(MIGestureRecognizerStateFailed);
        last_tap.tag = INVALID_TOUCH_TAG;
        num_of_taps = 0;
        upping = false;
    }
    
    void MITapGestureRecognizer::longStayCall(z11::MGObject *sender)
    {
        setState(MIGestureRecognizerStateFailed);
        last_tap.tag = INVALID_TOUCH_TAG;
        num_of_taps = 0;
        upping = false;
    }
}
