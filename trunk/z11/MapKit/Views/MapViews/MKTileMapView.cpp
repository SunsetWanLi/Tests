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

#include "MKTileMapView.h"
#include "MapKit/Fetcher/Pyramid/MKGlobalMapTiles.h"
#include "../../Fetcher/FetcherThread/FetcherThread.h"
#include "GraphicCore/MGImage.h"
#include "MapKit/Views/MapViews/MKMapView.h"
#include <sstream>
#include "MGReusableQueue.h"
//#include "MapKit/Views/MapViews/MKStandardMapView.h"
#include "MKMapLayerView.h"
#include "GUI/GestureRecognizer/MIPinchGestureRecognizer.h"
#include "../../../GUI/GestureRecognizer/MITapGestureRecognizer.h"
#include "MapKit/Fetcher/WebDataFetcher/MKWebDataFetcher.h"
#include "System/Shared/MGLog.h"
#include "../../../System/Device/MGDevice.h"

namespace z11
{
    int z_base = 12;
    int z_max = 16;
    int z_min = 7;
    
	static float scaleValue(int zz)
	{
		float scale_value=1.0f;
		while (zz < z_base)
		{
			scale_value *= 2;
			zz++;
		}
		while (zz > z_base)
		{
			scale_value /= 2;
			zz--;
		}
		return scale_value;
	}
	MGIntPoint MKTileMapView::lefttop(int z)
	{
		return lefttop(z,getSubRectTransition());
	}

	z11::MGIntPoint MKTileMapView::lefttop( int z,MGRectTransition _trans )
	{
		MGRectTransition satelite_trans=_trans;
		if (satelite_trans.axis_x<-0.0f&&satelite_trans.axis_y<-0.0f)
		{
			float scale_value=scaleValue(z);
			MGPoint center=MGPointMake(-satelite_trans.axis_x+m_frame.width/2,
				-satelite_trans.axis_y+m_frame.height/2);
			MGPoint lefttop=MGPointMake(center.x-m_frame.width/2*scale_value,
				center.y-m_frame.height/2*scale_value);

			tile_start_x=(int_32)(lefttop.x/scale_value/MAP_CELL_SIZE);
			tile_start_y=(int_32)(lefttop.y/scale_value/MAP_CELL_SIZE);
			//reverse codec,google tile--> tile
			GlobalMercator mercator;
			MGIntPoint g=mercator.GoogleTile(tile_start_x,tile_start_y,z);
			tile_start_x=g.x;
			tile_start_y=g.y;
		}

		return MGIntPointMake(tile_start_x,tile_start_y);
	}

	void MKTileMapView::initWithFrame( MGRect frame )
	{
		z = z_base;
		GlobalMercator mercator;
		MGDoublePoint m=mercator.LatLonToMeters(m_region.center.latitude,m_region.center.longitude);
		MGIntPoint tiles=mercator.MetersToTile(m.x,m.y,z_base);
		tile_start_x=tiles.x;
		tile_start_y=tiles.y;

		MKMapScrollView::initWithFrame(frame);
		setCenterCoordinate(m_region.center);

		_scroll_view_flags.vertical_indicator_show=0;
		_scroll_view_flags.horizontal_indicator_show=0;
		_view_flags.sub_scale_enable = 1;
		tile_layer_view=new MKTileLayerView();
		tile_layer_view->initWithFrame(MGRectMake(0,0,(1<<30),(1<<30)));
		addSubview(tile_layer_view);

		adjustIndicatorInVertical();
		adjustIndicatorInHorizontal();

		overlay_layer_view=new MKOverlayScaledLayerView();
		overlay_layer_view->initWithFrame(MGRectMake(0,0,(1<<30),(1<<30)));
		addSubview(overlay_layer_view);

		map_view_delegate = this;
	}

	void MKTileMapView::scrollViewDidEndZoomingWithViewAtScale(MKMapScrollView *scrollView,MIView *view,float zoom_value)
	{
		//MGLogD(" MKTileMapView::zoom( float zoom_value )=%f",zoom_value);
		float start_zoom_value=1.0f/(1<<(z_base));
		assert(z_max > z_base);
		
		if (zoom_value < minimumZoomScale ||
            zoom_value > maximumZoomScale)
		{
			return;
		}
		
		zoom_value+=start_zoom_value/2;
		start_z=0;
		do
		{
			start_z++;
			start_zoom_value*=2;
		}while(start_zoom_value*2<zoom_value);
		//if (start_z!=z)
		{
			AUTO_LOCK_THIS;
			init(start_z);
			z=start_z;
		}


		MKMapView *map_view=getParentMapView();
		if(map_view)
		{
			map_view->m_overlay_layer_view->updateScale(this);
			map_view->m_annotation_layer_view->updateScale(this);
            if(map_view->delegate != NULL)
            {
                map_view->delegate->mapViewRegionDidChange(map_view);
            }
		}
	}
    
	void MKTileMapView::scrollViewDidEndDraggingWillDecelerate(MKMapScrollView *scrollView,bool decelerate)
	{
		MGRectTransition _sub_trans=scrollView->getSubRectTransition();
		MGPoint targetContentOffset=MGPointMake(_sub_trans.axis_x,_sub_trans.axis_y);
		//MGLogD("Transition %f",targetContentOffset.x);
		sub_trans.axis_x=targetContentOffset.x;
		sub_trans.axis_y=targetContentOffset.y;

		init(z);
		MKMapView *map_view=getParentMapView();
		if(map_view)
		{
			map_view->m_overlay_layer_view->updateTransition(this);
			map_view->m_annotation_layer_view->updateTransition(this);
		}
	}


	MKTileMapView::MKTileMapView()
	{
		class_name = "MKTileMapView";
		//assert(THREAD_NUM==1);
		for (int i=0;i<THREAD_NUM;i++)
		{
			fetcher_thread[i]=new MKFetcherThread();
			MIResponderSEL responder;
			responder.responder=this;

			SELHandler handler;
			handler.sender=this;
			handler.sel_func=(SELFunc)(&MKTileMapView::tileMapViewWillStartLoadingMap);
			responder.action=handler;
			fetcher_thread[i]->setStartResponder(responder);

			handler.sel_func=(SELFunc)(&MKTileMapView::tileMapViewDidFinishLoadingMap);
			responder.action=handler;
			fetcher_thread[i]->setFinshResponder(responder);

		}

		tile_layer_view=NULL;
		overlay_layer_view=NULL;

		tile_start_x=28352;
		tile_start_y=19244;
		start_z=z_base;

		_view_flags.clip_subviews = 0;
        isWebData = false;
		//reuse_queue=new MGReusableQueueBasedAccessTime();
	}

	MKTileMapView::~MKTileMapView()
	{
		MGLogD("in MKTileMapView::~MKTileMapView");
		for (int i=0;i<THREAD_NUM;i++)
		{
			fetcher_thread[i]->requestExit();
			fetcher_thread[i]=NULL;
			//fetcher_thread[i]->requestExitAndWait();
			//delete_and_set_null(fetcher_thread[i]);
		}
		MGLogD("out MKTileMapView::~MKTileMapView");
	}

	void MKTileMapView::init( int z )
	{
		AUTO_LOCK_THIS;
		assert(this->_view_flags.view_initialized == 1);
		if (z < z_min || z > z_max)
		{
			return;
		}
        
        // init max_scale and min_scale
        {
            const MKCoordinateRegion & region = getParentMapView()->getMapBounds();
            MGLongPoint lt = getParentMapView()->convertCoordinateToPoint(MKLocationCoordinate2D(region.center.latitude-region.span.latitudeDelta, region.center.longitude-region.span.longitudeDelta));
            MGLongPoint rb = getParentMapView()->convertCoordinateToPoint(MKLocationCoordinate2D(region.center.latitude+region.span.latitudeDelta, region.center.longitude+region.span.longitudeDelta));
            float min_scale = max(m_frame.width/fabs( (double)(rb.x-lt.x) ), m_frame.height/fabs((double)(lt.y-rb.y)) );
            
            maximumZoomScale = 1 << (z_max-z_base);
            minimumZoomScale = 1.0f / (1 << (z_base-z_min));
            if (minimumZoomScale < min_scale) {
                minimumZoomScale = min_scale;
            }
        }
        
		GlobalMercator mercator;
		MGIntPoint t;
		MGIntSize cell_size;
		{
			MGRect tmp_bounds = MGRectMake(0, 0, m_frame.width, m_frame.height);
			float angle = getParentMapView()->getRectRotate().angle;
			if((int)angle%360!=0){
				float width = getParentMapView()->getFrame().width;
				float height = getParentMapView()->getFrame().height;
				if(width>height){
					tmp_bounds.y = tmp_bounds.y-(width-height)/2 ;
					tmp_bounds.height = tmp_bounds.width;
				}
				else{
					tmp_bounds.x = tmp_bounds.x-(height-width)/2 ;
					tmp_bounds.width = tmp_bounds.height;
				}
			}
			MKLocationCoordinate2D _lt=convertPointToCoordinateFromView(MGPointMake(tmp_bounds.x,tmp_bounds.y));
			MKInternalCoordinate2D lt=GlobalMercator::get31TileNumber(_lt);
			MKLocationCoordinate2D _rb=convertPointToCoordinateFromView(MGPointMake(tmp_bounds.x+tmp_bounds.width,tmp_bounds.y+tmp_bounds.height));
			MKInternalCoordinate2D rb=GlobalMercator::get31TileNumber(_rb);
			cell_size.width=(((rb.longitude>>(23-z))-(lt.longitude>>(31-z)<<8))+255)>>8;
			cell_size.height=(((rb.latitude>>(23-z))-(lt.latitude>>(31-z)<<8))+255)>>8;
			if (cell_size.width<=0)
			{
				cell_size.width=(((1<<(8+z))-(lt.longitude>>(31-z)<<8))+255)>>8;
			}
			if (cell_size.height<=0)
			{
				cell_size.height=(((1<<(8+z))-(lt.latitude>>(31-z)<<8))+255)>>8;
			}
			
			t.x=lt.longitude>>(31-z);
			t.y=lt.latitude>>(31-z);
		}
        int all_cell_size = cell_size.width*cell_size.height;
        assert(THREAD_NUM > 0);
        int cell_per_thread = all_cell_size / THREAD_NUM;
        for (int tn = 0; tn < THREAD_NUM; ++tn) {
            Ref<MKFetcherTask> task(new MKFetcherTask());
            task->release();
            int task_value = (tn+1)*cell_per_thread;
            if ((tn+1) == THREAD_NUM) {
                task_value = all_cell_size;
            }
            for (int i=tn*cell_per_thread;i<task_value;++i)
            {
                int offset_x=(i%cell_size.width);
                int offset_y=(i/cell_size.width);
                offset_x+=t.x;
                offset_y+=t.y;
                
                MKGlobalTileIndex index(offset_x,  offset_y,z);
                index.y=((1<<index.z) - 1) - index.y;
                task->tiles.push(index);
                //MGLogD("task->tiles.push %d,%d,%d.", index.x, index.y, index.z);
            }
            task->type=getMapType();
            task->isWebData = isWebData;
            fetcher_thread[tn]->addTask(task);
        }
	}

	MKMapRect MKTileMapView::getViewPortRect()
	{
		return MKMapRect();		
	}

	MIView * MKTileMapView::hitDetect( MITouch* touch, MIEvent *event, MITouchEventType& type )
	{
		AUTO_LOCK_THIS;
		MGPoint& point = touch->point;
		MIView* ret = NULL;
		if (m_scale.axis_x == 0 || m_scale.axis_y == 0) {
			return ret;
		}
		if (this->m_hidden == true || this->m_alpha < 0.1 || _view_flags.user_interaction_enabled == 0) {
			return ret;
		}
        if (_view_flags.user_multitouch_enabled == 0 && touch_manager.size() >= 1) {
            map<MITouchTag, MITouch>::iterator it = touch_manager.begin();
            if (it->first != touch->tag) {
                return ret;
            }
        }
		MGPoint current_point = point;
		translatePoint(current_point);
		if (pointInside(current_point, event)) {
			if (ret == NULL) {
				if (can_handle_touch) {
                    ret = this;
                    touch_hitted = true;
                }
				touch->point = current_point;
			}
		}
		return ret;
	}

	MKMapView * MKTileMapView::getParentMapView()
	{
		MKMapView *parent_view=dynamic_cast<MKMapView*>(parent());
		//assert(parent_view!=NULL);
		return parent_view;
	}

	void MKTileMapView::setCenterCoordinate( const MKLocationCoordinate2D &center_coord )
	{
        if(MKLocationShiftProvider::chinaShiftEnabled)
            m_region.center = MKLocationShiftProvider::applyChinaLocationShift(MKLocationCoordinate2D(center_coord.latitude, center_coord.longitude));
        else
            m_region.center = center_coord;

		if(_view_flags.view_initialized&&!m_moved)
		{
			GlobalMercator mercator;
			MGDoublePoint m=mercator.LatLonToMeters(m_region.center.latitude,m_region.center.longitude);
			MGLongPoint pt=mercator.MetersToPixels(m.x,m.y,z_base);
			sub_trans.axis_x=-pt.x;
			sub_trans.axis_y=pt.y-((1<<z_base) )*MAP_CELL_SIZE;

			sub_trans.axis_x+=getFrame().width/2;
			sub_trans.axis_y+=getFrame().height/2;
			//MGLogD("sub_trans=(%f,%f)",sub_trans.axis_x,sub_trans.axis_y);
			//MGLogD("pt=(%f,%f)",pt.x,((1<<z_base))*MAP_CELL_SIZE-pt.y);
			setSubRectTransition(sub_trans);

			if (getParentMapView()&&getParentMapView()->m_annotation_layer_view)
			{				
				if (!getParentMapView()->m_annotation_layer_view->isHide())
				{
					//MGLogD("MKTileMapView::setCenterCoordinate updateScale");
					getParentMapView()->m_annotation_layer_view->updateScale(this);
                    getParentMapView()->m_overlay_layer_view->updateScale(this);
					init(z);
				}
			}
		}
	}

	void MKTileMapView::clear()
	{
		AUTO_LOCK_THIS;
        for (int i=0;i<THREAD_NUM;i++)
		{
			fetcher_thread[i]->clear();
            fetcher_thread[i]->clearCache();
		}
		
		init(z);
	}

	void MKTileMapView::tileMapViewWillStartLoadingMap( MGObject*mapView )
	{
		MKMapView *map_view=getParentMapView();
		if (map_view!=NULL&&map_view->delegate!=NULL)
		{
			map_view->delegate->mapViewWillStartLoadingMap(map_view);
		}
	}

	void MKTileMapView::tileMapViewDidFinishLoadingMap( MGObject*mapView )
	{
		MKMapView *map_view=getParentMapView();
		if (map_view!=NULL&&map_view->delegate!=NULL)
		{
			map_view->delegate->mapViewDidFinishLoadingMap(map_view);
		}
	}

	void MKTileMapView::startThread()
	{
		for (int i=0;i<THREAD_NUM;i++)
		{
			//MGLogD("MKTileMapView::readytorun %p",fetcher_thread[i]);
			if (!fetcher_thread[i]->isExecuted())
			{
				fetcher_thread[i]->setParentView(this->getParentMapView(),this->tile_layer_view);
				fetcher_thread[i]->run("MKTileMapView::FetchThread");
			}
		}
		init(z);
	}

	void MKTileMapView::scrollViewWillEndDraggingWithVelocityTargetContentOffset( MKMapScrollView *scrollView, const MGPoint& velocity, MGPoint &targetContentOffset )
	{
		//MGLogD("MKTileMapView::scrollViewWillEndDraggingWithVelocityTargetContentOffset");
		MKMapView *map_view=getParentMapView();
		if(map_view)
		{
			map_view->m_overlay_layer_view->updateViewTransition(this,targetContentOffset);
			map_view->m_annotation_layer_view->updateViewTransition(this,targetContentOffset);
            if (map_view->delegate != NULL) {
                map_view->delegate->scrollViewWillEndDraggingWithVelocityTargetContentOffset(scrollView,velocity,targetContentOffset);
            }
			//map_view->m_overlay_layer_view->updateViewScale(this);
			//map_view->m_annotation_layer_view->updateScale(this);
		}
	}

	void MKTileMapView::scrollViewWillBeginZoomingWithView( MKMapScrollView *scrollView,MIView *view )
	{
		//MGLogD("MKTileMapView::scrollViewWillBeginZoomingWithView");
		MKMapView *map_view=getParentMapView();
		if(map_view)
		{
			map_view->m_overlay_layer_view->updateViewScale(this);
			map_view->m_annotation_layer_view->updateViewScale(this);
		}
	}

	z11::MIEventHandleState MKTileMapView::touchesMoved( const std::set<MITouch*>& touches, const MIEvent* const event )
	{
		getParentMapView()->setUserTrackingMode(MKUserTrackingModeNone);
		return MKMapScrollView::touchesMoved(touches,event);
	}

	void MKTileMapView::scrollViewTaped(MKMapScrollView *scrollView, MITapGestureRecognizer* gesture)
	{
		if (gesture->numberOfTapsRequired == 1 && gesture->numberOfTouchesRequired == 1)
        {
			// single tap //
			MGLogD("MKTileMapView Single tap");
			MKMapView* map_view = getParentMapView();
			if(map_view != NULL)
			{
                map_view->m_annotation_layer_view->setCurrentSelectView(NULL);
                
                MGPoint touch_point = gesture->getLastTap().locationInView(map_view);
                MKAnnotationView* closest_ann_view = NULL;
                float min_distance = 999999.0f;
                for(vector<MKAnnotationView*>::iterator it = map_view->m_annotation_views.begin(); it != map_view->m_annotation_views.end(); ++it)
                {
                    if(!(*it)->isHide() && (*it)->getAlpha() > 0.1f)
                    {
                        float current_distance = distanceOfTwoPoint((*it)->getCenter(), touch_point);
                        if(current_distance < min_distance)
                        {
                            min_distance = current_distance;
                            closest_ann_view = *it;
                        }
                    }
                }
                
                if(closest_ann_view != NULL)
                {
                    float extra_value = 0.036f*MINVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight());
                    MGRect ann_frame = closest_ann_view->getFrame();
                    float ann_radius = sqrtf(ann_frame.width*ann_frame.width+ann_frame.height*ann_frame.height);
                    
                    if(min_distance < extra_value + ann_radius)
                    {
                        map_view->m_annotation_layer_view->setCurrentSelectView(closest_ann_view);
                    }
                }

				if(map_view->delegate!=NULL){
					map_view->delegate->mapViewSingleTapped(map_view,gesture);
				}

			}
		}
	}

	z11::MKLocationCoordinate2D MKTileMapView::convertPointToCoordinateFromView( const MGPoint &point )
	{
		double x=point.x;
		double y=point.y;
		x-=m_frame.width/2;
		y-=m_frame.height/2;
		MGRectScale sub_scale=getSubRectScale();
        MGRectTransition sub_trans=getSubRectTransition();
        //MGLogD("sub_scale : %f, %f; sub_trans : %f, %f.", sub_scale.axis_x, sub_scale.axis_y, sub_trans.axis_x, sub_trans.axis_y);
		x/=sub_scale.axis_x;
		y/=sub_scale.axis_y;
		x-=(sub_trans.axis_x-m_frame.width/2);
		y-=(sub_trans.axis_y-m_frame.height/2);
		if (x<0)
		{
			x=0;
			//x+=(1<<(8+z_base));
		}
		if(y<0)
		{
			y=0;
			//y+=(1<<(8+z_base));
		}
		if (x>=(1<<(8+z_base)))
		{
			x=(1<<(8+z_base))-1;
			//x-=(1<<(8+z_base));
		}
		if (y>=(1<<(8+z_base)))
		{
			y=(1<<(8+z_base))-1;
			//y-=(1<<(8+z_base));
		}
		
		x*=(1<<(31-8-z_base));
		y*=(1<<(31-8-z_base));
		
		MKLocationCoordinate2D location= GlobalMercator::get31Coordinate2D(MKInternalCoordinate2D(y,x));
		return location;
	}

}