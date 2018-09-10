//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGSoapObject.h)  
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
#ifndef _MGSOAPOBJECT_H_
#define _MGSOAPOBJECT_H_
#include <string>
#include <map>
#include "../../CommonBase/MGString.h"
namespace z11
{

	class MGSoapObject:public MGObject
	{
	public:
		MGSoapObject(const std::string &name_space,const std::string &method_name);
		virtual ~MGSoapObject();
		MGSoapObject *addProperty(const std::string &name,MGObject *value);
		MGObject *getProperty(const std::string &name);
		void setProperty(const std::string &name, MGObject *value);
		size_t getPropertyCount();
		MGObject *getProperty(size_t i);
		void autoClean();
		void manualClean();
		const std::string &getNamespace();
		const std::string &getMethodName();
		void setNamespace(const std::string &_namespace);
		void setMethodName(const std::string &method);
	private:
		std::map<std::string,MGObject *> m_property;
		std::string m_namespace;
		std::string m_method_name;
		bool m_auto_clean;
	};

}

#endif //_MGSOAPOBJECT_H_
