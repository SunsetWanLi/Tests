//
//  MIDataTypeDef.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-21.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "../../CommonBase/CommonDef.h"

namespace z11 
{
//Key Words define.
#define FrameworkImp
#define CustomerImp
//Custom Type define.
    typedef void* WndHandle;

	enum {
		MIDataDetectorTypePhoneNumber   = 1 << 0,
		MIDataDetectorTypeLink          = 1 << 1,
		MIDataDetectorTypeAddress       = 1 << 2,
		MIDataDetectorTypeCalendarEvent = 1 << 3,
		MIDataDetectorTypeNone          = 0,
		MIDataDetectorTypeAll           = ~0,
	};
	typedef uint_32 MIDataDetectorTypes;

} //namesapce z11