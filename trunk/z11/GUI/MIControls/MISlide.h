//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MISlide.h)  
/// @brief (本文件实现的功能的简述)  
///  
///(本文件实现的功能的详述)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-11-10)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MISLIDE_H_
#define _MISLIDE_H_
#pragma once
#include "MIControl.h"
#include "../../GraphicCore/MGImage.h"
namespace z11
{
	class MISlideThumb;
	class MISlide : public MIControl
	{
	public:
		MISlide(void);
		virtual ~MISlide(void);

		//Accessing the Slider’s Value
		void setValue(float _value);
		float value();

		//Accessing the Slider’s Value Limits
		MGPROPERTY(float,m_mininum_value,MininumValue);
		MGPROPERTY(float,m_maxinum_value,MaxinumValue);

		//Modifying the Slider’s Behavior
		MGPROPERTY(bool,m_continus,Continus);

		//Changing the Slider’s Appearance
		MGPROPERTY_RETAIN(MGImage*, m_mininum_value_image, MininumValueImage);
		MGPROPERTY_RETAIN(MGImage*, m_maxinum_value_image, MaxinumValueImage);
		MGPROPERTY(RGBAColorRef, m_minimum_track_tintColor, MinimumTrackTintColor);
		MGPROPERTY(RGBAColorRef, m_maximum_track_tintColor, MaximumTrackTintColor);
		MGPROPERTY(RGBAColorRef, m_thumb_tintColor, ThumbTintColor);
		MGImage *currentMinimumTrackImage();
		MGImage *currentMaximumTrackImage();
		MGImage *currentThumbImage();
		MGImage *minimumTrackImageForState(MIControlState state);
		MGImage *maximumTrackImageForState(MIControlState state);
		MGImage *thumbImageForState(MIControlState state);
		void setMinimumTrackImageForState(MGImage *image,MIControlState state);
		void setMaximumTrackImageForState(MGImage *image,MIControlState state);
		void setThumbImageForState(MGImage *image,MIControlState state);

		//Overrides for Subclasses
		virtual MGRect maximumValueImageRectForBounds(const MGRect &_bounds);
		virtual MGRect minimumValueImageRectForBounds(const MGRect &_bounds);
		virtual MGRect trackRectForBounds(const MGRect &_bounds);
		virtual MGRect thumbRectForBounds(const MGRect &_bounds,const MGRect &rect ,float _value);

		MIVIEW_DECLARE;
		void valueChange(float _value);
	private:
		float m_value;
    protected:
        MISlideThumb *thumb_view;
	};

}

#endif //_MISLIDE_H_
