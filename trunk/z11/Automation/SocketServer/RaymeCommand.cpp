//
//  RaymeCommand.cpp
//  socket-server
//
//  Created by 王志鹏 on 15-9-11.
//  Copyright (c) 2015年 Rayme Tech Co.,LTD. All rights reserved.
//

#include "RaymeCommand.h"

using namespace z11;

namespace RAYME_SERVER {
    
#pragma mark - RaymeCommand -
    
    RaymeCommand::RaymeCommand()
    {
        type = RaymeCOMMAND_INVALID;
        num = 0;
        content.clear();
    }
    
    RaymeCommand::~RaymeCommand()
    {
        
    }
    
    vector<byte_8> RaymeCommand::getBytes() const 
    {
        vector<byte_8> ret;
        ret.clear();
        
        if (!cmdValid(type)) {
            return ret;
        }
        
        // Header
        ret.push_back(0xFF);
        ret.push_back(0xEE);
        // Type
        ret.push_back(type);
        // Number
        ret.push_back(num);
        ret.push_back(num >> 8);
        // Length
        uint_32 size = content.size();
        ret.push_back(size);
        ret.push_back(size >> 8);
        ret.push_back(size >> 16);
        ret.push_back(size >> 24);
        // Check
        uint_8 check = 0;
        for (uint_32 idx = 2; idx < 9; ++idx) {
            check += ret.at(idx);
        }
        ret.push_back(check);
        // Content
        for (uint_32 idx = 0; idx < size; ++idx) {
            ret.push_back(content.at(idx));
        }
        // Tailer
        ret.push_back(0xFF);
        ret.push_back(0xCC);
        
        return ret;
    }
    
    RaymeCommand RaymeCommand::compileCommand(RaymeCommandType t, uint_16 num, const vector<char>& content)
    {
        RaymeCommand cmd;
        cmd.type = t;
        cmd.num = num;
        cmd.content = content;
        return cmd;
    }
    
    RaymeCommand RaymeCommand::compileCommandStr(RaymeCommandType t, uint_16 num, const string& content)
    {
        RaymeCommand cmd;
        cmd.type = t;
        cmd.num = num;
        for (auto it = content.begin(); it != content.end(); ++it) {
            cmd.content.push_back(*it);
        }
        return cmd;
    }
    
    RaymeCommandType RaymeCommand::getType() const
    {
        return type;
    }
    
    int RaymeCommand::getNum() const
    {
        return num;
    }
    
    vector<char> RaymeCommand::getContent() const
    {
        return content;
    }
    
    string RaymeCommand::getContentStr() const
    {
        string ret;
        for (auto it = content.begin(); it != content.end(); ++it) {
            if (*it != 0) {
                ret.push_back(*it);
            }
        }
        return ret;
    }
    
    bool RaymeCommand::cmdValid(RaymeCommandType t)
    {
        return t >= RaymeCOMMAND_OK && t <= RaymeCOMMAND_RESPONSE;
    }
    
#pragma mark - RaymeRequest -

    RaymeRequest::RaymeRequest()
    {
        
    }
    
    RaymeRequest::~RaymeRequest()
    {
        
    }
    
}
