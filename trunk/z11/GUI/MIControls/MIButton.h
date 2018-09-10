//
//  MIButton.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-21.
//  Copyright 2011年 Zhejiang University. All rights reserved.
#pragma once

#include "MIControl.h"
#include "MILabel.h"
#include "MIImageView.h"
namespace z11
{
	typedef enum {
		MIButtonTypeCustom = 0,
		MIButtonTypeRoundedRect,
		MIButtonTypeDetailDisclosure,
		MIButtonTypeInfoLight,
		MIButtonTypeInfoDark,
		MIButtonTypeContactAdd,
	} MIButtonType;

    class MIButton : public MIControl
    {
    public:
        MIButton();
        virtual ~MIButton();

		//Creating Buttons
		static MIButton *buttonWithType(MIButtonType buttonType);

		//Configuring Button Title
		MIButtonType buttonType();
		MILabel *titleLabel();
        MGPROPERTY(RGBAColorRef, borderColor, BorderColor);
        MGPROPERTY(float, cornerRadius, CornerRadius);
        MGPROPERTY_BOOL(bool, borderEnabled, BorderEnabled);
        MGPROPERTY(float, borderWidth, BorderWidth);
		MGPROPERTY(bool,reversesTitleShadowWhenHighlighted,ReversesTitleShadowWhenHighlighted);				//Not Implemention
		
		void setTitleForState(const std::string &str,MIControlState state = MIControlStateNormal);
		void setTitleColorForState(RGBAColorRef color,MIControlState state = MIControlStateNormal);
		void setTitleShadowColorForState(RGBAColorRef color,MIControlState state = MIControlStateNormal);	//Not Implemention

		const std::string & titleForState(MIControlState state = MIControlStateNormal);
		RGBAColorRef titleColorForState(MIControlState state = MIControlStateNormal);
		RGBAColorRef titleShadowColorForState(MIControlState state = MIControlStateNormal);					//Not Implemention

		//Configuring Button Images
		MGPROPERTY(bool,adjustsImageWhenHighlighted,AdjustsImageWhenHighlighted);							//Not Implemention
		MGPROPERTY(bool,adjustsImageWhenDisabled,AdjustsImageWhenDisabled);									//Not Implemention
		MGPROPERTY(bool,showsTouchWhenHighlighted,ShowsTouchWhenHighlighted);								//Not Implemention
		MGImage *backgroundImageForState(MIControlState state = MIControlStateNormal);						//Not Implemention
		MGImage *imageForState(MIControlState state = MIControlStateNormal);
		void setBackgroundImageForState(MGImage *image,MIControlState state = MIControlStateNormal);		//Not Implemention
		void setImageForState(MGImage *image,MIControlState state = MIControlStateNormal);

		//Configuring Edge Insets
		MGPROPERTY(MGEdgeInsets,m_contentEdgeInsets,ContentEdgeInsets);										//Need Implemention
		MGPROPERTY(MGEdgeInsets,m_titleEdgeInsets,TitleEdgeInsets);											//Not Implemention
		MGPROPERTY(MGEdgeInsets,m_imageEdgeInsets,ImageEdgeInsets);											//Not Implemention
		
		MGPROPERTY(DrawRoundRectMode, m_draw_round_mode, DrawRoundMode);
		//Getting the Current State
		const string &currentTitle();
		const RGBAColorRef& currentTitleColor();
		const RGBAColorRef& currentTitleShadowColor();														//Not Implemention
		const MGImage *currentImage();
		const MGImage *currentBackgroundImage();															//Not Implemention
		MIImageView *imageView();																			//Not Implemention
		
		//Getting Dimensions
		//– backgroundRectForBounds:
		//– contentRectForBounds:
		//– titleRectForContentRect:
		//– imageRectForContentRect:

    protected:
		int m_current_state;
		void setCurrentState(int state);
        byte_8 m_is_clicked;
		MILabel *m_titleLabel;
		MIButtonType m_buttonType;
		std::vector<std::string> titles;
		std::vector<RGBAColorRef> colors;
		std::vector<MGImage *> images;
    public:
        MIRESPONDER_TOUCH_DECLARE
        MIVIEW_DECLARE;
    };
}