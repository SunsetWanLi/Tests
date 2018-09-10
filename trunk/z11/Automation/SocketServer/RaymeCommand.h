//
//  RaymeCommand.h
//  socket-server
//
//  Created by 王志鹏 on 15-9-11.
//  Copyright (c) 2015年 Rayme Tech Co.,LTD. All rights reserved.
//

#pragma once

#include <deque>
#include <vector>
#include <string>
#include <MGFoundation.h>
#include "RaymeNode.h"

using namespace std;
using namespace z11;

namespace RAYME_SERVER {
    
    typedef enum {
        RaymeCOMMAND_INVALID = -1,
        RaymeCOMMAND_OK = 0x00,
        RaymeCOMMAND_REQUEST,    // 0x01
        RaymeCOMMAND_RESPONSE    // 0x02
    }RaymeCommandType;
    
    class RaymeCommand {
    public:
        RaymeCommand();
        virtual ~RaymeCommand();
    public:
        vector<byte_8> getBytes() const;
        static RaymeCommand compileCommand(RaymeCommandType t, uint_16 num, const vector<char> &content);
        static RaymeCommand compileCommandStr(RaymeCommandType t, uint_16 num, const string &content);
        RaymeCommandType getType() const;
        int getNum() const;
        vector<char> getContent() const;
        string getContentStr() const;
        static bool cmdValid(RaymeCommandType t);
    protected:
        RaymeCommandType type;
        uint_16 num;
        vector<char> content;
    public:
        bool operator == (const RaymeCommand& r)
        {
            bool ret = true;
            if (content.size() != r.content.size()) {
                ret = false;
            } else {
                for (int idx = 0; idx < content.size(); ++idx) {
                    if (content[idx] != r.content[idx]) {
                        ret = false;
                        break;
                    }
                }
            }
            return (type == r.type && num == r.num && ret);
        }
    };
    
    class RaymeRequest {
    public:
        RaymeRequest();
        ~RaymeRequest();
    public:
        RaymeNode node;
        RaymeCommand cmd;
    };
}
