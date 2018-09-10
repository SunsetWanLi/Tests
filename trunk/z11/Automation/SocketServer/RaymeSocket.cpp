//
//  RaymeSocket.cpp
//  socket-server
//
//  Created by 王志鹏 on 15-9-11.
//  Copyright (c) 2015年 Rayme Tech Co.,LTD. All rights reserved.
//

#include "RaymeSocket.h"
#include "RaymeWriter.h"
#include "RaymeReader.h"

namespace RAYME_SERVER {
    
    RaymeSocket::RaymeSocket()
    {
        status = RaymeINVALID;
        reader = NULL;
        writer = NULL;
    }
    
    RaymeSocket::~RaymeSocket()
    {
        shutdownTranfer();
    }
    
    void RaymeSocket::initTransfer()
    {
        shutdownTranfer();
        
        MGLogD("RaymeSocket %p initTransfer.", this);
        
        writer = new RaymeWriter();
        writer->setRaymeSocket(this);
        writer->log_file_path = MGBundle::mainBundle()->documentsPathWithFileName("Log/"+node.addr+"_"+Utility::BuiltInTypeToString<uint_32>(node.port)+"_out.log");
        char ptr[100];
        sprintf(ptr, "%p", this);
        string name = ptr;
        name = "RaymeSocket: " + name + " Writer Thread.";
        writer->run(name.c_str());
        
        reader = new RaymeReader();
        reader->setRaymeSocket(this);
		reader->log_file_path = MGBundle::mainBundle()->documentsPathWithFileName("Log/"+node.addr+"_"+Utility::BuiltInTypeToString<uint_32>(node.port)+"_in.log");
        name = ptr;
        name = "RaymeSocket: " + name + " Reader Thread.";
        reader->run(name.c_str());
    }
    
    void RaymeSocket::shutdownTranfer()
    {
		if (reader != NULL) {
            reader->requestExitAndWait();
            delete_and_set_null(reader);
        }
        if (writer != NULL) {
            writer->requestExitAndWait();
            delete_and_set_null(writer);
        }
    }
    
}