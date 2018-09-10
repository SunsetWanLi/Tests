//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGRequestSoapObject.h)  
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
