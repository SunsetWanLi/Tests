/*
 * =====================================================================================
 *
 *       Filename:  MKAnnotationView.cpp
 *
 *    Description:  The MKAnnotationView class is responsible for 
 *    presenting annotations visually in a map view.
 *
 *        Version:  1.0
 *        Created:  04/12/2012 01:34:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MKAnnotationView.h"
#include "GUI/Controllers/MIViewController.h"
#include "GUI/MIControls/MIButton.h"
#include "System/Shared/MGLog.h"
#include "GraphicCore/MGImage.h"
#include "GraphicCore/MGFont.h"
#include "GraphicCore/GraphicStructure.h"
#include "../MapViews/MKMapLayerView.h"
#include "System/IO/MGFileManager.h"
#include "GraphicCore/MGCanvas.h"
#include "System/Device/MGDevice.h"
#include <stdio.h>
#include "../MapViews/MKMapView.h"
#include "../MapViews/MKTileMapView.h"
#include "GUI/Animations/MIAnimationsManager.h"
#include <sstream>
namespace z11
{
#define ANNOTATION_TOUCH_SIGNIFICANT_MOVE_VALVE 6.0f
#define CONTENT_OFFSET 20
    
    MGFont *shared_font()
    {
        static MGFont *font=MGFont::create(27,RGBAColorMake(255,255,255));
        return font;
    }
	class MKAnnotationCalloutView:public MIView
	{
	public:
		MGFont *font;
		MGRect m_right_callout_view_bounds;
		MGRect m_left_callout_view_bounds;
		MGRect m_title_bounds;
		string m_title;
		MGRect m_subtitle_bounds;
		string m_subtitle;
		MIView *m_rightCalloutAccessoryView;
		MIView *m_leftCalloutAccessoryView;
		MGImage *bubble_image;
		uint_32 font_size;
		uint_32 subfont_size;
		string pre_title;
		string pre_subtitle;
		MGRect m_init_frame;
		MKAnnotationCalloutView()
		{
			font_size=32;
			subfont_size=25;
			m_rightCalloutAccessoryView=NULL;
			m_leftCalloutAccessoryView=NULL;
			font=NULL;
			bubble_image=NULL;
			pre_title="";
			pre_subtitle="";
		}
		virtual void initWithFrame(MGRect frame)
		{
			m_init_frame = frame;
			MIView::initWithFrame(frame);
		}
		void adjustFrameByStringContent(const string &title,const string &subtitle)
		{
			m_title=title;
			m_subtitle=subtitle;

			MGRect frame_rect =m_init_frame;
			frame_rect.width=0;
			frame_rect.height=90;
			if (m_leftCalloutAccessoryView!=NULL)
			{
				m_left_callout_view_bounds.width = m_leftCalloutAccessoryView->getFrame().width;
				m_left_callout_view_bounds.height = m_leftCalloutAccessoryView->getFrame().height;
				m_left_callout_view_bounds.x = 20;
				m_left_callout_view_bounds.y = 20;
				frame_rect.width += ( m_left_callout_view_bounds.width+20 );
                frame_rect.height = frame_rect.height > (m_left_callout_view_bounds.height + 40) ? frame_rect.height : (m_left_callout_view_bounds.height + 40);
			}
            
            if (m_rightCalloutAccessoryView!=NULL)
			{
				m_right_callout_view_bounds.width = m_rightCalloutAccessoryView->getFrame().width;
				m_right_callout_view_bounds.height = m_rightCalloutAccessoryView->getFrame().height;
                m_right_callout_view_bounds.y = 20;
				frame_rect.width += ( m_right_callout_view_bounds.width+20 );
                frame_rect.height = frame_rect.height > (m_right_callout_view_bounds.height + 40) ? frame_rect.height : (m_right_callout_view_bounds.height + 40);
			}
			
            float height_diff = 0;
            if (frame_rect.height > 90) {
                height_diff = 10;
            } else {
                height_diff = frame_rect.height / 9;
            }
			float bounds_width=MGDevice::getScreenWidth()-frame_rect.width-40-height_diff;
			{
				if (font==NULL)
				{
                    MKAnnotationView *annotation_view=dynamic_cast<MKAnnotationView *>(parent());
					font = MGFont::create(27, annotation_view->m_call_out_font_color);
				}
				
				font->setFaceSize(font_size);
				MGRect string_bounds=font->getStringBoundForWidth(m_title,bounds_width);
				MGRect substring_bounds;
				{
					m_title_bounds.x=m_left_callout_view_bounds.x+m_left_callout_view_bounds.width+20;
					m_subtitle_bounds.x=m_title_bounds.x;
					m_title_bounds.width=string_bounds.width;
					m_title_bounds.height=32;
					if (subtitle!="")
					{
						m_title_bounds.y=10;
						font->setFaceSize(subfont_size);
						substring_bounds=font->getStringBoundForWidth(m_subtitle,bounds_width);

						m_subtitle_bounds.y=53;
						m_subtitle_bounds.width=substring_bounds.width;
						m_subtitle_bounds.height=25;
					}
					else
					{
						m_title_bounds.y=24;
					}
				}
				frame_rect.width += MAXVALUE(m_title_bounds.width,substring_bounds.width) + 40;
                if (frame_rect.height > 90) {
                    frame_rect.y -= frame_rect.height + 36;
                } else {
                    frame_rect.y-=frame_rect.height*1.4;
                }
				frame_rect.x-=frame_rect.width/2;
				frame_rect.x+=15;
				m_right_callout_view_bounds.x=frame_rect.width-m_right_callout_view_bounds.width-20;

				//ÒõÓ°²¿·Ö //
				frame_rect.width += height_diff;
				frame_rect.x+=parent()->getBounds().width/2;
				frame_rect.x-=height_diff;

			}
            
			setFrame(frame_rect);
		}
		void render()
		{
			MKAnnotationView *annotation_view=dynamic_cast<MKAnnotationView *>(parent());
			m_title=annotation_view->annotation()->title();
			m_subtitle=annotation_view->annotation()->subtitle();
			if(bubble_image==NULL||m_title != pre_title||m_subtitle != pre_subtitle)
			{		
				this->adjustFrameByStringContent(m_title,m_subtitle);
				pre_title = m_title;
				pre_subtitle = m_subtitle;
				if (m_rightCalloutAccessoryView!=NULL)
				{
					m_rightCalloutAccessoryView->setFrame(m_right_callout_view_bounds);
				}
				if (m_leftCalloutAccessoryView!=NULL)
				{
					m_leftCalloutAccessoryView->draw_image_mode = ImageAspectFit;
					m_leftCalloutAccessoryView->setFrame(m_left_callout_view_bounds);
				}


				char filename[1024];
				sprintf(filename,"controls/MKAnnotationView%dX%d.png",(int)m_bounds.width,(int)(m_bounds.height*1.5));
				string path=MGBundle::mainBundle()->tmpPathWithFileName(filename);
				if (!MGFileManager::defaultManager()->fileExistsAtPath(path))
				{
                    MGFileManager::defaultManager()->createDirectoryAtPath(path);
                    float height_value = 135;
                    if (m_bounds.height <= 90) {
                        height_value = m_bounds.height * 1.5;
                    } else {
                        height_value = m_bounds.height + 45;
                    }
					MGCanvas canvas(m_bounds.width,height_value, 32);
					canvas.drawBubble(MGRectMake(0,0,m_bounds.width,height_value),annotation_view->m_callout_alpha,annotation_view->m_call_out_color);
					canvas.writeToFile(path,true);
				}
				bubble_image=MGImage::imageWithContentsOfFile(path);

			}

		}
		virtual void drawself()
		{

			MGGraphicEngine *engine=MGGraphicEngine::sharedGraphicEngine();
			MKAnnotationView *annotation_view=dynamic_cast<MKAnnotationView *>(parent());
			if(annotation_view->canShowCallout){
				if(m_rightCalloutAccessoryView!=NULL)
					m_rightCalloutAccessoryView->show();
				if(m_leftCalloutAccessoryView!=NULL)
					m_leftCalloutAccessoryView->show();
			}
			if (!annotation_view->canShowCallout || !annotation_view->canDrawMapRect(MKMapRect(),0))
			{
				if(m_rightCalloutAccessoryView!=NULL)
					m_rightCalloutAccessoryView->hide();
				if(m_leftCalloutAccessoryView!=NULL)
					m_leftCalloutAccessoryView->hide();
				return;
			}
			render();
			if (bubble_image)
			{
				bubble_image->sendDataToContext();
			}
			if (bubble_image)
			{
				MGRect buble_rect=m_bounds;
                if (m_bounds.height > 87) {
                    buble_rect.height += 44;
                } else {
                    buble_rect.height*=1.5;
                }
				engine->drawImage(bubble_image,buble_rect);
			}

			{
				MGRect string_bounds=m_bounds;
				string_bounds.x+=m_title_bounds.x;
				string_bounds.y+=m_title_bounds.y;
				string_bounds.width=m_title_bounds.width;
				string_bounds.height=m_title_bounds.height;
				font->setFaceSize(font_size);
				if (m_subtitle!="")
				{
					engine->drawText(m_title,font,string_bounds,MILineBreakModeTailTruncation,MITextAlignmentLeftBottom);
				}
				else
				{
					//string_bounds.y-=m_bounds.height*0.3;
					string_bounds.height=m_bounds.height;
					engine->drawText(m_title,font,string_bounds,MILineBreakModeTailTruncation,MITextAlignmentLeftTop);
				}
			}
			if (m_subtitle!="")
			{
				MGRect string_bounds=m_bounds;
				string_bounds.x+=m_subtitle_bounds.x;
				string_bounds.y+=m_subtitle_bounds.y;
				string_bounds.width=m_subtitle_bounds.width;
				string_bounds.height=m_subtitle_bounds.height;
				font->setFaceSize(subfont_size);
				engine->drawText(m_subtitle,font,string_bounds,MILineBreakModeTailTruncation,MITextAlignmentLeftTop);
			}
			
			//engine->drawRect(m_bounds,RGBAColorRef::blue());

		}
		
		virtual ~MKAnnotationCalloutView()
		{
            delete_and_set_null(bubble_image);
			//delete_and_set_null(font);
		}
	};
	MKAnnotationView * MKAnnotationView::initWithAnnotation( MKAnnotation *_annotation,const std::string &_reuseIdentifier )
	{
		MKAnnotationView *ret = new MKAnnotationView(_annotation);
		ret->m_annotation=_annotation;
		ret->m_reuseIndentifier=_reuseIdentifier;
		return ret;
	}

	MKAnnotation * MKAnnotationView::annotation()
	{
		return m_annotation;
	}

	std::string MKAnnotationView::reuseIdentifier()
	{
		return m_reuseIndentifier;
	}

	void MKAnnotationView::prepareForReuse()
	{

	}

	MKAnnotationView::MKAnnotationView(MKAnnotation *_annotation)
	{
		m_annotation=_annotation;
		canShowCallout=true;
		m_selected=false;
		m_touched=false;
		m_image=NULL;
		callout=NULL;
		draw_image_mode=ImageCustomOriginalCenter;
		callout=new MKAnnotationCalloutView();
        _view_flags.user_multitouch_subview_enabled = 1;
        m_callout_alpha = 0.9412;
        m_call_out_color = RGBAColorRef::black();
        m_call_out_font_color = RGBAColorRef::white();
	}

	MKAnnotationView::~MKAnnotationView()
	{
	}

	void MKAnnotationView::drawself()
	{
		if (!canDrawMapRect(MKMapRect(),11))
		{
			return ;
		}
		
		
		//MGGraphicEngine::sharedGraphicEngine()->drawRect(m_bounds,RGBAColorRef::red());
		return MIView::drawself();
	}

	void MKAnnotationView::initWithFrame( MGRect frame )
	{
		MIView::initWithFrame(frame);
		callout->initWithFrame(frame);
		addSubview(callout);
		callout->hide();
	}

	MIView * MKAnnotationView::hitDetect( MITouch* touch, MIEvent *event, MITouchEventType& type )
	{
		AUTO_LOCK_THIS;
		if(canDrawMapRect(MKMapRect(),0))
		{

			MIView* hit_ret=MIView::hitDetect(touch,event,type);
			if (hit_ret!=NULL)
			{
				//MGLogD("MKAnnotationView::hitDetect");
			}
			else
			{
				if (selected())
				{
					hit_ret = callout->hitDetect(touch,event,type);
				}

			}
			
			return hit_ret;
		}
		else
			return NULL;
	}

	z11::MIEventHandleState MKAnnotationView::touchesEnded( const std::set<MITouch*>& touches, const MIEvent* const event )
	{
		MIEventHandleState ret = MIView::touchesEnded(touches, event);
		if (touches.size()==1)
		{
            MGPoint loc_in_view = (*touches.begin())->locationInView(this);
			if (pointInside(loc_in_view,NULL)&&m_touched)
			{
				//m_selected=!m_selected;
				MKAnnotationLayerView *parentView=dynamic_cast<MKAnnotationLayerView *>(parent());
				if (!m_selected)
				{
					//callout->show();
					m_selected=!m_selected;
					parentView->setCurrentSelectView(this);
				}
				//else
				//{
				//	//callout->hide();
				//	parentView->setCurrentSelectView(NULL);
				//}
				return MIEventHandleStateDone;
			}
		}
		m_touched=false;
		return ret;
	}

	z11::MIEventHandleState MKAnnotationView::touchesBegan( const std::set<MITouch*>& touches, const MIEvent* const event )
	{

		MIEventHandleState ret = MIView::touchesBegan(touches, event);
		m_touched=true;
		return MIEventHandleStateDone;
	}

	z11::MIEventHandleState MKAnnotationView::touchesMoved( const std::set<MITouch*>& touches, const MIEvent* const event )
	{
        if (touch_manager.size() != 1) {
            m_touched=false;
            return MIView::touchesMoved(touches,event);
        }
		if ((touch_manager.begin())->second.tag == (*touches.begin())->tag) {
            float dis = distanceOfTwoPoint((touch_manager.begin())->second.screen_point, (*touches.begin())->screen_point);
            if (dis <= ANNOTATION_TOUCH_SIGNIFICANT_MOVE_VALVE) {
                return MIEventHandleStateDone;
            }
        }
        return MIView::touchesMoved(touches,event);
	}

	bool MKAnnotationView::selected()
	{
		return m_selected;
	}

	void MKAnnotationView::setSelected( bool sel,bool isAnimation)
	{
		m_selected=sel;
		if (callout!=NULL)
		{
			if (m_selected)
			{
				if(true)
				{
					MKMapView *parent_view=dynamic_cast<MKMapView *>(this->parent()->parent());
					MGPoint targetContentOffset;
					callout->render();
					MGRect bounds=callout->getBounds();
					MGRectTransition parent_trans=parent()->getRectTransition();
					MGRectScale parent_scale=parent_view->m_tile_map_view->getSubRectScale();
					bool flag=false;
					float sceen_width=MGDevice::getScreenWidth();
					float offset=20;
					if (sceen_width-bounds.width<2*offset)
					{
						offset=(sceen_width-bounds.width)/2;
					}
					
					if (bounds.x+parent_trans.axis_x<0)
					{
						targetContentOffset.x=-bounds.x-parent_trans.axis_x+offset;
						targetContentOffset.x/=parent_scale.axis_x;
						flag=true;
					}
					else if (bounds.x+parent_trans.axis_x+bounds.width>sceen_width)
					{
						targetContentOffset.x=sceen_width-bounds.x-bounds.width-parent_trans.axis_x-offset;
						targetContentOffset.x/=parent_scale.axis_x;
						flag=true;
					}
//					if (flag)
//					{
//						MIView::animations_manager->lock();
//						//parent_view->m_tile_map_view->lock();
//						parent()->lock();
//						this->lock();
//						MGRectTransition trans=parent_view->m_tile_map_view->getSubRectTransition();
//						MGRectTransition old_trans=trans;
//						trans.axis_x+=targetContentOffset.x;
//						//parent_view->m_tile_map_view->lock();
//						parent_view->m_tile_map_view->setSubRectTransition(trans);
//						parent_view->m_tile_map_view->init(parent_view->m_tile_map_view->z);
//						if(true)
//						{
//							stringstream ss;
//							ss<<"MKAnnotationView::updateScale."<<(long)(this)<<MGTimeTicks::Now().ToInternalValue();
//							MIView::beginAnimation(ss.str());
//							MIView::setAnimationCurve(MIViewAnimationOptionCurveLinear);
//							MIView::setAnimationDuration(0.3);
//							parent_view->m_annotation_layer_view->updateScale(parent_view->m_tile_map_view);
//							parent_view->m_overlay_layer_view->updateScale(parent_view->m_tile_map_view);
//							MIView::commitAnimation();
//
//						}
//						else
//						{
//							parent_view->m_annotation_layer_view->updateScale(parent_view->m_tile_map_view);
//							parent_view->m_overlay_layer_view->updateScale(parent_view->m_tile_map_view);
//						}
//
//						parent_view->m_tile_map_view->setSubRectTransition(old_trans);
//						//parent_view->m_tile_map_view->unlock();
//						{
//							stringstream ss;
//							ss<<"MKAnnotationView::relocation."<<(long)(this)<<MGTimeTicks::Now().ToInternalValue();
//							MIView::beginAnimation(ss.str());
//							MIView::setAnimationCurve(MIViewAnimationOptionCurveLinear);
//							MIView::setAnimationDuration(0.3);
//							parent_view->m_tile_map_view->setSubRectTransition(trans);
//							MIView::commitAnimation();
//
//						}
//
//						this->unlock();
//						parent()->unlock();
//						//parent_view->m_tile_map_view->unlock();
//						MIView::animations_manager->unlock();
//					}
					
				}
				if(isAnimation){
					callout->setRectScale(MGRectScaleMake(0.9,0.9,1));
					stringstream ss;
					ss<<"MKAnnotationView::relocation."<<(long)(this)<<MGTimeTicks::Now().ToInternalValue();
					MIView::beginAnimation(ss.str());
					MIView::setAnimationCurve(MIViewAnimationOptionCurveDampingOnce);
					MIView::setAnimationDuration(0.3);
					callout->setRectScale(MGRectScaleMake(1,1,1));
					MIView::commitAnimation();
					callout->show();
				}
				else{
					callout->show();
				}
			}
			else
			{
				callout->hide();
			}
		}
		
		
	}

	void MKAnnotationView::drawBeforeSubviews()
	{
		m_alpha_old=m_alpha;
		m_alpha=1.0;
		MIView::drawBeforeSubviews();
	}

	void MKAnnotationView::drawAfterSubviews()
	{
		m_alpha=m_alpha_old;
		MIView::drawAfterSubviews();
	}

	MIView * MKAnnotationView::getParentView()
	{
		return parent()->parent();
	}

	z11::MGLongPoint MKAnnotationView::getMapPoint()
	{
		return map_point;
	}

	void MKAnnotationView::setMapPoint( const MGLongPoint &point )
	{
		map_point=point;
	}

	void MKAnnotationView::setRightCalloutAccessoryView( MIView* _view )
	{
		if (callout->m_rightCalloutAccessoryView!=NULL)
		{
			callout->m_rightCalloutAccessoryView->detachFromSuperview();
			delete_and_set_null(callout->m_rightCalloutAccessoryView);
		}
		callout->m_rightCalloutAccessoryView = _view;
		if (callout->m_rightCalloutAccessoryView!=NULL)
		{
			callout->addSubview(_view);
		}
	}

	MIView * MKAnnotationView::getRightCalloutAccessoryView()
	{
		return callout->m_rightCalloutAccessoryView;
	}

	void MKAnnotationView::setLeftCalloutAccessoryView( MIView* _view )
	{
		if (callout->m_leftCalloutAccessoryView!=NULL)
		{
			callout->m_leftCalloutAccessoryView->detachFromSuperview();
			delete_and_set_null(callout->m_leftCalloutAccessoryView);
		}
		callout->m_leftCalloutAccessoryView = _view;
		if (callout->m_leftCalloutAccessoryView!=NULL)
		{
			callout->addSubview(_view);
		}
	}

	MIView * MKAnnotationView::getLeftCalloutAccessoryView()
	{
		return callout->m_leftCalloutAccessoryView;
	}

	bool MKAnnotationView::canDrawMapRect( const MKMapRect &mapRect,MKZoomScale zoomScale )
	{
		if (m_bounds.y+m_bounds.height<0
			||m_bounds.x+m_bounds.width<0
			||m_bounds.x>MGDevice::getScreenWidth()
			||m_bounds.y>MGDevice::getScreenHeight())
		{
			return false;
		}
		if ((m_bounds.y<0||m_bounds.x<0)&&
			(m_rotate.axis_x!=0.0f||m_rotate.axis_y!=0.0f||m_rotate.axis_z!=0.0f))
		{
			return false;
		}
		return true;
	}


	void MKAnnotationViewAccessoryDetailDisclosureButton::initWithFrame( MGRect frame )
	{
		render();
		MGImage *image=MGImage::imageWithContentsOfFile(MGBundle::mainBundle()->tmpPathWithFileName("controls/disclosure_button.png"));
		assert(image!=NULL);

		setBackgroundColor(MGColor::fromImage(image));
		frame.width=image->getWidth();
		frame.height=image->getHeight();
		MIView::initWithFrame(frame);

	}

	MKAnnotationViewAccessoryDetailDisclosureButton::MKAnnotationViewAccessoryDetailDisclosureButton()
	{

	}

	void MKAnnotationViewAccessoryDetailDisclosureButton::render()
	{
		string path=MGBundle::mainBundle()->tmpPathWithFileName("controls/disclosure_button.png");
		if (!MGFileManager::defaultManager()->fileExistsAtPath(path))
		{
            MGFileManager::defaultManager()->createDirectoryAtPath(path);
			MGCanvas canvas(48,48,32);
			canvas.drawDisclosureButton(MGRectMake(0,0,48,48));
			canvas.writeToFile(path,true);
		}
	}
}
