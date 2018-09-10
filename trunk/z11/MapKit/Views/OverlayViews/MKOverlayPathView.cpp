/*
 * =====================================================================================
 *
 *       Filename:  MKOverlayPathView.cpp
 *
 *    Description:  The MKOverlayPathView class represents a generic 
 *    overlay that draws its contents using a CGPathRef data type
 *
 *        Version:  1.0
 *        Created:  04/12/2012 02:07:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MKOverlayPathView.h"
namespace z11
{
	void MKOverlayPathView::createPath()
	{

	}

	void MKOverlayPathView::invalidatePath()
	{
		delete_and_set_null(m_path);
	}

	void MKOverlayPathView::applyStrokePropertiesToContextAtZoomScale( MGContextRef *context,MKZoomScale zoomScale )
	{

	}

	void MKOverlayPathView::applyFillPropertiesToContextAtZoomScale( MGContextRef *context,MKZoomScale zoomScale )
	{

	}

	void MKOverlayPathView::strokePathInContext( MGGraphicPath *path,MGContextRef *context )
	{

	}

	void MKOverlayPathView::fillPathInContext( MGGraphicPath *path,MGContextRef *context )
	{

	}

	void MKOverlayPathView::setLineDashPattern( const vector<float> &pattern )
	{
		m_lineDashPattern=pattern;
	}

	MKOverlayPathView::MKOverlayPathView()
	{
		m_lineDashPhase=0;
		m_lineWidth=3;
		m_strokeColor=RGBAColorMake(0,0,255,128);
        class_name = "MKOverlayPathView";
        _view_flags.clip_subviews = 0;
        _view_flags.user_multitouch_subview_enabled = 1;
	}

}