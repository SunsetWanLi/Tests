#ifndef _CORE_RENDERENGINE_GMSTYLE_H
#define _CORE_RENDERENGINE_GMSTYLE_H

#include "PointStyleRender.h"
#include "LineStyleRender.h"
#include "PolygonStyleRender.h"
//#include "../ImageRender/ImageStyleRender.h"

namespace GIS_Embedded{

class GmStyleRender
{

public:

	PointStyleRender* m_point_render;
	LineStyleRender* m_line_render;
    PolygonStyleRender* m_polygon_render;
    
public:
	GmStyleRender()
	{
		m_point_render = NULL;
		m_line_render = NULL;
		m_polygon_render = NULL;
	}

    ~GmStyleRender()
	{
		delete m_point_render;
		delete m_line_render;
		delete m_polygon_render;
	}
};

}
#endif