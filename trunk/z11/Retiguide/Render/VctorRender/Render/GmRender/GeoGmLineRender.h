#ifndef _CORE_RENDERENGINE_GEOGMLINERENDER_H
#define _CORE_RENDERENGINE_GEOGMLINERENDER_H

#include "GeoGmRender.h"
#include "../StyleRender.h"

namespace GIS_Embedded
{
	enum LINE_RENDER_TYPE	
	{
		SOLID_LINE,
		PATH_BORDER,
		PATH_FACE
	};

	class GeoGmLineRender:public GeoGmRender
	{

	public:
		GeoGmLineRender()
		{
			m_render_type = SOLID_LINE;
		}

		void setRenderType(LINE_RENDER_TYPE type)
		{
			m_render_type = type;
		}

		int render(StyleRender& mg);

		int renderSolidLine(StyleRender& mg);
		int renderPathBorder(StyleRender& mg);
		int renderPathFace(StyleRender& mg);

	private:
		int transformPointsToMgPoints(z11::MGPoint* points,int sum,vector<MGPoint>& mg_points);
	private:
		LINE_RENDER_TYPE m_render_type;

	};
}

#endif