#include "MISlide.h"
#include "MIButton.h"
#include "../../GraphicCore/MGImage.h"
#include "../../System/Shared/MGLog.h"
namespace z11
{
	class MISlideThumb : public MIButton
	{
	public:
		virtual void drawself()
		{
			MIView::drawself();
		}
        
		virtual MIEventHandleState touchesMoved(const std::set<MITouch*>& touches,const MIEvent* const event)
		{
			MIEventHandleState state = MIButton::touchesMoved(touches,event);
			MITouch* touch = *touches.begin();
			MGPoint pt = touch->point;
			MGRect parent_bounds = parent()->getBounds();
            
            float start_x = parent_bounds.x+m_frame.width/2.0f;
            float end_x = parent_bounds.x+parent_bounds.width-m_frame.width/2.0f;
			//if(pt.x>parent_bounds.x&&pt.x<parent_bounds.x+parent_bounds.width&&pt.y>parent_bounds.y&&pt.y<parent_bounds.y+parent_bounds.height)
			{
				pt.y = m_bounds.y+m_bounds.width/2;
				if (pt.x< start_x)
				{
					pt.x = start_x;
				}
				else if(pt.x > end_x)
				{
					pt.x = end_x;
				}
				this->setCenter(pt);
			}
			//if(pt.y > parent_bounds.y && pt.y < parent_bounds.y+parent_bounds.height)
			{
				MISlide* slide = dynamic_cast<MISlide*>(parent());
				slide->valueChange((pt.x-start_x)/(parent_bounds.width-m_frame.width));
			}
			return state;
		}
	};
    
	MISlide::MISlide(void)
	{
        class_name = "MISlide";
		m_mininum_value = 0.0f;
		m_maxinum_value = 1.0f;
        
        m_mininum_value_image = m_maxinum_value_image = NULL;
	}

	MISlide::~MISlide(void)
	{
        delete_and_set_null(m_mininum_value_image);
        delete_and_set_null(m_maxinum_value_image);
	}
    
	void MISlide::initWithFrame(z11::MGRect frame)
	{
		MIView::initWithFrame(frame);
		thumb_view = new MISlideThumb();
		frame.x = frame.y = 0.0f;
		frame.width = frame.height;
		thumb_view->initWithFrame(frame);
		thumb_view->setBackgroundColor(MGColor::fromImage(MGImage::imageWithContentsOfFile(MGBundle::mainBundle()->drawablePathWithFileName("controls/slide_thumb.png"))));
		addSubview(thumb_view);
	}

	void MISlide::drawself()
	{
		MIView::drawself();
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        RGBAColorRef color_ref = RGBAColorRef(255, 255, 255, 255);
        
		MGRect draw_rect = m_bounds;
        // init draw_start_x_in_frame
        float draw_start_x_in_frame = 0.2f*thumb_view->getBounds().width;
        draw_rect.x += draw_start_x_in_frame;
		
		MGPoint center = thumb_view->getCenter();
		draw_rect.width = center.x-draw_rect.x;
        if(m_mininum_value_image != NULL)
        {
            m_mininum_value_image->sendDataToContext();
            MGRect image_rect;
            image_rect.width = m_mininum_value_image->getWidth()*draw_rect.width/m_frame.width;
            image_rect.height = m_mininum_value_image->getHeight();
            
            draw_rect.height = m_frame.width*m_mininum_value_image->getHeight()/m_mininum_value_image->getWidth();
            if(draw_rect.height > m_frame.height)
            {
                draw_rect.height = m_frame.height;
            }
            draw_rect.y = (m_frame.height-draw_rect.height)/2.0f+m_bounds.y;
            graphic_engine->drawImage(m_mininum_value_image, draw_rect, color_ref, ImageCustomFit, image_rect);
        }
        else
        {
            draw_rect.height = m_frame.height/2.0f;
            draw_rect.y = (m_frame.height-draw_rect.height)/2.0f+m_bounds.y;
            graphic_engine->drawFilledLightedRect(draw_rect, RGBAColorMake(100, 157, 255), 30, 70);
        }
		
		draw_rect.x += draw_rect.width;
		draw_rect.width = m_bounds.width-draw_rect.width-2.0f*draw_start_x_in_frame;
        if(m_maxinum_value_image != NULL)
        {
            m_maxinum_value_image->sendDataToContext();
            MGRect image_rect;
            image_rect.width = m_maxinum_value_image->getWidth()*draw_rect.width/m_frame.width;
            image_rect.x = m_maxinum_value_image->getWidth()-image_rect.width;
            image_rect.height = m_maxinum_value_image->getHeight();
            
            draw_rect.height = m_frame.width*m_maxinum_value_image->getHeight()/m_maxinum_value_image->getWidth();
            if(draw_rect.height > m_frame.height)
            {
                draw_rect.height = m_frame.height;
            }
            draw_rect.y = (m_frame.height-draw_rect.height)/2.0f+m_bounds.y;
            graphic_engine->drawImage(m_maxinum_value_image, draw_rect, color_ref, ImageCustomFit, image_rect);
        }
        else
        {
            draw_rect.height = m_frame.height/2.0f;
            draw_rect.y = (m_frame.height-draw_rect.height)/2.0f+m_bounds.y;
            graphic_engine->drawFilledLightedRect(draw_rect, RGBAColorMake(255, 255, 255), 30, 70);
        }
	}
	MGImage * MISlide::currentMinimumTrackImage()
	{
		return NULL;
	}

	MGImage * MISlide::minimumTrackImageForState( MIControlState state )
	{
		return NULL;
	}

	void MISlide::setMinimumTrackImageForState( MGImage *image,MIControlState state )
	{

	}

	MGImage * MISlide::maximumTrackImageForState( MIControlState state )
	{
		return NULL;
	}

	void MISlide::setMaximumTrackImageForState( MGImage *image,MIControlState state )
	{

	}

	MGImage * MISlide::currentMaximumTrackImage()
	{
		return NULL;
	}

	MGImage * MISlide::currentThumbImage()
	{
		return NULL;
	}

	MGImage * MISlide::thumbImageForState( MIControlState state )
	{
		return NULL;
	}

	void MISlide::setThumbImageForState( MGImage *image,MIControlState state )
	{

	}

	z11::MGRect MISlide::maximumValueImageRectForBounds( const MGRect &_bounds )
	{
		static MGRect rect;
		return rect;
	}

	z11::MGRect MISlide::minimumValueImageRectForBounds( const MGRect &_bounds )
	{
		static MGRect rect;
		return rect;

	}

	z11::MGRect MISlide::trackRectForBounds( const MGRect &_bounds )
	{

		static MGRect rect;
		return rect;
	}

	z11::MGRect MISlide::thumbRectForBounds( const MGRect &_bounds,const MGRect &_rect ,float _value )
	{

		static MGRect rect;
		return rect;
	}

	void MISlide::setValue(float _value)
	{
		m_value=_value;
		MGPoint pt = thumb_view->getCenter();
		pt.x = (m_bounds.x+thumb_view->getFrame().width/2.0f)+(m_value/m_maxinum_value)*(m_bounds.width-thumb_view->getBounds().height);
		thumb_view->setCenter(pt);
	}

	float MISlide::value()
	{
		return m_value;
	}

	void MISlide::valueChange( float _value )
	{
		//MGLogD("MISlide::valueChange %f",_value);
		if (_value<m_mininum_value-(1e-3))
		{
			m_value=m_mininum_value;
		}
		else if(_value>m_maxinum_value+(1e-3))
		{
			m_value=m_maxinum_value;
		}
		else
		{
			m_value=_value;
		}
		EventsMapIterator it = events_dispatch_map.find(MIControlEventValueChanged);
		if (it != events_dispatch_map.end()) {
			assert(it->second.view_controller != NULL);
			assert(it->second.event_func != NULL);
			EventFunc func = it->second.event_func;
			((it->second.view_controller)->*func)(this);
		}
	}



}