/*
 * =====================================================================================
 *
 *       Filename:  MKOverlayView.h
 *
 *    Description:  The MKOverlayView class defines the basic behavior 
 *    associated with all overlay views
 *
 *        Version:  1.0
 *        Created:  04/12/2012 01:06:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma  once
#include "MapKit/Protocols/MKOverlay.h"
#include "GUI/Base/MIView.h"
namespace z11
{
	class MGCanvas;
	class MKOverlayView:public MIView
	{
	public:
		// Initializing an Overlay View
		static MKOverlayView *initWithOverlay(MKOverlay* overlay);

		// Attributes of the Overlay
		virtual MKOverlay*  overlay();

		// Converting Points on the Map
		MGPoint pointForMapPoint(const MKMapPoint& mapPoint);
		
		MKMapPoint mapPointForPoint(const MGPoint &point);

		MGRect rectForMapRect(const MKMapRect &rect);

		MKMapRect mapRectForRect(const MGRect &rect);

		//Drawing the Overlay
		virtual bool canDrawMapRect(const MKMapRect &mapRect,MKZoomScale zoomScale);

		virtual void drawMapRectInContext(const MKMapRect &mapRect,MKZoomScale zoomScale,MGCanvas *context);

		void setNeedsDisplayInMapRect(const MKMapRect &mapRect);
		void setNeedsDisplayInMapRectAtZoomScale(const MKMapRect &mapRect,MKZoomScale zoomScale);
		virtual ~MKOverlayView();
	protected:
		MKOverlayView();
		MKOverlay *m_overlay;
	};
}
