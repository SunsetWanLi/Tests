/*
 * =====================================================================================
 *
 *       Filename:  MKTileLayerOverlay.h
 *
 *    Description:  Map View Cell View
 *
 *        Version:  1.0
 *        Created:  04/12/2012 11:39:42 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma once
#include <set>
#include <string>
#include <list>
#include "MapKit/DataTypes/Shapes/MKShape.h"
#include "MapKit/Protocols/MKAnnotation.h"
#include "GUI/Base/MIView.h"
#include "../OverlayViews/MKOverlayView.h"
#include "System/Thread/MGMutex.h"
#include "System/Shared/MGTime.h"
#include "MapKit/DataTypes/Base/MKTypes.h"
#include "CommonBase/Memory/MGRefObject.h"
#define MKMapViewCellHeight MAP_CELL_SIZE
#define MKMapViewCellWidth MAP_CELL_SIZE
namespace z11
{
	class MGCanvas;
	class MGImage ;
	class MKTileGrid:public MKShape
	{
	public:
		virtual MKLocationCoordinate2D coordinate();
		static MKTileGrid *tileGridWithMapRect(const MKMapRect &rect);
		virtual MKMapRect boundingMapRect();
		void setRect(const MKMapRect &rect);
	private:
		MKMapRect m_rect;

	};
	enum MKFetchState
	{
		MKFetchStateProcessing,
		MKFetchStateSuccess,
		MKFetchStateFailed,
	};
	class MKTileLayerOverlay : public MKOverlayView
	{
	public:
		MGPROPERTY(MKMapType,m_map_type,MapType) ;
		void setFetchState(MKFetchState state);
		MKFetchState fetchState();
		virtual void render()=0;
		virtual ~MKTileLayerOverlay();
	protected:
		MKFetchState m_fetch_state;
		MKTileLayerOverlay();
	};

	class MKTileMapView;
	class MKSateliteMapViewCell : public MKTileLayerOverlay
	{
	public:
		virtual ~MKSateliteMapViewCell();
		MKSateliteMapViewCell();
		virtual void render();
		virtual void drawself();
		MKGlobalTileIndex m_tms_code;
		int_64 reuseable_life;
		virtual MIEventHandleState touchesBegan(const std::set<MITouch*>& touches, const MIEvent* const event)
		{
			return MIEventHandleStateDone;
		}
		virtual MIEventHandleState touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event)
		{
			return MIEventHandleStatePass;
		}
		virtual MIEventHandleState touchesEnded(const std::set<MITouch*>& touches, const MIEvent* const event)
		{
			return MIEventHandleStateDone;
		}
		virtual bool canDrawMapRect(const MKMapRect &mapRect,MKZoomScale zoomScale);
        
        bool isWebData;
	private:
		virtual MKTileMapView *parentMapView();
        MGCanvas *image_canvas;
	};
}