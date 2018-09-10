#pragma once
#include "MKKMLPlacemark.h"
#include "MKKMLStyle.h"
#include <vector>
#include "ExternalLib/xml/rapidxml.hpp"
namespace z11
{
	class MKKMLDocument
	{
	public:
		static MKKMLDocument *initWithContentsOfFile(const std::string& path);
		bool writeToFile(const std::string& path);

		MKKMLDocument();
		std::vector<MKKMLStyle *> m_styles;
		std::vector<MKKMLPlacemark *> m_placemarks;
	private:
		void parse(const std::string& path);
		void parseFolder(rapidxml::xml_node<>*folder_root);
		MKKMLIconStyle * parseStyle(rapidxml::xml_node<>*style_root);
		MKKMLPlacemark * parsePlacemark(rapidxml::xml_node<>*placemark_root);
		MKKMLSound *parseSound(rapidxml::xml_node<>*sound_root);
		MKKMLPoint* parsePoint(rapidxml::xml_node<>*point_root);
		rapidxml::xml_node<>* generate( rapidxml::xml_document<>& doc);
		rapidxml::xml_node<>* generateStyle( rapidxml::xml_document<>& doc,MKKMLIconStyle *style);
		rapidxml::xml_node<>* generatePlacemark( rapidxml::xml_document<>& doc,MKKMLPlacemark *placemark);
		rapidxml::xml_node<>* generatePoint(rapidxml::xml_document<>& doc,MKKMLPoint *point);
		rapidxml::xml_node<>* generateSound(rapidxml::xml_document<>& doc,MKKMLSound *point);

	};
}
