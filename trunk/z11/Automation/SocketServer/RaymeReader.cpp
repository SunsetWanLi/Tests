//
//  RaymeReader.cpp
//  socket-server
//
//  Created by 王志鹏 on 15-9-11.
//  Copyright (c) 2015年 Rayme Tech Co.,LTD. All rights reserved.
//

#include "RaymeReader.h"
#include "RaymeSocket.h"
#include "RaymeServer.h"
#include "RaymeWriter.h"
#include <exception>
#include <iostream>
#include <stdio.h>
#if defined(_WIN32) || defined(_WIN64)
#define io_mode_set ioctlsocket
#pragma comment(lib, "wsock32")
#else
#include <sys/ioctl.h>
#include <errno.h>
#define io_mode_set ioctl
#endif
#include <fstream>

using namespace std;
using namespace z11;

namespace RAYME_SERVER {
    
    RaymeReader::RaymeReader()
    {
        sock = NULL;
        reading_cache.clear();
		log_file_path = "";
        delegate = NULL;
        cmd_nums.clear();
        max_cmd_nums = 1000;
    }
    
    RaymeReader::~RaymeReader()
    {
        requestExitAndWait();
    }
    
    bool RaymeReader::threadLoop()
    {
        if (sock == NULL) {
            return false;
        }
        try {
			lock();
            fd_set fdread;
            fd_set fdexception;
            FD_ZERO(&fdread);
            FD_SET(*sock, &fdread);
            FD_ZERO(&fdexception);
            FD_SET(*sock, &fdexception);
            {
                int maxsock = (*sock) + 1;
                if (select(maxsock,  &fdread, NULL, &fdexception, NULL) > 0) {
                    if (FD_ISSET(*sock, &fdexception) > 0) {
                        throw exception();
                    }
                    // Read data from socket
                    char buf[1000];
                    memset(buf, 0, sizeof(buf));
                    int_64 count = sock->recvData(buf, 1000);
                    for (int idx = 0; idx < count; ++idx) {
                        reading_cache.push_back(buf[idx]);
                    }
                    if (count == 0) {
                        throw exception();
                    }
                    //MGLogD("Recv Data %s.", buf);
                    analysisData();
                }
            }
			unlock();
        } catch (exception e) {
			unlock();
            //MGLogD("%s.", e.what());
            sock->node.server->removeNodeConnection(sock->node);
            return false;
        }
        return true;
    }
    
    void RaymeReader::analysisData()
    {
        while (1) {
            RaymeCommand cmd;
            {
                AUTO_LOCK_THIS;
                //MGLogD("analysisData");
                if (reading_cache.size() <= 2) {
                    break;
                }
                byte_8 h1 = reading_cache.front();
                while (h1 != 0xFF && reading_cache.size() > 1) {
                    reading_cache.pop_front();
					h1 = reading_cache.front();
                }
                if (reading_cache.size() < 9) {
                    break;
                }
                byte_8 h2 = reading_cache.at(1);
                if (h1 == 0xFF && h2 == 0xEE) {
                    // try to analysis
                    RaymeCommandType cmd_type = (RaymeCommandType)reading_cache.at(2);
                    if (!RaymeCommand::cmdValid(cmd_type)) {
                        reading_cache.pop_front();
                        continue;
                    }
                    uint_16 num = (uint_16)reading_cache.at(3);
                    num += (uint_16)reading_cache.at(4) << 8;
                    uint_32 content_len = (uint_32)reading_cache.at(5);
                    content_len += (uint_32)reading_cache.at(6) << 8;
                    content_len += (uint_32)reading_cache.at(7) << 16;
                    content_len += (uint_32)reading_cache.at(8) << 24;
                    
                    if (reading_cache.size() < content_len + 12) {
                        break;
                    }
                    
                    uint_8 check = 0;
                    for (int idx = 2; idx < 9; ++idx) {
                        check += (uint_8)reading_cache.at(idx);
                    }
                    
                    uint_8 check_sum = reading_cache.at(9);
                    if (check_sum != check) {
                        reading_cache.pop_front();
                        continue;
                    }
                    
                    vector<char> content;
                    bool bad_string = false;
                    for (uint_32 idx = 0; idx < content_len; ++idx) {
                        if (reading_cache.at(idx+10) == 0) {
                            bad_string = true;
                            break;
                        }
                        content.push_back(reading_cache.at(idx+10));
                    }
                    if (bad_string) {
                        reading_cache.pop_front();
                        continue;
                    }
                    
                    byte_8 e1 = reading_cache.at(content_len+10);
                    byte_8 e2 = reading_cache.at(content_len+11);
                    if (e1 == 0xFF && e2 == 0xCC) {
                        cmd = RaymeCommand::compileCommand(cmd_type, num, content);
                        reading_cache.erase(reading_cache.begin(), reading_cache.begin()+content_len+11);
                    } else {
                        reading_cache.pop_front();
                        continue;
                    }
                } else {
                    reading_cache.pop_front();
                }
            }
            if (cmd.getType() != RaymeCOMMAND_INVALID) {
                handleCommand(cmd);
            }
        }
    }
    
    bool RaymeReader::checkDuplicateCmd(uint_16 num)
    {
        bool ret = false;
        for (auto it = cmd_nums.begin(); it != cmd_nums.end(); ++it)
        {
            if (*it == num) {
                ret = true;
                break;
            }
        }
        if (ret == false) {
            if (cmd_nums.size() >= max_cmd_nums) {
                cmd_nums.pop_front();
            }
            cmd_nums.push_back(num);
        }
        return ret;
    }
    
    void RaymeReader::handleCommand(const RaymeCommand &cmd)
    {
        logCommand(cmd);
        if (cmd.getType() == RaymeCOMMAND_OK) {
            return handleOKCommand(cmd);
        }
        if (cmd.getType() == RaymeCOMMAND_REQUEST)
        {
            logString(cmd.getContentStr());
        }
        RaymeCommand res_cmd = RaymeCommand::compileCommandStr(RaymeCOMMAND_OK, cmd.getNum(), "OK");
        sock->writer->addCommand(res_cmd);
        
        // Do anything else
        if (delegate != NULL && checkDuplicateCmd(cmd.getNum()) == false) {
            RaymeRequest req;
            req.node = sock->node;
            req.cmd = cmd;
            delegate->handleRequest(req);
        }
    }
    
    void RaymeReader::handleOKCommand(const RaymeCommand &cmd)
    {
        sock->writer->releaseCommand(cmd);
    }
    
    void RaymeReader::setRaymeSocket(RaymeSocket *s)
    {
        AUTO_LOCK_THIS;
        sock = s;
        //MGLogD("Reader %p create for RtdSocket %p.", this, sock);
    }
    
    void RaymeReader::logCommand(const RaymeCommand& cmd)
    {
        if (log_file_path.size() > 0)
        {
            ofstream file;
            file.open(log_file_path.c_str(), ios::app | ios::out);
            if (file) {
                MGTime time = MGTime::Now();
                MGTime::Exploded ex;
                time.explode(ex, true);
                file<<ex.hour<<":"<<ex.minute<<":"<<ex.second<<": ";
                vector<byte_8> bytes = cmd.getBytes();
                for (auto it = bytes.begin(); it != bytes.end(); ++it) {
                    string data = Utility::BuiltInTypeToString<uint_16>(*it,2,std::hex);
                    Utility::toUpper(data);
                    file<<data.c_str()<<" ";
                }
                file<<endl;
                file.close();
            }
        }
    }
    
    void RaymeReader::logString(const string& str)
    {
        if (log_file_path.size() > 0)
        {
            ofstream file;
            file.open(log_file_path.c_str(), ios::app | ios::out);
            if (file) {
                MGTime time = MGTime::Now();
                MGTime::Exploded ex;
                time.explode(ex, true);
                file<<ex.hour<<":"<<ex.minute<<":"<<ex.second<<": "<<str<<endl;
                file.close();
            }
        }
    }
    
#pragma mark - RaymeReaderDelegate -
    
    RaymeReaderDelegate::RaymeReaderDelegate()
    {
        
    }
    
    RaymeReaderDelegate::~RaymeReaderDelegate()
    {
        
    }
    
    void RaymeReaderDelegate::handleRequest(const RAYME_SERVER::RaymeRequest &req)
    {
        
    }
    
}
