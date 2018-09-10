//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (MILayoutDocument.h)
/// @brief (本文件实现的功能的简述)
///
///(本文件实现的功能的详述)
///
/// @version 0.1   (版本声明)
/// @author        (Gu RongFang)
/// @date          (11-10-14)
///
///
///    修订说明：最初版本
//////////////////////////////////////////////////////////////////////////
#ifndef _MILAYOUTDOCUMENT_H_
#define _MILAYOUTDOCUMENT_H_
#include "MILayoutNode.h"
#include <set>
#pragma once
namespace z11
{
	class MILayoutDocument
	{
	public:
		~MILayoutDocument(void);
		MIView *findViewById(std::string _id, MIView* root = NULL);
		static MILayoutDocument* setContentView(MIView *root,std::string file_name, bool according_view = false);
		void addNodeById(std::string _id,MILayoutNode *_node);
		void addViewById(std::string _id,MIView *_view);
		void addAttributeById(std::string _id,std::string _attribute,std::string _value);


		MILayoutDocument(void);
		void save(std::string file_name);
	private:
        MIViewLayouNode *root_node; //only one
		NodesType nodes;
		ViewNodesType view_nodes;
        std::set<MILayoutNode*> need_to_delete_nodes;
	};

}

#endif //_MILAYOUTDOCUMENT_H_
