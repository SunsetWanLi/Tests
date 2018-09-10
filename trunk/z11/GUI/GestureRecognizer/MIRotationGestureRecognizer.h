/*
 * =====================================================================================
 *
 *       Filename:  MIRotateGestureRecognizer.h
 *
 *    Description:  Rotate gesture detector
 *
 *        Version:  1.0
 *        Created:  01/03/2013 03:45:06 PM
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

	class MIRotationGestureRecognizer:public MIGestureRecognizer
	{
	public :
		MIRotationGestureRecognizer();
		virtual ~MIRotationGestureRecognizer();
    public:
        float getRotation(){return m_rotation;}
        MGPoint getRotationCenter();
    protected:
        float m_rotation;
    protected:
        MIGestureRecognizerTouchesHandle
    private:
        MITouch first_touch;
        MITouch second_touch;
    public:
        static float rotation_significant_min_distance_rate;
	};

}
