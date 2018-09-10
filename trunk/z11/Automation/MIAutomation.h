//
//  MIAutomation.h
//  MIAutomation
//
//  Created by 王志鹏 on 16/8/11.
//  Copyright © 2016年 Rayme Tech Co.,LTD. All rights reserved.
//

#pragma once

#include <MGFoundation.h>
#include <MIKit.h>
#include "MIAutoAction.h"
#include "SocketServer/RaymeServer.h"
#include "SocketServer/RaymeReader.h"

#include <string>
#include <deque>

using namespace std;
using namespace RAYME_SERVER;

namespace z11 {
    
    typedef enum {
        SCRIPT_INVALID = -1,        // 无效脚本 //
        SCRIPT_OK = 0,              // 正常脚本 //
        SCRIPT_EMPTY,               // 空脚本 //
        SCRIPT_FAIL,                // 执行失败 //
        SCRIPT_NULL                 // 脚本注释或空行 //
    }ScriptStatus;
    
    class MIAutomation : public MGThread, RaymeReaderDelegate {
    private:
        MIAutomation();
    public:
        virtual ~MIAutomation();
    public:
        ScriptStatus initScriptWithFile(const string& script_file);
    protected:
        ScriptStatus addScript(const string& scr, const RaymeRequest& req);
        ScriptStatus executeScript();
        virtual bool threadLoop();
        virtual void handleRequest(const RaymeRequest& req);
    public:
        static MIAutomation* createInstance();
        static void releaseInstance();
        
        static void logBegin();
        static void logString(const string& str);
        static void logEnd();
    private:
        deque<MIAutoAction*> script;
        static Syncable lock;
        static MIAutomation* instance;
        static RaymeServer* rayme_server;
        
        static bool m_log_title;
        static bool m_script_finish;
    };
}
