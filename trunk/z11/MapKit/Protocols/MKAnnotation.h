/*
 * =====================================================================================
 *
 *       Filename:  MKAnnotation.h
 *
 *    Description:  The MKAnnotation protocol is used to provide annotation-related information to a map view.
 *
 *        Version:  1.0
 *        Created:  04/11/2012 03:52:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma  once
#include <string>
#include "MapKit/DataTypes/Base/MKGeometry.h"
#include "MapKit/DataTypes/UserLocation/MKLocationShiftProvider.h"
#include "CommonBase/Memory/MGRefObject.h"
#include "MapKit/DataTypes/Base/MKTypes.h"


namespace z11
{
	class MKAnnotation:public MGRefObject
	{
		public:
			virtual MKLocationCoordinate2D coordinate()=0;
			virtual void setCoordinate(const MKLocationCoordinate2D&coord);
			virtual std::string subtitle();
			virtual std::string title();
			virtual ~MKAnnotation();
            MKMapShiftType map_shift_type;
	};
}
