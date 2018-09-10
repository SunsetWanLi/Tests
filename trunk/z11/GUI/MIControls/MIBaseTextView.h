//
//  MIBaseTextView.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-7-6.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once
#include <string>
#include "../../CommonBase/MGStructure.h"
#include "../../GraphicCore/MGGraphicEngine.h"
#include "../../GraphicCore/MGFont.h"

using namespace std;

namespace z11 {
    MIINTERFACE MIBaseTextView
    {
    public:
        MIBaseTextView();
        virtual ~MIBaseTextView() = 0;
    public:
		MGPROPERTY( std::string , m_text_content , Text);
		MGPROPERTY(MITextAlignment,m_text_alignment,TextAlignment);
		MGPROPERTY(MILineBreakMode,m_lineBreakMode,LineBreakMode);
		void setFontSizeAndColor( z11::uint_32 font_size , const RGBAColorRef& font_color = RGBAColorRef::black() );
		MGFont *getFont();

        void setTextBorder(MGPoint border);
        MGPoint getTextBorder();
        virtual void resetText();
	protected:
        MGPoint m_text_border;
		MGFont *m_text_font;
    };
#define MIBASETEXTVIEW_DECLARE \
			public: \
            virtual void setText(const string& text); \
			virtual void resetText(); \
			virtual const string & getText();
} //namespace z11