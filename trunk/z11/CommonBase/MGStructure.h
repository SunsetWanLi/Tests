//
//  MGStructure.h
//  CommonBase
//
//  Created by Wang ZhiPeng on 11-5-21.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once
#ifndef _MG_STRUCTURE_H_
#define _MG_STRUCTURE_H_
#include "CommonDef.h"

namespace z11 {
	typedef struct _MGRange 
	{
		uint_32 location;
		uint_32 length;
		_MGRange(uint_32 loc=0,uint_32 len=0):location(loc),length(len){}
		bool operator==(const _MGRange& range)
		{
			return location == range.location && length == range.length;
		}
		bool operator!=(const _MGRange& range)
		{
			return location != range.location || length != range.length;
		}
        bool operator>(const _MGRange& range)
		{
            if (location == range.location)
                return length > range.length;
			return location < range.location && length >= range.length;
		}
		bool hasValue(uint_32 value)
		{
			return value >= location && value < location+length; 
		}
        uint_32 end()
        {
            return location+length;
        }
	}MGRange;

	typedef struct _MGPoint
	{
        _MGPoint():x(0.0f),y(0.0f){}
		float x;
		float y;
		bool operator==(const _MGPoint& point)
		{
			return x == point.x && y == point.y;
		}
		bool operator!=(const _MGPoint& point)
		{
			return x != point.x || y != point.y;
		}
	}MGPoint;
    
    float distanceOfTwoPoint(const MGPoint& point1, const MGPoint& point2);

	typedef struct _MGIntPoint
	{
        _MGIntPoint():x(0),y(0){}
		int_32 x;
		int_32 y;
		bool operator==(const _MGIntPoint& point)
		{
			return x == point.x && y == point.y;
		}
		bool operator!=(const _MGIntPoint& point)
		{
			return x != point.x || y != point.y;
		}
	}MGIntPoint;
    
    typedef struct _MGUIntPoint
	{
        _MGUIntPoint():x(0),y(0){}
		uint_32 x;
		uint_32 y;
		bool operator==(const _MGUIntPoint& point)
		{
			return x == point.x && y == point.y;
		}
		bool operator!=(const _MGUIntPoint& point)
		{
			return x != point.x || y != point.y;
		}
	}MGUIntPoint;
    
    typedef struct _MGDoublePoint
	{
        _MGDoublePoint():x(0),y(0){}
		double x;
		double y;
		bool operator==(const _MGDoublePoint& point)
		{
			return x == point.x && y == point.y;
		}
		bool operator!=(const _MGDoublePoint& point)
		{
			return x != point.x || y != point.y;
		}
	}MGDoublePoint;
    
    typedef struct _MGLongPoint
	{
        _MGLongPoint():x(0),y(0){}
		int_64 x;
		int_64 y;
		bool operator==(const _MGLongPoint& point)
		{
			return x == point.x && y == point.y;
		}
		bool operator!=(const _MGLongPoint& point)
		{
			return x != point.x || y != point.y;
		}
	}MGLongPoint, MGInt64Point;

	typedef struct _MGIntRect
	{
        _MGIntRect():x(0),y(0),width(0),height(0){}
		int x;
		int y;
		int width;
		int height;
	}MGIntRect;
    
    typedef struct _MGUIntRect
	{
        _MGUIntRect():x(0),y(0),width(0),height(0){}
		uint_32 x;
		uint_32 y;
		uint_32 width;
		uint_32 height;
	}MGUIntRect;

	typedef struct _MGRectThreeAxis
	{
        _MGRectThreeAxis():axis_x(0.0f),axis_y(0.0f),axis_z(0.0f){}
		float axis_x;
		float axis_y;
		float axis_z;
		bool operator==(const _MGRectThreeAxis& axis)
		{
			return axis_x == axis.axis_x && axis_y == axis.axis_y && axis_z == axis.axis_z;
		}
		bool operator!=(const _MGRectThreeAxis& axis)
		{
			return axis_x != axis.axis_x || axis_y != axis.axis_y || axis_z != axis.axis_z;
		}
	}MGRectTransition, MGRectScale;
    
    typedef struct _MGRectThreeAxisWithAngle
	{
        _MGRectThreeAxisWithAngle():axis_x(0.0f),axis_y(0.0f),axis_z(0.0f),angle(0.0f){}
		float axis_x;
		float axis_y;
		float axis_z;
        float angle;
		bool operator==(const _MGRectThreeAxisWithAngle& axis)
		{
			return axis_x == axis.axis_x && axis_y == axis.axis_y && axis_z == axis.axis_z && angle == axis.angle;
		}
		bool operator!=(const _MGRectThreeAxisWithAngle& axis)
		{
			return axis_x != axis.axis_x || axis_y != axis.axis_y || axis_z != axis.axis_z || angle != axis.angle;
		}
	}MGRectRotate;

	MGRectRotate MGRectRotateMake(float _x, float _y, float _z, float _angle);
	MGRectTransition MGRectTransitionMake(float _x, float _y, float _z);
	MGRectScale MGRectScaleMake(float _x, float _y, float _z);

	class MGSize
	{
	public:
		float        width;
		float        height;
		MGSize():width(0.0f),height(0.0f)
		{
		}
		MGSize(float _width,float _height):width(_width),height(_height)
		{
		}
		bool operator==(const MGSize& _size)
		{
			return width == _size.width && height == _size.height ;
		}
		bool operator!=(const MGSize& _size)
		{
			return width != _size.width || height != _size.height ;
		}
	};
    class MGDoubleSize
	{
	public:
		double        width;
		double        height;
		MGDoubleSize():width(0.0),height(0.0)
		{
		}
		MGDoubleSize(double _width,double _height):width(_width),height(_height)
		{
		}
		bool operator==(const MGDoubleSize& _size)
		{
			return width == _size.width && height == _size.height ;
		}
		bool operator!=(const MGDoubleSize& _size)
		{
			return width != _size.width || height != _size.height ;
		}
	};
	class MGIntSize
	{
	public:
		int        width;
		int        height;
		MGIntSize():width(0),height(0)
		{
		}
		MGIntSize(long _width,long _height):width(_width),height(_height)
		{
		}
		bool operator==(const MGIntSize& _size)
		{
			return width == _size.width && height == _size.height ;
		}
		bool operator!=(const MGIntSize& _size)
		{
			return width != _size.width || height != _size.height ;
		}
	};
	typedef struct _MGRect
	{
		float x;
		float y;
		float width;
		float height;
	public:
		_MGRect(){x = 0.0f; y = 0.0f; width = 0.0f; height = 0.0f;}
		_MGRect(MGPoint pt,MGSize size)
		{
			x		=	pt.x;
			y		=	pt.y;
			width	=	static_cast<float>(size.width);
			height	=	static_cast<float>(size.height);
		}
		bool operator==(const _MGRect& rect)
		{
			return x == rect.x && y == rect.y && width == rect.width && height == rect.height;
		}
		bool operator!=(const _MGRect& rect)
		{
			return x != rect.x || y != rect.y || width != rect.width || height != rect.height;
		}
		const _MGRect& operator =(const _MGRect& rect);
	}MGRect;
    
	typedef struct _MGLongRect
	{
	public:
		long    left;
		long    top;
		long    right;
		long    bottom;
		_MGLongRect (long _left,long _top,long _right,long _bottom)
		{
			left=_left;
			top=_top;
			right=_right;
			bottom=_bottom;
		}
		_MGLongRect(const MGRect &rect)
		{
			this->left=static_cast<int>(rect.x);
			this->top=static_cast<int>(rect.y);
			this->right=static_cast<int>(rect.x+rect.width);
			this->bottom=static_cast<int>(rect.y+rect.height);
		}
		_MGLongRect()
		{
			left=top=right=bottom=0;
		}
		_MGLongRect(const _MGLongRect &rect)
		{
			*this=rect;
		}
	}MGRect2, MGLongRect;//MGRect2
    
    typedef struct _MGDoubleRect
	{
		double x;
		double y;
		double width;
		double height;
	public:
		_MGDoubleRect(){x = 0.0; y = 0.0; width = 0.0; height = 0.0;}
		_MGDoubleRect(MGDoublePoint pt,MGDoubleSize size)
		{
			x		=	pt.x;
			y		=	pt.y;
			width	=	static_cast<double>(size.width);
			height	=	static_cast<double>(size.height);
		}
		bool operator==(const _MGDoubleRect& rect)
		{
			return x == rect.x && y == rect.y && width == rect.width && height == rect.height;
		}
		bool operator!=(const _MGDoubleRect& rect)
		{
			return x != rect.x || y != rect.y || width != rect.width || height != rect.height;
		}
		const _MGDoubleRect& operator =(const _MGDoubleRect& rect);
	}MGDoubleRect;
    
	MGIntRect MGIntRectMake(int _x, int _y, int _width, int _height);
	MGRect MGRectMake(float _x, float _y, float _width, float _height);
    MGDoubleRect MGDoubleRectMake(double _x, double _y, double _width, double _height);
	MGPoint MGPointMake(float _x, float _y);
    MGDoublePoint MGDoublePointMake(double _x, double _y);
	MGIntPoint MGIntPointMake(int _x,int _y);
    MGUIntPoint MGUIntPointMake(uint_32 _x, uint_32 _y);
    MGLongPoint MGLongPointMake(int_64 _x, int_64 _y);

	typedef struct _UIEdgeInsets
	{
		float top;
		float left;
		float bottom;
		float right;
		_UIEdgeInsets():top(0.0f),left(0.0f),bottom(0.0f),right(0.0f){}
		_UIEdgeInsets(float _top,float _left,float _bottom,float _right):top(_top),left(_left),bottom(_bottom),right(_right){}
		bool isZero();
	}MGEdgeInsets;
	MGEdgeInsets  MGEdgeInsetsMake (float top,float left,float bottom,float right);

	typedef MGPoint MGOffset;

} //namespace z11 

#endif
