/*
* =====================================================================================
*
*       Filename:  MKPinAnnotationView.h
*
*    Description:  The MKPinAnnotationView class provides a concrete
*    annotation view that displays a pin icon like the ones found in the 
*    Maps application
*
*        Version:  1.0
*        Created:  04/12/2012 03:10:32 PM
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Lucifer (GRF), gurongfang@gmail.com
*        Company:  ZJU
*
* =====================================================================================
*/
#pragma  once
#include "MKAnnotationView.h"
namespace z11
{
	enum {
		MKPinAnnotationColorRed = 0,
		MKPinAnnotationColorGreen,
		MKPinAnnotationColorPurple
	};
	typedef uint_32 MKPinAnnotationColor;

	class MKPinAnnotationView:public MKAnnotationView
	{
	public:
		static MKPinAnnotationView *initWithAnnotation(MKAnnotation *annotation);
		MKPinAnnotationColor pinColor;
		bool animatesDrop;
	private:
		MKPinAnnotationView(MKAnnotation *_annotation);
	};

}
