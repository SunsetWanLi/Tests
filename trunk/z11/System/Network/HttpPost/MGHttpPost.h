//
//  MGHttpPost.h
//  System
//
//  Created by slj on 13-3-8.
//  Copyright (c) 2013年 Zhejiang University. All rights reserved.
//
#pragma once
#ifndef __System__MGHttpPost__
#define __System__MGHttpPost__

#include <string>
#include <list>

namespace curlpp
{
    class Easy;
}

namespace z11
{
    class MGNameValuePair
    {
    public:
        MGNameValuePair();
        MGNameValuePair(const std::string &name, const std::string &value);
        
        std::string name;
        std::string value;
    };
    
	class MGHttpPost
	{
	public:
		MGHttpPost();
        ~MGHttpPost();
		void setURL(const std::string &url);
		const std::string &getURL();
		std::string sendPost(const std::list<MGNameValuePair> &params);
        std::string sendPost(const std::list<MGNameValuePair> &params, int& code);
        std::string sendPost(const std::list<MGNameValuePair> &params, const std::string &file_full_path);
		
		// default connection time_out: 10 sec
		void setConnectTimeOut(long connect_time_out);
        // include connection and transmission
        void setTimeOut(long time_out);
        void pushFilePair(const MGNameValuePair &param);

		void setPostJson(const std::string &json);
        void clearFilePair();
        
        void setVerifyHost(long v);

		void setHttpHeader(const std::list<std::string>& headers);
		//void setProxy(const std::string &host);
		curlpp::Easy* getRequest() const;
    private:
        void initCurlRequest(const std::list<MGNameValuePair> &params);
	private:
        std::string posted_json;
		curlpp::Easy* request;
        std::string m_url;
        long m_time_out;
		long m_connect_time_out;
        long m_verify_host;
		std::list<MGNameValuePair> m_file_pair;
		std::list<std::string> m_headers;

		
	};
}

#endif /* defined(__System__MGHttpPost__) */
