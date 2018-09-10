/*
 * =====================================================================================
 *
 *       Filename:  MKPolylineView.cpp
 *
 *    Description:  The MKPolylineView class provides the visual 
 *    representation for an MKPolyline annotation object
 *
 *        Version:  1.0
 *        Created:  04/12/2012 02:48:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MKPolylineView.h"
namespace z11
{
    MKPolylineView::MKPolylineView()
    {
        class_name = "MKPolylineView";
        _view_flags.clip_subviews = 0;
    }
    
	MKPolylineView * MKPolylineView::initWithPolyline( MKPolyline *_polyline )
	{
		MKPolylineView *ret = new MKPolylineView();
		ret->m_polyline=_polyline;
		return ret;
	}

	void MKPolylineView::createPath()
	{
		//do something
	}

	MKPolyline * MKPolylineView::polyline()
	{
		return m_polyline;
	}

	MKPolylineView::~MKPolylineView()
	{
		//delete_and_set_null(m_polyline);
	}

	void MKPolylineView::drawself()
	{
		//MGGraphicEngine::sharedGraphicEngine()->drawFilledRoundRect(m_bounds,RGBAColorRef::blue());
		const vector<MGPoint> &points=polyline()->points();
		MGGraphicEngine::sharedGraphicEngine()->drawDashPolyLines(points,m_strokeColor,m_lineWidth,m_lineDashPattern,m_lineDashPhase,RGBAColorMake(255,255,255,m_alpha*255));
	}

	MKOverlay* MKPolylineView::overlay()
	{
		return polyline();
	}

}