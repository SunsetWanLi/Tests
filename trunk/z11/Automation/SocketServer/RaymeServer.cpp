//
//  RaymeServer.cpp
//  socket-server
//
//  Created by 王志鹏 on 15/9/11.
//  Copyright (c) 2015年 Rayme Tech Co.,LTD. All rights reserved.
//

#include "RaymeServer.h"
#include "RaymeWriter.h"
#include "RaymeReader.h"

using namespace z11;

namespace RAYME_SERVER {
    
    RaymeSocketClean::RaymeSocketClean()
    {
        owner = NULL;
    }
    
    RaymeSocketClean::~RaymeSocketClean()
    {
        
    }
    
    bool RaymeSocketClean::threadLoop()
    {
        if (owner != NULL) {
            owner->cleanSockets();
        }
        pause();
        return true;
    }
    
    RaymeServer::RaymeServer()
    {
        listen_port = 0;
        nodes_pool.clear();
        clean_pool.clear();
    }
    
    RaymeServer::~RaymeServer()
    {
        if (listen_socket != 0) {
            listen_socket.shutdownSocket();
            listen_socket.closeSocket();
        }
        if (cleaner != NULL) {
            cleaner->requestExitAndWait();
            delete_and_set_null(cleaner);
        }
    }
    
    RaymeServer* RaymeServer::initInstance(int port)
    {
        RaymeServer* ret = new RaymeServer();
        ret->listen_port = port;
        ret->cleaner = new RaymeSocketClean();
        ret->cleaner->owner = ret;
        ret->cleaner->run();
        return ret;
    }
    
    int RaymeServer::readyToRun()
    {
        if (listen_socket != 0) {
            listen_socket.shutdownSocket();
            listen_socket.closeSocket();
        }
        listen_socket.create();
        while (listen_socket.bindOn(listen_port) == false)
            listen_port++;
        MGLogD("Socket Server BindOn Port: %d", listen_port);
        listen_socket.listenOn();
        return 0;
    }
    
    bool RaymeServer::threadLoop()
    {
        bool ret = true;
        RaymeSocket* sock = new RaymeSocket();
        string fromip;
		uint_32 port = 0;
        if (listen_socket.acceptFor(*sock, fromip, port)) {
            MGLogD("Accept Sock From %s:%d.", fromip.c_str(), port);
            RaymeNode node;
            node.server = this;
            node.addr = fromip;
			node.port = port;
            RaymeSocket* last_sock = getNodeConnection(node);
            if (last_sock != NULL) {
                delete_and_set_null(sock);
            } else {
                sock->node = node;
                updateNodeConnection(node, sock);
                sock->reader->delegate = delegate;
            }
        }
        MGSleep(5);
        return ret;
    }
    
    void RaymeServer::clearConnectionPool()
    {
        AUTO_LOCK_THIS;
        for (auto it = nodes_pool.begin(); it != nodes_pool.end(); ++it) {
            if (it->second != 0) {
                it->second->shutdownSocket();
                it->second->closeSocket();
                delete_and_set_null(it->second);
            }
        }
        nodes_pool.clear();
    }
    
    RaymeSocket* RaymeServer::getNodeConnection(const RaymeNode& node)
    {
        RaymeSocket* ret = NULL;
        AUTO_LOCK_THIS;
        auto it = nodes_pool.find(node);
        if (it != nodes_pool.end()) {
            ret = it->second;
        }
        return ret;
    }
    
    void RaymeServer::updateNodeConnection(const RaymeNode& node, RaymeSocket* const sock)
    {
        AUTO_LOCK_THIS;
        RaymeSocket* last_sock = NULL;
        auto it = nodes_pool.find(node);
        if (it != nodes_pool.end()) {
            last_sock = it->second;
        }
        if (last_sock != NULL && last_sock != sock) {
            last_sock->shutdownSocket();
            last_sock->closeSocket();
            delete_and_set_null(last_sock);
        }
        sock->node = node;
        nodes_pool[node] = sock;
        sock->initTransfer();
    }
    
    void RaymeServer::removeNodeConnection(const RaymeNode &node)
    {
        AUTO_LOCK_THIS;
        auto it = nodes_pool.find(node);
        if (it != nodes_pool.end()) {
            clean_pool.push_back(it->second);
			nodes_pool.erase(it);
        }
        if (clean_pool.size() > 0) {
            cleaner->resume();
        }
    }
    
    void RaymeServer::cleanSockets()
    {
        AUTO_LOCK_THIS;
        for (auto it = clean_pool.begin(); it != clean_pool.end(); ++it) {
            delete_and_set_null(*it);
        }
        clean_pool.clear();
    }
    
    void RaymeServer::handleAdapterResponse(RaymeRequest& req, const string &res)
    {
        AUTO_LOCK_THIS;
        auto it = nodes_pool.find(req.node);
        if (it != nodes_pool.end()) {
            vector<char> data;
            for (auto it = res.begin(); it != res.end(); ++it) {
                data.push_back(*it);
            }
            RaymeCommand cmd = RaymeCommand::compileCommand(RaymeCOMMAND_RESPONSE, req.cmd.getNum(), data);
            it->second->writer->addCommand(cmd);
        }
    }
    
    void RaymeServer::handleAdapterResponseData(RaymeRequest& req, const vector<char> &res)
    {
        AUTO_LOCK_THIS;
        auto it = nodes_pool.find(req.node);
        if (it != nodes_pool.end()) {
            RaymeCommand cmd = RaymeCommand::compileCommand(RaymeCOMMAND_RESPONSE, req.cmd.getNum(), res);
            it->second->writer->addCommand(cmd);
        }
    }

}
