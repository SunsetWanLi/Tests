//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGURL.h)  
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
#ifndef _MGURL_H_
#define _MGURL_H_
#include <string>
#include <map>
#include <vector>
#include "../../../CommonBase/Memory/MGRefObject.h"
#include "../../../CommonBase/MGError.h"
namespace z11
{
	class MGURL:public MGRefObject
	{
	public:
		static MGURL *URLWithString(const std::string &url);
		std::string absoluteString() const;
		~MGURL();
	private:
		std::string m_url_string;
		MGURL(const std::string &url="");
	};

	class MGURLDictionary
	{
	public:
		static MGURLDictionary *dictionaryWithObjectsForKeys(const std::vector<std::string > &objects,const std::vector<std::string>& keys);
		const std::string &objectForKey(const std::string &key);
		static MGURLDictionary *dictionaryWithDictionary(const MGURLDictionary *dict);
		MGURLDictionary *initWithDictionary(const MGURLDictionary *dict);
		std::vector<std::string> allKeys();
	protected:
		std::map<std::string,std::string> key_value_pair;
	};
	class MGMutableURLDictionary :public MGURLDictionary
	{
	public:
		void setObjectForKey(const std::string &object,const std::string &key);
	};

}

#endif //_MGURL_H_
