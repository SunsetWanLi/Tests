#include "CommonStruct.h"
namespace z11
{
	MGGeoRect MGGeoRectMake(int_32 x, int_32 y, int_32 width, int_32 height)
	{
		MGGeoRect rect;
		rect.x = x;
		rect.y = y;
		rect.width = width;
		rect.height = height;
		return rect;
	}
	MGGeoPoint MGGeoPointMake(int_32 x, int_32 y)
	{
		MGGeoPoint point;
		point.x = x;
		point.y = y;
		return point;
	}
}//z11