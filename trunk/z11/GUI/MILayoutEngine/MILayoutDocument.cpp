#include "MILayoutDocument.h"
#include "../Base/MIView.h"
#include "../../System/Shared/MGLog.h"
#include "../../CommonBase/MGData.h"
using namespace rapidxml;
using namespace std;
namespace z11
{
	MILayoutDocument::MILayoutDocument(void)
	{
		root_node=NULL;
	}

	MILayoutDocument::~MILayoutDocument(void)
	{
        std::set<MILayoutNode *>::iterator it;
		for (it=need_to_delete_nodes.begin();it!=need_to_delete_nodes.end();it++)
		{
            MILayoutNode *node=*it;
			delete_and_set_null(node);
		}
        delete_and_set_null(root_node);
	}


	MIView * MILayoutDocument::findViewById( std::string _id, MIView* root )
	{
		//ÒÑ´æÔÚ
		ViewNodesIt vIt= view_nodes.find(_id);
		if (vIt!=view_nodes.end())
		{
			return vIt->second;
		}
		//Î´´æÔÚ
		NodesIt it=nodes.find(_id);
		if (it!=nodes.end())
		{
            MIView *ret = NULL;
            if (root == NULL) {
                ret=it->second->create(NULL, false);
            } else {
                ret = it->second->create(root, true);
            }
			view_nodes[_id]=ret;
			return ret;
		}
		MGLogD("MILayoutDocument::findViewById :%s failed",_id.c_str());
		return NULL;
	}

	MILayoutDocument* MILayoutDocument::setContentView( MIView *root_view,std::string file_name, bool according_view)
	{
		MGData *fdoc=MGData::dataWithContentsOfFile(file_name);
		xml_document<>  doc;
		doc.parse<0>((char *)(fdoc->bytes()));

		MILayoutDocument *layout_doc=new MILayoutDocument();
		xml_node<>* root=doc.first_node();

		layout_doc->root_node=MIViewLayouNode::loadXML(root,layout_doc);
		layout_doc->root_node->create(root_view, according_view);
		delete_and_set_null(fdoc);
		return layout_doc;
	}

	void MILayoutDocument::addNodeById( std::string _id,MILayoutNode *_node )
	{
        //NodesIt it=nodes.find(_id);
        //assert(it==nodes.end());
		nodes[_id]=_node;
        need_to_delete_nodes.insert(_node);
	}

	void MILayoutDocument::addViewById( std::string _id,MIView *_view )
	{
        //ViewNodesIt it=view_nodes.find(_id);
        //assert(it==view_nodes.end());
		view_nodes[_id]=_view;
	}

	void MILayoutDocument::addAttributeById( std::string _id,std::string _attribute,std::string _value )
	{
		nodes[_id]->addAttribute(_attribute,_value);
	}

	void MILayoutDocument::save( std::string file_name )
	{

		xml_document<> doc;
		// xml_header
		doc.append_node(doc.allocate_node(rapidxml::node_pi,doc.allocate_string("xml version='1.0' encoding='utf-8'")));
		doc.append_node(root_node->save(&doc));
		std::ofstream out(file_name.c_str());
		out << doc;
	}

}
