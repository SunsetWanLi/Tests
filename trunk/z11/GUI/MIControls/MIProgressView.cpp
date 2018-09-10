///    修订说明：最初版本                 //

#include "MIProgressView.h"
#include "GraphicCore/MGImage.h"
#include "../LightGraphic/LightGraphic.h"

namespace z11
{
	MIProgressView::MIProgressView()
	{
        class_name = "MIProgressView";
		m_progress_view_style = MIProgressViewStyleDefault;
		m_progress_tint_color = MGColor::fromRGBA(100,157,255);
		
		m_progress = 0.0f;
		m_real_progress = 0.0f;
		animated = true;
		progress_go_back = false;
		animation_total_time = 0.8f;

		draw_image_mode = ImageCustomFit;
	}

	MIProgressView::~MIProgressView()
	{
		delete_and_set_null(m_progress_tint_color);
	}
	
	void MIProgressView::drawself()
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
		if(graphic_engine == NULL)
			return;
			
		// get the real progress
		if(animated && m_progress != m_real_progress)
		{
			//get step  ///
			float step = LightGraphic::getInterval()/animation_total_time;
			do 
			{
				float max_scale = 5.0f;
				float delta_progress = m_real_progress-m_progress;
				if(delta_progress < 0.0f)
					delta_progress = -delta_progress;

				float real_scale = (max_scale-1)*delta_progress+1;
				step *= real_scale;
			} 
			while(false);

			//get m_real_process //
			if(progress_go_back)
			{
				m_real_progress -= step;
				if(m_real_progress <= m_progress)
				{
					m_real_progress = m_progress;
				}
			}
			else
			{
				m_real_progress += step;
				if(m_real_progress >= m_progress)
				{
					m_real_progress = m_progress;
				}
			}
		}

		//draw the bkg
		RGBAColorRef bkg_color_ref;
		if(m_background_color != NULL)
		{
			bkg_color_ref = m_background_color->m_color;
			bkg_color_ref.m_alpha_component = (byte_8)(m_alpha*bkg_color_ref.m_alpha_component);
		}
		else
        {
			bkg_color_ref = RGBAColorMake(0, 0, 0, 0);
		}
		
		if(m_background_color != NULL && m_background_color->m_image != NULL)
		{
			m_background_color->m_image->sendDataToContext();
			MGRect image_rect;
			image_rect.width=m_background_color->m_image->getWidth();
			image_rect.height=m_background_color->m_image->getHeight();
			graphic_engine->drawImage(m_background_color->m_image, m_bounds, bkg_color_ref, draw_image_mode, image_rect);
		}
		else
		{
			graphic_engine->drawFilledRect(m_bounds, bkg_color_ref);
		}
		
		//draw the progress
		RGBAColorRef progress_color_ref;
		progress_color_ref = m_progress_tint_color->m_color;
		progress_color_ref.m_alpha_component = (byte_8)(m_alpha * progress_color_ref.m_alpha_component);

		if(m_progress_tint_color != NULL && m_progress_tint_color->m_image != NULL)
		{
			progress_color_ref = RGBAColorMake(255, 255, 255, 255);
			progress_color_ref.m_alpha_component = (byte_8)(m_alpha * progress_color_ref.m_alpha_component);

			m_progress_tint_color->m_image->sendDataToContext();
			MGRect image_rect;
			image_rect.width = m_real_progress*m_progress_tint_color->m_image->getWidth();
			image_rect.height = m_progress_tint_color->m_image->getHeight();
			MGRect draw_rect = m_bounds;
			draw_rect.width *= m_real_progress;
			graphic_engine->drawImage(m_progress_tint_color->m_image, draw_rect, progress_color_ref, draw_image_mode, image_rect);
		}
		else
		{
			progress_color_ref.m_alpha_component = (byte_8)(m_alpha*progress_color_ref.m_alpha_component);
			MGRect draw_rect = m_bounds;
			draw_rect.width *= m_real_progress;
			{
				MGRect rect = m_bounds;
				rect.width = draw_rect.width;
				MGGraphicEngine *engine=MGGraphicEngine::sharedGraphicEngine();
				engine->drawFilledLightedRect(rect, m_progress_tint_color->m_color,30,70);
				rect.x+=rect.width;
				rect.width=m_bounds.width-rect.width;
				engine->drawFilledLightedRect(rect,RGBAColorMake(255,255,255),30,70);
			}
			//graphic_engine->drawFilledRect(draw_rect, progress_color_ref);
		}
	}

	float MIProgressView::getProgress()
	{
		return m_progress;
	}

	void MIProgressView::setProgress(float _progress, bool _animated)
	{
		if(_progress < 0.0f)
			m_progress = 0.0f;
		else if(_progress > 1.0f)
			m_progress = 1.0f;
		else
			m_progress = _progress;
			
		animated = _animated;
		if(animated)
		{
			if(m_progress < m_real_progress)
				progress_go_back = true;
			else
				progress_go_back = false;
		}
		else
		{
			m_real_progress = m_progress;
		}
	}

	void MIProgressView::setAnimationTotalTime(float _time)
	{
		animation_total_time = _time;
	}

}