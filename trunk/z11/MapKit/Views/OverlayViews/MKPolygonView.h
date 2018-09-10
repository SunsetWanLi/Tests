/*
 * =====================================================================================
 *
 *       Filename:  MKPolygonView.h
 *
 *    Description:  The MKPolygonView class provides the visual 
 *    representation for an MKPolygon annotation object.
 *
 *        Version:  1.0
 *        Created:  04/12/2012 02:44:59 PM
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
#include "MapKit/DataTypes/Shapes/MKPolygon.h"
namespace z11
{
	class MKPolygonView:public MKOverlayPathView
	{
    protected:
        MKPolygonView();
	public:
		static MKPolygonView* initWithPolygon(MKPolygon *polygon);
		MKPolygon *polygon();
		virtual ~MKPolygonView();
        virtual void drawself();
        virtual MKOverlay*  overlay();
	private:
		virtual void createPath();
		MKPolygon * m_polygon;
	
	};
}
