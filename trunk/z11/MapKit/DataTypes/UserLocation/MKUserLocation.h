/*
 * =====================================================================================
 *
 *       Filename:  MKUserLocation.h
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
#include <string>
#include "../../Protocols/MKAnnotation.h"
#include "../Base/MKGeometry.h"
#include "System/LocationService/MGLocation.h"
#include "System/LocationService/MGHeading.h"
namespace z11
{
	class MKUserLocation:public MKAnnotation
	{
	public:
		MGLocation location;
		MGHeading heading;
		virtual MKLocationCoordinate2D coordinate();
		MGPROPERTY_READONLY(bool , updating,Updating);
	private:
		std::string m_title;
		std::string m_subtitle;
	};
}
