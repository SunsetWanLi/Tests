#pragma once
#include "MKAnnotationView.h"
namespace z11
{
	class MKPOIAnnotationView : public MKAnnotationView
	{
	public:
		static MKPOIAnnotationView *initWithAnnotation(MKAnnotation *annotation);
		~MKPOIAnnotationView(void);
		MKZoomScale getMaximumZoomScale();
		void setMaximumZoomScale(MKZoomScale maximum_scale );
		MKZoomScale getMinimumZoomScale();
		void setMinimumZoomScale(MKZoomScale minimum_scale);
		virtual void drawself();
		virtual bool canDrawMapRect(const MKMapRect &mapRect,MKZoomScale zoomScale);
	private:
		MKPOIAnnotationView(MKAnnotation *annotation );
		MKZoomScale m_maximum_scale;
		MKZoomScale m_minimum_scale;
	};
}