#include "MITextButton.h"
#include "../../System/Shared/Utility.h"
#include "../../GraphicCore/MGFont.h"
#include "../../System/Device/MGDevice.h"
#include "../../GraphicCore/MGGraphicEngine.h"
#include "../../System/Shared/MGLog.h"
#include <string>

namespace z11
{

	MITextButton::MITextButton()
	{
        class_name = "MITextButton";
		m_text_content = "";
		m_text_font = MGFont::create(22,RGBAColorMake(32,0,32));
		m_background_color=MGColor::fromRGBA(255,255,255);
	}

	MITextButton::~MITextButton()
	{
		MGLogD("~MITextButton()");
	}

	void MITextButton::drawself()
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
		MGRect rect = MGRectMake(m_bounds.x + m_text_border.x, m_bounds.y + m_text_border.y, m_bounds.width - m_text_border.x, m_bounds.height - m_text_border.y);
		RGBAColorRef color_ref;
		if (m_background_color != NULL) {
			color_ref = m_background_color->m_color;
			color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
		}else{
			color_ref = RGBAColorMake(0, 0, 0, (byte_8)(m_alpha * 255));
		}
		if (m_is_clicked == 0 || (m_action_type & MIActionTypeBlend) == 0) {
			graphic_engine->drawFilledRect(m_bounds, color_ref);
			graphic_engine->drawText(m_text_content, m_text_font, rect);
		}else{
			graphic_engine->drawFilledRect(m_bounds, color_ref, RGBAColorRef::lightBlue());
			graphic_engine->drawText(m_text_content, m_text_font, rect);
		}

			
		//MIView::drawself();
		//MGGraphicEngine *ge=MGGraphicEngine::sharedGraphicEngine();
		//ge->drawText(m_text_content,m_text_font,m_bounds, m_text_mode);
	}

	void MITextButton::initWithFrame(MGRect frame)
	{
		MIControl::initWithFrame(frame);
	}

	MIEventHandleState MITextButton::touchesBegan(const std::set<MITouch*>& touches,const MIEvent* const event)
	{
		return MIButton::touchesBegan(touches, event);
	}

	MIEventHandleState MITextButton::touchesMoved(const std::set<MITouch*>& touches,const MIEvent* const event)
	{
		return MIButton::touchesMoved(touches, event);
	}

	MIEventHandleState MITextButton::touchesEnded(const std::set<MITouch*>& touches,const MIEvent* const event)
	{
		return MIButton::touchesEnded(touches, event);
	}

	MIEventHandleState MITextButton::touchesCancelled(const std::set<MITouch*>& touches,const MIEvent* const event)
	{
		return MIButton::touchesCancelled(touches, event);
	}

	MIEventHandleState MITextButton::handleEvent(const MIEvent* const event)
	{
		return MIButton::handleEvent(event);
	}

	void MITextButton::setText(const string& text)
	{
		m_text_content = text;
	}

	void MITextButton::resetText()
	{
		m_text_content.clear();
	}

	const string &MITextButton::getText()
	{
		return m_text_content;
	}
}//z11