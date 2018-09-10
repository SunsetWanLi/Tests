//
//  RaymeNode.cpp
//  socket-server
//
//  Created by 王志鹏 on 15-9-11.
//  Copyright (c) 2015年 Rayme Tech Co.,LTD. All rights reserved.
//

#include "RaymeNode.h"

namespace RAYME_SERVER {
    
    RaymeNode::RaymeNode()
    {
        addr = "";
        server = NULL;
        port = 0;
    }
    
    RaymeNode::~RaymeNode()
    {
        
    }
    
    string RaymeNode::getAddr() const
    {
        return addr + ":" + Utility::BuiltInTypeToString<int>(port);
    }
    
}