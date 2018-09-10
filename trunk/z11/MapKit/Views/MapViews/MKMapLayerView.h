/*
 * =====================================================================================
 *
 *       Filename:  MKMapLayerView.h
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
#pragma  once
#include "GUI/Base/MIView.h"
namespace z11
{
	class MKTileMapView;
	class MKMapView;
	class MKAnnotationView;
	class MKMapLayerView:public MIView
	{
    protected:
        MKMapLayerView();
	public:
		virtual ~MKMapLayerView()
		{

		}
	};
	class MKTileLayerView:public MKMapLayerView
	{
    public:
        MKTileLayerView();
	public:
		virtual ~MKTileLayerView()
		{

		}
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

	};
	class MKAnnotationLayerView:public MKMapLayerView
	{
	public:
		MKAnnotationLayerView();
		virtual ~MKAnnotationLayerView();
		virtual MIView* hitDetect(MITouch* touch, MIEvent *event, MITouchEventType& type);
		void updateViewScale(MKTileMapView *view);
		void updateViewTransition(MKTileMapView *view,const MGPoint &targetContentOffset);
		virtual void updateTransition(MKTileMapView *view);
		virtual void updateScale(MKTileMapView *view);
		MKMapView *parent_view;
		void setCurrentSelectView(MKAnnotationView *view);
		MKAnnotationView * getCurrentSelectView();
	private:
		MKAnnotationView *m_selected_annotation_view;
	};
	class MKOverlayLayerView:public MKAnnotationLayerView
	{
    public:
        MKOverlayLayerView();
	public:
		virtual MIView* hitDetect(MITouch* touch, MIEvent *event, MITouchEventType& type);
		virtual void updateScale(MKTileMapView *view);
		virtual void updateTransition( MKTileMapView *view );
	};
	class MKOverlayScaledLayerView:public MKTileLayerView
	{
    public:
        MKOverlayScaledLayerView();
	public:
		virtual MIView* hitDetect(MITouch* touch, MIEvent *event, MITouchEventType& type);
		virtual void updateTransition(MKTileMapView *view);
		virtual void updateScale(MKTileMapView *view);
		virtual void drawself();
	};
};
