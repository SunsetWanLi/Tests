#include <cstring>
#include "MILayoutEngine.h"
#include "../../System/IO/MGBundle.h"
#include "../../System/Shared/MGLog.h"
#include "../../CommonBase/MGData.h"
#include "../../ExternalLib/xml/rapidxml.hpp"
#include "../../ExternalLib/xml/rapidxml_utils.hpp"
#include "../../ExternalLib/xml/rapidxml_print.hpp"
#include "../../System/Shared/Utility.h"
using namespace rapidxml;
using namespace std;


namespace z11
{
	class MITextViewLayoutNode;


	MILayoutEngine::MILayoutEngine(void)
	{
	}

	MILayoutEngine::~MILayoutEngine(void)
	{
	}

	MILayoutEngine& MILayoutEngine::sharedEngine()
	{
		static MILayoutEngine engine;
		return engine;
	}

	xml_document<> * read_document(std::string file_name)
	{
		file<> fdoc(file_name.c_str());
		xml_document<>  *doc=new xml_document<>;
		doc->parse<0>(fdoc.data());
		return doc;
	}

	void MILayoutEngine::loadStrings( const std::string & file_full_path )
	{
		MGData *fdoc = MGData::dataWithContentsOfFile(file_full_path);
		xml_document<> doc;
		doc.parse<0>((char *)(fdoc->bytes()));

		xml_node<>* root = doc.first_node();//<resources>

		for(xml_node<>* node = root->first_node(); node != NULL; node = node->next_sibling())
		{
			vector<string> node_strings;
            for(xml_node<>* sub_node = node->first_node(); sub_node != NULL; sub_node = sub_node->next_sibling())
            {
                node_strings.push_back(sub_node->value());
            }
            if(node_strings.empty())
            {
                node_strings.push_back(node->value());
            }
			string key = node->first_attribute("name")->value();
			strings[key] = node_strings;
		}
        delete_and_set_null(fdoc);
	}

	void MILayoutEngine::loadColors( const std::string & file_full_path )
	{
		MGData *fdoc = MGData::dataWithContentsOfFile(file_full_path);
		if (fdoc==NULL)
		{
			return ;
		}
		//MGLogD("MILayoutEngine::loadColors %s",file_name.c_str());
		xml_document<>  doc;
		doc.parse<0>((char *)(fdoc->bytes()));

		//! 获取根节点
		xml_node<>* root = doc.first_node();//<resources>

		//获取数据
		for(xml_node<>* color=root->first_node();color!=NULL;color=color->next_sibling())
		{
			int rgb=0;
#if 0
			MGLogD("Colors %s:%s",color->first_attribute("name")->value(),color->value());
#endif
			if (strlen(color->value()+1)==6) //RRGGBB
			{
				sscanf(color->value()+1,"%x",&rgb);
				colors[color->first_attribute("name")->value()]=
					RGBAColorMake((rgb>>16)&0xFF,(rgb>>8)&0xFF,rgb&0xFF,0xFF);
			}
			else if(strlen(color->value()+1)==8)//AARRGGBB
			{
				sscanf(color->value()+1,"%x",&rgb);
				colors[color->first_attribute("name")->value()]=
					RGBAColorMake((rgb>>16)&0xFF,(rgb>>8)&0xFF,rgb&0xFF,(rgb>>24)&0xFF);

			}
		}
        delete_and_set_null(fdoc);
		//MGLogD("MILayoutEngine::loadColors %s over.",file_name.c_str());
	}

	MILayoutDocument* MILayoutEngine::loadLayout( const std::string & file_full_path )
	{
		return NULL;
		//return MILayoutDocument::load(file_name);
	}

	MGColor* MILayoutEngine::findColorById( std::string _id )
	{
		MGColor *color=NULL;
		{
            string file_name = "";
			if (_id.find("@drawable")!=string::npos) {
				_id.erase(0,sizeof("@drawable"));
                file_name =MGBundle::mainBundle()->drawablePathWithFileName(_id);
            } else {
                file_name = MGBundle::mainBundle()->pathWithFileName(_id);
            }
			ColorsType &colors=MILayoutEngine::sharedEngine().colors;
			ColorsIt it;
			if (_id!="")
			{
				if ((it=colors.find(_id))!=colors.end()) //it's color
				{
					color=MGColor::fromColorRef(it->second);
				}
				else //it's image
				{
					color=MGColor::fromImage(file_name);
					if (color==NULL)
					{
						MGLogD("can't find color:%s",_id.c_str());
					}
				}
			}
		}
		return color;
	}

	const std::vector<std::string> * MILayoutEngine::findStringsById( const std::string& _id )
	{
		StringsType::iterator it=strings.find(_id);
		if (it!=strings.end())
			return &(it->second);
		else
			return NULL;
	}
}
