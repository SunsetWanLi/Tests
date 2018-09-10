#ifndef _CORE_RENDERENGINE_CROSSLAYER_H
#define _CORE_RENDERENGINE_CROSSLAYER_H

#include <string>
using namespace std;

namespace GIS_Embedded{

	class CrossLayerStyle
	{
	public:
		CrossLayerStyle()
		{
			m_grade = 1;
			m_width = 3;
		}

	public:
		int m_grade;
		int m_width;
	};

	class CrossLayer {

	public:
		int m_layer_frc;
		CrossLayerStyle m_layer_style;

	};

}
#endif