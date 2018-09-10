#include "FeatureLayerRender.h"
#include "../Render/GmRender/GeoGmLineRender.h"
#include "../Render/GmRender/GeoGmPointRender.h"
#include "../Render/GmRender/GeoGmPolygonRender.h"
#include "Retiguide/RenderEngine/GmRender/GmStyleRender.h"
#include "Retiguide/Common/String_Tools.h"
#include "Retiguide/Common/CRS/GlobalCrt.h"
#include "System/Shared/Utility.h"
#include "System/Shared/MGTime.h"
#include "System/Shared/MGLog.h"
//#include "MapKit/Embedded/BasicType/Polygon.h"
#include <vector>

using namespace z11;

namespace GIS_Embedded
{
	int FeatureLayerRender::renderMapLayer(StyleRender& mg,FeatureLayer& f_layer,vector<Embedded::IShape*> &layer_data,
								AnnotationLayer& a_layer,ScreenPixMap& screen_map)
	{
		int sum = layer_data.size();
		if(sum == 0)
			return -1;

		GmStyleRender* gm_style_render = f_layer.m_feature_layer_style.m_current_style;

		mg.setGmStyleRender(gm_style_render);

		bool labelRenderFlag = a_layer.m_need_render;

		GeoGmRender* gm_render;

		switch(f_layer.m_layer_type)
		{
		case FeatureLayer::t_POINT:
			gm_render=&m_gm_point_render;
// 			if(!m_setting_config->map.show_point_labels)
// 				labelRenderFlag = false;
			break;
		case FeatureLayer::t_LINE:
			gm_render=&m_gm_line_render;
 			if(f_layer.m_is_path_layer)
			{
				m_gm_line_render.setRenderType(PATH_FACE);/*PATH_FACE*/
				int pix_width = m_transformer->transformDistanceToPix(mg.getLineMeterWidth());
				mg.setLinePixelWidth(pix_width);
			}
 			else
 			{
				m_gm_line_render.setRenderType(SOLID_LINE);
				int pix_width = m_transformer->transformDistanceToPix(mg.getLineMeterWidth());
				mg.setLinePixelWidth(pix_width);
 			}

// 			if(!m_setting_config->map.show_street_labels)
// 				labelRenderFlag = false;
			break;
		case FeatureLayer::t_POLYGON:
 			gm_render=&m_gm_polygon_render;
			break; 
		case FeatureLayer::t_IMAGE:
			gm_render=&m_gm_polygon_render;
			break;
		default:
			return -1;
		}
		if(labelRenderFlag)
		{
			for (int i=0; i < sum; i++)
			{
				IShape *geo=layer_data[i];
				gm_render->setGeo(geo);

 				z11::MGWString wstr = geo->getName();
				std::string label = Utility::wstr2ustr(geo->getName());

				/*static z11::MGWString ee(L"梅山江 "); 
				if( ee.compare(0,3,wstr) == 0  )
				{
					Embedded::Polygon* po = dynamic_cast<Embedded::Polygon*>(geo);
					int num = po->posNum();
					if(num == 175)*/
 				if(gm_render->render(mg)>=0 && !label.empty())
				{
					//return 0; //GRF 2012-6-8
					a_layer.checkGeoLabelPos(gm_render->m_gm_buff,gm_render->m_buff_valid,label,screen_map,TXT_ARABIC);//m_setting_config->language_units.program_language
					/*}*/
				}
 					
			}
		}
		else
		{
			for (int i=0; i < sum; i++)
			{
				IShape *geo = layer_data[i];
				gm_render->setGeo(geo);								
				gm_render->render(mg);//首先，给gm_render->m_gm_buff,gm_render->m_buff_valid赋值，绘制标注时要用到
			}
		}

		return 0;

	}

 	int FeatureLayerRender::renderPathLayerFace(StyleRender& mg,FeatureLayer &feature_layer,vector<Embedded::IShape*> &layer_data)
 	{
 		int sum = layer_data.size();
 		if(sum == 0)
 			return -1;
 
 		GmStyleRender* gm_style_render = feature_layer.m_feature_layer_style.m_current_style;
 		if(gm_style_render == NULL)
 			return -1;
 
 		mg.setGmStyleRender(gm_style_render);
 
 		if(!feature_layer.m_is_path_layer)
 			return -1;
 
 		m_gm_line_render.setRenderType(PATH_FACE);
 
 		for (int i=0; i < sum; i++)
 		{
 			IShape *geo=layer_data[i];
 			m_gm_line_render.setGeo(geo);
 			m_gm_line_render.render(mg);
 		}
 
 		return 0;
 	}
// 
// 	void FeatureLayerRender::checkLayerGeosToRender(vector<vector<GeographyObject>*>& layer_data, vector<GeographyObject *> &geos_to_render)
// 	{
// 		geos_to_render.clear();
// 		double side_length = m_transformer->transformPixDisToDistance(10);
// 
// 		for(int gridi=0,gridsum=layer_data.size(); gridi<gridsum; gridi++) //grids
// 			for (int geoi=0,geo_sum=layer_data[gridi]->size(); geoi < geo_sum; geoi++)
// 			{
// 				GeographyObject &geo=layer_data[gridi]->at(geoi);
// 
// 				if (! geo._geometry->interactWithScopeN(*m_scope_adjust))
// 					continue;
// 
// 				if(geo._geometry->getGeometryType() == Geometry::GM_POLYGON)
// 				{
// 					if(!geo._geometry->needToRenderBySideLength(side_length))
// 						continue;
// 				}
// 				geos_to_render.push_back(&geo);
// 			}
// 	}
// 
 	int FeatureLayerRender::renderPathLine(GIS_Embedded::StyleRender &mg, vector<pair<Embedded::IShape*,unsigned long> > &line_data)
 	{
//  		int sum = line_data.size();
//  		if(sum == 0)
//  			return -1;
//  		LineStyleRender tmp_line_render;
//  		tmp_line_render.m_line_style = SOLID;
//  		tmp_line_render.m_pixel_width = 8;
//  		tmp_line_render.m_filter_meter = 5;
//  		tmp_line_render.m_meter_width = 8;
//  
//  		GmStyleRender* gm_style_render = new GmStyleRender();
//  		gm_style_render->m_line_render = &tmp_line_render;
//  
//  		mg.setGmStyleRender(gm_style_render);
//  
//  		GeoGmRender* gm_render;
//  		gm_render=&m_gm_line_render;
//  		m_gm_line_render.setRenderType(SOLID_LINE);
//  		int pix_width = m_transformer->transformDistanceToPix(mg.getLineMeterWidth());
//  		mg.setLinePixelWidth(pix_width*1.5);
//  
//  		for (int i=0; i < sum; i++)
//  		{
//  			IShape *geo=line_data[i].first;
//  			gm_render->setGeo(*geo);
//  			tmp_line_render.setLineColor(line_data[i].second);//0xffff00
//  
//  			gm_render->render(mg);//首先，给gm_render->m_gm_buff,gm_render->m_buff_valid赋值，绘制标注时要用到
//  		}
 		return 0;
 	}
// 
// 	int FeatureLayerRender::getPathClor(GIS_Embedded::GeographyObject *geo)
// 	{
// 		int ret = -1;
// 		std::map<GeographyObject*,int>::iterator it = m_road_state_data.find(geo);
// 		if (it != m_road_state_data.end())
// 			ret = (*it).second;
// 		return ret;
// 	}


}