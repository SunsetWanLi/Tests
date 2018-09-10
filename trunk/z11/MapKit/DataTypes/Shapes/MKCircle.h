/*
 * =====================================================================================
 *
 *       Filename:  MKCircle.h
 *
 *    Description:  The MKCircle class is a concrete overlay object 
 *    representing a circular area on a map.
 *
 *        Version:  1.0
 *        Created:  04/12/2012 02:53:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma once
#include "MKShape.h"
#include "MapKit/Protocols/MKOverlay.h"
namespace z11
{
	class MKCircle:public MKShape
	{
	public:
		static MKCircle *circleWithCenterCoordinate(const MKLocationCoordinate2D &center,MKLocationDistance radius);
		static MKCircle *circleWithMapRect(const MKMapRect &mapRect);
		virtual MKMapRect boundingMapRect();
		virtual MKLocationCoordinate2D coordinate();

	private:
		MKLocationDistance m_radius;
		MKLocationCoordinate2D m_center;
	};
}
