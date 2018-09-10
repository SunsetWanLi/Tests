//
//  MGAttitude.h
//  System
//
//  Created by Gerry on 16-12-27.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../CommonBase/MGObject.h"

namespace z11 {
    class MGAttitude : public MGObject
    {
    public:
        MGAttitude();
        virtual ~MGAttitude();
    public:
		//roll --- one of Euler angle of device
		double roll;
		//pitch --- one of Euler angle of device
		double pitch;
		//yaw --- one of Euler angle of device
		double yaw;
    };
} //namespace z11
