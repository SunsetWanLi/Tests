//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGURLFetcher.h)  
/// @brief (本文件实现的功能的简述)  
///  
///(本文件实现的功能的详述)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-12-29)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MGURLFETCHER_H_
#define _MGURLFETCHER_H_
#include <string>
#include <list>

namespace curlpp
{
	class Easy;
}
namespace z11
{
	class MGURLFetcher
	{
	public:
		MGURLFetcher(const std::string &url="");
		void setURL(const std::string &url);
        void setUserPwd(const std::string &user, const std::string &pwd);
		const std::string &getURL();
		bool fetch(std::string &fetch_result);
		bool fetch(std::string &fetch_result, int& code);
		bool downloadFile(const std::string &file_path);
		bool downloadFile(const std::string &file_path, int& code);
		~MGURLFetcher();
		// default connection time_out: 10 sec
		void setConnectTimeOut(long connect_time_out);
        // include connection and transmission
        void setTimeOut(long time_out);
        void setHttpHeader(const std::list<std::string> &header);
        void setVerifyHost(long v);
        bool is_follow_location;

		curlpp::Easy* getRequest() const;

	private:
		bool perform();
		bool perform(int& code);
		curlpp::Easy* request;
		std::string m_url;
        long m_time_out;
		long m_connect_time_out;
        long m_verify_host;
	};
}

#endif //_MGURLFETCHER_H_
