//
//  MGLocation.h
//  System
//
//  Created by King Asdy on 11-10-19.
//  Copyright (c) 2011年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../CommonBase/MGObject.h"
#include "../Shared/MGTime.h"

namespace z11 {
    class MGLocation : public MGObject
    {
    public:
        MGLocation();
        virtual ~MGLocation();
    public:
        double longitude; //经度  //
        double latitude; //纬度  //
        double altitude; //海拔高度  //
        double speed; //速度  //
        double course; //方向  //
		double accuracy; //精度 //
        MGTime timestamp; // 定位的时间，从1970年开始 //
    };
} //namespace z11
