#ifndef _CORE_RENDERSTYLE_POLYGONSTYLERENDER_H
#define _CORE_RENDERSTYLE_POLYGONSTYLERENDER_H

#include "GraphicCore/MGGraphicEngine.h"

using namespace z11;
namespace z11
{
	class MGCanvas;
}
namespace GIS_Embedded{
class PolygonStyleRender
{

public:

	PolygonStyleRender()
	{
		m_face_color = RGBAColorMake(97,229,138);
		m_edge_color = RGBAColorMake(0,0,0);
		m_context_color = RGBAColorMake(0,0,0);
		m_filter_meter = 1;
		m_is_antialias = false;

		m_is_draw_pattern = false;
	}


 	void drawPolygonLine(MGCanvas *canvas,const vector<MGPoint>& control_points); //lw
 
	void fillRectWithFillColor(MGCanvas *canvas,int left,int top,int width,int height);

	void fillPolygonWithFillColor(MGCanvas *canvas,const vector<MGPoint>& points);

	void fillPolygonWithContextColor(MGCanvas *canvas,const vector<MGPoint>& points);
 
 	static void fillRect(MGCanvas *canvas,int left,int top,int width,int height,int r,int g,int b);
	static void fillRect(MGCanvas *canvas,int left,int top,int width,int height,RGBAColorRef color);
 
	void setFaceColor(int r,int g,int b)
	{
		m_face_color = RGBAColorMake(r,g,b);
	}
	void setFaceColor(RGBAColorRef color)
	{
		m_face_color = color;
	}

	void setEdgeColor(int r,int g,int b)
	{
		m_edge_color = RGBAColorMake(r,g,b);
	}

	void setContextColor(int r,int g,int b)
	{
		m_context_color = RGBAColorMake(r,g,b);
	}
	//把int值转为rgb值
	void setFaceColor(int color)
	{
		m_face_color = RGBAColorRef::fromRGB(color) ;
	}

	void setEdgeColor(int color)
	{
 		m_edge_color = RGBAColorRef::fromRGB(color) ;
	}

	void setContextColor(int color)
	{
 		m_context_color = RGBAColorRef::fromRGB(color) ;
	}

public:

	int m_filter_meter;
	bool m_is_antialias;

	bool m_is_draw_edge;
	bool m_is_draw_pattern;

private:
	RGBAColorRef m_face_color;
	RGBAColorRef m_edge_color;
    RGBAColorRef m_context_color;
};
}
#endif
