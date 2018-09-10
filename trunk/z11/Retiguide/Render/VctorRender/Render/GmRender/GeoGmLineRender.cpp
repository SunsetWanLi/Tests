#include "GeoGmLineRender.h"
#include "MapKit/Embedded/BasicType/Line.h"
#include "Retiguide/Analysis/TransformRoad.h"
#include "System/Shared/MGLog.h"
#include <vector>
//#include "include/MGFoundation.h"
using namespace Embedded;
using namespace z11;

namespace GIS_Embedded
{
	int GeoGmLineRender::render(StyleRender& mg)
	{
 		if(m_render_type == SOLID_LINE)
			return renderSolidLine(mg);
 		else if(m_render_type == PATH_BORDER)
 			return renderPathBorder(mg);
 		else if(m_render_type == PATH_FACE)
 			return renderPathFace(mg);

		return -1;
	}

	int GeoGmLineRender::renderSolidLine(StyleRender& mg)
	{
		Embedded::Line* line = dynamic_cast< Embedded::Line*> (m_geo);
		std::vector<Embedded::MapPos2D > points;
		if(line)
		{
			points.clear();
			line->getPoints(&points);
		}
		vector<Pos2DEx> control_pos2d(points.size());
		int filter_width = mg.getLineFilterPara();

		for (uint_32 k=0; k<points.size(); k++)
		{
			control_pos2d[k].d0 = points[k].d0;
			control_pos2d[k].d1 = points[k].d1;
		}

 		static vector<Pos2DEx> control_pos2d_filtered(256);
 		control_pos2d_filtered.clear();
 
 		if (filter_width <= 0)//防止下面调用函数出错  lw
 			filter_width = 1;
 
 		TransformRoad::filtrationPointDouglas(control_pos2d, filter_width, control_pos2d_filtered);

		int pos2d_sum = control_pos2d_filtered.size();
		resizeGmBuff(pos2d_sum+COORDINATE_CLIP_BUFFER);

		m_buff_valid = m_transformer->transformPos2DsToArray(control_pos2d_filtered,m_gm_buff);

		mg.drawPolyline(m_gm_buff,m_buff_valid);
		return 0;
	}

	int GeoGmLineRender::renderPathBorder(StyleRender& mg)
	{
		int road_width = mg.getLineMeterWidth();
		int filter_width = mg.getLineFilterPara();

		static vector<Pos2DEx> border_points(512);
		static vector<Pos2DEx> center_points(256);

		Embedded::Line* line = dynamic_cast< Embedded::Line*> (m_geo);
		std::vector<Embedded::MapPos2D > points;
		if(line)
		{
			points.clear();
			line->getPoints(&points);
		}
		vector<Pos2DEx> control_pos2d(points.size());//points.size()
		//MGLogD("GeoGmLineRender::renderPathBorder ");
		for (uint_32 k=0; k<points.size(); k++)
		{
			control_pos2d[k].d0 = points[k].d0;
			control_pos2d[k].d1 = points[k].d1;
			//MGLogD("point [%d] =(%d,%d) ",k,control_pos2d[k].d0,control_pos2d[k].d1);
		}
//
		TransformRoad::calculatePathGeoBorder(control_pos2d,*m_scope_adjust,road_width,filter_width,border_points,center_points);

		int border_pos2d_sum = border_points.size();
		if(border_pos2d_sum <= 1)
			return -1;

		resizeGmBuff(border_pos2d_sum+COORDINATE_CLIP_BUFFER);
		m_buff_valid = m_transformer->transformPos2DsToArray(border_points,m_gm_buff);

  		//mg.drawRoadBorder(m_gm_buff,m_buff_valid);
		//int center_pos2d_sum = center_points.size();
		//if(center_pos2d_sum <= 1)
		//	return -1;

		//resizeGmBuff(center_pos2d_sum+COORDINATE_CLIP_BUFFER);
		//m_buff_valid = m_transformer->transformPos2DsToArray(center_points,m_gm_buff);

		//MGLogD("draw Road Face times \n");
		mg.drawRoadFace(m_gm_buff, m_buff_valid);

		return 0;
	}

	int GeoGmLineRender::renderPathFace(StyleRender& mg)
	{
		Embedded::Line* line = dynamic_cast< Embedded::Line*> (m_geo);
		std::vector<Embedded::MapPos2D > points;
		if(line)
		{
			points.clear();
			line->getPoints(&points);
		}
		vector<Pos2DEx> control_pos2d(points.size());//points.size()

		for (uint_32 k=0; k<points.size(); k++)
		{
			control_pos2d[k].d0 = points[k].d0;
			control_pos2d[k].d1 = points[k].d1;
		}

		int pos2d_sum = control_pos2d.size();
 		if(pos2d_sum <=0)
 			return -1;

		resizeGmBuff(pos2d_sum+COORDINATE_CLIP_BUFFER);

		m_buff_valid = m_transformer->transformPos2DsToArray(control_pos2d,m_gm_buff);

 		mg.drawRoadFace(m_gm_buff, m_buff_valid);
//delete 存储点  //
		return 0;
	}

}