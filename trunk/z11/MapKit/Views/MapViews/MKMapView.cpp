/*
 * =====================================================================================
 *
 *       Filename:  MKMapView.cpp
 *
 *    Description:  An MKMapView object provides an embeddable map interface, similar to the one provided by the Maps application
 *
 *        Version:  1.0
 *        Created:  04/11/2012 02:57:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MKMapView.h"
//#include "MKStandardMapView.h"
#include "MKTileMapView.h"
#include "System/Shared/MGLog.h"
#include "GUI/MIControls/MIButton.h"
#include "MapKit/Fetcher/FetcherThread/FetcherThread.h"
#include "MapKit/Fetcher/Pyramid/MKGlobalMapTiles.h"
#include "MKMapLayerView.h"
#include "../OverlayViews/MKPolylineView.h"
#include "../OverlayViews/MKPolygonView.h"
#include "MapKit/DataTypes/Shapes/MKPolygon.h"
#include "System/LocationService/MGLocation.h"
#include "System/Device/MGDevice.h"
#include "../../DataTypes/UserLocation/MKUserLocation.h"
#include "GraphicCore/MGImage.h"
#include "../../DataTypes/UserLocation/MKLocationShiftProvider.h"
#include "../../Service/MKSearch.h"
#include "GUI/Animations/MIAnimationsManager.h"
#include <vector>
#include <map>
#include <algorithm>
#include "MapKit/Fetcher/StandardDataFetcher/MKStandardDataFetcher.h"
#include "MapKit/Protocols/MKPOIAnnotation.h"
#include "MapKit/Views/AnnotationViews/MKPOIAnnotationView.h"
#include "System/IO/MGFileManager.h"
#include <cmath>

namespace z11
{
    extern int z_min;
    extern int z_max;
    extern int z_base;
    
	void MKMapView::setMapType( MKMapType map_type , bool isWebData)
	{
		AUTO_LOCK_THIS;
		m_mapType = map_type;
		{
			if (m_mapType==MKMapTypeCustom||m_mapType==MKMapTypeCustomTraffic||m_mapType==MKMapTypeCustomTile||m_mapType==MKMapTypeCustomWebTile)
			{
				MKLocationShiftProvider::chinaShiftEnabled=false;
			}
			else
			{
				MKLocationShiftProvider::chinaShiftEnabled=true;
			}
			
			if (m_tile_map_view == NULL)
			{
				MKTileMapView *tile_view = new MKTileMapView();
                if(m_mapType == MKMapTypeCustomTile||m_mapType==MKMapTypeCustomWebTile){
                    tile_view->isWebData = isWebData;
                }
				m_tile_map_view =tile_view;
				m_tile_map_view->setMapType(m_mapType);
				m_tile_map_view->setCenterCoordinate(m_center_coord);
				m_tile_map_view->initWithFrame(MGRectMake(0, 0, m_frame.width, m_frame.height));
				addSubview(m_tile_map_view);

				m_overlay_layer_view=new MKOverlayLayerView();
				m_overlay_layer_view->initWithFrame(MGRectMake(0, 0, m_frame.width, m_frame.height));
				addSubview(m_overlay_layer_view);

				m_annotation_layer_view=new MKAnnotationLayerView();
				m_annotation_layer_view->initWithFrame(MGRectMake(0, 0, m_frame.width, m_frame.height));
				assert (m_annotation_layer_view!=NULL);
				addSubview(m_annotation_layer_view);

				if(m_showsUserLocation)
				{
					m_userLocation = new MKUserLocation();
					m_userLocationView = MKAnnotationView::initWithAnnotation(m_userLocation);
					m_userLocationView->canShowCallout = false;
                    MGRect user_location_view_rect = MGRectMake(0.0f, 0.0f, 0.0f, 0.0f);
                    user_location_view_rect.width = user_location_view_rect.height = 0.08f*MINVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight());
                    string user_location_view_bkg = MGBundle::mainBundle()->pathWithFileName("MapData/annotaions/m_pos.png");
                    MGImage* image_bkg = NULL;
                    if(Utility::isFileORDirExist(user_location_view_bkg))
                    {
                        image_bkg = MGImage::imageWithContentsOfFile(user_location_view_bkg);
                    }
                    if(image_bkg != NULL)
                    {
                        m_userLocationView->setBackgroundColor(MGColor::fromImage(image_bkg));
                        user_location_view_rect.height = user_location_view_rect.width/image_bkg->getWidth()*image_bkg->getHeight();
                    }
                    
					m_userLocationView->initWithFrame(user_location_view_rect);
					m_userLocationView->draw_image_mode = ImageCustomFit;
					//m_userLocationView->setCenterOffset(MGPointMake(0,-24));

					m_location_manager = new MGLocationManager();
					m_location_manager->setDelegate(this);
					if (m_simulate_gps_data != "")
					{
						m_location_manager->startUpdatingLocationWithSimulateGPSData(m_simulate_gps_data);
					}
					else
					{
						m_location_manager->startUpdatingLocation();
					}

				}
			}
			else
			{
                if(m_mapType == MKMapTypeCustomTile||m_mapType==MKMapTypeCustomWebTile){
                    m_tile_map_view->isWebData = isWebData;
                }
				m_tile_map_view->setMapType(m_mapType);
				m_tile_map_view->clear();
			}

#if !defined(DO_NOT_USE_OLD_MAP)

			if ((m_mapType==MKMapTypeCustom||m_mapType==MKMapTypeCustomTraffic)&&!initPOIData)
			{
				initPOIData=true;
				vector<MKPOIAnnotation *> annotations=MKStandardDataFetcher::getInstance()->getPOIAnnotations();
				for (uint_32 i=0;i<annotations.size();i++)
				{
					MKPOIAnnotation *annotation=annotations[i];
					MKPOIAnnotationView *annotation_view=MKPOIAnnotationView::initWithAnnotation(annotation);
					annotation_view->setMaximumZoomScale(annotation->m_maximum_scale);
					annotation_view->setMinimumZoomScale(annotation->m_minimum_scale);
					string small_image_path=MGBundle::mainBundle()->pathWithFileName("MapData/annotaions/"+annotation->m_image_path+"_small.png");
					string image_path=MGBundle::mainBundle()->pathWithFileName("MapData/annotaions/"+annotation->m_image_path+".png");
					if (!MGFileManager::defaultManager()->fileExistsAtPath(small_image_path))
					{
						small_image_path=image_path;
					}
					annotation_view->setBackgroundColor(MGColor::fromImage(small_image_path));
					annotation_view->initWithFrame(MGRectMake(0,0,24,24));

					initAnnotationView(annotation_view);
					{
						MIButton *left=new MIButton();
						left->initWithFrame(MGRectMake(0,0,48,48));
						left->setBackgroundColor(MGColor::fromImage(image_path));
						annotation_view->setLeftCalloutAccessoryView(left);
					}
					m_annotations.push_back(annotations[i]);
					m_poi_annotations.insert(annotations[i]);
				}
			}
#endif
			
			for (uint_32 i=0;i<m_annotation_views.size();i++)
			{
				MKAnnotationView *annotaion_view=m_annotation_views[i];
				annotaion_view->setMapPoint(convertCoordinateToPoint(annotaion_view->annotation()->coordinate()));
				MGPoint center_point=convertCoordinateToPointToView(annotaion_view->annotation()->coordinate(),m_tile_map_view);
				MGPoint offset_center_point=annotaion_view->getCenterOffset();
				annotaion_view->setCenter(MGPointMake(center_point.x+offset_center_point.x,center_point.y+offset_center_point.y));
			}
            if(m_mapType == MKMapTypeCustomTile||m_mapType==MKMapTypeCustomWebTile){
                m_tile_map_view->isWebData = isWebData;
            }
			m_tile_map_view->startThread();

		}
		
	}

	z11::MKMapType MKMapView::getMapType()
	{
		return m_mapType;
	}
    
    int MKMapView::getZoom()
    {
        return m_tile_map_view->z;
    }
    
    void MKMapView::setZoom(int level, bool animated/* = true*/, float animation_duration)
    {
        if(level > z_max || level < z_min)
        {
            return;
        }
        
        float scale_value = pow(2.0f, (level-m_tile_map_view->z));
        m_tile_map_view->zoomScale(scale_value, animated, animation_duration);
    }

	void MKMapView::addAnnotation( MKAnnotation *annotation )
	{
		AUTO_LOCK_THIS;
		if (m_tile_map_view!=NULL && delegate!=NULL)
		{
			MKAnnotationView *annotaion_view=delegate->mapViewForAnnotation(annotation);
			if (annotaion_view!=NULL)
			{
				MGRectTransition trans=m_annotation_layer_view->getRectTransition();
				annotaion_view->setMapPoint(convertCoordinateToPoint(annotaion_view->annotation()->coordinate()));
				m_annotation_layer_view->addSubview(annotaion_view);
				MGPoint center_point=convertCoordinateToPointToView(annotaion_view->annotation()->coordinate(),m_tile_map_view);
				MGPoint offset_center_point=annotaion_view->getCenterOffset();
				annotaion_view->setCenter(MGPointMake(center_point.x+offset_center_point.x-trans.axis_x,center_point.y+offset_center_point.y-trans.axis_y));
				m_annotation_views.push_back(annotaion_view);
			}
			
		}
		m_annotations.push_back(annotation);
	}

	void MKMapView::addAnnotations( const std::vector<MKAnnotation *> &annotations )
	{
        AUTO_LOCK_THIS;
		for (std::vector<MKAnnotation *>::const_iterator it=annotations.begin();it!=annotations.end();it++)
		{
			addAnnotation(*it);
		}
	}

	void MKMapView::removeAnnotationView( MKAnnotation *annotation )
	{
		for (vector<MKAnnotationView *>::iterator it=m_annotation_views.begin();it!=m_annotation_views.end();it++)
		{
			if ((*it)->annotation()==annotation)
			{
				if (*it==m_annotation_layer_view->getCurrentSelectView())
				{
					m_annotation_layer_view->setCurrentSelectView(NULL);
				}

				(*it)->detachFromSuperview();
				delete_and_set_null(*it);
				m_annotation_views.erase(it);
				break;
			}
		}
	}

	void MKMapView::removeAnnotation( MKAnnotation *annotation ,bool freeAnnotation)
	{
		AUTO_LOCK_THIS;
		vector<MKAnnotation*>::iterator annotation_it = find(m_annotations.begin(), m_annotations.end(), annotation);
		if (annotation_it != m_annotations.end())
		{
			removeAnnotationView(annotation);
			if (freeAnnotation)
			{
				delete_and_set_null(annotation);
			}
			m_annotations.erase(annotation_it);
		}
	}

	void MKMapView::removeAnnotations( const std::vector<MKAnnotation *> &annotations,bool freeAnnotation )
	{
        AUTO_LOCK_THIS;
		vector<MKAnnotation *>::const_iterator annotation_it;
		for (annotation_it=annotations.begin(); annotation_it!=annotations.end(); annotation_it++) {
			removeAnnotation(*annotation_it,freeAnnotation);
		}
	}

	MKAnnotationView* MKMapView::viewForAnnotation(const MKAnnotation* const annotation) const
	{
		for (vector<MKAnnotationView*>::const_iterator it = m_annotation_views.begin(); it != m_annotation_views.end(); ++it)
		{
			if ((*it)->annotation() == annotation)
			{
				return *it;
			}
		}
		
		return NULL;
	}
    
    MKOverlayView* MKMapView::viewForOverlay(const MKOverlay* const overlay) const
    {
        for (vector<MKOverlayView*>::const_iterator it = m_overlay_views.begin(); it != m_overlay_views.end(); ++it)
		{
			if ((*it)->overlay() == overlay)
			{
				return *it;
			}
		}
		
		return NULL;
    }

	std::set<MKAnnotation *> MKMapView::annotationsInMapRect( const MKMapRect &rect )
	{
		std::set<MKAnnotation *> ret;
		return ret;
	}

	MKAnnotationView * MKMapView::dequeueReusableAnnotationViewWithIdentifier( const std::string &identifier )
	{
		return NULL;
	}

	MKMapView::MKMapView()
	{
		class_name = "MKMapView";
        setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::darkGray()));
		initPOIData=false;
		for (uint_32 i=0 ; i<MKMapTypeCount;i++)
		{
			m_buttons[i]=NULL;
		}
		m_tile_map_view=NULL;
		m_center_coord = MKLocationCoordinate2D(30.274089,120.155069);
		delegate=NULL;
		m_annotation_layer_view=NULL;
		m_overlay_layer_view=NULL;
		m_userLocation=NULL;
		m_showsUserLocation=false;
		m_location_manager=NULL;
		m_tracking_mode=MKUserTrackingModeNone;
        _view_flags.clip_subviews = 0;
        _view_flags.user_multitouch_subview_enabled = 1;

		m_map_bounds.center = MKLocationCoordinate2D(0, 0);
		m_map_bounds.span.latitudeDelta = 80;
		m_map_bounds.span.longitudeDelta = 160;
	}

	MKMapView::~MKMapView()
	{
		if (m_location_manager!=NULL)
		{
			m_location_manager->stopUpdatingLocation();
			delete_and_set_null(m_location_manager);
		}
		
		clearOverlayLayer();
		clearAnnotationLayer();
        delete_and_set_null(m_userLocation);
	}

	void MKMapView::initWithFrame( MGRect frame )
	{
		MIView::initWithFrame(frame);
	}

	void MKMapView::setCenterCoordinate( const MKLocationCoordinate2D &centerCoordinate )
	{
		m_center_coord = centerCoordinate;
		assert(_view_flags.view_initialized != 0);
		if (m_tile_map_view == NULL)
		{
            return;
			MGLogD("MKMapView::setCenterCoordinate should be called after MKMapView::setMapType");
			assert (m_tile_map_view!=NULL);
		}
		
		assert(m_tile_map_view->_view_flags.view_initialized != 0);
		m_tile_map_view->setCenterCoordinate(centerCoordinate);
	}

	const MKLocationCoordinate2D & MKMapView::centerCoordinate()
	{
		return m_center_coord;
	}

	void MKMapView::setVisibleMapRect( const MKMapRect & visual_map_rect )
	{
		//m_visibleMapRect=visual_map_rect;
	}

	const MKMapRect& MKMapView::visibleMapRect()
	{
        static MKMapRect ret;
		return ret;
	}

	void MKMapView::scrollViewDidEndZoomingWithViewAtScale( MKMapScrollView *scrollView,MIView *view,float scale )
	{
		MGLogD(" MKMapView::scrollViewDidEndZoomingWithViewAtScale scale=%f",scale);
		//adjustAnnotationView(m_current_view->getSubRectTransition(),scale);
	}

	void MKMapView::scrollViewWillEndDraggingWithVelocityTargetContentOffset( MKMapScrollView *scrollView, const MGPoint& velocity, MGPoint &targetContentOffset )
	{
		MGLogD(" MKMapView::scrollViewWillEndDraggingWithVelocityTargetContentOffset targetContentOffset=%f,%f",targetContentOffset.x,targetContentOffset.y);
	}


	z11::MGLongPoint MKMapView::convertCoordinateToPoint( const MKLocationCoordinate2D &coord )
	{
		GlobalMercator mercator;
		MGDoublePoint m=mercator.LatLonToMeters(coord.latitude,coord.longitude);
		MGLongPoint p=mercator.MetersToPixels(m.x,m.y,z_base);
		p.y= (1<<(z_base+8))-p.y;
		return p;
	}

	z11::MGPoint MKMapView::convertCoordinateToPointToView( const MKLocationCoordinate2D &coord,MIView *view )
	{
		if (view==NULL)
		{
			view=m_tile_map_view;
		}
		
		MGLongPoint annotatin_point=convertCoordinateToPoint(coord);
		MGRectTransition sub_trans=view->getSubRectTransition();
		float xoffset=annotatin_point.x+(sub_trans.axis_x-m_frame.width/2);
		float yoffset=annotatin_point.y+(sub_trans.axis_y-m_frame.height/2);
		MGRectScale sub_scale=view->getSubRectScale();
		xoffset*=sub_scale.axis_x;
		yoffset*=sub_scale.axis_y;
		return (MGPointMake(xoffset+m_frame.width/2,yoffset+m_frame.height/2));
	}

	void MKMapView::addOverlay( MKOverlay *overlay )
	{
		if (m_tile_map_view!=NULL && delegate!=NULL)
		{
			MKOverlayView *overlay_view=delegate->mapViewForOverlay(overlay);
			if (overlay_view==NULL)
			{
				return ;
			}
			
			if (dynamic_cast<MKPolygon*>(overlay)!=NULL)
			{
                MGRectTransition trans=m_overlay_layer_view->getRectTransition();
				m_overlay_layer_view->addSubview(overlay_view);
                MGPoint center_point=convertCoordinateToPointToView(overlay_view->overlay()->coordinate(),m_tile_map_view);
                overlay_view->setCenter(MGPointMake(center_point.x,center_point.y));
                
                {
                    vector<MGPoint> points;
                    vector<MKLocationCoordinate2D> coords;
                    MKPolygonView *poly_view=dynamic_cast<MKPolygonView *>(overlay_view);
                    poly_view->polygon()->getCoordinates(coords);
                    for (vector<MKLocationCoordinate2D>::iterator it=coords.begin();it!=coords.end();it++)
                    {
                        MGPoint pt=convertCoordinateToPointToView((*it),m_tile_map_view);
                        pt.x-=trans.axis_x;
                        pt.y-=trans.axis_y;
                        points.push_back(pt);
                    }
                    poly_view->polygon()->setPoints(points);
                }
                
			}
			else if(dynamic_cast<MKPolyline*>(overlay) != NULL)
			{
				MGRectTransition trans=m_overlay_layer_view->getRectTransition();
				m_overlay_layer_view->addSubview(overlay_view);
				MGPoint center_point=convertCoordinateToPointToView(overlay_view->overlay()->coordinate(),m_tile_map_view);
				overlay_view->setCenter(MGPointMake(center_point.x,center_point.y));


				{
					vector<MGPoint> points;
					vector<MKLocationCoordinate2D> coords;
					MKPolylineView *poly_view=dynamic_cast<MKPolylineView *>(overlay_view);
					poly_view->polyline()->getCoordinates(coords);
					for (vector<MKLocationCoordinate2D>::iterator it=coords.begin();it!=coords.end();it++)
					{
						MGPoint pt=convertCoordinateToPointToView((*it),m_tile_map_view);
						pt.x-=trans.axis_x;
						pt.y-=trans.axis_y;
						points.push_back(pt);
					}
					poly_view->polyline()->setPoints(points);
				}
			}
            else
            {
				m_overlay_layer_view->addSubview(overlay_view);
				MGPoint center_point=convertCoordinateToPointToView(overlay_view->overlay()->coordinate(),m_tile_map_view);
				overlay_view->setCenter(MGPointMake(center_point.x,center_point.y));
                
				MKMapRect map_rect = overlay_view->overlay()->boundingMapRect();
				
                MGPoint left_bottom_point = convertCoordinateToPointToView(MKLocationCoordinate2D(map_rect.origin.x, map_rect.origin.y), m_tile_map_view);
                MGPoint right_up_point = convertCoordinateToPointToView(MKLocationCoordinate2D(map_rect.origin.x+map_rect.size.width, map_rect.origin.y+map_rect.size.height), m_tile_map_view);
                
                MGRect overlay_frame = MGRectMake(left_bottom_point.x, right_up_point.y, right_up_point.x-left_bottom_point.x, left_bottom_point.y-right_up_point.y);
                overlay_view->setFrame(overlay_frame);
            }
            
			m_overlay_views.push_back(overlay_view);
			
		}
		m_overlays.push_back(overlay);
	}

	void MKMapView::clearAnnotationLayer()
	{
		AUTO_LOCK_THIS;
		m_annotation_layer_view->setCurrentSelectView(NULL);
		m_annotation_layer_view->detachFromSuperview();
		for (vector<MKAnnotationView *>::iterator it=m_annotation_views.begin();it!=m_annotation_views.end();it++)
		{
			(*it)->detachFromSuperview();
			delete_and_set_null(*it);
		}
		m_annotation_views.clear();
        for (vector<MKAnnotation *>::iterator it=m_annotations.begin(); it!=m_annotations.end(); it++) {
            delete_and_set_null(*it);
        }
		m_annotations.clear();
		addSubview(m_annotation_layer_view);
        //delete_and_set_null(m_userLocation);
	}

	void MKMapView::clearOverlayLayer()
	{
		m_tile_map_view->overlay_layer_view->hide();
		m_overlay_layer_view->detachFromSuperview();
		for (vector<MKOverlayView *>::iterator it=m_overlay_views.begin();it!=m_overlay_views.end();it++)
		{
			(*it)->detachFromSuperview();
			delete_and_set_null(*it);
		}
		m_overlay_views.clear();
        for (vector<MKOverlay *>::iterator it=m_overlays.begin(); it!=m_overlays.end(); it++) {
            delete_and_set_null(*it);
        }
		m_overlays.clear();
		m_overlay_layer_view->setRectTransition(MGRectTransitionMake(0,0,0));
		m_tile_map_view->overlay_layer_view->show();
		addSubview(m_overlay_layer_view);
	}

	//Accessing the Device's Current Location
	void MKMapView::setShowsUserLocation( bool shows_user_location )
	{
		if (m_showsUserLocation==shows_user_location)
		{
			return ;
		}
		
		m_showsUserLocation=shows_user_location;
	}

	bool MKMapView::showsUserLocation()
	{
		return m_showsUserLocation;
	}


	void MKMapView::updatingLocation( const MGLocation& to_location, const MGLocation& from_location )
	{
		if(m_showsUserLocation)
		{
			m_userLocation->location=to_location;
			//if(MKLocationShiftProvider::chinaShiftEnabled)
			//{
			//	MKLocationCoordinate2D shift_coords=MKLocationShiftProvider::applyChinaLocationShift(
			//		MKLocationCoordinate2D(to_location.latitude,to_location.longitude));
			//	m_userLocation->location.latitude=shift_coords.latitude;
			//	m_userLocation->location.longitude=shift_coords.longitude;
			//}
			m_userLocationView->detachFromSuperview();
			m_userLocationView->setMapPoint(convertCoordinateToPoint(m_userLocationView->annotation()->coordinate()));
			m_annotation_layer_view->addSubview(m_userLocationView);
			MGPoint center_point=convertCoordinateToPointToView(m_userLocationView->annotation()->coordinate(),m_tile_map_view);
			MGPoint offset_center_point=m_userLocationView->getCenterOffset();
			
			{
				MGRectTransition trans=m_annotation_layer_view->getRectTransition();
				//MGLogD("m_annotation_layer_view->getSubRectTransition = %f,%f",trans.axis_x,trans.axis_y);
				m_userLocationView->setCenter(
					MGPointMake(center_point.x+offset_center_point.x-trans.axis_x,center_point.y+offset_center_point.y-trans.axis_y));
				if (m_tracking_mode!=MKUserTrackingModeNone)
				{
					setCenterCoordinate(MKLocationCoordinate2D(m_userLocation->location.latitude,m_userLocation->location.longitude));
				}
				

			}

			m_userLocationView->setRectRotate(MGRectRotateMake(0,0,1,-m_userLocation->location.course));
		}
	}

	void MKMapView::updatingHeading( const MGHeading& new_heading )
	{

	}

	bool MKMapView::isUserLocationVisible()
	{
		return showsUserLocation();
	}

	MKUserLocation * MKMapView::uerLocation()
	{
		return m_userLocation;
	}

	void MKMapView::useSimulateGPSData( const std::string &simulate_gps_data )
	{
		m_simulate_gps_data=simulate_gps_data;
	}

	std::vector<MKAnnotation *> MKMapView::annotations()
	{
		return m_annotations;
	}

	void MKMapView::setUserTrackingMode( MKUserTrackingMode mode,bool animated/*=true*/ )
	{
		m_tracking_mode = mode;
	}

	z11::MKUserTrackingMode MKMapView::userTrackingMode()
	{
		return m_tracking_mode;
	}

    MIEventHandleState MKMapView::touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        if ((*touches.begin())->maintained_view != this) {
            m_tile_map_view->touchesMoved(touches, event);
            return MIEventHandleStateDone;
        }
        return MIView::touchesMoved(touches, event);
    }

	void MKMapView::initAnnotationView( MKAnnotationView *annotation_view )
	{
		if (annotation_view!=NULL)
		{
			annotation_view->setMapPoint(convertCoordinateToPoint(annotation_view->annotation()->coordinate()));
			m_annotation_layer_view->addSubview(annotation_view);
			MGPoint center_point=convertCoordinateToPointToView(annotation_view->annotation()->coordinate(),m_tile_map_view);
			MGPoint offset_center_point=annotation_view->getCenterOffset();
			annotation_view->setCenter(MGPointMake(center_point.x+offset_center_point.x,center_point.y+offset_center_point.y));
			m_annotation_views.push_back(annotation_view);
		}
	}

	std::set<MKAnnotation *> MKMapView::annotationsWithType( const std::string &type_name,const MKCoordinateRegion &region )
	{
		MKLocationCoordinate2D lt=MKLocationCoordinate2D(
			region.center.latitude+region.span.latitudeDelta,
			region.center.longitude-region.span.longitudeDelta);
		MKLocationCoordinate2D rb=MKLocationCoordinate2D(
			region.center.latitude-region.span.latitudeDelta,
			region.center.longitude+region.span.longitudeDelta);
		std::set<MKAnnotation *> ret;
		for (std::set<MKAnnotation *>::iterator it = m_poi_annotations.begin();
			it!=m_poi_annotations.end();++it)
		{
			MKPOIAnnotation *annotation=dynamic_cast<MKPOIAnnotation *>(*it);
			if(annotation!=NULL)
			{
				if (annotation->m_image_path==type_name)
				{
					MKLocationCoordinate2D coord=annotation->coordinate();
					if (coord.latitude<lt.latitude&&coord.latitude>rb.latitude)
					{
						if (coord.longitude<rb.longitude&&coord.longitude>lt.longitude)
						{
							ret.insert(annotation);
						}
						
					}
					
				}
			}
		}
		return ret;
		
	}
	void MKMapView::setRotate(MGRectRotate &rotate)
	{
		setRectRotate(rotate);
		m_tile_map_view->init(m_tile_map_view->z);
	}
	void MKMapView::setMapBounds( const MKCoordinateRegion &map_bounds )
	{
		m_map_bounds = map_bounds;
	}

	z11::MKLocationCoordinate2D MKMapView::convertPointToCoordinateFromView( const MGPoint &point,MIView *view/*=NULL*/ )
	{
		return m_tile_map_view->convertPointToCoordinateFromView(point);
	}

	void MKMapView::removeOverlay( MKOverlay *overlay )
	{
		AUTO_LOCK_THIS;
		for (vector<MKOverlayView *>::iterator it=m_overlay_views.begin();it!=m_overlay_views.end();it++)
		{
			if ((*it)->overlay()==overlay)
			{
				MKOverlayView * overlay_view=*it;
				overlay_view->detachFromSuperview();
				m_overlay_views.erase(it);
				delete_and_set_null(overlay_view);
				break;
			}
		}

		for (vector<MKOverlay*>::iterator it=m_overlays.begin();it!=m_overlays.end();it++)
		{
			if (*it==overlay)
			{
				m_overlays.erase(it);
				release_and_set_null(overlay);
				break;
			}
			
		}
		
	}









}