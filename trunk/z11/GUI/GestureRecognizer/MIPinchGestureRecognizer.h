/*
 * =====================================================================================
 *
 *       Filename:  MIPinchGestureRecognizer.h
 *
 *    Description:  pinch gesture detector
 *
 *        Version:  1.0
 *        Created:  04/06/2012 03:45:06 PM
 *       Revision:  none
 *       Compiler:  gcc
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

	class MIPinchGestureRecognizer:public MIGestureRecognizer
	{
	public :
		MIPinchGestureRecognizer();
		virtual ~MIPinchGestureRecognizer();
    public:
        float getScale(){return m_scale;}
        MGPoint getPinchCenter(MIView* view = NULL);
    protected:
        float m_scale;
    protected:
        MIGestureRecognizerTouchesHandle
    private:
        MITouch first_touch;
        MITouch second_touch;
    public:
        static float pinch_significant_min_distance_rate;
	};

}
