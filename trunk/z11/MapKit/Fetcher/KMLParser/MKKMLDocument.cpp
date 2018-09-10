#include "MKKMLDocument.h"
#include "CommonBase/MGData.h"
#include "System/Shared/MGLog.h"
#include <sstream>
#include <fstream>
#include "ExternalLib/xml/rapidxml_utils.hpp"
#include "ExternalLib/xml/rapidxml_print.hpp"
using namespace std;
using namespace rapidxml;
namespace z11
{

	MKKMLDocument * z11::MKKMLDocument::initWithContentsOfFile( const std::string& path )
	{
		MKKMLDocument *doc=new MKKMLDocument();
		doc->parse(path);
		return doc;
	}

	void z11::MKKMLDocument::parse( const std::string& path )
	{
		MGData *data=MGData::dataWithContentsOfFile(path);
		if (data==NULL)
		{
			MGLogD("MKKMLDocument::parse Error:file %s not found",path.c_str());
			return ;
		}

		rapidxml::xml_document<> xml_doc;
		xml_doc.parse<0>((char *)data->bytes());
		rapidxml::xml_node<>* root = xml_doc.first_node();//<kml>
		root=root->first_node();//<Folder>
		if (strcmp(root->name(),"Folder")==0)
		{
			parseFolder(root);
		}
	}


	void MKKMLDocument::parseFolder( rapidxml::xml_node<>*folder_root )
	{
		rapidxml::xml_node<>* node=NULL;
		for (node=folder_root->first_node();node!=NULL;node=node->next_sibling())
		{
			if (strcmp(node->name(),"Folder")==0)
			{
				parseFolder(node);
			}
			else
			{
				if (strcmp(node->name(),"Style")==0)
				{
					parseStyle(node);
				}
				else if (strcmp(node->name(),"Placemark")==0)
				{
					parsePlacemark(node);
				}
			}
		}
	}

	MKKMLIconStyle * MKKMLDocument::parseStyle( rapidxml::xml_node<>*style_root )
	{
		rapidxml::xml_node<> *icon_style_node=style_root->first_node("IconStyle");
		if (icon_style_node!=NULL)
		{
			MKKMLIconStyle *style=new MKKMLIconStyle();
			style->style_id=style_root->first_attribute("id")->value();
			rapidxml::xml_node<> *icon_node=icon_style_node->first_node("Icon");
			if (icon_node!=NULL)
			{
				rapidxml::xml_node<> *href_node=icon_node->first_node("href");
				if (href_node->value()!=NULL)
				{
					style->icon_href=href_node->value();
				}


			}

			m_styles.push_back(style);
			return style;
		}
		return NULL;
	}

	MKKMLPlacemark * MKKMLDocument::parsePlacemark( rapidxml::xml_node<>*placemark_root )
	{
		MKKMLPlacemark *place_mark=new MKKMLPlacemark();
		rapidxml::xml_node<> *node=placemark_root->first_node("name");
		if (node!=NULL)
		{
			place_mark->m_name=node->value();
		}

		node=placemark_root->first_node("description");
		if (node!=NULL)
		{
			rapidxml::xml_node<> *cdata_node=node->first_node();
			if (cdata_node!=NULL)
			{
				std::string description=cdata_node->value();
				rapidxml::xml_document<> xml_doc;
				xml_doc.parse<0>((char *)description.c_str());
				rapidxml::xml_node<> *p_node=xml_doc.first_node("p");
				if (p_node!=NULL)
				{
					place_mark->m_text_description=p_node->value();
				}

				rapidxml::xml_node<> *img_node=xml_doc.first_node("img");
				if (img_node!=NULL)
				{
					rapidxml::xml_attribute<> *src=img_node->first_attribute("src");
					if (src!=NULL)
					{
						place_mark->m_image_description=src->value();
					}
				}

			}
			else
			{
				place_mark->m_text_description=node->value();
			}
		}

		node=placemark_root->first_node("styleUrl");
		if (node!=NULL)
		{
			place_mark->m_style_url=node->value();
			int skip_sharp=0;
			for (skip_sharp=0;place_mark->m_style_url[skip_sharp]=='#';skip_sharp++)
			{
			}
			place_mark->m_style_url.erase(0,skip_sharp);
		}

		node=placemark_root->first_node("Sound");
		if (node!=NULL)
		{
			place_mark->m_sound=parseSound(node);
		}

		node=placemark_root->first_node("Point");
		if (node!=NULL)
		{
			place_mark->geometry=parsePoint(node);
			m_placemarks.push_back(place_mark);
		}
		else
		{
			delete_and_set_null(place_mark);
			place_mark=NULL;
		}

		return place_mark;
	}

	MKKMLPoint* MKKMLDocument::parsePoint( rapidxml::xml_node<>*point_root )
	{
		MKKMLPoint *ret=new MKKMLPoint();
		ret->read(point_root->first_node("coordinates")->value());
		return ret;
	}

	MKKMLDocument::MKKMLDocument()
	{

	}

	bool MKKMLDocument::writeToFile( const std::string& path )
	{
		xml_document<> doc;
		// xml_header
		doc.append_node(doc.allocate_node(rapidxml::node_pi,doc.allocate_string("xml version=\"1.0\" encoding=\"utf-8\"")));
		doc.append_node(generate(doc));
		std::ofstream out(path.c_str());
		out << doc;
		return true;
	}

	rapidxml::xml_node<>* MKKMLDocument::generate( rapidxml::xml_document<>& doc )
	{
		xml_node<char> *root=doc.allocate_node(rapidxml::node_element,"kml","");
		xml_attribute<> *root_attribute=doc.allocate_attribute("xmlns","http://www.opengis.net/kml/2.2");
		root->append_attribute(root_attribute);
		if(true)
		{
			xml_node<char> *folder_root=doc.allocate_node(rapidxml::node_element,"Folder","");

			for (uint_32 i=0;i<m_styles.size();i++)
			{
				MKKMLIconStyle *icon_style=dynamic_cast<MKKMLIconStyle *>(m_styles[i]);
				if (icon_style!=NULL)
				{
					folder_root->append_node(generateStyle(doc,icon_style));
				}
			}
			for (uint_32 i=0;i<m_placemarks.size();i++)
			{
				folder_root->append_node(generatePlacemark(doc,m_placemarks[i]));
			}
			root->append_node(folder_root);
		}
		//doc.append_node(root);
		return root;
	}

	rapidxml::xml_node<>* MKKMLDocument::generateStyle(rapidxml::xml_document<>& doc,MKKMLIconStyle *style )
	{
		xml_node<> *style_root=doc.allocate_node(rapidxml::node_element,"Style","");
		xml_attribute<> *style_id=doc.allocate_attribute("id",doc.allocate_string(style->style_id.c_str()));
		style_root->insert_attribute(NULL,style_id);

		{
			xml_node<> *icon_style=doc.allocate_node(rapidxml::node_element,"IconStyle","");


			{
				xml_node<> *icon=doc.allocate_node(rapidxml::node_element,"Icon","");

				{

					xml_node<> *href=doc.allocate_node(rapidxml::node_element,"href",doc.allocate_string(style->icon_href.c_str()));
					icon->append_node(href);
				}
				
				icon_style->append_node(icon);
			}
			
			style_root->append_node(icon_style);
		}
		
		return style_root;

	}

	rapidxml::xml_node<>* MKKMLDocument::generatePlacemark(  rapidxml::xml_document<>& doc,MKKMLPlacemark *placemark )
	{
		xml_node<> *placemark_root=doc.allocate_node(rapidxml::node_element,"Placemark","");
		if (placemark->m_name!="")
		{
			xml_node<> *name_node=doc.allocate_node(rapidxml::node_element,"name",
				doc.allocate_string(placemark->m_name.c_str()));
			placemark_root->append_node(name_node);

			xml_node<> *description_node=doc.allocate_node(rapidxml::node_element,"description",NULL);
			{
				string description_cdata;
				if (placemark->m_text_description!="")
				{
					description_cdata="<p>"+placemark->m_text_description+"</p>";
				}
				if (placemark->m_image_description!="")
				{
					description_cdata+=("<img src=\""+placemark->m_image_description+"\" />");
				}

				description_node->append_node(doc.allocate_node(rapidxml::node_cdata,NULL,doc.allocate_string(description_cdata.c_str())));
			}
			placemark_root->append_node(description_node);
			
			{
				MKKMLPoint *point=dynamic_cast<MKKMLPoint *>(placemark->geometry);
				if (point!=NULL)
				{
					xml_node<> *geometry_node=generatePoint(doc,point);
					placemark_root->append_node(geometry_node);
				}
			}

			xml_node<> *styleUrl_node=doc.allocate_node(rapidxml::node_element,"styleUrl",
				doc.allocate_string(("#"+placemark->m_style_url).c_str()));
			placemark_root->append_node(styleUrl_node);

			if (placemark->m_sound!=NULL)
			{
				placemark_root->append_node(generateSound(doc,placemark->m_sound));
			}
		}
		return placemark_root;
	}

	rapidxml::xml_node<>* MKKMLDocument::generatePoint(  rapidxml::xml_document<>& doc,MKKMLPoint *point )
	{
		xml_node<> *point_root=doc.allocate_node(rapidxml::node_element,"Point","");
		stringstream ss;
		ss.precision(6);
		ss<<point->longitude<<","<<point->latitude<<","<<point->altitude;
		point_root->append_node(
			doc.allocate_node(rapidxml::node_element,"coordinates",
			doc.allocate_string(ss.str().c_str())));
		return point_root;
	}

	MKKMLSound * MKKMLDocument::parseSound( rapidxml::xml_node<>*sound_root )
	{
		if (sound_root!=NULL)
		{
			rapidxml::xml_node<> *link=sound_root->first_node("link");
			if (link!=NULL)
			{
				rapidxml::xml_node<> *href=link->first_node("href");
				if (href!=NULL)
				{
					MKKMLSound *sound=new MKKMLSound(href->value());
					return sound;
				}

			}

		}
		return NULL;
	}

	rapidxml::xml_node<>* MKKMLDocument::generateSound( rapidxml::xml_document<>& doc,MKKMLSound *sound )
	{
		if (sound==NULL||sound->link_href=="")
		{
			return NULL;
		}
		xml_node<> *sound_node=doc.allocate_node(node_element,"Sound","");
		{

			xml_node<> *link_node=doc.allocate_node(node_element,"link","");
			{

				xml_node<> *href_node=doc.allocate_node(node_element,"href",doc.allocate_string(sound->link_href.c_str()));
				link_node->append_node(href_node);

			}
			sound_node->append_node(link_node);
		}
		return sound_node;
	}

}