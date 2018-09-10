#ifndef _CORE_RENDERSTYLE_POINTSTYLERENDER_H
#define _CORE_RENDERSTYLE_POINTSTYLERENDER_H

#include <string>
#include "../ImageRender/ImageStyleRender.h"
#include "GraphicCore/MGGraphicEngine.h"

using namespace z11;

namespace GIS_Embedded{

class PointStyleRender
{

public:
	PointStyleRender();
	~PointStyleRender();

	void drawPoint(int x,int y);
	void drawPoint(const MGPoint& point,RGBAColorRef color = RGBAColorMake(255,255,255), float size = 1.0f, RGBAColorRef blend = RGBAColorMake(255,255,255,255));
	void drawPoints(const vector<MGPoint>& points,RGBAColorRef color = RGBAColorMake(255,255,255), float size = 1.0f, RGBAColorRef blend = RGBAColorMake(255,255,255,255));

// public:
 	MGImage *m_bit_map;
	std::string m_filename;
};
}
#endif