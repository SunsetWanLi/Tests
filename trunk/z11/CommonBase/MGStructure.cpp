//
//  MGStructure.cpp
//  CommonBase
//
//  Created by Wang ZhiPeng on 11-5-23.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MGStructure.h"
#include <math.h>

namespace z11
{
    MGRect MGRectMake(float _x, float _y, float _width, float _height)
    {
        MGRect ret;
        ret.x = _x;
        ret.y = _y;
        ret.width = _width;
        ret.height = _height;
        return ret;
    }
    MGDoubleRect MGDoubleRectMake(double _x, double _y, double _width, double _height)
    {
        MGDoubleRect ret;
        ret.x = _x;
        ret.y = _y;
        ret.width = _width;
        ret.height = _height;
        return ret;
    }
	MGIntRect MGIntRectMake(int _x, int _y, int _width, int _height)
	{
		MGIntRect ret;
		ret.x = _x;
		ret.y = _y;
		ret.width = _width;
		ret.height = _height;
		return ret;
	}
    MGPoint MGPointMake(float _x, float _y)
    {
        MGPoint ret;
        ret.x = _x;
        ret.y = _y;
        return ret;
	}
    
    MGDoublePoint MGDoublePointMake(double _x, double _y)
    {
        MGDoublePoint ret;
        ret.x = _x;
        ret.y = _y;
        return ret;
	}
	
	MGIntPoint MGIntPointMake(int _x,int _y)
	{
		MGIntPoint pt;
		pt.x = _x;
		pt.y = _y;
		return pt;
	}
    
    MGUIntPoint MGUIntPointMake(uint_32 _x, uint_32 _y)
	{
		MGUIntPoint pt;
		pt.x = _x;
		pt.y = _y;
		return pt;
	}
    
    MGLongPoint MGLongPointMake(int_64 _x, int_64 _y)
	{
		MGLongPoint pt;
		pt.x = _x;
		pt.y = _y;
		return pt;
	}
    
    MGRectRotate MGRectRotateMake(float _x, float _y, float _z, float _angle)
    {
        MGRectRotate ret;
        ret.axis_x = _x;
        ret.axis_y = _y;
        ret.axis_z = _z;
        ret.angle = _angle;
        return ret;
    }
    MGRectTransition MGRectTransitionMake(float _x, float _y, float _z)
    {
        MGRectTransition ret;
        ret.axis_x = _x;
        ret.axis_y = _y;
        ret.axis_z = _z;
        return ret;
    }
    MGRectScale MGRectScaleMake(float _x, float _y, float _z)
    {
        MGRectScale ret;
        ret.axis_x = _x;
        ret.axis_y = _y;
        ret.axis_z = _z;
        return ret;
    }
    
    float distanceOfTwoPoint(const MGPoint& point1, const MGPoint& point2)
    {
        float ret = 0.0f;
        ret = sqrtf(pow((point1.x - point2.x) , 2) + pow((point1.y - point2.y), 2));
		return ret;
    }

	const _MGRect& _MGRect::operator=( const _MGRect& rect )
	{
		x=rect.x;
		y=rect.y;
		width=rect.width;
		height=rect.height;
		return *this;
	}

	MGEdgeInsets  MGEdgeInsetsMake (float top,float left,float bottom,float right)
	{
		return MGEdgeInsets(top,left,bottom,right);
	}

	inline bool _isZero(float a)
	{
		if (a<0.00001f&&a>-0.00001f)
		{
			return true;
		}
		return false;
	}
	bool _UIEdgeInsets::isZero()
	{
		return _isZero(top)&&_isZero(left)&&_isZero(bottom)&&_isZero(right);
	}

}