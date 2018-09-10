//
//  RaymeReader.h
//  socket-server
//
//  Created by 王志鹏 on 15-9-11.
//  Copyright (c) 2015年 Rayme Tech Co.,LTD. All rights reserved.
//

#pragma ONCE

#include <iostream>
#include <MGFoundation.h>
#include "RaymeCommand.h"
#include <deque>

using namespace std;
using namespace z11;

namespace RAYME_SERVER {
    
    class RaymeReaderDelegate;
    class RaymeSocket;
    class RaymeReader : public MGThread, public Syncable {
    public:
        RaymeReader();
        virtual ~RaymeReader();
    public:
        virtual bool threadLoop();
        void setRaymeSocket(RaymeSocket* s);
    protected:
        void analysisData();
        void handleCommand(const RaymeCommand& cmd);
        void handleOKCommand(const RaymeCommand& cmd);
		void logCommand(const RaymeCommand& cmd);
        void logString(const string& str);
        bool checkDuplicateCmd(uint_16 num);
    protected:
        deque<byte_8> reading_cache;
        RaymeSocket* sock;
        deque<uint_16> cmd_nums;
        uint_16 max_cmd_nums;
	public:
		string log_file_path;
        RaymeReaderDelegate* delegate;
    };
    
    class RaymeReaderDelegate {
    public:
        RaymeReaderDelegate();
        virtual ~RaymeReaderDelegate();
    public:
        virtual void handleRequest(const RaymeRequest& req) = 0;
    };
}

