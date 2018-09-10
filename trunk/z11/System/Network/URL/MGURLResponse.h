//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGURLResponse.h)  
/// @brief (���ļ�ʵ�ֵĹ��ܵļ���)  
///  
///(���ļ�ʵ�ֵĹ��ܵ�����)  
///  
/// @version 0.1   (�汾����)  
/// @author        (Gu RongFang)  
/// @date          (11-12-29)  
///  
///  
///    �޶�˵��������汾  
////////////////////////////////////////////////////////////////////////// 
#pragma once
#ifndef _MGURL_RESPONSE_H_
#define _MGURL_RESPONSE_H_
#include "MGURL.h"
namespace z11
{
	class MGURLResponse:public MGRefObject
	{
	public:
		//don't retain url
		static MGURLResponse *initWithURL(const MGURL *theUrl,const std::string &MIMEType,
			int expectedContentLength,const std::string &textEncodingName);
		static MGURLResponse *responseWithResponse(const MGURLResponse* response);
		MGURL *URL() const ;
		std::string MIMEType() const ;
		std::string textEncodingName() const ;
		int expectedContentLength() const ;
		std::string suggestedFilename() const ;
		~MGURLResponse();
		MGURLDictionary *allHTTPHeaderFields();
	private:
		MGURLDictionary m_allHTTPHeaderFields;
		std::string m_MIMEType;
		std::string m_textEncodingName;
		std::string m_suggestedFilename;
		int m_expectedContentLength;
		MGURL *m_url;
		MGURLResponse();
		MGURLResponse(const std::string &theUrl,const std::string &MIMEType,
			int expectedContentLength,const std::string &textEncodingName);
	};
}

#endif //_MGURL_RESPONSE_H_
