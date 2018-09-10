/*
 * =====================================================================================
 *
 *       Filename:  MKAnnotationView.h
 *
 *    Description:  The MKAnnotationView class is responsible for 
 *    presenting annotations visually in a map view.
 *
 *        Version:  1.0
 *        Created:  04/12/2012 01:35:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma  once
#include "MapKit/Protocols/MKAnnotation.h"
#include "GUI/Base/MIView.h"
#include "GUI/MIControls/MIControl.h"
#include "GUI/MIControls/MIButton.h"
namespace z11
{
	enum MKAnnotationViewDragState
	{
		MKAnnotationViewDragStateNone = 0,
	    MKAnnotationViewDragStateStarting,
		MKAnnotationViewDragStateDragging,
		MKAnnotationViewDragStateCanceling,
		MKAnnotationViewDragStateEnding
	};
	class MKAnnotationCalloutView;
	class MKAnnotationView:public MIView
	{
		public:
			//Initializing and Preparing the View
			static MKAnnotationView *initWithAnnotation(MKAnnotation *annotation,const std::string &_reuseIdentifier="");

			virtual void prepareForReuse();
			virtual void drawself();
			virtual void initWithFrame(MGRect frame);

			//Getting and Setting Attributes
			MGPROPERTY(bool,enabled,Enabled);
			MGPROPERTY(MGImage *,m_image,Image);
			MGPROPERTY(bool,m_highlighted,Highlighted);
			MKAnnotation *annotation();
			MGPROPERTY(MGPoint,m_centerOffset,CenterOffset);
			MGPROPERTY(MGPoint,m_calloutOffset,CalloutOffset);
            MGPROPERTY(float,m_callout_alpha,CalloutAlpha);
            MGPROPERTY(RGBAColorRef,m_call_out_color,CalloutColor);
            MGPROPERTY(RGBAColorRef,m_call_out_font_color,CalloutFontColor);
			std::string reuseIdentifier();

			//Managing the Selection State
			bool selected();
			void setSelected(bool sel,bool isAnimation=true);

			//Managing Callout Views
			bool canShowCallout;
			void setRightCalloutAccessoryView(MIView* _view);
			MIView *getRightCalloutAccessoryView();
			void setLeftCalloutAccessoryView(MIView* _view);
			MIView *getLeftCalloutAccessoryView();

			//Supporting Drag Operations
			MGPROPERTY(bool,m_draggable,Draggable);
			MGPROPERTY(MKAnnotationViewDragState,m_dragState,DragState);

			virtual MIView * hitDetect(MITouch* touch, MIEvent *event, MITouchEventType& type);
			virtual void drawBeforeSubviews();
			virtual void drawAfterSubviews();
			MGLongPoint getMapPoint();
			void setMapPoint(const MGLongPoint &point);
			virtual bool canDrawMapRect(const MKMapRect &mapRect,MKZoomScale zoomScale);
	public:
		virtual ~MKAnnotationView();
	protected:
		MGLongPoint map_point;
		bool m_selected;
		bool m_touched;
        
		virtual MIEventHandleState touchesBegan(const std::set<MITouch*>& touches,const MIEvent* const event);
		virtual MIEventHandleState touchesMoved(const std::set<MITouch*>& touches,const MIEvent* const event);
		virtual MIEventHandleState touchesEnded(const std::set<MITouch*>& touches,const MIEvent* const event);
		MKAnnotationView(MKAnnotation *_annotation);
		std::string m_reuseIndentifier;
		MKAnnotation *m_annotation;
		MIView *getParentView();
		friend class MKAnnotationCalloutView;
		MKAnnotationCalloutView *callout;
		float m_alpha_old;

	};
	class MKAnnotationViewAccessoryDetailDisclosureButton:public MIButton
	{
	public:
		MKAnnotationViewAccessoryDetailDisclosureButton();
		virtual void initWithFrame(MGRect frame);
	private:
		void render();
	};
}
