/*
 * =====================================================================================
 *
 *       Filename:  MKCircle.cpp
 *
 *    Description:  The MKCircle class is a concrete overlay object 
 *    representing a circular area on a map.
 *
 *        Version:  1.0
 *        Created:  04/12/2012 02:53:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MKCircle.h"
namespace z11
{


	MKCircle * MKCircle::circleWithCenterCoordinate( const MKLocationCoordinate2D &center,MKLocationDistance radius )
	{
		MKCircle *ret=new MKCircle();
		ret->m_radius=radius;
		ret->m_center=center;
		return ret;
	}

	MKCircle * MKCircle::circleWithMapRect( const MKMapRect &mapRect )
	{
		MKCircle *ret=new MKCircle();
		assert(0);
		return ret;
	}

	z11::MKLocationCoordinate2D MKCircle::coordinate()
	{
		return m_center;
	}

	z11::MKMapRect MKCircle::boundingMapRect()
	{
		assert(0);
		return MKMapRect();
	}
}