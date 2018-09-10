//
//  MITextAreaCell.h
//  Gui
//
//  Created by Hu DanYuan on 11-9-9
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MITextAreaCell.h"
#include "../../GraphicCore/MGGraphicEngine.h"
#include "../../System/Shared/MGLog.h"
#include "../../System/Shared/Utility.h"
#include "../../GraphicCore/MGFont.h"
#include "../Events/MIEvent.h"
#include "../GuiContainer/GuiContainer.h"
#include <assert.h>
#include <stdlib.h>
#include <cmath>

#define TEXTAREA_CELL_VIEW_INCREASE_IN_STEP 30.0f
namespace z11
{

	MITextAreaCell::MITextAreaCell()
	{
        class_name = "MITextAreaCell";
		m_text_content = "<TextArea>";
		m_text_alignment = MITextAlignmentLeftTop;
		m_text_font = NULL;
		m_lineBreakMode = MILineBreakModeCharacterWrap;
		_view_flags.clip_subviews = 1;
		setFontSizeAndColor(22,RGBAColorMake(32,0,32));
	}

	MITextAreaCell::~MITextAreaCell()
	{
		m_text_content.clear();
	}

	void MITextAreaCell::drawself()
	{
		//MIView::drawself();
        MGGraphicEngine *ge=MGGraphicEngine::sharedGraphicEngine();
		MGRect rect=m_bounds;
        rect.x = rect.x + 10.0f;
        rect.width = rect.width - 30.0f;
        RGBAColorRef color_ref;
        if (m_background_color != NULL) {
            color_ref = m_background_color->m_color;
            color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
        }else{
            color_ref = RGBAColorMake(0, 0, 0, (byte_8)(m_alpha * 255));
        }
		ge->drawFilledRect(m_bounds, color_ref);
		if (m_text_font == NULL)
		{
			ge->drawText(m_text_content,MGFont::shareFont(),rect, m_lineBreakMode,m_text_alignment);
		}
		else
        {
			ge->drawText(m_text_content,m_text_font,rect, m_lineBreakMode,m_text_alignment);
        }
	}

	void MITextAreaCell::drawBeforeSubviews()
	{
		/*float text_width = m_bounds.width - 2*m_text_border.x;
		float text_height = m_bounds.height - 2*m_text_border.y;
		MGRect rect = MGRectMake(m_bounds.x + m_text_border.x, m_bounds.y + m_text_border.y, text_width, text_height);
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
		if (graphic_engine != NULL)
			graphic_engine->drawText(m_text_content, m_text_font, rect,m_lineBreakMode,m_text_alignment);
         */
	}

	void MITextAreaCell::initWithFrame(MGRect frame)
	{
		MIControl::initWithFrame(frame);
		m_init_height = frame.height;
	}

	//管理响应链 
	//触摸事件的响应 
	MIEventHandleState MITextAreaCell::touchesBegan(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesBegan(touches, event);
		std::set<MITouch*>::const_iterator touch_it;
		for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
			MGPoint loc_in_view = (*touch_it)->locationInView(this);
            if (pointInside(loc_in_view, event)) {
				break;
			}
		}
		if (touch_it != touches.end()) {
			becomeFirstResponder();
		}
		return ret;
	}
	MIEventHandleState MITextAreaCell::touchesMoved(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesMoved(touches, event);
		return ret;
	}
	MIEventHandleState MITextAreaCell::touchesEnded(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesEnded(touches, event);
		std::set<MITouch*>::const_iterator touch_it;
		for (touch_it = touches.begin(); touch_it != touches.end(); ++touch_it) {
			MGPoint loc_in_view = (*touch_it)->locationInView(this);
            if (!pointInside(loc_in_view, event)) {
				break;
			}
		}
		if (touch_it != touches.end()) {
			resignFirstResponder();
		}
		return ret;
	}
	MIEventHandleState MITextAreaCell::touchesCancelled(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIControl::touchesCancelled(touches, event);
		return ret;
	}
	//事件响应 
	MIEventHandleState MITextAreaCell::handleEvent(const MIEvent* const event)
	{
		MIEventHandleState ret =  MIControl::handleEvent(event);
		switch (event->_event_type) {
			case KEYBOARD_INPUT:
            {
                m_preinput_text = "";
                MGTextContent* content = static_cast<MGTextContent*>(event->_content);
                if (content != NULL) {
                    insertText(content->text_content);
                }
                break;
            }
            case KEYBOARD_PRE_INPUT:
            {
                MGTextContent* content = static_cast<MGTextContent*>(event->_content);
                if (content != NULL) {
                    m_preinput_text = content->text_content;
                }
                break;
            }
            case KEYBOARD_DELETE_BACKWARD:
            {
                m_preinput_text = "";
				deleteBackward();
				MGLogD("KEYBOARD_DELETE_BACKWARD");
                break;
            }
            case KEY_DOWN_EVENT:
            {
                m_preinput_text = "";
                MGKeyDownContent* content = static_cast<MGKeyDownContent*>(event->_content);
                if (content != NULL) {
                    char ctr[2];
                    ctr[0] = content->key_code;
                    ctr[1] = 0;
                    string str = ctr;
                    insertText(str);
                }
                break;
            }
			default:
				break;
		}
		//adjustArea();
		return ret;
	}

	//MIEditableTextView interface//
	bool MITextAreaCell::hasText()
	{
		return (m_text_content.size() > 0);
	}
	void MITextAreaCell::insertText(const string& text)
	{
		m_text_content.append(text);
	}
	void MITextAreaCell::deleteBackward()
	{
		if (!m_text_content.empty())
		{
			char lastchar;
			if (m_text_content.length()>=3)
				lastchar=m_text_content[m_text_content.length()-3];
			if ((lastchar&0xF0)==0xE0) //中文的utf-8编码  
			{
				if (m_text_content.length()>=3)
				{
					m_text_content = m_text_content.substr(0,m_text_content.length()-3);
				}
				//m_text_content = m_text_content.substr(0,m_text_content.length()-3);
			}
			else //暂时不考虑中文和英文之外的字符  
				m_text_content = m_text_content.substr(0,m_text_content.length()-1);

		}
	}

	void MITextAreaCell::setText(const string& text)
	{
		MIBaseTextView::setText(text);
		//adjustArea();
	}

	void MITextAreaCell::resetText()
	{
		MIBaseTextView::resetText();
		//adjustArea();
	}
	
	const string & MITextAreaCell::getText()
	{
		return m_text_content;
	}

	void MITextAreaCell::adjustArea()
	{
		MGRect rect = getFrame();
		if (m_lineBreakMode&MILineBreakModeCharacterWrap)
		{
			MGSize text_size = m_text_font->getStringWidthAndHeight(m_text_content);
			MGLogD("m_text_content size(%d, %d)", text_size.width, text_size.height);
			if (text_size.height > rect.height-m_text_border.y)
			{
				rect.height += TEXTAREA_CELL_VIEW_INCREASE_IN_STEP;
			}
			else if (rect.height-m_text_border.y-text_size.height > TEXTAREA_CELL_VIEW_INCREASE_IN_STEP)
			{
				rect.height -= TEXTAREA_CELL_VIEW_INCREASE_IN_STEP;
			}
		}
		rect.height = MAXVALUE(m_init_height, rect.height);
		_setFrame(rect);
	}
} //namespace z11