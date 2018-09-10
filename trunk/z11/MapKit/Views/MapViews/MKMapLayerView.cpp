/*
 * =====================================================================================
 *
 *       Filename:  MKMapLayerView.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/09/2012 02:57:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MKMapLayerView.h"
#include "System/Shared/MGLog.h"
#include "MapKit/Views/MapViews/MKTileMapView.h"
#include "MapKit/Fetcher/Pyramid/MKGlobalMapTiles.h"
#include "MapKit/Views/AnnotationViews/MKAnnotationView.h"
#include "MapKit/Views/MapViews/MKMapView.h"
#include "MapKit/Views/OverlayViews/MKPolylineView.h"
#include "MapKit/Views/OverlayViews/MKPolygonView.h"
namespace z11
{
    
    MKMapLayerView::MKMapLayerView()
    {
        class_name = "MKMapLayerView";
        _view_flags.clip_subviews = 0;
        _view_flags.user_multitouch_subview_enabled = 1;
    }
    
    MKTileLayerView::MKTileLayerView()
    {
        class_name = "MKTileLayerView";
        _view_flags.clip_subviews = 0;
    }
    
    MKOverlayLayerView::MKOverlayLayerView()
    {
        class_name = "MKOverlayLayerView";
        _view_flags.clip_subviews = 0;
    }
    
    MKOverlayScaledLayerView::MKOverlayScaledLayerView()
    {
        class_name = "MKOverlayScaledLayerView";
        _view_flags.clip_subviews = 0;
    }

	MIView* MKAnnotationLayerView::hitDetect( MITouch* touch, MIEvent *event, MITouchEventType& type )
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
		{
				point = current_point;
				std::deque<MIView *>::reverse_iterator it;
				for (it = m_subviews.rbegin(); it !=m_subviews.rend(); ++it) {
					assert((*it) != NULL);
					if(*it != NULL)
					{
                        (*it)->can_handle_touch_from_superview = can_handle_touch_from_superview;
                        if (_view_flags.user_multitouch_subview_enabled == 0 && m_have_touches_through_subview) {
                            (*it)->can_handle_touch_from_superview = (*it)->m_have_touches_through_subview || (*it)->touch_manager.size() > 0 || (*it)->touch_hitted;
                        }
                        (*it)->can_handle_touch = (*it)->can_handle_touch_from_superview && ((*it)->touch_manager.size() > 0 || (*it)->touch_hitted || !m_have_touches_through_subview || _view_flags.user_multitouch_subview_enabled == 1);
						ret = (*it)->hitDetect(touch, event, type);
						if (ret != NULL) {
							break;
						}
					}
				}

			}
			if (ret == NULL) {
				//setCurrentSelectView(NULL);
			//	ret = this;
				touch->point = current_point;
			}
		}
		return ret;
	}

	void MKAnnotationLayerView::updateViewScale( MKTileMapView *view )
	{
		AUTO_LOCK_THIS;
        //MGLogD("==============Ready=================");
       // MGLogD("==============this %p, ParentView:%p",this, parent_view);
        //MIView *parent_view2=parent();
        //if (parent_view2==NULL) {
        //    return;
        //}
        //MGLogD("==============Begin=================");
		//parent_view=dynamic_cast<MKMapView*>(parent());
		//detachFromSuperview();
	//	this->hide();
        //MGLogD("==============this %p, ParentView:%p",this, parent_view);
	}

	void MKAnnotationLayerView::updateViewTransition( MKTileMapView *view,const MGPoint &targetContentOffset )
	{
		AUTO_LOCK_THIS;
			
		MGRectTransition trans=MGRectTransitionMake(m_transition.axis_x+targetContentOffset.x,m_transition.axis_y+targetContentOffset.y,0);
		//MGLogD("trans x=%f,y=%f",trans.axis_x,trans.axis_y);
		this->setRectTransition(trans);
		return ;

	}

	void MKAnnotationLayerView::updateTransition( MKTileMapView *view )
	{
		AUTO_LOCK_THIS;
	//	MGLogD(" MKAnnotationLayerView::updateTransition");

		SubviewIterator it;
		for (it=m_subviews.begin();it!=m_subviews.end();it++)
		{
			MIView *sub_view=(*it);
			MGPoint center=sub_view->getCenter();
			center.x+=m_transition.axis_x;
			center.y+=m_transition.axis_y;
			sub_view->setCenter(center);
		}
		this->setRectTransition(MGRectTransitionMake(0,0,0));
	}
	void MKAnnotationLayerView::updateScale( MKTileMapView *view )
	{
		AUTO_LOCK_THIS;
        //MGLogD("===============End==================");
        //MGLogD("==============this %p, ParentView:%p",this, parent_view);
		//MGLogD("MKAnnotationLayerView::update Scale");
		SubviewIterator it;
		for (it=m_subviews.begin();it!=m_subviews.end();it++)
		{
			MKAnnotationView *annotaion_view=dynamic_cast<MKAnnotationView*>(*it);
			parent_view=dynamic_cast<MKMapView *>(parent());
			if (parent_view!=NULL)
			{
				MGPoint center_point=parent_view->convertCoordinateToPointToView(annotaion_view->annotation()->coordinate(),parent_view->m_tile_map_view);
				MGPoint offset_center_point=annotaion_view->getCenterOffset();
				annotaion_view->setCenter(MGPointMake(center_point.x+offset_center_point.x,center_point.y+offset_center_point.y));
			}
		}
        //if(this->parent()==NULL)
        //    parent_view->addSubview(this);
		this->show();
	}

	MKAnnotationLayerView::MKAnnotationLayerView()
	{
        class_name = "MKAnnotationLayerView";
        _view_flags.clip_subviews = 0;
		parent_view=NULL;
		m_selected_annotation_view=NULL;
	}

	MKAnnotationLayerView::~MKAnnotationLayerView()
	{

	}

	void MKAnnotationLayerView::setCurrentSelectView( MKAnnotationView *view )
	{
		MKMapView *map_view=dynamic_cast<MKMapView *>(parent());
		if (m_selected_annotation_view!=NULL)
		{
			m_selected_annotation_view->setSelected(false);

			if (map_view!=NULL&&map_view->delegate!=NULL)
			{
				map_view->delegate->mapViewDidDeselectAnnotationView(map_view, m_selected_annotation_view);
			}
		}
		m_selected_annotation_view = view;
		if (m_selected_annotation_view!=NULL)
		{
			m_selected_annotation_view->detachFromSuperview();
			addSubview(m_selected_annotation_view);
			m_selected_annotation_view->setSelected(true);
			if (map_view!=NULL&&map_view->delegate!=NULL)
			{
				map_view->delegate->mapViewDidSelectAnnotationView(map_view, m_selected_annotation_view);
			}
			
		}
		//else
		//{
		//	if (map_view!=NULL&&map_view->delegate!=NULL)
		//	{
		//		map_view->delegate->mapViewDidDeselectAnnotationView(map_view,view);
		//	}
		//}
	}

	MKAnnotationView * MKAnnotationLayerView::getCurrentSelectView()
	{
		return m_selected_annotation_view;
	}

	MIView* MKOverlayLayerView::hitDetect( MITouch* touch, MIEvent *event, MITouchEventType& type )
	{
		//ignore all touch hit
		return NULL;
	}

	void MKOverlayLayerView::updateScale( MKTileMapView *view )
	{
		//MGLogD("Begin scaled");
		AUTO_LOCK_THIS;
		//MGLogD("MKAnnotationLayerView::update Scale");
		setRectTransition(MGRectTransitionMake(0, 0, 0));
        parent_view = dynamic_cast<MKMapView*>(parent());
        
		for(SubviewIterator it = m_subviews.begin(); it != m_subviews.end(); ++it)
		{
			MKPolylineView* polyline_view = dynamic_cast<MKPolylineView*>(*it);
			if(polyline_view != NULL)
            {
                vector<MGPoint> points;
				vector<MKLocationCoordinate2D> coords;
				MKPolylineView* poly_view = dynamic_cast<MKPolylineView*>(polyline_view);
				poly_view->polyline()->getCoordinates(coords);
				if (parent_view != NULL)
				{
					for (vector<MKLocationCoordinate2D>::iterator it=coords.begin();it!=coords.end();it++)
					{
						points.push_back(parent_view->convertCoordinateToPointToView((*it),parent_view->m_tile_map_view));
					}
				}
				
				poly_view->lock();
				poly_view->polyline()->setPoints(points);
				poly_view->unlock();
                
                continue;
            }
            
            
            MKPolygonView* polygon_view = dynamic_cast<MKPolygonView*>(*it);
            if(polygon_view != NULL)
            {
                vector<MGPoint> points;
                vector<MKLocationCoordinate2D> coords;
                polygon_view->polygon()->getCoordinates(coords);
                if (parent_view != NULL)
                {
                    for (vector<MKLocationCoordinate2D>::iterator it=coords.begin();it!=coords.end();it++)
                    {
                        points.push_back(parent_view->convertCoordinateToPointToView((*it),parent_view->m_tile_map_view));
                    }
                }
                
                polygon_view->lock();
                polygon_view->polygon()->setPoints(points);
                polygon_view->unlock();
                
                continue;
            }
            
            
            MKOverlayView* overlay_view = dynamic_cast<MKOverlayView*>(*it);
            if(overlay_view != NULL)
            {
                MKMapRect map_rect = overlay_view->overlay()->boundingMapRect();
				if (parent_view != NULL)
				{
					MGPoint left_bottom_point = parent_view->convertCoordinateToPointToView(MKLocationCoordinate2D(map_rect.origin.x, map_rect.origin.y), parent_view->m_tile_map_view);
                    MGPoint right_up_point = parent_view->convertCoordinateToPointToView(MKLocationCoordinate2D(map_rect.origin.x+map_rect.size.width, map_rect.origin.y+map_rect.size.height), parent_view->m_tile_map_view);
                    
                    MGRect overlay_frame = MGRectMake(left_bottom_point.x, right_up_point.y, right_up_point.x-left_bottom_point.x, left_bottom_point.y-right_up_point.y);
                    overlay_view->setFrame(overlay_frame);
				}
				
                continue;
            }
		}
		this->show();
	}

	void MKOverlayLayerView::updateTransition( MKTileMapView *view )
	{
		//AUTO_LOCK_THIS;
		//MGLogD(" MKAnnotationLayerView::updateTransition");

		//SubviewIterator it;
		//for (it=m_subviews.begin();it!=m_subviews.end();it++)
		//{
		//	MIView *sub_view=(*it);
		//	MGPoint center=sub_view->getCenter();
		//	center.x+=m_transition.axis_x;
		//	center.y+=m_transition.axis_y;
		//	sub_view->setCenter(center);
		//}
		//this->setRectTransition(MGRectTransitionMake(0,0,0));
	}

	MIView* MKOverlayScaledLayerView::hitDetect( MITouch* touch, MIEvent *event, MITouchEventType& type )
	{
		//ignore all touch hit
		return NULL;
	}

	void MKOverlayScaledLayerView::updateTransition( MKTileMapView *view )
	{
	//	return ;

		this->setSubRectTransition(view->getSubRectTransition());
	}

	void MKOverlayScaledLayerView::updateScale( MKTileMapView *view )
	{
		this->setSubRectScale(view->getSubRectScale());
	}

	void MKOverlayScaledLayerView::drawself()
	{
        
	}
}