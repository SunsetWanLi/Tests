//
//  MIBaseTextView.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-7-6.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MIBaseTextView.h"

namespace z11 {
	MIBaseTextView::MIBaseTextView():
		m_text_content("")
    {
        m_text_alignment = MITextAlignmentLeftMiddle;
        m_text_border = MGPointMake(4.0f, 1.0f);
		m_lineBreakMode = MILineBreakModeClip;
		m_text_font = NULL;
    }
    MIBaseTextView::~MIBaseTextView()
    {
        delete_and_set_null(m_text_font);
    }

	void MIBaseTextView::setFontSizeAndColor(z11::uint_32 font_size, const RGBAColorRef& font_color)
	{	
		delete_and_set_null(m_text_font);
		m_text_font = MGFont::create(font_size, font_color);
	}
    
    void MIBaseTextView::setTextBorder(MGPoint border)
    {
        m_text_border = border;
    }

    MGPoint MIBaseTextView::getTextBorder()
    {
        return m_text_border;
    }

	void MIBaseTextView::resetText()
	{
		m_text_content.clear();
	}


}