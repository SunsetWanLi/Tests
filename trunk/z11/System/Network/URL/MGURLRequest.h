//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGURLRequest.h)  
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
#pragma once
#ifndef _MGURL_REQUEST_H_
#define _MGURL_REQUEST_H_
#include "MGURL.h"
#include "../../../CommonBase/MGData.h"
#include <map>
namespace z11
{
	typedef size_t MGTimeInterval;
	class MGURLRequest:public MGRefObject
	{
	public:
		//retain url
		static MGURLRequest *requestWithURL(MGURL *theUrl);
		//deep copy
		static MGURLRequest *requestWithRequest(const MGURLRequest *request);
		static MGURLRequest *requestWithURL(MGURL *theUrl,MGTimeInterval _timeoutInterval);
		MGURL *URL() const;
		~MGURLRequest();
		MGTimeInterval timeoutInterval() const;
		void setValueForHTTPHeaderField(const std::string &value,const std::string &field);
		string valueForHTTPHeaderField(const std::string &field);
		//retain data;
		void setHTTPBody(MGData *data);
		MGData* HTTPBody();
		void setHTTPMethod(const std::string &method);
		MGURLDictionary* allHTTPHeaderFields() ;
	private:
		MGData *m_http_body;
		MGURL *m_url;
		MGMutableURLDictionary http_header_field;
		MGTimeInterval m_timeoutInterval;
		MGURLRequest(MGURL *theUrl);
	};

	class MGMutableURLRequest:public MGURLRequest
	{

	};
}

#endif //_MGURL_REQUEST_H_
