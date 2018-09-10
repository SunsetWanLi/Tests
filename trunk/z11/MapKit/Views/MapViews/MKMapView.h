/*
 * =====================================================================================
 *
 *       Filename:  MKMapView.h
 *
 *    Description:  An MKMapView object provides an embeddable map interface, similar to the one provided by the Maps application
 *
 *        Version:  1.0
 *        Created:  04/11/2012 02:57:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma  once
#include "CommonBase/MGError.h"
#include "GUI/Base/MIView.h"
#include "MapKit/DataTypes/Base/MKGeometry.h"
#include "MapKit/Views/AnnotationViews/MKAnnotationView.h"
#include "MapKit/Views/OverlayViews/MKOverlayView.h"
#include <vector>
#include "MapKit/DataTypes/Base/MKTypes.h"
#include "System/LocationService/MGLocationManager.h"
namespace z11
{
	enum {
		MKUserTrackingModeNone = 0,
		MKUserTrackingModeFollow,
		MKUserTrackingModeFollowWithHeading, //now same as MKUserTrackingModeFollow
		MKUserTrackingModeNum
	};
	typedef z11::uint_32 MKUserTrackingMode;
    
	class MIButton;
	class MIPanGestureRecognizer;
	class MIPinchGestureRecognizer;
	class MKMapViewDelegate;
	class MKUserLocation;
	class MKMapScrollView;
	class MKTileMapView;
	class MKAnnotationLayerView;
	class MKOverlayScaledLayerView;
	class MKOverlayLayerView;
    
	class MKMapView : public MIView, public MGLocationManagerDelegate
	{
	public:
		MKMapView();
		virtual ~MKMapView();
        virtual void initWithFrame(MGRect frame);
		void setMapType(MKMapType map_type, bool isWebData = false);
		MKMapType getMapType();
		MGPROPERTY(bool, m_enable_zoom, ZoomEnabled);
		MGPROPERTY(bool, m_enable_scroll, ScrollEnabled);

		//Accessing the Delegate
		MKMapViewDelegate *delegate;
        
	public:
        // properties
        int getZoom();
        void setZoom(int level, bool animated = true, float animation_duration = 0.2f);
        
		void setMapBounds(const MKCoordinateRegion &map_bounds);
        const MKCoordinateRegion & getMapBounds() const { return m_map_bounds; }
        
		void setCenterCoordinate(const MKLocationCoordinate2D &center_coordinate);
		const MKLocationCoordinate2D & centerCoordinate();
        
		void setVisibleMapRect(const MKMapRect & visual_map_rect); // todo...
		const MKMapRect & visibleMapRect(); // todo...

		void setRotate(MGRectRotate &rotate);
        
        // location
        void useSimulateGPSData(const std::string &simulate_gps_data);
        
        void setShowsUserLocation(bool shows_user_location);
		bool showsUserLocation();
		bool isUserLocationVisible();
		MKUserLocation* uerLocation();
        
		void setUserTrackingMode(MKUserTrackingMode mode,bool animated=true);
		MKUserTrackingMode userTrackingMode();
		
        virtual void updatingLocation(const MGLocation& to_location, const MGLocation& from_location);
		virtual void updatingHeading(const MGHeading& new_heading);
        
        // utilities
        MGLongPoint convertCoordinateToPoint(const MKLocationCoordinate2D &coord);
		MGPoint convertCoordinateToPointToView(const MKLocationCoordinate2D &coord,MIView *view=NULL);
		MKLocationCoordinate2D convertPointToCoordinateFromView(const MGPoint &point,MIView *view=NULL);

        // annotations
		std::vector<MKAnnotation *> annotations();
        MKAnnotationView* viewForAnnotation(const MKAnnotation* const annotation) const;
        
		void addAnnotation(MKAnnotation *annotation);
		void addAnnotations(const std::vector<MKAnnotation *> &annotations);
		void removeAnnotation(MKAnnotation *annotation,bool freeAnnotation=true);
		void removeAnnotations(const std::vector<MKAnnotation *> &annotations,bool freeAnnotation = true);
        void clearAnnotationLayer();
        
        std::set<MKAnnotation *> annotationsInMapRect(const MKMapRect &rect);
		std::set<MKAnnotation *> annotationsWithType(const std::string &type_name,const MKCoordinateRegion &region);
		MGPROPERTY_READONLY(MGRect, annotationVisibleRect, annotationVisibleRect);
		MKAnnotationView *dequeueReusableAnnotationViewWithIdentifier(const std::string &identifier);
		MGPROPERTY(std::vector<MKAnnotation*>, selectedAnnotations, selectedAnnotations);
        
        // overlays
        MKOverlayView* viewForOverlay(const MKOverlay* const overlay) const;
		void addOverlay(MKOverlay *overlay);
		void removeOverlay(MKOverlay *overlay);
        void clearOverlayLayer();
		
    protected:
		MKUserTrackingMode m_tracking_mode;
		std::string m_simulate_gps_data;
		bool  m_showsUserLocation;
		MKUserLocation *m_userLocation;
		MGLocationManager *m_location_manager;
		MKAnnotationView *m_userLocationView;
		std::vector<MKAnnotation *> m_annotations;
		std::set<MKAnnotation *> m_poi_annotations;
	
	protected:
		void initAnnotationView(MKAnnotationView *annotation_view);
		void scrollViewDidEndZoomingWithViewAtScale(MKMapScrollView *scrollView,MIView *view,float scale);
		void scrollViewWillEndDraggingWithVelocityTargetContentOffset(MKMapScrollView *scrollView,const MGPoint& velocity,MGPoint &targetContentOffset);
		void removeAnnotationView(MKAnnotation *annotation);
        virtual MIEventHandleState CustomerImp touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event);
        
    protected:
		std::vector<MKAnnotationView *> m_annotation_views;
		std::vector<MKOverlayView *> m_overlay_views;
		
		std::vector<MKOverlay *> m_overlays;
		MKMapType m_mapType;	
		//MIView* m_views[MKMapTypeCount];
		MIButton *m_buttons[MKMapTypeCount];
		MKTileMapView *m_tile_map_view;
		MKAnnotationLayerView *m_annotation_layer_view;
		MKOverlayLayerView *m_overlay_layer_view;
        
    private:
        // Bounds of the Map
		MKCoordinateRegion m_map_bounds;
        // Manipulating the Visible Portion of the Map
		MKLocationCoordinate2D m_center_coord;
		bool initPOIData;
        
        friend class MKTileMapView;
		friend class MKAnnotationView;
		friend class MKPOIAnnotationView;
		friend class MKAnnotationLayerView;
		friend class MKOverlayLayerView;
	};

	class MKMapViewDelegate
	{
	public:
		virtual void mapViewSingleTapped(MKMapView *mapView,MIGestureRecognizer *recognizer)
		{
		}
		//Responding to Map Position Changes
		virtual void mapViewRegionWillChange(MKMapView *mapView)
		{
		}

		virtual void mapViewRegionDidChange(MKMapView *mapView)
		{
		}

		//Loading the Map Data
		virtual void mapViewWillStartLoadingMap(MKMapView *mapView)
		{

		}

		virtual void mapViewDidFinishLoadingMap(MKMapView *mapView)
		{

		}

		virtual void mapViewDidFailLoadingMapWithError(MKMapView *mapView,Ref<MGError> error)
		{

		}

		//Tracking the User Location
		virtual void mapViewWillStartLocatingUser(MKMapView *mapView)
		{

		}

		virtual void mapViewDidStopLocatingUser(MKMapView *mapView)
		{

		}

		virtual void mapViewDidUpdateUserLocation(MKMapView *mapView)
		{

		}

		virtual void mapViewDidFailToLocateUserWithError(MKMapView *mapView)
		{

		}

		virtual void mapViewDidChangeUserTrackingMode(MKMapView *mapView,MKUserTrackingMode mode)
		{

		}

		//Managing Annotation Views
		virtual MKAnnotationView *mapViewForAnnotation(MKAnnotation *annotation)
		{
			return NULL;
		}

		virtual void mapViewDidAddAnnotationViews(MKMapView *mapView,const std::vector<MKAnnotationView *> &views)
		{

		}

		//(void)mapView:(MKMapView *)mapView annotationView:(MKAnnotationView *)view calloutAccessoryControlTapped:(UIControl *)control

		//Dragging an Annotation View
		virtual void mapViewAnnotationViewDidChangeDragStateFromOldState(
			MKMapView *mapView,MKAnnotationView *annotationView,
			MKAnnotationViewDragState newState,
			MKAnnotationViewDragState oldState)
		{}

		//Selecting Annotation Views
		virtual void mapViewDidSelectAnnotationView(MKMapView *mapView,MKAnnotationView *view)
		{}

		virtual void mapViewDidDeselectAnnotationView(MKMapView *mapView,MKAnnotationView *view)
		{}

		//Managing Overlay Views
		virtual MKOverlayView *mapViewForOverlay(MKOverlay *overlay)
		{
			return NULL;
		}

		virtual void mapViewDidAddOverlayViews(MKMapView *mapView,const std::vector<MKOverlayView*> overlayViews)
		{}

		//@unstable 
		virtual void mapViewLongPressed(MKMapView *mapView,MIGestureRecognizer *recognizer)
		{}
        
		virtual void scrollViewWillEndDraggingWithVelocityTargetContentOffset(MKMapScrollView *scrollView, const MGPoint& velocity, MGPoint &targetContentOffset )
		{}
	};
};
