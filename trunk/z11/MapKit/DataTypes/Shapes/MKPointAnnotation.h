/*
 * =====================================================================================
 *
 *       Filename:  MKPointAnnotation.h
 *
 *    Description:  The MKPointAnnotation class defines a concrete 
 *    annotation object located at a specified point
 *
 *        Version:  1.0
 *        Created:  04/12/2012 03:34:07 PM
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
#include "../Base/MKTypes.h"
namespace z11
{
	class MKPointAnnotation:public MKShape
	{
	public:
			virtual MKLocationCoordinate2D coordinate();
			virtual void setCoordinate(const MKLocationCoordinate2D&coord);
			virtual ~MKPointAnnotation();
			MKPointAnnotation(){map_shift_type=MKMapShiftGoogle;}
	private:
		virtual MKMapRect boundingMapRect();
		MKLocationCoordinate2D m_coordinate;
	};
}
