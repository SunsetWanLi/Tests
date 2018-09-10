/*
 * =====================================================================================
 *
 *       Filename:  MKPinAnnotationView.cpp
 *
 *    Description:  The MKPinAnnotationView class provides a concrete 
 *    annotation view that displays a pin icon like the ones found in 
 *    the Maps application
 *
 *        Version:  1.0
 *        Created:  04/12/2012 03:10:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MKPinAnnotationView.h"
namespace z11
{
	MKPinAnnotationView * MKPinAnnotationView::initWithAnnotation( MKAnnotation *annotation )
	{
		MKPinAnnotationView *ret=new MKPinAnnotationView(annotation);
		return ret;
	}

	MKPinAnnotationView::MKPinAnnotationView( MKAnnotation *_annotation ) :MKAnnotationView(_annotation)
	{

	}

}