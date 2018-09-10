/*
 * =====================================================================================
 *
 *       Filename:  MKPolyline.h
 *
 *    Description:  The MKPolyline class represents a shape consisting of 
 *    one or more points that define connecting line segments
 *
 *        Version:  1.0
 *        Created:  04/12/2012 12:58:57 PM
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
	class MKPolyline:public MKMultiPoint
	{
	public:
		static MKPolyline * polylineWithPoints(const std::vector<MKMapPoint>&points);
		static MKPolyline* polylineWithCoordinates(const std::vector<MKLocationCoordinate2D> &coords);
		virtual MKMapRect boundingMapRect();
		virtual MKLocationCoordinate2D coordinate();
		virtual void setCoordinate(const MKLocationCoordinate2D&coord);
    public:
		MKPolyline(const std::vector<MKLocationCoordinate2D>&coords);
        virtual ~MKPolyline(){}
	private:
		MKLocationCoordinate2D center_coord;
		MKPolyline(const std::vector<MKMapPoint>&points);
	};
}
