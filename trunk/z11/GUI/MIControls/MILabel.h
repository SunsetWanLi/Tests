//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MILabel.h)  
/// @brief (���ļ�ʵ�ֵĹ��ܵļ���)  
///  
///(���ļ�ʵ�ֵĹ��ܵ�����)  
///  
/// @version 0.1   (�汾����)  
/// @author        (Gu RongFang)  
/// @date          (11-09-27)  
///  
///  
///    �޶�˵��������汾  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MILABEL_H_
#define _MILABEL_H_

#include "MIControl.h"

namespace z11
{
	class MGFont;

	class MILabel : public MIControl
	{
	public:
		MILabel();
		virtual ~MILabel();

		//Accessing the Text Attributes
		MGPROPERTY(std::string,m_text_content,Text);
		void setFontSizeAndColor(z11::uint_32 font_size, const RGBAColorRef& font_color = RGBAColorRef::black());
		MGFont *getFont();
		MGPROPERTY(MITextAlignment,m_text_alignment,TextAlignment);
		MGPROPERTY(MILineBreakMode,m_lineBreakMode,LineBreakMode);
		//MIControl::enabled 

		//Sizing the Label��s Text
		MGPROPERTY(bool,adjustsFontSizeToFitWidth,AdjustsFontSizeToFitWidth);	//Not implemention
		MGPROPERTY(float,minimumFontSize,MinimumFontSize);						//Not implemention
		MGPROPERTY(int,numberOfLines,NumberOfLines);							//Not implemention

		//Managing Highlight Values
		MGPROPERTY(RGBAColorRef,highlightedTextColor,HighlightedTextColor);		//Not implemention,what's the default color
		//MIControl::isHighlighted

		//Drawing a Shadow
		MGPROPERTY(RGBAColorRef,shadowColor,ShadowColor);						//Not implemention
		MGPROPERTY(MGSize,shadowOffset,ShadowOffset);							//Not implemention

		//Drawing and Positioning Overrides
		virtual MGRect textRectForBoundsLimitedToNumberOfLines(MGRect bounds,int numberOfLines); //Not implemention

		//Setting and Getting Attributes
		MGPROPERTY(bool,userInteractionEnabled,UserInteractionEnabled);			//Not implemention
        
        void adjustFrameToFitSize(bool only_width = false);
	protected:
		MGFont *m_text_font;
		//Drawing and Positioning Overrides
		virtual void drawTextInRect(const MGRect& rect);						//Not implemention
	public:
		MIRESPONDER_TOUCH_DECLARE;
		MIVIEW_DECLARE;
	protected:
	};
}

#endif //_MILABEL_H_
