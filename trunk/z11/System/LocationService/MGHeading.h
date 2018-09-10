//
//  MGHeading.h
//  System
//
//  Created by King Asdy on 11-10-19.
//  Copyright (c) 2011年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../CommonBase/MGObject.h"

namespace z11 {
    class MGHeading : public MGObject
    {
    public:
        MGHeading();
        virtual ~MGHeading();
    public:
        double magnetic_heading;
        double true_heading;
        double heading_accuracy;
        double x;
        double y;
        double z;
    };
} //namespace z11