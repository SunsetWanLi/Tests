//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MIDatePicker.h)  
/// @brief (���ļ�ʵ�ֵĹ��ܵļ���)  
///  
///(���ļ�ʵ�ֵĹ��ܵ�����)  
///  
/// @version 0.1   (�汾����)  
/// @author        (Gu RongFang)  
/// @date          (11-11-11)  
///  
///  
///    �޶�˵��������汾  
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
