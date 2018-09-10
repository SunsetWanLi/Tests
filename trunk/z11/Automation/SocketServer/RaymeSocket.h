//
//  RaymeSocket.h
//  socket-server
//
//  Created by 王志鹏 on 15-9-11.
//  Copyright (c) 2015年 Rayme Tech Co.,LTD. All rights reserved.
//

#pragma once

#include <iostream>
#include <deque>
#include <MGFoundation.h>
#include "RaymeNode.h"

using namespace std;
using namespace z11;

namespace RAYME_SERVER {
    
    typedef enum {
        RaymeINVALID = 0,            // 初始无效值 //
        RaymeCREATED = 1,            // 连接创建 //
        RaymeCONNECTED = 1 << 1,     // 连接完成 //
        RaymeREADY = 1 << 9,         // 握手完成 //
        RaymeLOST = 1 << 10           // 连接丢失 //
    } RaymeServerConnectionStatus;
    
    class RaymeReader;
    class RaymeWriter;
    class RaymeServer;
    class RaymeSocket : public MGSocket {
    public:
        RaymeSocket();
        virtual ~RaymeSocket();
    public:
        void initTransfer();
    protected:
        void shutdownTranfer();
    public:
        RaymeServerConnectionStatus status;
        RaymeNode node;
    protected:
        RaymeWriter* writer;
        RaymeReader* reader;
        friend class RaymeWriter;
        friend class RaymeReader;
        friend class RaymeServer;
    };
}
