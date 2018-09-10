/*
* =====================================================================================
*
*       Filename:  MILongPressGestureRecognizer.h
*
*    Description:  Long Press gesture detector
*
*        Version:  1.0
*        Created:  14/10/2012 10:30:06 AM
*       Revision:  none
*       Compiler:  vc
*
*         Author:  Mephi (WangZhipeng), asdyking@me.com
*        Company:  ZJU
*
* =====================================================================================
*/
#pragma once
#include "MIGestureRecognizer.h"
#include "../../System/Timer/MGTimer.h"

namespace z11
{
	class MILongPressGestureRecognizer :
		public MIGestureRecognizer 
	{
	public:
		MILongPressGestureRecognizer(void);
		virtual ~MILongPressGestureRecognizer(void);
        void setMinimumPressDuration(const MGTimeDelta &minimumPressDuration);
        MIGestureRecognizerTouchesHandle;
        
    public:
        uint_8 numberOfTapsRequired;
        uint_8 numberOfTouchesRequired;
        float allowableMovement;    // move时超出这个距离则认为手势失败 //
		MGTimeDelta intervalOfTaps;
		MGPoint locationInView(MIView *view);
        
    private:
        MGTimeDelta minimumPressDuration;
        MGTimeTicks last_tick;
        int num_of_taps;
		MGTimer* long_press_timer;
		MGPoint m_touch_in_view;
    private:
        void longPressCall(MGObject *sender);
	};
}