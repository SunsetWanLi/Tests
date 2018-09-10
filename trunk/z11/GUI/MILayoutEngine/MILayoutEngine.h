//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MILayoutEngine.h)  
/// @brief (MILayoutEngine ��������layout xml)  
///  
///(���ļ�ʵ�ֵĹ��ܵ�����)  
///  
/// @version 0.1   (�汾����)  
/// @author        (Gu RongFang)  
/// @date          (11-10-13)  
///  
///  
///    �޶�˵��������汾  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MILAYOUTENGINE_H_
#define _MILAYOUTENGINE_H_
#pragma once
#include "MILayoutDocument.h"
namespace z11
{
	typedef std::map<std::string, std::vector<std::string> > StringsType;
	class MIView;
	class MILayoutEngine
	{
	public:
		static MILayoutEngine & sharedEngine();
		void loadStrings(const std::string & file_full_path);
		void loadColors(const std::string & file_full_path);
		MGColor* findColorById(std::string _id);
		const std::vector<std::string> * findStringsById(const std::string& _id);
	private:
		MILayoutDocument* loadLayout(const std::string & file_full_path);
		MILayoutEngine(void);
		~MILayoutEngine(void);
		StringsType strings;
		ColorsType colors;
		NodesType nodes;
		friend class MILayoutNode;
	};
}

#endif //_MILAYOUTENGINE_H_
