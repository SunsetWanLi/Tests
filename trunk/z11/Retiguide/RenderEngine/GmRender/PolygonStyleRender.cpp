#include "PolygonStyleRender.h"

#include "GraphicCore/MGCanvas.h"
#include "System/Shared/MGLog.h"
#include "System/Shared/MGTime.h"
extern MGCanvas *global_test_canvas;

namespace GIS_Embedded
{
 	void PolygonStyleRender::drawPolygonLine(MGCanvas *canvas,const vector<MGPoint>& control_points)
 	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}

		//graphic_engine->drawPolyLoopLines(control_points,m_edge_color);
		canvas->PolygonAA(control_points,m_edge_color,1);
 	}
 
 	void PolygonStyleRender::fillRectWithFillColor(MGCanvas *canvas,int left,int top,int width,int height)
 	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}

		MGRect tmp_rect;
		tmp_rect.x = left,tmp_rect.y = top,tmp_rect.width = width,tmp_rect.height = height;
		//graphic_engine->drawFilledRect(tmp_rect,m_face_color);
		canvas->RectFill(tmp_rect,m_face_color);
 	}
 
 	void PolygonStyleRender::fillPolygonWithFillColor(MGCanvas *canvas,const vector<MGPoint>& points)
 	{	
		//MGTimeTicks ticks=MGTimeTicks::Now();

		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}

		//graphic_engine->drawPolyLoopLines(points,m_face_color);

		//graphic_engine->drawFilledPolyLoopLines(points,m_face_color);

		//LogStart();
		canvas->FastPolygonFillAA(points,m_face_color);
		//LogEnd("FastPolygonFillAA");
		//LogStart();
		//canvas->PolygonFillAA(points,m_face_color);
		//LogEnd("PolygonFillAA");
		//MGLogD("PolygonStyleRender::fillPolygonWithFillColor %f",(MGTimeTicks::Now()-ticks).InMillisecondsF());
 	}
 
 	void PolygonStyleRender::fillPolygonWithContextColor(MGCanvas *canvas,const vector<MGPoint>& points)
 	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}

		//graphic_engine->drawFilledPolyLoopLines(points,m_context_color); 
		canvas->FastPolygonFillAA(points,m_context_color);
 	}
 
 	void PolygonStyleRender::fillRect(MGCanvas *canvas,int left,int top,int width,int height,int r,int g,int b)
 	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}

		MGRect tmp_rect;
		tmp_rect.x = left,tmp_rect.y = top,tmp_rect.width = width,tmp_rect.height = height;
		//graphic_engine->drawFilledRect(tmp_rect,RGBAColorMake(r,g,b));
		canvas->RectFill(tmp_rect,RGBAColorMake(r,g,b));
 	}

	void PolygonStyleRender::fillRect(MGCanvas *canvas,int left,int top,int width,int height,RGBAColorRef color)
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}

		MGRect tmp_rect;
		tmp_rect.x = left,tmp_rect.y = top,tmp_rect.width = width,tmp_rect.height = height;
		//graphic_engine->drawFilledRect(tmp_rect,color);
		canvas->RectFill(tmp_rect,color);
	}
}