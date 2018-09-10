/*
 * =====================================================================================
 *
 *       Filename:  MKOverlay.cpp
 *
 *    Description:  The MKOverlay protocol defines a specific type of annotation that represents both a point and an area on a map
 *
 *        Version:  1.0
 *        Created:  04/11/2012 04:06:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "MKOverlay.h"
namespace z11
{
	bool MKOverlay::intersectsMapRect( const MKMapRect &mapRect )
	{
		return mapRect.intersect(boundingMapRect());
	}

}