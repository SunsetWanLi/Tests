/*
 * =====================================================================================
 *
 *       Filename:  MKTileMapView.h
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  05/11/2012 02:57:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma  once
#include "MKMapScrollView.h"
#include "MKTileLayerOverlay.h"
namespace z11
{
#define THREAD_NUM 2
	class MKMapView;
	class MKFetcherThread;
	class MKMapLayerView;
	class MKAnnotationLayerView;
	class MKOverlayLayerView;
	class MKOverlayScaledLayerView;
	class MKTileMapView:public MKMapScrollView,public MKMapScrollViewDelegate
	{
	public:
		MKTileMapView();
		virtual ~MKTileMapView();
		virtual void initWithFrame(MGRect frame);
		virtual void  scrollViewWillEndDraggingWithVelocityTargetContentOffset(MKMapScrollView *scrollView,const MGPoint& velocity,MGPoint &targetContentOffset);
		virtual void scrollViewDidEndDraggingWillDecelerate(MKMapScrollView *scrollView,bool decelerate);
		virtual void scrollViewWillBeginZoomingWithView(MKMapScrollView *scrollView,MIView *view);
		virtual void scrollViewDidEndZoomingWithViewAtScale(MKMapScrollView *scrollView,MIView *view,float scale);
		void init(int z);
		MKMapRect getViewPortRect();
		MKFetcherThread *fetcher_thread[THREAD_NUM];
		MIView * hitDetect( MITouch* touch, MIEvent *event, MITouchEventType& type );
		int z;
		MKMapLayerView *tile_layer_view;
		//MKMapLayerView *annotation_layer_view;
		MKOverlayScaledLayerView *overlay_layer_view;
		void setCenterCoordinate(const MKLocationCoordinate2D &center_coordinate);
		z11::MKLocationCoordinate2D convertPointToCoordinateFromView( const MGPoint &point);
	public:
        bool isWebData;
		MKMapView *getParentMapView();
		void clear();
		MGPROPERTY(MKMapType,m_map_type,MapType) ;
		void startThread();
	private:
		MKCoordinateRegion m_region;
		MGIntPoint lefttop(int z);
		MGIntPoint lefttop(int z,MGRectTransition _trans);
		MGRectTransition sub_trans;

	private:
		uint_32 tile_start_x;
		uint_32 tile_start_y;

		float start_z;
        
	protected:
		virtual MIEventHandleState touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event);
		virtual void scrollViewTaped(MKMapScrollView* scrollView, MITapGestureRecognizer* recognizer);
	
    private:
		//Loading the Map Data
		void tileMapViewWillStartLoadingMap(MGObject*mapView);

		void tileMapViewDidFinishLoadingMap(MGObject*mapView);
        
        friend class MKMapView;
	};
};