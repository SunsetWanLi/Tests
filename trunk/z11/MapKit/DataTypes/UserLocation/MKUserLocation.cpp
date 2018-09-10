/*
 * =====================================================================================
 *
 *       Filename:  MKUserLocation.cpp
 *
 *    Description:  The MKUserLocation class defines a specific type of annotation that 
 *    identifies the user’s current location. You do not create instances of this class 
 *    directly. Instead, you retrieve an existing MKUserLocation object from the userLocation 
 *    property of the map view displayed in your application.
 *
 *
 *
 *        Version:  1.0
 *        Created:  04/11/2012 03:37:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "MKUserLocation.h"
namespace z11
{
	MKLocationCoordinate2D MKUserLocation::coordinate()
	{
		return MKLocationShiftProvider::applyChinaLocationShift(MKLocationCoordinate2D(location.latitude,location.longitude));
	}
}
