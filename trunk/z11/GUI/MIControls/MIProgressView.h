//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MIProgressView.h)  
/// @brief (本文件实现的功能的简述)  
///  
///(本文件实现的功能的详述)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (12-06-06)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MIPROGRESSVIEW_H_
#define _MIPROGRESSVIEW_H_
#pragma once
#include "../Base/MIView.h"
namespace z11
{
	typedef enum {
		MIProgressViewStyleDefault,
		MIProgressViewStyleBar,
	} MIProgressViewStyle;
	
	class MIProgressView : public MIView
	{
	public:
		MIProgressView();
		virtual ~MIProgressView();
		virtual void drawself();

		//managing the progress
		float getProgress();
		void setProgress(float _progress, bool _animated = true);
		void setAnimationTotalTime(float _time);

		//the current graphical style of the receiver
		MGPROPERTY(MIProgressViewStyle, m_progress_view_style, ProgressViewStyle);
		//the color shown for the portion of the progress bar that is filled
		MGPROPERTY_RETAIN(MGColor*, m_progress_tint_color, ProgressTintColor);

	protected:
		//the current progress is represented by a floating-point value between 0.0f and 1.0f
		float m_progress;
		//YES if the change should be animated, NO if the change should happen immediately.
		bool animated;
		//the total time of the progress_animation from 0.0f to 1.0f
		float animation_total_time;
		
	private:
		float m_real_progress;
		bool progress_go_back;
	};
}
#endif //_MIPROGRESSVIEW_H_
