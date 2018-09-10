//
//  RaymeServer.h
//  socket-server
//
//  Created by 王志鹏 on 15/9/11.
//  Copyright (c) 2015年 Rayme Tech Co.,LTD. All rights reserved.
//

#pragma once

#include <map>
#include <MGFoundation.h>
#include "RaymeSocket.h"
#include "RaymeNode.h"
#include "RaymeCommand.h"

using namespace z11;
using namespace std;

namespace RAYME_SERVER {
    
    class RaymeSocketClean;
    class RaymeReaderDelegate;
    class RaymeServer : public MGThread, public Syncable {
    protected:
        RaymeServer();
    public:
        virtual ~RaymeServer();
        void clearConnectionPool();
        RaymeSocket* getNodeConnection(const RaymeNode& node);
        void updateNodeConnection(const RaymeNode& node, RaymeSocket* const sock);
        void removeNodeConnection(const RaymeNode &node);
        void cleanSockets();
        static RaymeServer* initInstance(int port);
        void handleAdapterResponse(RaymeRequest& req, const string &res);
        void handleAdapterResponseData(RaymeRequest& req, const vector<char> &res);
    protected:
        virtual bool threadLoop();
        virtual int readyToRun();
    protected:
        int listen_port;
        RaymeSocketClean* cleaner;
        RaymeSocket listen_socket;
        map<RaymeNode, RaymeSocket*> nodes_pool;
        deque<RaymeSocket*> clean_pool;
    public:
        RaymeReaderDelegate* delegate;
    };
    
    class RaymeSocketClean : public MGThread {
    public:
        RaymeSocketClean();
        virtual ~RaymeSocketClean();
    protected:
        virtual bool threadLoop();
    public:
        RaymeServer* owner;
    };
}
