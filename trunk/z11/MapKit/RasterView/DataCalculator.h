//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (DataCalculator.h)  
/// @brief (���ļ�ʵ�ֵĹ��ܵļ���)  
///  
///(���ļ�ʵ�ֵĹ��ܵ�����)  
///  
/// @version 0.1   (�汾����)  
/// @author        (Hu DanYuan)  
/// @date          (11-12-21)  
///  
///  
///    �޶�˵��������汾  
////////////////////////////////////////////////////////////////////////// 

#pragma once

#include "CommonStruct.h"
#include "GUI/Events/MITouch.h"
#include "CommonBase/MGStructure.h"
#include <vector>
#include <set>
#include <math.h>
namespace z11 
{
	class DataCalculator
	{
	public:
		DataCalculator();
		~DataCalculator();
	
	public:
		static bool isEqualZero(double value);
		static bool isEqual(double value_1, double value_2);
		static double rad(double d);
		static double getMeterFromGeo(double lat1, double lng1, double lat2, double lng2);

		//rect��Ա����һ��Ҫ��x,y,width, height
		template<typename _Type>
		static _Type intersectRect(const _Type &left, const _Type &right);
		static double getPerimeter(const std::vector<MGGeoPoint>& points);
		static double getDistance(const std::vector<MGGeoPoint>& points);
		static double getAcreage(const std::vector<MGGeoPoint>& points);
		static double calPolygonArea(const std::vector<MGGeoPoint> &v_points);
		static void getCentrePointFromTouches(const std::set<MITouch*>& touches, MGPoint &point);
		static void getRadiusFromTouches(const std::set<MITouch*>& touches, float &radius);
		static std::vector<MGGeoPoint> &transPointsToGeo(std::vector<MGGeoPoint> &dest, const std::vector<MGPoint> &src);
		static MGGeoPoint &transPointToGeo(MGGeoPoint &dest, const MGPoint &src);

		static const int GEO_EXPANSIVITY = 1000000;
	};

	//��ȡ����Rect���ཻ����  
	template<typename _Type>
	_Type DataCalculator::intersectRect(const _Type &left, const _Type &right)
	{
		_Type rect;

		rect.x = left.x > right.x ? left.x : right.x ;
		rect.y = left.y > right.y ? left.y : right.y ;
		rect.width = ((left.x+left.width) < (right.x+right.width) ? (left.x+left.width) : (right.x+right.width)) - rect.x;
		rect.height = ((left.y+left.height) < (right.y+right.height) ? (left.y+left.height) : (right.y+right.height)) - rect.y;
		if (rect.width<0 || rect.height<0)
			rect.x = rect.y = rect.width = rect.height;
		return rect;
	}

}//z11