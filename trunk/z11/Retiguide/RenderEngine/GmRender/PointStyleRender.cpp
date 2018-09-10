#include "PointStyleRender.h"
#include "GraphicCore/MGCanvas.h"
extern MGCanvas *global_test_canvas;

namespace GIS_Embedded
{
	void PointStyleRender::drawPoint(const MGPoint& point,RGBAColorRef color, float size , RGBAColorRef blend )
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}
		graphic_engine->drawPoint(point,color,size,blend);


	}

	void PointStyleRender::drawPoints(const vector<MGPoint>& points,RGBAColorRef color , float size, RGBAColorRef blend)
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}
		graphic_engine->drawPoints(points,color,size,blend);
	}

	void PointStyleRender::drawPoint(int x,int y)
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		if (graphic_engine == NULL) {
			return;
		}
#define POINT_IMAGE_SIZE 48
		MGPoint point;
		point.x = x, point.y = y;
		graphic_engine->drawPoint(point);
		if (m_bit_map!=NULL)
		{
			m_bit_map->sendDataToContext();
			int half_width=m_bit_map->getWidth()/2;
			int half_height=m_bit_map->getHeight()/2;
			graphic_engine->drawImage(m_bit_map,MGRectMake(x-POINT_IMAGE_SIZE/2,y-POINT_IMAGE_SIZE/2,POINT_IMAGE_SIZE,POINT_IMAGE_SIZE));
		}
	}

	PointStyleRender::PointStyleRender()
	{
		m_bit_map = NULL;
	}

	PointStyleRender::~PointStyleRender()
	{
		delete_and_set_null(m_bit_map);
	}
}
