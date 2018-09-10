/*
* =====================================================================================
*
*       Filename:  MISwipeGestureRecognizer.h
*
*    Description:  Swipe gesture detector
*
*        Version:  1.0
*        Created:  01/04/2013 15:45:06 PM
*       Revision:  none
*       Compiler:  llvm
*
*         Author:  Mephi (WZP), asdyking@hotmail.com
*        Company:  ZJU
*
* =====================================================================================
*/
#pragma once
#include "MIGestureRecognizer.h"
namespace z11
{
    enum {
        MISwipeGestureRecognizerDirectionRight = 1 << 0,
        MISwipeGestureRecognizerDirectionLeft  = 1 << 1,
        MISwipeGestureRecognizerDirectionUp    = 1 << 2,
        MISwipeGestureRecognizerDirectionDown  = 1 << 3
    };
    typedef uint_8 MISwipeGestureRecognizerDirection;
    
	class MISwipeGestureRecognizer :
		public MIGestureRecognizer 
	{
	public:
		MISwipeGestureRecognizer(void);
		virtual ~MISwipeGestureRecognizer(void);
    public:
        MITouch getCurrentTouch(){return begin_touch;}
		void setSwipeAngle(float _angle);
	public:
		uint_8 numberOfTouchesRequired;
        MISwipeGestureRecognizerDirection direction;
    protected:
        MIGestureRecognizerTouchesHandle
    private:
		float m_angle;
        MITouch begin_touch;
        MITouch move_touch;
    public:
        static float swipe_significant_move_rate;
	};
}