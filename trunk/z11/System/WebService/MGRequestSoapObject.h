//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGRequestSoapObject.h)  
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
#ifndef _MGREQUESTSOAPOBJECT_H_
#define _MGREQUESTSOAPOBJECT_H_
#include "MGSoapObject.h"
namespace z11
{
	class MGWebServiceDelegate;
	class MGRequestSoapObject:public MGSoapObject
	{
    public:
        virtual ~MGRequestSoapObject();
	public:
		MGRequestSoapObject (const std::string &url, const std::string &name_space,const std::string &method_name);
		std::string		m_url;
		MGWebServiceDelegate *delegate;
	};
}


#endif //_MGREQUESTSOAPOBJECT_H_
