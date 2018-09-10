#pragma once
#include "MKKMLFeature.h"
#include <string>
#include "MKKMLGeometry.h"
#include "MKKMLSound.h"
namespace z11
{

	class MKKMLPlacemark:public MKKMLFeature
	{
	public:
		MKKMLPlacemark(void);
		~MKKMLPlacemark(void);
		std::string m_name;
		std::string m_text_description;
		std::string m_image_description;
		std::string m_style_url;
		MKKMLGeometry *geometry;
		MKKMLSound *m_sound;
	};

}