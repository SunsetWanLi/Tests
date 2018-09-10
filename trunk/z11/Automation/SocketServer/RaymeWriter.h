//
//  RaymeWriter.h
//  socket-server
//
//  Created by 王志鹏 on 15-9-11.
//  Copyright (c) 2015年 Rayme Tech Co.,LTD. All rights reserved.
//

#pragma once

#include "RaymeCommand.h"
#include <iostream>
#include <map>
#include <vector>
#include <MGFoundation.h>

using namespace z11;
using namespace std;

namespace RAYME_SERVER {
    
    class RaymeSocket;
    class CheckTimer;
    class RaymeWriter : public MGThread, public Syncable {
    public:
        RaymeWriter();
        virtual ~RaymeWriter();
    public:
        virtual bool threadLoop();
        void clearConnectionPool();
        void setRaymeSocket(RaymeSocket* s);
        void addCommand(const RaymeCommand& cmd);
        void addCommands(const deque<RaymeCommand>& cmds);
        void releaseCommand(const RaymeCommand& cmd);
        void checkCacheCommands();
		void logCommand(const RaymeCommand& cmd);
    protected:
        deque<RaymeCommand> commands;
        deque<RaymeCommand> cache_commands;
        RaymeSocket* sock;
        CheckTimer* cache_timer;
	public:
		string log_file_path;
    };
    
    class CheckTimer : public MGThread {
    public:
        CheckTimer();
        ~CheckTimer();
    public:
        virtual bool threadLoop();
    public:
        RaymeWriter* owner;
    };
}

