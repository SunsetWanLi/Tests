//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (CommonStruct.h)  
/// @brief (���ļ�ʵ�ֵĹ��ܵļ���)  
///  
///(���ļ�ʵ�ֵĹ��ܵ�����)  
///  
/// @version 0.1   (�汾����)  
/// @author        (Hu DanYuan)  
/// @date          (11-12-22)  
///  
///  
///    �޶�˵��������汾  
////////////////////////////////////////////////////////////////////////// 
#ifndef _RASTER_COMMON_STRUCT_H_
#define _RASTER_COMMON_STRUCT_H_
#pragma once

#include "CommonBase/CommonDef.h"
#include <vector>
namespace z11
{
	typedef struct _MGGeoRect
	{
		_MGGeoRect():x(0),y(0),width(0),height(0){}
		int_32 x;
		int_32 y;
		int_32 width;
		int_32 height;
	}MGGeoRect;
	typedef struct _MGGeoPoint
	{
		_MGGeoPoint():x(0),y(0){}
		int_32 x;
		int_32 y;
	}MGGeoPoint;
	MGGeoPoint MGGeoPointMake(int_32 x, int_32 y);
	MGGeoRect MGGeoRectMake(int_32 x, int_32 y, int_32 width, int_32 height);

	typedef std::vector<MGGeoPoint> MGRegion;
	typedef std::vector<MGGeoPoint>::iterator MGRegionIt;
}//z11
#endif//_RASTER_COMMON_STRUCT_H_