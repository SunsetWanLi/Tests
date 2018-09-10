
//  MILabel.cpp
//  Gui

#include "MILabel.h"
#include "../../GraphicCore/MGFont.h"
#include "../../GraphicCore/MGGraphicEngine.h"
#include "../../GraphicCore/MGImage.h"
#include "../../System/Device/MGDevice.h"
#include "../../System/Shared/MGLog.h"
#include "../../System/Shared/Utility.h"

#include <assert.h>

#include <iostream>
using namespace std;
#ifdef FONT_SHARED
#undef FONT_SHARED
#define FONT_SHARED 1
#endif
namespace z11
{
	MILabel::MILabel()
	{
        class_name = "MILabel";
		//m_text_content = "GIS SDK Test Program";
		m_text_font=MGFont::create(12,RGBAColorRef::black());
		m_text_alignment=MITextAlignmentLeftMiddle;
		m_lineBreakMode=MILineBreakModeTailTruncation;
		m_background_color=MGColor::fromRGBA(0,0,0,0);
        _view_flags.user_interaction_enabled = 0;
	}

	MILabel::~MILabel()
	{
		//MGLogD("~MILabel()");
		delete_and_set_null(m_text_font);
	}

	void MILabel::drawself()
	{
		//---edit by Hu Danyuan  2011-7-13
		MGGraphicEngine *ge=MGGraphicEngine::sharedGraphicEngine();
		//MIView::drawself();
		MGRect rect=m_bounds;
//		MGRect rect = MGRectMake(m_bounds.x + m_text_border.x, m_bounds.y + m_text_border.y, m_bounds.width - m_text_border.x, m_bounds.height - m_text_border.y);
        
        // draw background
        RGBAColorRef color_ref = RGBAColorMake(0, 0, 0, (byte_8)(m_alpha * 255));;
        if (m_background_color != NULL)
        {
            color_ref = m_background_color->m_color;
            color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
        }
        
        if(m_background_color != NULL && m_background_color->m_image != NULL)
        {
            
            m_background_color->m_image->sendDataToContext();
            ge->drawImage(m_background_color->m_image, m_bounds, color_ref, draw_image_mode);
        }
        else if(m_background_color != NULL)
            ge->drawFilledRect(m_bounds, color_ref);

        // draw text
		if (m_text_font == NULL)
		{

#if FONT_SHARED
			MGFont *m_shared_font=MGFont::shareFont();
			m_shared_font->setFaceSize(17);
			m_shared_font->setColor(RGBAColorRef::black());
			ge->drawText(m_text_content,m_shared_font,rect, m_lineBreakMode,m_text_alignment,m_shared_font->getColor(),RGBAColorMake(255,255,255,m_alpha*255));
#else
			m_text_font=MGFont::create(17,RGBAColorRef::black());
			ge->drawText(m_text_content,m_text_font,rect, m_lineBreakMode,m_text_alignment,m_text_font->getColor(),RGBAColorMake(255,255,255,m_alpha*255));
#endif
		}
		else
		{
#if FONT_SHARED 
		//save memory
		MGFont *m_shared_font=MGFont::shareFont();
		m_shared_font->setFaceSize(m_text_font->getFontSize());
		m_shared_font->setColor(m_text_font->getColor());
		ge->drawText(m_text_content,m_shared_font,rect, m_lineBreakMode,m_text_alignment,m_shared_font->getColor(),RGBAColorMake(255,255,255,m_alpha*255));
#else
		MGFont *m_shared_font=m_text_font;
		ge->drawText(m_text_content,m_text_font,rect, m_lineBreakMode,m_text_alignment,m_text_font->getColor(),RGBAColorMake(255,255,255,m_alpha*255));
#endif
		}
		//ge->drawRoundRect(m_bounds,RGBAColorRef::lightGray());

	}

	void MILabel::initWithFrame(MGRect frame)
	{
		MIControl::initWithFrame(frame);
	}


	MIEventHandleState MILabel::touchesBegan(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		//...
		return MIEventHandleStatePass;
	}

	MIEventHandleState MILabel::touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		//...
		return MIEventHandleStatePass;
	}

	MIEventHandleState MILabel::touchesEnded(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		//...
		return MIEventHandleStatePass;
	}

	MIEventHandleState MILabel::touchesCancelled(const std::set<MITouch*>& touches, const MIEvent* const event)
	{
		//...
		return MIEventHandleStatePass;
	}

	MIEventHandleState MILabel::handleEvent(const MIEvent* const event)
	{
		//...
		return MIEventHandleStatePass;
	}

	z11::MGRect MILabel::textRectForBoundsLimitedToNumberOfLines( MGRect bounds,int numberOfLines )
	{
		static MGRect rc;
		return rc;
	}

	void MILabel::drawTextInRect( const MGRect& rect )
	{

	}

	void MILabel::setFontSizeAndColor( z11::uint_32 font_size, const RGBAColorRef& font_color /*= RGBAColorMake(32,0,32)*/ )
	{
		getFont()->setFaceSize(font_size);
		getFont()->setColor(font_color);
		//delete_and_set_null(m_text_font);
		//m_text_font=MGFont::create(font_size,font_color);
	}

	MGFont * MILabel::getFont()
	{
		return m_text_font;
	}

    void MILabel::adjustFrameToFitSize(bool only_width)
    {
        MGSize size = getFont()->getStringWidthAndHeight(getText());
        if(only_width){
            setFrame(MGRectMake(m_frame.x, m_frame.y, size.width, m_frame.height));
        }
        else{
            setFrame(MGRectMake(m_frame.x, m_frame.y, size.width, size.height));
        }
    }

}
