#ifndef _CORE_RENDERENGINE_FEATURELAYERRENDER_H
#define _CORE_RENDERENGINE_FEATURELAYERRENDER_H

#include "Retiguide/Common/Util/Transformer.h"
#include "Retiguide/Common/Share/Scope.h"
#include "../Render/GmRender/GeoGmRender.h"
#include "../Render/GmRender/GeoGmPointRender.h"
#include "../Render/GmRender/GeoGmLineRender.h"
#include "../Render/GmRender/GeoGmPolygonRender.h"
#include "../Render/GmRender/GeoGmImageRender.h"
#include "FeatureLayer.h"
#include "../Annotation/AnnotationLayer.h"
#include "../Map/ScreenPixMap.h"
#include "../Render/StyleRender.h"
#include "Retiguide/Common/Types/Settings.h"

namespace GIS_Embedded{
class FeatureLayerRender {

public:	

	FeatureLayerRender(/*Settings* &setConf*/)//:m_setting_config(setConf)
	{}
	
	void setTransformer(const Transformer* trans)
	{
		m_transformer = trans;

		m_gm_line_render.setTansformer(trans);
		m_gm_point_render.setTansformer(trans);
		m_gm_polygon_render.setTansformer(trans);
		m_gm_image_render.setTansformer(trans);
	}

	void setViewScopeAdjust(const Scope* scope)
	{
		m_scope_adjust = scope;

		m_gm_line_render.setScopeAdjust(scope);
		m_gm_point_render.setScopeAdjust(scope);
		m_gm_polygon_render.setScopeAdjust(scope);
		m_gm_image_render.setScopeAdjust(scope);
	}
	//for raster map
	void setIsRasterMap(bool flag)
	{
		m_is_raster_map = flag;
	}
	bool getIsRasterMap()
	{
		return m_is_raster_map;
	}

	void checkLayerGeosToRender(vector<vector<Embedded::IShape>*>& layer_data, vector<Embedded::IShape *> &geos_to_render);

	// for new data 
	int renderMapLayer(StyleRender& mg,FeatureLayer& f_layer,vector<Embedded::IShape*> &layer_data,
						AnnotationLayer& a_layer,ScreenPixMap& screen_map);
// 	//for road state
 	int renderPathLine(StyleRender&mg,vector<pair<Embedded::IShape*,unsigned long> > &line_data);
 	int renderPathLayerFace(StyleRender& mg,FeatureLayer &feature_layer,vector<Embedded::IShape*> &layer_data);


private:
	Settings* m_setting_config;

    const Transformer* m_transformer;
	const Scope* m_scope_adjust;
   
    GeoGmLineRender m_gm_line_render;
    GeoGmPointRender m_gm_point_render;
    GeoGmPolygonRender m_gm_polygon_render;
	GeoGmImageRender m_gm_image_render;
	//for raster map
	bool m_is_raster_map;

// 	hash_map<GeographyObject*,int> m_road_state_data;
  
};
}
#endif