/*
 * =====================================================================================
 *
 *       Filename:  MKPointAnnotation.cpp
 *
 *    Description:  The MKPointAnnotation class defines a concrete 
 *    annotation object located at a specified point
 *
 *        Version:  1.0
 *        Created:  04/12/2012 03:33:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "MKPointAnnotation.h"
namespace z11
{
	z11::MKLocationCoordinate2D z11::MKPointAnnotation::coordinate()
	{
		MKLocationCoordinate2D tmp=m_coordinate;
		if((map_shift_type&MKMapShiftGoogle)!=0)
			tmp=MKLocationShiftProvider::applyChinaLocationShift(m_coordinate);
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
// 		switch(map_shift_type){
// 		case MKMapShiftNone:
// 			int tmp_shift = map_shift_type&MKMapShiftGoogle;
// 			break;
// 		case MKMapShiftGoogle:
// 			tmp=MKLocationShiftProvider::applyChinaLocationShift(m_coordinate);
// 			break;
// 		case MKMapShiftBaidu:
// 			tmp=MKLocationShiftProvider::applyChinaLocationShift(m_coordinate);
// 			break;
// 		case MKMapShiftBing:
// 			tmp=MKLocationShiftProvider::applyChinaLocationShift(m_coordinate);
// 			break;
// 		case MKMapShiftOSM:
// 			tmp=MKLocationShiftProvider::applyChinaLocationShift(m_coordinate);
// 			break;
// 		default:
// 			break;
// 		}
		return tmp;
	}


	void MKPointAnnotation::setCoordinate( const MKLocationCoordinate2D&coord )
	{
		m_coordinate=coord;
	}

	z11::MKMapRect MKPointAnnotation::boundingMapRect()
	{
		return MKMapRect();
	}

	MKPointAnnotation::~MKPointAnnotation()
	{
		
	}
}
