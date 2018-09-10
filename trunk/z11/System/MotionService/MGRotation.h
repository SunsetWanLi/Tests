//
//  MGRotation.h
//  System
//
//  Created by King Asdy on 13-01-07.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../CommonBase/MGObject.h"

namespace z11 {
    class MGRotation : public MGObject
    {
    public:
        MGRotation();
        virtual ~MGRotation();
    public:
        // The X-axis rotation rate in radians per second. The sign follows the right hand rule: If the right hand is wrapped around the X axis such that the tip of the thumb points toward positive X, a positive rotation is one toward the tips of the other four fingers.
        double x;
        // The Y-axis rotation rate in radians per second. The sign follows the right hand rule: If the right hand is wrapped around the Y axis such that the tip of the thumb points toward positive Y, a positive rotation is one toward the tips of the other four fingers.
        double y;
        // The Z-axis rotation rate in radians per second. The sign follows the right hand rule: If the right hand is wrapped around the Z axis such that the tip of the thumb points toward positive Z, a positive rotation is one toward the tips of the other four fingers.
        double z;
    };
} //namespace z11
