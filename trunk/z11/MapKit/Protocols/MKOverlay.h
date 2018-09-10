/*
 * =====================================================================================
 *
 *       Filename:  MKOverlay.h
 *
 *    Description:  The MKOverlay protocol defines a specific type of annotation that represents both a point and an area on a map
 *
 *        Version:  1.0
 *        Created:  04/11/2012 04:08:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma  once
#include "MKAnnotation.h"
namespace z11
{
	class MKOverlay:public MKAnnotation
	{
		public:
			virtual MKMapRect boundingMapRect()=0;
			virtual bool intersectsMapRect(const MKMapRect &mapRect);
			virtual ~MKOverlay(){}
	};
};
