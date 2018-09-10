//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MISegmentedControl.h)  
/// @brief (本文件实现的功能的简述)  
///  
///(本文件实现的功能的详述)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-09-30)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MISEGMENTEDCONTROL_H_
#define _MISEGMENTEDCONTROL_H_
#pragma once

#include "MIControl.h"
#include <vector>
#include <string>
namespace z11
{
	typedef enum {
		MISegmentedControlStylePlain,
		MISegmentedControlStyleBordered,
		MISegmentedControlStyleBar,
		MISegmentedControlStyleBezeled,
	} MISegmentedControlStyle;

	class MISegmentedControl : public MIControl
	{
	public:
		MISegmentedControl(void);
		~MISegmentedControl(void);
		
		//Initializing a Segmented Control
		void initWithItems(const std::vector<MGImage *>& items);
		void initWithItems(const std::vector<std::string>& items);

		//Managing Segment Content
		void setImageForSegmentAtIndex(MGImage *image,uint_32 segment);
		const MGImage* imageForSegmentAtIndex(uint_32 segment);
		void setTitleForSegmentAtIndex(const std::string &title,uint_32 segment);
		const std::string &titleForSegmentAtIndex(uint_32 segment);
	
		//Managing Segments
		void insertSegmentWithImage(MGImage *image,uint_32 segment,bool animated=true);
		void insertSegmentWithTitle(const std::string &title,uint_32 segment,bool animated=true);
		void removeAllSegments();
		void removeSegmentAtIndex(uint_32 segment,bool animated=true);
		MGPROPERTY(int_32,m_selected_segment_index,SelectedSegmentIndex);
		uint_32 numberOfSegments();

		//Managing Segment Behavior and Appearance
		MGPROPERTY(bool,momentary,Momentary);
		MGPROPERTY(MISegmentedControlStyle,segmentedControlStyle,SegmentedControlStyle);
		MGPROPERTY(RGBAColorRef,tintColor,TintColor);

		//Font
		MGPROPERTY(uint_32,m_font_size,FontSize);

		void setEnabledForSegmentAtIndex(bool enabled ,uint_32 segment);
		bool isEnabledForSegmentAtIndex(uint_32 segment);
		void setContentOffsetForSegmentAtIndex(const MGSize &offset,uint_32 segment);
		const MGSize &contentOffsetForSegmentAtIndex(uint_32 segment);
		void setWidthForSegmentAtIndex(float width ,uint_32 segment);
		float widthForSegmentAtIndex(uint_32 segment);
	public:
		MIVIEW_DECLARE;
		
		virtual MIEventHandleState touchesBegan(const std::set<MITouch*>& touches,\
			const MIEvent* const event);
		virtual MIEventHandleState touchesEnded(const std::set<MITouch*>& touches,\
			const MIEvent* const event);
		virtual MIEventHandleState touchesCancelled(const std::set<MITouch*>& touches,\
			const MIEvent* const event);
	private:
		bool m_is_clicked;
		std::vector<std::string> titles;
		std::vector< MGImage * > images;
	};

}


#endif //_MISEGMENTEDCONTROL_H_
