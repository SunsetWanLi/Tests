/*
 * =====================================================================================
 *
 *       Filename:  MKPolygonView.cpp
 *
 *    Description:  The MKPolygonView class provides the visual 
 *    representation for an MKPolygon annotation object.
 *
 *        Version:  1.0
 *        Created:  04/12/2012 02:44:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MKPolygonView.h"
namespace z11
{
    MKPolygonView::MKPolygonView()
    {
        class_name = "MKPolygonView";
        _view_flags.clip_subviews = 0;
    }
    
	MKPolygonView::~MKPolygonView()
	{

	}

	MKPolygonView* MKPolygonView::initWithPolygon( MKPolygon *polygon )
	{
		MKPolygonView *ret= new MKPolygonView();
		ret->m_polygon=polygon;
		return ret;
	}

	void MKPolygonView::createPath()
	{
		//do something
	}

	MKPolygon * MKPolygonView::polygon()
	{
		return m_polygon;
	}
    
    MKOverlay*  MKPolygonView::overlay()
    {
        return polygon();
    }
    
    void MKPolygonView::drawself()
    {
        const vector<MGPoint> &points=polygon()->points();
        MGGraphicEngine::sharedGraphicEngine()->drawFilledPolyLoopLines(points,m_fillColor,5);
    }

}


