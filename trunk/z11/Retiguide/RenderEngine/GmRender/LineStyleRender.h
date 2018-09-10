#ifndef _CORE_RENDERSTYLE_LINESTYLERENDER_H
#define _CORE_RENDERSTYLE_LINESTYLERENDER_H

#include "GraphicCore/MGGraphicEngine.h"

#include <vector>
using namespace z11;

namespace z11
{
	class MGCanvas;
}
namespace GIS_Embedded{

enum LINE_STYLE
{
	SOLID = 0,
	DASH = 1,
	PATH = 3 //閬撹矾锛岄渶鏋勯潰锛屽苟甯︽湁閬撹矾杈圭嚎
};

class LineStyleRender
{

public:

	LineStyleRender( );
 
	void setLineColor(int r,int g,int b)
	{
		m_line_color = RGBAColorMake(r,g,b);
	}

	void setFaceColor(int r,int g,int b)
	{
		m_face_color = RGBAColorMake(r,g,b);
	}
	void setLineColor(int color);
	const RGBAColorRef &getLineColor();

	void setFaceColor(int color)
	{
 		m_face_color = RGBAColorRef::fromRGB(color) ;
	}

    void drawPolyline(MGCanvas *canvas,const vector<MGPoint>& control_points);

	void drawRoadBorder(MGCanvas *canvas,const vector<MGPoint>& control_points);

	void drawRoadFace(MGCanvas *canvas,const vector<MGPoint>& control_points);

	void drawLine(MGCanvas *canvas,int p0_x,int p0_y,int p1_x,int p1_y);

	void drawRect(MGCanvas *canvas,int left,int top,int width,int height);
	static void drawCircle(MGCanvas *canvas,int x,int y,int r);
	static void drawLines(MGCanvas *canvas,const vector<MGPoint>& control_points);


public:
	LINE_STYLE m_line_style;    
    int m_pixel_width;
	int m_meter_width;
	int m_filter_meter;
	bool m_is_antialias;

private:

	RGBAColorRef m_line_color;
	RGBAColorRef m_face_color;

};
}
#endif
