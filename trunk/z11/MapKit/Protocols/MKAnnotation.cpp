/*
 * =====================================================================================
 *
 *       Filename:  MKAnnotation.cpp
 *
 *    Description:  The MKAnnotation protocol is used to provide annotation-related information to a map view.
 *
 *        Version:  1.0
 *        Created:  04/11/2012 03:52:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "MKAnnotation.h"
namespace z11
{
	void z11::MKAnnotation::setCoordinate( const MKLocationCoordinate2D&coord )
	{
	}


	std::string MKAnnotation::subtitle()
	{
		return "";
	}

	std::string MKAnnotation::title()
	{
		return "";
	}

	MKAnnotation::~MKAnnotation()
	{

	}
}