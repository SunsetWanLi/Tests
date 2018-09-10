#include "StyleRender.h"
#include "GraphicCore/MGImage.h"
#include "System/Shared/MGLog.h"
#include "System/IO/MGBundle.h"


using namespace z11;

namespace GIS_Embedded
{
	void StyleRender::drawPoint(int x, int y)
	{
		m_gm_render->m_point_render->drawPoint(x, y);
	}

	void StyleRender::drawPolyline(z11::MGPoint* points, int sum)
	{
		if(sum <=0)
			return;

		vector<MGPoint> mg_points;
		transformPointsToMgPoints(points,sum,mg_points);

		m_gm_render->m_line_render->drawPolyline(m_canvas,mg_points);
	}

	void StyleRender::drawPolygonLine(z11::MGPoint* points,int sum)
	{
		if (sum<0)
			return;

		vector<MGPoint> mg_points;
		transformPointsToMgPoints(points,sum,mg_points);

		m_gm_render->m_polygon_render->drawPolygonLine(m_canvas,mg_points);
	}

	void StyleRender::drawRoadBorder(z11::MGPoint* points,int sum)
	{
		if(sum <=0)
			return;

		vector<MGPoint> mg_points;
		transformPointsToMgPoints(points,sum,mg_points);

		m_gm_render->m_line_render->drawRoadBorder(m_canvas,mg_points);
		
	}

	void StyleRender::drawRoadFace(z11::MGPoint* points,int sum)
	{
		if(sum <=0)
			return;
		vector<MGPoint> mg_points;
		transformPointsToMgPoints(points,sum,mg_points);

		//////////////////////////////////////////////////////////////////////////

		//MGLogD("RoadFace %d",sum);
		//for (uint_32 i=0;i<sum;i++)
		//{
		//	MGLogD("%.1f %.1f",points[i].x,points[i].y);
		//}

		//FILE *file=fopen(MGBundle::mainBundle()->tmpPathWithFileName("points.txt").c_str(),"a");
		//fprintf(file,"RoadFace %d\n",sum);
		//for (uint_32 i=0;i<sum;i++)
		//{
		//	fprintf(file,"%.1f %.1f\n",points[i].x,points[i].y);
		//}
		//fclose(file);
		//////////////////////////////////////////////////////////////////////////

		m_gm_render->m_line_render->drawRoadFace(m_canvas,mg_points);
	}

	void StyleRender::fillPolygonWithFillColor(z11::MGPoint* points, int sum)
	{
		if(sum <=0)
			return;

		vector<MGPoint> mg_points;
		transformPointsToMgPoints(points,sum,mg_points);

		m_gm_render->m_polygon_render->fillPolygonWithFillColor(m_canvas,mg_points);
	}

	void StyleRender::fillPolygonWithContextColor(z11::MGPoint* points, int sum)
	{
		if(sum <=0)
			return;

		vector<MGPoint> mg_points;
		transformPointsToMgPoints(points,sum,mg_points);

		m_gm_render->m_polygon_render->fillPolygonWithContextColor(m_canvas,mg_points);
	}

	void StyleRender::drawImage(MGImage* image,int left,int top,int right,int bottom)
	{
 		ImageStyleRender::drawStretchLeftTop(image,left,top,right-left,bottom-top);
	}

	void StyleRender::drawString(int left, int top, int escape,const std::string& label)
	{
		m_label_render->drawString(label, left, top, escape);
	}
	void StyleRender::drawStringCenterXY(int center_x, int center_y, int escape,const std::string& label)//lw
	{
		m_label_render->drawStringCenterXY(label,center_x,center_y);
	}
	
	void StyleRender::drawCharacter(int left, int top, int escape, const std::string& label)
	{
		m_label_render->drawCharacter(label, left, top, escape);
	}

  	void StyleRender::getStringBound(const std::string& text,UserScreenRect *rect)
  	{
  		m_label_render->getStringBound(text,rect);
  	}

	void StyleRender::getStringExtent(const std::string& text, int *width, int *height )
	{
		m_label_render->getStringExtent(text, width, height);
	}

	void StyleRender::setCanvas( MGCanvas * canvas )
	{
		m_canvas = canvas;
	}

	StyleRender::StyleRender()
	{
		m_gm_render = NULL;
		m_label_render = NULL;
		m_canvas = NULL;
	}
}