/*
* =====================================================================================
*
*       Filename:  MITapGestureRecognizer.h
*
*    Description:  Tap gesture detector
*
*        Version:  1.0
*        Created:  12/10/2012 04:38:06 PM
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
	class MITapGestureRecognizer :
		public MIGestureRecognizer 
	{
	public:
		MITapGestureRecognizer(void);
		virtual ~MITapGestureRecognizer(void);
        MIGestureRecognizerTouchesHandle;
    public:
        MITouch getLastTap(){return last_tap;}
    public:
        uint_8 numberOfTapsRequired;
        uint_8 numberOfTouchesRequired;
        MGTimeDelta intervalOfTaps;
        MGTimeDelta stayOfTaps;
        float allowableMovement;
    private:
        MITouch last_tap;
        int num_of_taps;
        MGTimer *interval_timer;
        MGTimer *stay_timer;
        bool upping;
    private:
        void longIntervalCall(MGObject *sender);
        void longStayCall(MGObject *sender);
	};
}