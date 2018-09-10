//
//  MGLocationRegion.cpp
//  System
//
//  Created by King Asdy on 16-7-27.
//  Copyright (c) 2016年 Zhejiang University. All rights reserved.
//

#include "MGLocationRegion.h"

namespace z11 {
	MGLocationRegion::MGLocationRegion():uuid("")
    {
        
    }
    MGLocationRegion::~MGLocationRegion()
    {
        
    }
    
    MGBeacon::MGBeacon()
    {
        uuid = "";
        rssi = 0;
        distance = BEACON_UNKNOWN;
    }
    MGBeacon::~MGBeacon()
    {
        
    }
} //namespace z11