/*
 * =====================================================================================
 *
 *       Filename:  MKMultiPoint.h
 *
 *    Description:  The MKMultiPoint class is an abstract superclass used 
 *    to define shapes composed of multiple points
 *
 *        Version:  1.0
 *        Created:  04/12/2012 12:28:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma once
#include <vector>
#include "MKShape.h"
#include "CommonBase/MGStructure.h"
#include "MapKit/DataTypes/UserLocation/MKLocationShiftProvider.h"
namespace z11
{
	class MKMultiPoint:public MKShape
	{
		public:
			uint_32 pointCount();
			const std::vector<MGPoint> &points();
			void getCoordinates(std::vector<MKLocationCoordinate2D > &coords);
			virtual ~MKMultiPoint()=0;
			MKMultiPoint(const std::vector<MKMapPoint>& pts);
			MKMultiPoint(const std::vector<MKLocationCoordinate2D>& coords);
			void setPoints(const std::vector<MGPoint>&points);
			static MKLocationCoordinate2D getCenterCoordinate(const std::vector<MKLocationCoordinate2D > &coords);
		protected:
			std::vector<MGPoint> m_points;
			std::vector<MKLocationCoordinate2D> m_coords;
	};
}
