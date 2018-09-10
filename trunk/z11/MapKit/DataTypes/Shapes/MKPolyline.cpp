/*
 * =====================================================================================
 *
 *       Filename:  MKPolyline.cpp
 *
 *    Description:  The MKPolyline class represents a shape consisting of 
 *    one or more points that define connecting line segments
 *
 *        Version:  1.0
 *        Created:  04/12/2012 12:58:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MKPolyline.h"
#include <assert.h>
namespace z11
{

	MKPolyline::MKPolyline( const std::vector<MKMapPoint>&points ) :MKMultiPoint(points)
	{
		map_shift_type=MKMapShiftGoogle;
	}

	MKPolyline::MKPolyline( const std::vector<MKLocationCoordinate2D>&coords ) :MKMultiPoint(coords)
	{
		map_shift_type=MKMapShiftGoogle;
	}

	MKPolyline * MKPolyline::polylineWithPoints( const std::vector<MKMapPoint>&points )
	{
		MKPolyline *ret=new MKPolyline(points);
		return ret;
	}

	MKPolyline* MKPolyline::polylineWithCoordinates( const std::vector<MKLocationCoordinate2D> &coords )
	{
		//MKPolyline *ret=NULL;
		MKPolyline *ret=new MKPolyline(coords);
		ret->setCoordinate(getCenterCoordinate(coords));
		return ret;
	}

	z11::MKLocationCoordinate2D MKPolyline::coordinate()
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

	z11::MKMapRect MKPolyline::boundingMapRect()
	{
		assert(0);
		return MKMapRect();
	}

	void MKPolyline::setCoordinate( const MKLocationCoordinate2D&coord )
	{
		center_coord=coord;
	}
}