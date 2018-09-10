//
//  MITextArea.h
//  Gui
//
//  Created by Hu DanYuan on 11-6-22
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#pragma once

#include "MIControl.h"
#include "MITextAreaCell.h"
#include "MIScrollView.h"
#include "MIEditableTextView.h"
#include "MIBaseTextView.h"
#include "../../System/Shared/Utility.h"
#include <string>

#define TEXTAREA_CELL_VIEW_INITIAL_DEFAULT_HEIGHT 40.0f
//#define TEXTAREA_CELL_VIEW_INITIAL_DEFAULT_WIDTH 40.0f
namespace z11
{
	class MITextAreaCell;
	class MITextAreaViewDataSource;
	class MITextArea : public MIScrollView
	{
	public:
		MITextArea();
		MITextArea(MITextAreaViewDataSource *textareaview_datasource);
		virtual ~MITextArea();
	public:
		MIRESPONDER_TOUCH_DECLARE;
		MIVIEW_DECLARE;

		void setTextAreaViewDataSource(MITextAreaViewDataSource* datasource){m_textareaview_datasource = datasource;}
        void setCellBackgroundColor(MGColor *_color);

		//text property
		virtual void setText(const std::string& text);
		virtual const std::string &getText();
		virtual void resetText();
		//font property 
		void setFontSizeAndColor(z11::uint_32 font_size, const RGBAColorRef& font_color = RGBAColorMake(32,0,32));
		
		//textAlignment  property
		void setTextAlignment(MITextAlignment alignment);
		MITextAlignment getTextAlignment();
		
		void setTextMode(MILineBreakMode text_mode);
		void setTextBorder(MGPoint border);
		MGPoint getTextBorder();

	private:
		void reloadData();
		
	private:
		MITextAreaCell *m_text_area_cell;
		MITextAreaViewDataSource *m_textareaview_datasource;
	};

	MIINTERFACE MITextAreaViewDataSource
	{
	public:
		virtual float heightForCell(MITextArea *textarea_view){return TEXTAREA_CELL_VIEW_INITIAL_DEFAULT_HEIGHT;}
        //virtual float widthForCell(MITextArea *textarea_view){return TEXTAREA_CELL_VIEW_INITIAL_DEFAULT_WIDTH;}
	};
#define MITEXTAREAVIEW_DATASOURCE_DECLARE \
	virtual float heightForCell(MITextArea *textarea_view); 
    //virtual float widthForCell(MITextArea *textarea_view);
}