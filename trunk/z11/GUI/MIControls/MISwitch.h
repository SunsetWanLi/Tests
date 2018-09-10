//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MISwitch.h)  
/// @brief (本文件实现的功能的简述)  
///  
///(本文件实现的功能的详述)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-09-28)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MISWITCH_H_
#define _MISWITCH_H_
#pragma once

#include "MIControl.h"
namespace z11
{
	class MISwitchImpl;
	class MISwitch : public MIControl
	{
	public:
		MISwitch(void);
		~MISwitch(void);
		bool isOn();
		void setOn(bool on,bool animated=true);
	public:
		MIVIEW_DECLARE;
	private:
		MISwitchImpl *impl;
		bool m_on;
	};
}

#endif //_MISWITCH_H_
