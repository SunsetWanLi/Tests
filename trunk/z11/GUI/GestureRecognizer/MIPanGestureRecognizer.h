/*
* =====================================================================================
*
*       Filename:  MIPanGestureRecognizer.h
*
*    Description:  pan gesture detector
*
*        Version:  1.0
*        Created:  04/06/2012 03:45:06 PM
*       Revision:  none
*       Compiler:  vc
*
*         Author:  Lucifer (GRF), gurongfang@gmail.com
*        Company:  ZJU
*
* =====================================================================================
*/
#pragma once
#include "MIGestureRecognizer.h"
namespace z11
{
	class MIPanGestureRecognizer :
		public MIGestureRecognizer 
	{
	public:
		MIPanGestureRecognizer(void);
		virtual ~MIPanGestureRecognizer(void);
	public:
		uint_8 maximumNumberOfTouches ;
		uint_8 minimumNumberOfTouches;
        MGPoint translationInView(MIView* view);
        MGTimeDelta timeDelta();
        void setTranslationInView();
        MIGestureRecognizerTouchesHandle;
	public:
		MITouch getCurrentTouch(){return move_touch;}
    private:
        MITouch begin_touch;
        MITouch move_touch;
    public:
        static float pan_significant_move_rate;
	};
}