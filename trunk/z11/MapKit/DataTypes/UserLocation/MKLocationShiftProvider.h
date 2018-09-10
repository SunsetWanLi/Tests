/*
 * =====================================================================================
 *
 *       Filename:  MKLocationShiftProvider.h
 *
 *    Description:  
 *
 *
 *
 *        Version:  1.0
 *        Created:  07/05/2012 03:37:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma once
#include <string>
#include "MapKit/DataTypes/Base/MKGeometry.h"
namespace z11
{
	class MKLocationShiftProvider
	{
	public:
		static MKLocationCoordinate2D applyChinaLocationShift(const MKLocationCoordinate2D &coord);
		static bool chinaShiftEnabled;
	private:
		static void transform(double wgLat, double wgLon, double& mgLat, double& mgLon);
		static bool outOfChina(double lat, double lon);
		static double transformLat(double x, double y);
		static double transformLon(double x, double y);
		static const double earth_radius;
		static const double ee;
	};
}
