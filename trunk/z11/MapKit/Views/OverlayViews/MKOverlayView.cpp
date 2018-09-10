/*
 * =====================================================================================
 *
 *       Filename:  MKOverlayView.cpp
 *
 *    Description:  The MKOverlayView class defines the basic behavior 
 *    associated with all overlay views
 *
 *        Version:  1.0
 *        Created:  04/12/2012 01:05:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "MKOverlayView.h"
namespace z11
{
	MKOverlayView * z11::MKOverlayView::initWithOverlay(MKOverlay* overlay)
	{
		MKOverlayView *ret=new MKOverlayView();
		ret->m_overlay=overlay;
		return ret;
	}

	MKOverlay* z11::MKOverlayView::overlay()
	{
		return m_overlay;
	}

	z11::MGPoint z11::MKOverlayView::pointForMapPoint( const MKMapPoint& mapPoint )
	{
		MGPoint ret;
		return ret;
	}

	z11::MKMapPoint z11::MKOverlayView::mapPointForPoint( const MGPoint &point )
	{
		MKMapPoint ret;
		return ret;
	}

	z11::MGRect z11::MKOverlayView::rectForMapRect( const MKMapRect &rect )
	{
		MGRect ret;
		return ret;
	}

	z11::MKMapRect z11::MKOverlayView::mapRectForRect( const MGRect &rect )
	{
		MKMapRect ret;
		return ret;
	}

	bool z11::MKOverlayView::canDrawMapRect( const MKMapRect &mapRect,MKZoomScale zoomScale )
	{
		return true;
	}

	void z11::MKOverlayView::drawMapRectInContext( const MKMapRect &mapRect,MKZoomScale zoomScale,MGCanvas *context )
	{

	}

	void z11::MKOverlayView::setNeedsDisplayInMapRect( const MKMapRect &mapRect )
	{

	}

	void MKOverlayView::setNeedsDisplayInMapRectAtZoomScale( const MKMapRect &mapRect,MKZoomScale zoomScale )
	{

	}

	MKOverlayView::MKOverlayView()
	{
		m_overlay=NULL;
		class_name = "MKOverlayView";
		_view_flags.clip_subviews = 0;
        _view_flags.user_multitouch_subview_enabled = 1;
	}

	MKOverlayView::~MKOverlayView()
	{

	}
}

