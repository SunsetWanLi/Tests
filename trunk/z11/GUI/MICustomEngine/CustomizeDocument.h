#pragma once

#include <set>
#include "CustomizeNode.h"

namespace z11
{
	class CustomizeNode;
	class CustomizeRootNode;
	class CustomizeMapNode;

	typedef std::map<std::string,CustomizeNode*> NodeMap;
	typedef NodeMap::iterator MyNodeIt;

	typedef std::map<std::string,RGBAColorRef> ColorMap;
	typedef ColorMap::iterator ColorIt;

	typedef std::map<std::string,MIView *>ViewMap;
	typedef ViewMap::iterator ViewIt;

	typedef vector<CustomizeNode*>::iterator CustomizeNodeIt;
	
	
	class CustomizeDocument
	{
	public:
		CustomizeDocument(void);
		~CustomizeDocument(void);
		static CustomizeDocument *setCustomizeView(MIView *root_view, std::string file_name,MIViewController *root_controller=NULL);
		MIView *findViewById(std::string _id);
		void addNodeById(std::string _id, CustomizeNode *_node);
		void addViewById(std::string _id,MIView *_view);
		ViewMap view_map; 
		 std::set<CustomizeNode*> need_to_delete_nodes;
		 NodeMap node_map;
		string root_view_name;
		string current_view;
		MGRect root_rect;
		float width,height;
		
	private:
		vector<CustomizeDocument*>layout_set;
        CustomizeRootNode *root_node; 
		CustomizeMapNode *root_map_node;
	};
}

