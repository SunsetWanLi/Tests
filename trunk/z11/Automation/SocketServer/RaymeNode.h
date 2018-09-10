//
//  RaymeNode.h
//  socket-server
//
//  Created by 王志鹏 on 15-9-11.
//  Copyright (c) 2015年 Rayme Tech Co.,LTD. All rights reserved.
//

#pragma once

#include <iostream>
#include <MGFoundation.h>

using namespace std;
using namespace z11;

namespace RAYME_SERVER {
    
    class RaymeServer;
    class RaymeNode {
    public:
        RaymeNode();
        ~RaymeNode();
        string getAddr() const;
        // 比较方式 IP:Port
        bool operator < (const RaymeNode& r) const
        {
            return getAddr() < r.getAddr();
        }
        bool operator > (const RaymeNode& r) const
        {
            return getAddr() > r.getAddr();
        }
        bool operator == (const RaymeNode& r) const
        {
            return getAddr() == r.getAddr();
        }
        bool operator != (const RaymeNode& r) const
        {
            return getAddr() != r.getAddr();
        }
    public:
        string addr;
        int port;
        RaymeServer* server;
    };
}

