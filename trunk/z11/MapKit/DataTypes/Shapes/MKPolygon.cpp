/*
 * =====================================================================================
 *
 *       Filename:  MKPolygon.cpp
 *
 *    Description:  The MKPolygon class represents a shape consisting of 
 *    one or more points that define a closed polygon
 *
 *        Version:  1.0
 *        Created:  04/12/2012 12:48:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MKPolygon.h"

namespace z11
{


	MKPolygon * MKPolygon::polygonWithPoints( const std::vector<MKMapPoint> &points )
	{
		MKPolygon *ret=new MKPolygon(points);
		return ret;
	}

	MKPolygon * MKPolygon::polygonWithCoordinates( const std::vector<MKLocationCoordinate2D> &coords )
	{
		MKPolygon *ret=new MKPolygon(coords);
		ret->setCoordinate(getCenterCoordinate(coords));
		//MKPolygon *ret=new MKPolygon();
		return ret;
	}

	MKPolygon::~MKPolygon()
	{

	}

	MKPolygon::MKPolygon( const std::vector<MKMapPoint> &points ) :MKMultiPoint(points)
	{
        map_shift_type=MKMapShiftGoogle;
	}

	MKPolygon::MKPolygon( const std::vector<MKLocationCoordinate2D> &coords ) :MKMultiPoint(coords)
	{
        map_shift_type=MKMapShiftGoogle;
	}

	z11::MKLocationCoordinate2D MKPolygon::coordinate()
	{
        MKLocationCoordinate2D tmp=center_coord;
        if((map_shift_type&MKMapShiftGoogle)!=0)
            tmp=MKLocationShiftProvider::applyChinaLocationShift(center_coord);
        else
            ;
        if((map_shift_type&MKMapShiftBaidu)!=0)
            ;
        else
            ;
        if((map_shift_type&MKMapShiftBing)!=0)
            ;
        else
            ;
        if((map_shift_type&MKMapShiftOSM)!=0)
            ;
        else;
        return tmp;
        assert(0);
        return MKLocationCoordinate2D();
	}

	z11::MKMapRect MKPolygon::boundingMapRect()
	{
		assert(0);
		return MKMapRect();
	}

	void MKPolygon::setCoordinate( const MKLocationCoordinate2D&coord )
	{
		center_coord=coord;
	}
}