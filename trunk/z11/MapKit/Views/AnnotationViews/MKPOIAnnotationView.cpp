#include "MKPOIAnnotationView.h"
#include "../MapViews/MKTileMapView.h"
#include "../MapViews/MKMapView.h"
#include "../../Protocols/MKPOIAnnotation.h"
#include "../../../GraphicCore/MGFont.h"
namespace z11
{
	static MGFont *sharedFont()
	{
		static MGFont *font=MGFont::create(30,RGBAColorRef::black());
		return font;
	}
	MKPOIAnnotationView::MKPOIAnnotationView(MKAnnotation *annotation )
		:MKAnnotationView(annotation)
	{
		MKPOIAnnotation *poi_annotation=dynamic_cast<MKPOIAnnotation *>(annotation);
		if (poi_annotation!=NULL)
		{
			m_maximum_scale = poi_annotation -> m_maximum_scale;
			m_minimum_scale = poi_annotation -> m_minimum_scale;
			//setBackgroundColor(MGColor::fromImage(poi_annotation->m_image_path)) ;
		}
		else
		{
			m_maximum_scale = 21;
			m_minimum_scale = 0;
		}
	}


	MKPOIAnnotationView::~MKPOIAnnotationView(void)
	{
	}

	MKPOIAnnotationView * MKPOIAnnotationView::initWithAnnotation( MKAnnotation *annotation )
	{
		MKPOIAnnotationView *ret=new MKPOIAnnotationView(annotation);
		return ret;
	}

	z11::MKZoomScale MKPOIAnnotationView::getMaximumZoomScale()
	{
		return m_maximum_scale ;
	}

	void MKPOIAnnotationView::setMaximumZoomScale( MKZoomScale maximum_scale )
	{
		m_maximum_scale = maximum_scale ;
	}

	z11::MKZoomScale MKPOIAnnotationView::getMinimumZoomScale()
	{
		return m_minimum_scale;
	}

	void MKPOIAnnotationView::setMinimumZoomScale( MKZoomScale minimum_scale )
	{
		m_minimum_scale = minimum_scale;
	}

	void MKPOIAnnotationView::drawself()
	{
		if (canDrawMapRect(MKMapRect(),0))
		{
			MKAnnotationView::drawself();
			MGFont *font=sharedFont();
			font->setFaceSize(m_bounds.width);
			MKPOIAnnotation *poi_annotation=dynamic_cast<MKPOIAnnotation *>(m_annotation);
			MGGraphicEngine::sharedGraphicEngine()->drawText(poi_annotation->title(),font,
				MGRectMake(m_bounds.x+m_bounds.width+20,m_bounds.y,m_bounds.width,m_bounds.height));
		}
		else
		{
			if (m_selected)
			{
				setSelected(false);
			}
		}
	}

	bool MKPOIAnnotationView::canDrawMapRect( const MKMapRect &mapRect,MKZoomScale zoomScale )
	{
		if (!MKAnnotationView::canDrawMapRect(mapRect,zoomScale))
		{
			return false;
		}
		MKMapView *parent_view=dynamic_cast<MKMapView *>(getParentView());
		float z=1<<(21-parent_view->m_tile_map_view->z);
		if (z<m_maximum_scale+(1e-6)&&z>m_minimum_scale-(1e-6))
		{
			return true;
		}
		return false;
	}

}
