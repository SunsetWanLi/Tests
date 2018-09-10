#include "LineStyleRender.h"
#include <math.h>
#include "GraphicCore/MGCanvas.h"
#include "System/Shared/MGLog.h"
#include "System/IO/MGBundle.h"
#include "System/Shared/MGTime.h"
MGCanvas *global_test_canvas;
namespace GIS_Embedded
{
	void LineStyleRender::drawRoadBorder(MGCanvas *canvas,const vector<MGPoint>& control_points)
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}

		//m_line_color = RGBAColorMake(255,255,0);
		//test
		//graphic_engine->drawPolyLines(control_points,m_line_color,2);
		canvas->PolygonAA(control_points,m_line_color,2);

	}

	void LineStyleRender::drawRoadFace(MGCanvas *canvas,const vector<MGPoint>& control_points)
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}

		////////////////////////////////////////////////////////////////////////////

		//MGLogD("RoadFace %d",control_points.size());
		//for (uint_32 i=0;i<control_points.size();i++)
		//{
		//	MGLogD("%.1f %.1f",control_points[i].x,control_points[i].y);
		//}

		//FILE *file=fopen(MGBundle::mainBundle()->tmpPathWithFileName("points.txt").c_str(),"a");
		//fprintf(file,"RoadFace %d\n",control_points.size());
		//for (uint_32 i=0;i<control_points.size();i++)
		//{
		//	fprintf(file,"%.1f %.1f\n",control_points[i].x,control_points[i].y);
		//}
		//fclose(file);
		////////////////////////////////////////////////////////////////////////////

		//MGTimeTicks ticks=MGTimeTicks::Now();
		int pixel_width=m_pixel_width<<1;
		if (pixel_width<=3)
		{
			pixel_width=3;
		}
		canvas->PolylineAA(control_points,m_line_color,pixel_width*1.2);
		canvas->PolylineAA(control_points,m_face_color,pixel_width);
		//MGLogD("LineStyleRender::drawRoadFacer %f",(MGTimeTicks::Now()-ticks).InMillisecondsF());
	}

	void LineStyleRender::drawPolyline(MGCanvas *canvas, const vector<MGPoint>& control_points)
	{
		if(m_pixel_width <= 0)
			return;

		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}

		canvas->PolylineAA(control_points,m_line_color,m_pixel_width);

	}
//  	void LineStyleRender::drawPolygonLine(const MGPoint* control_points,int sum,int clor)
//  	{
//  		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
//  
//  		if (graphic_engine == NULL) {
//  			return;
//  		}
//  
//  		graphic_engine->drawPolyLines(control_points,m_line_color);
//  	}

	void LineStyleRender::drawRect(MGCanvas *canvas,int left,int top,int width,int height)
	{
		if(m_pixel_width <= 0)
			return;

		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}

		MGRect tmp_rect;
		tmp_rect.x = left,tmp_rect.y = top,tmp_rect.width = width,tmp_rect.height = height;
		//graphic_engine->drawRect(tmp_rect);
		canvas->Rect(tmp_rect);
	}

	void LineStyleRender::drawLine(MGCanvas *canvas,int p0_x,int p0_y,int p1_x,int p1_y)
	{
		if(m_pixel_width <= 0)
			return;
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}

		MGPoint start_pos,end_pos;
		start_pos.x = p0_x,start_pos.y = p0_y;
		end_pos.x = p1_x,end_pos.y = p1_y;

		////test
		//graphic_engine->drawLine(start_pos,end_pos,m_line_color,m_pixel_width);
		canvas->LineAA(start_pos,end_pos,m_line_color,m_pixel_width);

	}

	void LineStyleRender::drawCircle(MGCanvas *canvas,int x,int y,int r)
	{
// 		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
// 
// 		if (graphic_engine == NULL) {
// 			return;
// 		}

	//	::GXCircle(gdc,x,y,r);
	}
	void LineStyleRender::drawLines(MGCanvas *canvas, const vector<MGPoint>& control_points)
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}

		//test
		//graphic_engine->drawPolyLines(control_points,RGBAColorRef::fromRGB(7846911),2);//RGBAColorMake(255,255,0)
		////graphic_engine->drawLine(control_points[0],control_points[1],RGBAColorMake(255,255,0),2);//
		canvas->PolygonAA(control_points,RGBAColorRef::fromRGB(7846911),2);

	}

	LineStyleRender::LineStyleRender()
	{

		m_line_style = SOLID;   
		m_is_antialias = false;
		m_line_color = RGBAColorMake(0,0,0);
		m_face_color = RGBAColorMake(0,0,0);
		m_pixel_width = -1;
		m_meter_width = 1;
		m_filter_meter = 100;
	}

	const RGBAColorRef & LineStyleRender::getLineColor()
	{
		return m_line_color;
	}

	void LineStyleRender::setLineColor( int color )
	{
		m_line_color = RGBAColorRef::fromRGB(color) ;
	}
}