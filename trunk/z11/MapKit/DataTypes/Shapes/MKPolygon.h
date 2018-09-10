/*
 * =====================================================================================
 *
 *       Filename:  MKPolygon.h
 *
 *    Description:  The MKPolygon class represents a shape consisting of 
 *    one or more points that define a closed polygon
 *
 *        Version:  1.0
 *        Created:  04/12/2012 12:49:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma once
#include "MKMultiPoint.h"
#include "MapKit/Protocols/MKOverlay.h"
#include "../Base/MKTypes.h"
namespace z11
{
	class MKPolygon :public MKMultiPoint
	{
	public:
		static MKPolygon * polygonWithPoints(const std::vector<MKMapPoint> &points);
		static MKPolygon * polygonWithCoordinates(const std::vector<MKLocationCoordinate2D> &coords);
		virtual MKMapRect boundingMapRect();
		virtual MKLocationCoordinate2D coordinate();
		virtual void setCoordinate(const MKLocationCoordinate2D&coord);
		virtual ~MKPolygon();
        MKMapShiftType map_shift_type;
	private:
		MKLocationCoordinate2D center_coord;
		MKPolygon(const std::vector<MKMapPoint> &points);
		MKPolygon(const std::vector<MKLocationCoordinate2D> &coords);
	};
}
