//
//  RaymeWriter.cpp
//  socket-server
//
//  Created by 王志鹏 on 15-9-11.
//  Copyright (c) 2014年 Rayme Tech Co.,LTD. All rights reserved.
//

#include "RaymeWriter.h"
#include "RaymeSocket.h"
#include "RaymeServer.h"
#include <iostream>
#include <fstream>

using namespace z11;

namespace RAYME_SERVER {
    
    CheckTimer::CheckTimer()
    {
        owner = NULL;
    }
    
    CheckTimer::~CheckTimer()
    {
        owner = NULL;
    }
    
    bool CheckTimer::threadLoop()
    {
        if (owner != NULL) {
            owner->checkCacheCommands();
        }
        MGSleep(2000);
        return true;
    }
    
    RaymeWriter::RaymeWriter()
    {
        commands.clear();
        cache_commands.clear();
        sock = NULL;
        cache_timer = new CheckTimer();
        cache_timer->owner = this;
        cache_timer->run();
		log_file_path = "";
    }
    
    RaymeWriter::~RaymeWriter()
    {
        requestExitAndWait();
        if (cache_timer != NULL) {
            cache_timer->requestExitAndWait();
            delete_and_set_null(cache_timer);
        }
    }
    
    bool RaymeWriter::threadLoop()
    {
        if (sock == NULL)
            return false;
        
        try {
			lock();
            // Send Commands Using RtdSocket //
            for (auto it = commands.begin(); it != commands.end(); ++it) {
                logCommand(*it);
                //MGLogD("Send CMD %d to Node %s.", it->getType(), sock->node.addr.c_str());
                vector<byte_8> data = it->getBytes();
                sock->sendData((char*)&(data[0]), (int)data.size());
                if ((*it).getType() != RaymeCOMMAND_OK) {
                    cache_commands.push_back(*it);
                }
            }
            commands.clear();
            pause();
			unlock();
        } catch (exception e) {
            //MGLogD("%s.", e.what());
			unlock();
            sock->node.server->removeNodeConnection(sock->node);
            return false;
        }
        return true;
    }
    
    void RaymeWriter::setRaymeSocket(RaymeSocket *s)
    {
        AUTO_LOCK_THIS;
        sock = s;
    }
    
    void RaymeWriter::addCommand(const RaymeCommand& cmd)
    {
        AUTO_LOCK_THIS;
        commands.push_back(cmd);
        resume();
    }
    
    void RaymeWriter::releaseCommand(const RaymeCommand& cmd)
    {
        AUTO_LOCK_THIS;
        auto it = cache_commands.begin();
        while (it != cache_commands.end()) {
            if ((*it).getNum() == cmd.getNum()) {
                it = cache_commands.erase(it);
            } else
                ++it;
        }
    }
    
    void RaymeWriter::addCommands(const deque<RaymeCommand>& cmds)
    {
        AUTO_LOCK_THIS;
        for (auto it = cmds.begin(); it != cmds.end(); ++it) {
            commands.push_back(*it);
        }
        resume();
    }
    
    void RaymeWriter::checkCacheCommands()
    {
        AUTO_LOCK_THIS;
        for (auto it = cache_commands.begin(); it != cache_commands.end(); ++it) {
            addCommand(*it);
        }
        cache_commands.clear();
    }

	void RaymeWriter::logCommand(const RaymeCommand& cmd)
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
    
}