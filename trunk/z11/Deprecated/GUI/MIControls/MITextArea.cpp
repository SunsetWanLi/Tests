//
//  MITextArea.h
//  Gui
//
//  Created by Hu DanYuan on 11-6-22
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MITextArea.h"
#include "../../GraphicCore/MGGraphicEngine.h"
#include "../../System/Shared/MGLog.h"
#include "../../System/Shared/Utility.h"
#include "../../GraphicCore/MGFont.h"
#include "../Events/MIEvent.h"
#include "../GuiContainer/GuiContainer.h"
#include <assert.h>
#include <stdlib.h>
#include <cmath>

namespace z11
{

	MITextArea::MITextArea()
	{
        class_name = "MITextArea";
		_scroll_view_flags.horizontal_bounce_enabled = 0;
		_scroll_view_flags.horizontal_indicator_show = 0;
		m_textareaview_datasource = NULL;

		m_text_area_cell = new MITextAreaCell;
        if (m_text_area_cell != NULL)
            m_text_area_cell->setBackgroundColor(MGColor::fromRGBA(255, 255, 255));
		//m_text_area_cell = NULL;
	}

	MITextArea::MITextArea(MITextAreaViewDataSource *textareaview_datasource):m_textareaview_datasource(textareaview_datasource)
	{
		_scroll_view_flags.horizontal_bounce_enabled = 0;
		_scroll_view_flags.horizontal_indicator_show = 0;
		m_text_area_cell = new MITextAreaCell;
        if (m_text_area_cell != NULL)
            m_text_area_cell->setBackgroundColor(MGColor::fromRGBA(255, 255, 255));
		//m_text_area_cell = NULL;
	}

	MITextArea::~MITextArea()
	{
	}

	void MITextArea::drawself()
	{
		MIView::drawself();
	}

	void MITextArea::initWithFrame(MGRect frame)
	{
		MIView::initWithFrame(frame);
		reloadData();
	}

	void MITextArea::reloadData()
	{
		if (m_textareaview_datasource==NULL || m_text_area_cell == NULL)
			return ;
			//m_textareaview_datasource=new MITextAreaViewDataSource();
		float height = m_textareaview_datasource->heightForCell(this);
        //float width = m_textareaview_datasource->widthForCell(this);
		//m_text_area_cell = new MITextAreaCell;
		m_text_area_cell->initWithFrame(MGRectMake(0.0f, 0.0f, m_frame.width, height));
		//m_text_area_cell->setBackgroundColor(MGColor::fromRGBA(255, 255, 255));
		addSubview(m_text_area_cell);
	}
    
    void MITextArea::setCellBackgroundColor(MGColor *_color)
    {
        if (m_text_area_cell != NULL)
            m_text_area_cell->setBackgroundColor(_color);
    }


	//管理响应链 
	//触摸事件的响应 
	MIEventHandleState MITextArea::touchesBegan(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIScrollView::touchesBegan(touches, event);
		return ret;
	}
	MIEventHandleState MITextArea::touchesMoved(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIScrollView::touchesMoved(touches, event);
		return ret;
	}
	MIEventHandleState MITextArea::touchesEnded(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIScrollView::touchesEnded(touches, event);
		return ret;
	}
	MIEventHandleState MITextArea::touchesCancelled(const std::set<MITouch*>& touches,
		const MIEvent* const event)
	{
		MIEventHandleState ret = MIScrollView::touchesCancelled(touches, event);
		return ret;
	}
	//事件响应 
	MIEventHandleState MITextArea::handleEvent(const MIEvent* const event)
	{
		MIEventHandleState ret =  MIScrollView::handleEvent(event);
		return ret;
	}

	void MITextArea::setFontSizeAndColor(z11::uint_32 font_size, const RGBAColorRef& font_color)
	{
		if (m_text_area_cell != NULL)
			m_text_area_cell->setFontSizeAndColor(font_size, font_color);
	}
	void MITextArea::setTextAlignment(MITextAlignment alignment)
	{
		if (m_text_area_cell != NULL)
			m_text_area_cell->setTextAlignment(alignment);
	}
	void MITextArea::setTextMode(MILineBreakMode text_mode)
	{
		if (m_text_area_cell != NULL)
			m_text_area_cell->setLineBreakMode(text_mode);
	}
	MITextAlignment MITextArea::getTextAlignment()
	{
		MITextAlignment ret = MITextAlignmentLeftMiddle;
		if (m_text_area_cell != NULL)
			ret = m_text_area_cell->getTextAlignment();
		return ret;
	}
	void MITextArea::setTextBorder(MGPoint border)
	{
		if (m_text_area_cell != NULL)
			m_text_area_cell->setTextBorder(border);
	}
	MGPoint MITextArea::getTextBorder()
	{
		MGPoint ret = MGPointMake(0.0f, 0.0f);
		if (m_text_area_cell != NULL)
			ret = m_text_area_cell->getTextBorder();
		return ret;
	}
	void MITextArea::setText(const string& text)
	{
		if (m_text_area_cell != NULL)
			m_text_area_cell->setText(text);
	}
	void MITextArea::resetText()
	{
		if (m_text_area_cell != NULL)
			m_text_area_cell->resetText();
	}
	const std::string& MITextArea::getText()
	{
		if (m_text_area_cell != NULL)
			return m_text_area_cell->getText();
		static string ret="";
		return ret;
	}

} //namespace z11