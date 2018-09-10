/*
* =====================================================================================
*
*       Filename:  MKTileLayerOverlay.cpp
*
*    Description:  Map View Cell View
*
*        Version:  1.0
*        Created:  04/12/2012 11:39:31 AM
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Lucifer (GRF), gurongfang@gmail.com
*        Company:  ZJU
*
* =====================================================================================
*/

#include "MKTileLayerOverlay.h"
#include "GraphicCore/MGCanvas.h"
#include "MapKit/Fetcher/StandardDataFetcher/MKStandardDataFetcher.h"
#include "MapKit/Fetcher/WebDataFetcher/MKWebDataFetcher.h"
#include "MapKit/Fetcher/LocalDataFetcher/MKLocalDataFetcher.h"
#include "System/Shared/MGLog.h"
//#include "MKMapViewBase.h"
#include "GraphicCore/MGPPMCodec.h"
#include "GraphicCore/MGPNGCodec.h"
#include "System/Device/MGDevice.h"
#include "MapKit/Fetcher/Pyramid/MKGlobalMapTiles.h"
#include "MKTileMapView.h"
#include "MKMapView.h"
#include "MapKit/Fetcher/OsmMap/MKOsmMapFetcher.h"

namespace z11
{
	MKTileLayerOverlay::MKTileLayerOverlay()
	{
		m_fetch_state=MKFetchStateFailed;
		class_name = "MKTileLayerOverlay";
        _view_flags.clip_subviews = 0;
	}

	MKTileLayerOverlay::~MKTileLayerOverlay()
	{
	}

	void MKTileLayerOverlay::setFetchState( MKFetchState state )
	{
		AUTO_LOCK_THIS;
		m_fetch_state=state;
	}

	z11::MKFetchState MKTileLayerOverlay::fetchState()
	{
		AUTO_LOCK_THIS;
		return m_fetch_state;
	}

	z11::MKLocationCoordinate2D MKTileGrid::coordinate()
	{
		assert(0);
		return MKLocationCoordinate2D();
	}

	MKTileGrid * MKTileGrid::tileGridWithMapRect( const MKMapRect &rect )
	{
		MKTileGrid *ret=new MKTileGrid();
		ret->m_rect=rect;
		return ret;
	}

	z11::MKMapRect MKTileGrid::boundingMapRect()
	{
		return m_rect;
	}

	void MKTileGrid::setRect( const MKMapRect &rect )
	{
		m_rect=rect;
	}

	void MKSateliteMapViewCell::render()
	{
		MKSateliteMapViewCell *cell=this;
		MGImage *image=NULL;

#if !defined(DO_NOT_USE_OLD_MAP)
		if (getMapType()==MKMapTypeCustom)
		{
			//MGTimeLog log(" MKStandardDataFetcher::render total %d,%d,%d",m_tms_code.x,m_tms_code.y,m_tms_code.z);
			image= MKStandardDataFetcher::getInstance()->getImage(cell->m_tms_code,getMapType());
		}
		else if(getMapType()==MKMapTypeCustomTraffic)
		{
			MGImage *buffer_image = MKStandardDataFetcher::getInstance()->getImage(cell->m_tms_code,MKMapTypeCustom);
			image_canvas->setBufferImage(MGImageRef(buffer_image));

			image= MKStandardDataFetcher::getInstance()->getImage(cell->m_tms_code,MKMapTypeCustomTraffic);
			image_canvas->drawImage(image,MGRectMake(0,0,256,256));
			delete_and_set_null(image);

			image_canvas->setBufferImage(NULLREF);
			image=buffer_image;

		}
        else if(getMapType()==MKMapTypeCustomTile)
        {
            if(!isWebData)
                image= MKLocalDataFetcher::getInstance()->getImage(cell->m_tms_code,getMapType());
            else
                image= MKWebDataFetcher::getInstance()->getImage(cell->m_tms_code,getMapType());
        }
        else if(getMapType()==MKMapTypeCustomWebTile)
        {
            GlobalMercator mercator;
            MKGlobalTileIndex index = cell->m_tms_code;
            MGIntPoint g= mercator.GoogleTile(index.x, index.y, index.z);
            index.y = g.y;
            if(!isWebData)
                image= MKLocalDataFetcher::getInstance()->getImage(index,getMapType());
            else
                image= MKWebDataFetcher::getInstance()->getImage(index,getMapType());
        }
		else
#endif
			if(getMapType()==MKMapTypeHybrid)
			{
				MGImage *buffer_image = MKWebDataFetcher::getInstance()->getImage(cell->m_tms_code,MKMapTypeHybrid);
				if (buffer_image!=NULL)
				{
					image_canvas->setBufferImage(MGImageRef(buffer_image));

					if (MKWebDataFetcher::getInstance()->mapProvider()==MKMapProviderGoogle)
					{
						image= MKWebDataFetcher::getInstance()->getImage(cell->m_tms_code,MKMapTypeSatellite);
					}
					else
					{
#if defined(DO_NOT_USE_OLD_MAP)
						MKOsmMapFetcher::getInstance()->noPolygons=true;
						image= MKOsmMapFetcher::getInstance()->getImage(cell->m_tms_code);
#endif
					}

					image_canvas->drawImage(image,MGRectMake(0,0,256,256),false);
					delete_and_set_null(image);

					image_canvas->setBufferImage(NULLREF);
				}
                
                image = buffer_image;
			}
			else if(getMapType()==MKMapTypeOsm)
			{
#if defined(DO_NOT_USE_OLD_MAP)
				MKOsmMapFetcher::getInstance()->noPolygons=false;
				image= MKOsmMapFetcher::getInstance()->getImage(cell->m_tms_code);
#endif
			}
			else
			{
				image= MKWebDataFetcher::getInstance()->getImage(cell->m_tms_code,getMapType());
			}

        if (image!=NULL)
        {
            cell->setBackgroundColor(MGColor::fromImage(image));
        }

        //alpha animation
        cell->setAlpha(0.0f);
        stringstream ss;
        ss<<"MKTileMapView::reloadCell."<<(long)(cell)<<MGTimeTicks::Now().ToInternalValue();
        MIView::beginAnimation(ss.str());
        MIView::setAnimationCurve(MIViewAnimationOptionShowHideTransitionViews);
        MIView::setAnimationDuration(0.1);
        cell->setAlpha(1.0f);
        MIView::commitAnimation();
	}

	void MKSateliteMapViewCell::drawself()
	{
		MIView::drawself();
	}

	MKSateliteMapViewCell::MKSateliteMapViewCell()
	{
        isWebData = false;
        
		image_canvas=new MGCanvas();
        class_name = "MKSateliteMapViewCell";
	}

	MKTileMapView * MKSateliteMapViewCell::parentMapView()
	{
		MKTileMapView *parent_view=NULL;
		if (parent()!=NULL)
		{
			parent_view=dynamic_cast<MKTileMapView *>(parent()->parent());
		}
		return parent_view;
	}

	MKSateliteMapViewCell::~MKSateliteMapViewCell()
	{
		delete_and_set_null(image_canvas);
	}

	bool MKSateliteMapViewCell::canDrawMapRect( const MKMapRect &mapRect,MKZoomScale zoomScale )
	{
		return true;
	}
}