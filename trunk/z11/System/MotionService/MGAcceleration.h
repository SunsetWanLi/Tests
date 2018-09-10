//
//  MGAcceleration.h
//  System
//
//  Created by King Asdy on 13-01-07.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../CommonBase/MGObject.h"

namespace z11 {
    class MGAcceleration : public MGObject
    {
    public:
        MGAcceleration();
        virtual ~MGAcceleration();
    public:
        // Unit is a G. A G is a unit of gravitation force equal to that exerted by the earth’s gravitational field (9.81 m s−2).
        double x;
        double y;
        double z;
    };
} //namespace z11