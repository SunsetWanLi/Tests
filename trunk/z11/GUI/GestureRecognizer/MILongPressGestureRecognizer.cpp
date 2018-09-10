#include "MILongPressGestureRecognizer.h"
#include "../../System/Shared/MGLog.h"

namespace z11
{
	MILongPressGestureRecognizer::MILongPressGestureRecognizer(void)
	{
		numberOfTapsRequired = 0;
        numberOfTouchesRequired = 1;
        minimumPressDuration = MGTimeDelta::FromSecondsF(0.5); // 默认LongPress的最小时间，需要超出1秒钟 //
        allowableMovement = 10; // 如果move小于此值，则认为还是LongPress手势 //
        num_of_taps = 0;
        intervalOfTaps = MGTimeDelta::FromMilliseconds(300);
        
        MIEvent long_press_event;
        long_press_event._event_type = LONG_PRESS_GESTURE_EVENT;
        long_press_event._content = NULL;
        long_press_event._responder = this;
        SELHandler selector;
        selector.sender = this;
        selector.sel_func = (SELFunc)&MILongPressGestureRecognizer::longPressCall;
        long_press_event._handler = selector;
        long_press_timer = MGTimer::timerWithTimeInterval(minimumPressDuration, long_press_event, false);
	}
    
	MILongPressGestureRecognizer::~MILongPressGestureRecognizer(void)
	{
        if (long_press_timer != NULL) {
            delete_and_set_null(long_press_timer);
        }
	}
    
    void MILongPressGestureRecognizer::touchesBegan( const std::set<MITouch*> &touches, const MIEvent* event)
	{
        if(m_view->touch_manager.size() > 0)
        {
            setState(MIGestureRecognizerStateFailed);
            return;
        }
		int_32 num = touches.size();
        if (num == numberOfTouchesRequired) {
            if (num_of_taps > 0) {
                MGTimeTicks tick = (*touches.begin())->timestamp;
                if ((tick - last_tick) > intervalOfTaps) {
                    num_of_taps = 0;
                }
            }
            if (num_of_taps == numberOfTapsRequired) {
                long_press_timer->validate();
            }
            setState(MIGestureRecognizerStatePossible);
        }else{
            setState(MIGestureRecognizerStateFailed);
		}


		m_touch_in_view.x=0;
		m_touch_in_view.y=0;
		std::set<MITouch*>::iterator it;
		for (it=touches.begin();
			it!=touches.end();++it)
		{
			m_touch_in_view.x+=(*it)->screen_point.x;
			m_touch_in_view.y+=(*it)->screen_point.y;
		}
		m_touch_in_view.x/=touches.size();
		m_touch_in_view.y/=touches.size();
	}
    
	void MILongPressGestureRecognizer::touchesMoved( const std::set<MITouch*> &touches, const MIEvent* event)
	{
        std::set<MITouch*>::const_iterator it;
        for (it = touches.begin(); it != touches.end(); ++it) {
            TouchMIt t_it = m_view->touch_manager.find((*it)->tag);
            if (t_it != m_view->touch_manager.end()) {
                float dis = distanceOfTwoPoint((*it)->screen_point, t_it->second.screen_point);
                if (dis <= allowableMovement) {
                    continue;
                }
            }
            setState(MIGestureRecognizerStateFailed);
            if (long_press_timer->isValid()) {
                long_press_timer->invalidate();
            }
            num_of_taps = 0;
            break;
        }
	}
    
	void MILongPressGestureRecognizer::touchesEnded( const std::set<MITouch*> &touches, const MIEvent* event)
	{
		int_32 num = touches.size();
        if (num == numberOfTouchesRequired) {
            if (state() == MIGestureRecognizerStatePossible) {
                if (num_of_taps < numberOfTapsRequired) {
                    ++num_of_taps;
                    last_tick = (*touches.begin())->timestamp;
                }else{
                    if (long_press_timer->isValid()) {
                        setState(MIGestureRecognizerStateFailed);
                        long_press_timer->invalidate();
                        num_of_taps = 0;
                    }
                }
            }
        }else{
            setState(MIGestureRecognizerStateFailed);
            if (long_press_timer->isValid()) {
                long_press_timer->invalidate();
            }
            num_of_taps = 0;
        }
	}
	void MILongPressGestureRecognizer::touchesCancelled(const std::set<MITouch *> & touches, const MIEvent* event)
	{
		setState(MIGestureRecognizerStateFailed);
        if (long_press_timer->isValid()) {
            long_press_timer->invalidate();
        }
        num_of_taps = 0;
	}
    
    void MILongPressGestureRecognizer::setMinimumPressDuration(const MGTimeDelta &minimumPressDuration)
    {
        this->minimumPressDuration = minimumPressDuration;
        long_press_timer->setTimerInterval(minimumPressDuration);
    }
    
    void MILongPressGestureRecognizer::reset()
    {
        setState(MIGestureRecognizerStatePossible);
        long_press_timer->setTimerInterval(minimumPressDuration);
        num_of_taps = 0;
    }
    
    void MILongPressGestureRecognizer::longPressCall(z11::MGObject *sender)
    {
        setState(MIGestureRecognizerStateRecognized);
        num_of_taps = 0;
	}
    
	z11::MGPoint MILongPressGestureRecognizer::locationInView( MIView *view )
	{
		MGPoint point = m_touch_in_view;
        if (view != NULL) {
            MITouch c_touch;
            c_touch.screen_point = point;
            c_touch.point = point;
            c_touch.saved_point = point;
            point = c_touch.locationInView(view);
        }
		return point;
	}
}
