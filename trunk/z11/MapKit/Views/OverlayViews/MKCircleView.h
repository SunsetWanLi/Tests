/*
 * =====================================================================================
 *
 *       Filename:  MKCircleView.h
 *
 *    Description:  The MKCircleView class provides the visual
 *    representation for an MKCircle annotation object
 *
 *        Version:  1.0
 *        Created:  04/12/2012 03:03:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "MKOverlayPathView.h"
#include "MapKit/DataTypes/Shapes/MKCircle.h"
namespace z11
{
	class MKCircleView:public MKOverlayPathView
	{
    protected:
        MKCircleView();
	public:
		static MKCircleView* initWithCircle(MKCircle *_circle);
		MKCircle *circle();
		virtual void createPath();
	private:
		MKCircle *m_circle;
	};
}
