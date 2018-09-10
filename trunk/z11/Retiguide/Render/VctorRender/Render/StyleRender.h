#ifndef _CORE_RENDERSTYLE_STYLERENDER_H
#define _CORE_RENDERSTYLE_STYLERENDER_H

#include <string>
#include <vector>

#include "Retiguide/RenderEngine/GmRender/PointStyleRender.h"
#include "Retiguide/RenderEngine/GmRender/LineStyleRender.h"
#include "Retiguide/RenderEngine/GmRender/PolygonStyleRender.h"
#include "Retiguide/RenderEngine/LabelRender/LabelStyleRender.h"
#include "Retiguide/RenderEngine/ImageRender/ImageStyleRender.h"
#include "Retiguide/RenderEngine/GmRender/GmStyleRender.h"
#include "Retiguide/Common/Share/UserScreenRect.h"

using namespace std;
using namespace z11;

namespace z11
{
	class MGCanvas;
}
namespace GIS_Embedded{

class StyleRender
{

public:

	StyleRender();
   
    void setGmStyleRender(GmStyleRender*& gm_render)
	{
		m_gm_render = gm_render;
	}

    void setLabelStyleRender(LabelStyleRender*& label_render)
	{
		m_label_render = label_render;
	}

	int getLineMeterWidth()
	{
		return m_gm_render->m_line_render->m_meter_width;
	}

	void setLinePixelWidth(int pixel_width)
	{
		if(pixel_width < 1)
			pixel_width = 1;
			
		m_gm_render->m_line_render->m_pixel_width = pixel_width;
	}

	int getLineFilterPara()
	{
		return m_gm_render->m_line_render->m_filter_meter;
	}

	int getPolygonFilterPara()
	{
		return m_gm_render->m_polygon_render->m_filter_meter;
	}

	void drawPoint(int x, int y);
	void drawPolyline(z11::MGPoint* points, int sum);
	void drawRoadBorder(z11::MGPoint* points,int sum);
	void drawRoadFace(z11::MGPoint* points,int sum);
    void fillPolygonWithFillColor(z11::MGPoint* points, int sum);
    void fillPolygonWithContextColor(z11::MGPoint* points, int sum);
	//lw
	void drawPolygonLine(z11::MGPoint* points,int sum);
	//void drawCircle(int x,int y,int r);

	void drawImage(MGImage* image,int left,int top,int right,int bottom);
    void drawString(int left, int top, int escape,const std::string& label);
	void drawStringCenterXY(int center_x, int center_y, int escape,const std::string& label);
	void drawCharacter(int left, int top, int escape, const std::string& label);
  	void getStringBound(const std::string& text,UserScreenRect *rect);
	void getStringExtent(const std::string& text, int *width, int *height);
	
	void setCanvas( MGCanvas * canvas);
private:
	int transformPointsToMgPoints(z11::MGPoint* points,int sum,vector<MGPoint>& mg_points)
	{
		MGPoint tmp_point;
		int i=0;
		for(i=0;i<sum;i++)
		{
			tmp_point.x = points[i].x;
			tmp_point.y = points[i].y;
			mg_points.push_back(tmp_point);
		}
		return sum;
	}

private:
	GmStyleRender* m_gm_render;
    LabelStyleRender* m_label_render;
   MGCanvas *m_canvas;

};
}
#endif