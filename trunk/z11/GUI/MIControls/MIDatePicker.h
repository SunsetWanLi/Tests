//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MIDatePicker.h)  
/// @brief (本文件实现的功能的简述)  
///  
///(本文件实现的功能的详述)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-11-11)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MIDATEPICKER_H_
#define _MIDATEPICKER_H_
#pragma once
#include "MIControl.h"
#include "../../System/Shared/MGTime.h"
namespace z11
{
	typedef enum {
		MIDatePickerModeTime,
		MIDatePickerModeDate,
		MIDatePickerModeDateAndTime,
		MIDatePickerModeCountDownTimer
	} MIDatePickerMode;
	class MIPickerView;
	class DatePickerDelegate;
	class MIDatePicker:public MIControl
	{
	public:
		MIDatePicker(void);
		~MIDatePicker(void);
		virtual void initWithFrame(MGRect frame);
		void setDate(const MGTime::Exploded& _date);
		MGTime::Exploded getDate();

		//Configuring the Date Picker Mode
		MGPROPERTY(MIDatePickerMode,datePickerMode,DatePickerMode);

		//Configuring Temporal Attributes
		MGPROPERTY(MGTime::Exploded,maximumDate,MaximumDate);
		MGPROPERTY(MGTime::Exploded,minimumDate,MinimumDate);
		MGPROPERTY(int,minuteInterval ,MinuteInterval);
		MGPROPERTY(MGTimeDelta,countDownDuration,CountDownDuration);
	private:
		MGTime time;
		MGTime::Exploded date;
		MIPickerView *impl;
		DatePickerDelegate *delegate;
	};


}

#endif //_MIDATEPICKER_H_
