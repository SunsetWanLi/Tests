#include "CustomizeDocument.h"
#include "CustomizeNode.h"
#include "../Base/MIView.h"
#include "../../System/Shared/MGLog.h"
#include "../../CommonBase/MGData.h"
using namespace rapidxml;
using namespace std;
namespace z11
{
	CustomizeDocument::CustomizeDocument(void)
	{
		root_rect=MGRectMake(0,0,MGDevice::getScreenWidth(),MGDevice::getScreenHeight());
		width=MGDevice::getScreenWidth();
		height=MGDevice::getScreenHeight();
	}

	CustomizeDocument::~CustomizeDocument(void)
	{
		if(layout_set.size()>0){
			for(int i=0;i<layout_set.size();i++){
				std::set<CustomizeNode *>::iterator it;
				for (it=layout_set[i]->need_to_delete_nodes.begin();it!=layout_set[i]->need_to_delete_nodes.end();it++){
					CustomizeNode *node=*it;
					delete_and_set_null(node);
				}
			}
		}
		else{
			std::set<CustomizeNode *>::iterator it;
			for (it=need_to_delete_nodes.begin();it!=need_to_delete_nodes.end();it++){
				CustomizeNode *node=*it;
				delete_and_set_null(node);
			}
		}

		delete_and_set_null(root_node);
	}
	
	CustomizeDocument *CustomizeDocument::setCustomizeView(MIView *root_view, std::string file_name, MIViewController *root_controller)
	{
		MGData *fdoc=MGData::dataWithContentsOfFile(file_name);
		xml_document<>  doc;
		doc.parse<0>((char *)(fdoc->bytes()));

		CustomizeDocument *layout_doc=new CustomizeDocument();
		xml_node<>* root=doc.first_node();
		layout_doc->root_rect = root_view->getFrame();
		if(strcmp(root->name(),"MapView")==0){
			layout_doc->root_map_node=CustomizeMapNode::loadXML(root,layout_doc,root_controller);
			layout_doc->root_map_node->create(root_view);
		}
		else{
			layout_doc->root_node=CustomizeRootNode::loadXML(root,layout_doc,root_controller);
			layout_doc->root_node->create(root_view);
		}
		delete_and_set_null(fdoc);
		return layout_doc;
	}
	MIView *CustomizeDocument::findViewById(std::string _id)
	{
		ViewIt it = view_map.find(_id);
		if(it!=view_map.end()){
			return it->second;
		}
		return NULL;
	}

	void CustomizeDocument::addViewById( std::string _id,MIView *_view )
	{
		view_map[_id]=_view;
	}

	void CustomizeDocument::addNodeById(std::string _id, CustomizeNode *_node)
	{
		node_map[_id]=_node;
		need_to_delete_nodes.insert(_node);
	}

}

