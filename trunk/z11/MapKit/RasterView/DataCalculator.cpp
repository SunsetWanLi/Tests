#include "DataCalculator.h"
#include "System/Shared/Utility.h"
#include <assert.h>

#ifndef ESPLON
#define ESPLON 0.000001f
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
const double EARTH_RADIUS = 6378137.0f; //meter
namespace z11
{
	DataCalculator::DataCalculator()
	{

	}
	DataCalculator::~DataCalculator()
	{

	}

	double DataCalculator::rad(double d)
	{
		return d * M_PI / 180.0;
	} 

	double DataCalculator::getMeterFromGeo(double lat1, double lng1, double lat2, double lng2)
	{
		double radLat1 = rad(lat1);
		double radLat2 = rad(lat2);
		double a = radLat1 - radLat2;
		double b = rad(lng1) - rad(lng2);

		double s = 2 * asin(sqrt(
			pow(sin(a / 2), 2)
			+ cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2) ));
		s = s * EARTH_RADIUS;
		s = int_64(s * 1000 + 0.5f) / 1000.0f;
		return s;	//crruracy 1m
	}

	double DataCalculator::getPerimeter(const std::vector<MGGeoPoint>& points)
	{
		int_32 size = points.size();
		double ret = 0.0f;
		if (size==0 || size==1)
			return 0.0f;
		std::vector<MGGeoPoint>::const_iterator pre_it;
		std::vector<MGGeoPoint>::const_iterator curr_it;
		for (curr_it=points.begin(),pre_it=curr_it; curr_it!=points.end(); pre_it=curr_it++) {
			if (pre_it == curr_it)
				continue;
			ret += getMeterFromGeo(pre_it->x/(1.0*GEO_EXPANSIVITY), pre_it->y/(1.0*GEO_EXPANSIVITY), 
				curr_it->x/(1.0*GEO_EXPANSIVITY), curr_it->y/(1.0*GEO_EXPANSIVITY));
		}
		ret += getMeterFromGeo(points[0].x/(1.0*GEO_EXPANSIVITY), points[0].y/(1.0*GEO_EXPANSIVITY), 
			points[size-1].x/(1.0*GEO_EXPANSIVITY), points[size-1].y/(1.0*GEO_EXPANSIVITY));
		return ret;
	}

	double DataCalculator::getDistance(const std::vector<MGGeoPoint>& points)
	{
		int_32 size = points.size();
		double ret = 0.0f;
		double comp_ret = 0.0f;
		if (size==0 || size==1)
			return 0.0f;
		std::vector<MGGeoPoint>::const_iterator pre_it;
		std::vector<MGGeoPoint>::const_iterator curr_it;
		for (curr_it=points.begin(),pre_it=curr_it; curr_it!=points.end(); pre_it=curr_it++) {
			if (pre_it == curr_it)
				continue;
			comp_ret += sqrt((double)pow((double)((*curr_it).x - (*pre_it).x) , 2) + (double)pow((double)((*curr_it).y - (*pre_it).y), 2));
			ret += getMeterFromGeo(pre_it->x/(1.0*GEO_EXPANSIVITY), pre_it->y/(1.0*GEO_EXPANSIVITY), 
				curr_it->x/(1.0*GEO_EXPANSIVITY), curr_it->y/(1.0*GEO_EXPANSIVITY));
		}
		comp_ret = comp_ret * getMeterFromGeo(0,0,1,0) / (1.0*GEO_EXPANSIVITY);
		return ret;
	}

	double DataCalculator::getAcreage(const std::vector<MGGeoPoint>& points)
	{
		double ret = calPolygonArea(points);
		return ret;
	}

	double DataCalculator::calPolygonArea(const std::vector<MGGeoPoint> &v_points)
	{
		if (v_points.size() < 3)
			return 0.0;
		double s = 0.0;
		int_32 size = v_points.size();

		for(int_32 i=1; i<size; i++)
			s += getMeterFromGeo(0, 0, (v_points[i].y+v_points[i-1].y)/(1.0*GEO_EXPANSIVITY), 0) 
			* getMeterFromGeo(0, 0, (v_points[i].x-v_points[i-1].x)/(1.0*GEO_EXPANSIVITY), 0);
			//s += (v_points[i].y + v_points[i-1].y) * (v_points[i].x - v_points[i-1].x);

		//if (v_points[0] != v_points[size - 1])
		//s += (v_points[0].y + v_points[size - 1].y) * (v_points[0].x - v_points[size - 1].x);
		s += getMeterFromGeo(0, 0, (v_points[0].y+v_points[size-1].y)/(1.0*GEO_EXPANSIVITY), 0) 
			* getMeterFromGeo(0, 0, (v_points[0].x-v_points[size-1].x)/(1.0*GEO_EXPANSIVITY), 0);

		return fabs(s / 2.0);
	}

	//获得touches的几何中心    
	void DataCalculator::getCentrePointFromTouches(const std::set<MITouch*>& touches, MGPoint &point)
	{
		std::set<MITouch*>::const_iterator touch_it;
		point.x = point.y = 0;
		if (touches.size() == 0)
			return;
		for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
			assert((*touch_it) != NULL);
			point.x += (*touch_it)->point.x;
			point.y += (*touch_it)->point.y;
		}
		point.x = point.x / touches.size();
		point.y = point.y / touches.size();
	}

	//获得touches的等效半径 
	void DataCalculator::getRadiusFromTouches(const std::set<MITouch*>& touches, float &radius)
	{
		radius = 0.0f;
		if (touches.size() == 0)
			return;
		MGPoint centra_point;
		getCentrePointFromTouches(touches, centra_point);
		std::set<MITouch*>::const_iterator touch_it;
		for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
			assert((*touch_it) != NULL);
			radius += sqrtf((float)pow((centra_point.x - (*touch_it)->point.x) , 2) + (float)pow((centra_point.y - (*touch_it)->point.y), 2));
		}
		radius = radius / touches.size();
	}

	std::vector<MGGeoPoint> &DataCalculator::transPointsToGeo(vector<MGGeoPoint> &dest, const vector<MGPoint> &src)
	{
		dest.clear();
		vector<MGPoint>::const_iterator const_it;
		for (const_it=src.begin(); const_it!=src.end(); const_it++)
		{
			MGGeoPoint geo_point;
			dest.push_back(transPointToGeo(geo_point, *const_it));
		}
		return dest;
	}

	MGGeoPoint &DataCalculator::transPointToGeo(MGGeoPoint &dest, const MGPoint &src)
	{
		dest = MGGeoPointMake(src.x*DataCalculator::GEO_EXPANSIVITY, src.y*DataCalculator::GEO_EXPANSIVITY);
		return dest;
	}

	bool DataCalculator::isEqualZero(double value)
	{
		return isEqual(value, 0.0);
	}

	bool DataCalculator::isEqual(double value_1, double value_2)
	{
		bool ret = false;
		if (value_1 > (value_2-ESPLON) && value_1 < (value_2+ESPLON)) {
			ret = true;
		}
		return ret;
	}
}