//
//  FtpUtil.h
//  System
//
//  Created by slj on 13-3-8.
//  Copyright (c) 2013年 Zhejiang University. All rights reserved.
//
#pragma once
#ifndef __System__FtpUtil__
#define __System__FtpUtil__

#include <string>
#include <list>
#include "FtpParseInfo.h"

namespace curlpp
{
    class Easy;
}

namespace z11
{
	class FtpUtil
	{
	public:
		FtpUtil();
        ~FtpUtil();
		
		// connect
		bool connect();
		void disconnect();

		// properties
		void setServer(const std::string &server, int port = 21);
		const std::string & getServer() const { return m_server; }
		int getCurlCode() const { return m_curl_code; }
		void setUserAndPassword(const std::string &user, const std::string &password);
        MGPROPERTY(long, m_time_out, TimeOut);

		// operations
        std::list<FtpParseInfo> getList(const std::string &remote_dir);
        double getFileSize(const string &remote_file_path);
        
		bool createDir(const std::string &remote_dir);
		bool removeDir(const std::string &remote_dir);
        bool removeFile(const std::string &remote_file_path);
		bool upload(const std::string &remote_file_path, const std::string &local_file_path);
		bool download(const std::string &remote_file_path, const std::string &local_file_path);
		
	private:
        // basic operations
        void initCommonParams();
		bool perform();
        bool initParentDirAndName(std::string &parent, std::string &name, const string &full_dir);
        bool postQuote(std::string &remote_dir, const std::string &value);

	private:
		curlpp::Easy* request;
        std::string m_server;
		long m_port;
		std::string m_user;
		std::string m_password;
		int m_curl_code;
	};
}
#endif /* defined(__System__FtpUtil__) */