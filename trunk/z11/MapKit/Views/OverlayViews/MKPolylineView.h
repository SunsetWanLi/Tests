/*
 * =====================================================================================
 *
 *       Filename:  MKPolylineView.h
 *
 *    Description:  The MKPolylineView class provides the visual 
 *    representation for an MKPolyline annotation object
 *
 *        Version:  1.0
 *        Created:  04/12/2012 02:49:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma once
#include "MKOverlayPathView.h"
#include "MapKit/DataTypes/Shapes/MKPolyline.h"
namespace z11
{
	class MKPolylineView:public MKOverlayPathView
	{
    protected:
        MKPolylineView();
	public:
		static MKPolylineView *initWithPolyline(MKPolyline *_polyline);
		virtual void createPath();
		MKPolyline *polyline();
		virtual MKOverlay*  overlay();
		virtual void drawself();
	private:
		virtual ~MKPolylineView();
		MKPolyline *m_polyline;

	};
}
